#----------------------------------------------------------------------
# Name:        wxPython.lib.buttons
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

"""
This module implements various forms of generic buttons, meaning that
they are not built on native controls but are self-drawn.

The wxGenButton is the base.  It acts like a normal button but you
are able to better control how it looks, bevel width, colours, etc.

wxGenBitmapButton is a button with one or more bitmaps that show
the various states the button can be in.

wxGenToggleButton stays depressed when clicked, until clicked again.

wxGenBitmapToggleButton the same but with bitmaps.

"""

from wxPython.wx import *
import imageutils


#----------------------------------------------------------------------

class wxGenButtonEvent(wxPyCommandEvent):
    def __init__(self, eventType, ID):
        wxPyCommandEvent.__init__(self, eventType, ID)
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

class wxGenButton(wxPyControl):
    labelDelta = 1

    def __init__(self, parent, ID, label,
                 pos = wxDefaultPosition, size = wxDefaultSize,
                 style = 0, validator = wxDefaultValidator,
                 name = "genbutton"):
        if style == 0:
            style = wxNO_BORDER
        wxPyControl.__init__(self, parent, ID, pos, size, style, validator, name)

        self.up = True
        self.bezelWidth = 2
        self.hasFocus = False
        self.useFocusInd = True

        self.SetLabel(label)
        self.SetPosition(pos)
        font = parent.GetFont()
        if not font.Ok():
            font = wxSystemSettings_GetSystemFont(wxSYS_DEFAULT_GUI_FONT)
        self.SetFont(font)
        self.SetBestSize(size)
        self.InitColours()

        EVT_LEFT_DOWN(self,        self.OnLeftDown)
        EVT_LEFT_UP(self,          self.OnLeftUp)
        if wxPlatform == '__WXMSW__':
            EVT_LEFT_DCLICK(self,  self.OnLeftDown)
        EVT_MOTION(self,           self.OnMotion)
        EVT_SET_FOCUS(self,        self.OnGainFocus)
        EVT_KILL_FOCUS(self,       self.OnLoseFocus)
        EVT_KEY_DOWN(self,         self.OnKeyDown)
        EVT_KEY_UP(self,           self.OnKeyUp)
        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        EVT_PAINT(self,            self.OnPaint)


    def SetBestSize(self, size=None):
        """
        Given the current font and bezel width settings, calculate
        and set a good size.
        """
        if size is None:
            size = wxSize(-1,-1)
        if type(size) == type(()):
            size = wxSize(size[0], size[1])
        size = wxSize(size.width, size.height)  # make a copy

        best = self.GetBestSize()
        if size.width == -1:
            size.width = best.width
        if size.height == -1:
            size.height = best.height

        self.SetSize(size)


    def DoGetBestSize(self):
        """Overridden base class virtual.  Determines the best size of the
        button based on the label and bezel size."""
        w, h, useMin = self._GetLabelSize()
        defSize = wxButton_GetDefaultSize()
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


    def Enable(self, enable=True):
        wxPyControl.Enable(self, enable)
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
        faceClr      = wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNFACE)
        textClr      = wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNTEXT)
        self.faceDnClr = faceClr
        self.SetBackgroundColour(faceClr)
        self.SetForegroundColour(textClr)

        shadowClr    = wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNSHADOW)
        highlightClr = wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNHIGHLIGHT)
        self.shadowPen    = wxPen(shadowClr, 1, wxSOLID)
        self.highlightPen = wxPen(highlightClr, 1, wxSOLID)
        if wxPlatform == "__WXMAC__":
            self.focusIndPen = wxPen(textClr, 1, wxSOLID)
        else:
            self.focusIndPen  = wxPen(textClr, 1, wxUSER_DASH)
            self.focusIndPen.SetDashes([1,1])
            self.focusIndPen.SetCap(wxCAP_BUTT)
        self.focusClr = highlightClr


    def SetBackgroundColour(self, colour):
        wxPyControl.SetBackgroundColour(self, colour)
        colour = self.GetBackgroundColour()

        # Calculate a new set of highlight and shadow colours based on
        # the new background colour.  Works okay if the colour is dark...
        r, g, b = colour.Get()
        fr, fg, fb = min(255,r+32), min(255,g+32), min(255,b+32)
        self.faceDnClr = wxColour(fr, fg, fb)
        sr, sg, sb = max(0,r-32), max(0,g-32), max(0,b-32)
        self.shadowPen = wxPen(wxColour(sr,sg,sb), 1, wxSOLID)
        hr, hg, hb = min(255,r+64), min(255,g+64), min(255,b+64)
        self.highlightPen = wxPen(wxColour(hr,hg,hb), 1, wxSOLID)
        self.focusClr = wxColour(hr, hg, hb)


    def _GetLabelSize(self):
        """ used internally """
        w, h = self.GetTextExtent(self.GetLabel())
        return w, h, True


    def Notify(self):
        evt = wxGenButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED, self.GetId())
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
            dc.SetTextForeground(wxSystemSettings_GetSystemColour(wxSYS_COLOUR_GRAYTEXT))
        label = self.GetLabel()
        tw, th = dc.GetTextExtent(label)
        if not self.up:
            dw = dy = self.labelDelta
        dc.DrawText(label, (width-tw)/2+dw, (height-th)/2+dy)


    def DrawFocusIndicator(self, dc, w, h):
        bw = self.bezelWidth
##         if self.hasFocus:
##             self.focusIndPen.SetColour(self.GetForegroundColour())
##         else:
##             #self.focusIndPen.SetColour(self.GetBackgroundColour())
##             self.focusIndPen.SetColour(self.GetForegroundColour())
        self.focusIndPen.SetColour(self.focusClr)
        dc.SetLogicalFunction(wxINVERT)
        dc.SetPen(self.focusIndPen)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        dc.DrawRectangle(bw+2,bw+2, w-bw*2-4, h-bw*2-4)
        dc.SetLogicalFunction(wxCOPY)


    def OnPaint(self, event):
        (width, height) = self.GetClientSizeTuple()
        x1 = y1 = 0
        x2 = width-1
        y2 = height-1
        dc = wxBufferedPaintDC(self)
        if self.up:
            dc.SetBackground(wxBrush(self.GetBackgroundColour(), wxSOLID))
        else:
            dc.SetBackground(wxBrush(self.faceDnClr, wxSOLID))
        dc.Clear()
        self.DrawBezel(dc, x1, y1, x2, y2)
        self.DrawLabel(dc, width, height)
        if self.hasFocus and self.useFocusInd:
            self.DrawFocusIndicator(dc, width, height)


    def OnEraseBackground(self, event):
        pass


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
        dc = wxClientDC(self)
        w, h = self.GetClientSizeTuple()
        if self.useFocusInd:
            self.DrawFocusIndicator(dc, w, h)


    def OnLoseFocus(self, event):
        self.hasFocus = False
        dc = wxClientDC(self)
        w, h = self.GetClientSizeTuple()
        if self.useFocusInd:
            self.DrawFocusIndicator(dc, w, h)


    def OnKeyDown(self, event):
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = False
            self.Refresh()
        event.Skip()


    def OnKeyUp(self, event):
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = True
            self.Notify()
            self.Refresh()
        event.Skip()


#----------------------------------------------------------------------

class wxGenBitmapButton(wxGenButton):
    def __init__(self, parent, ID, bitmap,
                 pos = wxDefaultPosition, size = wxDefaultSize,
                 style = 0, validator = wxDefaultValidator,
                 name = "genbutton"):
        self.bmpDisabled = None
        self.bmpFocus = None
        self.bmpSelected = None
        self.SetBitmapLabel(bitmap)
        wxGenButton.__init__(self, parent, ID, "", pos, size, style, validator, name)


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
            image = wxImageFromBitmap(bitmap)
            imageutils.grayOut(image)
            self.SetBitmapDisabled(wxBitmapFromImage(image))


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


class wxGenBitmapTextButton(wxGenBitmapButton):     # generic bitmapped button with Text Label
    def __init__(self, parent, ID, bitmap, label,
                 pos = wxDefaultPosition, size = wxDefaultSize,
                 style = 0, validator = wxDefaultValidator,
                 name = "genbutton"):
        wxGenBitmapButton.__init__(self, parent, ID, bitmap, pos, size, style, validator, name)
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
            dc.SetTextForeground(wxSystemSettings_GetSystemColour(wxSYS_COLOUR_GRAYTEXT))

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
            if self.up != self.saveUp:
                self.Notify()
            self.ReleaseMouse()
            self.Refresh()

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
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = not self.up
            self.Notify()
            self.Refresh()
        event.Skip()




class wxGenToggleButton(__ToggleMixin, wxGenButton):
    pass

class wxGenBitmapToggleButton(__ToggleMixin, wxGenBitmapButton):
    pass

class wxGenBitmapTextToggleButton(__ToggleMixin, wxGenBitmapTextButton):
    pass

#----------------------------------------------------------------------


