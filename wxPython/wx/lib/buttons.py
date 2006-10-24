#----------------------------------------------------------------------
# Name:        wx.lib.buttons
# Purpose:     Various kinds of generic buttons, (not native controls but
#              self-drawn.)
#
# Author:      Robin Dunn
#
# Created:     9-Dec-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Tested with updated demo
# 

"""
This module implements various forms of generic buttons, meaning that
they are not built on native controls but are self-drawn.  They act
like normal buttons but you are able to better control how they look,
bevel width, colours, etc.
"""

import wx
import imageutils


#----------------------------------------------------------------------

class GenButtonEvent(wx.PyCommandEvent):
    """Event sent from the generic buttons when the button is activated. """
    def __init__(self, eventType, id):
        wx.PyCommandEvent.__init__(self, eventType, id)
        self.isDown = False
        self.theButton = None

    def SetIsDown(self, isDown):
        self.isDown = isDown

    def GetIsDown(self):
        return self.isDown

    def SetButtonObj(self, btn):
        self.theButton = btn

    def GetButtonObj(self):
        return self.theButton


#----------------------------------------------------------------------

class GenButton(wx.PyControl):
    """A generic button, and base class for the other generic buttons."""

    labelDelta = 1

    def __init__(self, parent, id=-1, label='',
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        cstyle = style
        if cstyle == 0:
            cstyle = wx.BORDER_NONE
        wx.PyControl.__init__(self, parent, id, pos, size, cstyle, validator, name)

        self.up = True
        self.hasFocus = False
        self.style = style
        if style & wx.BORDER_NONE:
            self.bezelWidth = 0
            self.useFocusInd = False
        else:
            self.bezelWidth = 2
            self.useFocusInd = True

        self.SetLabel(label)
        self.InheritAttributes()
        self.SetBestFittingSize(size)
        self.InitColours()

        self.Bind(wx.EVT_LEFT_DOWN,        self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP,          self.OnLeftUp)
        if wx.Platform == '__WXMSW__':
            self.Bind(wx.EVT_LEFT_DCLICK,  self.OnLeftDown)
        self.Bind(wx.EVT_MOTION,           self.OnMotion)
        self.Bind(wx.EVT_SET_FOCUS,        self.OnGainFocus)
        self.Bind(wx.EVT_KILL_FOCUS,       self.OnLoseFocus)
        self.Bind(wx.EVT_KEY_DOWN,         self.OnKeyDown)
        self.Bind(wx.EVT_KEY_UP,           self.OnKeyUp)
        self.Bind(wx.EVT_PAINT,            self.OnPaint)


    def SetBestSize(self, size=None):
        """
        Given the current font and bezel width settings, calculate
        and set a good size.
        """
        if size is None:
            size = wx.DefaultSize            
        wx.PyControl.SetBestFittingSize(self, size)


    def DoGetBestSize(self):
        """
        Overridden base class virtual.  Determines the best size of the
        button based on the label and bezel size.
        """
        w, h, useMin = self._GetLabelSize()
        defSize = wx.Button.GetDefaultSize()
        width = 12 + w
        if useMin and width < defSize.width:
           width = defSize.width
        height = 11 + h
        if useMin and height < defSize.height:
            height = defSize.height
        width = width + self.bezelWidth - 1
        height = height + self.bezelWidth - 1
        return (width, height)


    def AcceptsFocus(self):
        """Overridden base class virtual."""
        return self.IsShown() and self.IsEnabled()


    def GetDefaultAttributes(self):
        """
        Overridden base class virtual.  By default we should use
        the same font/colour attributes as the native Button.
        """
        return wx.Button.GetClassDefaultAttributes()


    def ShouldInheritColours(self):
        """
        Overridden base class virtual.  Buttons usually don't inherit
        the parent's colours.
        """
        return False
    

    def Enable(self, enable=True):
        wx.PyControl.Enable(self, enable)
        self.Refresh()


    def SetBezelWidth(self, width):
        """Set the width of the 3D effect"""
        self.bezelWidth = width

    def GetBezelWidth(self):
        """Return the width of the 3D effect"""
        return self.bezelWidth

    def SetUseFocusIndicator(self, flag):
        """Specifiy if a focus indicator (dotted line) should be used"""
        self.useFocusInd = flag

    def GetUseFocusIndicator(self):
        """Return focus indicator flag"""
        return self.useFocusInd


    def InitColours(self):
        """
        Calculate a new set of highlight and shadow colours based on
        the background colour.  Works okay if the colour is dark...
        """
        faceClr = self.GetBackgroundColour()
        r, g, b = faceClr.Get()
        fr, fg, fb = min(255,r+32), min(255,g+32), min(255,b+32)
        self.faceDnClr = wx.Colour(fr, fg, fb)
        sr, sg, sb = max(0,r-32), max(0,g-32), max(0,b-32)
        self.shadowPen = wx.Pen(wx.Colour(sr,sg,sb), 1, wx.SOLID)
        hr, hg, hb = min(255,r+64), min(255,g+64), min(255,b+64)
        self.highlightPen = wx.Pen(wx.Colour(hr,hg,hb), 1, wx.SOLID)
        self.focusClr = wx.Colour(hr, hg, hb)

        textClr = self.GetForegroundColour()
        if wx.Platform == "__WXMAC__":
            self.focusIndPen = wx.Pen(textClr, 1, wx.SOLID)
        else:
            self.focusIndPen  = wx.Pen(textClr, 1, wx.USER_DASH)
            self.focusIndPen.SetDashes([1,1])
            self.focusIndPen.SetCap(wx.CAP_BUTT)
        
        
    def SetBackgroundColour(self, colour):
        wx.PyControl.SetBackgroundColour(self, colour)
        self.InitColours()


    def SetForegroundColour(self, colour):
        wx.PyControl.SetForegroundColour(self, colour)
        self.InitColours()

    def SetDefault(self):
        self.GetParent().SetDefaultItem(self)
        
    def _GetLabelSize(self):
        """ used internally """
        w, h = self.GetTextExtent(self.GetLabel())
        return w, h, True


    def Notify(self):
        evt = GenButtonEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, self.GetId())
        evt.SetIsDown(not self.up)
        evt.SetButtonObj(self)
        evt.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(evt)


    def DrawBezel(self, dc, x1, y1, x2, y2):
        # draw the upper left sides
        if self.up:
            dc.SetPen(self.highlightPen)
        else:
            dc.SetPen(self.shadowPen)
        for i in range(self.bezelWidth):
            dc.DrawLine(x1+i, y1, x1+i, y2-i)
            dc.DrawLine(x1, y1+i, x2-i, y1+i)

        # draw the lower right sides
        if self.up:
            dc.SetPen(self.shadowPen)
        else:
            dc.SetPen(self.highlightPen)
        for i in range(self.bezelWidth):
            dc.DrawLine(x1+i, y2-i, x2+1, y2-i)
            dc.DrawLine(x2-i, y1+i, x2-i, y2)


    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        dc.SetFont(self.GetFont())
        if self.IsEnabled():
            dc.SetTextForeground(self.GetForegroundColour())
        else:
            dc.SetTextForeground(wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT))
        label = self.GetLabel()
        tw, th = dc.GetTextExtent(label)
        if not self.up:
            dw = dy = self.labelDelta
        dc.DrawText(label, (width-tw)/2+dw, (height-th)/2+dy)


    def DrawFocusIndicator(self, dc, w, h):
        bw = self.bezelWidth
        self.focusIndPen.SetColour(self.focusClr)
        dc.SetLogicalFunction(wx.INVERT)
        dc.SetPen(self.focusIndPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle(bw+2,bw+2,  w-bw*2-4, h-bw*2-4)
        dc.SetLogicalFunction(wx.COPY)

    def OnPaint(self, event):
        (width, height) = self.GetClientSizeTuple()
        x1 = y1 = 0
        x2 = width-1
        y2 = height-1

        dc = wx.PaintDC(self)
        brush = self.GetBackgroundBrush(dc)
        if brush is not None:
            dc.SetBackground(brush)
            dc.Clear()

        self.DrawBezel(dc, x1, y1, x2, y2)
        self.DrawLabel(dc, width, height)
        if self.hasFocus and self.useFocusInd:
            self.DrawFocusIndicator(dc, width, height)


    def GetBackgroundBrush(self, dc):
        if self.up:
            colBg = self.GetBackgroundColour()
            brush = wx.Brush(colBg, wx.SOLID)
            if self.style & wx.BORDER_NONE:
                myAttr = self.GetDefaultAttributes()
                parAttr = self.GetParent().GetDefaultAttributes()
                myDef = colBg == myAttr.colBg
                parDef = self.GetParent().GetBackgroundColour() == parAttr.colBg
                if myDef and parDef:
                    if wx.Platform == "__WXMAC__":
                        brush.MacSetTheme(1) # 1 == kThemeBrushDialogBackgroundActive
                    elif wx.Platform == "__WXMSW__":
                        if self.DoEraseBackground(dc):
                            brush = None
                elif myDef and not parDef:
                    colBg = self.GetParent().GetBackgroundColour()
                    brush = wx.Brush(colBg, wx.SOLID)
        else:
            # this line assumes that a pressed button should be hilighted with
            # a solid colour even if the background is supposed to be transparent
            brush = wx.Brush(self.faceDnClr, wx.SOLID)
        return brush


    def OnLeftDown(self, event):
        if not self.IsEnabled():
            return
        self.up = False
        self.CaptureMouse()
        self.SetFocus()
        self.Refresh()
        event.Skip()


    def OnLeftUp(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if self.HasCapture():
            self.ReleaseMouse()
            if not self.up:    # if the button was down when the mouse was released...
                self.Notify()
            self.up = True
            if self:           # in case the button was destroyed in the eventhandler
                self.Refresh()
                event.Skip()


    def OnMotion(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if event.LeftIsDown() and self.HasCapture():
            x,y = event.GetPositionTuple()
            w,h = self.GetClientSizeTuple()
            if self.up and x<w and x>=0 and y<h and y>=0:
                self.up = False
                self.Refresh()
                return
            if not self.up and (x<0 or y<0 or x>=w or y>=h):
                self.up = True
                self.Refresh()
                return
        event.Skip()


    def OnGainFocus(self, event):
        self.hasFocus = True
        self.Refresh()
        self.Update()


    def OnLoseFocus(self, event):
        self.hasFocus = False
        self.Refresh()
        self.Update()


    def OnKeyDown(self, event):
        if self.hasFocus and event.GetKeyCode() == ord(" "):
            self.up = False
            self.Refresh()
        event.Skip()


    def OnKeyUp(self, event):
        if self.hasFocus and event.GetKeyCode() == ord(" "):
            self.up = True
            self.Notify()
            self.Refresh()
        event.Skip()


#----------------------------------------------------------------------

class GenBitmapButton(GenButton):
    """A generic bitmap button."""

    def __init__(self, parent, id=-1, bitmap=wx.NullBitmap,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        self.bmpDisabled = None
        self.bmpFocus = None
        self.bmpSelected = None
        self.SetBitmapLabel(bitmap)
        GenButton.__init__(self, parent, id, "", pos, size, style, validator, name)


    def GetBitmapLabel(self):
        return self.bmpLabel
    def GetBitmapDisabled(self):
        return self.bmpDisabled
    def GetBitmapFocus(self):
        return self.bmpFocus
    def GetBitmapSelected(self):
        return self.bmpSelected


    def SetBitmapDisabled(self, bitmap):
        """Set bitmap to display when the button is disabled"""
        self.bmpDisabled = bitmap

    def SetBitmapFocus(self, bitmap):
        """Set bitmap to display when the button has the focus"""
        self.bmpFocus = bitmap
        self.SetUseFocusIndicator(False)

    def SetBitmapSelected(self, bitmap):
        """Set bitmap to display when the button is selected (pressed down)"""
        self.bmpSelected = bitmap

    def SetBitmapLabel(self, bitmap, createOthers=True):
        """
        Set the bitmap to display normally.
        This is the only one that is required. If
        createOthers is True, then the other bitmaps
        will be generated on the fly.  Currently,
        only the disabled bitmap is generated.
        """
        self.bmpLabel = bitmap
        if bitmap is not None and createOthers:
            image = wx.ImageFromBitmap(bitmap)
            imageutils.grayOut(image)
            self.SetBitmapDisabled(wx.BitmapFromImage(image))


    def _GetLabelSize(self):
        """ used internally """
        if not self.bmpLabel:
            return -1, -1, False
        return self.bmpLabel.GetWidth()+2, self.bmpLabel.GetHeight()+2, False

    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        bmp = self.bmpLabel
        if self.bmpDisabled and not self.IsEnabled():
            bmp = self.bmpDisabled
        if self.bmpFocus and self.hasFocus:
            bmp = self.bmpFocus
        if self.bmpSelected and not self.up:
            bmp = self.bmpSelected
        bw,bh = bmp.GetWidth(), bmp.GetHeight()
        if not self.up:
            dw = dy = self.labelDelta
        hasMask = bmp.GetMask() != None
        dc.DrawBitmap(bmp, (width-bw)/2+dw, (height-bh)/2+dy, hasMask)


#----------------------------------------------------------------------


class GenBitmapTextButton(GenBitmapButton):
    """A generic bitmapped button with text label"""
    def __init__(self, parent, id=-1, bitmap=wx.NullBitmap, label='',
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        GenBitmapButton.__init__(self, parent, id, bitmap, pos, size, style, validator, name)
        self.SetLabel(label)


    def _GetLabelSize(self):
        """ used internally """
        w, h = self.GetTextExtent(self.GetLabel())
        if not self.bmpLabel:
            return w, h, True       # if there isn't a bitmap use the size of the text

        w_bmp = self.bmpLabel.GetWidth()+2
        h_bmp = self.bmpLabel.GetHeight()+2
        width = w + w_bmp
        if h_bmp > h:
            height = h_bmp
        else:
            height = h
        return width, height, True


    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        bmp = self.bmpLabel
        if bmp != None:     # if the bitmap is used
            if self.bmpDisabled and not self.IsEnabled():
                bmp = self.bmpDisabled
            if self.bmpFocus and self.hasFocus:
                bmp = self.bmpFocus
            if self.bmpSelected and not self.up:
                bmp = self.bmpSelected
            bw,bh = bmp.GetWidth(), bmp.GetHeight()
            if not self.up:
                dw = dy = self.labelDelta
            hasMask = bmp.GetMask() != None
        else:
            bw = bh = 0     # no bitmap -> size is zero

        dc.SetFont(self.GetFont())
        if self.IsEnabled():
            dc.SetTextForeground(self.GetForegroundColour())
        else:
            dc.SetTextForeground(wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT))

        label = self.GetLabel()
        tw, th = dc.GetTextExtent(label)        # size of text
        if not self.up:
            dw = dy = self.labelDelta

        pos_x = (width-bw-tw)/2+dw      # adjust for bitmap and text to centre
        if bmp !=None:
            dc.DrawBitmap(bmp, pos_x, (height-bh)/2+dy, hasMask) # draw bitmap if available
            pos_x = pos_x + 2   # extra spacing from bitmap

        dc.DrawText(label, pos_x + dw+bw, (height-th)/2+dy)      # draw the text


#----------------------------------------------------------------------


class __ToggleMixin:
    def SetToggle(self, flag):
        self.up = not flag
        self.Refresh()
    SetValue = SetToggle

    def GetToggle(self):
        return not self.up
    GetValue = GetToggle

    def OnLeftDown(self, event):
        if not self.IsEnabled():
            return
        self.saveUp = self.up
        self.up = not self.up
        self.CaptureMouse()
        self.SetFocus()
        self.Refresh()

    def OnLeftUp(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if self.HasCapture():
            self.ReleaseMouse()
            self.Refresh()
            if self.up != self.saveUp:
                self.Notify()

    def OnKeyDown(self, event):
        event.Skip()

    def OnMotion(self, event):
        if not self.IsEnabled():
            return
        if event.LeftIsDown() and self.HasCapture():
            x,y = event.GetPositionTuple()
            w,h = self.GetClientSizeTuple()
            if x<w and x>=0 and y<h and y>=0:
                self.up = not self.saveUp
                self.Refresh()
                return
            if (x<0 or y<0 or x>=w or y>=h):
                self.up = self.saveUp
                self.Refresh()
                return
        event.Skip()

    def OnKeyUp(self, event):
        if self.hasFocus and event.GetKeyCode() == ord(" "):
            self.up = not self.up
            self.Notify()
            self.Refresh()
        event.Skip()




class GenToggleButton(__ToggleMixin, GenButton):
    """A generic toggle button"""
    pass

class GenBitmapToggleButton(__ToggleMixin, GenBitmapButton):
    """A generic toggle bitmap button"""
    pass

class GenBitmapTextToggleButton(__ToggleMixin, GenBitmapTextButton):
    """A generic toggle bitmap button with text label"""
    pass

#----------------------------------------------------------------------


