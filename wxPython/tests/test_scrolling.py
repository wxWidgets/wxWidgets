
import wx
print wx.version()

SIZE=1000
UNIT=20

class TestWindow(wx.ScrolledWindow):
    def __init__(self, parent):
        wx.ScrolledWindow.__init__(self, parent)
        self.SetBackgroundColour("white")
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda evt: None)
        self.Bind(wx.EVT_SCROLLWIN_LINEUP, self.OnLineUp)
        self.Bind(wx.EVT_SCROLLWIN_LINEDOWN, self.OnLineDown)
        self.Bind(wx.EVT_SCROLLWIN_THUMBTRACK, self.OnThumbTrack)
        
        self.SetVirtualSize((400,SIZE))
        self.SetScrollRate(0, UNIT)
              
        
    def OnPaint(self, evt):
        w,h = self.GetClientSize()
        dc = wx.BufferedPaintDC(self)
        self.PrepareDC(dc)
        
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        dc.SetPen(wx.Pen("dark grey", 1))
        dc.SetFont(wx.FFont(8, wx.SWISS))
        y = 0
        while y < SIZE+1:
            dc.DrawLine(0, y, w, y)
            dc.DrawText(str(y/UNIT), 10, y+2)
            y += UNIT


    def OnLineUp(self, evt):
        print "up  ", self.GetViewStart()
        evt.Skip()

    def OnLineDown(self, evt):
        print "down", self.GetViewStart()
        evt.Skip()

    def OnThumbTrack(self, evt):
        print "thumb", self.GetViewStart()
        evt.Skip()


app = wx.App(False)
frm = wx.Frame(None)
win = TestWindow(frm)
frm.Show()
app.MainLoop()
