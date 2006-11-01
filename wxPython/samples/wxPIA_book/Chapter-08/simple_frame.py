import wx

if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = wx.Frame(None, -1, "A Frame", style=wx.DEFAULT_FRAME_STYLE,
        size=(200, 100))
    frame.Show()
    app.MainLoop()
