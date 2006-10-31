#----------------------------------------------------------------------
# Name:        wx.lib.statbmp
# Purpose:     A generic StaticBitmap class.  
#
# Author:      Robin Dunn
#
# Created:     12-May-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import wx

#----------------------------------------------------------------------

class GenStaticBitmap(wx.PyControl):
    labelDelta = 1

    def __init__(self, parent, ID, bitmap,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0,
                 name = "genstatbmp"):
        if not style & wx.BORDER_MASK:
            style = style | wx.BORDER_NONE
        wx.PyControl.__init__(self, parent, ID, pos, size, style,
                             wx.DefaultValidator, name)
        self._bitmap = bitmap
        self.InheritAttributes()
        self.SetInitialSize(size)

        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT,            self.OnPaint)


    def SetBitmap(self, bitmap):
        self._bitmap = bitmap
        self.SetInitialSize( (bitmap.GetWidth(), bitmap.GetHeight()) )
        self.Refresh()


    def GetBitmap(self):
        return self._bitmap
    

    def DoGetBestSize(self):
        """
        Overridden base class virtual.  Determines the best size of the
        control based on the bitmap size.
        """
        return wx.Size(self._bitmap.GetWidth(), self._bitmap.GetHeight())


    def AcceptsFocus(self):
        """Overridden base class virtual."""
        return False


    def GetDefaultAttributes(self):
        """
        Overridden base class virtual.  By default we should use
        the same font/colour attributes as the native StaticBitmap.
        """
        return wx.StaticBitmap.GetClassDefaultAttributes()
    

    def ShouldInheritColours(self):
        """
        Overridden base class virtual.  If the parent has non-default
        colours then we want this control to inherit them.
        """
        return True
    

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        if self._bitmap:
            dc.DrawBitmap(self._bitmap, 0, 0, True)
        

    def OnEraseBackground(self, event):
        pass




#----------------------------------------------------------------------


