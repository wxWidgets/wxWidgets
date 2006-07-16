#----------------------------------------------------------------------------
# Name:         sysutils.py
# Purpose:      System Utilities
#
# Author:       Joel Hare
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import sys
import os
import time

# this will be set to true in IDE.py when we are running release builds.
isRelease = False

# Commented out for now.....
# Required for Unicode support with python
# Put over here because of py2exe problems
# Python suggests modifying site.py 
#if hasattr(sys,"setdefaultencoding"):
#    sys.setdefaultencoding("UTF-8")


MAINMODULE_DIR = "AG_MAINMODULE_DIR"
IS_RELEASE = "AG_IS_RELEASE"
IS_COMMERCIAL = "AG_IS_COMMERCIAL"
AG_SYSTEM_START_TIME_ENV_NAME = "AG_SYSTEM_START_TIME"

def isCommercial():
    
    return os.path.exists(os.path.join(mainModuleDir,"commercial.txt")) or 'true' == (str(os.getenv(IS_COMMERCIAL)).lower())
 
def isRelease():
    return 'true' == (str(os.getenv(IS_RELEASE)).lower())
    
def setRelease(value):
    if value:
        os.environ[IS_RELEASE]= "TRUE"
    else:
        os.environ[IS_RELEASE]= "FALSE"

def isWindows():
    return os.name == 'nt'

__isServer = False

def setServerMode(isServer):
    global __isServer
    __isServer = isServer
    
def isServer():
    global __isServer
    return __isServer
    
def _generateMainModuleDir():
    mainModuleDir = os.getenv(MAINMODULE_DIR)
    if mainModuleDir:  # if environment variable set, return it
        return mainModuleDir
    
    # On Mac, the python executable sometimes has a capital "P" so we need to 
    # lower the string first
    sysExecLower = sys.executable.lower()
    if sysExecLower == "/" or sysExecLower.find('python') != -1 or sysExecLower.find('apache') != -1:
        utilModuleDir = os.path.dirname(__file__)
        if not os.path.isabs(utilModuleDir):
            utilModuleDir = os.path.join(os.getcwd(), utilModuleDir)
        mainModuleDir = os.path.normpath(os.path.join(utilModuleDir, os.path.join(os.path.pardir, os.path.pardir)))
        if mainModuleDir.endswith('.zip'):
            mainModuleDir = os.path.dirname(mainModuleDir) # Get rid of library.zip
    else:
        mainModuleDir = os.path.dirname(sys.executable)
        
    os.environ[MAINMODULE_DIR] = mainModuleDir  # pythonBug: os.putenv doesn't work, set environment variable
    
    return mainModuleDir

mainModuleDir = _generateMainModuleDir()
    
def _generatePythonExecPath():
    # On Mac, the python executable sometimes has a capital "P" so we need to 
    # lower the string first
    sysExecLower = sys.executable.lower()
    if sysExecLower.find('python') != -1 or sysExecLower.find('apache') != -1:
        pythonExecPath = sys.executable
    else:
        # this is where py2app puts the Python executable
        if sys.platform == "darwin":
            pythonExecPath = os.path.join(os.path.dirname(sys.executable), "../Frameworks/Python.Framework/Versions/2.4/Python/bin")
        else:
            pythonExecPath = os.path.join(os.path.dirname(sys.executable), '3rdparty\python2.4\python')
    return pythonExecPath

pythonExecPath = _generatePythonExecPath()

def getCommandNameForExecPath(execPath):
    if isWindows():
        return '"%s"' % execPath
    return execPath

def getUserName():
    if isWindows():
        return os.getenv('USERNAME')
    else:
        # 06-Feb-06 stoens@activegrid.com --
        # this blows up the linux cc runs with "Inappropriate ioctl for device"
        #return os.getlogin()
        return os.getenv('USER')        

def getCurrentTimeAsFloat():
    return time.time()

systemStartTime = getCurrentTimeAsFloat()
