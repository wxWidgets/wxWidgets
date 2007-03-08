import wx

styles = [
    ('wx.BORDER_DEFAULT', wx.BORDER_DEFAULT),
    ('wx.BORDER_THEME', wx.BORDER_THEME),
    ('wx.BORDER_NONE',    wx.BORDER_NONE),
    ('wx.BORDER_STATIC',  wx.BORDER_STATIC),
    ('wx.BORDER_SIMPLE',  wx.BORDER_SIMPLE),
    ('wx.BORDER_RAISED',  wx.BORDER_RAISED),
    ('wx.BORDER_SUNKEN',  wx.BORDER_SUNKEN),
    ('wx.BORDER_DOUBLE',  wx.BORDER_DOUBLE),   
    ]

class TestPanel(wx.Panel):
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)
        fgs = wx.FlexGridSizer(cols=3, hgap=10, vgap=10)
        
        for name, bdr in styles:
            txt = wx.TextCtrl(self, size=(100,-1), style=bdr )#| wx.TE_RICH | wx.TE_MULTILINE)
            label = wx.StaticText(self, -1, name)
            ctrl = wx.Control(self, -1, style=bdr)
            ctrl.SetMinSize(txt.GetSize())
            ctrl.SetBackgroundColour("white")
            #txt.SetBackgroundColour("pink")
            fgs.Add(label)
            fgs.Add(ctrl)
            fgs.Add(txt)


        box = wx.BoxSizer()
        box.Add(fgs, 0, wx.ALL, 10)
        self.SetSizer(box)
        self.Fit()
        

import wx.lib.mixins.inspection
class TestApp(wx.App, wx.lib.mixins.inspection.InspectionMixin):
    pass

app = TestApp(False)
app.Init()
frm = wx.Frame(None, title="border styles")
pnl = TestPanel(frm)
frm.Fit()
frm.Show()
app.MainLoop()


