
from wxPython.wx import *

import string

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This example uses the wxSpinCtrl control.",
                               wxPoint(45, 15))

        sc = wxSpinCtrl(self, -1, "", wxPoint(30, 50), wxSize(80, -1))
        sc.SetRange(1,100)
        sc.SetValue(5)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------







overview = """\
"""
