
from wxPython.wx import *

#---------------------------------------------------------------------------
class MyMiniFrame(wxMiniFrame):
    def __init__(self, parent, title, style):
        wxMiniFrame.__init__(self, parent, -1, title, style=style)
        panel = wxPanel(self, -1)

        button = wxButton(panel, 1003, "Close Me")
        button.SetPosition(wxPoint(15, 15))
        EVT_BUTTON(self, 1003, self.OnCloseMe)
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseMe(self, event):
        self.Close(true)

    def OnCloseWindow(self, event):
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyMiniFrame(frame, "This is a wxMiniFrame",
                      wxDEFAULT_FRAME_STYLE | wxTINY_CAPTION_HORIZ)
    win.SetSize((200, 200))
    win.CenterOnParent(wxBOTH)
    frame.otherWin = win
    win.Show(true)


#---------------------------------------------------------------------------










overview = """\
"""
