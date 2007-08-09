import sys, os
import wx

class MyApp(wx.App):
    def OnInit(self):
        f = wx.Frame(None, title="Hello World")
        f.Show()
        return True
    
    def MacOpenFile(self, filename):
        # code to load filename goes here.
        wx.MessageBox(
            "You requested to open this file:\n\"%s\"" % filename)

app = MyApp()
app.MainLoop()

