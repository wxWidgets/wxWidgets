# Name:         globals.py
# Purpose:      XRC editor, global variables
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      02.12.2002
# RCS-ID:       $Id$

from wxPython.wx import *

# Global constants

if wxPlatform == '__WXGTK__':
    labelFont = wxFont(12, wxDEFAULT, wxNORMAL, wxBOLD)
    modernFont = wxFont(12, wxMODERN, wxNORMAL, wxNORMAL)
else:
    labelFont = wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD)
    modernFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)

progname = 'XRCed'
version = '0.0.9-6'

# Global variables

class Globals:
    panel = None
    tree = None
    frame = None
    undoMan = None
    testWin = None
    testWinPos = wxDefaultPosition
    currentXXX = None

g = Globals()
