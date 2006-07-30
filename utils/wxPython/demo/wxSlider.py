
from wxPython.wx import *

import string

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This is a wxSlider.", wxPoint(45, 15))

        slider = wxSlider(self, 100, 25, 1, 100, wxPoint(30, 60),
                          wxSize(250, -1),
                          wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS )
        slider.SetTickFreq(5, 1)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""
