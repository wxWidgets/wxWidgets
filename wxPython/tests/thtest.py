
from wxPython.wx import *

class someData:
    def __init__(self, data="spam"):
        self.data = data

class errApp(wxApp):
    def OnInit(self):
        frame = wxFrame(NULL, -1, "Close to get an error", wxDefaultPosition,
                        wxSize(200,200))

        tree = wxTreeCtrl(frame, -1, wxDefaultPosition, wxDefaultSize)

        root = tree.AddRoot("Spam")
        tree.SetPyData(root, someData())
        #tree.SetPyData(root, "A string")
        #tree.SetPyData(root, ["a list", "A string"])

        frame.Show(true)
        self.SetTopWindow(frame)
        self.frame = frame
        return true


app = errApp(0)
app.MainLoop()
print "got to the end"

