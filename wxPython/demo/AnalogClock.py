# AnalogClock demo
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   12 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

import wx
import wx.lib.analogclock as ac

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent)

        # A mostly default clock
        c1 = ac.AnalogClock(self, size=(200,200))

        if True: # for a simpler test case just set this to False and
                 # only the one clock will be created
            
            # A plain clock, with square hour and round minute marks, no
            # shadow, static border
            c2 = ac.AnalogClock(self, style=wx.STATIC_BORDER,
                                hoursStyle=ac.TICKS_SQUARE,
                                minutesStyle=ac.TICKS_CIRCLE,
                                clockStyle=ac.SHOW_HOURS_TICKS| \
                                           ac.SHOW_MINUTES_TICKS|
                                           ac.SHOW_HOURS_HAND| \
                                           ac.SHOW_MINUTES_HAND| \
                                           ac.SHOW_SECONDS_HAND)
            c2.SetTickSize(12, target=ac.HOUR)

            # No minute tick marks
            c3 = ac.AnalogClock(self, hoursStyle=ac.TICKS_CIRCLE,
                                clockStyle=ac.SHOW_HOURS_TICKS| \
                                           ac.SHOW_HOURS_HAND| \
                                           ac.SHOW_MINUTES_HAND| \
                                           ac.SHOW_SECONDS_HAND| \
                                           ac.SHOW_SHADOWS)
            c3.SetTickSize(12)

            # A clock with hex numbers no seconds hand and different colours.
            c4 = ac.AnalogClock(self, hoursStyle=ac.TICKS_HEX,
                                clockStyle=ac.SHOW_HOURS_TICKS| \
                                           ac.SHOW_HOURS_HAND| \
                                           ac.SHOW_MINUTES_HAND| \
                                           ac.SHOW_SHADOWS)
            colour = wx.Colour(0, 255, 255)
            c4.SetForegroundColour(colour)
            colour = wx.Colour(0, 132, 132)
            c4.SetShadowColour(colour)
            c4.SetTickFont(wx.Font(10, wx.FONTFAMILY_MODERN, wx.NORMAL, wx.BOLD))
            c4.SetBackgroundColour(wx.BLACK)
            c4.SetFaceBorderColour(wx.BLACK)
            c4.SetFaceFillColour(wx.BLACK)

            # A clock with binary numbers shown only at the quarter tick marks,
            # no minutes ticks and different colours.
            c5 = ac.AnalogClock(self, style = wx.RAISED_BORDER, 
                                hoursStyle=ac.TICKS_BINARY,
                                clockStyle=ac.SHOW_QUARTERS_TICKS| \
                                           ac.SHOW_HOURS_HAND| \
                                           ac.SHOW_MINUTES_HAND| \
                                           ac.SHOW_SECONDS_HAND| \
                                           ac.SHOW_SHADOWS)
            colour = wx.Colour(0, 128, 0)
            c5.SetHandFillColour(colour, target=ac.SECOND)
            c5.SetHandBorderColour(colour, target=ac.SECOND)
            c5.SetBackgroundColour(colour)
            colour = wx.Colour(128, 0, 64)
            c5.SetTickFillColour(colour)
            c5.SetFaceBorderColour(colour)
            c5.SetFaceBorderWidth(1)
            colour = wx.Colour(0, 198, 0)
            c5.SetFaceFillColour(colour)
            c5.SetShadowColour(wx.WHITE)

            # A clock with a sunken border, roman numerals shown only at the 
            # quarter tick marks with a roman font, circular minutes ticks,
            # no seconds hand, no shadows, tick overlapping and different colours.
            c6 = ac.AnalogClock(self, style = wx.SUNKEN_BORDER,
                                hoursStyle=ac.TICKS_ROMAN,
                                minutesStyle=ac.TICKS_CIRCLE,
                                clockStyle=ac.SHOW_QUARTERS_TICKS| \
                                           ac.SHOW_MINUTES_TICKS| \
                                           ac.SHOW_HOURS_HAND| \
                                           ac.SHOW_MINUTES_HAND| \
                                           ac.OVERLAP_TICKS)
            colour = wx.Colour(128, 0, 0)
            c6.SetHandFillColour(colour)
            colour = wx.Colour(179, 0, 89)
            c6.SetHandBorderColour(colour)
            c6.SetTickFillColour(colour)
            c6.SetTickBorderColour(colour)
            colour = wx.Colour(225, 255, 255)
            c6.SetFaceBorderColour(colour)
            c6.SetBackgroundColour(colour)
            colour = wx.Colour(249, 255, 255)
            c6.SetFaceFillColour(colour)
            colour = wx.Colour(255, 213, 213)
            c6.SetShadowColour(colour)
            c6.SetTickFont(wx.Font(10, wx.FONTFAMILY_ROMAN, wx.NORMAL, wx.BOLD))

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
            sizer.Add(gs, 1, wx.EXPAND|wx.ALL, 10)

            self.SetSizerAndFit(sizer)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html>
<PRE><FONT SIZE=-1>
""" + ac.__doc__.replace("<", "").replace(">", "") + """
</FONT></PRE>"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

