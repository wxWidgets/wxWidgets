# AnalogClock's colour selector for setup dialog
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

import wx
from wx.lib.newevent import NewEvent
from wx.lib.buttons import GenBitmapButton

#----------------------------------------------------------------------------

(ColourSelectEvent, EVT_COLOURSELECT) = NewEvent()

#----------------------------------------------------------------------------

class ColourSelect(GenBitmapButton):
    def __init__(self, parent, size=(21, 21), value=wx.BLACK):
        w, h = size[0] - 5, size[1] - 5
        GenBitmapButton.__init__(self, parent, wx.ID_ANY, wx.EmptyBitmap(w, h),
                                 size=size)
        self.SetBezelWidth(1)

        self.parent = parent
        self.SetValue(value)

        self.parent.Bind(wx.EVT_BUTTON, self.OnClick, self)


    def _makeBitmap(self):
        bdr = 8; w, h = self.GetSize()
        bmp = wx.EmptyBitmap(w - bdr, h - bdr)

        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.Brush(self.value, wx.SOLID))
        dc.Clear()
        dc.SelectObject(wx.NullBitmap)

        self.SetBitmapLabel(bmp)
        self.Refresh()


    def GetValue(self):
        return self.value


    def SetValue(self, value):
        self.value = value
        self._makeBitmap()


    def OnClick(self, event):
        win = wx.GetTopLevelParent(self)

        data = wx.ColourData()
        data.SetChooseFull(True)
        data.SetColour(self.value)
        [data.SetCustomColour(colour_index, win.customcolours[colour_index])
         for colour_index in range(0, 16)]

        dlg = wx.ColourDialog(win, data)
        dlg.SetTitle("Select Colour")
        changed = dlg.ShowModal() == wx.ID_OK

        if changed:
            data = dlg.GetColourData()
            self.SetValue(data.GetColour())
            win.customcolours = [data.GetCustomColour(colour_index) \
                                 for colour_index in range(0, 16)]
        dlg.Destroy()

        if changed:
            nevt = ColourSelectEvent(id=self.GetId(), obj=self, val=self.value)
            wx.PostEvent(self.parent, nevt)


#
##
### eof
