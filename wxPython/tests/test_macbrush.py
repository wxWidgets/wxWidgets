import wx


class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        br = wx.Brush("light blue")
        dc.SetBrush(br)
        dc.DrawRectangle(10,10, 50,50)

        br = wx.Brush(self.GetBackgroundColour())
        br.MacSetTheme(1)
        dc.SetBrush(br)
        dc.DrawRectangle(80,10, 50,50)
        

app = wx.App(False)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
