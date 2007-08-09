#----------------------------------------------------------------------------
# Name:        Joystick.py
# Purpose:     Demonstrate use of wx.Joystick
#
# Author:      Jeff Grimmett (grimmtoo@softhome.net), adapted from original
#              .wdr-derived demo
#
# Created:     02-Jan-2004
# RCS-ID:      $Id$
# Copyright:
# Licence:     wxWindows license
#----------------------------------------------------------------------------
#

import  math
import  wx

haveJoystick = True
if wx.Platform == "__WXMAC__":
    haveJoystick = False

#----------------------------------------------------------------------------

# Once all supported versions of Python support 32-bit integers on all
# platforms, this can go up to 32.
MAX_BUTTONS = 16

#----------------------------------------------------------------------------

class Label(wx.StaticText):
    # A derived StaticText that always aligns right and renders
    # in a bold font.
    def __init__(self, parent, label):
        wx.StaticText.__init__(self, parent, -1, label, style=wx.ALIGN_RIGHT)

        self.SetFont(
            wx.Font(
                parent.GetFont().GetPointSize(),
                parent.GetFont().GetFamily(),
                parent.GetFont().GetStyle(),
                wx.BOLD
                ))

#----------------------------------------------------------------------------


class JoyGauge(wx.Panel):
    def __init__(self, parent, stick):

        self.stick = stick
        size = (100,100)
        
        wx.Panel.__init__(self, parent, -1, size=size)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda e: None)

        self.buffer = wx.EmptyBitmap(*size)
        dc = wx.BufferedDC(None, self.buffer)
        self.DrawFace(dc)
        self.DrawJoystick(dc)
       

    def OnSize(self, event):
        # The face Bitmap init is done here, to make sure the buffer is always
        # the same size as the Window
        w, h  = self.GetClientSize()
        self.buffer = wx.EmptyBitmap(w,h)
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawJoystick(dc)


    def DrawFace(self, dc):
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()


    def OnPaint(self, evt):
        # When dc is destroyed it will blit self.buffer to the window,
        # since no other drawing is needed we'll just return and let it
        # do it's thing
        dc = wx.BufferedPaintDC(self, self.buffer)


    def DrawJoystick(self, dc):
        # draw the guage as a maxed square in the center of this window.
        w, h = self.GetClientSize()
        edgeSize = min(w, h)

        xorigin = (w - edgeSize) / 2
        yorigin = (h - edgeSize) / 2
        center = edgeSize / 2

        # Restrict our drawing activities to the square defined
        # above.
        dc.SetClippingRegion(xorigin, yorigin, edgeSize, edgeSize)

        # Optimize drawing a bit (for Win)
        dc.BeginDrawing()

        dc.SetBrush(wx.Brush(wx.Colour(251, 252, 237)))
        dc.DrawRectangle(xorigin, yorigin, edgeSize, edgeSize)

        dc.SetPen(wx.Pen(wx.BLACK, 1, wx.DOT_DASH))

        dc.DrawLine(xorigin, yorigin + center, xorigin + edgeSize, yorigin + center)
        dc.DrawLine(xorigin + center, yorigin, xorigin + center, yorigin + edgeSize)

        if self.stick:
            # Get the joystick position as a float
            joyx =  float(self.stick.GetPosition().x)
            joyy =  float(self.stick.GetPosition().y)

            # Get the joystick range of motion
            xmin = self.stick.GetXMin()
            xmax = self.stick.GetXMax()
            if xmin < 0:
                xmax += abs(xmin)
                joyx += abs(xmin)
                xmin = 0
            xrange = max(xmax - xmin, 1)

            ymin = self.stick.GetYMin()
            ymax = self.stick.GetYMax()
            if ymin < 0:
                ymax += abs(ymin)
                joyy += abs(ymin)
                ymin = 0
            yrange = max(ymax - ymin, 1)

            # calc a ratio of our range versus the joystick range
            xratio = float(edgeSize) / xrange
            yratio = float(edgeSize) / yrange

            # calc the displayable value based on position times ratio
            xval = int(joyx * xratio)
            yval = int(joyy * yratio)

            # and normalize the value from our brush's origin
            x = xval + xorigin
            y = yval + yorigin

            # Now to draw it.
            dc.SetPen(wx.Pen(wx.RED, 2))
            dc.CrossHair(x, y)

        # Turn off drawing optimization
        dc.EndDrawing()


    def Update(self):
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawJoystick(dc)


#----------------------------------------------------------------------------

class JoyPanel(wx.Panel):
    def __init__(self, parent, stick):

        self.stick = stick

        wx.Panel.__init__(self, parent, -1)

        sizer = wx.BoxSizer(wx.VERTICAL)

        fn = wx.Font(
                parent.GetFont().GetPointSize() + 3,
                parent.GetFont().GetFamily(),
                parent.GetFont().GetStyle(),
                wx.BOLD
                )

        t = wx.StaticText(self, -1, "X - Y Axes", style = wx.ALIGN_CENTRE)
        t.SetFont(fn)
        sizer.Add(t, 0, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER | wx.ALIGN_CENTER_HORIZONTAL, 1)

        self.control = JoyGauge(self, self.stick)
        sizer.Add(self.control, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER | wx.ALIGN_CENTER_HORIZONTAL, 1)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def Update(self):
        self.control.Update()


#----------------------------------------------------------------------------

class POVGauge(wx.Panel):
    #
    # Display the current postion of the POV control
    #
    def __init__(self, parent, stick):

        self.stick = stick
        self.size = (100, 100)
        self.avail = False
        self.fourDir = False
        self.cts = False

        wx.Panel.__init__(self, parent, -1, size=self.size)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda e: None)

        self.buffer = wx.EmptyBitmap(*self.size)
        dc = wx.BufferedDC(None, self.buffer)
        self.DrawFace(dc)
        self.DrawPOV(dc)


    def OnSize(self, event):
        # calculate the size of our display and make a buffer for it.
        w, h  = self.GetClientSize()
        s = min(w, h)
        self.size = (s, s)
        self.buffer = wx.EmptyBitmap(w,h)
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawPOV(dc)

    
    def DrawFace(self, dc):
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()


    def OnPaint(self, evt):
        # When dc is destroyed it will blit self.buffer to the window,
        # since no other drawing is needed we'll just return and let it
        # do it's thing
        dc = wx.BufferedPaintDC(self, self.buffer)


    def DrawPOV(self, dc):
        # draw the guage as a maxed circle in the center of this window.
        w, h = self.GetClientSize()
        diameter = min(w, h)

        xorigin = (w - diameter) / 2
        yorigin = (h - diameter) / 2
        xcenter = xorigin + diameter / 2
        ycenter = yorigin + diameter / 2

        # Optimize drawing a bit (for Win)
        dc.BeginDrawing()

        # our 'raster'.
        dc.SetBrush(wx.Brush(wx.WHITE))
        dc.DrawCircle(xcenter, ycenter, diameter/2)
        dc.SetBrush(wx.Brush(wx.BLACK))
        dc.DrawCircle(xcenter, ycenter, 10)

        # fancy decorations
        dc.SetPen(wx.Pen(wx.BLACK, 1, wx.DOT_DASH))
        dc.DrawLine(xorigin, ycenter, xorigin + diameter, ycenter)
        dc.DrawLine(xcenter, yorigin, xcenter, yorigin + diameter)

        if self.stick:
            if self.avail:

                pos = -1

                # use the appropriate function to get the POV position
                if self.fourDir:
                    pos = self.stick.GetPOVPosition()

                if self.cts:
                    pos = self.stick.GetPOVCTSPosition()

                # trap invalid values
                if 0 <= pos <= 36000:
                    vector = 30
                else:
                    vector = 0

                # rotate CCW by 90 so that 0 is up.
                pos = (pos / 100) - 90

                # Normalize
                if pos < 0:
                    pos = pos + 360

                # Stolen from wx.lib.analogclock :-)
                radiansPerDegree = math.pi / 180
                pointX = int(round(vector * math.cos(pos * radiansPerDegree)))
                pointY = int(round(vector * math.sin(pos * radiansPerDegree)))

                # normalise value to match our actual center.
                nx = pointX + xcenter
                ny = pointY + ycenter

                # Draw the line
                dc.SetPen(wx.Pen(wx.BLUE, 2))
                dc.DrawLine(xcenter, ycenter, nx, ny)

                # And a little thing to show the endpoint
                dc.SetBrush(wx.Brush(wx.BLUE))
                dc.DrawCircle(nx, ny, 8)

        # Turn off drawing optimization
        dc.EndDrawing()


    def Update(self):
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawPOV(dc)


    def Calibrate(self):
        s = self.stick
        self.avail = s.HasPOV()
        self.fourDir = s.HasPOV4Dir()
        self.cts = s.HasPOVCTS()


#----------------------------------------------------------------------------

class POVStatus(wx.Panel):
    #
    # Displays static info about the POV control
    #
    def __init__(self, parent, stick):

        self.stick = stick

        wx.Panel.__init__(self, parent, -1, size=(100, 100))

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add((20,20))
        
        self.avail = wx.CheckBox(self, -1, "Available")
        sizer.Add(self.avail, 0, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 2)

        self.fourDir = wx.CheckBox(self, -1, "4-Way Only")
        sizer.Add(self.fourDir, 0, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 2)

        self.cts = wx.CheckBox(self, -1, "Continuous")
        sizer.Add(self.cts, 0, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 2)

        self.SetSizer(sizer)
        sizer.Fit(self)

        # Effectively makes the checkboxes read-only.
        self.Bind(wx.EVT_CHECKBOX, self.Calibrate)


    def Calibrate(self, evt=None):
        s = self.stick
        self.avail.SetValue(s.HasPOV())
        self.fourDir.SetValue(s.HasPOV4Dir())
        self.cts.SetValue(s.HasPOVCTS())


#----------------------------------------------------------------------------

class POVPanel(wx.Panel):
    def __init__(self, parent, stick):

        self.stick = stick

        wx.Panel.__init__(self, parent, -1, size=(100, 100))

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        gsizer = wx.BoxSizer(wx.VERTICAL)

        sizer.Add((25,25))
        
        fn = wx.Font(
                parent.GetFont().GetPointSize() + 3,
                parent.GetFont().GetFamily(),
                parent.GetFont().GetStyle(),
                wx.BOLD
                )
        t = wx.StaticText(self, -1, "POV Control", style = wx.ALIGN_CENTER)
        t.SetFont(fn)
        gsizer.Add(t, 0, wx.ALL | wx.EXPAND, 1)
        
        self.display = POVGauge(self, stick)
        gsizer.Add(self.display, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 1)
        sizer.Add(gsizer, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 1)
        
        self.status = POVStatus(self, stick)
        sizer.Add(self.status, 1, wx.ALL | wx.EXPAND | wx.ALIGN_CENTER, 1)

        self.SetSizer(sizer)
        sizer.Fit(self)


    def Calibrate(self):
        self.display.Calibrate()
        self.status.Calibrate()


    def Update(self):
        self.display.Update()


#----------------------------------------------------------------------------

class LED(wx.Panel):
    def __init__(self, parent, number):

        self.state = -1
        self.size = (20, 20)
        self.number = number

        self.fn = wx.Font(
                parent.GetFont().GetPointSize() - 1,
                parent.GetFont().GetFamily(),
                parent.GetFont().GetStyle(),
                wx.BOLD
                )

        wx.Panel.__init__(self, parent, -1, size=self.size)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda e: None)

        self.buffer = wx.EmptyBitmap(*self.size)
        dc = wx.BufferedDC(None, self.buffer)
        self.DrawFace(dc)
        self.DrawLED(dc)


    def OnSize(self, event):
        # calculate the size of our display.
        w, h  = self.GetClientSize()
        s = min(w, h)
        self.size = (s, s)
        self.buffer = wx.EmptyBitmap(*self.size)
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawLED(dc)


    def DrawFace(self, dc):
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()


    def OnPaint(self, evt):
        # When dc is destroyed it will blit self.buffer to the window,
        # since no other drawing is needed we'll just return and let it
        # do it's thing
        dc = wx.BufferedPaintDC(self, self.buffer)


    def DrawLED(self, dc):
        # bitmap size
        bw, bh = self.size

        # center of bitmap
        center = bw / 2

        # calc the 0, 0 origin of the bitmap
        xorigin = center - (bw / 2)
        yorigin = center - (bh / 2)

        # Optimize drawing a bit (for Win)
        dc.BeginDrawing()

        # our 'raster'.
        if self.state == 0:
            dc.SetBrush(wx.Brush(wx.RED))
        elif self.state == 1:
            dc.SetBrush(wx.Brush(wx.GREEN))
        else:
            dc.SetBrush(wx.Brush(wx.BLACK))

        dc.DrawCircle(center, center, bw/2)

        txt = str(self.number)

        # Set the font for the DC ...
        dc.SetFont(self.fn)
        # ... and calculate how much space our value
        # will take up.
        fw, fh = dc.GetTextExtent(txt)

        # Calc the center of the LED, and from that
        # derive the origin of our value.
        tx = center - (fw/2)
        ty = center - (fh/2)

        # I draw the value twice so as to give it a pseudo-shadow.
        # This is (mostly) because I'm too lazy to figure out how
        # to blit my text onto the gauge using one of the logical
        # functions. The pseudo-shadow gives the text contrast
        # regardless of whether the bar is under it or not.
        dc.SetTextForeground(wx.WHITE)
        dc.DrawText(txt, tx, ty)

        # Turn off drawing optimization
        dc.EndDrawing()


    def Update(self):
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawFace(dc)
        self.DrawLED(dc)


#----------------------------------------------------------------------------

class JoyButtons(wx.Panel):
    def __init__(self, parent, stick):

        self.stick = stick
        self.leds = {}

        wx.Panel.__init__(self, parent, -1)

        tsizer = wx.BoxSizer(wx.VERTICAL)

        fn = wx.Font(
                parent.GetFont().GetPointSize() + 3,
                parent.GetFont().GetFamily(),
                parent.GetFont().GetStyle(),
                wx.BOLD
                )

        t = wx.StaticText(self, -1, "Buttons", style = wx.ALIGN_LEFT)
        t.SetFont(fn)
        tsizer.Add(t, 0, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 1)

        sizer = wx.FlexGridSizer(4, 16, 2, 2)

        fn.SetPointSize(parent.GetFont().GetPointSize() + 1)

        for i in range(0, MAX_BUTTONS):
            t = LED(self, i)
            self.leds[i] = t
            sizer.Add(t, 1, wx.ALL|wx.ALIGN_CENTER|wx.ALIGN_CENTER_VERTICAL, 1)
            sizer.AddGrowableCol(i)

        tsizer.Add(sizer, 1, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 1)

        self.SetSizer(tsizer)
        tsizer.Fit(self)

    def Calibrate(self):
        for i in range(0, MAX_BUTTONS):
            self.leds[i].state = -1

        t = self.stick.GetNumberButtons()

        for i in range(0, t):
            self.leds[i].state = 0

    def Update(self):
        t = self.stick.GetButtonState()

        for i in range(0, MAX_BUTTONS):
            if self.leds[i].state == 1:
                self.leds[i].state = 0

            if (t & (1<<i)):
                self.leds[i].state = 1

            self.leds[i].Update()


#----------------------------------------------------------------------------

class InfoPanel(wx.Panel):
    def __init__(self, parent, stick):

        self.stick = stick

        wx.Panel.__init__(self, parent, -1)

        sizer = wx.GridBagSizer(1, 1)

        sizer.Add(Label(self, 'Mfr ID: '), (0, 0), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.MfgID = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.MfgID, (0, 1), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        sizer.Add(Label(self, 'Prod Name: '), (0, 2), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.ProdName = wx.TextCtrl(self, -1, value='', style=wx.TE_READONLY)
        sizer.Add(self.ProdName, (0, 3), (1, 3), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        sizer.Add(Label(self, 'Threshold: '), (0, 6), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.Threshold = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.Threshold, (0, 7), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        #----------------------------------------------------------------------------
        b = wx.Button(self, -1, "Calibrate")
        sizer.Add(b, (1, 0), (2, 2), wx.ALL | wx.ALIGN_CENTER, 2)

        sizer.Add(Label(self, '# of Sticks: '), (1, 2), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.NumJoysticks = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.NumJoysticks, (1, 3), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        sizer.Add(Label(self, '# of Axes: '), (1, 4), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.NumAxis = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.NumAxis, (1, 5), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        sizer.Add(Label(self, 'Max # Axes: '), (1, 6), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.MaxAxis = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.MaxAxis, (1, 7), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        #----------------------------------------------------------------------------

        sizer.Add(Label(self, 'Polling -- '), (2, 3), (1, 1), wx.ALL | wx.GROW, 2)

        sizer.Add(Label(self, 'Min: '), (2, 4), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.PollMin = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.PollMin, (2, 5), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        sizer.Add(Label(self, 'Max: '), (2, 6), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_RIGHT, 2)
        self.PollMax = wx.TextCtrl(self, -1, value='', size=(45, -1), style=wx.TE_READONLY)
        sizer.Add(self.PollMax, (2, 7), (1, 1), wx.ALL | wx.GROW | wx.ALIGN_LEFT, 2)

        #----------------------------------------------------------------------------

        self.SetSizer(sizer)
        sizer.Fit(self)


    def Calibrate(self):
        if not self.stick:
            return

        s = self.stick

        self.MfgID.SetValue(str(s.GetManufacturerId()))
        self.ProdName.SetValue(str(s.GetProductName()))
        self.Threshold.SetValue(str(s.GetMovementThreshold()))
        self.NumJoysticks.SetValue(str(s.GetNumberJoysticks()))
        self.NumAxis.SetValue(str(s.GetNumberAxes()))
        self.MaxAxis.SetValue(str(s.GetMaxAxes()))
        self.PollMin.SetValue(str(s.GetPollingMin()))
        self.PollMax.SetValue(str(s.GetPollingMax()))


#----------------------------------------------------------------------------

class AxisBar(wx.Gauge):
    #
    # This class allows us to use a wx.Gauge to display the axis value
    # with a fancy label overlayed onto the guage itself. Two values are
    # used to do things: first of all, since the gauge is limited to
    # positive numbers, the scale is fixed at 0 to 1000. We will receive
    # an adjusted value to use to render the gauge itself. The other value
    # is a raw value and actually reflects the value from the joystick itself,
    # which is then drawn over the gauge.
    #
    def __init__(self, parent):
        wx.Gauge.__init__(self, parent, -1, 1000, size=(-1, 20), style = wx.GA_HORIZONTAL | wx.GA_SMOOTH )

        # This is the value we will display.
        self.rawvalue = 0

        self.SetBackgroundColour('light blue')
        self.SetForegroundColour('orange')

        # Capture paint events for purpose of updating
        # the displayed value.
        self.Bind(wx.EVT_PAINT, self.onPaint)

    def Update(self, value, rawvalue):
        # Updates the gauge itself, sets the raw value for
        # the next EVT_PAINT
        self.SetValue(value)
        self.rawvalue = rawvalue

    def onPaint(self, evt):
        # Must always create a PaintDC when capturing
        # an EVT_PAINT event
        self.ShowValue(wx.PaintDC(self), evt)

    def ShowValue(self, dc, evt):
        # This method handles actual painting of and drawing
        # on the gauge.

        # Clear out the gauge
        dc.Clear()
        # and then carry out business as usual
        wx.Gauge.OnPaint(self, evt)

        # This is the size available to us.
        w, h = dc.GetSize()

        # This is what we will overlay on the gauge.
        # It reflects the actual value received from the
        # wx.Joystick.
        txt = str(self.rawvalue)

        # Copy the default font, make it bold.
        fn = wx.Font(
                self.GetFont().GetPointSize(),
                self.GetFont().GetFamily(),
                self.GetFont().GetStyle(),
                wx.BOLD
                )

        # Set the font for the DC ...
        dc.SetFont(fn)
        # ... and calculate how much space our value
        # will take up.
        fw, fh = dc.GetTextExtent(txt)

        # Calc the center of the gauge, and from that
        # derive the origin of our value.
        center = w / 2
        tx = center - (fw/2)

        center = h / 2
        ty = center - (fh/2)

        # I draw the value twice so as to give it a pseudo-shadow.
        # This is (mostly) because I'm too lazy to figure out how
        # to blit my text onto the gauge using one of the logical
        # functions. The pseudo-shadow gives the text contrast
        # regardless of whether the bar is under it or not.
        dc.SetTextForeground(wx.BLACK)
        dc.DrawText(txt, tx, ty)

        dc.SetTextForeground('white')
        dc.DrawText(txt, tx-1, ty-1)


#----------------------------------------------------------------------------

class Axis(wx.Panel):
    #
    # This class is a container for the min, max, and current
    # values of the joystick axis in question. It contains
    # also special features to render a 'dummy' if the axis
    # in question is not available.
    #
    def __init__(self, parent, token, stick):

        self.stick = stick

        #
        # token represents the type of axis we're displaying.
        #
        self.token = token

        #
        # Create a call to the 'Has*()' method for the stick.
        # X and Y are always there, so we tie the Has* method
        # to a hardwired True value.
        #
        if token not in ['X', 'Y']:
            self.HasFunc = eval('stick.Has%s' % token)
        else:
            self.HasFunc = self.alwaysTrue

        # Now init the panel.
        wx.Panel.__init__(self, parent, -1)

        sizer = wx.BoxSizer(wx.HORIZONTAL)

        if self.HasFunc():
            #
            # Tie our calibration functions to the appropriate
            # stick method. If we don't have the axis in question,
            # we won't need them.
            #
            self.GetMin = eval('stick.Get%sMin' % token)
            self.GetMax = eval('stick.Get%sMax' % token)

            # Create our displays and set them up.
            self.Min = wx.StaticText(self, -1, str(self.GetMin()), style=wx.ALIGN_RIGHT)
            self.Max = wx.StaticText(self, -1, str(self.GetMax()), style=wx.ALIGN_LEFT)
            self.bar = AxisBar(self)

            sizer.Add(self.Min, 0, wx.ALL | wx.ALIGN_RIGHT | wx.ALIGN_CENTER_VERTICAL, 1)
            sizer.Add(self.bar, 1, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 1)
            sizer.Add(self.Max, 0, wx.ALL | wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL, 1)

        else:
            # We go here if the axis in question is not available.
            self.control = wx.StaticText(self, -1, '       *** Not Present ***')
            sizer.Add(self.control, 1, wx.ALL | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 1)

        #----------------------------------------------------------------------------

        self.SetSizer(sizer)
        sizer.Fit(self)
        wx.CallAfter(self.Update)


    def Calibrate(self):
        if not self.HasFunc():
            return

        self.Min.SetLabel(str(self.GetMin()))
        self.Max.SetLabel(str(self.GetMax()))


    def Update(self):
        # Don't bother if the axis doesn't exist.
        if not self.HasFunc():
            return

        min = int(self.Min.GetLabel())
        max = int(self.Max.GetLabel())

        #
        # Not all values are available from a wx.JoystickEvent, so I've elected
        # to not use it at all. Therefore, we are getting our values direct from
        # the stick. These values also seem to be more stable and reliable than
        # those received from the event itself, so maybe it's a good idea to
        # use the stick directly for your program.
        #
        # Here we either select the appropriate member of stick.GetPosition() or
        # apply the appropriate Get*Position method call.
        #
        if self.token == 'X':
            val = self.stick.GetPosition().x
        elif self.token == 'Y':
            val = self.stick.GetPosition().y
        else:
            val = eval('self.stick.Get%sPosition()' % self.token)


        #
        # While we might be able to rely on a range of 0-FFFFFF on Win, that might
        # not be true of all drivers on all platforms. Thus, calc the actual full
        # range first.
        #
        if min < 0:
            max += abs(min)
            val += abs(min)
            min = 0        
        range = float(max - min)
        
        #
        # The relative value is used by the derived wx.Gauge since it is a
        # positive-only control.
        #
        relative = 0
        if range:
            relative = int( val / range * 1000)

        #
        # Pass both the raw and relative values to the derived Gauge
        #
        self.bar.Update(relative, val)


    def alwaysTrue(self):
        # a dummy method used for X and Y axis.
        return True


#----------------------------------------------------------------------------

class AxisPanel(wx.Panel):
    #
    # Contained herein is a panel that offers a graphical display
    # of the levels for all axes supported by wx.Joystick. If
    # your system doesn't have a particular axis, it will be
    # 'dummied' for transparent use.
    #
    def __init__(self, parent, stick):

        self.stick = stick

        # Defines labels and 'tokens' to identify each
        # supporte axis.
        axesList = [
            ('X Axis ', 'X'),   ('Y Axis ', 'Y'),
            ('Z Axis ', 'Z'),   ('Rudder ', 'Rudder'),
            ('U Axis ', 'U'),   ('V Axis ', 'V')
            ]

        # Contains a list of all axis initialized.
        self.axes = []

        wx.Panel.__init__(self, parent, -1)

        sizer = wx.FlexGridSizer(3, 4, 1, 1)
        sizer.AddGrowableCol(1)
        sizer.AddGrowableCol(3)

        #----------------------------------------------------------------------------

        # Go through the list of labels and tokens and add a label and
        # axis display to the sizer for each.
        for label, token in axesList:
            sizer.Add(Label(self, label), 0, wx.ALL | wx.ALIGN_RIGHT, 2)
            t = Axis(self, token, self.stick)
            self.axes.append(t)
            sizer.Add(t, 1, wx.ALL | wx.EXPAND | wx.ALIGN_LEFT, 2)

        #----------------------------------------------------------------------------

        self.SetSizer(sizer)
        sizer.Fit(self)
        wx.CallAfter(self.Update)

    def Calibrate(self):
        for i in self.axes:
            i.Calibrate()

    def Update(self):
        for i in self.axes:
            i.Update()


#----------------------------------------------------------------------------

class JoystickDemoPanel(wx.Panel):

    def __init__(self, parent, log):

        self.log = log

        wx.Panel.__init__(self, parent, -1)

        # Try to grab the control. If we get it, capture the stick.
        # Otherwise, throw up an exception message and play stupid.
        try:
            self.stick = wx.Joystick()
            self.stick.SetCapture(self)
            # Calibrate our controls
            wx.CallAfter(self.Calibrate)
            wx.CallAfter(self.OnJoystick)
        except NotImplementedError, v:
            wx.MessageBox(str(v), "Exception Message")
            self.stick = None

        # One Sizer to Rule Them All...
        sizer = wx.GridBagSizer(2,2)

        self.info = InfoPanel(self, self.stick)
        sizer.Add(self.info, (0, 0), (1, 3), wx.ALL | wx.GROW, 2)

        self.info.Bind(wx.EVT_BUTTON, self.Calibrate)

        self.joy = JoyPanel(self, self.stick)
        sizer.Add(self.joy, (1, 0), (1, 1), wx.ALL | wx.GROW, 2)
                  
        self.pov = POVPanel(self, self.stick)
        sizer.Add(self.pov, (1, 1), (1, 2), wx.ALL | wx.GROW, 2)
        
        self.axes = AxisPanel(self, self.stick)
        sizer.Add(self.axes, (2, 0), (1, 3), wx.ALL | wx.GROW, 2)

        self.buttons = JoyButtons(self, self.stick)
        sizer.Add(self.buttons, (3, 0), (1, 3), wx.ALL | wx.EXPAND | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL, 1)

        self.SetSizer(sizer)
        sizer.Fit(self)

        # Capture Joystick events (if they happen)
        self.Bind(wx.EVT_JOYSTICK_EVENTS, self.OnJoystick)
        self.stick.SetMovementThreshold(10)


    def Calibrate(self, evt=None):
        # Do not try this without a stick
        if not self.stick:
            return

        self.info.Calibrate()
        self.axes.Calibrate()
        self.pov.Calibrate()
        self.buttons.Calibrate()


    def OnJoystick(self, evt=None):
        if not self.stick:
            return

        self.axes.Update()
        self.joy.Update()
        self.pov.Update()
        if evt is not None and evt.IsButton():
            self.buttons.Update()


    def ShutdownDemo(self):
        if self.stick:
            self.stick.ReleaseCapture()
        self.stick = None
        
#----------------------------------------------------------------------------

def runTest(frame, nb, log):
    if haveJoystick:
        win = JoystickDemoPanel(nb, log)
        return win
    else:
        from Main import MessagePanel
        win = MessagePanel(nb, 'wx.Joystick is not available on this platform.',
                           'Sorry', wx.ICON_WARNING)
        return win
    

#----------------------------------------------------------------------------

overview = """\
<html>
<body>
<h1>wx.Joystick</h1>
This demo illustrates the use of the wx.Joystick class, which is an interface to
one or more joysticks attached to your system.

<p>The data that can be retrieved from the joystick comes in four basic flavors.
All of these are illustrated in the demo. In fact, this demo illustrates everything
you <b>can</b> get from the wx.Joystick control.

<ul>
<li>Static information such as Manufacturer ID and model name,
<li>Analog input from up to six axes, including X and Y for the actual stick,
<li>Button input from the fire button and any other buttons that the stick has,
<li>and the POV control (a kind of mini-joystick on top of the joystick) that many sticks come with.
</ul>

<p>Getting data from the joystick can be event-driven thanks to four event types associated
with wx.JoystickEvent, or the joystick can be polled programatically to get data on
a regular basis.

<h2>Data types</h2>

Data from the joystick comes in two flavors: that which defines the boundaries, and that
which defines the current state of the stick. Thus, we have Get*Max() and Get*Min() 
methods for all axes, the max number of axes, the max number of buttons, and so on. In
general, this data can be read once and stored to speed computation up.

<h3>Analog Input</h3>

Analog input (the axes) is delivered as a whole, positive number. If you need to know 
if the axis is at zero (centered) or not, you will first have to calculate that center
based on the max and min values. The demo shows a bar graph for each axis expressed
in native numerical format, plus a 'centered' X-Y axis compass showing the relationship
of that input to the calculated stick position.

Analog input may be jumpy and spurious, so the control has a means of 'smoothing' the
analog data by setting a movement threshold. This demo sets the threshold to 10, but
you can set it at any valid value between the min and max.

<h3>Button Input</h3>

Button state is retrieved as one int that contains each button state mapped to a bit.
You get the state of a button by AND-ing its bit against the returned value, in the form

<pre>
     # assume buttonState is what the stick returned, and buttonBit 
     # is the bit you want to examine
     
     if (buttonState & ( 1 &lt;&lt; buttonBit )) :
         # button pressed, do something with it
</pre>

<p>The problem here is that some OSs return a 32-bit value for up to 32 buttons 
(imagine <i>that</i> stick!). Python V2.3 will generate an exception for bit 
values over 30. For that reason, this demo is limited to 16 buttons.

<p>Note that more than one button can be pressed at a time, so be sure to check all of them!
     

<h3>POV Input</h3>

POV hats come in two flavors: four-way, and continuous. four-way POVs are restricted to
the cardinal points of the compass; continuous, or CTS POV hats can deliver input in
.01 degree increments, theoreticaly. The data is returned as a whole number; the last
two digits are considered to be to the right of the decimal point, so in order to 
use this information, you need to divide by 100 right off the bat. 

<p>Different methods are provided to retrieve the POV data for a CTS hat 
versus a four-way hat.

<h2>Caveats</h2>

The wx.Joystick control is in many ways incomplete at the C++ library level, but it is
not insurmountable.  In short, while the joystick interface <i>can</i> be event-driven,
the wx.JoystickEvent class lacks event binders for all event types. Thus, you cannot
rely on wx.JoystickEvents to tell you when something has changed, necessarilly.

<ul>
<li>There are no events associated with the POV control.
<li>There are no events associated with the Rudder
<li>There are no events associated with the U and V axes.
</ul>

<p>Fortunately, there is an easy workaround. In the top level frame, create a wx.Timer
that will poll the stick at a set interval. Of course, if you do this, you might as
well forgo catching wxEVT_JOYSTICK_* events at all and rely on the timer to do the
polling. 

<p>Ideally, the timer should be a one-shot; after it fires, collect and process data as 
needed, then re-start the timer, possibly using wx.CallAfter().

</body>
</html>
"""

#----------------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
