import wx

class TwoButtonEvent(wx.PyCommandEvent):
    def __init__(self, evtType, id):
        wx.PyCommandEvent.__init__(self, evtType, id)
        self.clickCount = 0

    def GetClickCount(self):
        return self.clickCount

    def SetClickCount(self, count):
        self.clickCount = count

myEVT_TWO_BUTTON = wx.NewEventType()
EVT_TWO_BUTTON = wx.PyEventBinder(myEVT_TWO_BUTTON, 1)

class TwoButtonPanel(wx.Panel):
    def __init__(self, parent, id=-1, leftText="Left",
            rightText="Right"):
        wx.Panel.__init__(self, parent, id)
        self.leftButton = wx.Button(self, label=leftText)
        self.rightButton = wx.Button(self, label=rightText,
                                     pos=(100,0))
        self.leftClick = False
        self.rightClick = False
        self.clickCount = 0
        self.leftButton.Bind(wx.EVT_LEFT_DOWN, self.OnLeftClick)
        self.rightButton.Bind(wx.EVT_LEFT_DOWN, self.OnRightClick)

    def OnLeftClick(self, event):
        self.leftClick = True
        self.OnClick()
        event.Skip()

    def OnRightClick(self, event):
        self.rightClick = True
        self.OnClick()
        event.Skip()

    def OnClick(self):
        self.clickCount += 1
        if self.leftClick and self.rightClick:
            self.leftClick = False
            self.rightClick = False
            evt = TwoButtonEvent(myEVT_TWO_BUTTON, self.GetId())
            evt.SetClickCount(self.clickCount)
            self.GetEventHandler().ProcessEvent(evt)


class CustomEventFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'Click Count: 0',
                          size=(300, 100))
        panel = TwoButtonPanel(self)
        self.Bind(EVT_TWO_BUTTON, self.OnTwoClick, panel)

    def OnTwoClick(self, event):
        self.SetTitle("Click Count: %s" % event.GetClickCount())

if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = CustomEventFrame(parent=None, id=-1)
    frame.Show()
    app.MainLoop()
