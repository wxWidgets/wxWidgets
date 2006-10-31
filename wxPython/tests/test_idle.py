
import wx

class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.gauge = wx.Gauge(self, range=100, pos=(20,20), size=(100,-1))
        self.Bind(wx.EVT_IDLE, self.OnIdle)
        self.count = 1
        self.skipNext = False
        
    def OnIdle(self, evt):
        if self.skipNext:
            self.skipNext = False
            return
        self.skipNext = True
        
        print "OnIdle:", self.count
        #self.gauge.SetValue(self.count)
        self.count += 1
        if self.count >= 100:
            self.count = 1



app = wx.App(False)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
