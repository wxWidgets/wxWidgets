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

# Cliff Wells, 2002/02/07
# - Added ColourSelect Event

EVT_COMMAND_COLOURSELECT = wxNewId()

class ColourSelectEvent(wxPyCommandEvent):
        def __init__(self, id, value):
                wxPyCommandEvent.__init__(self, id = id)
                self.SetEventType(EVT_COMMAND_COLOURSELECT)
                self.value = value

        def GetValue(self):
                return self.value

def EVT_COLOURSELECT(win, id, func):
    win.Connect(id, -1, EVT_COMMAND_COLOURSELECT, func)

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
        wxPostEvent(self, ColourSelectEvent(self.GetId(), self.GetValue()))
        if self.callback is not None:
            self.callback()

    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(True)
        data.SetColour(self.set_colour_val)
        dlg = wxColourDialog(self.GetParent(), data)
        changed = dlg.ShowModal() == wxID_OK
        if changed:
            data = dlg.GetColourData()
            self.SetColour(data.GetColour().Get())
        dlg.Destroy()

        if changed:
            self.OnChange() # moved after dlg.Destroy, since who knows what the callback will do...

