import wx
from blockwindow import BlockWindow

labels = "one two three four five six seven eight nine".split()
flags = {"one": wx.BOTTOM, "two": wx.ALL, "three": wx.TOP, 
         "four": wx.LEFT, "five": wx.ALL, "six": wx.RIGHT, 
         "seven": wx.BOTTOM | wx.TOP, "eight": wx.ALL,
         "nine": wx.LEFT | wx.RIGHT}

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "GridSizer Borders")
        sizer = wx.GridSizer(rows=3, cols=3, hgap=5, vgap=5)
        for label in labels:
            bw = BlockWindow(self, label=label)
            flag = flags.get(label, 0)
            sizer.Add(bw, 0, flag, 10)
        self.SetSizer(sizer)
        self.Fit()

app = wx.PySimpleApp()
TestFrame().Show()
app.MainLoop()
