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

    def SetColour(self, bcolour):
        self.SetBackgroundColour(bcolour)

    def SetColourValue(self, bcolour):
        self.set_colour = bcolour
        self.SetBackgroundColour(self.Get_wxColour())
        self.SetForegroundColour(wxWHITE)

    def SetValue(self, bcolour):
        self.SetColourValue(bcolour)

    def GetColour(self):
        return self.set_colour

    def Get_wxColour(self):
        return wxColour(self.set_colour[0], self.set_colour[1], self.set_colour[2])

    def OnChange(self):
        if self.callback != None:
            self.callback()

    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        data.SetColour(self.Get_wxColour())
        dlg = wxColourDialog(self.win, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetColourData()
            self.set_colour = set = data.GetColour().Get()
            self.SetBackgroundColour(self.Get_wxColour())
            self.OnChange()
        dlg.Destroy()

