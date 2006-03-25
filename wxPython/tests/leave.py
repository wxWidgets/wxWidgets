from wxPython.wx import *

class TestFrame(wxFrame):

    def __init__(self):
        wxFrame.__init__(self,NULL,-1,"Test Frame",wxPoint(200,200))
        win = wxWindow(self, -1)
        self.Show(true)
        EVT_LEAVE_WINDOW(win, self.onLeave)
        EVT_ENTER_WINDOW(win, self.onEnter)

    def onLeave(self, event):
        print("out")

    def onEnter(self, event):
        print('in')

class MyApp(wxApp):

    def OnInit(self):
        self.mainFrame = TestFrame()
        return true

app = MyApp(0)
app.MainLoop()

