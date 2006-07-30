


import wx
from wx.lib import analogclock as ac


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        # A mostly default clock
        c1 = ac.AnalogClockWindow(self)
        c1.SetBackgroundColour("RED")
        c1.SetHandColours("BLUE")
        c1.SetTickColours("WHITE")
        c1.SetTickSizes(h=5, m=2)

        
        # A clock with roman numerals, shown only at the quarter
        # marks, and a separatly coloured watch face.
        c2 = ac.AnalogClockWindow(self)
        c2.SetBackgroundColour("WHITE")
        c2.SetHandColours("RED")
        c2.SetTickColours("BLUE")
        c2.SetTickStyles(ac.TICKS_ROMAN)
        c2.SetClockStyle(ac.SHOW_QUARTERS_TICKS | ac.SHOW_SHADOWS)
        c2.SetWatchPenBrush(p=wx.Pen((238, 238, 227), 1, wx.SOLID),
                            b=wx.Brush("CADET BLUE", wx.SOLID))
        c2.SetTickSizes(h=12)

        # A clock with rotated decimal numbers, shown at all twelve
        # hour marks
        c3 = ac.AnalogClockWindow(self)
        c3.SetBackgroundColour("BLUE")
        c3.SetHandColours("WHITE")
        c3.SetTickColours("RED")
        c3.SetTickStyles(h=ac.TICKS_DECIMAL)
        c3.SetClockStyle(ac.SHOW_HOURS_TICKS | ac.ROTATE_TICKS)
        c3.SetTickSizes(h=14)

        # a plain clock, with square hour and round minute marks, no
        # shadow raised border
        c4 = ac.AnalogClockWindow(self, style=wx.RAISED_BORDER)
        c4.SetTickStyles(h=ac.TICKS_SQUARE, m=ac.TICKS_CIRCLE)
        c4.SetClockStyle(ac.SHOW_HOURS_TICKS | ac.SHOW_MINUTES_TICKS)
        c4.SetTickSizes(h=5, m=2)


        # no minute tick marks
        c5 = ac.AnalogClockWindow(self)
        c5.SetTickStyles(h=ac.TICKS_CIRCLE)
        c5.SetClockStyle(ac.SHOW_HOURS_TICKS | ac.SHOW_SHADOWS | ac.ROTATE_TICKS)
        c5.SetTickSizes(h=5, m=2)

        # sunken border
        c6 = ac.AnalogClockWindow(self, style=wx.SUNKEN_BORDER)
        c6.SetTickSizes(h=5, m=2)


        # layout the clocks in a grid
        gs = wx.GridSizer(2, 3, 4, 4)
        gs.Add(c1, 0, wx.EXPAND)
        gs.Add(c2, 0, wx.EXPAND)
        gs.Add(c3, 0, wx.EXPAND)
        gs.Add(c4, 0, wx.EXPAND)
        gs.Add(c5, 0, wx.EXPAND)
        gs.Add(c6, 0, wx.EXPAND)

        # put it in another sizer for a border
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(gs, 1, wx.EXPAND | wx.ALL, 10)

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
<p>
Check the options available by right-clicking the clock.
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

