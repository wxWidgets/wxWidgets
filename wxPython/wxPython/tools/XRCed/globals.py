# Name:         globals.py
# Purpose:      XRC editor, global variables
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      02.12.2002
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *

# Global constants

sysFont = wxSystemSettings_GetFont(wxSYS_SYSTEM_FONT)
labelFont = wxFont(sysFont.GetPointSize(), wxDEFAULT, wxNORMAL, wxBOLD)
modernFont = wxFont(sysFont.GetPointSize(), wxMODERN, wxNORMAL, wxNORMAL)
smallerFont = wxFont(sysFont.GetPointSize()-2, wxDEFAULT, wxNORMAL, wxNORMAL)

progname = 'XRCed'
version = '0.1.1-3'

try:
    True
except NameError:
    True = 1==1
    False = 1==0

# Global variables

class Globals:
    panel = None
    tree = None
    frame = None
    tools = None
    undoMan = None
    testWin = None
    testWinPos = wxDefaultPosition
    currentXXX = None
    currentEncoding = sys.getdefaultencoding() # wxLocale_GetSystemEncodingName()

g = Globals()
