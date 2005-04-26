# Name:         globals.py
# Purpose:      XRC editor, global variables
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      02.12.2002
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *
try:
    from wxPython.wizard import *
except:
    pass
import sys

# Global constants

progname = 'XRCed'
version = '0.1.5-1'
# Can be changed to set other default encoding different
defaultEncoding = sys.getdefaultencoding()

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
    currentEncoding = defaultEncoding

    def _makeFonts(self):
        self._sysFont = wxSystemSettings_GetFont(wxSYS_SYSTEM_FONT)
        self._labelFont = wxFont(self._sysFont.GetPointSize(), wxDEFAULT, wxNORMAL, wxBOLD)
        self._modernFont = wxFont(self._sysFont.GetPointSize(), wxMODERN, wxNORMAL, wxNORMAL)
        self._smallerFont = wxFont(self._sysFont.GetPointSize()-2, wxDEFAULT, wxNORMAL, wxNORMAL)
        
    def sysFont(self):
        if not hasattr(self, "_sysFont"): self._makeFonts()
        return self._sysFont
    def labelFont(self):
        if not hasattr(self, "_labelFont"): self._makeFonts()
        return self._labelFont
    def modernFont(self):
        if not hasattr(self, "_modernFont"): self._makeFonts()
        return self._modernFont
    def smallerFont(self):
        if not hasattr(self, "_smallerFont"): self._makeFonts()
        return self._smallerFont
    

g = Globals()
