
from wxPython.wx import *
from Main import opj

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        b = wxButton(self, -1, "Play Sound 1", (25, 25))
        EVT_BUTTON(self, b.GetId(), self.OnButton1)

        b = wxButton(self, -1, "Play Sound 2", (25, 65))
        EVT_BUTTON(self, b.GetId(), self.OnButton2)


    def OnButton1(self, evt):
        try:
            wave = wxWave(opj('data/anykey.wav'))
            wave.Play()
        except NotImplementedError, v:
            wxMessageBox(str(v), "Exception Message")


    def OnButton2(self, evt):
        try:
            wave = wxWave(opj('data/plan.wav'))
            wave.Play()
        except NotImplementedError, v:
            wxMessageBox(str(v), "Exception Message")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win

#----------------------------------------------------------------------


overview = """\
"""
