
from wxPython.wx import *
from wxPython.gizmos import *

import time

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        led = wxLEDNumberCtrl(self, -1, (25,25), (280, 50))
        led.SetValue("01234")

        led = wxLEDNumberCtrl(self, -1, (25,100), (280, 50))
        led.SetValue("56789")
        led.SetAlignment(wxLED_ALIGN_RIGHT)
        led.SetDrawFaded(False)

        led = wxLEDNumberCtrl(self, -1, (25,175), (280, 50),
                              wxLED_ALIGN_CENTER)# | wxLED_DRAW_FADED)
        self.clock = led
        self.OnTimer(None)

        self.timer = wxTimer(self)
        self.timer.Start(1000)
        EVT_TIMER(self, -1, self.OnTimer)


    def OnTimer(self, evt):
        t = time.localtime(time.time())
        st = time.strftime("%I-%M-%S", t)
        self.clock.SetValue(st)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """\
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

