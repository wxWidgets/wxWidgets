import wx
import time

print wx.version()


def OnButton(evt):
    bi = wx.BusyInfo("This is a wx.BusyInfo widget...")
    
    # This yield is needed on wxGTK in order to see the text.  Is
    # there a way around this?
    if "wxGTK" in wx.PlatformInfo:
        wx.Yield(); wx.Yield()
        
    time.sleep(5)


app = wx.App()
frm = wx.Frame(None)
pnl = wx.Panel(frm)
btn = wx.Button(pnl, label="Click me", pos=(10,10))
btn.Bind(wx.EVT_BUTTON, OnButton)
frm.Show()
app.MainLoop()
