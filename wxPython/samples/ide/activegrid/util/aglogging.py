#----------------------------------------------------------------------------
# Name:         aglogging.py
# Purpose:      Utilities to help with logging
#
# Author:       Jeff Norton
#
# Created:      01/04/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import sys
import os
import re
import traceback

global agTestMode
agTestMode = False

def setTestMode(mode):
    global agTestMode
    if (mode):
        agTestMode = True
    else:
        agTestMode = False
        
def getTestMode():
    global agTestMode
    return agTestMode
    
def testMode(normalObj, testObj=None):
    if getTestMode():
        return testObj
    return normalObj

def toDiffableString(value):
    s = repr(value)
    ds = ""
    i = s.find(" at 0x") 
    start = 0
    while (i >= 0):
        j = s.find(">", i)
        if (j < i):
            break
        ds += s[start:i]
        start = j
        i = s.find(" at 0x", start) 
    return ds + s[start:]
    
def removeFileRefs(str):
    str = re.sub(r'(?<=File ")[^"]*(\\[^\\]*")(, line )[0-9]*', _fileNameReplacement, str)
    return str
    
def _fileNameReplacement(match):
    return "...%s" % match.group(1)

def getTraceback():
    extype, val, tb = sys.exc_info()
    tbs = "\n"
    for s in traceback.format_tb(tb):
        tbs += s
    return tbs

def reportException(out=None, stacktrace=False, diffable=False):
    extype, val, t = sys.exc_info()
    if (diffable):
        exstr = removeFileRefs(str(val))
    else:
        exstr = str(val)
    if (out == None):
        print "Got Exception = %s: %s" % (extype, exstr)
    else:
        print >> out, "Got Exception = %s: %s" % (extype, exstr)
    if (stacktrace):
        fmt = traceback.format_exception(extype, val, t)
        for s in fmt:
            if (diffable):
                s = removeFileRefs(s)
            if (out == None):
                print s
            else:
                print >> out, s
            
