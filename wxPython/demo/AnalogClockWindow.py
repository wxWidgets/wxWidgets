


from wxPython.wx import *
from wxPython.lib.analogclock import AnalogClockWindow

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        c1 = AnalogClockWindow(self)
        c1.SetBackgroundColour("RED")
        c1.SetHandsColour("BLUE")
        c1.SetTickMarkColours("WHITE")

        c2 = AnalogClockWindow(self)
        c2.SetBackgroundColour("WHITE")
        c2.SetHandsColour("RED")
        c2.SetTickMarkColours("BLUE")

        c3 = AnalogClockWindow(self)
        c3.SetBackgroundColour("BLUE")
        c3.SetHandsColour("WHITE")
        c3.SetTickMarkColours("RED")

        c4 = AnalogClockWindow(self, style=wxRAISED_BORDER)
        c4.SetTickMarkStyle(AnalogClockWindow.TICKS_CIRCLE)

        c5 = AnalogClockWindow(self)
        c5.SetTickMarkStyle(AnalogClockWindow.TICKS_NONE)

        c6 = AnalogClockWindow(self, style=wxSUNKEN_BORDER)


        # layout the clocks in a grid
        gs = wxGridSizer(2, 3, 4, 4)
        gs.Add(c1, 0, wxEXPAND)
        gs.Add(c2, 0, wxEXPAND)
        gs.Add(c3, 0, wxEXPAND)
        gs.Add(c4, 0, wxEXPAND)
        gs.Add(c5, 0, wxEXPAND)
        gs.Add(c6, 0, wxEXPAND)

        # put it in another sizer for a border
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(gs, 1, wxEXPAND|wxALL, 10)

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

