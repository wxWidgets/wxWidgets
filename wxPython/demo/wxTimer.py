
from wxPython.wx import *

import time

#---------------------------------------------------------------------------

class TestTimer(wxTimer):
    def __init__(self, log = None):
        wxTimer.__init__(self)
        self.log = log

    def Notify(self):
        wxBell()
        if self.log:
            self.log.WriteText('beep!\n')

#---------------------------------------------------------------------------

_timer = TestTimer()


class TestTimerWin(wxPanel):
    def __init__(self, parent, log):
        _timer.log = log
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This is a timer example",
                               wxPoint(15, 30))

        wxButton(self, 11101, ' Start ', wxPoint(15, 75), wxDefaultSize)
        wxButton(self, 11102, ' Stop ', wxPoint(115, 75), wxDefaultSize)
        EVT_BUTTON(self, 11101, self.OnStart)
        EVT_BUTTON(self, 11102, self.OnStop)

    def OnStart(self, event):
        _timer.Start(1000)

    def OnStop(self, event):
        _timer.Stop()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestTimerWin(nb, log)
    return win

#---------------------------------------------------------------------------











overview = """\
The wxTimer class allows you to execute code at specified intervals. To use it, derive a new class and override the Notify member to perform the required action. Start with Start, stop with Stop, it's as simple as that.

wxTimer()
------------------

Constructor.
"""
