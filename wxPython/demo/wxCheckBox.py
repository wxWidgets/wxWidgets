
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestCheckBox(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This example uses the wxCheckBox control.",
                               wxPoint(10, 10))

        cID = wxNewId()
        cb1 = wxCheckBox(self, cID,   "  Apples", wxPoint(65, 40), wxSize(150, 20), wxNO_BORDER)
        cb2 = wxCheckBox(self, cID+1, "  Oranges", wxPoint(65, 60), wxSize(150, 20), wxNO_BORDER)
        cb2.SetValue(True)
        cb3 = wxCheckBox(self, cID+2, "  Pears", wxPoint(65, 80), wxSize(150, 20), wxNO_BORDER)

        EVT_CHECKBOX(self, cID,   self.EvtCheckBox)
        EVT_CHECKBOX(self, cID+1, self.EvtCheckBox)
        EVT_CHECKBOX(self, cID+2, self.EvtCheckBox)


    def EvtCheckBox(self, event):
        self.log.WriteText('EvtCheckBox: %d\n' % event.Checked())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestCheckBox(nb, log)
    return win

#---------------------------------------------------------------------------















overview = """\
A checkbox is a labelled box which is either on (checkmark is visible) or off (no checkmark).

"""
