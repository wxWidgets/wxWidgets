#!/bin/env python
#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Box Selection Control
#
# Author:       Lorne White, Lorne.White@telusplanet.net
#
# Created:	    Feb 25, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

# creates a colour wxButton with selectable color
# button click provides a colour selection box
# button colour will change to new colour
# GetColour method to get the selected colour

class ColourSelect:
    def __init__(self, parent, position = wxPoint(20, 20), bcolour = [0, 0, 0], size = wxSize(20, 20)):
        self.win = parent
        if size is None:
            size = wxSize(20, 20)
        
        mID = NewId()
        self.b = b = wxButton(parent, mID, "", position, size)
        EVT_BUTTON(parent, mID, self.OnClick)
        
        self.set_colour_val = set_colour = wxColor(bcolour[0], bcolour[1], bcolour[2])
        b.SetBackgroundColour(set_colour)
        b.SetForegroundColour(wxWHITE)
        self.set_colour = bcolour

    def SetColour(self, bcolour):
        self.b.SetBackgroundColour(bcolour)
        
    def GetColour(self):
        return self.set_colour
    
    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        data.SetColour(self.set_colour_val)
        dlg = wxColourDialog(self.win, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetColourData()
            self.set_colour = set = data.GetColour().Get()            
            self.set_colour_val = bcolour = wxColour(set[0],set[1],set[2])
            self.b.SetBackgroundColour(bcolour)
        dlg.Destroy()
