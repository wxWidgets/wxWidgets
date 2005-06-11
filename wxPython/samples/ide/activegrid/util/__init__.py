#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      Utilities
#
# Author:       Joel Hare
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import traceback
import sys
import os

def isWindows():
    return os.name == 'nt'

def _generateMainModuleDir():
    if sys.executable.find('python') != -1:
        utilModuleDir = os.path.dirname(__file__)
        if not os.path.isabs(utilModuleDir):
            utilModuleDir = os.path.join(os.getcwd(), utilModuleDir)
        mainModuleDir = os.path.normpath(os.path.join(utilModuleDir, os.path.join(os.path.pardir, os.path.pardir)))
        if mainModuleDir.endswith('.zip'):
            mainModuleDir = os.path.dirname(mainModuleDir) # Get rid of library.zip
    else:
        mainModuleDir = os.path.dirname(sys.executable)
    return mainModuleDir

mainModuleDir = _generateMainModuleDir()


def _generatePythonExecPath():
    if sys.executable.find('python') != -1:
        pythonExecPath = sys.executable
    else:
        pythonExecPath = os.path.join(os.path.dirname(sys.executable), '3rdparty\python2.3\python')
    return pythonExecPath

pythonExecPath = _generatePythonExecPath()

def getCommandNameForExecPath(execPath):
    if isWindows():
        return '"%s"' % execPath
    return execPath

