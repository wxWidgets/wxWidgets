#----------------------------------------------------------------------------
# Name:         ActiveGridIDE.py
# Purpose:
#
# Author:       Lawrence Bruhmuller
#
# Created:      3/30/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
import wx.lib.pydocview
import activegrid.tool.IDE

import os
import sys
sys.stdout = sys.stderr

# This is here as the base IDE entry point.  Only difference is that -baseide is passed.

sys.argv.append('-baseide');

# Put activegrid dir in path so python files can be found from py2exe
# This code should never do anything when run from the python interpreter
execDir = os.path.dirname(sys.executable)
try:
    sys.path.index(execDir)
except ValueError:
    sys.path.append(execDir)
app = activegrid.tool.IDE.IDEApplication(redirect = False)
app.GetTopWindow().Raise()  # sometimes it shows up beneath other windows.  e.g. running self in debugger
app.MainLoop()

