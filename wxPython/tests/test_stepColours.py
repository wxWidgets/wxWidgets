import wx
import wx.lib.colourselect as cs
import wx.lib.imageutils as iu

class TestPanel(wx.Panel):
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)

        self.colour = wx.NamedColour("purple")
        
        self.cpnl = wx.Panel(self, size=(100,100), style=wx.SIMPLE_BORDER)
        self.slider = wx.Slider(self,
                                minValue=0,
                                value=100,
                                maxValue=200,
                                size=(300, -1),
                                style=wx.SL_HORIZONTAL | wx.SL_LABELS)
        csel = cs.ColourSelect(self, colour=self.colour)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.cpnl, 0, wx.ALL, 20)
        sizer.Add(self.slider, 0, wx.LEFT, 20)
        sizer.Add(csel, 0, wx.ALL, 20)
        self.SetSizer(sizer)

        self.slider.Bind(wx.EVT_SCROLL, self.OnSliderChanged)
        self.Bind(cs.EVT_COLOURSELECT, self.OnColourSelect)

        self.UpdatePanel()


    def UpdatePanel(self):
        val = self.slider.GetValue()

        colour = iu.stepColour(self.colour, val)

        self.cpnl.SetBackgroundColour(colour)
        self.cpnl.Refresh()
        

    def OnSliderChanged(self, evt):
        self.UpdatePanel()


    def OnColourSelect(self, evt):
        self.colour = evt.GetValue()
        self.UpdatePanel()



app = wx.App()
frm = wx.Frame(None, title="Stepping Colours")
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()

