# 11/4/03 - grimmtooth@softhome.net (Jeff Grimmett)
# 
# o wx Namespace 
#

import  wx
import  wx.lib.analogclock  as  aclock

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        # A red background with blue hands and white markings
        c1 = aclock.AnalogClockWindow(self)
        c1.SetBackgroundColour("RED")
        c1.SetHandsColour("BLUE")
        c1.SetTickMarkColours("WHITE")

        # A white background with red hands and blue markings
        c2 = aclock.AnalogClockWindow(self)
        c2.SetBackgroundColour("WHITE")
        c2.SetHandsColour("RED")
        c2.SetTickMarkColours("BLUE")

        # A blue background with white hands and red markings
        c3 = aclock.AnalogClockWindow(self)
        c3.SetBackgroundColour("BLUE")
        c3.SetHandsColour("WHITE")
        c3.SetTickMarkColours("RED")

        # Raised border, circular tick marks.
        c4 = aclock.AnalogClockWindow(self, style=wx.RAISED_BORDER)
        c4.SetTickMarkStyle(aclock.AnalogClockWindow.TICKS_CIRCLE)

        # No tick marks
        c5 = aclock.AnalogClockWindow(self)
        c5.SetTickMarkStyle(aclock.AnalogClockWindow.TICKS_NONE)

        # Sunken into window
        c6 = aclock.AnalogClockWindow(self, style=wx.SUNKEN_BORDER)

        # layout the clocks in a grid sizer
        gs = wx.GridSizer(2, 3, 4, 4)
        gs.Add(c1, 0, wx.EXPAND)
        gs.Add(c2, 0, wx.EXPAND)
        gs.Add(c3, 0, wx.EXPAND)
        gs.Add(c4, 0, wx.EXPAND)
        gs.Add(c5, 0, wx.EXPAND)
        gs.Add(c6, 0, wx.EXPAND)

        # put it in another sizer for a border
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(gs, 1, wx.EXPAND|wx.ALL, 10)

        self.SetSizer(sizer)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>AnalogClockWindow</center></h2>

This is a nice little clock class that was contributed to by several
members of the wxPython-users group.

</body></html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

