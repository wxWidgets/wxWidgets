#----------------------------------------------------------------------
# Name:        compatdc.py
# Purpose:     Make wxPython 2.4 DC classes compatible with the 2.5
#              DC classes
#
# Author:      Robin Dunn
#
# Created:     21-Apr-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import wx


def MakeDCCompatible(klass, full=False):
    """
    Manipulate the DC class passed to this funciton such that it is
    more compatible with the DC classes used in wxPython 2.5.  This
    should help with writing code that works with both versions.  If
    full is True then in addition to creating the 'XY' versions of the
    methods, a 'point/size' version (which are the new defaults in
    2.5) will also be created.
    """
    if wx.VERSION >= (2,5):
        return   # Nothing to do

    # first create XY methods from the current ones.
    klass.FloodFillXY =            klass.FloodFill
    klass.GetPixelXY =             klass.GetPixel
    klass.DrawLineXY =             klass.DrawLine
    klass.CrossHairXY =            klass.CrossHair
    klass.DrawArcXY =              klass.DrawArc
    klass.DrawEllipticArcXY =      klass.DrawEllipticArc
    klass.DrawPointXY =            klass.DrawPoint
    klass.DrawRectangleXY =        klass.DrawRectangle
    klass.DrawRoundedRectangleXY = klass.DrawRoundedRectangle
    klass.DrawCircleXY =           klass.DrawCircle
    klass.DrawEllipseXY =          klass.DrawEllipse
    klass.DrawIconXY =             klass.DrawIcon
    klass.DrawBitmapXY =           klass.DrawBitmap
    klass.DrawTextXY =             klass.DrawText
    klass.DrawRotatedTextXY =      klass.DrawRotatedText
    klass.BlitXY =                 klass.Blit

    # now, make some functions that we can use as new methods
    if full:
        def FloodFill(self, pt, col, style=wx.FLOOD_SURFACE):
            pt = wx.Point(*pt)
            return self.FloodFillXY(pt.x, pt.y, col, style)
        klass.FloodFill = FloodFill

        def GetPixel(self, pt):
            pt = wx.Point(*pt)
            return self.GetPixelXY(pt.x, pt.y)
        klass.GetPixel = GetPixel

        def DrawLine(self, pt1, pt2):
            pt1 = wx.Point(*pt1)
            pt2 = wx.Point(*pt2)
            return self.DrawLineXY(pt1.x, pt1.y, pt2.x, pt2.y)
        klass.DrawLine = DrawLine

        def CrossHair(self, pt):
            pt = wx.Point(*pt)
            return self.CrossHairXY(pt.x, pt.y)
        klass.CrossHair = CrossHair

        def DrawArc(self,  pt1, pt2, centre):
            pt1 = wx.Point(*pt1)
            pt2 = wx.Point(*pt2)
            return self.DrawArcXY(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y)
        klass.DrawArc = DrawArc

        def DrawEllipticArc(self, pt, sz, sa, ea):
            pt = wx.Point(*pt)
            sz = wx.Size(*sz)
            return self.DrawEllipticArcXY(pt.x, pt.y, sz.width, sz.height, sa, ea)
        klass.DrawEllipticArc = DrawEllipticArc

        def DrawPoint(self, pt):
            pt = wx.Point(*pt)
            return self.DrawPointXY(pt.x, pt.y)
        klass.DrawPoint = DrawPoint

        def DrawRectangle(self, pt, sz):
            pt = wx.Point(*pt)
            sz = wx.Size(*sz)
            return self.DrawRectangleXY(pt.x, pt.y, sz.width, sz.height)
        klass.DrawRectangle = DrawRectangle

        def DrawRoundedRectangle(self, pt, sz, radius):
            pt = wx.Point(*pt)
            sz = wx.Size(*sz)
            return self.DrawRoundedRectangleXY(pt.x, pt.y, sz.width, sz.height, radius)
        klass.DrawRoundedRectangle = DrawRoundedRectangle

        def DrawCircle(self, pt, radius):
            pt = wx.Point(*pt)
            return self.DrawCircleXY(pt.x, pt.y, radius)
        klass.DrawCircle = DrawCircle

        def DrawEllipse(self, pt, sz):
            pt = wx.Point(*pt)
            sz = wx.Size(*sz)
            return self.DrawEllipseXY(pt.x, pt.y, sz.width, sz.height)
        klass.DrawEllipse = DrawEllipse

        def DrawIcon(self, icon, pt):
            pt = wx.Point(*pt)
            return self.DrawIconXY(icon, pt.x, pt.y )
        klass.DrawIcon = DrawIcon

        def DrawBitmap(self, bmp, pt):
            pt = wx.Point(*pt)
            return self.DrawBitmapXY(bmp, pt.x, pt.y)
        klass.DrawBitmap = DrawBitmap

        def DrawText(self, text, pt):
            pt = wx.Point(*pt)
            return self.DrawTextXY(text, pt.x, pt.y)
        klass.DrawText = DrawText

        def DrawRotatedText(self, text, pt, angle):
            pt = wx.Point(*pt)
            return self.DrawRotatedTextXY(text, pt.x, pt.y, angle)
        klass.DrawRotatedText = DrawRotatedText

        def Blit(self, destPt, sz, source, srcPt,
                 rop=wx.COPY, useMask=False, srcPtMask=wx.DefaultPosition):
            return self.BlitXY(destPt.x, destPt.y, sz.width, sz.height,
                               source, srcPt.x, srcPt.y, rop, useMask,
                               srcPtMask.x, srcPtMask.y)
        klass.Blit = Blit


def MakeAllDCsCompatible(full=False):
    """
    Run MakeDCCompatible on all DC classes in wx.
    """
    MakeDCCompatible(wx.BufferedPaintDC, full)
    MakeDCCompatible(wx.BufferedDC, full)
    MakeDCCompatible(wx.MemoryDC, full)
    MakeDCCompatible(wx.ScreenDC, full)
    MakeDCCompatible(wx.ClientDC, full)
    MakeDCCompatible(wx.PaintDC, full)
    MakeDCCompatible(wx.WindowDC, full)
    MakeDCCompatible(wx.PostScriptDC, full)
    if hasattr(wx, "MetaFileDC"):
        MakeDCCompatible(wx.MetaFileDC, full)
    if hasattr(wx, "PrinterDC"):
        MakeDCCompatible(wx.PrinterDC, full)
    MakeDCCompatible(wx.DC, full)
    
