# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  time
import  wx

#---------------------------------------------------------------------------

## For your convenience; an example of creating your own timer class.
##
## class TestTimer(wxTimer):
##     def __init__(self, log = None):
##         wxTimer.__init__(self)
##         self.log = log
##     def Notify(self):
##         wxBell()
##         if self.log:
##             self.log.WriteText('beep!\n')

#---------------------------------------------------------------------------

ID_Start  = wx.NewId()
ID_Stop   = wx.NewId()
ID_Timer  = wx.NewId()
ID_Timer2 = wx.NewId()

class TestTimerWin(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        wx.StaticText(self, -1, "This is a timer example", (15, 30))
        wx.Button(self, ID_Start, ' Start ', (15, 75), wx.DefaultSize)
        wx.Button(self, ID_Stop, ' Stop ', (115, 75), wx.DefaultSize)

        self.timer = wx.Timer(self,      # object to send the event to
                              ID_Timer)  # event id to use

        self.timer2 = wx.Timer(self,      # object to send the event to
                              ID_Timer2)  # event id to use

        self.Bind(wx.EVT_BUTTON, self.OnStart, id=ID_Start)
        self.Bind(wx.EVT_BUTTON, self.OnStop, id=ID_Stop)
        self.Bind(wx.EVT_TIMER, self.OnTimer, id=ID_Timer)
        self.Bind(wx.EVT_TIMER, self.OnTimer2, id=ID_Timer2)

    def OnStart(self, event):
        self.timer.Start(1000)
        self.timer2.Start(1500)

    def OnStop(self, event):
        self.timer.Stop()
        self.timer2.Stop()

    def OnTimer(self, event):
        wx.Bell()
        if self.log:
            self.log.WriteText('beep!\n')

    def OnTimer2(self, event):
        wx.Bell()
        if self.log:
            self.log.WriteText('beep 2!\n')

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestTimerWin(nb, log)
    return win

#---------------------------------------------------------------------------



overview = """\
The wxTimer class allows you to execute code at specified intervals from
within the wxPython event loop. Timers can be one-shot or repeating.

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
