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




class ColourSelect(wxBitmapButton):
    def __init__(self, parent, id, label="", colour=wxBLACK,
                 pos=wxDefaultPosition, size=wxDefaultSize,
                 callback=None, style=0):
        if label:
            w, h = parent.GetTextExtent(label)
            w += 6
            h += 6
        else:
            w, h = 20, 20
        wxBitmapButton.__init__(self, parent, id, wxEmptyBitmap(w,h),
                                pos=pos, size=size, style=style|wxBU_AUTODRAW)
        if type(colour) == type( () ):
            colour = wxColour(*colour)
        self.colour = colour
        self.SetLabel(label)
        self.callback = callback
        bmp = self.MakeBitmap()
        self.SetBitmap(bmp)
        EVT_BUTTON(parent, self.GetId(), self.OnClick)


    def GetColour(self):
        return self.colour


    def GetValue(self):
        return self.colour


    def SetValue(self, colour):
        self.SetColour(colour)


    def SetColour(self, colour):
        if type(colour) == type( () ):
            colour = wxColour(*colour)
        self.colour = colour
        bmp = self.MakeBitmap()
        self.SetBitmap(bmp)


    def MakeBitmap(self):
        bdr = 10
        sz = self.GetSize()
        bmp = wxEmptyBitmap(sz.width-bdr, sz.height-bdr)
        dc = wxMemoryDC()
        dc.SelectObject(bmp)
        label = self.GetLabel()
        # Just make a little colored bitmap
        dc.SetBackground(wxBrush(self.colour))
        dc.Clear()
        if label:
            # Add a label to it
            avg = reduce(lambda a, b: a + b, self.colour.Get()) / 3
            fcolour = avg > 128 and wxBLACK or wxWHITE
            dc.SetTextForeground(fcolour)
            dc.DrawLabel(label, (0,0, sz.width-bdr, sz.height-bdr),
                         wxALIGN_CENTER)
            
        dc.SelectObject(wxNullBitmap)
        return bmp


    def SetBitmap(self, bmp):
        self.SetBitmapLabel(bmp)
        self.SetBitmapSelected(bmp)
        self.SetBitmapDisabled(bmp)
        self.SetBitmapFocus(bmp)
        self.SetBitmapSelected(bmp)


    def OnChange(self):
        wxPostEvent(self, ColourSelectEvent(self.GetId(), self.GetValue()))
        if self.callback is not None:
            self.callback()

    def OnClick(self, event):
        data = wxColourData()
        data.SetChooseFull(True)
        data.SetColour(self.colour)
        dlg = wxColourDialog(self.GetParent(), data)
        changed = dlg.ShowModal() == wxID_OK
        if changed:
            data = dlg.GetColourData()
            self.SetColour(data.GetColour())
        dlg.Destroy()

        if changed:
            self.OnChange() # moved after dlg.Destroy, since who knows what the callback will do...

