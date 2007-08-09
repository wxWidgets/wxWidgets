import wx

##import os; print os.getpid(); raw_input("press enter...")


class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.GCDC(wx.PaintDC(self))
        #dc = wx.PaintDC(self)
        r = wx.Rect(64, 25, 45, 18)
        dc.SetPen(wx.Pen("black", 1))
        dc.SetBrush(wx.Brush("red"))
        dc.DrawRectangleRect(r)

        dc.SetPen(wx.TRANSPARENT_PEN)
        #dc.SetPen(wx.Pen("light blue", 1))
        dc.SetBrush(wx.Brush("light blue"))
        dc.DrawRectangle(r.x+1, r.y+1, r.width-2, r.height-2)

        dc.SetPen(wx.Pen("black", 1))
        dc.DrawLine(r.x+r.width+02, r.y,
                    r.x+r.width+15, r.y)
        dc.DrawLine(r.x+r.width+02, r.y+r.height-1,
                    r.x+r.width+15, r.y+r.height-1)
        

app = wx.App(False)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
