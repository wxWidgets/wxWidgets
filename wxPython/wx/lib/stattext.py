#----------------------------------------------------------------------
# Name:        wx.lib.stattext
# Purpose:     A generic wxGenStaticText class.  Using this should
#              eliminate some of the platform differences in wxStaticText,
#              such as background colours and mouse sensitivity.
#
# Author:      Robin Dunn
#
# Created:     8-July-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/12/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Untested.
#

import wx

#----------------------------------------------------------------------

class GenStaticText(wx.PyControl):
    labelDelta = 1

    def __init__(self, parent, ID, label,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0,
                 name = "genstattext"):
        wx.PyControl.__init__(self, parent, ID, pos, size, style|wx.NO_BORDER,
                             wx.DefaultValidator, name)

        wx.PyControl.SetLabel(self, label) # don't check wx.ST_NO_AUTORESIZE yet
        self.defBackClr = self.GetBackgroundColour()
        self.InheritAttributes()
        self.SetBestFittingSize(size)

        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT,            self.OnPaint)


    def SetLabel(self, label):
        """
        Sets the static text label and updates the control's size to exactly
        fit the label unless the control has wx.ST_NO_AUTORESIZE flag.
        """
        wx.PyControl.SetLabel(self, label)
        style = self.GetWindowStyleFlag()
        if not style & wx.ST_NO_AUTORESIZE:
            best = self.GetBestSize()
            self.SetSize(best)
            self.SetMinSize(best)
        self.Refresh()


    def SetFont(self, font):
        """
        Sets the static text font and updates the control's size to exactly
        fit the label unless the control has wx.ST_NO_AUTORESIZE flag.
        """
        wx.PyControl.SetFont(self, font)
        style = self.GetWindowStyleFlag()
        if not style & wx.ST_NO_AUTORESIZE:
            best = self.GetBestSize()
            self.SetSize(best)
            self.SetMinSize(best)
        self.Refresh()


    def DoGetBestSize(self):
        """
        Overridden base class virtual.  Determines the best size of
        the button based on the label size.
        """
        label = self.GetLabel()
        maxWidth = totalHeight = 0
        for line in label.split('\n'):
            if line == '':
                w, h = self.GetTextExtent('W')  # empty lines have height too
            else:
                w, h = self.GetTextExtent(line)
            totalHeight += h
            maxWidth = max(maxWidth, w)
        return wx.Size(maxWidth, totalHeight)


    def AcceptsFocus(self):
        """Overridden base class virtual."""
        return False


    def GetDefaultAttributes(self):
        """
        Overridden base class virtual.  By default we should use
        the same font/colour attributes as the native StaticText.
        """
        return wx.StaticText.GetClassDefaultAttributes()


    def ShouldInheritColours(self):
        """
        Overridden base class virtual.  If the parent has non-default
        colours then we want this control to inherit them.
        """
        return True
    

    def OnPaint(self, event):
        dc = wx.BufferedPaintDC(self)
        #dc = wx.PaintDC(self)
        width, height = self.GetClientSize()
        if not width or not height:
            return

        clr = self.GetBackgroundColour()
        backBrush = wx.Brush(clr, wx.SOLID)
        if wx.Platform == "__WXMAC__" and clr == self.defBackClr:
            # if colour is still the default then use the striped background on Mac
            backBrush.MacSetTheme(1) # 1 == kThemeBrushDialogBackgroundActive
        dc.SetBackground(backBrush)

        dc.SetTextForeground(self.GetForegroundColour())
        dc.Clear()
        dc.SetFont(self.GetFont())
        label = self.GetLabel()
        style = self.GetWindowStyleFlag()
        x = y = 0
        for line in label.split('\n'):
            if line == '':
                w, h = self.GetTextExtent('W')  # empty lines have height too
            else:
                w, h = self.GetTextExtent(line)
            if style & wx.ALIGN_RIGHT:
                x = width - w
            if style & wx.ALIGN_CENTER:
                x = (width - w)/2
            dc.DrawText(line, x, y)
            y += h


    def OnEraseBackground(self, event):
        pass




#----------------------------------------------------------------------


