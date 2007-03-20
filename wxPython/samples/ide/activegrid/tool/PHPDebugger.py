#---------------------------------------------------------------------------
# Name:         PHPDebugger.py
# Purpose:      php dbg client and supporting code
# Author:       Matt Fryer, Kevin Wang
# Created:      2/1/06
# Copyright:    (c) 2006 ActiveGrid, Inc.
# License:      wxWindows License
#---------------------------------------------------------------------------


import os
import socket
import sys
import threading
import traceback
import wx
import DebuggerService
import activegrid.util.sysutils as sysutils


DBGC_REPLY                              = 0x0               # reply to previous DBGA_REQUEST request
DBGC_STARTUP                            = 0x0001            # script startup
DBGC_END                                = 0x0002            # script done
DBGC_BREAKPOINT                         = 0x0003            # user definded breakpoint occured
DBGC_STEPINTO_DONE                      = 0x0004            # step to the next statement is completed
DBGC_STEPOVER_DONE                      = 0x0005            # step to the next statement is completed
DBGC_STEPOUT_DONE                       = 0x0006            # step to the next statement is completed
DBGC_EMBEDDED_BREAK                     = 0x0007            # breakpoint caused by DebugBreak() function
DBGC_ERROR                              = 0x0010            # error occured
DBGC_LOG                                = 0x0011            # logging support
DBGC_SID                                = 0x0012            # send SID
DBGC_PAUSE                              = 0x0013            # pause current session as soon as possible
DBGC_AG_SHUTDOWN_REQ                    = 0x0201            # special ActiveGrid UI shutdown listening thread command


FRAME_STACK                             = 100000            # "call:stack" - e.g. backtrace
FRAME_SOURCE                            = 100100            # source text
FRAME_SRC_TREE                          = 100200            # tree of source files
FRAME_RAWDATA                           = 100300            # raw data or string
FRAME_ERROR                             = 100400            # error notification
FRAME_EVAL                              = 100500            # evaluating/watching
FRAME_BPS                               = 100600            # set/remove breakpoint
FRAME_BPL                               = 100700            # breakpoint(s) request = get the list
FRAME_VER                               = 100800            # version request
FRAME_SID                               = 100900            # session id info
FRAME_SRCLINESINFO                      = 101000            # source lines info
FRAME_SRCCTXINFO                        = 101100            # source contexts info
FRAME_LOG                               = 101200            # logging
FRAME_PROF                              = 101300            # profiler
FRAME_PROF_C                            = 101400            # profiler counter/accuracy
FRAME_SET_OPT                           = 101500            # set/update options


DBGF_STARTED                            = 0x0001            # debugger has been started
DBGF_FINISHED                           = 0x0002            # DBGC_END notification has been sent
DBGF_WAITACK                            = 0x0004            # awaiting replay|request
DBGF_UNSYNC                             = 0x0008            # protocol has been unsynchronized
DBGF_REQUESTPENDING                     = 0x0010            # Debug session request pending
DBGF_REQUESTFOUND                       = 0x0020            # Debug session request found
DBGF_REJECTIONFOUND                     = 0x0040            # DBGSESSID=-1 found - session rejection


E_ERROR                                 = 1 << 0
E_WARNING                               = 1 << 1
E_PARSE                                 = 1 << 2
E_NOTICE                                = 1 << 3
E_CORE_ERROR                            = 1 << 4
E_CORE_WARNING                          = 1 << 5
E_COMPILE_ERROR                         = 1 << 6
E_COMPILE_WARNING                       = 1 << 7
E_USER_ERROR                            = 1 << 8
E_USER_WARNING                          = 1 << 9
E_USER_NOTICE                           = 1 << 10


BPS_DELETED                             = 0
BPS_DISABLED                            = 1
BPS_ENABLED                             = 2
BPS_UNRESOLVED                          = 0x100


DBG_SYNC                                = 0x5953
DBG_SYNC2_STR                           = chr(0) + chr(0) + chr(89) + chr(83)
RESPONSE_HEADER_SIZE                    = 16


_VERBOSE = False
def myprint(format, vlist=None):
    if _VERBOSE:
        if vlist:
            print format % vlist
        else:
            print format


#
# 4 Char's to an Integer
#
def C4ToInt(ch, startPos):
    retval = 0
    pos    = startPos

    retval = retval + (CharToInt(ch[pos]) << 24)
    pos    = pos + 1
    retval = retval + (CharToInt(ch[pos]) << 16)
    pos    = pos + 1
    retval = retval + (CharToInt(ch[pos]) << 8)
    pos    = pos + 1
    retval = retval + (CharToInt(ch[pos]) << 0)

    return retval


def CharToInt(ch):
    return int((ord(ch) & 0x00FF));


#
# An Integer to 4 Char's
#
def IntToC4(num):
    retval  = chr((num >> 24) & 0x00FF)
    retval += chr((num >> 16) & 0x00FF)
    retval += chr((num >> 8 ) & 0x00FF)
    retval += chr((num >> 0 ) & 0x00FF)

    return retval


DBGA_CONTINUE                           = IntToC4(0x8001)
DBGA_STOP                               = IntToC4(0x8002)
DBGA_STEPINTO                           = IntToC4(0x8003)
DBGA_STEPOVER                           = IntToC4(0x8004)
DBGA_STEPOUT                            = IntToC4(0x8005)
DBGA_IGNORE                             = IntToC4(0x8006)
DBGA_REQUEST                            = IntToC4(0x8010)


def getCommandString(code):
    if code == DBGC_REPLY:
        return "REPLY"
    elif code == DBGC_STARTUP:
        return "STARTUP"
    elif code == DBGC_END:
        return "END"
    elif code == DBGC_BREAKPOINT:
        return "BREAKPOINT"
    elif code == DBGC_STEPINTO_DONE:
        return "STEPINTO DONE"
    elif code == DBGC_STEPOVER_DONE:
        return "STEPOVER DONE"
    elif code == DBGC_STEPOUT_DONE:
        return "STEPOUT DONE"
    elif code == DBGC_EMBEDDED_BREAK:
        return "EMBEDDED BREAK"
    elif code == DBGC_PAUSE:
        return "PAUSE"
    elif code == DBGC_ERROR:
        return "ERROR"
    elif code == DBGC_LOG:
        return "LOG"
    elif code == DBGC_SID:
        return "SEND SID"
    elif code == DBGC_AG_SHUTDOWN_REQ:
        return "AG SHUTDOWN REQ"


def reportFlags(flagsValue):
    flagsRetVal = ""
    if flagsValue & DBGF_STARTED:                  # debugger has been started
        flagsRetVal += "started+"
    if flagsValue & DBGF_FINISHED:                 # DBGC_END notification has been sent
        flagsRetVal += "finished+"
    if flagsValue & DBGF_WAITACK:                  # awaiting replay|request
        flagsRetVal += "awaiting ack+"
    if flagsValue & DBGF_UNSYNC:                   # protocol has been unsynchronized
        flagsRetVal += "protocol unsynchronized+"
    if flagsValue & DBGF_REQUESTPENDING:           # Debug session request pending
        flagsRetVal += "request pending+"
    if flagsValue & DBGF_REQUESTFOUND:             # Debug session request found
        flagsRetVal += "request found+"
    if flagsValue & DBGF_REJECTIONFOUND :          # DBGSESSID=-1 found - session rejection
        flagsRetVal += "session rejection+"
    return flagsRetVal


def getErrorTypeString(code):
    if code == E_ERROR:
        return "[Error]"
    elif code == E_WARNING:
        return "[Warning]"
    elif code == E_PARSE:
         return "[Parse Error]"
    elif code == E_NOTICE:
        return "[Notice]"
    elif code == E_CORE_ERROR:
        return "[Core Error]"
    elif code == E_CORE_WARNING:
        return "[Core Warning]"
    elif code == E_COMPILE_ERROR:
        return  "[Compile Error]"
    elif code == E_COMPILE_WARNING:
        return  "[Compile Warning]"
    elif code == E_USER_ERROR:
        return  "[User Error]"
    elif code == E_USER_WARNING:
        return  "[User Warning]"
    elif code == E_USER_NOTICE:
        return  "[User Notice]"
    else:
        return "[Unexpected Error]"


class ResponseHeader(object):
    def __init__(self, conn, blocking = False):
        self.isValid = False
        receivedData = conn.recv(RESPONSE_HEADER_SIZE, blocking)

        if not receivedData:
            myprint("Tried to get %d bytes of PHP DBG header, got None\n" % RESPONSE_HEADER_SIZE)
            return
        elif len(receivedData) != RESPONSE_HEADER_SIZE:
            myprint("Tried to get %d bytes of PHP DBG header, got %d\n" % (RESPONSE_HEADER_SIZE, len(receivedData)))
            return

        self.sync    = C4ToInt(receivedData, 0)
        self.command = C4ToInt(receivedData, 4)
        self.flags   = C4ToInt(receivedData, 8)
        self.toRead  = C4ToInt(receivedData, 12)

        myprint("ResponseHeader: sync=%x, command=%s, flags=(%s), toRead=%s\n", (self.sync, getCommandString(self.command), reportFlags(self.flags), self.toRead))
        if self.sync != DBG_SYNC:
            myprint("Sync wrong for header! Expected %x, got %s\n" % (DBG_SYNC, self.sync))
            return

        self.isValid = True


class ResponsePacketFrame(object):
    def __init__(self, conn, size, data, blocking = False):
        self.isValid      = False
        self.conn         = conn
        self.data         = ''

        if data:
            self.data     = data
            newlyReceived = False
        elif conn:
            newlyReceived = True

            sizeToReceive = size
            while True:
                oneChunk  = conn.recv(sizeToReceive, blocking)
                sizeReceived = len(oneChunk)
                if sizeReceived > 0:
                    self.data = self.data + oneChunk

                if sizeReceived < sizeToReceive:
                    sizeToReceive = sizeToReceive - sizeReceived
                    continue
                else:
                    break

            if len(self.data) != size:
                myprint("Expected to get %d bytes of a PHP DBG packet, got %d\n" % (size, len(self.data)))
                return
        else:
            return

        self.frameName = C4ToInt(self.data, 0)
        self.frameSize = C4ToInt(self.data, 4)
        if newlyReceived:
            myprint("Newly received ResponsePacketFrame: frameName=%d, frameSize=%d", (self.frameName, self.frameSize))
        else:
            myprint("Created from existing ResponsePacketFrame: frameName=%d, frameSize=%d", (self.frameName, self.frameSize))

        if self.frameSize == 0:
            return

        self.currPos      = 8
        self.totalDataLen = len(self.data)
        self.length       = 8 + self.frameSize
        self.isValid      = True
        myprint("new ResponsePacketFrame: currPos=%s, totalDataLen=%s, length=%s", (repr(self.currPos), repr(self.totalDataLen), repr(self.length)))
        return

    def getNextInt(self):
        myprint("getNextInt(): currPos=%s, totalDataLen=%s, length=%s", (repr(self.currPos), repr(self.totalDataLen), repr(self.length)))
        if self.isValid and self.currPos + 4 <= self.length:
            val = C4ToInt(self.data, self.currPos)
            self.currPos = self.currPos + 4
            myprint("getNextInt(): got an integar: %s", repr(val))
            return val
        else:
            return self._errorReturn("getNextInt(): no more integar available with current frame: ")

    def getNextString(self, strLen):
        endPos = self.currPos + strLen
        if self.isValid and endPos <= self.length:
            #
            # Trim the ending '\0'.  TODO: confirm this applies to all raw string data.
            #
            str          = self.data[self.currPos:endPos - 1]
            self.currPos = endPos
            myprint("getNextString(): got a string: %s", str)
            return str
        else:
            return self._errorReturn("getNextString(): no more string available with current frame: ")

    def getNextFrame(self, useAbsolutePos = False):
        if useAbsolutePos:
            #
            # Skip this frame's header (8 bytes for frameSize and frameSize) and frame data (frameSize).
            #
            self.currPos = self.length

        if self.isValid and self.currPos < self.totalDataLen:
            return ResponsePacketFrame(None, None, self.data[self.currPos:])
        else:
            return self._errorReturn("getNextFrame(): no more frame available with current frame: ")

    def _errorReturn(self, preMsg = ''):
        myprint(preMsg + "frameName=%s, frameSize=%s, totalDataLen=%s, length=%s, currPos:%s", (repr(self.frameName), repr(self.frameSize), repr(self.totalDataLen), repr(self.length), repr(self.currPos)))
        self.isValid = False
        return None


class PHPDBGFrame(object):
    FRAME_HEADER_SIZE = 8
    def __init__(self, frameType):
        self.frameType = IntToC4(frameType)
        self.frameData = ""

    def addInt(self, intVal):
        self.frameData = self.frameData + IntToC4(intVal)

    def addChar(self, charVal):
        self.frameData = self.frameData + charVal

    def addStr(self, string):
        #
        # Add the trailing '\0'.
        #
        self.frameData = self.frameData + string + '\0'

    def getSize(self):
        return len(self.frameData) + PHPDBGFrame.FRAME_HEADER_SIZE

    def writeFrame(self, conn):
        header = self.frameType + IntToC4(len(self.frameData))
        conn.sendall(header)
        conn.sendall(self.frameData)


class PHPDBGPacket(object):
    def __init__(self, packetType):
        self.header     = DBG_SYNC2_STR + packetType
        self.frames     = []
        self.packetSize = 0

    def addFrame(self, frame):
        self.frames.append(frame)
        self.packetSize += frame.getSize()

    def sendPacket(self, conn, flags = 0):
        self.header += IntToC4(flags)
        self.header += IntToC4(self.packetSize)
        conn.sendall(self.header)
        for frame in self.frames:
            frame.writeFrame(conn)


class PHPDBGException(Exception):
    def __init__(self, msg = None, cause = None):
        if (msg == None):
            Exception.__init__(self)
        elif (cause == None):
            Exception.__init__(self, msg)
        else:
            Exception.__init__(self, "PHPDBGException: message:%s\n, cause:%s" % (msg, cause))


class PHPDBGConnException(PHPDBGException):
    pass


class PHPDebuggerCallback(object):
    ACTION_NONE              = 0
    ACTION_STOP              = 1
    ACTION_LISTEN            = 2

    def __init__(self, ui, service, lsnrHosti, lsnrPorti):
        self.ui              = ui
        self.service         = service
        self.lsnrHost        = lsnrHosti
        self.lsnrPort        = lsnrPorti
        self.lsnrThr         = None
        self.lsnrAction      = PHPDebuggerCallback.ACTION_NONE
        self.clearInternals()
        self.initLsnrThr()


    ############################################################################
    #                      Public callback functions begin
    #
    def Start(self):
        self.lsnrThr.start()

    def ShutdownServer(self, stopLsnr = True):
        #
        # First to tell php debugger to stop execution of the current PHP
        # program.  Disconnect with php dbg module too.
        #
        self.stopPhpDbg()

        #
        # Stop debug listener.
        #
        if stopLsnr:
            self.stopLsnr()

    def BreakExecution(self):
        reqPacket = PHPDBGPacket(IntToC4(DBGC_PAUSE))

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.awaitAndHandleResponse()
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.ui.LoadPHPFramesList(self.stackList)
        return

    def SingleStep(self):
        reqPacket = PHPDBGPacket(DBGA_STEPINTO)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.lastCommand = DBGA_STEPINTO
            self.awaitAndHandleResponse(blocking = True)
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.ui.LoadPHPFramesList(self.stackList)
        return

    def Next(self):
        reqPacket = PHPDBGPacket(DBGA_STEPOVER)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.lastCommand = DBGA_STEPOVER
            self.awaitAndHandleResponse(blocking = True)
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.ui.LoadPHPFramesList(self.stackList)
        return

    def Continue(self):
        reqPacket = PHPDBGPacket(DBGA_CONTINUE)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.lastCommand = DBGA_CONTINUE
            self.awaitAndHandleResponse(blocking = True)
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.ui.LoadPHPFramesList(self.stackList)
        return

    def Return(self):
        reqPacket = PHPDBGPacket(DBGA_STEPOUT)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.lastCommand = DBGA_STEPOUT
            self.awaitAndHandleResponse(blocking = True)
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.ui.LoadPHPFramesList(self.stackList)
        return

    def PushBreakpoints(self, noRemove = False):
        tmpList = []
        bps     = self.service.GetMasterBreakpointDict()
        for fileName in bps.keys():
            if fileName.endswith('.php'):
                lines = bps[fileName]
                if lines:
                    for lineNo in lines:
                        if lineNo:
                            #
                            # A tuple (fileName, lineNo) is an item which is
                            # used as a key in self.bpDict.
                            #
                            tmpList.append(self.createBpKey(fileName, lineNo))
                            myprint("PushBreakpoints(): global breakpoint \'%s:%i\'", (fileName, lineNo))

        #
        # Check to see if we have any new breakpoints added.
        #
        for oneKey in tmpList:
            if not self.bpDict.has_key(oneKey):
                #
                # A new breakpoint.
                #
                newBp = BreakPoint(self, oneKey[0], oneKey[1])
                newBp.addSelf()
                self.bpDict[oneKey] = newBp
                myprint("PushBreakpoints(): newly added global breakpoint \'%s:%i\'", (oneKey[0], oneKey[1]))

        if noRemove:
            return

        #
        # Check to see if any bp that is in our list, but not in the latest
        # global list.  If so, it must have been removed recently in the
        # global one.  Remove it from our list and tell php debugger to do
        # so as well.
        #
        toRemoveList = []
        for oneKey in self.bpDict.keys():
            if tmpList.count(oneKey) == 0:
                toRemoveList.append((oneKey, self.bpDict[oneKey]))
                myprint("PushBreakpoints(): recently removed global breakpoint \'%s:%i\'", (oneKey[0], oneKey[1]))

        for bp in toRemoveList:
            bp[1].removeSelf()
            del self.bpDict[bp[0]]
            myprint("PushBreakpoints(): successfully removed breakpoint \'%s:%i\' from both our local list and php debugger", (bp[0][0], bp[0][1]))

        return
    #
    #                      Public callback functions end
    ############################################################################


    def newConnEventHandler(self):
        #
        # Ok, we've got a connection from the php debugger, and some initial
        # frame data from it.  Everything is ready and let's make some initial
        # actions.
        #
        self.clearInternals()

        try:
            self.awaitAndHandleResponse(self.lsnrThr.getConnHeader())
        except PHPDBGConnException:
            self.currConnFinished()
            return

        self.PushBreakpoints(True)
        self.ui.LoadPHPFramesList(self.stackList)

    #
    # This could be called when this object is constructed or when self is
    # re-initialized after getting a new dbg module connection as a new
    # session.
    #
    def clearInternals(self):
        self.stackList       = []
        self.errStackList    = []
        self.stackFrameIndex = 0
        self.isErrStack      = False
        self.errStr          = ''
        self.modList         = []
        self.stopOnError     = True
        self.lastCommand     = None
        self.evalRet         = ''
        self.modDict         = {}
        self.bpDict          = {}
        self.rawDataDict     = {}
        self.sessID          = 0
        self.sessType        = 0
        self.sessEnded       = False
        self.frameCounter    = 1000
        self.variableList    = []
        self.verMajor        = 0
        self.verMinor        = 0
        self.verDesc         = None

    def initLsnrThr(self):
        self.actionEvent = threading.Event()
        self.lsnrThr     = PHPDBGLsnrThr(self, self.lsnrHost, self.lsnrPort, self.actionEvent, self.ui)

    def awaitAndHandleResponse(self, header = None, blocking = False, disable = True, stopping = False):
        if disable:
            self.ui.DisableWhileDebuggerRunning()

        while self.readResponse(header, blocking) != 0:
            myprint("Waiting for response")

        if stopping:
            self.ui.DisableAfterStop()
        else:
            self.ui.EnableWhileDebuggerStopped()

    def requestDBGVersion(self):
        #TODO: necessary?
        pass

    def getSourceTree(self):
        #TODO: necessary?
        pass

    def addDBGModName(self):
        #TODO: necessary?
        pass

    def getNextFrameCounter(self):
        self.frameCounter = self.frameCounter + 1
        return self.frameCounter

    def getVariables(self, stack):
        self.variableList = []

        reqPacket = PHPDBGPacket(DBGA_REQUEST)
        reqFrame  = PHPDBGFrame(FRAME_EVAL)

        reqFrame.addInt(0)
        reqFrame.addInt(stack.getFrameScopeId())
        reqPacket.addFrame(reqFrame)
        myprint("PHPDebuggerCallback::getVariables(): about to send eval request")

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.awaitAndHandleResponse(disable = False)
        except PHPDBGConnException:
            self.currConnFinished()
            return self.variableList

        myprint("PHPDebuggerCallback::getVariables(): evalRet=%s", self.evalRet)
        evalStr = PHPDBGEvalString(stack, self.evalRet)
        if evalStr:
            self.variableList = evalStr.getVars()
        myprint("PHPDebuggerCallback::getVariables(): about to return")

        return self.variableList

    def evalBlock(self, stack, evalStr):
        reqPacket = PHPDBGPacket(DBGA_REQUEST)
        reqFrame1 = PHPDBGFrame(FRAME_EVAL)
        reqFrame2 = PHPDBGFrame(FRAME_RAWDATA)

        frameID = self.getNextFrameCounter()
        reqFrame1.addInt(frameID)
        reqFrame1.addInt(1)

        reqFrame2.addInt(frameID)
        reqFrame2.addInt(len(evalStr) + 1)
        reqFrame2.addStr(evalString)

        reqPacket.addFrame(reqFrame2)
        reqPacket.addFrame(reqFrame1)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.awaitAndHandleResponse(disable = False)
        except PHPDBGConnException:
            self.currConnFinished()
            return None

        evalStr = PHPDBGEvalString(stack, self.evalRet)

        return evalStr.getVars()

    def getBPUnderHit(self):
        for bp in self.bpDict.values():
            if bp.isUnderHit():
                return bp

        return None

    def getRawFrameData(self, frameNo):
        if self.rawDataDict.has_key(frameNo):
            #
            # Once the frameData is consumed, remove it from rawDataDict.
            #
            return self.rawDataDict.pop(frameNo)
        else:
            #
            # TODO: do we need to handle the case when the raw frame data hasn't
            # been received before?
            #
            return None

    def getModByNum(self, modNum):
        if self.modDict.has_key(modNum):
            return self.modDict[modNum]
        else:
            return None

    def getModByFileName(self, fileName):
        for mn, fn in self.modDict.iteritems():
            if fn == fileName:
                return mn

        return 0

    def setMod(self, modNum, fileName):
        if modNum != 0 and fileName:
            self.modDict[modNum] = fileName

        return

    def readResponse(self, headeri = None, blockingi = False):
        inHeader        = headeri
        header          = None
        cmdReceived     = 0
        isFirstPacket   = True
        blocking        = blockingi
        self.isErrStack = False
        self.rawDataDict.clear()

        while True:
            #
            # If we have already received the first packet, we can't block any
            # more.
            #
            if not isFirstPacket:
                blocking = False

            #
            # If this is the first loop and we have a non-empty header passed in, use it.  Otherwise,
            # read in a new header.  For subsequent loops, inHeader is None so we always read a new
            # header from the wire.
            #
            if inHeader:
                header   = inHeader
                inHeader = None
            else:
                header   = ResponseHeader(self.lsnrThr, blocking)

            if not header.isValid:
                return 0

            cmdReceived = header.command
            frame       = ResponsePacketFrame(self.lsnrThr, header.toRead, None, blocking)
            if not frame.isValid:
                return 0

            isFirstPacket     = False
            isFirstStackFrame = True
            while frame and frame.isValid:
                frameName = frame.frameName
                if frameName == FRAME_STACK:
                    if self.isErrStack:
                        self.errStackList = self.handleRespFrameStack(self.errStackList, frame, isFirstStackFrame)
                    else:
                        self.stackList = self.handleRespFrameStack(self.stackList, frame, isFirstStackFrame)

                    if isFirstStackFrame:
                        isFirstStackFrame = False
                elif frameName == FRAME_SOURCE:
                    self.handleRespFrameSource(frame)
                elif frameName == FRAME_SRC_TREE:
                    self.handleRespFrameSrcTree(frame)
                elif frameName == FRAME_RAWDATA:
                    self.handleRespFrameRawdata(frame)
                elif frameName == FRAME_ERROR:
                    self.handleRespFrameError(frame)
                elif frameName == FRAME_EVAL:
                    self.handleRespFrameEval(frame)
                elif frameName == FRAME_BPS:
                    self.handleRespFrameBps(frame)
                elif frameName == FRAME_BPL:
                    self.handleRespFrameBpl(frame)
                elif frameName == FRAME_VER:
                    self.handleRespFrameVer(frame)
                elif frameName == FRAME_SID:
                    self.handleRespFrameSid(frame)
                elif frameName == FRAME_SRCLINESINFO:
                    self.handleRespFrameSrclinesinfo(frame)
                elif frameName == FRAME_SRCCTXINFO:
                    self.handleRespFrameSrcctxinfo(frame)
                elif frameName == FRAME_LOG:
                    self.handleRespFrameLog(frame)
                elif frameName == FRAME_PROF:
                    self.handleRespFrameProf(frame)
                elif frameName == FRAME_PROF_C:
                    self.handleRespFrameProfC(frame)
                elif frameName == FRAME_SET_OPT:
                    self.handleRespFrameSetOpt(frame)
                else:
                    self.handleRespFrameUnknown(frame)
                    return 0

                #
                # After handling of this frame, force frame to point to the
                # next one based on current frame's absolute size.
                #
                frame = frame.getNextFrame(True)

            if cmdReceived == DBGC_REPLY:
                self.handleRespCmdReply()
            elif cmdReceived == DBGC_STARTUP:
                self.handleRespCmdStartup()
            elif cmdReceived == DBGC_END:
                self.handleRespCmdEnd()
            elif cmdReceived == DBGC_BREAKPOINT:
                self.handleRespCmdBreakpoint()
                cmdReceived = 0
            elif cmdReceived == DBGC_STEPINTO_DONE:
                self.handleRespCmdStepintoDone()
            elif cmdReceived == DBGC_STEPOVER_DONE:
                self.handleRespCmdStepoverDone()
            elif cmdReceived == DBGC_STEPOUT_DONE:
                self.handleRespCmdStepoutDone()
            elif cmdReceived == DBGC_EMBEDDED_BREAK:
                self.handleRespCmdEmbeddedBreak()
            elif cmdReceived == DBGC_PAUSE:
                self.handleRespCmdPause()
            elif cmdReceived == DBGC_ERROR:
                self.handleRespCmdError()
            elif cmdReceived == DBGC_LOG:
                self.handleRespCmdLog()
            elif cmdReceived == DBGC_SID:
                self.handleRespCmdSid()
            else:
                self.handleRespCmdUnknown()

        return cmdReceived

    def handleRespFrameStack(self, stackList, frame, isFirst):
        if isFirst:
            stackList            = []
            self.stackFrameIndex = 0

        lineNo  = frame.getNextInt()
        modNo   = frame.getNextInt()
        scopeId = frame.getNextInt()
        frameId = frame.getNextInt()
        if modNo != 0:
            newStackFrame = PHPStackFrame(self, self.getModByNum(modNo), lineNo, self.stackFrameIndex, scopeId, self.getRawFrameData(frameId), modNo)
            stackList.append(newStackFrame)
            self.stackFrameIndex = self.stackFrameIndex + 1

        return stackList

    def handleRespFrameSource(self, frame):
        modNo           = frame.getNextInt()
        fromFilePos     = frame.getNextInt()
        error           = frame.getNextInt()
        fullSize        = frame.getNextInt()
        fileNameFrameId = frame.getNextInt()
        textFrameId     = frame.getNextInt()

        fileName = self.getModByNum(modNo)
        if not fileName:
            self.setFileMod(modNo, fileNameFrameId)

        #
        # TODO: fullSize string and textFrameId are not handled here.
        #
        return

    def handleRespFrameSrcTree(self, frame):
        parentModNo     = frame.getNextInt()
        parentLineNo    = frame.getNextInt()
        modNo           = frame.getNextInt()
        fileNameFrameId = frame.getNextInt()

        fileName = self.getModByNum(modNo)
        if not fileName:
            self.setFileMod(modNo, fileNameFrameId)

        return

    def handleRespFrameRawdata(self, frame):
        frameNo = frame.getNextInt()
        if frameNo > 0:
            toRead = frame.getNextInt()
            if toRead > 0:
                str = frame.getNextString(toRead)
                self.rawDataDict[frameNo] = str
                myprint("handleRespFrameRawdata(): added \'%d\'=\'%s\' to rawDataDict.", (frameNo, str))

        return

    def handleRespFrameError(self, frame):
        self.isErrStack = True

        #
        # Type of the error.
        #
        errInt0         = frame.getNextInt()
        #
        # ID of error message.
        #
        errInt1         = frame.getNextInt()

        if errInt0 == E_ERROR:
            errIdStr = "[Error]"
        elif errInt0 == E_WARNING:
            errIdStr = "[Warning]"
        elif errInt0 == E_PARSE:
            errIdStr = "[Parse Error]"
        elif errInt0 == E_NOTICE:
            errIdStr = "[Notice]"
        elif errInt0 == E_CORE_ERROR:
            errIdStr = "[Core Error]"
        elif errInt0 == E_CORE_WARNING:
            errIdStr = "[Core Warning]"
        elif errInt0 == E_COMPILE_ERROR:
            errIdStr = "[Compile Error]"
        elif errInt0 == E_COMPILE_WARNING:
            errIdStr = "[Compile Warning]"
        elif errInt0 == E_USER_ERROR:
            errIdStr = "[User Error]"
        elif errInt0 == E_USER_WARNING:
            errIdStr = "[User Warning]"
        elif errInt0 == E_USER_NOTICE:
            errIdStr = "[User Notice]"
        else:
            errIdStr = "[Unexpected Error]"

        errMsg = self.getRawFrameData(errInt1)
        if errMsg and len(errMsg) > 0:
            self.errStr = errIdStr + ": " + errMsg + "\n"
        else:
            self.errStr = errIdStr + ": <Invalid Error Message>\n"

        if not self.stopOnError:
            if self.lastCommand == DBGA_CONTINUE:
                self.Continue()
            elif self.lastCommand == DBGA_STEPINTO:
                self.SingleStep()
            elif self.lastCommand == DBGA_STEPOUT:
                self.Return()
            elif self.lastCommand == DBGA_STEPOVER:
                self.Next()

        return

    def handleRespFrameEval(self, frame):
        evalInt0        = frame.getNextInt()
        evalInt1        = frame.getNextInt()
        evalInt2        = frame.getNextInt()
        self.evalRet    = self.getRawFrameData(evalInt1)
        #TODO: is the following necessary?
        evalStr         = self.getRawFrameData(evalInt0)

        return

    def handleRespFrameBps(self, frame):
        return

    def handleRespFrameBpl(self, frame):
        #
        # Get this breakpoint.
        #
        dbgBp = []
        for i in range(10):
            dbgBp.append(frame.getNextInt())

        if dbgBp[2] != 0:
            #
            # If filename is sent, get it from the rawDataDict.
            #
            fileName = self.getRawFrameData(dbgBp[2])
            if not fileName:
                return

            #
            # If this filename comes with a mod number, store this
            # modNum/fileName into this session's modDict.  Notice it might
            # overwrite previous value.
            #
            if dbgBp[0] != 0:
                self.setMod(dbgBp[0], fileName)
        elif dbgBp[0] != 0:
            #
            # Use modNum to get the fileName.
            #
            fileName = self.getModByNum(dbgBp[0])
            if not fileName:
                return
        else:
            #
            # Couldn't get the filename; nothing we can do with.
            #
            return

        bpKey = self.createBpKey(fileName, dbgBp[1])
        if not self.bpDict.has_key(bpKey):
            #
            # Not in our bp list?  Anyway, create one for it.
            #
            ourBp              = BreakPoint(self, fileName, dbgBp[1], dbgBp[0], dbgBp[3], dbgBp[4], dbgBp[5], dbgBp[6], dbgBp[7], dbgBp[8], dbgBp[9])
            self.bpDict[bpKey] = ourBp
            newlyCreated       = True
        else:
            ourBp              = self.bpDict[bpKey]
            newlyCreated       = False

        #
        # Update with the latest bp information.
        #
        if not newlyCreated:
            ourBp.update(dbgBp)

        return

    def handleRespFrameVer(self, frame):
        self.verMajor = frame.getNextInt()
        self.verMinor = frame.getNextInt()
        verFrameNo    = frame.getNextInt()
        self.verDesc  = self.getRawFrameData(verFrameNo)
        myprint("respFrameVer: verMajor=%s, verMinor=%s, verDesc=%s", (repr(self.verMajor), repr(self.verMinor), repr(self.verDesc)))

        return

    def handleRespFrameSid(self, frame):
        self.sessID   = frame.getNextInt()
        self.sessType = frame.getNextInt()
        myprint("respFrameSid: sessID=%s, sessType=%s", (self.sessID, self.sessType))

        return

    def handleRespFrameSrclinesinfo(self, frame):
        return

    def handleRespFrameSrcctxinfo(self, frame):
        return

    def handleRespFrameLog(self, frame):
        #
        # TODO:
        # Now we don't do much here besides following the protocol to retrieve
        # the data.
        #
        logId           = frame.getNextInt()
        logType         = frame.getNextInt()
        modNo           = frame.getNextInt()
        lineNo          = frame.getNextInt()
        fileNameFrameId = frame.getNextInt()
        extInfo         = frame.getNextInt()

        fileName = self.getModByNum(modNo)
        if not fileName:
            self.setFileMod(modNo, fileNameFrameId)

        return

    def handleRespFrameProf(self, frame):
        return

    def handleRespFrameProfC(self, frame):
        return

    def handleRespFrameSetOpt(self, frame):
        return

    def handleRespCmdReply(self):
        return

    def handleRespCmdStartup(self):
        return

    def handleRespCmdEnd(self):
        self.sessEnded = True
        return

    def handleRespCmdBreakpoint(self):
        return

    def handleRespCmdStepintoDone(self):
        return

    def handleRespCmdStepoverDone(self):
        return

    def handleRespCmdStepoutDone(self):
        return

    def handleRespCmdEmbeddedBreak(self):
        return

    def handleRespCmdPause(self):
        return

    def handleRespCmdError(self):
        self.stackList = []

        if len(self.errStackList) > 0:
            self.errStr = self.errStr + "Stack Trace:\n"

        while len(self.errStackList) > 0:
            oneStack = self.errStackList.pop()
            self.errStr = self.errStr + "%s\n" % oneStack.getLongDisplayStr()

        self.ui.showErrorDialog(self.errStr, "PHP Error")
        myprint("Got PHP Error:\n%s", self.errStr)

        return

    def handleRespCmdLog(self):
        return

    def handleRespCmdSid(self):
        return

    def setFileMod(self, modNo, fileNameFrameId):
        if fileNameFrameId != 0:
            fileName = self.getRawFrameData(fileNameFrameId)
            if fileName and modNo != 0:
                self.setMod(modNo, fileName)

        return

    def createBpKey(self, fileName, lineNo):
        #
        # This is to work around a bug in dbg module where it changes the path
        # names that we pass to it to lower cases.
        #
        if sysutils.isWindows():
            fileName = fileName.lower()

        return (fileName, lineNo)

    def setLsnrAction(self, actioni):
        self.lsnrAction = actioni
        return

    def getLsnrAction(self):
        return self.lsnrAction

    def currConnFinished(self):
        self.clearInternals()
        self.setLsnrAction(PHPDebuggerCallback.ACTION_LISTEN)
        self.actionEvent.set()
        self.ui.DisableAfterStop()

    def stopPhpDbg(self):
        #
        # TODO: should send a request to stop the current running PHP program.
        #       should handle network blocking issue correctly, otherwise, we
        #       might hang here.
        #
        reqPacket = PHPDBGPacket(DBGA_STOP)

        try:
            reqPacket.sendPacket(self.lsnrThr)
            self.awaitAndHandleResponse(stopping = True)
        except PHPDBGConnException:
            pass

        self.currConnFinished()
        return

    def stopLsnr(self):
        if not self.lsnrThr:
            return

        #
        # Then we try to stop our listener thread.
        #
        if self.lsnrThr.hasBeenConnected():
            #
            # If the listener thread has already accepted a connection from a
            # php debug module/client, it is sleeping now and wait for this
            # condition object to be set so that it can exit.
            #
            self.setLsnrAction(PHPDebuggerCallback.ACTION_STOP)
            self.actionEvent.set()
        else:
            #
            # If the listener thread has never been connected from a php debug
            # module/client, it is still blocking on a accept() call.  We
            # connect to it here and send a special shutdown command asking it
            # to exit.
            #
            shutdownMessage = IntToC4(DBG_SYNC) + IntToC4(DBGC_AG_SHUTDOWN_REQ) + IntToC4(0) + IntToC4(0)
            tempSocket      = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                tempSocket.connect((self.lsnrHost, self.lsnrPort))
                tempSocket.sendall(shutdownMessage)
            except:
                myprint("shutdown connection/send message got exception!")

            tempSocket.close()

        self.lsnrThr.join()
        self.lsnrThr = None


class PHPDBGLsnrThr(threading.Thread):
    def __init__(self, interfacei, hosti, porti, actionEventi, uii):
        threading.Thread.__init__(self)
        self.interface          = interfacei
        self.svrHost            = hosti
        self.svrPort            = porti
        self.actionEvent        = actionEventi
        self.svrSocket          = None
        self.clntConn           = None
        self.clntAddr           = None
        self.nonBlockingTimeout = 1
        self.connHeader         = None
        self.ui                 = uii

    def initSvrSocket(self):
        self.svrSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.svrSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.svrSocket.bind((self.svrHost, self.svrPort))

    def waitForClntConn(self):
        self.svrSocket.listen(5)
        self.clntConn, self.clntAddr = self.svrSocket.accept()
        self.clntConn.settimeout(self.nonBlockingTimeout)

    def run(self):
        #
        # Initialize this server socket.
        #
        self.initSvrSocket()

        while True:
            #
            # Block until we get a new connection from a php debug client or our
            # debugger ui (with a special shutting down header/command).
            #
            self.waitForClntConn()
    
            #
            # Ok, a new connection comes in ...  Read the header to see where it
            # comes from.
            #
            self.connHeader = ResponseHeader(self)
            if self.connHeader.command == DBGC_AG_SHUTDOWN_REQ:
                #
                # This is a special command coming from our UI asking this
                # thread to exit.  This only happens if after this thread has
                # been waiting for new connections from PHP debug module, no one
                # connects, and UI is ready to shutdown this thread.
                #
                self.shutdown()
                break
            else:
                #
                # Tell the main gui thread to handle this new connection.
                #
                wx.CallAfter(self.interface.newConnEventHandler)
    
                #
                # From now on, PHPDebuggerCallback will communicate with the php
                # debug module using this thread's clntConn socket.  This thread
                # itself will keep sleeping until get notified to make some
                # actions.
                #
                self.actionEvent.wait()
                self.actionEvent.clear()

                action = self.interface.getLsnrAction()
                if action == PHPDebuggerCallback.ACTION_STOP:
                    self.shutdown()
                    break
                elif action == PHPDebuggerCallback.ACTION_LISTEN:
                    if self.clntConn:
                        self.clntConn.shutdown(socket.SHUT_RDWR)
                        self.clntConn.close()
                        self.clntConn = None

                    continue
                else:
                    continue

    def shutdown(self):
        #
        # Cleanup and ready to exit.
        #
        self.clntConn.shutdown(socket.SHUT_RDWR)
        self.clntConn.close()
        self.svrSocket.close()

    def recv(self, size, blocking = False):
        if self.clntConn:
            myprint("recv: trying to receive %d bytes of data ...", size)
            if blocking:
                self.clntConn.settimeout(None)
            else:
                self.clntConn.settimeout(self.nonBlockingTimeout)

            try:
                rData = self.clntConn.recv(size)
            except socket.timeout:
                myprint("recv: got timed out")
                rData = None
            except:
                myprint("recv: got an unexpected exception: %s", sys.exc_info()[0])
                raise PHPDBGConnException

            return rData
        else:
            raise PHPDBGConnException

    def sendall(self, message):
        if self.clntConn:
            try:
                self.clntConn.sendall(message)
            except:
                myprint("sendall: got an unexpected exception: %s", sys.exc_info()[0])
                raise PHPDBGConnException
        else:
            raise PHPDBGConnException

    def hasBeenConnected(self):
        return self.clntConn != None

    def getConnHeader(self):
        return self.connHeader


class PHPValue(object):
    PEV_NAMES           = ("undefined", "long", "double", "string", "array", "object", "boolean", "resource", "reference", "soft reference", "null")
    PEVT_UNKNOWN        = 0
    PEVT_LONG           = 1
    PEVT_DOUBLE         = 2
    PEVT_STRING         = 3
    PEVT_ARRAY          = 4
    PEVT_OBJECT         = 5
    PEVT_BOOLEAN        = 6
    PEVT_RESOURCE       = 7
    PEVT_REF            = 8
    PEVT_SOFTREF        = 9
    PEVT_NULL           = 10

    NULL_VALUE_STR      = "NULL"
    TRUE_VALUE_STR      = "True"
    FALSE_VALUE_STR     = "False"
    OBJECT_VALUE_STR    = "<%s> object"
    STRING_VALUE_STR    = "\"%s\""
    REFERENCE_VALUE_STR = "<reference><%s>"
    RESOURCE_VALUE_STR  = "<%s><%s>"

    def __init__(self, frame, type, valueList):
        self.fStackFrame = frame
        self.fValueType  = type

        if type == self.PEVT_OBJECT:
            self.fValueString = self.OBJECT_VALUE_STR % valueList[0]
            self.fVariables   = valueList[1:]
        elif type == self.PEVT_ARRAY:
            self.fValueString = ''
            self.fVariables   = valueList
        else:
            self.fVariables = []
            if type == self.PEVT_STRING:
                self.fValueString = self.STRING_VALUE_STR % valueList[0]
            elif type == self.PEVT_NULL:
                self.fValueString = self.NULL_VALUE_STR
            elif type == self.PEVT_BOOLEAN:
                if valueList[0] == "0":
                    self.fValueString = self.FALSE_VALUE_STR
                else:
                    self.fValueString = self.TRUE_VALUE_STR
            elif type == self.PEVT_REF or type == self.PEVT_SOFTREF:
                self.fValueString = self.REFERENCE_VALUE_STR % valueList[0]
            elif type == self.PEVT_RESOURCE:
                self.fValueString = self.RESOURCE_VALUE_STR % (valueList[0], valueList[1])
            else:
                self.fValueString = valueList[0]

    def addVariable(self, item):
        if item != None:
            self.fVariables.append(item)

        return self.fVariables

    def setParent(self, parent):
        if self.fVariables != None and len(self.fVariables) > 0:
            for item in self.fVariables:
                item.setParent(parent)

    def getReferenceType(self):
        return self.fValueType

    def getReferenceTypeName(self):
        return self.PEV_NAMES[self.fValueType]

    def setReferenceType(self, type):
        self.fValueType = type

    def getValueString(self):
        return self.fValueString

    def getChildrenVariables(self):
        return self.fVariables

    def hasVariables(self):
        return len(self.fVariables) > 0

    def childrenIsSortable(self):
        #
        # TODO: if self.fValueType != self.PEVT_ARRAY:
        #
        return True


class PHPVariable(object):
    def __init__(self, frame, parent, valueType, name, valueList):
        self.fStackFrame = frame
        self.fName       = None
        self.fLongName   = None
        self.fPureName   = None
        self.fValue      = PHPValue(frame, valueType, valueList)
        self.fParent     = parent
        self.setName(name)
        self.setChildrensParent(valueList)

    def setName(self, name):
        self.fPureName = name

        type = self.getReferenceType()
        if type == PHPValue.PEVT_ARRAY:
            numItems = len(self.getChildrenVariables())
            self.fName = name + "[" + str(numItems) + "]"
        else:
            self.fName = name

        if not self.fParent or self.fParent.getName() == None:
            self.fLongName = name
        else:
            self.setLongName()

        return

    def setLongName(self):
        parentType = self.fParent.getReferenceType()
        if parentType == PHPValue.PEVT_ARRAY:
            self.fLongName = self.fParent.getLongName() + "['" + self.fPureName + "']"
        elif parentType == PHPValue.PEVT_OBJECT:
            self.fLongName = self.fParent.getLongName() + "." + self.fName
        else:
            self.fLongName = self.fName

        return

    def getValue(self):
        return self.fValue

    def getValueString(self):
        return self.fValue.getValueString()

    def getChildrenVariables(self):
        return self.fValue.getChildrenVariables()

    def getName(self):
        return self.fName

    def getParent(self):
        return self.fParent

    def setParent(self, parent):
        self.fParent = parent
        self.setLongName()
        return

    def setChildrensParent(self, childrenList):
        if self.fValue.hasVariables():
            for child in self.fValue.getChildrenVariables():
                child.setParent(self)

    def getLongName(self):
        return self.fLongName

    def getReferenceTypeName(self):
        return self.fValue.getReferenceTypeName()

    def getReferenceType(self):
        return self.fValue.getReferenceType()

    def setReferenceType(self, type):
        tp = self.getValue.setReferenceType(type)
        return tp

    def setValue(self, expression):
        if self.fValue.getReferenceType() == PHPValue.PEVT_STRING:
            evalString = self.fLongName + "=\"" + expression + "\""
        else:
            evalString = self.fLongName + "=" + expression

        vars = self.fStackFrame.getPHPDBGInterface().evalBlock(self.fStackFrame, evalString)
        self.fValue = vars[0].fValue

    def toString(self):
        rtype = self.getReferenceType()
        if rtype == PHPValue.PEVT_ARRAY:
            elements = len(self.fValue.getChildrenVariables())
            if elements == 0:
                tmpStr = self.getName() + " [no elements]"
            elif elements == 1:
                tmpStr = self.getName() + " [1 element]"
            else:
                tmpStr = self.getName() + " [" + str(elements) + " elements]"
        elif rtype == PHPValue.PEVT_OBJECT:
            tmpStr = self.getName() + " [ class: " + self.fValue.getValueString() + "]"
        elif rtype == PHPValue.PEVT_STRING:
            tmpStr = self.getName() + " = \"" + self.fValue.getValueString() + "\""
        else:
            tmpStr = self.getName() + " = " + self.fValue.getValueString()

        return tmpStr

    def hasChildren(self):
        return self.fValue.hasVariables()

    def childrenIsSortable(self):
        return self.fValue.childrenIsSortable()


class PHPStackFrame(object):
    def __init__(self, interface, file, line, frameIndex, scopeId, desc, modNum):
        self.interface      = interface
        self.fileName       = file
        self.lineNo         = line
        self.frameIndex     = frameIndex
        self.scopeId        = scopeId
        self.desc           = desc
        self.modNum         = modNum
        self.variables      = []
        self.shortFileName  = None
        self.shortDesc      = None
        self.displayStr     = None
        self.longDisplayStr = None

        self._getFileNamesAndShortDesc()

        myprint("PHPStackFrame::__init__(): new PHPStackFrame: file=%s, lineNo=%s, frameIndex=%s, scopeId=%s, desc=%s, modNum=%s, shortFileName=%s, shortDesc=%s", (repr(file), repr(line), repr(frameIndex), repr(scopeId), repr(desc), repr(modNum), repr(self.shortFileName), repr(self.shortDesc)))

    def _getFileNamesAndShortDesc(self):
        tmp = []
        if self.desc:
            tmp = self.desc.split("::")

        if self.fileName:
            self.shortFileName = os.path.basename(self.fileName)
            if len(tmp) == 2:
                self.shortDesc = tmp[1]
            elif len(tmp) == 1:
                self.shortDesc = tmp[0]
            else:
                self.shortDesc = None

            return

        #
        # The fileName is None, we will try our best efforts to get it.
        #
        if len(tmp) == 2:
            #
            # We retrieved long finename from the description.  If we haven't
            # stored the file mod before, set this one as the new one.
            # Otherwise, we prefer to keep the stored one.
            #
            if self.modNum != 0:
                storedFileName = self.interface.getModByNum(self.modNum)
                if not storedFileName:
                    self.interface.setMod(self.modNum, tmp[0])

            self.fileName      = tmp[0]
            self.shortFileName = os.path.basename(tmp[0])
            self.shortDesc     = tmp[1]
        elif len(tmp) == 1:
            self.fileName      = None
            self.shortFileName = None
            self.shortDesc     = tmp[0]
        else:
            self.shortFileName = None
            self.shortDesc     = None
            myprint("PHPStackFrame::_getFileNamesAndShortDesc(): something wrong with desc: %s?", self.desc)

        return

    def getShortFileName(self):
        return self.shortFileName

    def setShortFileName(self, shortFileName):
        self.shortFileName = shortFileName

    def getShortDesc(self):
        return self.shortDesc

    def getLineNo(self):
        return self.lineNo

    def getInterface(self):
        return self.interface

    def getVariables(self):
        if len(self.variables) == 0:
            self.variables = self.interface.getVariables(self)

        return self.variables

    def findVariables(self, s):
        if self.hasVariables():
            name = "$" + s
            for var in self.variables:
                if var.getName() == name:
                    return var

        return None

    def hasVariables(self):
        if len(self.variables) == 0:
            return False
        else:
            return True

    def getName(self):
        if self.getDesc():
            return self.getDesc() + " [line: " + str(self.getLineNo()) + "]"
        else:
            return self.getFileName() + " [line: " + str(self.getLineNo()) + "]"

    def getFileName(self):
        return self.fileName

    def setFileName(self, fileName):
        self.fileName = fileName

    def setDesc(self, desc):
        self.desc = desc

    def getDesc(self):
        return self.desc

    def getFrameScopeId(self):
        return self.scopeId

    def getFrameIndex(self):
        return self.frameIndex

    def getDisplayStr(self, stackList = None):
        if self.displayStr:
            return self.displayStr

        if not self.shortFileName:
            if stackList:
                i = stackList.index(self)
                for j in range(i + 1, len(stackList)):
                    self.shortFileName = stackList[j].getShortFileName()
                    if self.shortFileName:
                        self.fileName = stackList[j].getFileName()

        if self.shortFileName:
            if self.shortDesc:
                self.displayStr = "<%s> at %s:%d" % (self.shortDesc, self.shortFileName, self.lineNo)
            else:
                self.displayStr = "%s:%d" % (self.shortFileName, self.lineNo)
        else:
            if self.shortDesc:
                self.displayStr = "<%s>" % self.shortDesc
            else:
                self.displayStr = "<internal stack error>"

        return self.displayStr

    def getLongDisplayStr(self):
        if self.longDisplayStr:
            return self.longDisplayStr

        if self.fileName:
            if self.shortDesc:
                self.longDisplayStr = "<%s> at %s:%d" % (self.shortDesc, self.fileName, self.lineNo)
            else:
                self.longDisplayStr = "%s:%d" % (self.fileName, self.lineNo)
        else:
            if self.shortDesc:
                self.longDisplayStr = "<%s>" % self.shortDesc
            else:
                self.longDisplayStr = "<internal stack error>"

        return self.longDisplayStr

class BreakPoint(object):
    def __init__(self, interface, fileName, lineNo, modNum = 0, state = BPS_ENABLED + BPS_UNRESOLVED, isTemp = 0, hitCount = 0, skipHits = 0, condition = 0, bpId = 0, isUnderHit = 0):
        self.interface  = interface
        self.fileName   = fileName
        self.lineNo     = lineNo
        self.bpID       = bpId
        self.state      = state
        self.isTemp     = isTemp
        self.hitCount   = hitCount
        self.skipHits   = skipHits
        self.condition  = condition
        self.isUnderHit = 0
        if modNum == 0:
            self.modNum = self.interface.getModByFileName(fileName)
        else:
            self.modNum = modNum

        if self.modNum:
            self.fCounterOrZero = 0
        else:
            self.fCounterOrZero = interface.getNextFrameCounter()

    def sendSelf(self):
        reqPacket = PHPDBGPacket(DBGA_REQUEST)
        reqFrame1 = PHPDBGFrame(FRAME_BPS)

        if self.modNum:
            reqFrame1.addInt(self.modNum)
        else:
            #
            # 0 in modNum to tell to use fileName instead.
            #
            reqFrame1.addInt(0)

        reqFrame1.addInt(self.lineNo)             # lineNo
        reqFrame1.addInt(self.fCounterOrZero)     # fileName frameCounter or 0
        reqFrame1.addInt(self.state)              # state
        reqFrame1.addInt(self.isTemp)             # isTemp
        reqFrame1.addInt(self.hitCount)           # hitCount
        reqFrame1.addInt(self.skipHits)           # skipHits
        reqFrame1.addInt(self.condition)          # condition
        reqFrame1.addInt(self.bpID)               # breakpoint sequence id
        reqFrame1.addInt(self.isUnderHit)         # isUnderHit

        if not self.modNum:
            reqFrame2 = PHPDBGFrame(FRAME_RAWDATA)
            reqFrame2.addInt(self.fCounterOrZero)
            reqFrame2.addInt(len(self.fileName) + 1)
            reqFrame2.addStr(self.fileName)
            reqPacket.addFrame(reqFrame2)

        reqPacket.addFrame(reqFrame1)

        try:
            reqPacket.sendPacket(self.interface.lsnrThr)
            self.interface.awaitAndHandleResponse()
        except PHPDBGConnException:
            self.interface.currConnFinished()

        return

    def addSelf(self):
        self.sendSelf()

    def removeSelf(self):
        self.state = BPS_DISABLED
        self.sendSelf()

    def isUnderHit(self):
        return self.isUnderHit == 1

    def update(self, dbgBp):
        self.modNum     = dbgBp[0]
        self.state      = dbgBp[3]
        self.isTemp     = dbgBp[4]
        self.hitCount   = dbgBp[5]
        self.skipHits   = dbgBp[6]
        self.condition  = dbgBp[7]
        self.bpID       = dbgBp[8]
        self.isUnderHit = dbgBp[9]


class PHPDBGEvalString(object):
    def __init__(self, stackFrame, dataStr):
        self.stackFrame = stackFrame
        self.dataStr    = dataStr

    #
    # Get a list of variables under self.stackFrame.
    #
    def getVars(self):
        return self.parseAVariable(isRealVar = False)

    #
    # if isRealVar:
    #    returnList[0] = The Variable
    # else:
    #    returnList    = list of variables.
    #
    def parseAVariable(self, isRealVar = True):
        returnList = []

        #
        # Get the variable name first.  Notice we ignore this entity's data
        # type here.
        #
        if isRealVar:
            nameEntity = self.parseAnEntity()
            if not nameEntity or len(nameEntity) != 2 or type(nameEntity[1]) != str:
                myprint("PHPDBGEvalStr::parseAVariable() got a wrong name entity")
                return returnList
            else:
                varName = nameEntity[1]

        #
        # Get the variable's value.
        #
        valueEntity = self.parseAnEntity()
        if not valueEntity or len(valueEntity) < 1:
            myprint("PHPDBGEvalStr::parseAVariable(): couldn't get a variable's value entity.")
            return returnList

        #
        # This variable's data type.
        #
        varType = valueEntity[0]

        if isRealVar:
            #
            # If this is a real variable, return a list which contains only
            # this variable item.
            #
            #valueEntity = valueEntity[1:]
            variable    = PHPVariable(self.stackFrame, None, varType, varName, valueEntity[1:])
            #myprint("xxxxCreated variable varName=%s, valueEntity=%s", (repr(varName), repr(valueEntity[1])))
            myprint("xxxxCreated variable: %s", repr(variable.toString()))
            returnList.append(variable)
        else:
            #
            # If this is a root variable container, returns a list of
            # variables under the root.  Do a sanity check here.
            #
            if valueEntity[0] != PHPValue.PEVT_ARRAY:
                myprint("PHPDBGEvalStr::parseAVariable(): failed to parse the root variable container.")
            else:
                returnList = valueEntity[1:]

        return returnList

    #
    # An entity could be a variable's name or its value.
    #
    # returnList[0]  = variable data type
    # returnList[1:] = the real list
    #
    def parseAnEntity(self):
        if not self.dataStr or len(self.dataStr) < 2  or (self.dataStr[1] != ':' and self.dataStr[1] != ';'):
            myprint("PHPDBGEvalStr::parseAnEntity(): failed to parse %s.", repr(self.dataStr))
            return None

        returnList    = []
        typeChar      = self.dataStr[0]
        self.dataStr  = self.dataStr[2:]
        if typeChar   == 'i':
            returnList.append(PHPValue.PEVT_LONG)
            self.parseInt(returnList)
        elif typeChar == 'a':
            returnList.append(PHPValue.PEVT_ARRAY)
            self.parseArray(returnList)
        elif typeChar == 's':
            returnList.append(PHPValue.PEVT_STRING)
            self.parseString(returnList)
        elif typeChar == 'O':
            returnList.append(PHPValue.PEVT_OBJECT)
            self.parseObject(returnList)
        elif typeChar == 'r':
            returnList.append(PHPValue.PEVT_SOFTREF)
            self.parseReference(returnList, isSoftRef = True)
        elif typeChar == 'R':
            returnList.append(PHPValue.PEVT_REF)
            self.parseReference(returnList, isSoftRef = False)
        elif typeChar == 'b':
            returnList.append(PHPValue.PEVT_BOOLEAN)
            self.parseBoolean(returnList)
        elif typeChar == 'd':
            returnList.append(PHPValue.PEVT_DOUBLE)
            self.parseDouble(returnList)
        elif typeChar == 'z':
            returnList.append(PHPValue.PEVT_RESOURCE)
            self.parseResource(returnList)
        elif typeChar == 'N':
            returnList.append(PHPValue.PEVT_NULL)
            self.parseNull(returnList)
        else:
            myprint("PHPDBGEvalStr::parseAnEntity(): unknown data type: %s", typeChar)

        return returnList

    def parseInt(self, returnList):
        myprint("enter parseInt().")
        returnList.append(self.getAnIntStr(';'))

        return

    def parseArray(self, returnList):
        myprint("enter parseArray().")
        #
        # The shortest array is 'a:0:{}'.
        #
        if len(self.dataStr) < 4:
            myprint("PHPDBGEvalStr::parseArray(): failed (1) to parse an array: %s.", repr(self.dataStr))
            return

        expectedNumItems = self.getAnInt(':')
        if len(self.dataStr) < 2 or self.dataStr[0] != '{':
            myprint("PHPDBGEvalStr::parseArray(): failed (3) to parse an array: %s.", repr(self.dataStr))
            return

        self.dataStr = self.dataStr[1:]
        varList      = []
        while self.dataStr and len(self.dataStr) > 0 and self.dataStr[0] != '}':
            tmpList = self.parseAVariable()
            if not tmpList or len(tmpList) != 1 or not tmpList[0]:
                myprint("PHPDBGEvalStr::parseArray(): failed (4) to parse an array.  dataStr=%s.", repr(self.dataStr))
                break
            else:
                varList.append(tmpList[0])

        if expectedNumItems != len(varList):
            myprint("PHPDBGEvalStr::parseArray(): failed (5) expected no. of items=%d, but got %d", (expectedNumItems, len(varList)))

        #
        # An array should end with a '}'.
        #
        if self.dataStr and len(self.dataStr) > 0 and self.dataStr[0] == '}':
            self.dataStr = self.dataStr[1:]
            returnList.extend(varList)
        else:
            myprint("PHPDBGEvalStr::parseArray(): failed (6) to parse an array.  dataStr=%s.", repr(self.dataStr))

        myprint("parseArray() ends.")
        return

    def parseString(self, returnList, endChar = ';'):
        myprint("enter parseString().")
        #
        # The shortest string is 's:<str_len>:"<str>"<endChar>'.
        #
        if len(self.dataStr) < 5:
            myprint("PHPDBGEvalStr::parseString(): failed (1) to parse a string.  dataStr=%s.", repr(self.dataStr))
            return

        expectedStrLen = self.getAnInt(':')
        if len(self.dataStr) < expectedStrLen + 3 or self.dataStr[0] != '"':
            myprint("PHPDBGEvalStr::parseString(): failed (3) to parse a string.  dataStr=%s.", repr(self.dataStr))
            return

        strValue = self.dataStr[1:expectedStrLen + 1]
        if self.dataStr[expectedStrLen + 1:expectedStrLen + 3] != '"' + endChar:
            myprint("PHPDBGEvalStr::parseString(): failed (4) to parse a string.  dataStr=%s.", repr(self.dataStr))
            return

        #
        # Skip the starting double quote, real string, ending double quote, and ending semicolon.
        #
        self.dataStr = self.dataStr[expectedStrLen + 3:]
        returnList.append(strValue)

        return

    def parseObject(self, returnList):
        #
        # A simple sanity check.  The shortest object is:
        # 'O:<class_name_len>:"<class_name>":<num_of_items>:{<list_of_items>}'
        #
        if not self.dataStr or len(self.dataStr) < 10:
            myprint("PHPDBGEvalStr::parseObject(): failed (1) to parse an object: %s.", repr(self.dataStr))

        #
        # Get the class name in classNameList[0].
        #
        classNameList = []
        self.parseString(classNameList, ':')

        expectedNumItems = self.getAnInt(':')
        if len(self.dataStr) < 2 or self.dataStr[0] != '{':
            myprint("PHPDBGEvalStr::parseObject(): failed (2) to parse an object: %s.", repr(self.dataStr))
            return

        self.dataStr = self.dataStr[1:]
        varList      = []
        while self.dataStr and len(self.dataStr) > 0 and self.dataStr[0] != '}':
            tmpList = self.parseAVariable()
            if not tmpList or len(tmpList) != 1 or not tmpList[0]:
                myprint("PHPDBGEvalStr::parseObject(): failed (3) to parse an object.  dataStr=%s.", repr(self.dataStr))
                break
            else:
                varList.append(tmpList[0])

        if expectedNumItems != len(varList):
            myprint("PHPDBGEvalStr::parseObject(): failed (4) expected no. of items=%d, but got %d", (expectedNumItems, len(varList)))

        #
        # An object should end with a '}'.
        #
        if self.dataStr and len(self.dataStr) > 0 and self.dataStr[0] == '}':
            self.dataStr = self.dataStr[1:]
            returnList.append(classNameList[0])
            returnList.extend(varList)
        else:
            myprint("PHPDBGEvalStr::parseObject(): failed (6) to parse an object.  dataStr=%s.", repr(self.dataStr))

        myprint("parseObject() ends.")
        return

    def parseReference(self, returnList, isSoftRef):
        myprint("enter parseReference().")
        intStr = self.getAnIntStr(';')
        if intStr:
            returnList.append(intStr)

        return

    def parseBoolean(self, returnList):
        tmpBooleanStr = self.getAnIntStr(';')
        returnList.append(tmpBooleanStr)

        return

    def parseDouble(self, returnList):
        tmpStr = self.getAStrTillEndChar(';')
        if tmpStr:
            returnList.append(tmpStr)

        return

    def parseResource(self, returnList):
        tmpList = []
        self.parseString(tmpList, ':')

        if len(tmpList) == 1:
            returnList.extend(tmpList)
        else:
            return

        resourceId = self.getAnIntStr(';')
        if resourceId:
            returnList.append(resourceId)

        return

    def parseNull(self, returnList):
        return

    def getAStrTillEndChar(self, endChar):
        if len(self.dataStr) < 1:
            myprint("PHPDBGEvalStr::getAStrTillEndChar(): no more data string to work with.")
            return

        i = self.findNextChar(self.dataStr, endChar)
        if i == -1:
            myprint("PHPDBGEvalStr::getAStrTillEndChar(): no double/float string supplied.")
            return

        tmpStr       = self.dataStr[:i]
        self.dataStr = self.dataStr[i + 1:]

        if self.isFloat(tmpStr):
            return tmpStr
        else:
            myprint("PHPDBGEvalStr::getAStrTillEndChar(): parsing error.  tried to get an float number, but get %s.", tmpStr)
            return None

    def getAnInt(self, endChar):
        tmpStr = self.getAnIntStr(endChar)
        if tmpStr:
            return int(tmpStr)
        else:
            return 0

    def getAnIntStr(self, endChar):
        if len(self.dataStr) == 0:
            myprint("PHPDBGEvalStr::getAnIntStr(): no more data string to work with.")
            return

        i = self.findNextChar(self.dataStr, endChar)
        if i == -1:
            tmpStr = self.dataStr
            self.dataStr = ''
        else:
            tmpStr = self.dataStr[:i]
            self.dataStr = self.dataStr[i + 1:]

        if self.isInt(tmpStr):
            return tmpStr
        else:
            myprint("PHPDBGEvalStr::getAnIntStr(): parsing error.  tried to get an integer, but get %s.", tmpStr)
            return None

    def isInt(self, aStr):
        try:
            int(aStr)
        except ValueError:
            return False

        return True

    def isFloat(self, aStr):
        try:
            float(aStr)
        except ValueError:
            return False

        return True

    def findNextChar(self, aStr, aChar):
        try:
            index = aStr.index(aChar)
        except ValueError:
            index = -1

        return index
