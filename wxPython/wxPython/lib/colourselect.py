#!/bin/env python
#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Box Selection Control
#
# Author:       Lorne White, Lorne.White@telusplanet.net
#
# Created:      Feb 25, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

# creates a colour wxButton with selectable color
# button click provides a colour selection box
# button colour will change to new colour
# GetColour method to get the selected colour

class ColourSelect(wxButton):
    def __init__(self, parent, id, bcolour=(0, 0, 0), pos=wxDefaultPosition, size=wxDefaultSize):
        wxButton.__init__(self, parent, id, "", pos=pos, size=size)
        EVT_BUTTON(parent, self.GetId(), self.OnClick)
        self.SetForegroundColour(wxWHITE)
        self.SetColour(bcolour)


    def SetColour(self, bcolour):
        self.set_colour_val = set_colour = wxColor(bcolour[0], bcolour[1], bcolour[2])
        self.SetBackgroundColour(set_colour)
        self.set_colour = bcolour


    def GetColour(self):
        return self.set_colour


    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        data.SetColour(self.set_colour_val)
        dlg = wxColourDialog(self.GetParent(), data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetColourData()
            self.SetColour(data.GetColour().Get())
        dlg.Destroy()

