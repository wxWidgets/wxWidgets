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

# Updates:
# call back to function if changes made

# Cliff Wells, logiplexsoftware@earthlink.net:
# - Made ColourSelect into "is a button" rather than "has a button"
# - Added label parameter and logic to adjust the label colour according to the background
#   colour
# - Added id argument
# - Rearranged arguments to more closely follow wx conventions
# - Simplified some of the code


class ColourSelect(wxButton):
    def __init__(self, parent, id, label = "", bcolour=(0, 0, 0),
            pos = wxDefaultPosition, size = wxDefaultSize,
                callback = None):
        wxButton.__init__(self, parent, id, label, pos=pos, size=size)
        self.SetColour(bcolour)
        self.callback = callback
        EVT_BUTTON(parent, self.GetId(), self.OnClick)

    def GetColour(self):
        return self.set_colour

    def GetValue(self):
        return self.set_colour

    def SetColour(self, bcolour):
        self.set_colour_val = wxColor(bcolour[0], bcolour[1], bcolour[2])
        self.SetBackgroundColour(self.set_colour_val)
        avg = reduce(lambda a, b: a + b, bcolour) / 3
        fcolour = avg > 128 and (0, 0, 0) or (255, 255, 255)
        self.SetForegroundColour(apply(wxColour, fcolour))
        self.set_colour = bcolour

    def SetValue(self, bcolour):
        self.SetColour(bcolour)

    def OnChange(self):
        if self.callback is not None:
            self.callback()

    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        data.SetColour(self.set_colour_val)
        dlg = wxColourDialog(self.GetParent(), data)
        changed = dlg.ShowModal() == wxID_OK
        if changed:
            data = dlg.GetColourData()
            self.SetColour(data.GetColour().Get())
        dlg.Destroy()

        if changed:
            self.OnChange() # moved after dlg.Destroy, since who knows what the callback will do...
