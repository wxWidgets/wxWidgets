# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------------
# Name:         divided.py
# Purpose:      DividedShape class
#
# Author:       Pierre Hjälm (from C++ original by Julian Smart)
#
# Created:      20040508
# RCS-ID:       
# Copyright:    (c) 2004 Pierre Hjälm - 1998 Julian Smart
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from __future__ import division

import sys
import wx

from basic import ControlPoint, RectangleShape, Shape
from oglmisc import *



class DividedShapeControlPoint(ControlPoint):
    def __init__(self, the_canvas, object, region, size, the_m_xoffset, the_m_yoffset, the_type):
        ControlPoint.__init__(self, the_canvas, object, size, the_m_xoffset, the_m_yoffset, the_type)
        self.regionId = region

    # Implement resizing of divided object division
    def OnDragLeft(self, draw, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        dc.SetLogicalFunction(OGLRBLF)
        dottedPen = wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT)
        dc.SetPen(dottedPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        dividedObject = self._shape
        x1 = dividedObject.GetX()-dividedObject.GetWidth() / 2
        y1 = y
        x2 = dividedObject.GetX() + dividedObject.GetWidth() / 2
        y2 = y

        dc.DrawLine(x1, y1, x2, y2)

    def OnBeginDragLeft(self, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        dc.SetLogicalFunction(OGLRBLF)
        dottedPen = wx.Pen(wx.Colour(0, 0, 0), 1, wx.DOT)
        dc.SetPen(dottedPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        dividedObject = self._shape
        
        x1 = dividedObject.GetX()-dividedObject.GetWidth() / 2
        y1 = y
        x2 = dividedObject.GetX() + dividedObject.GetWidth() / 2
        y2 = y

        dc.DrawLine(x1, y1, x2, y2)
        self._canvas.CaptureMouse()

    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        dc = wx.ClientDC(self.GetCanvas())
        self.GetCanvas().PrepareDC(dc)

        dividedObject = self._shape
        if not dividedObject.GetRegions()[self.regionId]:
            return
        
        thisRegion = dividedObject.GetRegions()[self.regionId]
        nextRegion = None

        dc.SetLogicalFunction(wx.COPY)

        if self._canvas.HasCapture():
            self._canvas.ReleaseMouse()

        # Find the old top and bottom of this region,
        # and calculate the new proportion for this region
        # if legal.
        currentY = dividedObject.GetY()-dividedObject.GetHeight() / 2
        maxY = dividedObject.GetY() + dividedObject.GetHeight() / 2

        # Save values
        theRegionTop = 0
        nextRegionBottom = 0
        
        for i in range(len(dividedObject.GetRegions())):
            region = dividedObject.GetRegions()[i]
            proportion = region._regionProportionY
            yy = currentY + dividedObject.GetHeight() * proportion
            actualY = min(maxY, yy)

            if region == thisRegion:
                thisRegionTop = currentY
                
                if i + 1<len(dividedObject.GetRegions()):
                    nextRegion = dividedObject.GetRegions()[i + 1]
            if region == nextRegion:
                nextRegionBottom = actualY

            currentY = actualY

        if not nextRegion:
            return

        # Check that we haven't gone above this region or below
        # next region.
        if y <= thisRegionTop or y >= nextRegionBottom:
            return

        dividedObject.EraseLinks(dc)

        # Now calculate the new proportions of this region and the next region
        thisProportion = (y-thisRegionTop) / dividedObject.GetHeight()
        nextProportion = (nextRegionBottom-y) / dividedObject.GetHeight()

        thisRegion.SetProportions(0, thisProportion)
        nextRegion.SetProportions(0, nextProportion)
        self._yoffset = y-dividedObject.GetY()

        # Now reformat text
        for i, region in enumerate(dividedObject.GetRegions()):
            if region.GetText():
                s = region.GetText()
                dividedObject.FormatText(dc, s, i)

        dividedObject.SetRegionSizes()
        dividedObject.Draw(dc)
        dividedObject.GetEventHandler().OnMoveLinks(dc)
        


class DividedShape(RectangleShape):
    """A DividedShape is a rectangle with a number of vertical divisions.
    Each division may have its text formatted with independent characteristics,
    and the size of each division relative to the whole image may be specified.

    Derived from:
      RectangleShape
    """
    def __init__(self, w, h):
        RectangleShape.__init__(self, w, h)
        self.ClearRegions()

    def OnDraw(self, dc):
        RectangleShape.OnDraw(self, dc)

    def OnDrawContents(self, dc):
        if self.GetRegions():
            defaultProportion = 1 / len(self.GetRegions())
        else:
            defaultProportion = 0
        currentY = self._ypos-self._height / 2
        maxY = self._ypos + self._height / 2

        leftX = self._xpos-self._width / 2
        rightX = self._xpos + self._width / 2

        if self._pen:
            dc.SetPen(self._pen)

        dc.SetTextForeground(self._textColour)

        # For efficiency, don't do this under X - doesn't make
        # any visible difference for our purposes.
        if sys.platform[:3]=="win":
            dc.SetTextBackground(self._brush.GetColour())

        if self.GetDisableLabel():
            return

        xMargin = 2
        yMargin = 2

        dc.SetBackgroundMode(wx.TRANSPARENT)

        for region in self.GetRegions():
            dc.SetFont(region.GetFont())
            dc.SetTextForeground(region.GetActualColourObject())

            if region._regionProportionY<0:
                proportion = defaultProportion
            else:
                proportion = region._regionProportionY

            y = currentY + self._height * proportion
            actualY = min(maxY, y)

            centreX = self._xpos
            centreY = currentY + (actualY-currentY) / 2

            DrawFormattedText(dc, region._formattedText, centreX, centreY, self._width-2 * xMargin, actualY-currentY-2 * yMargin, region._formatMode)

            if y <= maxY and region != self.GetRegions()[-1]:
                regionPen = region.GetActualPen()
                if regionPen:
                    dc.SetPen(regionPen)
                    dc.DrawLine(leftX, y, rightX, y)

            currentY = actualY

    def SetSize(self, w, h, recursive = True):
        self.SetAttachmentSize(w, h)
        self._width = w
        self._height = h
        self.SetRegionSizes()

    def SetRegionSizes(self):
        """Set all region sizes according to proportions and this object
        total size.
        """
        if not self.GetRegions():
            return

        if self.GetRegions():
            defaultProportion = 1 / len(self.GetRegions())
        else:
            defaultProportion = 0
        currentY = self._ypos-self._height / 2
        maxY = self._ypos + self._height / 2
        
        for region in self.GetRegions():
            if region._regionProportionY <= 0:
                proportion = defaultProportion
            else:
                proportion = region._regionProportionY

            sizeY = proportion * self._height
            y = currentY + sizeY
            actualY = min(maxY, y)

            centreY = currentY + (actualY-currentY) / 2

            region.SetSize(self._width, sizeY)
            region.SetPosition(0, centreY-self._ypos)

            currentY = actualY

    # Attachment points correspond to regions in the divided box
    def GetAttachmentPosition(self, attachment, nth = 0, no_arcs = 1, line = None):
        totalNumberAttachments = len(self.GetRegions()) * 2 + 2
        if self.GetAttachmentMode() == ATTACHMENT_MODE_NONE or attachment >= totalNumberAttachments:
            return Shape.GetAttachmentPosition(self, attachment, nth, no_arcs)

        n = len(self.GetRegions())
        isEnd = line and line.IsEnd(self)

        left = self._xpos-self._width / 2
        right = self._xpos + self._width / 2
        top = self._ypos-self._height / 2
        bottom = self._ypos + self._height / 2

        # Zero is top, n + 1 is bottom
        if attachment == 0:
            y = top
            if self._spaceAttachments:
                if line and line.GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                    # Align line according to the next handle along
                    point = line.GetNextControlPoint(self)
                    if point.x<left:
                        x = left
                    elif point.x>right:
                        x = right
                    else:
                        x = point.x
                else:
                    x = left + (nth + 1) * self._width / (no_arcs + 1)
            else:
                x = self._xpos
        elif attachment == n + 1:
            y = bottom
            if self._spaceAttachments:
                if line and line.GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                    # Align line according to the next handle along
                    point = line.GetNextControlPoint(self)
                    if point.x<left:
                        x = left
                    elif point.x>right:
                        x = right
                    else:
                        x = point.x
                else:
                    x = left + (nth + 1) * self._width / (no_arcs + 1)
            else:
                x = self._xpos
        else: # Left or right
            isLeft = not attachment<(n + 1)
            if isLeft:
                i = totalNumberAttachments-attachment-1
            else:
                i = attachment-1

            region = self.GetRegions()[i]
            if region:
                if isLeft:
                    x = left
                else:
                    x = right

                # Calculate top and bottom of region
                top = self._ypos + region._y-region._height / 2
                bottom = self._ypos + region._y + region._height / 2

                # Assuming we can trust the absolute size and
                # position of these regions
                if self._spaceAttachments:
                    if line and line.GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE:
                        # Align line according to the next handle along
                        point = line.GetNextControlPoint(self)
                        if point.y<bottom:
                            y = bottom
                        elif point.y>top:
                            y = top
                        else:
                            y = point.y
                    else:
                        y = top + (nth + 1) * region._height / (no_arcs + 1)
                else:
                    y = self._ypos + region._y
            else:
                return False
        return x, y

    def GetNumberOfAttachments(self):
        # There are two attachments for each region (left and right),
        # plus one on the top and one on the bottom.
        n = len(self.GetRegions()) * 2 + 2

        maxN = n-1
        for point in self._attachmentPoints:
            if point._id>maxN:
                maxN = point._id

        return maxN + 1

    def AttachmentIsValid(self, attachment):
        totalNumberAttachments = len(self.GetRegions()) * 2 + 2
        if attachment >= totalNumberAttachments:
            return Shape.AttachmentIsValid(self, attachment)
        else:
            return attachment >= 0

    def MakeControlPoints(self):
        RectangleShape.MakeControlPoints(self)
        self.MakeMandatoryControlPoints()

    def MakeMandatoryControlPoints(self):
        currentY = self.GetY()-self._height / 2
        maxY = self.GetY() + self._height / 2

        for i, region in enumerate(self.GetRegions()):
            proportion = region._regionProportionY

            y = currentY + self._height * proportion
            actualY = min(maxY, y)

            if region != self.GetRegions()[-1]:
                controlPoint = DividedShapeControlPoint(self._canvas, self, i, CONTROL_POINT_SIZE, 0, actualY-self.GetY(), 0)
                self._canvas.AddShape(controlPoint)
                self._controlPoints.append(controlPoint)

            currentY = actualY

    def ResetControlPoints(self):
        # May only have the region handles, (n - 1) of them
        if len(self._controlPoints)>len(self.GetRegions())-1:
            RectangleShape.ResetControlPoints(self)

        self.ResetMandatoryControlPoints()

    def ResetMandatoryControlPoints(self):
        currentY = self.GetY()-self._height / 2
        maxY = self.GetY() + self._height / 2

        i = 0
        for controlPoint in self._controlPoints:
            if isinstance(controlPoint, DividedShapeControlPoint):
                region = self.GetRegions()[i]
                proportion = region._regionProportionY

                y = currentY + self._height * proportion
                actualY = min(maxY, y)

                controlPoint._xoffset = 0
                controlPoint._yoffset = actualY-self.GetY()

                currentY = actualY

                i += 1
                
    def EditRegions(self):
        """Edit the region colours and styles. Not implemented."""
        print "EditRegions() is unimplemented"
        
    def OnRightClick(self, x, y, keys = 0, attachment = 0):
        if keys & KEY_CTRL:
            self.EditRegions()
        else:
            RectangleShape.OnRightClick(self, x, y, keys, attachment)
