#----------------------------------------------------------------------------
# Name:         ColourSelect.py
# Purpose:      Colour Box Selection Control
#
# Author:       Lorne White, Lorne.White@telusplanet.net
#
# Created:      Feb 25, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

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

# 12/01/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for 2.5 compatability.
#

"""
Provides a `ColourSelect` button that, when clicked, will display a
colour selection dialog.  The selected colour is displayed on the
button itself.
"""

#----------------------------------------------------------------------------

import  wx

#----------------------------------------------------------------------------

wxEVT_COMMAND_COLOURSELECT = wx.NewEventType()

class ColourSelectEvent(wx.PyCommandEvent):
    def __init__(self, id, value):
        wx.PyCommandEvent.__init__(self, id = id)
        self.SetEventType(wxEVT_COMMAND_COLOURSELECT)
        self.value = value

    def GetValue(self):
        return self.value

EVT_COLOURSELECT = wx.PyEventBinder(wxEVT_COMMAND_COLOURSELECT, 1)

#----------------------------------------------------------------------------

class ColourSelect(wx.BitmapButton):
    def __init__(self, parent, id=wx.ID_ANY, label="", colour=wx.BLACK,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 callback=None, style=0):
        if label:
            w, h = parent.GetTextExtent(label)
            w += 6
            h += 6
        else:
            w, h = 20, 20
        wx.BitmapButton.__init__(self, parent, id, wx.EmptyBitmap(w,h),
                                 pos=pos, size=size, style=style|wx.BU_AUTODRAW)

        if type(colour) == type( () ):
            colour = wx.Colour(*colour)
        self.colour = colour
        self.SetLabel(label)
        self.callback = callback
        bmp = self.MakeBitmap()
        self.SetBitmap(bmp)
        parent.Bind(wx.EVT_BUTTON, self.OnClick, self)


    def GetColour(self):
        return self.colour

    def GetValue(self):
        return self.colour

    def SetValue(self, colour):
        self.SetColour(colour)

    def SetColour(self, colour):
        if type(colour) == tuple:
            colour = wx.Colour(*colour)
        if type(colour) == str:
            colour = wx.NamedColour(colour)
            
        self.colour = colour
        bmp = self.MakeBitmap()
        self.SetBitmap(bmp)


    def SetLabel(self, label):
        self.label = label

    def GetLabel(self):
        return self.label


    def MakeBitmap(self):
        bdr = 8
        width, height = self.GetSize()

        # yes, this is weird, but it appears to work around a bug in wxMac
        if "wxMac" in wx.PlatformInfo and width == height:
            height -= 1
            
        bmp = wx.EmptyBitmap(width-bdr, height-bdr)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetFont(self.GetFont())
        label = self.GetLabel()
        # Just make a little colored bitmap
        dc.SetBackground(wx.Brush(self.colour))
        dc.Clear()

        if label:
            # Add a label to it
            avg = reduce(lambda a, b: a + b, self.colour.Get()) / 3
            fcolour = avg > 128 and wx.BLACK or wx.WHITE
            dc.SetTextForeground(fcolour)
            dc.DrawLabel(label, (0,0, width-bdr, height-bdr),
                         wx.ALIGN_CENTER)
            
        dc.SelectObject(wx.NullBitmap)
        return bmp


    def SetBitmap(self, bmp):
        self.SetBitmapLabel(bmp)
        self.SetBitmapSelected(bmp)
        self.SetBitmapDisabled(bmp)
        self.SetBitmapFocus(bmp)
        self.SetBitmapSelected(bmp)
        self.Refresh()
        

    def OnChange(self):
        wx.PostEvent(self, ColourSelectEvent(self.GetId(), self.GetValue()))
        if self.callback is not None:
            self.callback()

    def OnClick(self, event):
        data = wx.ColourData()
        data.SetChooseFull(True)
        data.SetColour(self.colour)
        dlg = wx.ColourDialog(wx.GetTopLevelParent(self), data)
        changed = dlg.ShowModal() == wx.ID_OK

        if changed:
            data = dlg.GetColourData()
            self.SetColour(data.GetColour())
        dlg.Destroy()

        # moved after dlg.Destroy, since who knows what the callback will do...
        if changed:
            self.OnChange() 

