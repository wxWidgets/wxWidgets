#!/bin/env python
#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Box Selection Control
#
# Author:       Lorne White, Lorne.White@telusplanet.net
#
# Created:      Sept 4, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

# creates a colour wxButton with selectable color
# button click provides a colour selection box
# button colour will change to new colour
# GetColour method to get the selected colour

# Updates:
# call back to function if changes made

class ColourSelect(wxButton):
    def __init__(self, parent, ID, bcolour=[0, 0, 0], pos=wxPoint(20, 20), size=wxSize(20, 20), style=0, callback=None):
        wxButton.__init__(self, parent, ID, "", pos, size, style)
        self.win = parent
        self.callback = callback
        EVT_BUTTON(self, ID, self.OnClick)
        self.SetColourValue(bcolour)

    def __del__(self):
        if hasattr(self, "set_colour_val"):
            del self.set_colour_val

    def SetColour(self, bcolour):
        self.SetBackgroundColour(bcolour)

    def SetColourValue(self, bcolour):
        self.set_colour_val = wxColour(bcolour[0], bcolour[1], bcolour[2])
        self.set_colour = bcolour

        self.SetBackgroundColour(self.set_colour_val)
        self.SetForegroundColour(wxWHITE)

    def SetValue(self, bcolour):
        self.SetColourValue(bcolour)

    def GetColour(self):
        return self.set_colour

    def OnChange(self):
        if self.callback != None:
            self.callback()

    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        data.SetColour(self.set_colour_val)
        dlg = wxColourDialog(self.win, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetColourData()
            self.set_colour = set = data.GetColour().Get()
            self.set_colour_val = bcolour = wxColour(set[0],set[1],set[2])
            self.SetBackgroundColour(bcolour)
            self.OnChange()
        dlg.Destroy()





