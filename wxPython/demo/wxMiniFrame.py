
from wxPython.wx import *

#---------------------------------------------------------------------------
class MyMiniFrame(wxMiniFrame):
    def __init__(self, parent, title, pos=wxDefaultPosition, size=wxDefaultSize,
                 style=wxDEFAULT_FRAME_STYLE ):
        wxMiniFrame.__init__(self, parent, -1, title, pos, size, style)
        panel = wxPanel(self, -1)

        button = wxButton(panel, -1, "Close Me")
        button.SetPosition(wxPoint(15, 15))
        EVT_BUTTON(self, button.GetId(), self.OnCloseMe)
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseMe(self, event):
        self.Close(True)

    def OnCloseWindow(self, event):
        print "OnCloseWindow"
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyMiniFrame(frame, "This is a wxMiniFrame",
                      #pos=(250,250), size=(200,200),
                      style=wxDEFAULT_FRAME_STYLE | wxTINY_CAPTION_HORIZ)
    win.SetSize((200, 200))
    win.CenterOnParent(wxBOTH)
    frame.otherWin = win
    win.Show(True)


#---------------------------------------------------------------------------










overview = """\
"""
