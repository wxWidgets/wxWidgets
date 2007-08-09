import wx
from blockwindow import BlockWindow

labels = "one two three four five six seven eight nine".split()

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "StaticBoxSizer Test")
        self.panel = wx.Panel(self)

        # make three static boxes with windows positioned inside them
        box1 = self.MakeStaticBoxSizer("Box 1", labels[0:3])
        box2 = self.MakeStaticBoxSizer("Box 2", labels[3:6])
        box3 = self.MakeStaticBoxSizer("Box 3", labels[6:9])

        # We can also use a sizer to manage the placement of other
        # sizers (and therefore the windows and sub-sizers that they
        # manage as well.)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(box1, 0, wx.ALL, 10)
        sizer.Add(box2, 0, wx.ALL, 10)
        sizer.Add(box3, 0, wx.ALL, 10)
        
        self.panel.SetSizer(sizer)
        sizer.Fit(self)


    def MakeStaticBoxSizer(self, boxlabel, itemlabels):
        # first the static box
        box = wx.StaticBox(self.panel, -1, boxlabel)

        # then the sizer
        sizer = wx.StaticBoxSizer(box, wx.VERTICAL)

        # then add items to it like normal
        for label in itemlabels:
            bw = BlockWindow(self.panel, label=label)
            sizer.Add(bw, 0, wx.ALL, 2)

        return sizer


app = wx.PySimpleApp()
TestFrame().Show()
app.MainLoop()
