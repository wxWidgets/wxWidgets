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
import logging
from activegrid.util.lang import *

LEVEL_FATAL = logging.FATAL
LEVEL_ERROR = logging.ERROR
LEVEL_WARN = logging.WARN
LEVEL_INFO = logging.INFO
LEVEL_DEBUG = logging.DEBUG

TEST_MODE_NONE = 0
TEST_MODE_DETERMINISTIC = 1
TEST_MODE_NON_DETERMINISTIC = 2

global agTestMode
agTestMode = TEST_MODE_NONE

def setTestMode(mode):
    global agTestMode
    agTestMode = mode
        
def getTestMode():
    global agTestMode
    return agTestMode
    
def testMode(normalObj, testObj=None):
    if getTestMode() > TEST_MODE_NONE:
        return testObj
    return normalObj

pythonFileRefPattern = asString(r'(?<=File ")[^"]*(#[^#]*")(, line )[0-9]*')
phpFileRefPattern = asString(r'( in ).*#([^#]*#[^ ]*)(?= on line )')
pathSepPattern = os.sep
if (pathSepPattern == "\\"):
    pathSepPattern = "\\\\"
pythonFileRefPattern = pythonFileRefPattern.replace("#", pathSepPattern)
pythonFileRefPattern = re.compile(pythonFileRefPattern)
phpFileRefPattern = phpFileRefPattern.replace("#", pathSepPattern)
phpFileRefPattern = re.compile(phpFileRefPattern)

def removeFileRefs(str):
    str = pythonFileRefPattern.sub(_fileNameReplacement, str)
    str = phpFileRefPattern.sub(_fileNameReplacementPHP, str)
    return str
    
def removePHPFileRefs(str):
    str = phpFileRefPattern.sub(_fileNameReplacementPHP, str)
    return str
    
def _fileNameReplacement(match):
    return "...%s" % match.group(1).replace(os.sep, "/")
    
def _fileNameReplacementPHP(match):
    return "%s...%s" % (match.group(1), match.group(2).replace(os.sep, "/"))
    
def getTraceback():
    extype, val, tb = sys.exc_info()
    tbs = "\n"
    for s in traceback.format_tb(tb):
        tbs += s
    return tbs

def reportException(out=None, stacktrace=False, diffable=False, exception=None):
    if (True): # exception == None):
        extype, val, t = sys.exc_info()
    else:
        extype = type(exception)
        val = exception
        if (stacktrace):
            e,v,t = sys.exc_info()
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
            
