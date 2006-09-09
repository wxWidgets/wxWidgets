
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wx.StaticText(self, -1, "This example shows the wx.Gauge control.", (45, 15))

        self.g1 = wx.Gauge(self, -1, 50, (110, 50), (250, 25))
        self.g2 = wx.Gauge(self, -1, 50, (110, 95), (250, 25))

        self.Bind(wx.EVT_TIMER, self.TimerHandler)
        self.timer = wx.Timer(self)
        self.timer.Start(100)

    def TimerHandler(self, event):
        self.count = self.count + 1

        if self.count >= 50:
            self.count = 0

        self.g1.SetValue(self.count)
        self.g2.Pulse()
    


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
A Gauge is a horizontal or vertical bar which shows a quantity in a graphical
fashion. It is often used to indicate progress through lengthy tasks, such as
file copying or data analysis.

When the Gauge is initialized, it's "complete" value is usually set; at any rate,
before using the Gauge, the maximum value of the control must be set. As the task
progresses, the Gauge is updated by the program via the <code>SetValue</code> method.

This control is for use within a GUI; there is a seperate ProgressDialog class
to present the same sort of control as a dialog to the user.
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

