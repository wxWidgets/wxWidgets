#!/bin/env python
#----------------------------------------------------------------------------
# Name:         ColorPanel.py
# Purpose:      Testing lots of stuff, controls, window types, etc.
#
# Author:       Robin Dunn & Gary Dumer
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

#---------------------------------------------------------------------------


class ColoredPanel(wxWindow):
    def __init__(self, parent, color):
        wxWindow.__init__(self, parent, -1,
                          wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
        self.SetBackgroundColour(color)

#---------------------------------------------------------------------------