# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------------
# Name:         lines.py
# Purpose:      LineShape class
#
# Author:       Pierre Hjälm (from C++ original by Julian Smart)
#
# Created:      2004-05-08
# RCS-ID:       $Id$
# Copyright:    (c) 2004 Pierre Hjälm - 1998 Julian Smart
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys
import math

from _basic import Shape, ShapeRegion, ShapeTextLine, ControlPoint, RectangleShape
from _oglmisc import *

# Line alignment flags
# Vertical by default
LINE_ALIGNMENT_HORIZ =              1
LINE_ALIGNMENT_VERT =               0
LINE_ALIGNMENT_TO_NEXT_HANDLE =     2
LINE_ALIGNMENT_NONE =               0



class LineControlPoint(ControlPoint):
    def __init__(self, theCanvas = None, object = None, size = 0.0, x = 0.0, y = 0.0, the_type = 0):
        ControlPoint.__init__(self, theCanvas, object, size, x, y, the_type)
        self._xpos = x
        self._ypos = y
        self._type = the_type
        self._point = None
        self._originalPos = None

    def OnDraw(self, dc):
        RectangleShape.OnDraw(self, dc)

    # Implement movement of Line point
    def OnDragLeft(self, draw, x, y, keys = 0, attachment = 0):
        self._shape.GetEventHandler().OnSizingDragLeft(self, draw, x, y, keys, attachment)

    def OnBeginDragLeft(self, x, y, keys = 0, attachment = 0):
        self._shape.GetEventHandler().OnSizingBeginDragLeft(self, x, y, keys, attachment)

    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        self._shape.GetEventHandler().OnSizingEndDragLeft(self, x, y, keys, attachment)



class ArrowHead(object):
    def __init__(self, type = 0, end = 0, size = 0.0, dist = 0.0, name = "", mf = None, arrowId = -1):
        if isinstance(type, ArrowHead):
            pass
        else:
            self._arrowType = type
            self._arrowEnd = end
            self._arrowSize = size
            self._xOffset = dist
            self._yOffset = 0.0
            self._spacing = 5.0

            self._arrowName = name
            self._metaFile = mf
            self._id = arrowId
            if self._id == -1:
                self._id = wx.NewId()
            
    def _GetType(self):
        return self._arrowType

    def GetPosition(self):
        return self._arrowEnd

    def SetPosition(self, pos):
        self._arrowEnd = pos

    def GetXOffset(self):
        return self._xOffset

    def GetYOffset(self):
        return self._yOffset
    
    def GetSpacing(self):
        return self._spacing

    def GetSize(self):
        return self._arrowSize

    def SetSize(self, size):
        self._arrowSize = size
        if self._arrowType == ARROW_METAFILE and self._metaFile:
            oldWidth = self._metaFile._width
            if oldWidth == 0:
                return

            scale = float(size) / oldWidth
            if scale != 1:
                self._metaFile.Scale(scale, scale)
                
    def GetName(self):
        return self._arrowName

    def SetXOffset(self, x):
        self._xOffset = x

    def SetYOffset(self, y):
        self._yOffset = y

    def GetMetaFile(self):
        return self._metaFile

    def GetId(self):
        return self._id

    def GetArrowEnd(self):
        return self._arrowEnd

    def GetArrowSize(self):
        return self._arrowSize

    def SetSpacing(self, sp):
        self._spacing = sp



class LabelShape(RectangleShape):
    def __init__(self, parent, region, w, h):
        RectangleShape.__init__(self, w, h)
        self._lineShape = parent
        self._shapeRegion = region
        self.SetPen(wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT))

    def OnDraw(self, dc):
        if self._lineShape and not self._lineShape.GetDrawHandles():
            return

        x1 = self._xpos - self._width / 2.0
        y1 = self._ypos - self._height / 2.0

        if self._pen:
            if self._pen.GetWidth() == 0:
                dc.SetPen(wx.Pen(wx.WHITE, 1, wx.TRANSPARENT))
            else:
                dc.SetPen(self._pen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        if self._cornerRadius > 0:
            dc.DrawRoundedRectangle(x1, y1, self._width, self._height, self._cornerRadius)
        else:
            dc.DrawRectangle(x1, y1, self._width, self._height)

    def OnDrawContents(self, dc):
        pass

    def OnDragLeft(self, draw, x, y, keys = 0, attachment = 0):
        RectangleShape.OnDragLeft(self, draw, x, y, keys, attachment)

    def OnBeginDragLeft(self, x, y, keys = 0, attachment = 0):
        RectangleShape.OnBeginDragLeft(self, x, y, keys, attachment)

    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        RectangleShape.OnEndDragLeft(self, x, y, keys, attachment)

    def OnMovePre(self, dc, x, y, old_x, old_y, display):
        return self._lineShape.OnLabelMovePre(dc, self, x, y, old_x, old_y, display)

    # Divert left and right clicks to line object
    def OnLeftClick(self, x, y, keys = 0, attachment = 0):
        self._lineShape.GetEventHandler().OnLeftClick(x, y, keys, attachment)

    def OnRightClick(self, x, y, keys = 0, attachment = 0):
        self._lineShape.GetEventHandler().OnRightClick(x, y, keys, attachment)
        


class LineShape(Shape):
    """LineShape may be attached to two nodes;
    it may be segmented, in which case a control point is drawn for each joint.

    A wxLineShape may have arrows at the beginning, end and centre.

    Derived from:
      Shape
    """
    def __init__(self):
        Shape.__init__(self)

        self._sensitivity = OP_CLICK_LEFT | OP_CLICK_RIGHT
        self._draggable = False
        self._attachmentTo = 0
        self._attachmentFrom = 0
        self._from = None
        self._to = None
        self._erasing = False
        self._arrowSpacing = 5.0
        self._ignoreArrowOffsets = False
        self._isSpline = False
        self._maintainStraightLines = False
        self._alignmentStart = 0
        self._alignmentEnd = 0

        self._lineControlPoints = None

        # Clear any existing regions (created in an earlier constructor)
        # and make the three line regions.
        self.ClearRegions()
        for name in ["Middle","Start","End"]:
            newRegion = ShapeRegion()
            newRegion.SetName(name)
            newRegion.SetSize(150, 50)
            self._regions.append(newRegion)

        self._labelObjects = [None, None, None]
        self._lineOrientations = []
        self._lineControlPoints = []
        self._arcArrows = []

    def GetFrom(self):
        """Return the 'from' object."""
        return self._from
    
    def GetTo(self):
        """Return the 'to' object."""
        return self._to

    def GetAttachmentFrom(self):
        """Return the attachment point on the 'from' node."""
        return self._attachmentFrom

    def GetAttachmentTo(self):
        """Return the attachment point on the 'to' node."""
        return self._attachmentTo

    def GetLineControlPoints(self):
        return self._lineControlPoints

    def SetSpline(self, spline):
        """Specifies whether a spline is to be drawn through the control points."""
        self._isSpline = spline

    def IsSpline(self):
        """TRUE if a spline is drawn through the control points."""
        return self._isSpline

    def SetAttachmentFrom(self, attach):
        """Set the 'from' shape attachment."""
        self._attachmentFrom = attach

    def SetAttachmentTo(self, attach):
        """Set the 'to' shape attachment."""
        self._attachmentTo = attach

    # This is really to distinguish between lines and other images.
    # For lines, want to pass drag to canvas, since lines tend to prevent
    # dragging on a canvas (they get in the way.)
    def Draggable(self):
        return False

    def SetIgnoreOffsets(self, ignore):
        """Set whether to ignore offsets from the end of the line when drawing."""
        self._ignoreArrowOffsets = ignore

    def GetArrows(self):
        return self._arcArrows

    def GetAlignmentStart(self):
        return self._alignmentStart

    def GetAlignmentEnd(self):
        return self._alignmentEnd

    def IsEnd(self, nodeObject):
        """TRUE if shape is at the end of the line."""
        return self._to == nodeObject

    def MakeLineControlPoints(self, n):
        """Make a given number of control points (minimum of two)."""
        self._lineControlPoints = []
        
        for _ in range(n):
            point = wx.RealPoint(-999, -999)
            self._lineControlPoints.append(point)

        # pi: added _initialised to keep track of when we have set
        # the middle points to something other than (-999, -999)
        self._initialised = False
        
    def InsertLineControlPoint(self, dc = None, point = None):
        """Insert a control point at an optional given position."""
        if dc:
            self.Erase(dc)

        if point:
            line_x, line_y = point
        else:
            last_point = self._lineControlPoints[-1]
            second_last_point = self._lineControlPoints[-2]

            line_x = (last_point[0] + second_last_point[0]) / 2.0
            line_y = (last_point[1] + second_last_point[1]) / 2.0

        point = wx.RealPoint(line_x, line_y)
        self._lineControlPoints.insert(len(self._lineControlPoints)-1, point)

    def DeleteLineControlPoint(self):
        """Delete an arbitary point on the line."""
        if len(self._lineControlPoints) < 3:
            return False

        del self._lineControlPoints[-2]
        return True

    def Initialise(self):
        """Initialise the line object."""
        if self._lineControlPoints:
            # Just move the first and last control points
            first_point = self._lineControlPoints[0]
            last_point = self._lineControlPoints[-1]

            # If any of the line points are at -999, we must
            # initialize them by placing them half way between the first
            # and the last.

            for i in range(1,len(self._lineControlPoints)):
                point = self._lineControlPoints[i]
                if point[0] == -999:
                    if first_point[0] < last_point[0]:
                        x1 = first_point[0]
                        x2 = last_point[0]
                    else:
                        x2 = first_point[0]
                        x1 = last_point[0]
                    if first_point[1] < last_point[1]:
                        y1 = first_point[1]
                        y2 = last_point[1]
                    else:
                        y2 = first_point[1]
                        y1 = last_point[1]
                    self._lineControlPoints[i] = wx.RealPoint((x2 - x1) / 2.0 + x1, (y2 - y1) / 2.0 + y1)
            self._initialised = True
                    
    def FormatText(self, dc, s, i):
        """Format a text string according to the region size, adding
        strings with positions to region text list.
        """
        self.ClearText(i)

        if len(self._regions) == 0 or i >= len(self._regions):
            return

        region = self._regions[i]
        region.SetText(s)
        dc.SetFont(region.GetFont())

        w, h = region.GetSize()
        # Initialize the size if zero
        if (w == 0 or h == 0) and s:
            w, h = 100, 50
            region.SetSize(w, h)

        string_list = FormatText(dc, s, w - 5, h - 5, region.GetFormatMode())
        for s in string_list:
            line = ShapeTextLine(0.0, 0.0, s)
            region.GetFormattedText().append(line)

        actualW = w
        actualH = h
        if region.GetFormatMode() & FORMAT_SIZE_TO_CONTENTS:
            actualW, actualH = GetCentredTextExtent(dc, region.GetFormattedText(), self._xpos, self._ypos, w, h)
            if actualW != w or actualH != h:
                xx, yy = self.GetLabelPosition(i)
                self.EraseRegion(dc, region, xx, yy)
                if len(self._labelObjects) < i:
                    self._labelObjects[i].Select(False, dc)
                    self._labelObjects[i].Erase(dc)
                    self._labelObjects[i].SetSize(actualW, actualH)

                region.SetSize(actualW, actualH)

                if len(self._labelObjects) < i:
                    self._labelObjects[i].Select(True, dc)
                    self._labelObjects[i].Draw(dc)

        CentreText(dc, region.GetFormattedText(), self._xpos, self._ypos, actualW, actualH, region.GetFormatMode())
        self._formatted = True

    def DrawRegion(self, dc, region, x, y):
        """Format one region at this position."""
        if self.GetDisableLabel():
            return

        w, h = region.GetSize()

        # Get offset from x, y
        xx, yy = region.GetPosition()

        xp = xx + x
        yp = yy + y

        # First, clear a rectangle for the text IF there is any
        if len(region.GetFormattedText()):
            dc.SetPen(self.GetBackgroundPen())
            dc.SetBrush(self.GetBackgroundBrush())

            # Now draw the text
            if region.GetFont():
                dc.SetFont(region.GetFont())
                dc.DrawRectangle(xp - w / 2.0, yp - h / 2.0, w, h)

                if self._pen:
                    dc.SetPen(self._pen)
                dc.SetTextForeground(region.GetActualColourObject())

                DrawFormattedText(dc, region.GetFormattedText(), xp, yp, w, h, region.GetFormatMode())

    def EraseRegion(self, dc, region, x, y):
        """Erase one region at this position."""
        if self.GetDisableLabel():
            return

        w, h = region.GetSize()

        # Get offset from x, y
        xx, yy = region.GetPosition()

        xp = xx + x
        yp = yy + y

        if region.GetFormattedText():
            dc.SetPen(self.GetBackgroundPen())
            dc.SetBrush(self.GetBackgroundBrush())

            dc.DrawRectangle(xp - w / 2.0, yp - h / 2.0, w, h)

    def GetLabelPosition(self, position):
        """Get the reference point for a label.

        Region x and y are offsets from this.
        position is 0 (middle), 1 (start), 2 (end).
        """
        if position == 0:
            # Want to take the middle section for the label
            half_way = int(len(self._lineControlPoints) / 2.0)

            # Find middle of this line
            point = self._lineControlPoints[half_way - 1]
            next_point = self._lineControlPoints[half_way]

            dx = next_point[0] - point[0]
            dy = next_point[1] - point[1]

            return point[0] + dx / 2.0, point[1] + dy / 2.0
        elif position == 1:
            return self._lineControlPoints[0][0], self._lineControlPoints[0][1]
        elif position == 2:
            return self._lineControlPoints[-1][0], self._lineControlPoints[-1][1]

    def Straighten(self, dc = None):
        """Straighten verticals and horizontals."""
        if len(self._lineControlPoints) < 3:
            return

        if dc:
            self.Erase(dc)

        GraphicsStraightenLine(self._lineControlPoints[-1], self._lineControlPoints[-2])

        for i in range(len(self._lineControlPoints) - 2):
            GraphicsStraightenLine(self._lineControlPoints[i], self._lineControlPoints[i + 1])
                
        if dc:
            self.Draw(dc)

    def Unlink(self):
        """Unlink the line from the nodes at either end."""
        if self._to:
            self._to.GetLines().remove(self)
        if self._from:
            self._from.GetLines().remove(self)
        self._to = None
        self._from = None
        for i in range(3):
            if self._labelObjects[i]:
                self._labelObjects[i].Select(False)
                self._labelObjects[i].RemoveFromCanvas(self._canvas)
        self.ClearArrowsAtPosition(-1)

    def SetEnds(self, x1, y1, x2, y2):
        """Set the end positions of the line."""
        self._lineControlPoints[0] = wx.RealPoint(x1, y1)
        self._lineControlPoints[-1] = wx.RealPoint(x2, y2)

        # Find centre point
        self._xpos = (x1 + x2) / 2.0
        self._ypos = (y1 + y2) / 2.0

    # Get absolute positions of ends
    def GetEnds(self):
        """Get the visible endpoints of the lines for drawing between two objects."""
        first_point = self._lineControlPoints[0]
        last_point = self._lineControlPoints[-1]

        return first_point[0], first_point[1], last_point[0], last_point[1]

    def SetAttachments(self, from_attach, to_attach):
        """Specify which object attachment points should be used at each end
        of the line.
        """
        self._attachmentFrom = from_attach
        self._attachmentTo = to_attach

    def HitTest(self, x, y):
        if not self._lineControlPoints:
            return False

        # Look at label regions in case mouse is over a label
        inLabelRegion = False
        for i in range(3):
            if self._regions[i]:
                region = self._regions[i]
                if len(region._formattedText):
                    xp, yp = self.GetLabelPosition(i)
                    # Offset region from default label position
                    cx, cy = region.GetPosition()
                    cw, ch = region.GetSize() 
                    cx += xp
                    cy += yp
                    
                    rLeft = cx - cw / 2.0
                    rTop = cy - ch / 2.0
                    rRight = cx + cw / 2.0
                    rBottom = cy + ch / 2.0
                    if x > rLeft and x < rRight and y > rTop and y < rBottom:
                        inLabelRegion = True
                        break

        for i in range(len(self._lineControlPoints) - 1):
            point1 = self._lineControlPoints[i]
            point2 = self._lineControlPoints[i + 1]

            # For inaccurate mousing allow 8 pixel corridor
            extra = 4

            dx = point2[0] - point1[0]
            dy = point2[1] - point1[1]

            seg_len = math.sqrt(dx * dx + dy * dy)
            if dy == 0 and dx == 0:
                continue
            distance_from_seg = seg_len * float((x - point1[0]) * dy - (y - point1[1]) * dx) / (dy * dy + dx * dx)
            distance_from_prev = seg_len * float((y - point1[1]) * dy + (x - point1[0]) * dx) / (dy * dy + dx * dx)

            if abs(distance_from_seg) < extra and distance_from_prev >= 0 and distance_from_prev <= seg_len or inLabelRegion:
                return 0, distance_from_seg

        return False

    def DrawArrows(self, dc):
        """Draw all arrows."""
        # Distance along line of each arrow: space them out evenly
        startArrowPos = 0.0
        endArrowPos = 0.0
        middleArrowPos = 0.0

        for arrow in self._arcArrows:
            ah = arrow.GetArrowEnd()
            if ah == ARROW_POSITION_START:
                if arrow.GetXOffset() and not self._ignoreArrowOffsets:
                    # If specified, x offset is proportional to line length
                    self.DrawArrow(dc, arrow, arrow.GetXOffset(), True)
                else:
                    self.DrawArrow(dc, arrow, startArrowPos, False)
                    startArrowPos += arrow.GetSize() + arrow.GetSpacing()
            elif ah == ARROW_POSITION_END:
                if arrow.GetXOffset() and not self._ignoreArrowOffsets:
                    self.DrawArrow(dc, arrow, arrow.GetXOffset(), True)
                else:
                    self.DrawArrow(dc, arrow, endArrowPos, False)
                    endArrowPos += arrow.GetSize() + arrow.GetSpacing()
            elif ah == ARROW_POSITION_MIDDLE:
                arrow.SetXOffset(middleArrowPos)
                if arrow.GetXOffset() and not self._ignoreArrowOffsets:
                    self.DrawArrow(dc, arrow, arrow.GetXOffset(), True)
                else:
                    self.DrawArrow(dc, arrow, middleArrowPos, False)
                    middleArrowPos += arrow.GetSize() + arrow.GetSpacing()

    def DrawArrow(self, dc, arrow, XOffset, proportionalOffset):
        """Draw the given arrowhead (or annotation)."""
        first_line_point = self._lineControlPoints[0]
        second_line_point = self._lineControlPoints[1]

        last_line_point = self._lineControlPoints[-1]
        second_last_line_point = self._lineControlPoints[-2]

        # Position of start point of line, at the end of which we draw the arrow
        startPositionX, startPositionY = 0.0, 0.0

        ap = arrow.GetPosition()
        if ap == ARROW_POSITION_START:
            # If we're using a proportional offset, calculate just where this
            # will be on the line.
            realOffset = XOffset
            if proportionalOffset:
                totalLength = math.sqrt((second_line_point[0] - first_line_point[0]) * (second_line_point[0] - first_line_point[0]) + (second_line_point[1] - first_line_point[1]) * (second_line_point[1] - first_line_point[1]))
                realOffset = XOffset * totalLength

            positionOnLineX, positionOnLineY = GetPointOnLine(second_line_point[0], second_line_point[1], first_line_point[0], first_line_point[1], realOffset)
            
            startPositionX = second_line_point[0]
            startPositionY = second_line_point[1]
        elif ap == ARROW_POSITION_END:
            # If we're using a proportional offset, calculate just where this
            # will be on the line.
            realOffset = XOffset
            if proportionalOffset:
                totalLength = math.sqrt((second_last_line_point[0] - last_line_point[0]) * (second_last_line_point[0] - last_line_point[0]) + (second_last_line_point[1] - last_line_point[1]) * (second_last_line_point[1] - last_line_point[1]));
                realOffset = XOffset * totalLength
            
            positionOnLineX, positionOnLineY = GetPointOnLine(second_last_line_point[0], second_last_line_point[1], last_line_point[0], last_line_point[1], realOffset)
            
            startPositionX = second_last_line_point[0]
            startPositionY = second_last_line_point[1]
        elif ap == ARROW_POSITION_MIDDLE:
            # Choose a point half way between the last and penultimate points
            x = (last_line_point[0] + second_last_line_point[0]) / 2.0
            y = (last_line_point[1] + second_last_line_point[1]) / 2.0

            # If we're using a proportional offset, calculate just where this
            # will be on the line.
            realOffset = XOffset
            if proportionalOffset:
                totalLength = math.sqrt((second_last_line_point[0] - x) * (second_last_line_point[0] - x) + (second_last_line_point[1] - y) * (second_last_line_point[1] - y));
                realOffset = XOffset * totalLength

            positionOnLineX, positionOnLineY = GetPointOnLine(second_last_line_point[0], second_last_line_point[1], x, y, realOffset)
            startPositionX = second_last_line_point[0]
            startPositionY = second_last_line_point[1]

        # Add yOffset to arrow, if any

        # The translation that the y offset may give
        deltaX = 0.0
        deltaY = 0.0
        if arrow.GetYOffset and not self._ignoreArrowOffsets:
            #                             |(x4, y4)
            #                             |d
            #                             |
            #   (x1, y1)--------------(x3, y3)------------------(x2, y2)
            #   x4 = x3 - d * math.sin(theta)
            #   y4 = y3 + d * math.cos(theta)
            #
            #   Where theta = math.tan(-1) of (y3-y1) / (x3-x1)
            x1 = startPositionX
            y1 = startPositionY
            x3 = float(positionOnLineX)
            y3 = float(positionOnLineY)
            d = -arrow.GetYOffset() # Negate so +offset is above line

            if x3 == x1:
                theta = math.pi / 2.0
            else:
                theta = math.atan((y3 - y1) / (x3 - x1))

            x4 = x3 - d * math.sin(theta)
            y4 = y3 + d * math.cos(theta)
            
            deltaX = x4 - positionOnLineX
            deltaY = y4 - positionOnLineY

        at = arrow._GetType()
        if at == ARROW_ARROW:
            arrowLength = arrow.GetSize()
            arrowWidth = arrowLength / 3.0

            tip_x, tip_y, side1_x, side1_y, side2_x, side2_y = GetArrowPoints(startPositionX + deltaX, startPositionY + deltaY, positionOnLineX + deltaX, positionOnLineY + deltaY, arrowLength, arrowWidth)

            points = [[tip_x, tip_y],
                    [side1_x, side1_y],
                    [side2_x, side2_y],
                    [tip_x, tip_y]]

            dc.SetPen(self._pen)
            dc.SetBrush(self._brush)
            dc.DrawPolygon(points)
        elif at in [ARROW_HOLLOW_CIRCLE, ARROW_FILLED_CIRCLE]:
            # Find point on line of centre of circle, which is a radius away
            # from the end position
            diameter = arrow.GetSize()
            x, y = GetPointOnLine(startPositionX + deltaX, startPositionY + deltaY,
                               positionOnLineX + deltaX, positionOnLineY + deltaY,
                               diameter / 2.0)
            x1 = x - diameter / 2.0
            y1 = y - diameter / 2.0
            dc.SetPen(self._pen)
            if arrow._GetType() == ARROW_HOLLOW_CIRCLE:
                dc.SetBrush(self.GetBackgroundBrush())
            else:
                dc.SetBrush(self._brush)

            dc.DrawEllipse(x1, y1, diameter, diameter)
        elif at == ARROW_SINGLE_OBLIQUE:
            pass
        elif at == ARROW_METAFILE:
            if arrow.GetMetaFile():
                # Find point on line of centre of object, which is a half-width away
                # from the end position
                #
                #                 width
                #  <-- start pos  <-----><-- positionOnLineX
                #                 _____
                #  --------------|  x  | <-- e.g. rectangular arrowhead
                #                 -----
                #
                x, y = GetPointOnLine(startPositionX, startPositionY,
                                   positionOnLineX, positionOnLineY,
                                   arrow.GetMetaFile()._width / 2.0)
                # Calculate theta for rotating the metafile.
                #
                # |
                # |     o(x2, y2)   'o' represents the arrowhead.
                # |    /
                # |   /
                # |  /theta
                # | /(x1, y1)
                # |______________________
                #
                theta = 0.0
                x1 = startPositionX
                y1 = startPositionY
                x2 = float(positionOnLineX)
                y2 = float(positionOnLineY)

                if x1 == x2 and y1 == y2:
                    theta = 0.0
                elif x1 == x2 and y1 > y2:
                    theta = 3.0 * math.pi / 2.0
                elif x1 == x2 and y2 > y1:
                    theta = math.pi / 2.0
                elif x2 > x1 and y2 >= y1:
                    theta = math.atan((y2 - y1) / (x2 - x1))
                elif x2 < x1:
                    theta = math.pi + math.atan((y2 - y1) / (x2 - x1))
                elif x2 > x1 and y2 < y1:
                    theta = 2 * math.pi + math.atan((y2 - y1) / (x2 - x1))
                else:
                    raise "Unknown arrowhead rotation case"

                # Rotate about the centre of the object, then place
                # the object on the line.
                if arrow.GetMetaFile().GetRotateable():
                    arrow.GetMetaFile().Rotate(0.0, 0.0, theta)

                if self._erasing:
                    # If erasing, just draw a rectangle
                    minX, minY, maxX, maxY = arrow.GetMetaFile().GetBounds()
                    # Make erasing rectangle slightly bigger or you get droppings
                    extraPixels = 4
                    dc.DrawRectangle(deltaX + x + minX - extraPixels / 2.0, deltaY + y + minY - extraPixels / 2.0, maxX - minX + extraPixels, maxY - minY + extraPixels)
                else:
                    arrow.GetMetaFile().Draw(dc, x + deltaX, y + deltaY)

    def OnErase(self, dc):
        old_pen = self._pen
        old_brush = self._brush

        bg_pen = self.GetBackgroundPen()
        bg_brush = self.GetBackgroundBrush()
        self.SetPen(bg_pen)
        self.SetBrush(bg_brush)

        bound_x, bound_y = self.GetBoundingBoxMax()
        if self._font:
            dc.SetFont(self._font)

        # Undraw text regions
        for i in range(3):
            if self._regions[i]:
                x, y = self.GetLabelPosition(i)
                self.EraseRegion(dc, self._regions[i], x, y)

        # Undraw line
        dc.SetPen(self.GetBackgroundPen())
        dc.SetBrush(self.GetBackgroundBrush())

        # Drawing over the line only seems to work if the line has a thickness
        # of 1.
        if old_pen and old_pen.GetWidth() > 1:
            dc.DrawRectangle(self._xpos - bound_x / 2.0 - 2, self._ypos - bound_y / 2.0 - 2,
                             bound_x + 4, bound_y + 4)
        else:
            self._erasing = True
            self.GetEventHandler().OnDraw(dc)
            self.GetEventHandler().OnEraseControlPoints(dc)
            self._erasing = False

        if old_pen:
            self.SetPen(old_pen)
        if old_brush:
            self.SetBrush(old_brush)

    def GetBoundingBoxMin(self):
        x1, y1 = 10000, 10000
        x2, y2 = -10000, -10000

        for point in self._lineControlPoints:
            if point[0] < x1:
                x1 = point[0]
            if point[1] < y1:
                y1 = point[1]
            if point[0] > x2:
                x2 = point[0]
            if point[1] > y2:
                y2 = point[1]

        return x2 - x1, y2 - y1
        
    # For a node image of interest, finds the position of this arc
    # amongst all the arcs which are attached to THIS SIDE of the node image,
    # and the number of same.
    def FindNth(self, image, incoming):
        """Find the position of the line on the given object.

        Specify whether incoming or outgoing lines are being considered
        with incoming.
        """
        n = -1
        num = 0
        
        if image == self._to:
            this_attachment = self._attachmentTo
        else:
            this_attachment = self._attachmentFrom

        # Find number of lines going into / out of this particular attachment point
        for line in image.GetLines():
            if line._from == image:
                # This is the nth line attached to 'image'
                if line == self and not incoming:
                    n = num

                # Increment num count if this is the same side (attachment number)
                if line._attachmentFrom == this_attachment:
                    num += 1

            if line._to == image:
                # This is the nth line attached to 'image'
                if line == self and incoming:
                    n = num

                # Increment num count if this is the same side (attachment number)
                if line._attachmentTo == this_attachment:
                    num += 1

        return n, num

    def OnDrawOutline(self, dc, x, y, w, h):
        old_pen = self._pen
        old_brush = self._brush

        dottedPen = wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT)
        self.SetPen(dottedPen)
        self.SetBrush(wx.TRANSPARENT_BRUSH)

        self.GetEventHandler().OnDraw(dc)

        if old_pen:
            self.SetPen(old_pen)
        else:
            self.SetPen(None)
        if old_brush:
            self.SetBrush(old_brush)
        else:
            self.SetBrush(None)

    def OnMovePre(self, dc, x, y, old_x, old_y, display = True):
        x_offset = x - old_x
        y_offset = y - old_y

        if self._lineControlPoints and not (x_offset == 0 and y_offset == 0):
            for point in self._lineControlPoints:
                point[0] += x_offset
                point[1] += y_offset

        # Move temporary label rectangles if necessary
        for i in range(3):
            if self._labelObjects[i]:
                self._labelObjects[i].Erase(dc)
                xp, yp = self.GetLabelPosition(i)
                if i < len(self._regions):
                    xr, yr = self._regions[i].GetPosition()
                else:
                    xr, yr = 0, 0
                self._labelObjects[i].Move(dc, xp + xr, yp + yr)
        return True

    def OnMoveLink(self, dc, moveControlPoints = True):
        """Called when a connected object has moved, to move the link to
        correct position
        """
        if not self._from or not self._to:
            return

        # Do each end - nothing in the middle. User has to move other points
        # manually if necessary
        end_x, end_y, other_end_x, other_end_y = self.FindLineEndPoints()

        oldX, oldY = self._xpos, self._ypos

        # pi: The first time we go through FindLineEndPoints we can't
        # use the middle points (since they don't have sane values),
        # so we just do what we do for a normal line. Then we call
        # Initialise to set the middle points, and then FindLineEndPoints
        # again, but this time (and from now on) we use the middle
        # points to calculate the end points.
        # This was buggy in the C++ version too.
        
        self.SetEnds(end_x, end_y, other_end_x, other_end_y)

        if len(self._lineControlPoints) > 2:
            self.Initialise()

        # Do a second time, because one may depend on the other
        end_x, end_y, other_end_x, other_end_y = self.FindLineEndPoints()
        self.SetEnds(end_x, end_y, other_end_x, other_end_y)

        # Try to move control points with the arc
        x_offset = self._xpos - oldX
        y_offset = self._ypos - oldY

        # Only move control points if it's a self link. And only works
        # if attachment mode is ON
        if self._from == self._to and self._from.GetAttachmentMode() != ATTACHMENT_MODE_NONE and moveControlPoints and self._lineControlPoints and not (x_offset == 0 and y_offset == 0):
            for point in self._lineControlPoints[1:-1]:
                point[0] += x_offset
                point[1] += y_offset

        self.Move(dc, self._xpos, self._ypos)

    def FindLineEndPoints(self):
        """Finds the x, y points at the two ends of the line.

        This function can be used by e.g. line-routing routines to
        get the actual points on the two node images where the lines will be
        drawn to / from.
        """
        if not self._from or not self._to:
            return

        # Do each end - nothing in the middle. User has to move other points
        # manually if necessary.
        second_point = self._lineControlPoints[1]
        second_last_point = self._lineControlPoints[-2]

        # pi: If we have a segmented line and this is the first time,
        # do this as a straight line.
        if len(self._lineControlPoints) > 2 and self._initialised:
            if self._from.GetAttachmentMode() != ATTACHMENT_MODE_NONE:
                nth, no_arcs = self.FindNth(self._from, False) # Not incoming
                end_x, end_y = self._from.GetAttachmentPosition(self._attachmentFrom, nth, no_arcs, self)
            else:
                end_x, end_y = self._from.GetPerimeterPoint(self._from.GetX(), self._from.GetY(), second_point[0], second_point[1])

            if self._to.GetAttachmentMode() != ATTACHMENT_MODE_NONE:
                nth, no_arch = self.FindNth(self._to, True) # Incoming
                other_end_x, other_end_y = self._to.GetAttachmentPosition(self._attachmentTo, nth, no_arch, self)
            else:
                other_end_x, other_end_y = self._to.GetPerimeterPoint(self._to.GetX(), self._to.GetY(), second_last_point[0], second_last_point[1])
        else:
            fromX = self._from.GetX()
            fromY = self._from.GetY()
            toX = self._to.GetX()
            toY = self._to.GetY()

            if self._from.GetAttachmentMode() != ATTACHMENT_MODE_NONE:
                nth, no_arcs = self.FindNth(self._from, False)
                end_x, end_y = self._from.GetAttachmentPosition(self._attachmentFrom, nth, no_arcs, self)
                fromX = end_x
                fromY = end_y

            if self._to.GetAttachmentMode() != ATTACHMENT_MODE_NONE:
                nth, no_arcs = self.FindNth(self._to, True)
                other_end_x, other_end_y = self._to.GetAttachmentPosition(self._attachmentTo, nth, no_arcs, self)
                toX = other_end_x
                toY = other_end_y

            if self._from.GetAttachmentMode() == ATTACHMENT_MODE_NONE:
                end_x, end_y = self._from.GetPerimeterPoint(self._from.GetX(), self._from.GetY(), toX, toY)

            if self._to.GetAttachmentMode() == ATTACHMENT_MODE_NONE:
                other_end_x, other_end_y = self._to.GetPerimeterPoint(self._to.GetX(), self._to.GetY(), fromX, fromY)

        return end_x, end_y, other_end_x, other_end_y


    def OnDraw(self, dc):
        if not self._lineControlPoints:
            return

        if self._pen:
            dc.SetPen(self._pen)
        if self._brush:
            dc.SetBrush(self._brush)

        points = []
        for point in self._lineControlPoints:
            points.append(wx.Point(point[0], point[1]))

        if self._isSpline:
            dc.DrawSpline(points)
        else:
            dc.DrawLines(points)

        if sys.platform[:3] == "win":
            # For some reason, last point isn't drawn under Windows
            pt = points[-1]
            dc.DrawPoint(pt[0], pt[1])

        # Problem with pen - if not a solid pen, does strange things
        # to the arrowhead. So make (get) a new pen that's solid.
        if self._pen and self._pen.GetStyle() != wx.SOLID:
            solid_pen = wx.Pen(self._pen.GetColour(), 1, wx.SOLID)
            if solid_pen:
                dc.SetPen(solid_pen)

        self.DrawArrows(dc)

    def OnDrawControlPoints(self, dc):
        if not self._drawHandles:
            return

        # Draw temporary label rectangles if necessary
        for i in range(3):
            if self._labelObjects[i]:
                self._labelObjects[i].Draw(dc)

        Shape.OnDrawControlPoints(self, dc)

    def OnEraseControlPoints(self, dc):
        # Erase temporary label rectangles if necessary
        
        for i in range(3):
            if self._labelObjects[i]:
                self._labelObjects[i].Erase(dc)

        Shape.OnEraseControlPoints(self, dc)

    def OnDragLeft(self, draw, x, y, keys = 0, attachment = 0):
        pass

    def OnBeginDragLeft(self, x, y, keys = 0, attachment = 0):
        pass

    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        pass

    def OnDrawContents(self, dc):
        if self.GetDisableLabel():
            return

        for i in range(3):
            if self._regions[i]:
                x, y = self.GetLabelPosition(i)
                self.DrawRegion(dc, self._regions[i], x, y)

    def SetTo(self, object):
        """Set the 'to' object for the line."""
        self._to = object

    def SetFrom(self, object):
        """Set the 'from' object for the line."""
        self._from = object

    def MakeControlPoints(self):
        """Make handle control points."""
        if self._canvas and self._lineControlPoints:
            first = self._lineControlPoints[0]
            last = self._lineControlPoints[-1]

            control = LineControlPoint(self._canvas, self, CONTROL_POINT_SIZE, first[0], first[1], CONTROL_POINT_ENDPOINT_FROM)
            control._point = first
            self._canvas.AddShape(control)
            self._controlPoints.append(control)

            for point in self._lineControlPoints[1:-1]:
                control = LineControlPoint(self._canvas, self, CONTROL_POINT_SIZE, point[0], point[1], CONTROL_POINT_LINE)
                control._point = point
                self._canvas.AddShape(control)
                self._controlPoints.append(control)

            control = LineControlPoint(self._canvas, self, CONTROL_POINT_SIZE, last[0], last[1], CONTROL_POINT_ENDPOINT_TO)
            control._point = last
            self._canvas.AddShape(control)
            self._controlPoints.append(control)

    def ResetControlPoints(self):
        if self._canvas and self._lineControlPoints and self._controlPoints:
            for i in range(min(len(self._controlPoints), len(self._lineControlPoints))):
                point = self._lineControlPoints[i]
                control = self._controlPoints[i]
                control.SetX(point[0])
                control.SetY(point[1])

    # Override select, to create / delete temporary label-moving objects
    def Select(self, select, dc = None):
        Shape.Select(self, select, dc)
        if select:
            for i in range(3):
                if self._regions[i]:
                    region = self._regions[i]
                    if region._formattedText:
                        w, h = region.GetSize()
                        x, y = region.GetPosition()
                        xx, yy = self.GetLabelPosition(i)

                        if self._labelObjects[i]:
                            self._labelObjects[i].Select(False)
                            self._labelObjects[i].RemoveFromCanvas(self._canvas)

                        self._labelObjects[i] = self.OnCreateLabelShape(self, region, w, h)
                        self._labelObjects[i].AddToCanvas(self._canvas)
                        self._labelObjects[i].Show(True)
                        if dc:
                            self._labelObjects[i].Move(dc, x + xx, y + yy)
                        self._labelObjects[i].Select(True, dc)
        else:
            for i in range(3):
                if self._labelObjects[i]:
                    self._labelObjects[i].Select(False, dc)
                    self._labelObjects[i].Erase(dc)
                    self._labelObjects[i].RemoveFromCanvas(self._canvas)
                    self._labelObjects[i] = None

    # Control points ('handles') redirect control to the actual shape, to
    # make it easier to override sizing behaviour.
    def OnSizingDragLeft(self, pt, draw, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        dc.SetLogicalFunction(OGLRBLF)

        dottedPen = wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT)
        dc.SetPen(dottedPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        if pt._type == CONTROL_POINT_LINE:
            x, y = self._canvas.Snap(x, y)

            pt.SetX(x)
            pt.SetY(y)
            pt._point[0] = x
            pt._point[1] = y
            
            old_pen = self.GetPen()
            old_brush = self.GetBrush()

            self.SetPen(dottedPen)
            self.SetBrush(wx.TRANSPARENT_BRUSH)

            self.GetEventHandler().OnMoveLink(dc, False)
            
            self.SetPen(old_pen)
            self.SetBrush(old_brush)

    def OnSizingBeginDragLeft(self, pt, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        if pt._type == CONTROL_POINT_LINE:
            pt._originalPos = pt._point
            x, y = self._canvas.Snap(x, y)

            self.Erase(dc)

            # Redraw start and end objects because we've left holes
            # when erasing the line
            self.GetFrom().OnDraw(dc)
            self.GetFrom().OnDrawContents(dc)
            self.GetTo().OnDraw(dc)
            self.GetTo().OnDrawContents(dc)

            self.SetDisableLabel(True)
            dc.SetLogicalFunction(OGLRBLF)

            pt._xpos = x
            pt._ypos = y
            pt._point[0] = x
            pt._point[1] = y

            old_pen = self.GetPen()
            old_brush = self.GetBrush()

            dottedPen = wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT)
            self.SetPen(dottedPen)
            self.SetBrush(wx.TRANSPARENT_BRUSH)

            self.GetEventHandler().OnMoveLink(dc, False)

            self.SetPen(old_pen)
            self.SetBrush(old_brush)

        if pt._type == CONTROL_POINT_ENDPOINT_FROM or pt._type == CONTROL_POINT_ENDPOINT_TO:
            self._canvas.SetCursor(wx.StockCursor(wx.CURSOR_BULLSEYE))
            pt._oldCursor = wx.STANDARD_CURSOR

    def OnSizingEndDragLeft(self, pt, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        self.SetDisableLabel(False)

        if pt._type == CONTROL_POINT_LINE:
            x, y = self._canvas.Snap(x, y)

            rpt = wx.RealPoint(x, y)

            # Move the control point back to where it was;
            # MoveControlPoint will move it to the new position
            # if it decides it wants. We only moved the position
            # during user feedback so we could redraw the line
            # as it changed shape.
            pt._xpos = pt._originalPos[0]
            pt._ypos = pt._originalPos[1]
            pt._point[0] = pt._originalPos[0]
            pt._point[1] = pt._originalPos[1]

            self.OnMoveMiddleControlPoint(dc, pt, rpt)

        if pt._type == CONTROL_POINT_ENDPOINT_FROM:
            if pt._oldCursor:
                self._canvas.SetCursor(pt._oldCursor)

            if self.GetFrom():
                self.GetFrom().MoveLineToNewAttachment(dc, self, x, y)

        if pt._type == CONTROL_POINT_ENDPOINT_TO:
            if pt._oldCursor:
                self._canvas.SetCursor(pt._oldCursor)

            if self.GetTo():
                self.GetTo().MoveLineToNewAttachment(dc, self, x, y)

    # This is called only when a non-end control point is moved
    def OnMoveMiddleControlPoint(self, dc, lpt, pt):
        lpt._xpos = pt[0]
        lpt._ypos = pt[1]

        lpt._point[0] = pt[0]
        lpt._point[1] = pt[1]

        self.GetEventHandler().OnMoveLink(dc)

        return True

    def AddArrow(self, type, end = ARROW_POSITION_END, size = 10.0, xOffset = 0.0, name = "", mf = None, arrowId = -1):
        """Add an arrow (or annotation) to the line.

        type may currently be one of:

        ARROW_HOLLOW_CIRCLE
          Hollow circle. 
        ARROW_FILLED_CIRCLE
          Filled circle. 
        ARROW_ARROW
          Conventional arrowhead. 
        ARROW_SINGLE_OBLIQUE
          Single oblique stroke. 
        ARROW_DOUBLE_OBLIQUE
          Double oblique stroke. 
        ARROW_DOUBLE_METAFILE
          Custom arrowhead. 

        end may currently be one of:

        ARROW_POSITION_END
          Arrow appears at the end. 
        ARROW_POSITION_START
          Arrow appears at the start. 

        arrowSize specifies the length of the arrow.

        xOffset specifies the offset from the end of the line.

        name specifies a name for the arrow.

        mf can be a wxPseduoMetaFile, perhaps loaded from a simple Windows
        metafile.

        arrowId is the id for the arrow.
        """
        arrow = ArrowHead(type, end, size, xOffset, name, mf, arrowId)
        self._arcArrows.append(arrow)
        return arrow

    # Add arrowhead at a particular position in the arrowhead list
    def AddArrowOrdered(self, arrow, referenceList, end):
        """Add an arrowhead in the position indicated by the reference list
        of arrowheads, which contains all legal arrowheads for this line, in
        the correct order. E.g.

        Reference list:      a b c d e
        Current line list:   a d

        Add c, then line list is: a c d.

        If no legal arrowhead position, return FALSE. Assume reference list
        is for one end only, since it potentially defines the ordering for
        any one of the 3 positions. So we don't check the reference list for
        arrowhead position.
        """
        if not referenceList:
            return False

        targetName = arrow.GetName()

        # First check whether we need to insert in front of list,
        # because this arrowhead is the first in the reference
        # list and should therefore be first in the current list.
        refArrow = referenceList[0]
        if refArrow.GetName() == targetName:
            self._arcArrows.insert(0, arrow)
            return True

        i1 = i2 = 0
        while i1 < len(referenceList) and i2 < len(self._arcArrows):
            refArrow = referenceList[i1]
            currArrow = self._arcArrows[i2]

            # Matching: advance current arrow pointer
            if currArrow.GetArrowEnd() == end and currArrow.GetName() == refArrow.GetName():
                i2 += 1

            # Check if we're at the correct position in the
            # reference list
            if targetName == refArrow.GetName():
                if i2 < len(self._arcArrows):
                    self._arcArrows.insert(i2, arrow)
                else:
                    self._arcArrows.append(arrow)
                return True
            i1 += 1

        self._arcArrows.append(arrow)
        return True

    def ClearArrowsAtPosition(self, end):
        """Delete the arrows at the specified position, or at any position
        if position is -1.
        """
        if end == -1:
            self._arcArrows = []
            return

        for arrow in self._arcArrows:
            if arrow.GetArrowEnd() == end:
                self._arcArrows.remove(arrow)

    def ClearArrow(self, name):
        """Delete the arrow with the given name."""
        for arrow in self._arcArrows:
            if arrow.GetName() == name:
                self._arcArrows.remove(arrow)
                return True
        return False

    def FindArrowHead(self, position, name):
        """Find arrowhead by position and name.

        if position is -1, matches any position.
        """
        for arrow in self._arcArrows:
            if (position == -1 or position == arrow.GetArrowEnd()) and arrow.GetName() == name:
                return arrow

        return None

    def FindArrowHeadId(self, arrowId):
        """Find arrowhead by id."""
        for arrow in self._arcArrows:
            if arrowId == arrow.GetId():
                return arrow

        return None

    def DeleteArrowHead(self, position, name):
        """Delete arrowhead by position and name.

        if position is -1, matches any position.
        """
        for arrow in self._arcArrows:
            if (position == -1 or position == arrow.GetArrowEnd()) and arrow.GetName() == name:
                self._arcArrows.remove(arrow)
                return True
        return False
    
    def DeleteArrowHeadId(self, id):
        """Delete arrowhead by id."""
        for arrow in self._arcArrows:
            if arrowId == arrow.GetId():
                self._arcArrows.remove(arrow)
                return True
        return False

    # Calculate the minimum width a line
    # occupies, for the purposes of drawing lines in tools.
    def FindMinimumWidth(self):
        """Find the horizontal width for drawing a line with arrows in
        minimum space. Assume arrows at end only.
        """
        minWidth = 0.0
        for arrowHead in self._arcArrows:
            minWidth += arrowHead.GetSize()
            if arrowHead != self._arcArrows[-1]:
                minWidth += arrowHead + GetSpacing

        # We have ABSOLUTE minimum now. So
        # scale it to give it reasonable aesthetics
        # when drawing with line.
        if minWidth > 0:
            minWidth = minWidth * 1.4
        else:
            minWidth = 20.0

        self.SetEnds(0.0, 0.0, minWidth, 0.0)
        self.Initialise()

        return minWidth

    def FindLinePosition(self, x, y):
        """Find which position we're talking about at this x, y.

        Returns ARROW_POSITION_START, ARROW_POSITION_MIDDLE, ARROW_POSITION_END.
        """
        startX, startY, endX, endY = self.GetEnds()

        # Find distances from centre, start and end. The smallest wins
        centreDistance = math.sqrt((x - self._xpos) * (x - self._xpos) + (y - self._ypos) * (y - self._ypos))
        startDistance = math.sqrt((x - startX) * (x - startX) + (y - startY) * (y - startY))
        endDistance = math.sqrt((x - endX) * (x - endX) + (y - endY) * (y - endY))

        if centreDistance < startDistance and centreDistance < endDistance:
            return ARROW_POSITION_MIDDLE
        elif startDistance < endDistance:
            return ARROW_POSITION_START
        else:
            return ARROW_POSITION_END

    def SetAlignmentOrientation(self, isEnd, isHoriz):
        if isEnd:
            if isHoriz and self._alignmentEnd & LINE_ALIGNMENT_HORIZ != LINE_ALIGNMENT_HORIZ:
                self._alignmentEnd != LINE_ALIGNMENT_HORIZ
            elif not isHoriz and self._alignmentEnd & LINE_ALIGNMENT_HORIZ == LINE_ALIGNMENT_HORIZ:
                self._alignmentEnd -= LINE_ALIGNMENT_HORIZ
        else:
            if isHoriz and self._alignmentStart & LINE_ALIGNMENT_HORIZ != LINE_ALIGNMENT_HORIZ:
                self._alignmentStart != LINE_ALIGNMENT_HORIZ
            elif not isHoriz and self._alignmentStart & LINE_ALIGNMENT_HORIZ == LINE_ALIGNMENT_HORIZ:
                self._alignmentStart -= LINE_ALIGNMENT_HORIZ
            
    def SetAlignmentType(self, isEnd, alignType):
        if isEnd:
            if alignType == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                if self._alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE != LINE_ALIGNMENT_TO_NEXT_HANDLE:
                    self._alignmentEnd |= LINE_ALIGNMENT_TO_NEXT_HANDLE
            elif self._alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                self._alignmentEnd -= LINE_ALIGNMENT_TO_NEXT_HANDLE
        else:
            if alignType == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                if self._alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE != LINE_ALIGNMENT_TO_NEXT_HANDLE:
                    self._alignmentStart |= LINE_ALIGNMENT_TO_NEXT_HANDLE
            elif self._alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                self._alignmentStart -= LINE_ALIGNMENT_TO_NEXT_HANDLE
            
    def GetAlignmentOrientation(self, isEnd):
        if isEnd:
            return self._alignmentEnd & LINE_ALIGNMENT_HORIZ == LINE_ALIGNMENT_HORIZ
        else:
            return self._alignmentStart & LINE_ALIGNMENT_HORIZ == LINE_ALIGNMENT_HORIZ

    def GetAlignmentType(self, isEnd):
        if isEnd:
            return self._alignmentEnd & LINE_ALIGNMENT_TO_NEXT_HANDLE
        else:
            return self._alignmentStart & LINE_ALIGNMENT_TO_NEXT_HANDLE

    def GetNextControlPoint(self, shape):
        """Find the next control point in the line after the start / end point,
        depending on whether the shape is at the start or end.
        """
        n = len(self._lineControlPoints)
        if self._to == shape:
            # Must be END of line, so we want (n - 1)th control point.
            # But indexing ends at n-1, so subtract 2.
            nn = n - 2
        else:
            nn = 1
        if nn < len(self._lineControlPoints):
            return self._lineControlPoints[nn]
        return None

    def OnCreateLabelShape(self, parent, region, w, h):
        return LabelShape(parent, region, w, h)

    
    def OnLabelMovePre(self, dc, labelShape, x, y, old_x, old_y, display):
        labelShape._shapeRegion.SetSize(labelShape.GetWidth(), labelShape.GetHeight())

        # Find position in line's region list
        i = self._regions.index(labelShape._shapeRegion)
                
        xx, yy = self.GetLabelPosition(i)
        # Set the region's offset, relative to the default position for
        # each region.
        labelShape._shapeRegion.SetPosition(x - xx, y - yy)
        labelShape.SetX(x)
        labelShape.SetY(y)

        # Need to reformat to fit region
        if labelShape._shapeRegion.GetText():
            s = labelShape._shapeRegion.GetText()
            labelShape.FormatText(dc, s, i)
            self.DrawRegion(dc, labelShape._shapeRegion, xx, yy)
        return True
    
