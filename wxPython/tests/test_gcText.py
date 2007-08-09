
import wx

#----------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        try:
            gc = wx.GraphicsContext.Create(dc)
        except NotImplementedError:
            dc.DrawText("This build of wxPython does not support the wx.GraphicsContext "
                        "family of classes.",
                        25, 25)
            return

        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.BOLD)
        gc.SetFont(font)

        gc.Translate(10, 10)
        self.DrawText(gc, 'normal')

        gc.Translate(0, 25)
        gc.PushState()
        gc.Scale(2, 2)
        self.DrawText(gc, 'scaled')
        gc.PopState()
        
        gc.Translate(0, 35)
        self.DrawText(gc, '\nnewline')

    def DrawText(self, gc, txt):
        txt = "This is a test: " + txt
        w,h,d,e = gc.GetFullTextExtent(txt)
        ##print w,h,d,e

        gc.DrawText(txt, 0, 0)

        pen = wx.Pen("red", 1)
        gc.SetPen(pen)

        path = gc.CreatePath()       
        path.MoveToPoint(-1, -1)
        self.MakeCrosshair(path)
        gc.StrokePath(path)

        path = gc.CreatePath()       
        path.MoveToPoint(w+1, h+1)
        self.MakeCrosshair(path)
        gc.StrokePath(path)

        
        

    def MakeCrosshair(self, path):
        x, y = path.GetCurrentPoint()
        path.MoveToPoint(x-5, y)
        path.AddLineToPoint(x+5,y)
        path.MoveToPoint(x, y-5)
        path.AddLineToPoint(x, y+5)
        path.MoveToPoint(x,y)
        



app = wx.App(False)
frm = wx.Frame(None, title="Testing GC Text")
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
