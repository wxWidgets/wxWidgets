# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------
# Name:        wx.lib.analogclock
# Purpose:     A simple analog clock window
#
# Author:      several folks on wxPython-users
#
# Created:     16-April-2003
# RCS-ID:      $Id$
# Copyright:   (c) 2003 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Tested with updated demo and with builtin test.
# 
# 15-February-2004 - E. A. Tacao
#
# o Many ehnacements
#


import math
import sys
import string
import time

import wx

from analogclockopts import ACCustomizationFrame


# self.clockStyle:
SHOW_QUARTERS_TICKS = 1
SHOW_HOURS_TICKS    = 2
SHOW_MINUTES_TICKS  = 4
ROTATE_TICKS        = 8
SHOW_HOURS_HAND     = 16
SHOW_MINUTES_HAND   = 32
SHOW_SECONDS_HAND   = 64
SHOW_SHADOWS        = 128
OVERLAP_TICKS       = 256

# self.tickMarkHoursStyle and self.tickMarkMinutesStyle:
TICKS_NONE          = 1
TICKS_SQUARE        = 2
TICKS_CIRCLE        = 4
TICKS_POLY          = 8
TICKS_DECIMAL       = 16
TICKS_ROMAN         = 32


class AnalogClockWindow(wx.PyWindow):
    """An analog clock window"""

    def __init__(self, parent, ID=-1, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=0, name="clock"):
        
        # Initialize the wxWindow...
        wx.PyWindow.__init__(self, parent, ID, pos, size, style, name)

        # Initialize some variables and defaults...
        self.clockStep = 1
        self.prefs_open = False

        self.tickShapeHours = self.tickShapeMinutes= [[0,0],
                                                      [1,-1],
                                                      [2,0],
                                                      [1,4]]
        self.handHoursThickness = 5
        self.handHoursColour = (0, 0, 0)

        self.handMinutesThickness = 3
        self.handMinutesColour = (0, 0, 0)

        self.handSecondsThickness = 1
        self.handSecondsColour = (0, 0, 0)

        self.tickMarkHoursPen = wx.Pen((0, 0, 0), 1, wx.SOLID)
        self.tickMarkHoursBrush = wx.Brush((0, 0, 0), wx.SOLID)
        self.markSizeHour = 10
        self.tickMarkHoursFont = wx.Font(0, wx.SWISS, wx.NORMAL, wx.BOLD)
        self.tickMarkHoursFont.SetPointSize(self.markSizeHour)

        self.tickMarkMinutesPen = wx.Pen((0, 0, 0), 1, wx.SOLID)
        self.tickMarkMinutesBrush = wx.Brush((0, 0, 0), wx.SOLID)
        self.markSizeMin = 6
        self.tickMarkMinutesFont = wx.Font(self.markSizeMin, wx.SWISS, wx.NORMAL, wx.BOLD)

        self.offM = 0

        self.shadowPenColour = self.shadowBrushColour = (128,128,128)

        self.watchPen = None
        self.watchBrush = None

        self.clockStyle = SHOW_HOURS_TICKS | SHOW_MINUTES_TICKS | SHOW_SHADOWS | ROTATE_TICKS
        self.handsStyle = SHOW_SECONDS_HAND

        self.tickMarkHoursStyle = TICKS_POLY
        self.tickMarkMinutesStyle = TICKS_CIRCLE

        self.currentTime=None

        size = wx.Size(*size)
        bestSize = self.GetBestSize()
        size.x = max(size.x, bestSize.x)
        size.y = max(size.y, bestSize.y)
        self.SetSize(size)
        
        # Make an initial bitmap for the face, it will be updated and
        # painted at the first EVT_SIZE event.
        W, H = size
        self.faceBitmap = wx.EmptyBitmap(max(W,1), max(H,1))
        
        # Set event handlers...
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda x: None)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_TIMER, self.OnTimerExpire)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnQuit)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightClick)         

        # Initialize the timer that drives the update of the clock
        # face.  Update every half second to ensure that there is at
        # least one true update during each realtime second.
        self.timer = wx.Timer(self)
        self.timer.Start(500)


    def DoGetBestSize(self):
        return wx.Size(25,25)


    def OnPaint(self, event):
        dc = wx.BufferedPaintDC(self)
        self._doDrawHands(dc, True)


    def OnTimerExpire(self, event):
        size = self.GetClientSize()
        dc = wx.BufferedDC(wx.ClientDC(self), size)
        self._doDrawHands(dc, True)


    def OnQuit(self, event):
        self.timer.Stop()
        del self.timer

        
    def OnRightDown(self, event):
        self.x = event.GetX()
        self.y = event.GetY()
        event.Skip()


    def OnRightClick(self, event):
        # only do this part the first time so the events are only bound once
        if not hasattr(self, "popupID1"):
            self.popupID1 = wx.NewId()
            self.popupID2 = wx.NewId()
            self.Bind(wx.EVT_MENU, self.OnPopupOne, id=self.popupID1)
            self.Bind(wx.EVT_MENU, self.OnPopupTwo, id=self.popupID2)

        # make a menu
        sm = wx.Menu()

        sm.Append(self.popupID1, "Customize...")
        sm.Append(self.popupID2, "About...")

        # If there already a setup window open, we must not appear...
        if not self.prefs_open:
            # Popup the menu.  If an item is selected then its handler
            # will be called before PopupMenu returns.
            self.PopupMenu(sm, (self.x,self.y))
        sm.Destroy()


    def OnPopupOne(self, event):
        self.prefs_open=True
        frame = ACCustomizationFrame(self, -1, "AnalogClock Preferences")
        frame.Show(True)


    def OnPopupTwo(self, event):
        dlg = wx.MessageDialog(self, "AnalogClockWindow\n\nby Several folks on wxPython-users\nwith enhancements from E. A. Tacão",
                          'About', wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


    def OnSize(self, event):
        # The faceBitmap init is done here, to make sure the buffer is always
        # the same size as the Window
        size  = self.GetClientSize()
        if size.x < 1 or size.y < 1:
            return
        
        self.faceBitmap = wx.EmptyBitmap(size.width, size.height)

        # Update drawing coordinates...
        new_dim = size.Get()
        if not hasattr(self,"dim"):
            self.dim = new_dim

        x,y=[0,1]
        self.scale = min([float(new_dim[x]) / self.dim[x],
                          float(new_dim[y]) / self.dim[y]])

        self.centerX = self.faceBitmap.GetWidth() / 2
        self.centerY = self.faceBitmap.GetHeight() / 2

        self.shadowDistance = 2 * self.scale

        self.radius_watch = min(self.centerX, self.centerY)

        self._doDrawFace()
        


    def _doDrawHands(self, drawDC, force=0):
        currentTime = list(time.localtime(time.time())[3:6])

        if not (self.handsStyle & SHOW_SECONDS_HAND):
            currentTime[2]=-1

        if not (force or currentTime != self.currentTime):
            return
        self.currentTime = currentTime
        hour, minutes, seconds = currentTime

        # Start by drawing the face bitmap
        drawDC.DrawBitmap(self.faceBitmap, 0,0)


        # NOTE:  All this hand drawing code below should be refactored into a helper function.

        
        # Draw hours hand shadow
        mStep = 6 * self.clockStep
        angle = hour * 30
        if angle > 360:
            angle = angle - 360
        angle = angle + round(minutes/(mStep*2)) * mStep

        x,y,f = self._getCoords("hand_hours", angle)

        if f and self.clockStyle & SHOW_SHADOWS:
            drawDC.SetPen(wx.Pen(self.shadowPenColour,
                                 int(self.handHoursThickness * self.scale),
                                 wx.SOLID))
            drawDC.DrawLine(self.centerX + self.shadowDistance,
                            self.centerY + self.shadowDistance,
                            x + self.shadowDistance,
                            y + self.shadowDistance)

        # Draw minutes hand shadow
        angle = minutes * 6
        x,y,f = self._getCoords("hand_minutes", angle)

        if f and self.clockStyle & SHOW_SHADOWS:
            drawDC.SetPen(wx.Pen(self.shadowPenColour,
                                 int(self.handMinutesThickness * self.scale),
                                 wx.SOLID))
            drawDC.DrawLine(self.centerX + self.shadowDistance,
                            self.centerY + self.shadowDistance,
                            x + self.shadowDistance,
                            y + self.shadowDistance)

        # Draw seconds hand shadow if required
        if seconds >= 0:
            angle = seconds * 6
            x,y,f = self._getCoords("hand_seconds", angle)
            
            if f and self.clockStyle & SHOW_SHADOWS:
                drawDC.SetPen(wx.Pen(self.shadowPenColour,
                                     int(self.handSecondsThickness * self.scale),
                                     wx.SOLID))
                drawDC.DrawLine(self.centerX + self.shadowDistance,
                                self.centerY + self.shadowDistance,
                                x + self.shadowDistance,
                                y + self.shadowDistance)


        # Draw hours hand
        angle = hour * 30
        if angle > 360:
            angle = angle - 360
        angle = angle + round(minutes/(mStep*2)) * mStep
        
        x,y,f = self._getCoords("hand_hours", angle)

        if f:
            drawDC.SetPen(wx.Pen(self.handHoursColour,
                                 int(self.handHoursThickness * self.scale),
                                 wx.SOLID))
            drawDC.DrawLine(self.centerX, self.centerY, x, y)

        # Draw minutes hand
        angle = minutes * 6
        x,y,f = self._getCoords("hand_minutes", angle)

        if f:
            drawDC.SetPen(wx.Pen(self.handMinutesColour,
                                 int(self.handMinutesThickness * self.scale),
                                 wx.SOLID))
            drawDC.DrawLine(self.centerX, self.centerY, x, y)

        # Draw seconds hand if required
        if seconds >= 0:
            angle = seconds * 6
            x,y,f = self._getCoords("hand_seconds", angle)
            if f:
                drawDC.SetPen(wx.Pen(self.handSecondsColour,
                                     int(self.handSecondsThickness * self.scale),
                                     wx.SOLID))
                drawDC.DrawLine(self.centerX, self.centerY, x, y)



    def _doDrawFace(self):
        backgroundBrush = wx.Brush(self.GetBackgroundColour(), wx.SOLID)
        drawDC = wx.MemoryDC()
        drawDC.SelectObject(self.faceBitmap)
        drawDC.SetBackground(backgroundBrush)
        drawDC.Clear()

        self.handHoursLength = 0.65 * (self.radius_watch - self._getMarkMaxSize("ticks_hours", drawDC))
        self.handMinutesLength = 0.85 * (self.radius_watch - self._getMarkMaxSize("ticks_hours", drawDC))
        self.handSecondsLength = 0.85 * (self.radius_watch - self._getMarkMaxSize("ticks_hours", drawDC))

        self.radius_ticks_hours = self.radius_watch - self.shadowDistance - self._getMarkMaxSize("ticks_hours", drawDC)
        self.radius_ticks_minutes = self.radius_ticks_hours
        
        self._calcSteps()

        # Draw the watch...
        self._drawWatch(drawDC)

        # Draw the marks for hours and minutes...
        circle = 360
        mStep = 6 * self.clockStep

        if self.clockStyle & SHOW_SHADOWS:
            for i in range(0, circle, mStep):
                for t in self.coords.keys():
                    if t.find("ticks") > -1:
                        x,y,f = self._getCoords(t, i)
                        if f:
                            self._doDrawTickMark(i, drawDC, t,
                                                 x + self.shadowDistance,
                                                 y + self.shadowDistance,
                                                 True)

        for i in range(0, circle, mStep):
            for t in self.coords.keys():
                if t.find("ticks") > -1:
                    x,y,f = self._getCoords(t, i)
                    if f:
                        self._doDrawTickMark(i, drawDC, t, x, y)



    def _doDrawTickMark(self, angle, drawDC, tipo, x, y, is_a_shadow=None):
        opts = {"ticks_hours":    [self.tickMarkHoursPen, self.tickMarkHoursBrush, self.markSizeHour, self.tickMarkHoursStyle],
                "ticks_quarters": [self.tickMarkHoursPen, self.tickMarkHoursBrush, self.markSizeHour, self.tickMarkHoursStyle],
                "ticks_minutes":  [self.tickMarkMinutesPen, self.tickMarkMinutesBrush, self.markSizeMin, self.tickMarkMinutesStyle]}

        pen, brush, size, style = opts[tipo];
        size = size * self.scale

        if is_a_shadow:
            drawDC.SetPen(wx.Pen(self.shadowPenColour, 1, wx.SOLID))
            drawDC.SetBrush(wx.Brush(self.shadowBrushColour, wx.SOLID))
            drawDC.SetTextForeground(self.shadowBrushColour)
        else:
            drawDC.SetPen(pen)
            drawDC.SetBrush(brush)
            drawDC.SetTextForeground(brush.GetColour())

        if style & TICKS_CIRCLE:
            x, y = self._center2corner(x, y, tipo)
            drawDC.DrawEllipse(x, y, size, size)

        elif style & TICKS_SQUARE:
            x, y = self._center2corner(x, y, tipo)
            drawDC.DrawRectangle(x, y, size, size)

        elif (style & TICKS_DECIMAL) or (style & TICKS_ROMAN):
            self._draw_rotate_text(drawDC, x, y, tipo, angle)

        elif style & TICKS_POLY:
             self._draw_rotate_polygon(drawDC, x, y, tipo, angle)


    def _draw_rotate_text(self, drawDC, x, y, tipo, angle):
        text = self._build_text(angle, tipo)
        lX, lY = self._center2corner(x, y, tipo, drawDC)
        lX = lX * len(text)
        angle = 360 - angle

        if self.clockStyle & ROTATE_TICKS:
            radiansPerDegree = math.pi / 180
            x = int(x - 
                    ((math.cos((angle) * radiansPerDegree)*lX) + 
                     (math.sin((angle) * radiansPerDegree)*lY)))
            y = int(y - 
                    ((math.cos((angle) * radiansPerDegree)*lY) - 
                     (math.sin((angle) * radiansPerDegree)*lX)))
            drawDC.DrawRotatedText(text, x,y, angle)

        else:
            x = x - lX
            y = y - lY
            drawDC.DrawText(text, x, y)


    def _draw_rotate_polygon(self, drawDC, x, y, tipo, angle):
        if tipo=="ticks_quarters":
            tipo="ticks_hours"
            
        # Add to empty list to prevent system-wide hard freezes under XP...
        points = {"ticks_hours":self.tickShapeHours+[], "ticks_minutes":self.tickShapeMinutes+[]}[tipo]
        size = self.scale * {"ticks_hours":self.markSizeHour, "ticks_minutes":self.markSizeMin}[tipo]

        maxX = max(map(lambda x:x[0],points))
        minX = min(map(lambda x:x[0],points))
        maxY = max(map(lambda x:x[0],points))
        minY = min(map(lambda x:x[0],points))

        maxB = abs(max(maxX, maxY));
        f = size / maxB

        orgX = (maxX - minX) / 2.
        orgY = (maxY - minY) / 2.

        radiansPerDegree = math.pi / 180
        scaledX = x
        scaledY = y

        for z in range(0, len(points)):
            x,y = points[z]
            x = x * f - orgX * f
            y = y * f - orgY * f
            if self.clockStyle & ROTATE_TICKS:
                m,t = self._rect2pol(x,y)
                t = t + angle
                x,y = self._pol2rect(m,t)
            x = x + scaledX
            y = y + scaledY
            points[z] = [int(x), int(y)]

        drawDC.DrawPolygon(points)


    def _pol2rect(self, r, w, deg=1):		# radian if deg=0; degree if deg=1
        if deg:
    	     w = math.pi * w / 180.0
        return r * math.cos(w), r * math.sin(w)


    def _rect2pol(self, x, y, deg=1):		# radian if deg=0; degree if deg=1
        if deg:
    	    return math.hypot(x, y), 180.0 * math.atan2(y, x) / math.pi
        else:
    	    return math.hypot(x, y), math.atan2(y, x)


    def _center2corner(self, x, y, tipo, drawDC=None):
        if tipo == "ticks_quarters":
            tipo = "ticks_hours"

        style = {"ticks_hours":self.tickMarkHoursStyle, "ticks_minutes":self.tickMarkMinutesStyle}[tipo]
        size = self.scale * {"ticks_hours":self.markSizeHour, "ticks_minutes":self.markSizeMin}[tipo]

        if style & TICKS_DECIMAL or style & TICKS_ROMAN:
            font = {"ticks_hours":self.tickMarkHoursFont, "ticks_minutes":self.tickMarkMinutesFont}[tipo]
            font.SetPointSize(int(size));
            drawDC.SetFont(font)
            lX = drawDC.GetCharWidth() / 2.
            lY = drawDC.GetCharHeight() / 2.
            x = lX
            y = lY
        else:
            size = self.scale * {"ticks_hours":self.markSizeHour, "ticks_minutes":self.markSizeMin}[tipo]
            x=x-size/2.;y=y-size/2.
        return x, y


    def _build_text(self, angle, tipo):
        if tipo == "ticks_quarters":
            tipo = "ticks_hours"
        a = angle
        if a <= 0:
            a = a + 360
        divider = {"ticks_hours":30,"ticks_minutes":6}[tipo]
        a = int(a / divider)

        style = {"ticks_hours":self.tickMarkHoursStyle," ticks_minutes":self.tickMarkMinutesStyle}[tipo]
        if style & TICKS_ROMAN:
            text=["I","II","III","IV","V","VI","VII","VIII","IX","X", \
                  "XI","XII","XIII","XIV","XV","XVI","XVII","XVIII","XIX","XX", \
                  "XXI","XXII","XXIII","XXIV","XXV","XXVI","XXVII","XXVIII","XXIX","XXX", \
                  "XXXI","XXXII","XXXIII","XXXIV","XXXV","XXXVI","XXXVII","XXXVIII","XXXIX","XL", \
                  "XLI","XLII","XLIII","XLIV","XLV","XLVI","XLVII","XLVIII","XLIX","L", \
                  "LI","LII","LIII","LIV","LV","LVI","LVII","LVIII","LIX","LX"][a-1]
        else:
            text = "%s" % a

        return text


    def _getMarkMaxSize(self, tipo, drawDC=None):
        if tipo == "ticks_quarters":
            tipo = "ticks_hours"

        style = {"ticks_hours":self.tickMarkHoursStyle, "ticks_minutes":self.tickMarkMinutesStyle}[tipo]
        size = self.scale * {"ticks_hours":self.markSizeHour, "ticks_minutes":self.markSizeMin}[tipo]

        if style & TICKS_DECIMAL or style & TICKS_ROMAN:
            lX = 2 * drawDC.GetCharWidth()
            lY = drawDC.GetCharHeight()
            size = math.sqrt(lX**2 + lY**2) * self.scale
        else:
            size=math.sqrt(2) * size

        return size


    def _drawWatch(self, drawDC):
        # Draw the watch...
        if self.watchPen or self.watchBrush:
            if self.watchPen:
                drawDC.SetPen(self.watchPen)
            if self.watchBrush:
                drawDC.SetBrush(self.watchBrush)
            else:
                drawDC.SetBrush(wx.Brush(self.GetBackgroundColour(), wx.SOLID))
            drawDC.DrawCircle(self.centerX, self.centerY, self.radius_watch)


    def _calcSteps(self):
        # Calcule todos os pontos para:
        #  - marcas de horas
        #  - marcas de minutos
        #  - ponteiro de horas
        #  - ponteiro de minutos
        #  - ponteiro de segundos

        circle = 360
        mStep = 6 * self.clockStep # Step in degrees...

        vq = 90 * (self.clockStyle & SHOW_QUARTERS_TICKS) / SHOW_QUARTERS_TICKS
        vh = 30 * (self.clockStyle & SHOW_HOURS_TICKS) / SHOW_HOURS_TICKS
        vm = 1  * (self.clockStyle & SHOW_MINUTES_TICKS) / SHOW_MINUTES_TICKS

        coords = {"ticks_quarters": [self.radius_ticks_hours,  60,vq,{}],
                  "ticks_hours":    [self.radius_ticks_hours,  60,vh,{}],
                  "ticks_minutes":  [self.radius_ticks_minutes,60,vm,{}],
                  "hand_hours":     [self.handHoursLength,     60,1, {}],
                  "hand_minutes":   [self.handMinutesLength,   60,1, {}],
                  "hand_seconds":   [self.handSecondsLength,   60,1, {}]}

        radiansPerDegree = math.pi / 180

        for t in coords.keys():
            for i in range(0, circle+mStep, mStep):
                radius = coords[t][0]
                if t == "ticks_minutes":
                    radius = radius - self.offM
                step_angle = 360. / coords[t][1]
                pre = coords[t][2]
                x = self.centerX + radius * math.sin(i * radiansPerDegree)
                y = self.centerY + radius * math.cos(i * radiansPerDegree)
                f = (pre and (i/step_angle == int(i/step_angle)) and (float(i)/pre == int(i/pre)))
                coords[t][3][i] = [x,y,f]

        if not self.clockStyle & OVERLAP_TICKS:
            for i in range(0, circle + mStep, mStep):
                f=coords["ticks_minutes"][3][i][2]
                if f and \
                   (coords["ticks_hours"][3].get(i,[0,0,0])[2] or coords["ticks_quarters"][3].get(i,[0,0,0])[2]):
                    f=False
                coords["ticks_minutes"][3][i][2]=f

        self.coords = coords


    def _getCoords(self, tipo, angle):
        # Returns coords and 'use flag' based on current angle...
        k = 360 - (angle + 180)
        if k <= 0:
            k = k + 360
        return self.coords[tipo][3][k]


# -----------------------------------------------------
#
    def SetTickShapes(self, tsh, tsm=None):
        """
        tsh, tsm: [[x0,y0], [x1,y1], ... [xn,yn]]

        Sets lists of lists of points to be used as polygon shapes
        when using the TICKS_POLY style. If tsm is ommitted,
        we'll use tsh for both shapes.
        """

        if not tsm:
            tsm=tsh

        self.tickShapeHours = tsh
        self.tickShapeMinutes = tsm


    def SetHandWeights(self, h=None, m=None, s=None):
        """
        h, m, s: value

        Sets thickness of hands.
        """

        if h:
           self.handHoursThickness = h
        if m:
           self.handMinutesThickness = m
        if s:
           self.handSecondsThickness = s


    def SetHandColours(self, h=None, m=None, s=None):
        """
        h, m, s: wx.Colour

        Sets colours of hands. If m and s are ommitted,
        we'll use h for all.
        """

        if h and not m and not s:
            m=h
            s=h

        if h:
            self.handHoursColour = h
        if m:
            self.handMinutesColour = m
        if s:
            self.handSecondsColour = s


    def SetTickColours(self, h=None, m=None):
        """
        h, m: wx.Colour

        Sets colours of ticks. If m is ommitted,
        we'll use h for both.
        """

        if not m:
            m=h

        if h:
            self.tickMarkHoursPen = wx.Pen(h, 1, wx.SOLID)
            self.tickMarkHoursBrush = wx.Brush(h, wx.SOLID)

        if m:
           self.tickMarkMinutesPen = wx.Pen(m, 1, wx.SOLID)
           self.tickMarkMinutesBrush = wx.Brush(m, wx.SOLID)


    def SetTickSizes(self, h=None, m=None):
        """
        h, m: value

        Sizes for tick marks.
        """

        if h:
            self.markSizeHour = h
        if m:
            self.markSizeMin = m


    def SetTickFonts(self, h=None, m=None):
        """
        h, m: wx.Font

        Fonts for tick marks when using TICKS_DECIMAL or TICKS_ROMAN style.
        If m is ommitted, we'll use h for both.
        """

        if not m:
            m=h

        if h:
            self.tickMarkHoursFont = h
            self.tickMarkHoursFont.SetPointSize(self.markSizeHour)
        if m:
            self.tickMarkMinutesFont = m
            self.tickMarkMinutesFont.SetPointSize(self.markSizeMin)


    def SetMinutesOffset(self, o):
        """
        s = value

        Sets the distance between tick marks for hours and minutes.
        """
        self.offM = o


    def SetShadowColour(self, s):
        """
        s = wx.Colour or (r,g,b) tuple.

        Sets the colour to be used to draw shadows.
        """

        self.shadowPenColour = self.shadowBrushColour = s


    def SetWatchPenBrush(self, p=None, b=None):
        """
        p = wx.Pen; b = wx.Brush

        Set the pen and brush for the watch.
        """

        if p:
            self.watchPen = p
        if b:
            self.watchBrush = b


    def SetClockStyle(self, style):
        """
        Set the clock style, acording to these options:

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


    def SetTickStyles(self, h=None, m=None):
        """
        Set the ticks styles, acording to the options below.

            =================   =====================================
            TICKS_NONE          Don't show tick marks.
            TICKS_SQUARE        Use squares as tick marks.
            TICKS_CIRCLE        Use circles as tick marks.
            TICKS_POLY          Use a polygon as tick marks. The
                                polygon must be passed using
                                SetTickShapes, otherwise the default
                                polygon will be used.
            TICKS_DECIMAL       Use decimal numbers.
            TICKS_ROMAN         Use Roman numbers.
            =================   =====================================
        """

        if h:
            self.tickMarkHoursStyle = h
        if m:
            self.tickMarkMinutesStyle = m
#
# -----------------------------------------------------


if __name__ == "__main__":
    print wx.VERSION_STRING
    class App(wx.App):
        def OnInit(self):
            frame = wx.Frame(None, -1, "AnalogClockWindow", size=(375,375))

            clock = AnalogClockWindow(frame)
            
            # Settings below are used by default...
            #clock.SetClockStyle(SHOW_HOURS_TICKS|SHOW_MINUTES_TICKS|SHOW_SHADOWS|ROTATE_TICKS)
            #clock.SetTickStyles(TICKS_POLY, TICKS_CIRCLE)

            frame.Centre(wx.BOTH)
            frame.Show(True)
            self.SetTopWindow(frame)
            return True

    theApp = App(0)
    theApp.MainLoop()


#
##
### eof
