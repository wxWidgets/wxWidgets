import wx

class TestPanel(wx.Panel):
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        gc = wx.GraphicsContext.Create(wx.PaintDC(self))
        
        pen = wx.Pen("navy", 2)
        gc.SetPen(pen)
        brush = wx.Brush((255,32,32,128))
        gc.SetBrush(brush)
        gc.PushState()

        path = gc.CreatePath()
        path.MoveToPoint(50, 50)
        path.AddLineToPoint(25,25)
        path.AddLineToPoint(50,25)
        path.AddLineToPoint(50,50)
        path.CloseSubpath()
        
        gc.DrawPath(path)

        gc.Scale(2,2)
        gc.Translate(10,5)
        gc.DrawPath(path)

        gc.Translate(50,0)
        gc.FillPath(path)
        gc.Translate(0,5)
        gc.StrokePath(path)
        gc.Translate(0,5)

        brush = wx.Brush((32,32,255,128))
        gc.SetBrush(brush)

        gc.FillPath(path)
        gc.Translate(50,0)
        gc.DrawPath(path)

        gc.PopState()

        points = [ (5.2, 5.9),
                   (50, 50),
                   (35, 50),
                   (25,40),
                   wx.Point2D(20.5,50.9),
                   wx.Point2D(5,25),
                   (5,6)
                   ]
        
        gc.Translate(0, 150)
        gc.DrawLines(points)
        gc.Translate(75, 0)
        gc.StrokeLines(points)


        begin = [ (0,0),
                  (0,10),
                  (0,20),
                  (0,30),
                  (0,40),
                  (0,50),
                  ]

        end   = [ (50,0),
                  (50,10),
                  (50,20),
                  (50,30),
                  (50,40),
                  (50,50),
                  ]

        # in a floating point coordinate system the center of the
        # pixel is actually at x+0.5, y+0.5, so with anti-aliasing
        # turned on we'll get a crisper line by positioning our line
        # segments at the 0.5 offset.  For this test we'll just let
        # the GC do the translation for us.
        gc.Translate(0.5, 0.5)
        
        pen = wx.Pen("purple", 1)
        gc.SetPen(pen)

        gc.Translate(75, 0)
        gc.StrokeLineSegements(begin, end)
        
        gc.Translate(75, 0)
        gc.Scale(2,2)
        gc.StrokeLineSegements(begin, end)
        gc.DrawLines(points)

        del path
        

app = wx.App(False)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
