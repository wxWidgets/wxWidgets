import wx
from blockwindow import BlockWindow

labels = "one two three four".split()

class TestFrame(wx.Frame):
    title = "none"
    def __init__(self):
        wx.Frame.__init__(self, None, -1, self.title)
        sizer = self.CreateSizerAndWindows()
        self.SetSizer(sizer)
        self.Fit()
        
class VBoxSizerFrame(TestFrame):
    title = "Vertical BoxSizer"
    
    def CreateSizerAndWindows(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        for label in labels:
            bw = BlockWindow(self, label=label, size=(200,30))
            sizer.Add(bw, flag=wx.EXPAND)
        return sizer
    

class HBoxSizerFrame(TestFrame):
    title = "Horizontal BoxSizer"

    def CreateSizerAndWindows(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        for label in labels:
            bw = BlockWindow(self, label=label, size=(75,30))
            sizer.Add(bw, flag=wx.EXPAND)
        return sizer

class VBoxSizerStretchableFrame(TestFrame):
    title = "Stretchable BoxSizer"

    def CreateSizerAndWindows(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        for label in labels:
            bw = BlockWindow(self, label=label, size=(200,30))
            sizer.Add(bw, flag=wx.EXPAND)

        # Add an item that takes all the free space
        bw = BlockWindow(self, label="gets all free space", size=(200,30))
        sizer.Add(bw, 1, flag=wx.EXPAND)
        return sizer

class VBoxSizerMultiProportionalFrame(TestFrame):
    title = "Proportional BoxSizer"
    
    def CreateSizerAndWindows(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        for label in labels:
            bw = BlockWindow(self, label=label, size=(200,30))
            sizer.Add(bw, flag=wx.EXPAND)

        # Add an item that takes one share of the free space
        bw = BlockWindow(self, 
                label="gets 1/3 of the free space", 
                size=(200,30))
        sizer.Add(bw, 1, flag=wx.EXPAND)

        # Add an item that takes 2 shares of the free space
        bw = BlockWindow(self, 
                label="gets 2/3 of the free space", 
                size=(200,30))
        sizer.Add(bw, 2, flag=wx.EXPAND)
        return sizer

app = wx.PySimpleApp()
frameList = [VBoxSizerFrame, HBoxSizerFrame, 
             VBoxSizerStretchableFrame, 
             VBoxSizerMultiProportionalFrame]
for klass in frameList:
    frame = klass()
    frame.Show()
app.MainLoop()
