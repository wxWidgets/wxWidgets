
from wxPython.wx import *

#---------------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self, parent, ID, title, pos=wxDefaultPosition,
                 size=wxDefaultSize, style=wxDEFAULT_FRAME_STYLE):
        wxFrame.__init__(self, parent, ID, title, pos, size, style)
        panel = wxPanel(self, -1)

        button = wxButton(panel, 1003, "Close Me")
        button.SetPosition(wxPoint(15, 15))
        EVT_BUTTON(self, 1003, self.OnCloseMe)
        EVT_CLOSE(self, self.OnCloseWindow)


    def OnCloseMe(self, event):
        self.Close(True)

    def OnCloseWindow(self, event):
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyFrame(frame, -1, "This is a wxFrame", size=(350, 200),
                  style = wxDEFAULT_FRAME_STYLE)# |  wxFRAME_TOOL_WINDOW )
    frame.otherWin = win
    win.Show(True)


#---------------------------------------------------------------------------










overview = """\
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

