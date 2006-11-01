import wx

class Frame(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, title="ProgressDialog sample")
        self.progressMax = 100
        self.count = 0
        self.dialog = None
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)
        self.timer.Start(1000)

    def OnTimer(self, evt):
        if not self.dialog:
            self.dialog = wx.ProgressDialog("A progress box", "Time remaining",
                                            self.progressMax,
                                            style=wx.PD_CAN_ABORT
                                            | wx.PD_ELAPSED_TIME
                                            | wx.PD_REMAINING_TIME)

        self.count += 1
        keepGoing = self.dialog.Update(self.count)
        if not keepGoing or self.count == self.progressMax:
            self.dialog.Destroy()
            self.timer.Stop()
        

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = Frame(None)
    frame.Show()
    app.MainLoop()
    
