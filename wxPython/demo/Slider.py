
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wx.StaticText(self, -1, "This is a wx.Slider.", (45, 15))

        slider = wx.Slider(
            self, 100, 25, 1, 100, (30, 60), (250, -1), 
            wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS 
            )

        slider.SetTickFreq(5, 1)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------




overview = """\
A slider is a control with a handle which can be pulled back and forth to 
change the value.

In Windows versions below Windows 95, a scrollbar is used to simulate the slider. 
In Windows 95, the track bar control is used.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

