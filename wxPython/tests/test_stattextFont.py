import wx

def doStuff(stattxt):
    print stattxt.GetSize(), stattxt.GetBestSize()
    stattxt.SetFont(wx.FFont(24, wx.SWISS))
    print stattxt.GetSize(), stattxt.GetBestSize()

app = wx.App(False)
f = wx.Frame(None)
p = wx.Panel(f)
f.Show()
s = wx.StaticText(p, -1, "This is a test", (20,20))
wx.CallLater(2000, doStuff, s)
app.MainLoop()

