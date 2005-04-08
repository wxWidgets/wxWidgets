#----------------------------------------------------------------------------
# Name:         DebuggerService.py
# Purpose:      Debugger Service for Python.
#
# Author:       Matt Fryer
#
# Created:      12/9/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.intctrl
import wx.lib.docview
import wx.lib.dialogs
import wx.gizmos
import wx._core
import wx.lib.pydocview
import Service
import STCTextEditor
import CodeEditor
import PythonEditor
from IDE import ACTIVEGRID_BASE_IDE
if not ACTIVEGRID_BASE_IDE:
    import ProcessModelEditor
import wx.lib.scrolledpanel as scrolled
import sys
import time
import SimpleXMLRPCServer
import xmlrpclib
import os
import threading
import process
import Queue
import SocketServer
import ProjectEditor
import types
from xml.dom.minidom import parse, parseString
import bz2
import pickle
import DebuggerHarness
import traceback
import StringIO
if wx.Platform == '__WXMSW__':
    import win32api
    _WINDOWS = True
else:
    _WINDOWS = False
_ = wx.GetTranslation

_VERBOSE = False
_WATCHES_ON = False

# Class to read from stdout or stderr and write the result to a text control.
# Args: file=file-like object
#       callback_function= function that takes a single argument, the line of text
#       read.
class OutputReaderThread(threading.Thread):
    def __init__(self, file, callback_function, callbackOnExit=None, accumulate=True):
        threading.Thread.__init__(self)
        self._file = file
        self._callback_function = callback_function
        self._keepGoing = True
        self._lineCount = 0
        self._accumulate = accumulate
        self._callbackOnExit = callbackOnExit
         
    def run(self):
        file = self._file
        start = time.time()
        output = ""
        while self._keepGoing:
            try:
                # This could block--how to handle that?
                text = file.readline()
                if text == '' or text == None:
                    self._keepGoing = False
                elif not self._accumulate:
                    self._callback_function(text)
                else:
                    # Should use a buffer? StringIO?
                    output += text
                # Seems as though the read blocks if we got an error, so, to be
                # sure that at least some of the exception gets printed, always 
                # send the first hundred lines back as they come in.
                if self._lineCount < 100:    
                    self._callback_function(output)
                    self._lineCount += 1
                    output = "" 
                elif time.time() - start > 0.25:
                    try:
                        self._callback_function(output) 
                    except wx._core.PyDeadObjectError:
                        # GUI was killed while we were blocked.
                        self._keepGoing = False
                    start = time.time()
                    output = "" 
            except:
                tp, val, tb = sys.exc_info()
                print "Exception in OutputReaderThread.run():", tp, val
                self._keepGoing = False
        if self._callbackOnExit:
            try:
                self._callbackOnExit()
            except wx._core.PyDeadObjectError:
                pass
        if _VERBOSE: print "Exiting OutputReaderThread"
        
    def AskToStop(self):
        self._keepGoing = False
        
import  wx.lib.newevent
(UpdateTextEvent, EVT_UPDATE_STDTEXT) = wx.lib.newevent.NewEvent()
(UpdateErrorEvent, EVT_UPDATE_ERRTEXT) = wx.lib.newevent.NewEvent()
        
class Executor:
    
    def GetPythonExecutablePath():
        config = wx.ConfigBase_Get()
        path = config.Read("ActiveGridPythonLocation")
        if path:
            return path
        wx.MessageBox(_("To proceed I need to know the location of the python.exe you would like to use.\nTo set this, go to Tools-->Options and use the 'Python' tab to enter a value.\n"), _("Python Executable Location Unknown"))
        return None
    GetPythonExecutablePath = staticmethod(GetPythonExecutablePath)   
     
    def __init__(self, fileName, wxComponent, arg1=None, arg2=None, arg3=None, arg4=None, arg5=None, arg6=None, arg7=None, arg8=None, arg9=None, callbackOnExit=None):
        self._fileName = fileName
        self._stdOutCallback = self.OutCall
        self._stdErrCallback = self.ErrCall
        self._callbackOnExit = callbackOnExit
        self._wxComponent = wxComponent
        path = Executor.GetPythonExecutablePath()
        self._cmd = '"' + path + '" -u \"' + fileName + '\"'
        #Better way to do this? Quotes needed for windows file paths.
        if(arg1 != None):
            self._cmd += ' \"' + arg1 + '\"'
        if(arg2 != None):
            self._cmd += ' \"' + arg2 + '\"'
        if(arg3 != None):
            self._cmd += ' \"' + arg3 + '\"'
        if(arg4 != None):
            self._cmd += ' \"' + arg4 + '\"'
        if(arg5 != None):
            self._cmd += ' \"' + arg5 + '\"'
        if(arg6 != None):
            self._cmd += ' \"' + arg6 + '\"'
        if(arg7 != None):
            self._cmd += ' \"' + arg7 + '\"'
        if(arg8 != None):
            self._cmd += ' \"' + arg8 + '\"'
        if(arg9 != None):
            self._cmd += ' \"' + arg9 + '\"'
        
        self._stdOutReader = None
        self._stdErrReader = None
        self._process = None
        
    def OutCall(self, text):
        evt = UpdateTextEvent(value = text)
        wx.PostEvent(self._wxComponent, evt)
                   
    def ErrCall(self, text):
        evt = UpdateErrorEvent(value = text)
        wx.PostEvent(self._wxComponent, evt)
            
    def Execute(self, arguments, startIn=None, environment=None):
        if not startIn:
            startIn = str(os.getcwd())
        startIn = os.path.abspath(startIn)
        command = self._cmd + ' ' + arguments
        #stdinput = process.IOBuffer()
        #self._process = process.ProcessProxy(command, mode='b', cwd=startIn, stdin=stdinput)
        self._process = process.ProcessOpen(command, mode='b', cwd=startIn, env=environment)
        # Kick off threads to read stdout and stderr and write them
        # to our text control. 
        self._stdOutReader = OutputReaderThread(self._process.stdout, self._stdOutCallback, callbackOnExit=self._callbackOnExit)
        self._stdOutReader.start()
        self._stdErrReader = OutputReaderThread(self._process.stderr, self._stdErrCallback, accumulate=False)
        self._stdErrReader.start()
                
    
    def DoStopExecution(self):
        if(self._process != None):
            self._process.kill()
            self._process.close()
            self._process = None
        if(self._stdOutReader != None):
            self._stdOutReader.AskToStop()
        if(self._stdErrReader != None):
            self._stdErrReader.AskToStop()
        
class RunCommandUI(wx.Panel):
    
    def __init__(self, parent, id, fileName):
        wx.Panel.__init__(self, parent, id)
        self._noteBook = parent
        
        self.KILL_PROCESS_ID = wx.NewId()
        self.CLOSE_TAB_ID = wx.NewId()
        
        self.Bind(wx.EVT_END_PROCESS, self.OnProcessEnded)
        
        # GUI Initialization follows
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        self._tb = tb = wx.ToolBar(self,  -1, wx.DefaultPosition, (30,1000), wx.TB_VERTICAL| wx.TB_FLAT, "Runner" )
        tb.SetToolBitmapSize((16,16))
        sizer.Add(tb, 0, wx.EXPAND |wx.ALIGN_LEFT|wx.ALL, 1)

        close_bmp = getCloseBitmap()
        tb.AddSimpleTool( self.CLOSE_TAB_ID, close_bmp, _('Close Window'))
        wx.EVT_TOOL(self, self.CLOSE_TAB_ID, self.OnToolClicked)
        
        stop_bmp = getStopBitmap()
        tb.AddSimpleTool(self.KILL_PROCESS_ID, stop_bmp, _("Stop the Run."))
        wx.EVT_TOOL(self, self.KILL_PROCESS_ID, self.OnToolClicked)
        
        tb.Realize()
        self._textCtrl = STCTextEditor.TextCtrl(self, wx.NewId()) #id)
        sizer.Add(self._textCtrl, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        self._textCtrl.SetViewLineNumbers(False)
        self._textCtrl.SetReadOnly(True)
        if wx.Platform == '__WXMSW__':
            font = "Courier New"
        else:
            font = "Courier"
        self._textCtrl.SetFont(wx.Font(9, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName = font))
        self._textCtrl.SetFontColor(wx.BLACK)
        self._textCtrl.StyleClearAll()
     
        #Disabling for now...may interfere with file open. wx.stc.EVT_STC_DOUBLECLICK(self._textCtrl, self._textCtrl.GetId(), self.OnDoubleClick)

        self.SetSizer(sizer)
        sizer.Fit(self)
        
        # Executor initialization
        self._executor = Executor(fileName, self, callbackOnExit=self.ExecutorFinished)
        self.Bind(EVT_UPDATE_STDTEXT, self.AppendText)
        self.Bind(EVT_UPDATE_ERRTEXT, self.AppendErrorText)
        
    def __del__(self):
        self._executor.DoStopExecution()
                 
    def Execute(self, initialArgs, startIn, environment):
        self._executor.Execute(initialArgs, startIn, environment)
    
    def ExecutorFinished(self):
        self._tb.EnableTool(self.KILL_PROCESS_ID, False)
        nb = self.GetParent()
        for i in range(0,nb.GetPageCount()):
            if self == nb.GetPage(i):
                text = nb.GetPageText(i)
                newText = text.replace("Running", "Finished")
                nb.SetPageText(i, newText)
                break
           
    def StopExecution(self):
        self.Unbind(EVT_UPDATE_STDTEXT)
        self.Unbind(EVT_UPDATE_ERRTEXT)
        self._executor.DoStopExecution()
        
    def AppendText(self, event):
        self._textCtrl.SetReadOnly(False)
        self._textCtrl.AddText(event.value)
        self._textCtrl.ScrollToLine(self._textCtrl.GetLineCount())
        self._textCtrl.SetReadOnly(True)
        
    def AppendErrorText(self, event):
        self._textCtrl.SetReadOnly(False)
        self._textCtrl.SetFontColor(wx.RED)   
        self._textCtrl.StyleClearAll()
        self._textCtrl.AddText(event.value)
        self._textCtrl.ScrollToLine(self._textCtrl.GetLineCount())
        self._textCtrl.SetFontColor(wx.BLACK)   
        self._textCtrl.StyleClearAll()
        self._textCtrl.SetReadOnly(True)
        
    #------------------------------------------------------------------------------
    # Event handling
    #-----------------------------------------------------------------------------
    
    def OnToolClicked(self, event):
        id = event.GetId()
        
        if id == self.KILL_PROCESS_ID:
            self._executor.DoStopExecution()
            
        elif id == self.CLOSE_TAB_ID:
            self._executor.DoStopExecution()
            index = self._noteBook.GetSelection()
            self._noteBook.GetPage(index).Show(False)
            self._noteBook.RemovePage(index)
            
    def OnDoubleClick(self, event):
        # Looking for a stack trace line.
        lineText, pos = self._textCtrl.GetCurLine()
        fileBegin = lineText.find("File \"")
        fileEnd = lineText.find("\", line ") 
        lineEnd = lineText.find(", in ")   
        if lineText == "\n" or  fileBegin == -1 or fileEnd == -1 or lineEnd == -1:
            # Check the line before the one that was clicked on
            lineNumber = self._textCtrl.GetCurrentLine()
            if(lineNumber == 0):
                return
            lineText = self._textCtrl.GetLine(lineNumber - 1)
            fileBegin = lineText.find("File \"")
            fileEnd = lineText.find("\", line ") 
            lineEnd = lineText.find(", in ")   
            if lineText == "\n" or  fileBegin == -1 or fileEnd == -1 or lineEnd == -1:
                return
                
        filename = lineText[fileBegin + 6:fileEnd]
        lineNum = int(lineText[fileEnd + 8:lineEnd])

        foundView = None
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:
            if openDoc.GetFilename() == filename:
                foundView = openDoc.GetFirstView()
                break

        if not foundView:
            doc = wx.GetApp().GetDocumentManager().CreateDocument(filename, wx.lib.docview.DOC_SILENT)
            foundView = doc.GetFirstView()

        if foundView:
            foundView.GetFrame().SetFocus()
            foundView.Activate()
            foundView.GotoLine(lineNum)
            startPos = foundView.PositionFromLine(lineNum)
            
        # FACTOR THIS INTO DocManager
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:  
            if(isinstance(openDoc.GetFirstView(), CodeEditor.CodeView)): 
                openDoc.GetFirstView().GetCtrl().ClearCurrentLineMarkers()    
                
        foundView.GetCtrl().MarkerAdd(lineNum -1, CodeEditor.CodeCtrl.CURRENT_LINE_MARKER_NUM)

    def OnProcessEnded(self, evt):
        self._executor.DoStopExecution()

DEFAULT_PORT = 32032
DEFAULT_HOST = 'localhost' 
PORT_COUNT = 21      

class DebugCommandUI(wx.Panel):
    debuggerPortList = None   
    debuggers = []
    
    def NotifyDebuggersOfBreakpointChange():
        for debugger in DebugCommandUI.debuggers:
            debugger.BreakPointChange()
            
    NotifyDebuggersOfBreakpointChange = staticmethod(NotifyDebuggersOfBreakpointChange)
 
    def DebuggerRunning():
        for debugger in DebugCommandUI.debuggers:
            if debugger._executor:
                return True
        return False
    DebuggerRunning = staticmethod(DebuggerRunning)
    
    def ShutdownAllDebuggers():
        for debugger in DebugCommandUI.debuggers:
            debugger.StopExecution()
            
    ShutdownAllDebuggers = staticmethod(ShutdownAllDebuggers)

    def GetAvailablePort():
        for index in range( 0, len(DebugCommandUI.debuggerPortList)):               
            port = DebugCommandUI.debuggerPortList[index]
            if DebugCommandUI.PortAvailable(port):
                DebugCommandUI.debuggerPortList.pop(index)
                return port
        wx.MessageBox(_("Out of ports for debugging!  Please restart the application builder.\nIf that does not work, check for and remove running instances of python."), _("Out of Ports"))
        assert False, "Out of ports for debugger."
        
    GetAvailablePort = staticmethod(GetAvailablePort)

    def ReturnPortToPool(port):
        config = wx.ConfigBase_Get()
        startingPort = config.ReadInt("DebuggerStartingPort", DEFAULT_PORT)
        if port in range(startingPort, startingPort + PORT_COUNT):
            DebugCommandUI.debuggerPortList.append(port)
            
    ReturnPortToPool = staticmethod(ReturnPortToPool)

    def PortAvailable(port):
        config = wx.ConfigBase_Get()
        hostname = config.Read("DebuggerHostName", DEFAULT_HOST)
        try:
            server = AGXMLRPCServer((hostname, port))
            server.server_close()
            if _VERBOSE: print "Port ", str(port), " available."
            return True
        except:
            tp,val,tb = sys.exc_info()
            if _VERBOSE: traceback.print_exception(tp, val, tb)
            if _VERBOSE: print "Port ", str(port), " unavailable."
            return False
            
    PortAvailable = staticmethod(PortAvailable)
        
    def NewPortRange():
        config = wx.ConfigBase_Get()
        startingPort = config.ReadInt("DebuggerStartingPort", DEFAULT_PORT)
        DebugCommandUI.debuggerPortList = range(startingPort, startingPort + PORT_COUNT)
    NewPortRange = staticmethod(NewPortRange)
    
    def __init__(self, parent, id, command, service):
        # Check for ports before creating the panel.
        if not DebugCommandUI.debuggerPortList:
            DebugCommandUI.NewPortRange()
        self._debuggerPort = str(DebugCommandUI.GetAvailablePort())
        self._guiPort = str(DebugCommandUI.GetAvailablePort())
        self._debuggerBreakPort = str(DebugCommandUI.GetAvailablePort())

        wx.Panel.__init__(self, parent, id)
        
        self._parentNoteBook = parent
        self._command = command
        self._textCtrl = None
        self._service = service
        self._executor = None
        self.STEP_ID = wx.NewId()
        self.CONTINUE_ID = wx.NewId()
        self.STEP_OUT_ID = wx.NewId()
        self.NEXT_ID = wx.NewId()
        self.KILL_PROCESS_ID = wx.NewId()
        self.CLOSE_WINDOW_ID = wx.NewId()
        self.BREAK_INTO_DEBUGGER_ID = wx.NewId()
        self.CLEAR_ID = wx.NewId()
        self.ADD_WATCH_ID = wx.NewId()
        sizer = wx.BoxSizer(wx.VERTICAL)
        self._tb = tb = wx.ToolBar(self,  -1, wx.DefaultPosition, (1000,30), wx.TB_HORIZONTAL| wx.NO_BORDER| wx.TB_FLAT| wx.TB_TEXT, "Debugger" )
        sizer.Add(tb, 0, wx.EXPAND |wx.ALIGN_LEFT|wx.ALL, 1)
        tb.SetToolBitmapSize((16,16))
       
        close_bmp = getCloseBitmap()
        tb.AddSimpleTool( self.CLOSE_WINDOW_ID, close_bmp, _('Close Window'))
        wx.EVT_TOOL(self, self.CLOSE_WINDOW_ID, self.StopAndRemoveUI)
        
        stop_bmp = getStopBitmap()
        tb.AddSimpleTool( self.KILL_PROCESS_ID, stop_bmp, _("Stop Debugging"))
        wx.EVT_TOOL(self, self.KILL_PROCESS_ID, self.StopExecution)
        
        tb.AddSeparator()
        
        break_bmp = getBreakBitmap()
        tb.AddSimpleTool( self.BREAK_INTO_DEBUGGER_ID, break_bmp, _("Break into Debugger"))
        wx.EVT_TOOL(self, self.BREAK_INTO_DEBUGGER_ID, self.BreakExecution)
        
        tb.AddSeparator()
                
        continue_bmp = getContinueBitmap()
        tb.AddSimpleTool( self.CONTINUE_ID, continue_bmp, _("Continue Execution"))
        wx.EVT_TOOL(self, self.CONTINUE_ID, self.OnContinue)
         
        tb.AddSeparator()
               
        next_bmp = getNextBitmap()
        tb.AddSimpleTool( self.NEXT_ID, next_bmp, _("Step to next line"))
        wx.EVT_TOOL(self, self.NEXT_ID, self.OnNext)
        
        step_bmp = getStepInBitmap()
        tb.AddSimpleTool( self.STEP_ID, step_bmp, _("Step in"))
        wx.EVT_TOOL(self, self.STEP_ID, self.OnSingleStep)
        
        stepOut_bmp = getStepReturnBitmap()
        tb.AddSimpleTool(self.STEP_OUT_ID, stepOut_bmp, _("Stop at function return"))
        wx.EVT_TOOL(self, self.STEP_OUT_ID, self.OnStepOut)

        tb.AddSeparator()
        if _WATCHES_ON:
            watch_bmp = getAddWatchBitmap()
            tb.AddSimpleTool(self.ADD_WATCH_ID, watch_bmp, _("Add a watch"))
            wx.EVT_TOOL(self, self.ADD_WATCH_ID, self.OnAddWatch)
            tb.AddSeparator()
        
        clear_bmp = getClearOutputBitmap()
        tb.AddSimpleTool(self.CLEAR_ID, clear_bmp, _("Clear output pane"))
        wx.EVT_TOOL(self, self.CLEAR_ID, self.OnClearOutput)
        
        tb.Realize()
        self.framesTab = None
        self.DisableWhileDebuggerRunning()
        self._notebook = wx.Notebook(self, -1, wx.DefaultPosition, wx.DefaultSize, wx.LB_DEFAULT, "Debugger")
        sizer.Add(self._notebook, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        self.consoleTab = self.MakeConsoleTab(self._notebook, wx.NewId(), None)
        self.framesTab = self.MakeFramesTab(self._notebook, wx.NewId(), None)
        self.breakPointsTab = self.MakeBreakPointsTab(self._notebook, wx.NewId(), None)
        self._notebook.AddPage(self.consoleTab, "Output")
        self._notebook.AddPage(self.framesTab, "Frames")
        self._notebook.AddPage(self.breakPointsTab, "Break Points")
        
        self._statusBar = wx.StatusBar( self, -1)
        self._statusBar.SetFieldsCount(1)
        sizer.Add(self._statusBar, 0, wx.EXPAND |wx.ALIGN_LEFT|wx.ALL, 1)

        self.SetStatusText("Starting debug...")
        
        self.SetSizer(sizer)
        sizer.Fit(self)
        config = wx.ConfigBase_Get()
        self._debuggerHost = self._guiHost = config.Read("DebuggerHostName", DEFAULT_HOST)
        url = 'http://' + self._debuggerHost + ':' + self._debuggerPort + '/'
        self._breakURL = 'http://' + self._debuggerHost + ':' + self._debuggerBreakPort + '/'
        self._callback = DebuggerCallback(self._guiHost, self._guiPort, url, self._breakURL, self)
        if DebuggerHarness.__file__.find('library.zip') > 0:
            try:
                fname = DebuggerHarness.__file__
                parts = fname.split('library.zip')
                path = os.path.join(parts[0],'activegrid', 'tool', 'DebuggerHarness.py')
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)   
            
        else:
            print "Starting debugger on these ports: %s, %s, %s" % (str(self._debuggerPort) , str(self._guiPort) , str(self._debuggerBreakPort))
            path = DebuggerService.ExpandPath(DebuggerHarness.__file__)
        self._executor = Executor(path, self, self._debuggerHost, \
                                                self._debuggerPort, self._debuggerBreakPort, self._guiHost, self._guiPort, self._command, callbackOnExit=self.ExecutorFinished)
        
        self.Bind(EVT_UPDATE_STDTEXT, self.AppendText)
        self.Bind(EVT_UPDATE_ERRTEXT, self.AppendErrorText)
        DebugCommandUI.debuggers.append(self)
        self._stopped = False
        
    def OnSingleStep(self, event):
        self._callback.SingleStep()

    def OnContinue(self, event):
        self._callback.Continue()
        
    def OnStepOut(self, event):
        self._callback.Return()
        
    def OnNext(self, event):
        self._callback.Next()

    def BreakPointChange(self):
        if not self._stopped:
            self._callback.pushBreakpoints()
        self.breakPointsTab.PopulateBPList()
        
    def __del__(self):
        if self in DebugCommandUI.debuggers:
            DebugCommandUI.debuggers.remove(self)
        
    def SwitchToOutputTab(self):
        self._notebook.SetSelection(0)
        
    def DisableWhileDebuggerRunning(self):
        self._tb.EnableTool(self.STEP_ID, False)
        self._tb.EnableTool(self.CONTINUE_ID, False)
        self._tb.EnableTool(self.STEP_OUT_ID, False)
        self._tb.EnableTool(self.NEXT_ID, False)
        self._tb.EnableTool(self.BREAK_INTO_DEBUGGER_ID, True)
        if _WATCHES_ON:
            self._tb.EnableTool(self.ADD_WATCH_ID, False)
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:  
            if(isinstance(openDoc.GetFirstView(), CodeEditor.CodeView)): 
                openDoc.GetFirstView().GetCtrl().ClearCurrentLineMarkers() 
        if self.framesTab:   
            self.framesTab.ClearWhileRunning()
        #wx.GetApp().ProcessPendingEvents() #Yield(True)
        
    def EnableWhileDebuggerStopped(self):
        self._tb.EnableTool(self.STEP_ID, True)
        self._tb.EnableTool(self.CONTINUE_ID, True)
        self._tb.EnableTool(self.STEP_OUT_ID, True)
        self._tb.EnableTool(self.NEXT_ID, True)
        if _WATCHES_ON:
            self._tb.EnableTool(self.ADD_WATCH_ID, True)
        self._tb.EnableTool(self.BREAK_INTO_DEBUGGER_ID, False)
        #if _WINDOWS:
        #    wx.GetApp().GetTopWindow().RequestUserAttention()
 
    def ExecutorFinished(self):          
        if _VERBOSE: print "In ExectorFinished"
        try:          
            self.DisableAfterStop()
        except wx._core.PyDeadObjectError:
            pass
        try:
            nb = self.GetParent()
            for i in range(0, nb.GetPageCount()):
                if self == nb.GetPage(i):
                    text = nb.GetPageText(i)
                    newText = text.replace("Debugging", "Finished")
                    nb.SetPageText(i, newText)
                    if _VERBOSE: print "In ExectorFinished, changed tab title."
                    break
        except:
            if _VERBOSE: print "In ExectorFinished, got exception"

    def DisableAfterStop(self):
        self.DisableWhileDebuggerRunning()
        self._tb.EnableTool(self.BREAK_INTO_DEBUGGER_ID, False)
        self._tb.EnableTool(self.KILL_PROCESS_ID, False)

    def SynchCurrentLine(self, filename, lineNum):
        # FACTOR THIS INTO DocManager
        self.DeleteCurrentLineMarkers()
        
        # Filename will be <string> if we're in a bit of code that was executed from
        # a string (rather than a file). I haven't been able to get the original string
        # for display.
        if filename == '<string>':
            return
        foundView = None
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:
            # This ugliness to prevent comparison failing because the drive letter
            # gets lowercased occasionally. Don't know why that happens or why  it
            # only happens occasionally.
            if DebuggerService.ComparePaths(openDoc.GetFilename(),filename):
                foundView = openDoc.GetFirstView()
                break
                
        if not foundView:
            if _VERBOSE:
                print "filename=", filename
            doc = wx.GetApp().GetDocumentManager().CreateDocument(DebuggerService.ExpandPath(filename), wx.lib.docview.DOC_SILENT)
            foundView = doc.GetFirstView()

        if foundView:
            foundView.GetFrame().SetFocus()
            foundView.Activate()
            foundView.GotoLine(lineNum)
            startPos = foundView.PositionFromLine(lineNum)
                
        foundView.GetCtrl().MarkerAdd(lineNum -1, CodeEditor.CodeCtrl.CURRENT_LINE_MARKER_NUM)

    def DeleteCurrentLineMarkers(self):
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:  
            if(isinstance(openDoc.GetFirstView(), CodeEditor.CodeView)): 
                openDoc.GetFirstView().GetCtrl().ClearCurrentLineMarkers()    
        
    def LoadFramesListXML(self, framesXML):
        self.framesTab.LoadFramesListXML(framesXML)  
          
    def SetStatusText(self, text):
        self._statusBar.SetStatusText(text,0)
            
    def Execute(self, initialArgs, startIn, environment):
        self._callback.start()
        self._executor.Execute(initialArgs, startIn, environment)
        self._callback.waitForRPC() 
                
    def BreakExecution(self, event):
        self._callback.BreakExecution()
        
                       
    def StopExecution(self, event):
        self._stopped = True
        self.DisableAfterStop()
        try:
            self._callback.ServerClose()
        except:
            pass 
        try:
            if self._executor:
                self._executor.DoStopExecution()
                self._executor = None
        except:
            pass
        self.DeleteCurrentLineMarkers()
        DebugCommandUI.ReturnPortToPool(self._debuggerPort)
        DebugCommandUI.ReturnPortToPool(self._guiPort)
        DebugCommandUI.ReturnPortToPool(self._debuggerBreakPort)

    def StopAndRemoveUI(self, event):
        self.StopExecution(None)
        if self in DebugCommandUI.debuggers:
            DebugCommandUI.debuggers.remove(self)
        index = self._parentNoteBook.GetSelection()
        self._parentNoteBook.GetPage(index).Show(False)
        self._parentNoteBook.RemovePage(index)

    def GetConsoleTextControl(self):
        return self._textCtrl
        
    def OnClearOutput(self, event):
        self._textCtrl.SetReadOnly(False)
        self._textCtrl.ClearAll()  
        self._textCtrl.SetReadOnly(True)

    def OnAddWatch(self, event):
        if self.framesTab:
            self.framesTab.OnWatch(event)
                  
    def MakeConsoleTab(self, parent, id, debugger):
        panel = wx.Panel(parent, id)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        self._textCtrl = STCTextEditor.TextCtrl(panel, wx.NewId())
        sizer.Add(self._textCtrl, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        self._textCtrl.SetViewLineNumbers(False)
        self._textCtrl.SetReadOnly(True)
        if wx.Platform == '__WXMSW__':
            font = "Courier New"
        else:
            font = "Courier"
        self._textCtrl.SetFont(wx.Font(9, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName = font))
        self._textCtrl.SetFontColor(wx.BLACK)
        self._textCtrl.StyleClearAll()
        panel.SetSizer(sizer)
        sizer.Fit(panel)

        return panel
        
    def MakeFramesTab(self, parent, id, debugger):
        panel = FramesUI(parent, id, self)
        return panel
        
    def MakeBreakPointsTab(self, parent, id, debugger):
        panel = BreakpointsUI(parent, id, self)
        return panel
        
    def AppendText(self, event):
        self._textCtrl.SetReadOnly(False)
        self._textCtrl.AddText(event.value)
        self._textCtrl.ScrollToLine(self._textCtrl.GetLineCount())
        self._textCtrl.SetReadOnly(True)
        
    def AppendErrorText(self, event):
        self._textCtrl.SetReadOnly(False)
        self._textCtrl.SetFontColor(wx.RED)   
        self._textCtrl.StyleClearAll()
        self._textCtrl.AddText(event.value)
        self._textCtrl.ScrollToLine(self._textCtrl.GetLineCount())
        self._textCtrl.SetFontColor(wx.BLACK)   
        self._textCtrl.StyleClearAll()
        self._textCtrl.SetReadOnly(True)
        
class BreakpointsUI(wx.Panel):
    def __init__(self, parent, id, ui):
        wx.Panel.__init__(self, parent, id)
        self._ui = ui
        self.currentItem = None
        self.clearBPID = wx.NewId()
        self.Bind(wx.EVT_MENU, self.ClearBreakPoint, id=self.clearBPID)
        self.syncLineID = wx.NewId()
        self.Bind(wx.EVT_MENU, self.SyncBPLine, id=self.syncLineID)

        sizer = wx.BoxSizer(wx.VERTICAL)
        p1 = self
        self._bpListCtrl = wx.ListCtrl(p1, -1, pos=wx.DefaultPosition, size=(1000,1000), style=wx.LC_REPORT)
        sizer.Add(self._bpListCtrl, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        self._bpListCtrl.InsertColumn(0, "File")   
        self._bpListCtrl.InsertColumn(1, "Line")   
        self._bpListCtrl.InsertColumn(2, "Path")   
        self._bpListCtrl.SetColumnWidth(0, 150)
        self._bpListCtrl.SetColumnWidth(1, 50)
        self._bpListCtrl.SetColumnWidth(2, 450)
        self._bpListCtrl.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK, self.OnListRightClick)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.ListItemSelected, self._bpListCtrl)
        self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.ListItemDeselected, self._bpListCtrl)

        self.PopulateBPList()

        p1.SetSizer(sizer)
        sizer.Fit(p1)
        p1.Layout()
        
    def PopulateBPList(self):
        list = self._bpListCtrl
        list.DeleteAllItems()

        bps = wx.GetApp().GetService(DebuggerService).GetMasterBreakpointDict()
        index = 0
        for fileName in bps.keys():
            shortFile = os.path.basename(fileName)
            lines = bps[fileName]
            if lines:
                for line in lines:
                    list.InsertStringItem(index, shortFile)
                    list.SetStringItem(index, 1, str(line))
                    list.SetStringItem(index, 2, fileName)
                    
    def OnListRightClick(self, event):
        menu = wx.Menu()
        item = wx.MenuItem(menu, self.clearBPID, "Clear Breakpoint")
        menu.AppendItem(item)
        item = wx.MenuItem(menu, self.syncLineID, "Goto Source Line")
        menu.AppendItem(item)
        self.PopupMenu(menu, event.GetPosition())
        menu.Destroy()

    def SyncBPLine(self, event):
        if self.currentItem != -1:
            list = self._bpListCtrl
            fileName = list.GetItem(self.currentItem, 2).GetText()
            lineNumber = list.GetItem(self.currentItem, 1).GetText()
            self._ui.SynchCurrentLine( fileName, int(lineNumber) )  

    def ClearBreakPoint(self, event):
        if self.currentItem >= 0:
            list = self._bpListCtrl
            fileName = list.GetItem(self.currentItem, 2).GetText()
            lineNumber = list.GetItem(self.currentItem, 1).GetText()
            wx.GetApp().GetService(DebuggerService).OnToggleBreakpoint(None, line=int(lineNumber) -1, fileName=fileName )  
        
    def ListItemSelected(self, event):
        self.currentItem = event.m_itemIndex

    def ListItemDeselected(self, event):
        self.currentItem = -1
 
class Watch:
    CODE_ALL_FRAMES = 1
    CODE_THIS_BLOCK = 2
    CODE_THIS_LINE = 4
    CODE_RUN_ONCE = 8
    
    def __init__(self, name, command, show_code=CODE_ALL_FRAMES):
        self._name = name
        self._command = command 
        self._show_code = show_code
               
class WatchDialog(wx.Dialog):
    WATCH_ALL_FRAMES = "Watch in all frames"
    WATCH_THIS_FRAME = "Watch in this frame only"
    WATCH_ONCE = "Watch once and delete"
    def __init__(self, parent, title, chain):
        wx.Dialog.__init__(self, parent, -1, title, style=wx.DEFAULT_DIALOG_STYLE)
        self._chain = chain
        self.label_2 = wx.StaticText(self, -1, "Watch Name:")
        self._watchNameTextCtrl = wx.TextCtrl(self, -1, "")
        self.label_3 = wx.StaticText(self, -1, "eval(", style=wx.ALIGN_RIGHT)
        self._watchValueTextCtrl = wx.TextCtrl(self, -1, "")
        self.label_4 = wx.StaticText(self, -1, ",frame.f_globals, frame.f_locals)")
        self.radio_box_1 = wx.RadioBox(self, -1, "Watch Information", choices=[WatchDialog.WATCH_ALL_FRAMES, WatchDialog.WATCH_THIS_FRAME, WatchDialog.WATCH_ONCE], majorDimension=0, style=wx.RA_SPECIFY_ROWS)

        self._okButton = wx.Button(self, wx.ID_OK, "OK", size=(75,-1))
        self._okButton.SetDefault()
        self._okButton.SetHelpText(_("The OK button completes the dialog"))
        def OnOkClick(event):
            if self._watchNameTextCtrl.GetValue() == "":
                wx.MessageBox(_("You must enter a name for the watch."), _("Add Watch"))
                return
            if self._watchValueTextCtrl.GetValue() == "":
                wx.MessageBox(_("You must enter some code to run for the watch."), _("Add Watch"))
                return
            self.EndModal(wx.ID_OK)
        self.Bind(wx.EVT_BUTTON, OnOkClick, self._okButton)
            
        self._cancelButton = wx.Button(self, wx.ID_CANCEL, _("Cancel"), size=(75,-1))
        self._cancelButton.SetHelpText(_("The Cancel button cancels the dialog."))
        
        self.__set_properties()
        self.__do_layout()
        
    def GetSettings(self):
        return self._watchNameTextCtrl.GetValue(), self._watchValueTextCtrl.GetValue(), self.GetSendFrame(), self.GetRunOnce()
    
    def GetSendFrame(self):
        return (WatchDialog.WATCH_ALL_FRAMES != self.radio_box_1.GetStringSelection())
        
    def GetRunOnce(self):
        return (WatchDialog.WATCH_ONCE == self.radio_box_1.GetStringSelection())
        
    def __set_properties(self):
        self.SetTitle("Add a Watch")
        #self.SetSize((400, 250))
        self.radio_box_1.SetSelection(0)

    def __do_layout(self):
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_4 = wx.FlexGridSizer(1, 3, 5, 5)
        grid_sizer_2 = wx.FlexGridSizer(1, 2, 5, 5)
        grid_sizer_2.Add(self.label_2, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        grid_sizer_2.Add(self._watchNameTextCtrl, 0, wx.EXPAND, 0)
        grid_sizer_2.AddGrowableCol(1)
        sizer_1.Add(grid_sizer_2, 1, wx.EXPAND, 0)
        grid_sizer_4.Add(self.label_3, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        grid_sizer_4.Add(self._watchValueTextCtrl, 0, wx.EXPAND, 0)
        grid_sizer_4.AddGrowableCol(1)
        grid_sizer_4.Add(self.label_4, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_1.Add(grid_sizer_4, 0, wx.EXPAND, 0)
        sizer_1.Add(self.radio_box_1, 0, wx.EXPAND, 0)
        
        box = wx.BoxSizer(wx.HORIZONTAL)
        box.Add(self._okButton, 0, wx.ALIGN_RIGHT|wx.ALL, 5)
        box.Add(self._cancelButton, 0, wx.ALIGN_RIGHT|wx.ALL, 5)
        sizer_1.Add(box, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_1)
        self.Layout()
        
class FramesUI(wx.SplitterWindow):
    def __init__(self, parent, id, ui):
        wx.SplitterWindow.__init__(self, parent, id, style = wx.SP_3D)
        self._ui = ui
        sizer = wx.BoxSizer(wx.VERTICAL)
        self._p1 = p1 = wx.ScrolledWindow(self, -1)
        p1.Bind(wx.EVT_SIZE, self.OnSize)

        self._framesListCtrl = wx.ListCtrl(p1, -1, pos=wx.DefaultPosition, size=(250,150), style=wx.LC_REPORT)
        sizer.Add(self._framesListCtrl, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        self._framesListCtrl.InsertColumn(0, "Frame")   
        self._framesListCtrl.SetColumnWidth(0, 250)
        self._framesListCtrl.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK, self.OnListRightClick)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.ListItemSelected, self._framesListCtrl)
        self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.ListItemDeselected, self._framesListCtrl)

        sizer2 = wx.BoxSizer(wx.VERTICAL)
        self._p2 = p2 = wx.ScrolledWindow(self, -1)
        p2.Bind(wx.EVT_SIZE, self.OnSize)

        self._treeCtrl = wx.gizmos.TreeListCtrl(p2, -1, size=(530,250), style=wx.TR_DEFAULT_STYLE| wx.TR_FULL_ROW_HIGHLIGHT)
        self._treeCtrl.Bind(wx.EVT_TREE_ITEM_RIGHT_CLICK, self.OnRightClick)
        sizer2.Add(self._framesListCtrl, 1, wx.ALIGN_LEFT|wx.ALL|wx.EXPAND, 1)
        tree = self._treeCtrl                           
        tree.AddColumn("Thing")
        tree.AddColumn("Value")
        tree.SetMainColumn(0) # the one with the tree in it...
        tree.SetColumnWidth(0, 175)
        tree.SetColumnWidth(1, 355)
        self._root = tree.AddRoot("Frame")
        tree.SetItemText(self._root, "", 1)

        self.SetMinimumPaneSize(20)
        self.SplitVertically(p1, p2, 250)
        self.currentItem = None
        self.Layout()
     
    def OnRightClick(self, event):
        #Refactor this...
        self._introspectItem = event.GetItem()
        self._parentChain = self.GetItemChain(event.GetItem())
        watchOnly = len(self._parentChain) < 1
        if not _WATCHES_ON and watchOnly:
            return
        menu = wx.Menu()
        if not watchOnly:
            if not hasattr(self, "introspectID"):
                self.introspectID = wx.NewId()
                self.Bind(wx.EVT_MENU, self.OnIntrospect, id=self.introspectID)
            item = wx.MenuItem(menu, self.introspectID, "Attempt Introspection")
            menu.AppendItem(item)
            menu.AppendSeparator()
        if _WATCHES_ON:
            if not hasattr(self, "watchID"):
                self.watchID = wx.NewId()
                self.Bind(wx.EVT_MENU, self.OnWatch, id=self.watchID)
            item = wx.MenuItem(menu, self.watchID, "Create a Watch")
            menu.AppendItem(item)
            menu.AppendSeparator()
        if not watchOnly:
            if not hasattr(self, "viewID"):
                self.viewID = wx.NewId()
                self.Bind(wx.EVT_MENU, self.OnView, id=self.viewID)
            item = wx.MenuItem(menu, self.viewID, "View in Dialog")
            menu.AppendItem(item)
        offset = wx.Point(x=0, y=20)
        menuSpot = event.GetPoint() + offset
        self._treeCtrl.PopupMenu(menu, menuSpot)
        menu.Destroy()
        self._parentChain = None
        self._introspectItem = None
        
    def GetItemChain(self, item):
        parentChain = []
        if item:
            if _VERBOSE: print 'Exploding: %s' % self._treeCtrl.GetItemText(item, 0)
            while item != self._root:
                text = self._treeCtrl.GetItemText(item, 0)
                if _VERBOSE: print "Appending ", text
                parentChain.append(text)
                item = self._treeCtrl.GetItemParent(item)
            parentChain.reverse()
        return parentChain
    
    def OnView(self, event):
        title = self._treeCtrl.GetItemText(self._introspectItem,0)
        value = self._treeCtrl.GetItemText(self._introspectItem,1)
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, value, title, style=wx.DD_DEFAULT_STYLE | wx.RESIZE_BORDER)
        dlg.Show()

    def OnWatch(self, event):
        try:
            if hasattr(self, '_parentChain'):
                wd = WatchDialog(wx.GetApp().GetTopWindow(), "Add a Watch", self._parentChain)
            else:
                wd = WatchDialog(wx.GetApp().GetTopWindow(), "Add a Watch", None)
            if wd.ShowModal() == wx.ID_OK:
                name, text, send_frame, run_once = wd.GetSettings()
                if send_frame:
                    frameNode = self._stack[int(self.currentItem)]
                    message = frameNode.getAttribute("message") 
                else:
                    message = ""
                binType = self._ui._callback._debuggerServer.add_watch(name, text, message, run_once)
                xmldoc = bz2.decompress(binType.data)
                domDoc = parseString(xmldoc)
                nodeList = domDoc.getElementsByTagName('watch')
                if len(nodeList) == 1:
                    watchValue = nodeList.item(0).getAttribute("message")
        except:
            tp, val, tb = sys.exc_info()
            traceback.print_exception(tp, val, tb)   
            
    def OnIntrospect(self, event):
        wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

        try:
            list = self._framesListCtrl
            frameNode = self._stack[int(self.currentItem)]
            message = frameNode.getAttribute("message") 
            binType = self._ui._callback._debuggerServer.attempt_introspection(message, self._parentChain)
            xmldoc = bz2.decompress(binType.data)
               
            domDoc = parseString(xmldoc)
            nodeList = domDoc.getElementsByTagName('replacement')
            replacementNode = nodeList.item(0)
            if len(replacementNode.childNodes):
                thingToWalk = replacementNode.childNodes.item(0)
                tree = self._treeCtrl
                parent = tree.GetItemParent(self._introspectItem)
                treeNode = self.AppendSubTreeFromNode(thingToWalk, thingToWalk.getAttribute('name'), parent, insertBefore=self._introspectItem)
                tree.Delete(self._introspectItem)
        except:
            tp,val,tb = sys.exc_info()
            traceback.print_exception(tp, val, tb)
            
        wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
        
    def OnSize(self, event):
        self._treeCtrl.SetSize(self._p2.GetSize())
        w,h = self._p1.GetClientSizeTuple()
        self._framesListCtrl.SetDimensions(0, 0, w, h)

    def ClearWhileRunning(self):
        list = self._framesListCtrl
        list.DeleteAllItems()
        tree = self._treeCtrl     
        tree.Hide()
        
    def OnListRightClick(self, event):
        if not hasattr(self, "syncFrameID"):
            self.syncFrameID = wx.NewId()
            self.Bind(wx.EVT_MENU, self.OnSyncFrame, id=self.syncFrameID)
        menu = wx.Menu()
        item = wx.MenuItem(menu, self.syncFrameID, "Goto Source Line")
        menu.AppendItem(item)
        self.PopupMenu(menu, event.GetPosition())
        menu.Destroy()

    def OnSyncFrame(self, event):
        list = self._framesListCtrl
        frameNode = self._stack[int(self.currentItem)]
        file = frameNode.getAttribute("file")
        line = frameNode.getAttribute("line")
        self._ui.SynchCurrentLine( file, int(line) )  
        
    def LoadFramesListXML(self, framesXML):
        wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))
        try:
            domDoc = parseString(framesXML)
            list = self._framesListCtrl
            list.DeleteAllItems()
            self._stack = []
            nodeList = domDoc.getElementsByTagName('frame')
            frame_count = -1
            for index in range(0, nodeList.length):
                frameNode = nodeList.item(index)
                message = frameNode.getAttribute("message")
                list.InsertStringItem(index, message)
                self._stack.append(frameNode)
                frame_count += 1
            list.Select(frame_count) 
            self._p1.FitInside()    
            frameNode = nodeList.item(index)
            file = frameNode.getAttribute("file")
            line = frameNode.getAttribute("line")
            self._ui.SynchCurrentLine( file, int(line) )
        except:
            tp,val,tb=sys.exc_info()
            traceback.print_exception(tp, val, tb)
              
        wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))

    def ListItemDeselected(self, event):
        pass
        
    def ListItemSelected(self, event):
        self.currentItem = event.m_itemIndex
        frameNode = self._stack[int(self.currentItem)]
        self.PopulateTreeFromFrameNode(frameNode)
        # Temporarily doing this to test out automatically swicting to source line.
        self.OnSyncFrame(None)
        
    def PopulateTreeFromFrameNode(self, frameNode):
        tree = self._treeCtrl 
        tree.Show(True)
        root = self._root   
        tree.DeleteChildren(root)
        children = frameNode.childNodes
        firstChild = None
        for index in range(0, children.length):
            subNode = children.item(index)
            treeNode = self.AppendSubTreeFromNode(subNode, subNode.getAttribute('name'), root)
            if not firstChild:
                firstChild = treeNode
        tree.Expand(root)
        tree.Expand(firstChild)
        self._p2.FitInside()
            
    def AppendSubTreeFromNode(self, node, name, parent, insertBefore=None):
        tree = self._treeCtrl 
        if insertBefore != None:
            treeNode = tree.InsertItem(parent, insertBefore, name)
        else: 
            treeNode = tree.AppendItem(parent, name)
        children = node.childNodes
        if children.length == 0:
            tree.SetItemText(treeNode, self.StripOuterSingleQuotes(node.getAttribute("value")), 1)
        for index in range(0, children.length):
            subNode = children.item(index)
            if self.HasChildren(subNode):
                self.AppendSubTreeFromNode(subNode, subNode.getAttribute("name"), treeNode)      
            else:
                name = subNode.getAttribute("name")
                value = self.StripOuterSingleQuotes(subNode.getAttribute("value"))
                n = tree.AppendItem(treeNode, name)
                tree.SetItemText(n, value, 1)
        return treeNode
        
    def StripOuterSingleQuotes(self, string):
        if string.startswith("'") and string.endswith("'"):
            return string[1:-1]
        elif type(string) == types.UnicodeType:
            return string[1:-1]
        else:
            return string
            
    def HasChildren(self, node):
        try:
            return node.childNodes.length > 0
        except:
            tp,val,tb=sys.exc_info()
            return False
                    
class DebuggerView(Service.ServiceView):
    
    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, service):
        Service.ServiceView.__init__(self, service)
        
    def _CreateControl(self, parent, id):
        return None
        
    #------------------------------------------------------------------------------
    # Event handling
    #-----------------------------------------------------------------------------
    
    def OnToolClicked(self, event):
        self.GetFrame().ProcessEvent(event)
    
    #------------------------------------------------------------------------------
    # Class methods
    #-----------------------------------------------------------------------------

class Interaction:
    def __init__(self, message, framesXML,  info=None, quit=False):
        self._framesXML = framesXML
        self._message = message
        self._info = info
        self._quit = quit
        
    def getFramesXML(self):
        return self._framesXML
        
    def getMessage(self):
        return self._message
        
    def getInfo(self):
        return self._info
        
    def getQuit(self):
        return self._quit  
         
class AGXMLRPCServer(SimpleXMLRPCServer.SimpleXMLRPCServer):
    def __init__(self, address, logRequests=0):
        SimpleXMLRPCServer.SimpleXMLRPCServer.__init__(self, address, logRequests=logRequests)               
                           
class RequestHandlerThread(threading.Thread):
    def __init__(self, queue, address):
        threading.Thread.__init__(self)
        self._keepGoing = True
        self._queue = queue
        self._address = address
        self._server = AGXMLRPCServer(self._address,logRequests=0)
        self._server.register_function(self.interaction)
        self._server.register_function(self.quit)
        self._server.register_function(self.dummyOperation)
        if _VERBOSE: print "RequestHandlerThread on fileno %s" % str(self._server.fileno())
        
    def run(self):
        while self._keepGoing:
            try:
                self._server.handle_request() 
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)
                self._keepGoing = False
        if _VERBOSE: print "Exiting Request Handler Thread."
 
    def interaction(self, message, frameXML, info):
        if _VERBOSE: print "In RequestHandlerThread.interaction -- adding to queue"
        interaction = Interaction(message, frameXML, info)
        self._queue.put(interaction)
        return ""
        
    def quit(self):
        interaction = Interaction(None, None, info=None, quit=True)
        self._queue.put(interaction)
        return ""
    
    def dummyOperation(self):
        return ""
        
    def AskToStop(self):
        self._keepGoing = False
        if type(self._server) is not types.NoneType:
            try:
                # This is a really ugly way to make sure this thread isn't blocked in
                # handle_request.
                url = 'http://' + self._address[0] + ':' + str(self._address[1]) + '/'
                tempServer = xmlrpclib.ServerProxy(url, allow_none=1)
                tempServer.dummyOperation()
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)
            self._server.server_close()


class RequestBreakThread(threading.Thread):
        def __init__(self, server, interrupt=False, pushBreakpoints=False, breakDict=None, kill=False):
            threading.Thread.__init__(self)
            self._server = server

            self._interrupt = interrupt
            self._pushBreakpoints = pushBreakpoints
            self._breakDict = breakDict
            self._kill = kill
            
        def run(self):
            try:
                if _VERBOSE: print "RequestBreakThread, before call"
                if self._interrupt:
                    self._server.break_requested()
                if self._pushBreakpoints:
                    self._server.update_breakpoints(xmlrpclib.Binary(pickle.dumps(self._breakDict)))
                if self._kill:
                    try:
                        self._server.die()
                    except:
                        pass
                if _VERBOSE: print "RequestBreakThread, after call"
            except:
                tp,val,tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)

class DebuggerOperationThread(threading.Thread):
        def __init__(self, function):
            threading.Thread.__init__(self)
            self._function = function
            
        def run(self):
            if _VERBOSE: print "In DOT, before call"
            try:
                self._function()
            except:
                tp,val,tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)
            if _VERBOSE: print "In DOT, after call"
                
class DebuggerCallback:

    def __init__(self, host, port, debugger_url, break_url, debuggerUI):
        if _VERBOSE: print "+++++++ Creating server on port, ", str(port)
        
        self._queue = Queue.Queue(50)
        self._host = host
        self._port = int(port)
        threading._VERBOSE = _VERBOSE
        self._serverHandlerThread = RequestHandlerThread(self._queue, (self._host, self._port))
        
        self._debugger_url = debugger_url
        self._debuggerServer = None
        self._waiting = False
        self._service = wx.GetApp().GetService(DebuggerService)
        self._debuggerUI = debuggerUI
        self._break_url = break_url
        self._breakServer = None
        self._firstInteraction = True
        self._pendingBreak = False
        
    def start(self):
        self._serverHandlerThread.start()
    
    def ServerClose(self):
        rbt = RequestBreakThread(self._breakServer, kill=True)
        rbt.start()
        self.setWaiting(False)
        if self._serverHandlerThread:
            self._serverHandlerThread.AskToStop()
            self._serverHandlerThread = None
                            
    def BreakExecution(self):
        rbt = RequestBreakThread(self._breakServer, interrupt=True)
        rbt.start()
                
    def SingleStep(self):
        self._debuggerUI.DisableWhileDebuggerRunning()
        #dot = DebuggerOperationThread(self._debuggerServer.set_step)
        #dot.start()
        self._debuggerServer.set_step() # Figure out where to set allowNone
        self.waitForRPC()

    def Next(self):
        self._debuggerUI.DisableWhileDebuggerRunning()
        #dot = DebuggerOperationThread(self._debuggerServer.set_next)
        #dot.start()
        self._debuggerServer.set_next()
        self.waitForRPC()
        
    def Continue(self):
        self._debuggerUI.DisableWhileDebuggerRunning()
        #dot = DebuggerOperationThread(self._debuggerServer.set_continue)
        #dot.start()
        self._debuggerServer.set_continue()
        self.waitForRPC()
        
    def Return(self):
        self._debuggerUI.DisableWhileDebuggerRunning()
        #dot = DebuggerOperationThread(self._debuggerServer.set_return)
        #dot.start()
        self._debuggerServer.set_return()
        self.waitForRPC()
        
    def setWaiting(self, value):
        self._waiting = value
        
    def getWaiting(self):
        return self._waiting
            
    def readQueue(self):
        if self._queue.qsize():
            try:
                item = self._queue.get_nowait()
                if item.getQuit():
                    self.interaction(None, None, None, True)
                else:
                    data = bz2.decompress(item.getFramesXML().data)
                    self.interaction(item.getMessage().data, data, item.getInfo(), False)               
            except Queue.Empty:
                pass
                
    def pushBreakpoints(self):
        rbt = RequestBreakThread(self._breakServer, pushBreakpoints=True, breakDict=self._service.GetMasterBreakpointDict())
        rbt.start()

                               
    def waitForRPC(self):
        self.setWaiting(True)
        while self.getWaiting():
            try:
                self.readQueue()
                import time
                time.sleep(0.02)
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp, val, tb)
            wx.GetApp().Yield(True)
        if _VERBOSE: print "Exiting waitForRPC."
        
    def interaction(self, message, frameXML, info, quit):
        
        #This method should be hit as the debugger starts.
        if self._firstInteraction:
            self._firstInteraction = False
            self._debuggerServer = xmlrpclib.ServerProxy(self._debugger_url,  allow_none=1)
            self._breakServer = xmlrpclib.ServerProxy(self._break_url, allow_none=1)
            self.pushBreakpoints()
        self.setWaiting(False)
        if _VERBOSE: print "+"*40
        if(quit):
            self._debuggerUI.StopExecution(None)
            return ""
        if(info != ""):
            if _VERBOSE: print "Hit interaction with exception"
            #self._debuggerUI.StopExecution(None)
            #self._debuggerUI.SetStatusText("Got exception: " + str(info))
            self._debuggerUI.SwitchToOutputTab()
        else:
            if _VERBOSE: print "Hit interaction no exception"
        self._debuggerUI.SetStatusText(message)
        self._debuggerUI.LoadFramesListXML(frameXML)        
        self._debuggerUI.EnableWhileDebuggerStopped()
        if _VERBOSE: print "+"*40
        
class DebuggerService(Service.Service):

    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    TOGGLE_BREAKPOINT_ID = wx.NewId()
    CLEAR_ALL_BREAKPOINTS = wx.NewId()
    RUN_ID = wx.NewId()
    DEBUG_ID = wx.NewId()

    def ComparePaths(first, second):
        one = DebuggerService.ExpandPath(first)
        two = DebuggerService.ExpandPath(second)
        if _WINDOWS:
            return one.lower() == two.lower()
        else:
            return one == two
    ComparePaths = staticmethod(ComparePaths)
            
    # Make sure we're using an expanded path on windows.
    def ExpandPath(path):
        if _WINDOWS:
            try:
                return win32api.GetLongPathName(path)
            except:
                print "Cannot get long path for %s" % path
                
        return path
        
    ExpandPath = staticmethod(ExpandPath)
 
    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, serviceName, embeddedWindowLocation = wx.lib.pydocview.EMBEDDED_WINDOW_LEFT):
        Service.Service.__init__(self, serviceName, embeddedWindowLocation)
        self.BREAKPOINT_DICT_STRING = "MasterBreakpointDict"
        config = wx.ConfigBase_Get()
        pickledbps = config.Read(self.BREAKPOINT_DICT_STRING)
        if pickledbps:
            try:
                self._masterBPDict = pickle.loads(pickledbps.encode('ascii'))
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp,val,tb)
                self._masterBPDict = {}
        else:
            self._masterBPDict = {}
            
    def OnCloseFrame(self, event):
        # IS THIS THE RIGHT PLACE?
        try:
            config = wx.ConfigBase_Get()
            config.Write(self.BREAKPOINT_DICT_STRING, pickle.dumps(self._masterBPDict))
        except:
            tp,val,tb = sys.exc_info()
            traceback.print_exception(tp, val, tb)
        return True
        
    def _CreateView(self):
        return DebuggerView(self)


    #----------------------------------------------------------------------------
    # Service specific methods
    #----------------------------------------------------------------------------

    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        #Service.Service.InstallControls(self, frame, menuBar, toolBar, statusBar, document)

        config = wx.ConfigBase_Get()

        debuggerMenu = wx.Menu()
        if not menuBar.FindItemById(DebuggerService.CLEAR_ALL_BREAKPOINTS):
                    
            debuggerMenu.Append(DebuggerService.RUN_ID, _("&Run...\tCtrl+R"), _("Runs a file"))
            wx.EVT_MENU(frame, DebuggerService.RUN_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, DebuggerService.RUN_ID, frame.ProcessUpdateUIEvent)
            
            debuggerMenu.Append(DebuggerService.DEBUG_ID, _("&Debug...\tCtrl+D"), _("Debugs a file"))
            wx.EVT_MENU(frame, DebuggerService.DEBUG_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, DebuggerService.DEBUG_ID, frame.ProcessUpdateUIEvent)
            
            debuggerMenu.AppendSeparator()
            
            debuggerMenu.Append(DebuggerService.TOGGLE_BREAKPOINT_ID, _("&Toggle Breakpoint...\tCtrl+B"), _("Toggle a breakpoint"))
            wx.EVT_MENU(frame, DebuggerService.TOGGLE_BREAKPOINT_ID, self.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, DebuggerService.TOGGLE_BREAKPOINT_ID, self.ProcessUpdateUIEvent)

            debuggerMenu.Append(DebuggerService.CLEAR_ALL_BREAKPOINTS, _("&Clear All Breakpoints"), _("Clear All Breakpoints"))
            wx.EVT_MENU(frame, DebuggerService.CLEAR_ALL_BREAKPOINTS, self.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, DebuggerService.CLEAR_ALL_BREAKPOINTS, self.ProcessUpdateUIEvent)
            
            
        viewMenuIndex = menuBar.FindMenu(_("&Project"))
        menuBar.Insert(viewMenuIndex + 1, debuggerMenu, _("&Run"))

        return True


            
    #----------------------------------------------------------------------------
    # Event Processing Methods
    #----------------------------------------------------------------------------

    def ProcessEventBeforeWindows(self, event):
        return False


    def ProcessEvent(self, event):
        if Service.Service.ProcessEvent(self, event):
            return True

        an_id = event.GetId()
        if an_id == DebuggerService.TOGGLE_BREAKPOINT_ID:
            self.OnToggleBreakpoint(event)
            return True
        elif an_id == DebuggerService.CLEAR_ALL_BREAKPOINTS:
            self.ClearAllBreakpoints()
            return True
        elif an_id == DebuggerService.RUN_ID:
            self.OnRunProject(event)
            return True
        elif an_id == DebuggerService.DEBUG_ID:
            self.OnDebugProject(event)
            return True
        return False
        
    def ProcessUpdateUIEvent(self, event):
        if Service.Service.ProcessUpdateUIEvent(self, event):
            return True

        an_id = event.GetId()
        if an_id == DebuggerService.TOGGLE_BREAKPOINT_ID:
            currentView = self.GetDocumentManager().GetCurrentView()
            event.Enable(isinstance(currentView, PythonEditor.PythonView))
            return True
        elif an_id == DebuggerService.CLEAR_ALL_BREAKPOINTS:
            event.Enable(self.HasBreakpointsSet())
            return True
        elif an_id == DebuggerService.RUN_ID:
            event.Enable(self.HasAnyFiles())
            return True
        elif an_id == DebuggerService.DEBUG_ID:
            event.Enable(self.HasAnyFiles())
            return True
        else:
            return False

    #----------------------------------------------------------------------------
    # Class Methods
    #----------------------------------------------------------------------------                    
   
    def OnDebugProject(self, event):
        if not Executor.GetPythonExecutablePath():
            return
        if DebugCommandUI.DebuggerRunning():
            wx.MessageBox(_("A debugger is already running. Please shut down the other debugger first."), _("Debugger Running"))
            return
        self.ShowWindow(True)
        projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
        project = projectService.GetView().GetDocument()
        dlg = CommandPropertiesDialog(self.GetView().GetFrame(), 'Debug Python File', projectService, None, pythonOnly=True, okButtonName="Debug", debugging=True)
        if dlg.ShowModal() == wx.ID_OK:
            fileToDebug, initialArgs, startIn, isPython, environment = dlg.GetSettings()
            dlg.Destroy()
        else:
            dlg.Destroy()
            return
        self.PromptToSaveFiles()

        shortFile = os.path.basename(fileToDebug)
        fileToDebug = DebuggerService.ExpandPath(fileToDebug)
        try:
            page = DebugCommandUI(Service.ServiceView.bottomTab, -1, str(fileToDebug), self)
            count = Service.ServiceView.bottomTab.GetPageCount()
            Service.ServiceView.bottomTab.AddPage(page, "Debugging: " + shortFile)
            Service.ServiceView.bottomTab.SetSelection(count)
            page.Execute(initialArgs, startIn, environment)
        except:
            pass
    
    def HasAnyFiles(self):
        docs = wx.GetApp().GetDocumentManager().GetDocuments()
        return len(docs) > 0
        
    def PromptToSaveFiles(self, running=True):
        filesModified = False
        docs = wx.GetApp().GetDocumentManager().GetDocuments()
        for doc in docs:
            if doc.IsModified():
                filesModified = True
                break
        if filesModified:
            frame = self.GetView().GetFrame()  
            if running:              
                yesNoMsg = wx.MessageDialog(frame,
                          _("Files have been modified.\nWould you like to save all files before running?"),
                          _("Run"),
                          wx.YES_NO
                          )
            else:              
                yesNoMsg = wx.MessageDialog(frame,
                          _("Files have been modified.\nWould you like to save all files before debugging?"),
                          _("Debug"),
                          wx.YES_NO
                          )
            if yesNoMsg.ShowModal() == wx.ID_YES:
                docs = wx.GetApp().GetDocumentManager().GetDocuments()
                for doc in docs:
                    doc.Save()

    def OnExit(self):
        DebugCommandUI.ShutdownAllDebuggers()
        
    def OnRunProject(self, event):
        if not Executor.GetPythonExecutablePath():
            return
        projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
        project = projectService.GetView().GetDocument()
        dlg = CommandPropertiesDialog(self.GetView().GetFrame(), 'Run', projectService, None)
        if dlg.ShowModal() == wx.ID_OK:
            fileToRun, initialArgs, startIn, isPython, environment = dlg.GetSettings()
            

            dlg.Destroy()
        else:
            dlg.Destroy()
            return
        self.PromptToSaveFiles()
        # This will need to change when we can run more than .py and .bpel files.
        if not isPython:
            projectService.RunProcessModel(fileToRun)
            return
            
        self.ShowWindow(True)
        shortFile = os.path.basename(fileToRun)
        page = RunCommandUI(Service.ServiceView.bottomTab, -1, str(fileToRun))
        count = Service.ServiceView.bottomTab.GetPageCount()
        Service.ServiceView.bottomTab.AddPage(page, "Running: " + shortFile)
        Service.ServiceView.bottomTab.SetSelection(count)
        page.Execute(initialArgs, startIn, environment)

    def OnToggleBreakpoint(self, event, line=-1, fileName=None):
        if not fileName:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            # Test to make sure we aren't the project view.
            if not hasattr(view, 'MarkerExists'):
                return
            fileName = wx.GetApp().GetDocumentManager().GetCurrentDocument().GetFilename()
            if line < 0:
                line = view.GetCtrl().GetCurrentLine()
        if  self.BreakpointSet(fileName, line + 1):
            self.ClearBreak(fileName, line + 1)
        else:
            self.SetBreak(fileName, line + 1)
        # Now refresh all the markers icons in all the open views.
        self.ClearAllBreakpointMarkers()
        self.SetAllBreakpointMarkers()    
                 
    def SilentToggleBreakpoint(self, fileName, line):
        found = False
        for lineNumber in self.GetBreakpointList(fileName):
            if int(lineNumber) == int(line):
                found = True
                break
        if found:
            self.SetBreak(fileName, line)
        else:
            self.ClearBreak(fileName, line)
                  
    def SetBreak(self, fileName, line):
        expandedName = DebuggerService.ExpandPath(fileName)
        if not self._masterBPDict.has_key(expandedName):
            self._masterBPDict[expandedName] = [line]
        else:
            self._masterBPDict[expandedName] += [line]
        # If we're already debugging, pass this bp off to the DebuggerCallback
        self.NotifyDebuggersOfBreakpointChange()
    
    def NotifyDebuggersOfBreakpointChange(self):
        DebugCommandUI.NotifyDebuggersOfBreakpointChange()
        
    def GetBreakpointList(self, fileName):
        expandedName = DebuggerService.ExpandPath(fileName)
        if not self._masterBPDict.has_key(expandedName):
            return []
        else:
            return self._masterBPDict[expandedName] 

    def BreakpointSet(self, fileName, line):
        expandedName = DebuggerService.ExpandPath(fileName)
        if not self._masterBPDict.has_key(expandedName):
            return False
        else:
            newList = []
            for number in self._masterBPDict[expandedName]:
                if(int(number) == int(line)):
                    return True
        return False
        
    def ClearBreak(self, fileName, line):
        expandedName = DebuggerService.ExpandPath(fileName)
        if not self._masterBPDict.has_key(expandedName):
            print "In ClearBreak: no key"
            return
        else:
            newList = []
            for number in self._masterBPDict[expandedName]:
                if(int(number) != int(line)):
                    newList.append(number)
            self._masterBPDict[expandedName] = newList
        self.NotifyDebuggersOfBreakpointChange()
       
    def HasBreakpointsSet(self):
        for key, value in self._masterBPDict.items():
            if len(value) > 0:
                return True
        return False
                 
    def ClearAllBreakpoints(self):
        self._masterBPDict = {}
        self.NotifyDebuggersOfBreakpointChange()
        self.ClearAllBreakpointMarkers()
        
    def ClearAllBreakpointMarkers(self):
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:  
            if(isinstance(openDoc.GetFirstView(), CodeEditor.CodeView)): 
                openDoc.GetFirstView().MarkerDeleteAll(CodeEditor.CodeCtrl.BREAKPOINT_MARKER_NUM) 

    def GetMasterBreakpointDict(self):
        return self._masterBPDict
        
    def SetAllBreakpointMarkers(self):
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:  
            if(isinstance(openDoc.GetFirstView(), CodeEditor.CodeView)): 
                self.SetCurrentBreakpointMarkers(openDoc.GetFirstView())
            
    def SetCurrentBreakpointMarkers(self, view):
        if isinstance(view, CodeEditor.CodeView) and hasattr(view, 'GetDocument'):
            view.MarkerDeleteAll(CodeEditor.CodeCtrl.BREAKPOINT_MARKER_NUM)
            for linenum in self.GetBreakpointList(view.GetDocument().GetFilename()): 
                view.MarkerAdd(lineNum=int(linenum) - 1, marker_index=CodeEditor.CodeCtrl.BREAKPOINT_MARKER_NUM)

class DebuggerOptionsPanel(wx.Panel):


    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)
        SPACE = 10
        config = wx.ConfigBase_Get()
        localHostStaticText = wx.StaticText(self, -1, _("Local Host Name:"))
        self._LocalHostTextCtrl = wx.TextCtrl(self, -1, config.Read("DebuggerHostName", DEFAULT_HOST), size = (150, -1))
        portNumberStaticText = wx.StaticText(self, -1, _("Port Range:"))
        dashStaticText = wx.StaticText(self, -1, _("through to")) 
        startingPort=config.ReadInt("DebuggerStartingPort", DEFAULT_PORT)
        self._PortNumberTextCtrl = wx.lib.intctrl.IntCtrl(self, -1, startingPort, size = (50, -1))
        self._PortNumberTextCtrl.SetMin(1)#What are real values?
        self._PortNumberTextCtrl.SetMax(65514) #What are real values?
        self.Bind(wx.lib.intctrl.EVT_INT, self.MinPortChange, self._PortNumberTextCtrl)

        self._EndPortNumberTextCtrl = wx.lib.intctrl.IntCtrl(self, -1, startingPort + PORT_COUNT, size = (50, -1))
        self._EndPortNumberTextCtrl.SetMin(22)#What are real values?
        self._EndPortNumberTextCtrl.SetMax(65535)#What are real values?
        self._EndPortNumberTextCtrl.Enable( False )
        debuggerPanelBorderSizer = wx.BoxSizer(wx.VERTICAL)
        debuggerPanelSizer = wx.GridBagSizer(hgap = 5, vgap = 5)
        debuggerPanelSizer.Add( localHostStaticText, (0,0), flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        debuggerPanelSizer.Add( self._LocalHostTextCtrl, (0,1), (1,3), flag=wx.EXPAND|wx.ALIGN_CENTER)
        debuggerPanelSizer.Add( portNumberStaticText, (1,0), flag=wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL)
        debuggerPanelSizer.Add( self._PortNumberTextCtrl, (1,1), flag=wx.ALIGN_CENTER)
        debuggerPanelSizer.Add( dashStaticText, (1,2), flag=wx.ALIGN_CENTER)
        debuggerPanelSizer.Add( self._EndPortNumberTextCtrl, (1,3), flag=wx.ALIGN_CENTER)
        FLUSH_PORTS_ID = wx.NewId()
        self._flushPortsButton = wx.Button(self, FLUSH_PORTS_ID, "Reset Port List")
        wx.EVT_BUTTON(parent, FLUSH_PORTS_ID, self.FlushPorts)
        debuggerPanelSizer.Add(self._flushPortsButton, (2,2), (1,2), flag=wx.ALIGN_RIGHT)
        
        debuggerPanelBorderSizer.Add(debuggerPanelSizer, 0, wx.ALL, SPACE)
        self.SetSizer(debuggerPanelBorderSizer)
        self.Layout()
        parent.AddPage(self, _("Debugger"))
        
    def FlushPorts(self, event):
        if self._PortNumberTextCtrl.IsInBounds():
            config = wx.ConfigBase_Get()
            config.WriteInt("DebuggerStartingPort", self._PortNumberTextCtrl.GetValue())
            DebugCommandUI.NewPortRange()
        else:
            wx.MessageBox(_("The starting port is not valid. Please change the value and try again.", "Invalid Starting Port Number"))
                          
    def MinPortChange(self, event):
        self._EndPortNumberTextCtrl.Enable( True )
        self._EndPortNumberTextCtrl.SetValue( self._PortNumberTextCtrl.GetValue() + PORT_COUNT)
        self._EndPortNumberTextCtrl.Enable( False )
        
    def OnOK(self, optionsDialog):
        config = wx.ConfigBase_Get()
        config.Write("DebuggerHostName", self._LocalHostTextCtrl.GetValue())
        if self._PortNumberTextCtrl.IsInBounds():
            config.WriteInt("DebuggerStartingPort", self._PortNumberTextCtrl.GetValue())

class CommandPropertiesDialog(wx.Dialog):
    
    def __init__(self, parent, title, projectService, currentProjectDocument, pythonOnly=False, okButtonName="Run", debugging=False):
        if _WINDOWS:
            wx.Dialog.__init__(self, parent, -1, title)
        else:
            wx.Dialog.__init__(self, parent, -1, title, size=(390,270)) 
        self._projService = projectService
        self._pmext = None
        self._pyext = None
        for template in self._projService.GetDocumentManager().GetTemplates():
            if not ACTIVEGRID_BASE_IDE and template.GetDocumentType() == ProcessModelEditor.ProcessModelDocument:
                self._pmext = template.GetDefaultExtension()
            if template.GetDocumentType() == PythonEditor.PythonDocument:
                self._pyext = template.GetDefaultExtension()
        self._pythonOnly = pythonOnly
           
        self._currentProj = currentProjectDocument
        projStaticText = wx.StaticText(self, -1, _("Project:")) 
        fileStaticText = wx.StaticText(self, -1, _("File:")) 
        argsStaticText = wx.StaticText(self, -1, _("Arguments:")) 
        startInStaticText = wx.StaticText(self, -1, _("Start in:"))
        pythonPathStaticText = wx.StaticText(self, -1, _("PYTHONPATH:"))
        postpendStaticText = _("Postpend win32api path")
        cpPanelBorderSizer = wx.BoxSizer(wx.VERTICAL)
        self._projectNameList, self._projectDocumentList, selectedIndex = self.GetProjectList()
        self._projList = wx.Choice(self, -1, (200,-1), choices=self._projectNameList)
        self.Bind(wx.EVT_CHOICE, self.EvtListBox, self._projList)
        HALF_SPACE = 5
        flexGridSizer = wx.FlexGridSizer(cols = 3, vgap = 10, hgap = 10)
        
        flexGridSizer.Add(projStaticText, 0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        flexGridSizer.Add(self._projList, 1, flag=wx.EXPAND)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        
        flexGridSizer.Add(fileStaticText, 0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        self._fileList = wx.Choice(self, -1, (200,-1))
        self.Bind(wx.EVT_CHOICE, self.OnFileSelected, self._fileList)
        flexGridSizer.Add(self._fileList, 1, flag=wx.EXPAND)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        
        config = wx.ConfigBase_Get()
        self._lastArguments = config.Read("LastRunArguments")
        self._argsEntry = wx.TextCtrl(self, -1, str(self._lastArguments))
        self._argsEntry.SetToolTipString(str(self._lastArguments))
        def TextChanged(event):
            self._argsEntry.SetToolTipString(event.GetString())
        self.Bind(wx.EVT_TEXT, TextChanged, self._argsEntry)

        flexGridSizer.Add(argsStaticText, 0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        flexGridSizer.Add(self._argsEntry, 1, flag=wx.EXPAND)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        
        flexGridSizer.Add(startInStaticText, 0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        self._lastStartIn = config.Read("LastRunStartIn")
        if not self._lastStartIn:
            self._lastStartIn = str(os.getcwd())
        self._startEntry = wx.TextCtrl(self, -1, self._lastStartIn)
        self._startEntry.SetToolTipString(self._lastStartIn)
        def TextChanged2(event):
            self._startEntry.SetToolTipString(event.GetString())
        self.Bind(wx.EVT_TEXT, TextChanged2, self._startEntry)

        flexGridSizer.Add(self._startEntry, 1, wx.EXPAND)
        self._findDir = wx.Button(self, -1, _("Browse..."), size=(60,-1))
        self.Bind(wx.EVT_BUTTON, self.OnFindDirClick, self._findDir)
        flexGridSizer.Add(self._findDir, 0, wx.RIGHT, 10)
        
        flexGridSizer.Add(pythonPathStaticText, 0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        if os.environ.has_key('PYTHONPATH'):
            startval = os.environ['PYTHONPATH']
        else:
            startval = ""
        self._lastPythonPath = config.Read("LastPythonPath", startval)
        self._pythonPathEntry = wx.TextCtrl(self, -1, self._lastPythonPath)
        self._pythonPathEntry.SetToolTipString(self._lastPythonPath)
        flexGridSizer.Add(self._pythonPathEntry, 1, wx.EXPAND)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        if debugging:
            self._postpendCheckBox = wx.CheckBox(self, -1, postpendStaticText)
            checked = bool(config.ReadInt("PythonPathPostpend", 1))
            self._postpendCheckBox.SetValue(checked)
            flexGridSizer.Add(self._postpendCheckBox, 1, wx.EXPAND)
        flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
        cpPanelBorderSizer.Add(flexGridSizer, 0, wx.ALL, 10)
        
        box = wx.BoxSizer(wx.HORIZONTAL)
        self._okButton = wx.Button(self, wx.ID_OK, okButtonName, size=(75,-1))
        self._okButton.SetDefault()
        self._okButton.SetHelpText(_("The ") + okButtonName + _(" button completes the dialog"))
        box.Add(self._okButton, 0, wx.ALIGN_RIGHT|wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnOKClick, self._okButton)
        btn = wx.Button(self, wx.ID_CANCEL, _("Cancel"), size=(75,-1))
        btn.SetHelpText(_("The Cancel button cancels the dialog."))
        box.Add(btn, 0, wx.ALIGN_RIGHT|wx.ALL, 5)
        cpPanelBorderSizer.Add(box, 0, wx.ALIGN_RIGHT|wx.BOTTOM, 5)
        
        self.SetSizer(cpPanelBorderSizer)
        if _WINDOWS:
            self.GetSizer().Fit(self)
        
        self.Layout()
        
        # Set up selections based on last values used.
        self._fileNameList = None
        self._selectedFileIndex = 0
        lastProject = config.Read("LastRunProject")
        lastFile = config.Read("LastRunFile")

        if lastProject in self._projectNameList:
            selectedIndex = self._projectNameList.index(lastProject)
        elif selectedIndex < 0:
            selectedIndex = 0
        self._projList.Select(selectedIndex)
        self._selectedProjectIndex = selectedIndex
        self._selectedProjectDocument = self._projectDocumentList[selectedIndex]
        self.PopulateFileList(self._selectedProjectDocument, lastFile)
            
    def OnOKClick(self, event):
        startIn = self._startEntry.GetValue()
        fileToRun = self._fileList.GetStringSelection()
        if not fileToRun:
            wx.MessageBox(_("You must select a file to proceed. Note that not all projects have files that can be run or debugged."))
            return
        isPython = fileToRun.endswith(self._pyext)
        if isPython and not os.path.exists(startIn):
            wx.MessageBox(_("Starting directory does not exist. Please change this value."))
            return
        config = wx.ConfigBase_Get()
        config.Write("LastRunProject", self._projectNameList[self._selectedProjectIndex])
        config.Write("LastRunFile", fileToRun)
        # Don't update the arguments or starting directory unless we're runing python.
        if isPython:
            config.Write("LastRunArguments", self._argsEntry.GetValue())
            config.Write("LastRunStartIn", self._startEntry.GetValue())
            config.Write("LastPythonPath",self._pythonPathEntry.GetValue())
            if hasattr(self, "_postpendCheckBox"):
                config.WriteInt("PythonPathPostpend", int(self._postpendCheckBox.GetValue()))

        self.EndModal(wx.ID_OK)
            
    def GetSettings(self):  
        filename = self._fileNameList[self._selectedFileIndex]
        args = self._argsEntry.GetValue() 
        startIn = self._startEntry.GetValue()
        isPython = filename.endswith(self._pyext)
        env = os.environ
        if hasattr(self, "_postpendCheckBox"):
            postpend = self._postpendCheckBox.GetValue()
        else:
            postpend = False
        if postpend:
            env['PYTHONPATH'] = self._pythonPathEntry.GetValue() + os.pathsep + os.path.join(os.getcwd(), "3rdparty", "pywin32")
        else:
            env['PYTHONPATH'] = self._pythonPathEntry.GetValue()
            
        return filename, args, startIn, isPython, env
                      
    def OnFileSelected(self, event):
        self._selectedFileIndex = self._fileList.GetSelection()
        self.EnableForFileType(event.GetString())
        
    def EnableForFileType(self, fileName):
        show = fileName.endswith(self._pyext)
        self._startEntry.Enable(show)
        self._findDir.Enable(show)
        self._argsEntry.Enable(show)

        if not show:
            self._lastStartIn = self._startEntry.GetValue()
            self._startEntry.SetValue("")
            self._lastArguments = self._argsEntry.GetValue()
            self._argsEntry.SetValue("")  
        else:
            self._startEntry.SetValue(self._lastStartIn)
            self._argsEntry.SetValue(self._lastArguments)  
                
            

    def OnFindDirClick(self, event):
        dlg = wx.DirDialog(self, "Choose a starting directory:", self._startEntry.GetValue(),
                          style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)

        if dlg.ShowModal() == wx.ID_OK:
            self._startEntry.SetValue(dlg.GetPath())
            
        dlg.Destroy() 
           
    def EvtListBox(self, event):
        if event.GetString():
            index = self._projectNameList.index(event.GetString())
            self._selectedProjectDocument = self._projectDocumentList[index]
            self._selectedProjectIndex = index
            self.PopulateFileList(self._selectedProjectDocument)
            
    def FilterFileList(self, list):
        if self._pythonOnly:
            files = filter(lambda f: f.endswith(self._pyext), list)
        else:
            files = filter(lambda f: (self._pmext and f.endswith(self._pmext)) or f.endswith(self._pyext), list)
        return files
                 
    def PopulateFileList(self, project, shortNameToSelect=None):
        self._fileNameList = self.FilterFileList(project.GetFiles()[:])
        self._fileList.Clear()
        if not self._fileNameList:
            return
        self._fileNameList.sort(lambda a, b: cmp(os.path.basename(a).lower(), os.path.basename(b).lower()))
        strings = map(lambda file: os.path.basename(file), self._fileNameList)
        for index in range(0, len(strings)):
            if shortNameToSelect == strings[index]:
                self._selectedFileIndex = index 
                break 
        self._fileList.Hide()
        self._fileList.AppendItems(strings)
        self._fileList.Show()
        if self._selectedFileIndex not in range(0, len(strings)) : self._selectedFileIndex = 0
        self._fileList.SetSelection(self._selectedFileIndex)
        self.EnableForFileType(strings[self._selectedFileIndex])
        
    def GetProjectList(self):
        docList = []
        nameList = []
        found = False
        index = -1
        count = 0
        for document in self._projService.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectEditor.ProjectDocument and len(document.GetFiles()):
                docList.append(document)
                nameList.append(os.path.basename(document.GetFilename()))
                if document == self._currentProj:
                    found = True
                    index = count
                count += 1
        return nameList, docList, index
#----------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO
#----------------------------------------------------------------------
def getBreakData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x11\x08\x06\
\x00\x00\x00\xd4\xaf,\xc4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x97IDAT8\x8d\xbdSA\x0e\xc3 \x0c\x8b\x13\xfe=\x1e^\xe2\x1dF\xbb\x8c\
\xd2\x0c\xa9\xda,q\x88\x05\x8e\x1d\x00P\x93;\xd0[\xa7W\x04\xe8\x8d\x0f\xdfxU\
c%\x02\xbd\xbd\x05HQ+Xv\xb0\xa3VN\xf9\xd4\x01\xbd\x11j\x18\x1d\x00\x10\xa8AD\
\xa4\xa4\xd6_\x9b\x19\xbb\x03\xd8c|\x8f\x00\xe0\x93\xa8g>\x15 C\xee:\xe7\x8f\
\x08\xdesj\xcf\xe6\xde(\xddn\xec\x18f0w\xe0m;\x8d\x9b\xe4\xb1\xd4\n\xa6\x0e2\
\xc4{\x1f\xeb\xdf?\xe5\xff\t\xa8\x1a$\x0cg\xac\xaf\xb0\xf4\x992<\x01\xec\xa0\
U9V\xf9\x18\xc8\x00\x00\x00\x00IEND\xaeB`\x82' 

def getBreakBitmap():
    return BitmapFromImage(getBreakImage())

def getBreakImage():
    stream = cStringIO.StringIO(getBreakData())
    return ImageFromStream(stream)

def getBreakIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getBreakBitmap())
    return icon

#----------------------------------------------------------------------
def getClearOutputData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\xb4IDAT8\x8d\xa5\x92\xdd\r\x03!\x0c\x83\xbf\xa0n\xd4\x9d\xda5\xb81\
\xbaS\xbb\x12\xee\x03?\xe5\x08\xe5N\xba\xbc Db\xec\xd8p\xb1l\xb8\xa7\x83\xfe\
\xb0\x02H\x92F\xc0_\xa3\x99$\x99\x99\xedznc\xe36\x81\x88\x98"\xb2\x02\xa2\
\x1e\xc4Q\x9aUD\x161\xcd\xde\x1c\x83\x15\x084)\x8d\xc5)\x06\xab\xaaZ\x92\xee\
\xce\x11W\xdbGD\x0cIT\x06\xe7\x00\xdeY\xfe\xcc\x89\x06\xf0\xf2\x99\x00\xe0\
\x91\x7f\xab\x83\xed\xa4\xc8\xafK\x0c\xcf\x92\x83\x99\x8d\xe3p\xef\xe4\xa1\
\x0b\xe57j\xc8:\x06\t\x08\x87.H\xb2n\xa8\xc9\xa9\x12vQ\xfeG"\xe3\xacw\x00\
\x10$M\xd3\x86_\xf0\xe5\xfc\xb4\xfa\x02\xcb\x13j\x10\xc5\xd7\x92D\x00\x00\
\x00\x00IEND\xaeB`\x82' 

def getClearOutputBitmap():
    return BitmapFromImage(getClearOutputImage())

def getClearOutputImage():
    stream = cStringIO.StringIO(getClearOutputData())
    return ImageFromStream(stream)

def getClearOutputIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getClearOutputBitmap())
    return icon

#----------------------------------------------------------------------
def getCloseData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x12\x00\x00\x00\x12\x08\x06\
\x00\x00\x00V\xce\x8eW\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x00\x86IDAT8\x8d\xed\x90\xb1\r\x80 \x10E_"c\xd80\x02-\x138\x87;8\x8f\x8d\
\x8b\xb0\x02\xa5\xad\rS\x88\xcd\x11) \x82\xb6\xbe\xea\xf2\xc9\xbd\xfc\x03~\
\xdeb\x81\xb9\x90\xabJ^%\x00N\x849\x0e\xf0\x85\xbc\x8a\x12YZR2\xc7\x1eIB\xcb\
\xb2\xcb\x9at\x9d\x95c\xa5Y|\x92\x0c\x0f\xa2\r8e\x1e\x81\x1d8z\xdb8\xee?Ig\
\xfa\xb7\x92)\xcb\xacd\x01XZ$QD\xba\xf0\xa6\x80\xd5\x18cZ\x1b\x95$?\x1f\xb9\
\x00\x1d\x94\x1e*e_\x8a~\x00\x00\x00\x00IEND\xaeB`\x82' 

def getCloseBitmap():
    return BitmapFromImage(getCloseImage())

def getCloseImage():
    stream = cStringIO.StringIO(getCloseData())
    return ImageFromStream(stream)

def getCloseIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getCloseBitmap())
    return icon

#----------------------------------------------------------------------
def getContinueData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x9eIDAT8\x8d\xbd\x92\xd1\r\x83@\x0cC\xed\x8a\x11X\xac\x1b\xddF,\xd6\
\x11\x90\xdc\x0f\x9a\x93s)\x14Z\xa9\x91\x10\n\x97\xbc\xd89\x80?\x84\x1a\xa4\
\x83\xfc\x1c$\x1e)7\xdf<Y0\xaf\x0b\xe6\xf5\x1d\xa1\xb5\x13C\x03 !\xaa\xfd\
\xed\n:mr\xc0\x1d\x8f\xc9\x9a!\t$\xe5\xd3I\xe2\xe5B$\x99\x00[\x01\xe8\xc5\
\xd9G\xfaN`\xd8\x81I\xed\x8c\xb19\x94\x8d\xcbL\x00;t\xcf\x9fwPh\xdb\x0e\xe8\
\xd3,\x17\x8b\xc7\x9d\xbb>\x8a \xec5\x94\tc\xc4\x12\xab\x94\xeb\x7fkWr\xc9B%\
\xfc\xd2\xfcM<\x01\xf6tn\x12O3c\xe6\x00\x00\x00\x00IEND\xaeB`\x82' 

def getContinueBitmap():
    return BitmapFromImage(getContinueImage())

def getContinueImage():
    stream = cStringIO.StringIO(getContinueData())
    return ImageFromStream(stream)

def getContinueIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getContinueBitmap())
    return icon

#----------------------------------------------------------------------
def getNextData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x8eIDAT8\x8d\xa5SA\x12\xc4 \x08K\xb0\xff\xde\xe9\xbf\xb7\xa6\x87\
\x1d:\xba\xa2tZn(\x84`"i\x05obk\x13\xd5CmN+\xcc\x00l\xd6\x0c\x00\xf5\xf8\x0e\
gK\x06\x00 \xa5=k\x00\x00\xb0\xb2]\xd4?5f\xb1\xdb\xaf\xc6\xa2\xcb\xa8\xf0?\
\x1c\x98\xae\x82\xbf\x81\xa4\x8eA\x16\xe1\n\xd1\xa4\x19\xb3\xe9\n\xce\xe8\
\xf1\n\x9eg^\x18\x18\x90\xec<\x11\xf9#\x04XMZ\x19\xaac@+\x94\xd4\x99)SeP\xa1\
)\xd6\x1dI\xe7*\xdc\xf4\x03\xdf~\xe7\x13T^Q?:X\x19d\x00\x00\x00\x00IEND\xaeB\
`\x82' 

def getNextBitmap():
    return BitmapFromImage(getNextImage())

def getNextImage():
    stream = cStringIO.StringIO(getNextData())
    return ImageFromStream(stream)

def getNextIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getNextBitmap())
    return icon

#----------------------------------------------------------------------
def getStepInData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x87IDAT8\x8d\xadSA\x12\x84 \x0ck\x8a\xffv\xfc\xb74{X\xeb0P@\x07s\
\x84\xa4$\x01\x00M\xb2\x02]R\x8b\xc86\xda\xdc\xedd\xb4~\xe8\x86\xc6\x01-\x93\
\x96\xd9#\xf6\x06\xc3;p1I\xd1\x14\x0b#|\x17aF\xec\r\xeeF\xa0eB\xd34\xca\xd0A\
]j\x84\xa6\x03\x00""\xb7\xb0tRZ\xf7x\xb7\x83\x91]\xcb\x7fa\xd9\x89\x0fC\xfd\
\x94\x9d|9\x99^k\x13\xa1 \xb3\x16\x0f#\xd4\x88N~\x14\xe1-\x96\x7f\xe3\x0f\
\x11\x91UC\x0cX\'\x1e\x00\x00\x00\x00IEND\xaeB`\x82' 

def getStepInBitmap():
    return BitmapFromImage(getStepInImage())

def getStepInImage():
    stream = cStringIO.StringIO(getStepInData())
    return ImageFromStream(stream)

def getStepInIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getStepInBitmap())
    return icon

#----------------------------------------------------------------------
def getStopData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00FIDAT8\x8d\xed\x91\xc1\t\xc00\x0c\x03O!\x0b\xa9\xfb\xef\xa6\xfcB\xa1\
N\t\xf4Wr\xa0\x8f\xb1\x0f\x81\xe1\x97\xe4-\xb6}_V%\xc8\xc2, \t\x92\xe6]\xfbZ\
\xf7\x08\xa0W\xc3\xea5\xdb\rl_IX\xe5\xf0d\x00\xfa\x8d#\x7f\xc4\xf7'\xab\x00\
\x00\x00\x00IEND\xaeB`\x82" 

def getStopBitmap():
    return BitmapFromImage(getStopImage())

def getStopImage():
    stream = cStringIO.StringIO(getStopData())
    return ImageFromStream(stream)

def getStopIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getStopBitmap())
    return icon

#----------------------------------------------------------------------
def getStepReturnData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x8dIDAT8\x8d\xa5S\xd1\x0e\xc4 \x08\xa3\xb0\xff\xbe\xdc\x7fO\xba'6\
\xf1\xf44\xb3O$Phk\x04\xd4d\x07\xba\xc5\x16\x91#\nza\xdb\x84\x1a\xa2\xfe\xf8\
\x99\xfa_=p+\xe8\x91ED\xbc<\xa4 \xb4\x0b\x01\xb5{\x01\xf9\xbbG-\x13\x87\x16f\
\x84\xbf\x16V\xb0l\x01@\no\x86\xae\x82Q\xa8=\xa4\x0c\x80\xe70\xbd\x10jh\xbd\
\x07R\x06#\xc9^N\xb6\xde\x03)\x83\x18\xaeU\x90\x9c>a\xb2P\r\xb3&/Y\xa8\xd1^^\
\xb6\xf0\x16\xdb\xbf\xf1\x02\x81\xa5TK\x1d\x07\xde\x92\x00\x00\x00\x00IEND\
\xaeB`\x82" 

def getStepReturnBitmap():
    return BitmapFromImage(getStepReturnImage())

def getStepReturnImage():
    stream = cStringIO.StringIO(getStepReturnData())
    return ImageFromStream(stream)

def getStepReturnIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getStepReturnBitmap())
    return icon
    
def getAddWatchData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\x85IDAT8\x8dc`\x18h\xc0\x88.\xd0\xc0\xf0\xff?*\x9f\x11C\rN\x80\xae\
\x19\x97\x18\xd1\x9a\x896\x84\x18[p\xa9aA\xe6\xfc7f\xc0P\xc4x\x163\x9cp\x1a0\
\xeb,!w\x100 \x1dK\xac\x10\r\x08\x05".yFL\x85\x8c\x18b\xa8|Ty\xa2\x13\x92\'\
\xc3\xe4\xff\x9f\x18\x1e3\xb82t\xa2\x88\x13\xedg.\x06aa&\x06VV\x7f\x86\xb9\
\xcfU\x19\xbc\xb0\xba\x86h\xe0\xc8\xd0\xfc\xbf\x80\xe1>q)\x94\xe6\x00\x00\
\x85\x923_\xd22\xa4\xcd\x00\x00\x00\x00IEND\xaeB`\x82' 

def getAddWatchBitmap():
    return BitmapFromImage(getAddWatchImage())

def getAddWatchImage():
    stream = cStringIO.StringIO(getAddWatchData())
    return ImageFromStream(stream)

def getAddWatchIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getAddWatchBitmap())
    return icon
