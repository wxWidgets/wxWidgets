#----------------------------------------------------------------------
# Name:        wxPython.lib.analogclock
# Purpose:     A simple analog clock window
#
# Author:      several folks on wxPython-users
#
# Created:     16-April-2003
# RCS-ID:      $Id$
# Copyright:   (c) 2003 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import math, sys, string, time
from wxPython.wx import *



class AnalogClockWindow(wxWindow):
    """A simple analog clock window"""

    TICKS_NONE   = 0
    TICKS_SQUARE = 1
    TICKS_CIRCLE = 2

    def __init__(self, parent, ID=-1, pos=wxDefaultPosition, size=wxDefaultSize,
                 style=0, name="clock"):
        # Initialize the wxWindow...
        wxWindow.__init__(self, parent, ID, pos, size, style, name)

        # Initialize the default clock settings...
        self.minuteMarks = 60
        self.hourMarks = 12
        self.tickMarksBrushC = self.GetForegroundColour()
        self.tickMarksPenC   = self.GetForegroundColour()
        self.tickMarkStyle = self.TICKS_SQUARE

        # Make an initial bitmap for the face, it will be updated and
        # painted at the first EVT_SIZE event.
        W, H = size
        self.faceBitmap = wxEmptyBitmap(max(W,1), max(H,1))

        # Initialize the timer that drives the update of the clock
        # face.  Update every half second to ensure that there is at
        # least one true update during each realtime second.
        self.timer = wxTimer(self)
        self.timer.Start(500)

        # Set event handlers...
        EVT_PAINT(self, self.OnPaint)
        EVT_ERASE_BACKGROUND(self, lambda x: None)
        EVT_SIZE(self, self.OnSize)
        EVT_TIMER(self, -1, self.OnTimerExpire)
        EVT_WINDOW_DESTROY(self, self.OnQuit)


    def SetTickMarkStyle(self, style):
        """
        Set the style of the marks around the edge of the clock.
        Options are TICKS_NONE, TICKS_SQUARE, and TICKS_CIRCLE
        """
        self.tickMarkStyle = style


    def SetTickMarkColours(self, brushC, penC="BLACK"):
        """
        Set the brush colour and optionally the pen colour of
        the marks around the edge of the clock.
        """
        self.tickMarksBrushC = brushC
        self.tickMarksPenC   = penC

    SetTickMarkColour = SetTickMarkColours


    def SetHandsColour(self, c):
        """An alias for SetForegroundColour"""
        self.SetForegroundColour(c)  # the hands just use the foreground colour



    # Using the current settings, render the points and line endings for the
    # circle inside the specified device context.  In this case, the DC is
    # a memory based device context that will be blitted to the actual
    # display DC inside the OnPaint() event handler.
    def OnSize(self, event):
        # The faceBitmap init is done here, to make sure the buffer is always
        # the same size as the Window
        size  = self.GetClientSize()
        self.faceBitmap = wxEmptyBitmap(size.width, size.height)
        self.DrawFace()


    def OnPaint(self, event):
        self.DrawHands(wxPaintDC(self))


    def OnQuit(self, event):
        self.timer.Stop()
        del self.timer


    def OnTimerExpire(self, event):
        self.DrawHands(wxClientDC(self))


    def DrawHands(self, drawDC):
        # Start by drawing the face bitmap
        drawDC.DrawBitmap(self.faceBitmap,0,0)

        currentTime = time.localtime(time.time())
        hour, minutes, seconds = currentTime[3:6]

        W,H = self.faceBitmap.GetWidth(), self.faceBitmap.GetHeight()
        centerX = W / 2
        centerY = H / 2

        radius = min(centerX, centerY)
        hour += minutes / 60.0 # added so the hour hand moves continuously
        x, y = self.point(hour, 12, (radius * .65))
        hourX, hourY = (x + centerX), (centerY - y)
        x, y = self.point(minutes, 60, (radius * .85))
        minutesX, minutesY = (x + centerX), (centerY - y)
        x, y = self.point(seconds, 60, (radius * .85))
        secondsX, secondsY = (x + centerX), (centerY - y)

        # Draw the hour hand...
        drawDC.SetPen(wxPen(self.GetForegroundColour(), 5, wxSOLID))
        drawDC.DrawLine(centerX, centerY, hourX, hourY)

        # Draw the minutes hand...
        drawDC.SetPen(wxPen(self.GetForegroundColour(), 3, wxSOLID))
        drawDC.DrawLine(centerX, centerY, minutesX, minutesY)

        # Draw the seconds hand...
        drawDC.SetPen(wxPen(self.GetForegroundColour(), 1, wxSOLID))
        drawDC.DrawLine(centerX, centerY, secondsX, secondsY)


    # Draw the specified set of line marks inside the clock face for the
    # hours or minutes...
    def DrawFace(self):
        backgroundBrush = wxBrush(self.GetBackgroundColour(), wxSOLID)
        drawDC = wxMemoryDC()
        drawDC.SelectObject(self.faceBitmap)
        drawDC.SetBackground(backgroundBrush)
        drawDC.Clear()

        W,H = self.faceBitmap.GetWidth(), self.faceBitmap.GetHeight()
        centerX = W / 2
        centerY = H / 2

        # Draw the marks for hours and minutes...
        self.DrawTimeMarks(drawDC, self.minuteMarks, centerX, centerY, 4)
        self.DrawTimeMarks(drawDC, self.hourMarks, centerX, centerY, 9)


    def DrawTimeMarks(self, drawDC, markCount, centerX, centerY, markSize):
        for i in range(markCount):
            x, y = self.point(i + 1, markCount, min(centerX,centerY) - 16)
            scaledX = x + centerX - markSize/2
            scaledY = centerY - y - markSize/2

            drawDC.SetBrush(wxBrush(self.tickMarksBrushC, wxSOLID))
            drawDC.SetPen(wxPen(self.tickMarksPenC, 1, wxSOLID))
            if self.tickMarkStyle != self.TICKS_NONE:
                if self.tickMarkStyle == self.TICKS_CIRCLE:
                    drawDC.DrawEllipse(scaledX - 2, scaledY, markSize, markSize)
                else:
                    drawDC.DrawRectangle(scaledX - 3, scaledY, markSize, markSize)


    def point(self, tick, range, radius):
        angle = tick * (360.0 / range)
        radiansPerDegree = math.pi / 180
        pointX = int(round(radius * math.sin(angle * radiansPerDegree)))
        pointY = int(round(radius * math.cos(angle * radiansPerDegree)))
        return wxPoint(pointX, pointY)




if __name__ == "__main__":
    class App(wxApp):
        def OnInit(self):
            frame = wxFrame(None, -1, "AnalogClockWindow Test", size=(375,375))

            clock = AnalogClockWindow(frame)
            clock.SetTickMarkColours("RED")
            clock.SetHandsColour("WHITE")
            clock.SetBackgroundColour("BLUE")

            frame.Centre(wxBOTH)
            frame.Show(True)
            self.SetTopWindow(frame)
            return true

    theApp = App(0)
    theApp.MainLoop()



