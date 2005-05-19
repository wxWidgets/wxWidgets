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

def _registerMainModuleDir():
    global mainModuleDir
    if sys.executable.find('python') != -1:
        utilModuleDir = os.path.dirname(__file__)
        if not os.path.isabs(utilModuleDir):
            utilModuleDir = os.path.join(os.getcwd(), utilModuleDir)
        mainModuleDir = os.path.normpath(os.path.join(utilModuleDir, os.path.join(os.path.pardir, os.path.pardir)))
    else:
        mainModuleDir = os.path.dirname(sys.executable)

_registerMainModuleDir()

