
from wxPython.wx import *
from Main import opj

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        b = wxButton(self, -1, "Play Sound", (25, 25))
        EVT_BUTTON(self, b.GetId(), self.OnButton)


    def OnButton(self, evt):
        try:
            import time
            if int(time.time()) % 2 == 1:
                wave = wxWave(opj('data/anykey.wav'))
            else:
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
