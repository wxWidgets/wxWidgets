# 
# 1/11/2004 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o It appears that wx.Timer has an issue where if you use
#
#       self.timer = wx.Timer(self, -1)
#
#   to create it, then
#
#       self.timer.GetId()
#
#   doesn't seem to return anything meaningful. In the demo, doing this
#   results in only one of the two handlers being called for both timers.
#   This means that
#
#       self.Bind(wx.EVT_TIMER, self.onTimer, self.timer)
#
#   doesn't work right. However, using
#
#       self.timer = wx.Timer(self, wx.NewId())
#
#   makes it work OK. I believe this is a bug, but wiser heads than mine
#   should determine this.
#

import  time
import  wx

#---------------------------------------------------------------------------

## For your convenience; an example of creating your own timer class.
##
## class TestTimer(wx.Timer):
##     def __init__(self, log = None):
##         wx.Timer.__init__(self)
##         self.log = log
##     def Notify(self):
##         wx.Bell()
##         if self.log:
##             self.log.WriteText('beep!\n')

#---------------------------------------------------------------------------

class TestTimerWin(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        wx.StaticText(self, -1, "This is a timer example", (15, 30))
        startBtn = wx.Button(self, -1, ' Start ', (15, 75), wx.DefaultSize)
        stopBtn = wx.Button(self, -1, ' Stop ', (115, 75), wx.DefaultSize)

        self.timer = wx.Timer(self, wx.NewId())
        self.timer2 = wx.Timer(self, wx.NewId())  

        self.Bind(wx.EVT_BUTTON, self.OnStart, startBtn)
        self.Bind(wx.EVT_BUTTON, self.OnStop, stopBtn)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)
        self.Bind(wx.EVT_TIMER, self.OnTimer2, self.timer2)

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
The wx.Timer class allows you to execute code at specified intervals from
within the wxPython event loop. Timers can be one-shot or repeating.

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
