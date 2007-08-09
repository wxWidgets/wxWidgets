import wx

class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        r = wx.Rect(10,10, 22,22)
        dc.SetClippingRect(r)
        rndr = wx.RendererNative.Get()
        rndr.DrawComboBoxDropButton(self, dc, r)

        r.x = 50
        rndr.DrawPushButton(self, dc, r)
        

app = wx.App(False)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
