
import  wx

#---------------------------------------------------------------------------
class MyMiniFrame(wx.MiniFrame):
    def __init__(
        self, parent, title, pos=wx.DefaultPosition, size=wx.DefaultSize,
        style=wx.DEFAULT_FRAME_STYLE 
        ):

        wx.MiniFrame.__init__(self, parent, -1, title, pos, size, style)
        panel = wx.Panel(self, -1)

        button = wx.Button(panel, -1, "Close Me")
        button.SetPosition((15, 15))
        self.Bind(wx.EVT_BUTTON, self.OnCloseMe, button)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def OnCloseMe(self, event):
        self.Close(True)

    def OnCloseWindow(self, event):
        print "OnCloseWindow"
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyMiniFrame(frame, "This is a wx.MiniFrame",
                      #pos=(250,250), size=(200,200),
                      style=wx.DEFAULT_FRAME_STYLE | wx.TINY_CAPTION_HORIZ)
    win.SetSize((200, 200))
    win.CenterOnParent(wx.BOTH)
    frame.otherWin = win
    win.Show(True)


#---------------------------------------------------------------------------


overview = """\
A MiniFrame is a Frame with a small title bar. It is suitable for floating 
toolbars that must not take up too much screen area. In other respects, it's the 
same as a wx.Frame.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
