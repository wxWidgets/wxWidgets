# AnalogClock's font selector for setup dialog
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

import wx
from wx.lib.newevent import NewEvent
from wx.lib.buttons import GenButton

#----------------------------------------------------------------------------

(FontSelectEvent, EVT_FONTSELECT) = NewEvent()

#----------------------------------------------------------------------------

class FontSelect(GenButton):
    def __init__(self, parent, size=(75, 21), value=None):
        GenButton.__init__(self, parent, wx.ID_ANY, label="Select...", 
                           size=size)
        self.SetBezelWidth(1)

        self.parent = parent
        self.SetValue(value)
        
        self.parent.Bind(wx.EVT_BUTTON, self.OnClick, self)


    def GetValue(self):
        return self.value


    def SetValue(self, value):
        if value is None:
            value = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        self.value = value


    def OnClick(self, event):
        data = wx.FontData()
        data.EnableEffects(False)
        font = self.value; font.SetPointSize(10)
        data.SetInitialFont(font)

        dlg = wx.FontDialog(self, data)
        changed = dlg.ShowModal() == wx.ID_OK

        if changed:
            data = dlg.GetFontData()
            self.value = data.GetChosenFont()
            self.Refresh()
        dlg.Destroy()

        if changed:
            nevt = FontSelectEvent(id=self.GetId(), obj=self, val=self.value)
            wx.PostEvent(self.parent, nevt)


#
##
### eof
