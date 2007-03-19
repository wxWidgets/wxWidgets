import wx

class Canvas(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.Bind (wx.EVT_PAINT, self.OnPaint)
        self.bkg_brush = wx.Brush((190,190,255))
        self.v_pos = 100
       
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc = wx.GCDC(dc)
        dc.SetBackground(self.bkg_brush)
        dc.Clear()
        dc.DrawEllipticArc(100, 200, 200, 200-self.v_pos, 90,270)
        dc.DrawLine(100, 100, 100, 300)

       
class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="DrawEllipticArc")
        self.SetSize((640,480))
        self.canvas = Canvas(self)
        self.slider = wx.Slider(self, minValue=0, maxValue=100)
        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(self.canvas, 1, wx.EXPAND)
        box.Add(self.slider, 0, wx.EXPAND)
        self.SetSizer(box)
        self.Show()
        self.CenterOnScreen()
        self.slider.Bind (wx.EVT_SLIDER, self.OnSlide)
       
    def OnSlide(self, evt):
        self.canvas.v_pos = 100+self.slider.GetValue()
        self.canvas.Refresh(False)

       
if __name__ == "__main__":
    app = wx.App(0)
    frame = MyFrame()
    app.MainLoop()
