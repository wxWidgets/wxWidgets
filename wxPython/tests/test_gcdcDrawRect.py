
import wx

CLIP = True

class TestPanel(wx.Panel):
    def __init__(self, parent, *args):
        wx.Panel.__init__(self, parent, *args)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        pdc = wx.PaintDC(self)
        gcdc = wx.GCDC(pdc)

        for dc, y in [(gcdc, 10), (pdc, 40)]:
            r = wx.Rect(10, y, 100, 20)

            dc.SetPen(wx.Pen("red", 1))
            dc.SetBrush(wx.Brush("light blue"))

            if CLIP: dc.SetClippingRect(r)
            dc.DrawRectangleRect(r)
            dc.DestroyClippingRegion()

            r.Offset((120, 0))
            if CLIP: dc.SetClippingRect(r)
            dc.DrawRoundedRectangleRect(r, 8)
            dc.DestroyClippingRegion()


            r.Offset((120, 0))
            if CLIP: dc.SetClippingRect(r)
            dc.DrawEllipseRect(r)
            dc.DestroyClippingRegion()


app = wx.App(False)
frm = wx.Frame(None, title="wxGCDC Drawing Rectangles")
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()


        
