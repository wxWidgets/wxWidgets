
from wxPython.wx import *

#---------------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self, parent, ID, title, pos, size):
        wxFrame.__init__(self, parent, ID, title, pos, size)
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
    win = MyFrame(frame, -1, "This is a wxFrame", wxDefaultPosition, wxSize(350, 200))
    frame.otherWin = win
    win.Show(true)


#---------------------------------------------------------------------------










overview = """\
"""
