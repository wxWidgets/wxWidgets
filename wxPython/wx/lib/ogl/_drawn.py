# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------------
# Name:         drawn.py
# Purpose:      DrawnShape class
#
# Author:       Pierre Hjälm (from C++ original by Julian Smart)
#
# Created:      2004-08-25
# RCS-ID:       $Id$
# Copyright:    (c) 2004 Pierre Hjälm - 1998 Julian Smart
# License:      wxWindows license
#----------------------------------------------------------------------------

import os.path

from _basic import RectangleShape
from _oglmisc import *

METAFLAGS_OUTLINE         = 1
METAFLAGS_ATTACHMENTS     = 2

DRAWN_ANGLE_0        = 0
DRAWN_ANGLE_90       = 1
DRAWN_ANGLE_180      = 2
DRAWN_ANGLE_270      = 3

# Drawing operations
DRAWOP_SET_PEN               = 1
DRAWOP_SET_BRUSH             = 2
DRAWOP_SET_FONT              = 3
DRAWOP_SET_TEXT_COLOUR       = 4
DRAWOP_SET_BK_COLOUR         = 5
DRAWOP_SET_BK_MODE           = 6
DRAWOP_SET_CLIPPING_RECT     = 7
DRAWOP_DESTROY_CLIPPING_RECT = 8

DRAWOP_DRAW_LINE             = 20
DRAWOP_DRAW_POLYLINE         = 21
DRAWOP_DRAW_POLYGON          = 22
DRAWOP_DRAW_RECT             = 23
DRAWOP_DRAW_ROUNDED_RECT     = 24
DRAWOP_DRAW_ELLIPSE          = 25
DRAWOP_DRAW_POINT            = 26
DRAWOP_DRAW_ARC              = 27
DRAWOP_DRAW_TEXT             = 28
DRAWOP_DRAW_SPLINE           = 29
DRAWOP_DRAW_ELLIPTIC_ARC     = 30

class DrawOp(object):
    def __init__(self, theOp):
        self._op = theOp

    def GetOp(self):
        return self._op

    def GetPerimeterPoint(self, x1, y1, x2, y2, xOffset, yOffset, attachmentMode):
        return False

    def Scale(self,scaleX, scaleY):
        pass

    def Translate(self, x, y):
        pass

    def Rotate(self, x, y, theta, sinTheta, cosTheta):
        pass
    
class OpSetGDI(DrawOp):
    """Set font, brush, text colour."""
    def __init__(self, theOp, theImage, theGdiIndex, theMode = 0):
        DrawOp.__init__(self, theOp)

        self._gdiIndex = theGdiIndex
        self._image = theImage
        self._mode = theMode

    def Do(self, dc, xoffset = 0, yoffset = 0):
        if self._op == DRAWOP_SET_PEN:
            # Check for overriding this operation for outline colour
            if self._gdiIndex in self._image._outlineColours:
                if self._image._outlinePen:
                    dc.SetPen(self._image._outlinePen)
            else:
                try:
                    dc.SetPen(self._image._gdiObjects[self._gdiIndex])
                except IndexError:
                    pass
        elif self._op == DRAWOP_SET_BRUSH:
            # Check for overriding this operation for outline or fill colour
            if self._gdiIndex in self._image._outlineColours:
                # Need to construct a brush to match the outline pen's colour
                if self._image._outlinePen:
                    br = wx.TheBrushList.FindOrCreateBrush(self._image._outlinePen, wx.SOLID)
                    if br:
                        dc.SetBrush(br)
            elif self._gdiIndex in self._image._fillColours:
                if self._image._fillBrush:
                    dc.SetBrush(self._image._fillBrush)
            else:
                brush = self._image._gdiObjects[self._gdiIndex]
                if brush:
                    dc.SetBrush(brush)
        elif self._op == DRAWOP_SET_FONT:
            try:
                dc.SetFont(self._image._gdiObjects[self._gdiIndex])
            except IndexError:
                pass
        elif self._op == DRAWOP_SET_TEXT_COLOUR:
            dc.SetTextForeground(wx.Colour(self._r, self._g, self._b))
        elif self._op == DRAWOP_SET_BK_COLOUR:
            dc.SetTextBackground(wx.Colour(self._r, self._g, self._b))
        elif self._op == DRAWOP_SET_BK_MODE:
            dc.SetBackgroundMode(self._mode)

    
class OpSetClipping(DrawOp):
    """Set/destroy clipping."""
    def __init__(self, theOp, theX1, theY1, theX2, theY2):
        DrawOp.__init__(self, theOp)

        self._x1 = theX1
        self._y1 = theY1
        self._x2 = theX2
        self._y2 = theY2

    def Do(self, dc, xoffset, yoffset):
        if self._op == DRAWOP_SET_CLIPPING_RECT:
            dc.SetClippingRegion(self._x1 + xoffset, self._y1 + yoffset, self._x2 + xoffset, self._y2 + yoffset)
        elif self._op == DRAWOP_DESTROY_CLIPPING_RECT:
            dc.DestroyClippingRegion()

    def Scale(self, scaleX, scaleY):
        self._x1 *= scaleX
        self._y1 *= scaleY
        self._x2 *= scaleX
        self._y2 *= scaleY

    def Translate(self, x, y):
        self._x1 += x
        self._y1 += y


class OpDraw(DrawOp):
    """Draw line, rectangle, rounded rectangle, ellipse, point, arc, text."""
    def __init__(self, theOp, theX1, theY1, theX2, theY2, theRadius = 0.0, s = ""):
        DrawOp.__init__(self, theOp)

        self._x1 = theX1
        self._y1 = theY1
        self._x2 = theX2
        self._y2 = theY2
        self._x3 = 0.0
        self._y3 = 0.0
        self._radius = theRadius
        self._textString = s

    def Do(self, dc, xoffset, yoffset):
        if self._op == DRAWOP_DRAW_LINE:
            dc.DrawLine(self._x1 + xoffset, self._y1 + yoffset, self._x2 + xoffset, self._y2 + yoffset)
        elif self._op == DRAWOP_DRAW_RECT:
            dc.DrawRectangle(self._x1 + xoffset, self._y1 + yoffset, self._x2, self._y2)
        elif self._op == DRAWOP_DRAW_ROUNDED_RECT:
            dc.DrawRoundedRectangle(self._x1 + xoffset, self._y1 + yoffset, self._x2, self._y2, self._radius)
        elif self._op == DRAWOP_DRAW_ELLIPSE:
            dc.DrawEllipse(self._x1 + xoffset, self._y1 + yoffset, self._x2, self._y2)
        elif self._op == DRAWOP_DRAW_ARC:
            dc.DrawArc(self._x2 + xoffset, self._y2 + yoffset, self._x3 + xoffset, self._y3 + yoffset, self._x1 + xoffset, self._y1 + yoffset)
        elif self._op == DRAWOP_DRAW_ELLIPTIC_ARC:
            dc.DrawEllipticArc(self._x1 + xoffset, self._y1 + yoffset, self._x2, self._y2, self._x3 * 360 / (2 * math.pi), self._y3 * 360 / (2 * math.pi))
        elif self._op == DRAWOP_DRAW_POINT:
            dc.DrawPoint(self._x1 + xoffset, self._y1 + yoffset)
        elif self._op == DRAWOP_DRAW_TEXT:
            dc.DrawText(self._textString, self._x1 + xoffset, self._y1 + yoffset)
    def Scale(self, scaleX, scaleY):
        self._x1 *= scaleX
        self._y1 *= scaleY
        self._x2 *= scaleX
        self._y2 *= scaleY

        if self._op != DRAWOP_DRAW_ELLIPTIC_ARC:
            self._x3 *= scaleX
            self._y3 *= scaleY

        self._radius *= scaleX
        
    def Translate(self, x, y):
        self._x1 += x
        self._y1 += y

        if self._op == DRAWOP_DRAW_LINE:
            self._x2 += x
            self._y2 += y
        elif self._op == DRAWOP_DRAW_ARC:
            self._x2 += x
            self._y2 += y
            self._x3 += x
            self._y3 += y

    def Rotate(self, x, y, theta, sinTheta, cosTheta):
        newX1 = self._x1 * cosTheta + self._y1 * sinTheta + x * (1 - cosTheta) + y * sinTheta
        newY1 = self._x1 * sinTheta + self._y1 * cosTheta + y * (1 - cosTheta) + x * sinTheta

        if self._op == DRAWOP_DRAW_LINE:
            newX2 = self._x2 * cosTheta - self._y2 * sinTheta + x * (1 - cosTheta) + y * sinTheta
            newY2 = self._x2 * sinTheta + self._y2 * cosTheta + y * (1 - cosTheta) + x * sinTheta;

            self._x1 = newX1
            self._y1 = newY1
            self._x2 = newX2
            self._y2 = newY2

        elif self._op in [DRAWOP_DRAW_RECT, DRAWOP_DRAW_ROUNDED_RECT, DRAWOP_DRAW_ELLIPTIC_ARC]:
            # Assume only 0, 90, 180, 270 degree rotations.
            # oldX1, oldY1 represents the top left corner. Find the
            # bottom right, and rotate that. Then the width/height is
            # the difference between x/y values.
            oldBottomRightX = self._x1 + self._x2
            oldBottomRightY = self._y1 + self._y2
            newBottomRightX = oldBottomRightX * cosTheta - oldBottomRightY * sinTheta + x * (1 - cosTheta) + y * sinTheta
            newBottomRightY = oldBottomRightX * sinTheta + oldBottomRightY * cosTheta + y * (1 - cosTheta) + x * sinTheta

            # Now find the new top-left, bottom-right coordinates.
            minX = min(newX1, newBottomRightX)
            minY = min(newY1, newBottomRightY)
            maxX = max(newX1, newBottomRightX)
            maxY = max(newY1, newBottomRightY)

            self._x1 = minX
            self._y1 = minY
            self._x2 = maxX - minX # width
            self._y2 = maxY - minY # height

            if self._op == DRAWOP_DRAW_ELLIPTIC_ARC:
                # Add rotation to angles
                self._x3 += theta
                self._y3 += theta
        elif self._op == DRAWOP_DRAW_ARC:
            newX2 = self._x2 * cosTheta - self._y2 * sinTheta + x * (1 - cosTheta) + y * sinTheta
            newY2 = self._x2 * sinTheta + self._y2 * cosTheta + y * (1 - cosTheta) + x * sinTheta
            newX3 = self._x3 * cosTheta - self._y3 * sinTheta + x * (1 - cosTheta) + y * sinTheta
            newY3 = self._x3 * sinTheta + self._y3 * cosTheta + y * (1 - cosTheta) + x * sinTheta

            self._x1 = newX1
            self._y1 = newY1
            self._x2 = newX2
            self._y2 = newY2
            self._x3 = newX3
            self._y3 = newY3


class OpPolyDraw(DrawOp):
    """Draw polygon, polyline, spline."""
    def __init__(self, theOp, thePoints):
        DrawOp.__init__(self, theOp)

        self._noPoints = len(thePoints)
        self._points = thePoints

    def Do(self, dc, xoffset, yoffset):
        if self._op == DRAWOP_DRAW_POLYLINE:
            dc.DrawLines(self._points, xoffset, yoffset)
        elif self._op == DRAWOP_DRAW_POLYGON:
            dc.DrawPolygon(self._points, xoffset, yoffset)
        elif self._op == DRAWOP_DRAW_SPLINE:
            dc.DrawSpline(self._points) # no offsets in DrawSpline

    def Scale(self, scaleX, scaleY):
        for i in range(self._noPoints):
            self._points[i] = wx.Point(self._points[i][0] * scaleX, self._points[i][1] * scaleY)

    def Translate(self, x, y):
        for i in range(self._noPoints):
            self._points[i][0] += x
            self._points[i][1] += y

    def Rotate(self, x, y, theta, sinTheta, cosTheta):
        for i in range(self._noPoints):
            x1 = self._points[i][0]
            y1 = self._points[i][1]

            self._points[i] = x1 * cosTheta - y1 * sinTheta + x * (1 - cosTheta) + y * sinTheta, x1 * sinTheta + y1 * cosTheta + y * (1 - cosTheta) + x * sinTheta

    def OnDrawOutline(self, dc, x, y, w, h, oldW, oldH):
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        # Multiply all points by proportion of new size to old size
        x_proportion = abs(w / oldW)
        y_proportion = abs(h / oldH)

        dc.DrawPolygon([(x_proportion * x, y_proportion * y) for x, y in self._points], x, y)

    def GetPerimeterPoint(self, x1, y1, x2, y2, xOffset, yOffset, attachmentMode):
        # First check for situation where the line is vertical,
        # and we would want to connect to a point on that vertical --
        # oglFindEndForPolyline can't cope with this (the arrow
        # gets drawn to the wrong place).
        if attachmentMode == ATTACHMENT_MODE_NONE and x1 == x2:
            # Look for the point we'd be connecting to. This is
            # a heuristic...
            for point in self._points:
                if point[0] == 0:
                    if y2 > y1 and point[1] > 0:
                        return point[0]+xOffset, point[1]+yOffset
                    elif y2 < y1 and point[1] < 0:
                        return point[0]+xOffset, point[1]+yOffset

        return FindEndForPolyline([ p[0] + xOffset for p in self._points ],
                                  [ p[1] + yOffset for p in self._points ],
                                  x1, y1, x2, y2)
    
                    
class PseudoMetaFile(object):
    """
    A simple metafile-like class which can load data from a Windows
    metafile on all platforms.
    """
    def __init__(self):
        self._currentRotation = 0
        self._rotateable = True
        self._width = 0.0
        self._height = 0.0
        self._outlinePen = None
        self._fillBrush = None
        self._outlineOp = -1
        
        self._ops = []
        self._gdiObjects = []
        
        self._outlineColours = []
        self._fillColours = []

    def Clear(self):
        self._ops = []
        self._gdiObjects = []
        self._outlineColours = []
        self._fillColours = []
        self._outlineColours = -1

    def IsValid(self):
        return self._ops != []

    def GetOps(self):
        return self._ops

    def SetOutlineOp(self, op):
        self._outlineOp = op

    def GetOutlineOp(self):
        return self._outlineOp

    def SetOutlinePen(self, pen):
        self._outlinePen = pen

    def GetOutlinePen(self, pen):
        return self._outlinePen

    def SetFillBrush(self, brush):
        self._fillBrush = brush

    def GetFillBrush(self):
        return self._fillBrush

    def SetSize(self, w, h):
        self._width = w
        self._height = h

    def SetRotateable(self, rot):
        self._rotateable = rot
        
    def GetRotateable(self):
        return self._rotateable

    def GetFillColours(self):
        return self._fillColours

    def GetOutlineColours(self):
        return self._outlineColours
    
    def Draw(self, dc, xoffset, yoffset):
        for op in self._ops:
            op.Do(dc, xoffset, yoffset)

    def Scale(self, sx, sy):
        for op in self._ops:
            op.Scale(sx, sy)

        self._width *= sx
        self._height *= sy

    def Translate(self, x, y):
        for op in self._ops:
            op.Translate(x, y)

    def Rotate(self, x, y, theta):
        theta1 = theta - self._currentRotation
        if theta1 == 0:
            return

        cosTheta = math.cos(theta1)
        sinTheta = math.sin(theta1)

        for op in self._ops:
            op.Rotate(x, y, theta, sinTheta, cosTheta)

        self._currentRotation = theta

    def LoadFromMetaFile(self, filename, rwidth, rheight):
        if not os.path.exist(filename):
            return False

        print "LoadFromMetaFile not implemented yet."
        return False # TODO

    # Scale to fit size
    def ScaleTo(self, w, h):
        scaleX = w / self._width
        scaleY = h / self._height

        self.Scale(scaleX, scaleY)

    def GetBounds(self):
        maxX, maxY, minX, minY = -99999.9, -99999.9, 99999.9, 99999.9

        for op in self._ops:
            if op.GetOp() in [DRAWOP_DRAW_LINE, DRAWOP_DRAW_RECT, DRAWOP_DRAW_ROUNDED_RECT, DRAWOP_DRAW_ELLIPSE, DRAWOP_DRAW_POINT, DRAWOP_DRAW_TEXT]:
                if op._x1 < minX:
                    minX = op._x1
                if op._x1 > maxX:
                    maxX = op._x1
                if op._y1 < minY:
                    minY = op._y1
                if op._y1 > maxY:
                    maxY = op._y1
                if op.GetOp() == DRAWOP_DRAW_LINE:
                    if op._x2 < minX:
                        minX = op._x2
                    if op._x2 > maxX:
                        maxX = op._x2
                    if op._y2 < minY:
                        minY = op._y2
                    if op._y2 > maxY:
                        maxY = op._y2
                elif op.GetOp() in [ DRAWOP_DRAW_RECT, DRAWOP_DRAW_ROUNDED_RECT, DRAWOP_DRAW_ELLIPSE]:
                    if op._x1 + op._x2 < minX:
                        minX = op._x1 + op._x2
                    if op._x1 + op._x2 > maxX:
                        maxX = op._x1 + op._x2
                    if op._y1 + op._y2 < minY:
                        minY = op._y1 + op._y2
                    if op._y1 + op._y2 > maxX:
                        maxY = op._y1 + op._y2
            elif op.GetOp() == DRAWOP_DRAW_ARC:
                # TODO: don't yet know how to calculate the bounding box
                # for an arc. So pretend it's a line; to get a correct
                # bounding box, draw a blank rectangle first, of the 
                # correct size.
                if op._x1 < minX:
                    minX = op._x1
                if op._x1 > maxX:
                    maxX = op._x1
                if op._y1 < minY:
                    minY = op._y1
                if op._y1 > maxY:
                    maxY = op._y1
                if op._x2 < minX:
                    minX = op._x2
                if op._x2 > maxX:
                    maxX = op._x2
                if op._y2 < minY:
                    minY = op._y2
                if op._y2 > maxY:
                    maxY = op._y2
            elif op.GetOp() in [DRAWOP_DRAW_POLYLINE, DRAWOP_DRAW_POLYGON, DRAWOP_DRAW_SPLINE]:
                for point in op._points:
                    if point[0] < minX:
                        minX = point[0]
                    if point[0] > maxX:
                        maxX = point[0]
                    if point[1] < minY:
                        minY = point[1]
                    if point[1] > maxY:
                        maxY = point[1]

        return [minX, minY, maxX, maxY]

    # Calculate size from current operations
    def CalculateSize(self, shape):
        boundMinX, boundMinY, boundMaxX, boundMaxY = self.GetBounds()

        # By Pierre Hjälm: This is NOT in the old version, which
        # gets this totally wrong. Since the drawing is centered, we
        # cannot get the width by measuring from left to right, we
        # must instead make enough room to handle the largest
        # coordinates
        #self.SetSize(boundMaxX - boundMinX, boundMaxY - boundMinY)

        w = max(abs(boundMinX), abs(boundMaxX)) * 2
        h = max(abs(boundMinY), abs(boundMaxY)) * 2
        
        self.SetSize(w, h)
        
        if shape:
            shape.SetWidth(self._width)
            shape.SetHeight(self._height)

    # Set of functions for drawing into a pseudo metafile
    def DrawLine(self, pt1, pt2):
        op = OpDraw(DRAWOP_DRAW_LINE, pt1[0], pt1[1], pt2[0], pt2[1])
        self._ops.append(op)

    def DrawRectangle(self, rect):
        op = OpDraw(DRAWOP_DRAW_RECT, rect[0], rect[1], rect[2], rect[3])
        self._ops.append(op)

    def DrawRoundedRectangle(self, rect, radius):
        op = OpDraw(DRAWOP_DRAW_ROUNDED_RECT, rect[0], rect[1], rect[2], rect[3])
        op._radius = radius
        self._ops.append(op)

    def DrawEllipse(self, rect):
        op = OpDraw(DRAWOP_DRAW_ELLIPSE, rect[0], rect[1], rect[2], rect[3])
        self._ops.append(op)

    def DrawArc(self, centrePt, startPt, endPt):
        op = OpDraw(DRAWOP_DRAW_ARC, centrePt[0], centrePt[1], startPt[0], startPt[1])
        op._x3, op._y3 = endPt

        self._ops.append(op)

    def DrawEllipticArc(self, rect, startAngle, endAngle):
        startAngleRadians = startAngle * math.pi * 2 / 360
        endAngleRadians = endAngle * math.pi * 2 / 360

        op = OpDraw(DRAWOP_DRAW_ELLIPTIC_ARC, rect[0], rect[1], rect[2], rect[3])
        op._x3 = startAngleRadians
        op._y3 = endAngleRadians

        self._ops.append(op)

    def DrawPoint(self, pt):
        op = OpDraw(DRAWOP_DRAW_POINT, pt[0], pt[1], 0, 0)
        self._ops.append(op)

    def DrawText(self, text, pt):
        op = OpDraw(DRAWOP_DRAW_TEXT, pt[0], pt[1], 0, 0)
        op._textString = text
        self._ops.append(op)

    def DrawLines(self, pts):
        op = OpPolyDraw(DRAWOP_DRAW_POLYLINE, pts)
        self._ops.append(op)

    # flags:
    # oglMETAFLAGS_OUTLINE: will be used for drawing the outline and
    #                       also drawing lines/arrows at the circumference.
    # oglMETAFLAGS_ATTACHMENTS: will be used for initialising attachment
    #                       points at the vertices (perhaps a rare case...)
    def DrawPolygon(self, pts, flags = 0):
        op = OpPolyDraw(DRAWOP_DRAW_POLYGON, pts)
        self._ops.append(op)

        if flags & METAFLAGS_OUTLINE:
            self._outlineOp = len(self._ops) - 1

    def DrawSpline(self, pts):
        op = OpPolyDraw(DRAWOP_DRAW_SPLINE, pts)
        self._ops.append(op)

    def SetClippingRect(self, rect):
        OpSetClipping(DRAWOP_SET_CLIPPING_RECT, rect[0], rect[1], rect[2], rect[3])

    def DestroyClippingRect(self):
        op = OpSetClipping(DRAWOP_DESTROY_CLIPPING_RECT, 0, 0, 0, 0)
        self._ops.append(op)

    def SetPen(self, pen, isOutline = False):
        self._gdiObjects.append(pen)
        op = OpSetGDI(DRAWOP_SET_PEN, self, len(self._gdiObjects) - 1)
        self._ops.append(op)

        if isOutline:
            self._outlineColours.append(len(self._gdiObjects) - 1)

    def SetBrush(self, brush, isFill = False):
        self._gdiObjects.append(brush)
        op = OpSetGDI(DRAWOP_SET_BRUSH, self, len(self._gdiObjects) - 1)
        self._ops.append(op)

        if isFill:
            self._fillColours.append(len(self._gdiObjects) - 1)

    def SetFont(self, font):
        self._gdiObjects.append(font)
        op = OpSetGDI(DRAWOP_SET_FONT, self, len(self._gdiObjects) - 1)
        self._ops.append(op)

    def SetTextColour(self, colour):
        op = OpSetGDI(DRAWOP_SET_TEXT_COLOUR, self, 0)
        op._r, op._g, op._b = colour.Red(), colour.Green(), colour.Blue()

        self._ops.append(op)

    def SetBackgroundColour(self, colour):
        op = OpSetGDI(DRAWOP_SET_BK_COLOUR, self, 0)
        op._r, op._g, op._b = colour.Red(), colour.Green(), colour.Blue()

        self._ops.append(op)

    def SetBackgroundMode(self, mode):
        op = OpSetGDI(DRAWOP_SET_BK_MODE, self, 0)
        self._ops.append(op)
        
class DrawnShape(RectangleShape):
    """
    Draws a pseudo-metafile shape, which can be loaded from a simple
    Windows metafile.

    wxDrawnShape allows you to specify a different shape for each of four
    orientations (North, West, South and East). It also provides a set of
    drawing functions for programmatic drawing of a shape, so that during
    construction of the shape you can draw into it as if it were a device
    context.

    Derived from:
      RectangleShape
    """
    def __init__(self):
        RectangleShape.__init__(self, 100, 50)
        self._saveToFile = True
        self._currentAngle = DRAWN_ANGLE_0
        
        self._metafiles=PseudoMetaFile(), PseudoMetaFile(), PseudoMetaFile(), PseudoMetaFile()

    def OnDraw(self, dc):
        # Pass pen and brush in case we have force outline
        # and fill colours
        if self._shadowMode != SHADOW_NONE:
            if self._shadowBrush:
                self._metafiles[self._currentAngle]._fillBrush = self._shadowBrush
            self._metafiles[self._currentAngle]._outlinePen = wx.Pen(wx.WHITE, 1, wx.TRANSPARENT)
            self._metafiles[self._currentAngle].Draw(dc, self._xpos + self._shadowOffsetX, self._ypos + self._shadowOffsetY)

        self._metafiles[self._currentAngle]._outlinePen = self._pen
        self._metafiles[self._currentAngle]._fillBrush = self._brush
        self._metafiles[self._currentAngle].Draw(dc, self._xpos, self._ypos)

    def SetSize(self, w, h, recursive = True):
        self.SetAttachmentSize(w, h)

        if self.GetWidth() == 0.0:
            scaleX = 1
        else:
            scaleX = w / self.GetWidth()

        if self.GetHeight() == 0.0:
            scaleY = 1
        else:
            scaleY = h / self.GetHeight()

        for i in range(4):
            if self._metafiles[i].IsValid():
                self._metafiles[i].Scale(scaleX, scaleY)

        self._width = w
        self._height = h
        self.SetDefaultRegionSize()

    def Scale(self, sx, sy):
        """Scale the shape by the given amount."""
        for i in range(4):
            if self._metafiles[i].IsValid():
                self._metafiles[i].Scale(sx, sy)
                self._metafiles[i].CalculateSize(self)

    def Translate(self, x, y):
        """Translate the shape by the given amount."""
        for i in range(4):
            if self._metafiles[i].IsValid():
                self._metafiles[i].Translate(x, y)
                self._metafiles[i].CalculateSize(self)

    # theta is absolute rotation from the zero position
    def Rotate(self, x, y, theta):
        """Rotate about the given axis by the given amount in radians."""
        self._currentAngle = self.DetermineMetaFile(theta)

        if self._currentAngle == 0:
            # Rotate metafile
            if not self._metafiles[0].GetRotateable():
                return

            self._metafiles[0].Rotate(x, y, theta)

        actualTheta = theta - self._rotation

        # Rotate attachment points
        sinTheta = math.sin(actualTheta)
        cosTheta = math.cos(actualTheta)

        for point in self._attachmentPoints:
            x1 = point._x
            y1 = point._y

            point._x = x1 * cosTheta - y1 * sinTheta + x * (1.0 - cosTheta) + y * sinTheta
            point._y = x1 * sinTheta + y1 * cosTheta + y * (1.0 - cosTheta) + x * sinTheta

        self._rotation = theta

        self._metafiles[self._currentAngle].CalculateSize(self)

    # Which metafile do we use now? Based on current rotation and validity
    # of metafiles.
    def DetermineMetaFile(self, rotation):
        tolerance = 0.0001
        angles = [0.0, math.pi / 2, math.pi, 3 * math.pi / 2]

        whichMetaFile = 0

        for i in range(4):
            if RoughlyEqual(rotation, angles[i], tolerance):
                whichMetaFile = i
                break

        if whichMetaFile > 0 and not self._metafiles[whichMetaFile].IsValid():
            whichMetaFile = 0

        return whichMetaFile

    def OnDrawOutline(self, dc, x, y, w, h):
        if self._metafiles[self._currentAngle].GetOutlineOp() != -1:
            op = self._metafiles[self._currentAngle].GetOps()[self._metafiles[self._currentAngle].GetOutlineOp()]
            if op.OnDrawOutline(dc, x, y, w, h, self._width, self._height):
                return

        # Default... just use a rectangle
        RectangleShape.OnDrawOutline(self, dc, x, y, w, h)

    # Get the perimeter point using the special outline op, if there is one,
    # otherwise use default wxRectangleShape scheme
    def GetPerimeterPoint(self, x1, y1, x2, y2):
        if self._metafiles[self._currentAngle].GetOutlineOp() != -1:
            op = self._metafiles[self._currentAngle].GetOps()[self._metafiles[self._currentAngle].GetOutlineOp()]
            p = op.GetPerimeterPoint(x1, y1, x2, y2, self.GetX(), self.GetY(), self.GetAttachmentMode())
            if p:
                return p
            
        return RectangleShape.GetPerimeterPoint(self, x1, y1, x2, y2)

    def LoadFromMetaFile(self, filename):
        """Load a (very simple) Windows metafile, created for example by
        Top Draw, the Windows shareware graphics package."""
        return self._metafiles[0].LoadFromMetaFile(filename)

    # Set of functions for drawing into a pseudo metafile.
    # They use integers, but doubles are used internally for accuracy
    # when scaling.
    def DrawLine(self, pt1, pt2):
        self._metafiles[self._currentAngle].DrawLine(pt1, pt2)

    def DrawRectangle(self, rect):
        self._metafiles[self._currentAngle].DrawRectangle(rect)

    def DrawRoundedRectangle(self, rect, radius):
        """Draw a rounded rectangle.

        radius is the corner radius. If radius is negative, it expresses
        the radius as a proportion of the smallest dimension of the rectangle.
        """
        self._metafiles[self._currentAngle].DrawRoundedRectangle(rect, radius)

    def DrawEllipse(self, rect):
        self._metafiles[self._currentAngle].DrawEllipse(rect)

    def DrawArc(self, centrePt, startPt, endPt):
        """Draw an arc."""
        self._metafiles[self._currentAngle].DrawArc(centrePt, startPt, endPt)

    def DrawEllipticArc(self, rect, startAngle, endAngle):
        """Draw an elliptic arc."""
        self._metafiles[self._currentAngle].DrawEllipticArc(rect, startAngle, endAngle)

    def DrawPoint(self, pt):
        self._metafiles[self._currentAngle].DrawPoint(pt)

    def DrawText(self, text, pt):
        self._metafiles[self._currentAngle].DrawText(text, pt)

    def DrawLines(self, pts):
        self._metafiles[self._currentAngle].DrawLines(pts)

    def DrawPolygon(self, pts, flags = 0):
        """Draw a polygon.

        flags can be one or more of:
        METAFLAGS_OUTLINE (use this polygon for the drag outline) and
        METAFLAGS_ATTACHMENTS (use the vertices of this polygon for attachments).
        """
        if flags and METAFLAGS_ATTACHMENTS:
            self.ClearAttachments()
            for i in range(len(pts)):
                self._attachmentPoints.append(AttachmentPoint(i,pts[i][0],pts[i][1]))
        self._metafiles[self._currentAngle].DrawPolygon(pts, flags)

    def DrawSpline(self, pts):
        self._metafiles[self._currentAngle].DrawSpline(pts)

    def SetClippingRect(self, rect):
        """Set the clipping rectangle."""
        self._metafiles[self._currentAngle].SetClippingRect(rect)

    def DestroyClippingRect(self):
        """Destroy the clipping rectangle."""
        self._metafiles[self._currentAngle].DestroyClippingRect()

    def SetDrawnPen(self, pen, isOutline = False):
        """Set the pen for this metafile.

        If isOutline is True, this pen is taken to indicate the outline
        (and if the outline pen is changed for the whole shape, the pen
        will be replaced with the outline pen).
        """
        self._metafiles[self._currentAngle].SetPen(pen, isOutline)

    def SetDrawnBrush(self, brush, isFill = False):
        """Set the brush for this metafile.

        If isFill is True, the brush is used as the fill brush.
        """
        self._metafiles[self._currentAngle].SetBrush(brush, isFill)

    def SetDrawnFont(self, font):
        self._metafiles[self._currentAngle].SetFont(font)

    def SetDrawnTextColour(self, colour):
        """Set the current text colour for the current metafile."""
        self._metafiles[self._currentAngle].SetTextColour(colour)

    def SetDrawnBackgroundColour(self, colour):
        """Set the current background colour for the current metafile."""
        self._metafiles[self._currentAngle].SetBackgroundColour(colour)

    def SetDrawnBackgroundMode(self, mode):
        """Set the current background mode for the current metafile."""
        self._metafiles[self._currentAngle].SetBackgroundMode(mode)

    def CalculateSize(self):
        """Calculate the wxDrawnShape size from the current metafile.

        Call this after you have drawn into the shape.
        """
        self._metafiles[self._currentAngle].CalculateSize(self)

    def DrawAtAngle(self, angle):
        """Set the metafile for the given orientation, which can be one of:

        * DRAWN_ANGLE_0
        * DRAWN_ANGLE_90
        * DRAWN_ANGLE_180
        * DRAWN_ANGLE_270
        """
        self._currentAngle = angle

    def GetAngle(self):
        """Return the current orientation, which can be one of:

        * DRAWN_ANGLE_0
        * DRAWN_ANGLE_90
        * DRAWN_ANGLE_180
        * DRAWN_ANGLE_270
        """
        return self._currentAngle
    
    def GetRotation(self):
        """Return the current rotation of the shape in radians."""
        return self._rotation

    def SetSaveToFile(self, save):
        """If save is True, the image will be saved along with the shape's
        other attributes. The reason why this might not be desirable is that
        if there are many shapes with the same image, it would be more
        efficient for the application to save one copy, and not duplicate
        the information for every shape. The default is True.
        """
        self._saveToFile = save

    def GetMetaFile(self, which = 0):
        """Return a reference to the internal 'pseudo-metafile'."""
        return self._metafiles[which]
