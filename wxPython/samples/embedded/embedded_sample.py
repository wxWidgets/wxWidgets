import wx
from wx.py import shell, version

class MyPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)

        text = wx.StaticText(self, -1,
                            "Everything on this side of the splitter comes from Python.")
        
        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % version.VERSION
        pycrust = shell.Shell(self, -1, introText=intro)
        #pycrust = wx.TextCtrl(self, -1, intro)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text, 0, wx.EXPAND|wx.ALL, 10)
        sizer.Add(pycrust, 1, wx.EXPAND|wx.BOTTOM|wx.LEFT|wx.RIGHT, 10)

        self.SetSizer(sizer)

