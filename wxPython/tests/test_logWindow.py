import wx
print wx.VERSION


class Frame(wx.Frame):
    def __init__(self, *args, **kw):
        wx.Frame.__init__(self, *args, **kw)
        self.log = wx.LogWindow(self, "Program Log", False, False)

        pnl = wx.Panel(self)
        btn1 = wx.Button(pnl, -1, "Toggle Log")
        btn2 = wx.Button(pnl, -1, "Log Message")
        btn3 = wx.Button(pnl, -1, "Close")

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(btn1, 0, wx.ALL, 10)
        sizer.Add(btn2, 0, wx.ALL, 10)
        sizer.Add(btn3, 0, wx.ALL, 10)
        pnl.SetSizer(sizer)

        self.Bind(wx.EVT_BUTTON, self.OnToggleLog,  btn1)
        self.Bind(wx.EVT_BUTTON, self.OnLogMessage, btn2)
        self.Bind(wx.EVT_BUTTON, self.OnDoClose,    btn3)
        self.Bind(wx.EVT_CLOSE, self.OnClose)

        
    def OnClose(self, evt):
        self.log.this.disown()
        wx.Log.SetActiveTarget(None)
        self.Destroy()
            
        
    def OnToggleLog(self, evt):
        self.log.Show(not self.log.GetFrame().IsShown())
    
    def OnLogMessage(self, evt):
        wx.LogMessage("This is a test message")
    
    def OnDoClose(self, evt):
        self.Close()


app = wx.App(False)
frm = Frame(None, title="Testing wx.LogWindow")
frm.Show()
app.MainLoop()
    
