
from wxPython.wx import *

import time

#---------------------------------------------------------------------------

## class TestTimer(wxTimer):
##     def __init__(self, log = None):
##         wxTimer.__init__(self)
##         self.log = log
##     def Notify(self):
##         wxBell()
##         if self.log:
##             self.log.WriteText('beep!\n')

#---------------------------------------------------------------------------

ID_Start = wxNewId()
ID_Stop  = wxNewId()
ID_Timer = wxNewId()

class TestTimerWin(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        wxStaticText(self, -1, "This is a timer example",
                               wxPoint(15, 30))

        wxButton(self, ID_Start, ' Start ', wxPoint(15, 75), wxDefaultSize)
        wxButton(self, ID_Stop, ' Stop ', wxPoint(115, 75), wxDefaultSize)

        self.timer = wxTimer(self,      # object to send the event to
                             ID_Timer)  # event id to use

        EVT_BUTTON(self, ID_Start, self.OnStart)
        EVT_BUTTON(self, ID_Stop,  self.OnStop)
        EVT_TIMER(self,  ID_Timer, self.OnTimer)


    def OnStart(self, event):
        self.timer.Start(1000)

    def OnStop(self, event):
        self.timer.Stop()

    def OnTimer(self, event):
        wxBell()
        if self.log:
            self.log.WriteText('beep!\n')

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestTimerWin(nb, log)
    return win

#---------------------------------------------------------------------------











overview = """\
The wxTimer class allows you to execute code at specified intervals.

"""









