# AnalogClock's main class
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.
#
# For more info please see the __init__.py file.

import wx

from styles import *
from helpers import Dyer, Face, Hand, HandSet, TickSet, Box
from setup import Setup

#----------------------------------------------------------------------

class AnalogClock(wx.PyWindow):
    """An analog clock."""

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, name="AnalogClock",
                 clockStyle=DEFAULT_CLOCK_STYLE,
                 minutesStyle=TICKS_CIRCLE, hoursStyle=TICKS_POLY):

        wx.PyWindow.__init__(self, parent, id, pos, size, style, name)

        # Base size for scale calc purposes.
        self.basesize = wx.Size(348, 348)

        # Store some references.
        self.clockStyle = clockStyle
        self.minutesStyle = minutesStyle
        self.hoursStyle = hoursStyle

        self.DrawHands = self._drawHands
        self.DrawBox = self._drawBox
        self.RecalcCoords = self._recalcCoords

        self.shadowOffset = 3

        self.allHandStyles = [SHOW_HOURS_HAND,
                              SHOW_MINUTES_HAND,
                              SHOW_SECONDS_HAND]

        # Initialize clock face.
        #
        # By default we don't use colours or borders on the clock face.
        bg = self.GetBackgroundColour()
        face = Face(dyer=Dyer(bg, 0, bg))

        # Initialize tick marks.
        #
        # TickSet is a set of tick marks; there's always two TickSets defined
        # regardless whether they're being shown or not.
        ticksM = TickSet(self, style=minutesStyle, size=5, kind="minutes")
        ticksH = TickSet(self, style=hoursStyle, size=25, kind="hours",
                         rotate=clockStyle&ROTATE_TICKS)

        # Box holds the clock face and tick marks.
        self.Box = Box(self, face, ticksM, ticksH)

        # Initialize hands.
        #
        # HandSet is the set of hands; there's always one HandSet defined
        # regardless whether hands are being shown or not.
        #
        # A 'lenfac = 0.95', e.g., means that the lenght of that hand will
        # be 95% of the maximum allowed hand lenght ('nice' maximum lenght).
        handH = Hand(size=7, lenfac=0.7)
        handM = Hand(size=5, lenfac=0.95)
        handS = Hand(size=1, lenfac=0.95)
        self.Hands = HandSet(self, handH, handM, handS)

        # Create the customization dialog.
        self.Setup = None

        # Make a context menu.
        popup1 = wx.NewId()
        popup2 = wx.NewId()
        cm = self.cm = wx.Menu()
        cm.Append(popup1, "Customize...")
        cm.Append(popup2, "About...")

        # Set event handlers.
        self.Bind(wx.EVT_SIZE, self._OnSize)
        self.Bind(wx.EVT_PAINT, self._OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda evt: None)
        self.Bind(wx.EVT_TIMER, self._OnTimer)
        self.Bind(wx.EVT_WINDOW_DESTROY, self._OnDestroyWindow)
        self.Bind(wx.EVT_CONTEXT_MENU, self._OnContextMenu)
        self.Bind(wx.EVT_MENU, self._OnShowSetup, id=popup1)
        self.Bind(wx.EVT_MENU, self._OnShowAbout, id=popup2)

        # Set initial size based on given size, or best size
        self.SetBestFittingSize(size)

        # Do initial drawing (in case there is not an initial size event)
        self.RecalcCoords(self.GetSize())
        self.DrawBox()
        
        # Initialize the timer that drives the update of the clock face.
        # Update every half second to ensure that there is at least one true
        # update during each realtime second.
        self.timer = wx.Timer(self)
        self.timer.Start(500)


    def DoGetBestSize(self):
        # Just pull a number out of the air.  If there is a way to
        # calculate this then it should be done...
        size = wx.Size(50,50)
        self.CacheBestSize(size)
        return size
    

    def _OnSize(self, evt):
        size = self.GetClientSize()
        if size.x < 1 or size.y < 1:
            return

        self.RecalcCoords(size)
        self.DrawBox()


    def _OnPaint(self, evt):
        dc = wx.BufferedPaintDC(self)
        self.DrawHands(dc)


    def _OnTimer(self, evt):
        self.Refresh(False)
        self.Update()
        

    def _OnDestroyWindow(self, evt):
        self.timer.Stop()
        del self.timer


    def _OnContextMenu(self, evt):
        self.PopupMenu(self.cm)


    def _OnShowSetup(self, evt):
        if self.Setup is None:
            self.Setup = Setup(self)
        self.Setup.Show()
        self.Setup.Raise()


    def _OnShowAbout(self, evt):
        msg = "AnalogClock\n\n" \
              "by Several folks on wxPython-users\n" \
              "with enhancements from E. A. Tacao."
        title = "About..."
        style = wx.OK|wx.ICON_INFORMATION

        dlg = wx.MessageDialog(self, msg, title, style)
        dlg.ShowModal()
        dlg.Destroy()


    def _recalcCoords(self, size):
        """
        Recalculates all coordinates/geometry and inits the faceBitmap
        to make sure the buffer is always the same size as the window.
        """

        self.faceBitmap = wx.EmptyBitmap(*size.Get())

        # Recalc all coords.
        scale = min([float(size.width) / self.basesize.width,
                     float(size.height) / self.basesize.height])

        centre = wx.Point(size.width / 2., size.height / 2.)

        self.Box.RecalcCoords(size, centre, scale)
        self.Hands.RecalcCoords(size, centre, scale)

        # Try to find a 'nice' maximum length for the hands so that they won't
        # overlap the tick marks. OTOH, if you do want to allow overlapping the
        # lenfac value (defined on __init__ above) has to be set to
        # something > 1.
        niceradius = self.Box.GetNiceRadiusForHands(centre)
        self.Hands.SetMaxRadius(niceradius)


    def _drawBox(self):
        """Draws clock face and tick marks onto the faceBitmap."""
        dc = wx.BufferedDC(None, self.faceBitmap)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour(), wx.SOLID))
        dc.Clear()
        self.Box.Draw(dc)


    def _drawHands(self, dc):
        """
        Draws the face bitmap, created on the last DrawBox call, and
        clock hands.
        """
        dc.DrawBitmap(self.faceBitmap, 0, 0)
        self.Hands.Draw(dc)


    # Public methods --------------------------------------------------

    def GetHandSize(self, target=ALL):
        """Gets thickness of hands."""

        return self.Hands.GetSize(target)


    def GetHandFillColour(self, target=ALL):
        """Gets fill colours of hands."""

        return self.Hands.GetFillColour(target)


    def GetHandBorderColour(self, target=ALL):
        """Gets border colours of hands."""

        return self.Hands.GetBorderColour(target)


    def GetHandBorderWidth(self, target=ALL):
        """Gets border widths of hands."""

        return self.Hands.GetBorderWidth(target)


    def GetTickSize(self, target=ALL):
        """Gets sizes of ticks."""

        return self.Box.GetTickSize(target)



    def GetTickFillColour(self, target=ALL):
        """Gets fill colours of ticks."""

        return self.Box.GetTickFillColour(target)



    def GetTickBorderColour(self, target=ALL):
        """Gets border colours of ticks."""

        return self.Box.GetTickBorderColour(target)



    def GetTickBorderWidth(self, target=ALL):
        """Gets border widths of ticks."""

        return self.Box.GetTickBorderWidth(target)



    def GetTickPolygon(self, target=ALL):
        """
        Gets lists of points to be used as polygon shapes
        when using the TICKS_POLY style.
        """

        return self.Box.GetTickPolygon(target)



    def GetTickFont(self, target=ALL):
        """
        Gets fonts for tick marks when using TICKS_DECIMAL or
        TICKS_ROMAN style.
        """

        return self.Box.GetTickFont(target)



    def GetTickOffset(self, target=ALL):
        """Gets the distance of tick marks for hours from border."""

        return self.Box.GetTickOffset(target)



    def GetFaceFillColour(self):
        """Gets fill colours of watch."""

        return self.Box.Face.GetFillColour()



    def GetFaceBorderColour(self):
        """Gets border colours of watch."""

        return self.Box.Face.GetBorderColour()



    def GetFaceBorderWidth(self):
        """Gets border width of watch."""

        return self.Box.Face.GetBorderWidth()



    def GetShadowColour(self):
        """Gets the colour to be used to draw shadows."""

        a_clock_part = self.Box
        return a_clock_part.GetShadowColour()



    def GetClockStyle(self):
        """Returns the current clock style."""

        return self.clockStyle


    def GetTickStyle(self, target=ALL):
        """Gets the tick style(s)."""

        return self.Box.GetTickStyle(target)


    def Reset(self):
        """
        Forces an immediate recalculation and redraw of all clock
        elements.
        """
        size = self.GetClientSize()
        if size.x < 1 or size.y < 1:
            return
        self.RecalcCoords(size)
        self.DrawBox()
        self.Refresh(False)
        

    def SetHandSize(self, size, target=ALL):
        """Sets thickness of hands."""

        self.Hands.SetSize(size, target)


    def SetHandFillColour(self, colour, target=ALL):
        """Sets fill colours of hands."""

        self.Hands.SetFillColour(colour, target)


    def SetHandBorderColour(self, colour, target=ALL):
        """Sets border colours of hands."""

        self.Hands.SetBorderColour(colour, target)


    def SetHandBorderWidth(self, width, target=ALL):
        """Sets border widths of hands."""

        self.Hands.SetBorderWidth(width, target)


    def SetTickSize(self, size, target=ALL):
        """Sets sizes of ticks."""

        self.Box.SetTickSize(size, target)
        self.Reset()


    def SetTickFillColour(self, colour, target=ALL):
        """Sets fill colours of ticks."""

        self.Box.SetTickFillColour(colour, target)
        self.Reset()


    def SetTickBorderColour(self, colour, target=ALL):
        """Sets border colours of ticks."""

        self.Box.SetTickBorderColour(colour, target)
        self.Reset()


    def SetTickBorderWidth(self, width, target=ALL):
        """Sets border widths of ticks."""

        self.Box.SetTickBorderWidth(width, target)
        self.Reset()


    def SetTickPolygon(self, polygon, target=ALL):
        """
        Sets lists of points to be used as polygon shapes
        when using the TICKS_POLY style.
        """

        self.Box.SetTickPolygon(polygon, target)
        self.Reset()


    def SetTickFont(self, font, target=ALL):
        """
        Sets fonts for tick marks when using text-based tick styles
        such as TICKS_DECIMAL or TICKS_ROMAN.
        """

        self.Box.SetTickFont(font, target)
        self.Reset()


    def SetTickOffset(self, offset, target=ALL):
        """Sets the distance of tick marks for hours from border."""

        self.Box.SetTickOffset(offset, target)
        self.Reset()


    def SetFaceFillColour(self, colour):
        """Sets fill colours of watch."""

        self.Box.Face.SetFillColour(colour)
        self.Reset()


    def SetFaceBorderColour(self, colour):
        """Sets border colours of watch."""

        self.Box.Face.SetBorderColour(colour)
        self.Reset()


    def SetFaceBorderWidth(self, width):
        """Sets border width of watch."""

        self.Box.Face.SetBorderWidth(width)
        self.Reset()


    def SetShadowColour(self, colour):
        """Sets the colour to be used to draw shadows."""

        self.Hands.SetShadowColour(colour)
        self.Box.SetShadowColour(colour)
        self.Reset()


    def SetClockStyle(self, style):
        """
        Set the clock style, according to the options below.

            ====================  ================================
            SHOW_QUARTERS_TICKS   Show marks for hours 3, 6, 9, 12
            SHOW_HOURS_TICKS      Show marks for all hours
            SHOW_MINUTES_TICKS    Show marks for minutes

            SHOW_HOURS_HAND       Show hours hand
            SHOW_MINUTES_HAND     Show minutes hand
            SHOW_SECONDS_HAND     Show seconds hand

            SHOW_SHADOWS          Show hands and marks shadows

            ROTATE_TICKS          Align tick marks to watch
            OVERLAP_TICKS         Draw tick marks for minutes even
                                  when they match the hours marks.
            ====================  ================================
        """

        self.clockStyle = style
        self.Box.SetIsRotated(style & ROTATE_TICKS)
        self.Reset()


    def SetTickStyle(self, style, target=ALL):
        """
        Set the tick style, according to the options below.

            =================   ======================================
            TICKS_NONE          Don't show tick marks.
            TICKS_SQUARE        Use squares as tick marks.
            TICKS_CIRCLE        Use circles as tick marks.
            TICKS_POLY          Use a polygon as tick marks. A
                                polygon can be passed using
                                SetTickPolygon, otherwise the default
                                polygon will be used.
            TICKS_DECIMAL       Use decimal numbers as tick marks.
            TICKS_ROMAN         Use Roman numbers as tick marks.
            TICKS_BINARY        Use binary numbers as tick marks.
            TICKS_HEX           Use hexadecimal numbers as tick marks.
            =================   ======================================
        """

        self.Box.SetTickStyle(style, target)
        self.Reset()


    def SetBackgroundColour(self, colour):
        """Overriden base wx.Window method."""

        wx.Window.SetBackgroundColour(self, colour)
        self.Reset()


    def SetForegroundColour(self, colour):
        """
        Overriden base wx.Window method. This method sets a colour for
        all hands and ticks at once.
        """

        wx.Window.SetForegroundColour(self, colour)
        self.SetHandFillColour(colour)
        self.SetHandBorderColour(colour)
        self.SetTickFillColour(colour)
        self.SetTickBorderColour(colour)
        self.Reset()


    def SetWindowStyle(self, *args, **kwargs):
        """Overriden base wx.Window method."""

        size = self.GetSize()
        self.Freeze()
        wx.Window.SetWindowStyle(self, *args, **kwargs)
        self.SetSize((10, 10))
        self.SetSize(size)
        self.Thaw()


    def SetWindowStyleFlag(self, *args, **kwargs):
        """Overriden base wx.Window method."""

        self.SetWindowStyle(*args, **kwargs)


# For backwards compatibility -----------------------------------------

class AnalogClockWindow(AnalogClock):
    """
    A simple derived class that provides some backwards compatibility
    with the old analogclock module.
    """
    def SetTickShapes(self, tsh, tsm=None):
        self.SetTickPolygon(tsh)

    def SetHandWeights(self, h=None, m=None, s=None):
        if h:
            self.SetHandSize(h, HOUR)
        if m:
            self.SetHandSize(m, MINUTE)
        if s:
            self.SetHandSize(s, SECOND)

    def SetHandColours(self, h=None, m=None, s=None):
        if h and not m and not s:
            m=h
            s=h
        if h:
            self.SetHandBorderColour(h, HOUR)
            self.SetHandFillColour(h, HOUR)
        if m:
            self.SetHandBorderColour(m, MINUTE)
            self.SetHandFillColour(m, MINUTE)
        if s:
            self.SetHandBorderColour(s, SECOND)
            self.SetHandFillColour(s, SECOND)

    def SetTickColours(self, h=None, m=None):
        if not m:
            m=h
        if h:
            self.SetTickBorderColour(h, HOUR)
            self.SetTickFillColour(h, HOUR)
        if m:
            self.SetTickBorderColour(m, MINUTE)
            self.SetTickFillColour(m, MINUTE)

    def SetTickSizes(self, h=None, m=None):
        if h:
            self.SetTickSize(h, HOUR)
        if m:
            self.SetTickSize(m, MINUTE)

    def SetTickFontss(self, h=None, m=None):
        if h:
            self.SetTickFont(h, HOUR)
        if m:
            self.SetTickFont(m, MINUTE)


    def SetMinutesOffset(self, o):
        pass
    
    def SetShadowColour(self, s):
        pass
    
    def SetWatchPenBrush(self, p=None, b=None):
        if p:
            self.SetFaceBorderColour(p.GetColour())
            self.SetFaceBorderWidth(p.GetWidth())
        if b:
            self.SetFaceFillColour(b.GetColour())

    def SetClockStyle(self, style):
        style |= SHOW_HOURS_HAND|SHOW_MINUTES_HAND|SHOW_SECONDS_HAND
        AnalogClock.SetClockStyle(self, style)

    def SetTickStyles(self, h=None, m=None):
        if h:
            self.SetTickStyle(h, HOUR)
        if m:
            self.SetTickStyle(h, MINUTE)

        
# Test stuff ----------------------------------------------------------

if __name__ == "__main__":
    print wx.VERSION_STRING

    class AcDemoApp(wx.App):
        def OnInit(self):
            frame = wx.Frame(None, -1, "AnalogClock", size=(375, 375))
            clock = AnalogClock(frame)
            frame.CentreOnScreen()
            frame.Show()
            return True

    acApp = AcDemoApp(0)
    acApp.MainLoop()


#
##
### eof
