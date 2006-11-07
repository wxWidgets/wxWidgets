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

BUFFERED = 0   # In unbuffered mode we can let the theme shine through,
               # is there a way to do this when buffering?

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
        self.InheritAttributes()
        self.SetInitialSize(size)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        if BUFFERED:
            self.defBackClr = self.GetBackgroundColour()
            self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        else:
            self.SetBackgroundStyle(wx.BG_STYLE_SYSTEM)
            


    def SetLabel(self, label):
        """
        Sets the static text label and updates the control's size to exactly
        fit the label unless the control has wx.ST_NO_AUTORESIZE flag.
        """
        wx.PyControl.SetLabel(self, label)
        style = self.GetWindowStyleFlag()
        self.InvalidateBestSize()
        if not style & wx.ST_NO_AUTORESIZE:
            self.SetInitialSize(self.GetBestSize())
        self.Refresh()


    def SetFont(self, font):
        """
        Sets the static text font and updates the control's size to exactly
        fit the label unless the control has wx.ST_NO_AUTORESIZE flag.
        """
        wx.PyControl.SetFont(self, font)
        style = self.GetWindowStyleFlag()
        self.InvalidateBestSize()
        if not style & wx.ST_NO_AUTORESIZE:
            self.SetInitialSize(self.GetBestSize())
        self.Refresh()


    def DoGetBestSize(self):
        """
        Overridden base class virtual.  Determines the best size of
        the control based on the label size and the current font.
        """
        label = self.GetLabel()
        font = self.GetFont()
        if not font:
            font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        dc = wx.ClientDC(self)
        dc.SetFont(font)
        
        maxWidth = totalHeight = 0
        for line in label.split('\n'):
            if line == '':
                w, h = dc.GetTextExtent('W')  # empty lines have height too
            else:
                w, h = dc.GetTextExtent(line)
            totalHeight += h
            maxWidth = max(maxWidth, w)
        best = wx.Size(maxWidth, totalHeight)
        self.CacheBestSize(best)
        return best


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
        if BUFFERED:
            dc = wx.BufferedPaintDC(self)
        else:
            dc = wx.PaintDC(self)
        width, height = self.GetClientSize()
        if not width or not height:
            return

        if BUFFERED:
            clr = self.GetBackgroundColour()
            backBrush = wx.Brush(clr, wx.SOLID)
            if wx.Platform == "__WXMAC__" and clr == self.defBackClr:
                # if colour is still the default then use the striped background on Mac
                backBrush.MacSetTheme(1) # 1 == kThemeBrushDialogBackgroundActive
            dc.SetBackground(backBrush)
            dc.Clear()

        if self.IsEnabled():
            dc.SetTextForeground(self.GetForegroundColour())
        else:
            dc.SetTextForeground(wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT))
            
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


