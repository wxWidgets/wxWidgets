
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        wxStaticText(self, -1, "wxTextCtrl", wxPoint(5, 25), wxSize(75, 20))
        t = wxTextCtrl(self, 10, "Test it out and see", wxPoint(80, 25), wxSize(150, 20))
        t.SetInsertionPoint(0)
        EVT_TEXT(self, 10, self.EvtText)

        wxStaticText(self, -1, "Passsword", wxPoint(5, 50), wxSize(75, 20))
        wxTextCtrl(self, 20, "", wxPoint(80, 50), wxSize(150, 20), wxTE_PASSWORD)
        EVT_TEXT(self, 20, self.EvtText)

        wxStaticText(self, -1, "Multi-line", wxPoint(5, 75), wxSize(75, 20))
        t = wxTextCtrl(self, 30, "How does it work with a long line of text set in the control", wxPoint(80, 75), wxSize(200, 150), wxTE_MULTILINE)
        t.SetInsertionPoint(0)
        EVT_TEXT(self, 30, self.EvtText)

    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
"""
