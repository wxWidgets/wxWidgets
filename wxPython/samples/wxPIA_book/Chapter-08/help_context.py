import wx

class HelpFrame(wx.Frame):

    def __init__(self):
        pre = wx.PreFrame()
        pre.SetExtraStyle(wx.FRAME_EX_CONTEXTHELP)
        pre.Create(None, -1, "Help Context", size=(300, 100),
                style=wx.DEFAULT_FRAME_STYLE ^
                (wx.MINIMIZE_BOX | wx.MAXIMIZE_BOX))
        self.PostCreate(pre)

if __name__ == '__main__':
    app = wx.PySimpleApp()
    HelpFrame().Show()
    app.MainLoop()
