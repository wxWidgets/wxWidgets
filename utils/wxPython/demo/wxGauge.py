
from wxPython.wx import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This example uses the wxGauge control.",
                               wxPoint(45, 15))

        #self.g1 = wxGauge(self, -1, 50, wxPoint(40, 50), wxSize(40, 160),
        #                  wxGA_VERTICAL)
        #self.g1.SetBezelFace(3)
        #self.g1.SetShadowWidth(3)

        self.g2 = wxGauge(self, -1, 50, wxPoint(110, 50), wxSize(250, 25),
                          wxGA_HORIZONTAL)
        self.g2.SetBezelFace(5)
        self.g2.SetShadowWidth(5)

        EVT_IDLE(self, self.IdleHandler)


    def IdleHandler(self, event):
        self.count = self.count + 1
        if self.count >= 50:
            self.count = 0
        #self.g1.SetValue(self.count)
        self.g2.SetValue(self.count)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""
