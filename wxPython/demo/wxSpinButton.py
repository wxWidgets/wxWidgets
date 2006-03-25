
from wxPython.wx import *

import string

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This example uses the wxSpinButton control.",
                               wxPoint(45, 15))


        self.text = wxTextCtrl(self, -1, "1", wxPoint(30, 50), wxSize(60, -1))
        h = self.text.GetSize().height
        self.spin = wxSpinButton(self, 20, wxPoint(92, 50), wxSize(h, h),
                                 wxSP_VERTICAL)
        self.spin.SetRange(1, 100)
        self.spin.SetValue(1)

        EVT_SPIN(self, 20, self.OnSpin)


    def OnSpin(self, event):
        self.text.SetValue(str(event.GetPosition()))


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""
