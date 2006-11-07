#---------------------------------------------------------------------------
# Name:        expando.py
# Purpose:     A multi-line text control that expands and collapses as more
#              or less lines are needed to display its content.
#
# Author:      Robin Dunn
#
# Created:     18-Sept-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 by Total Control Software
# Licence:     wxWindows license
#
#---------------------------------------------------------------------------
"""
This module contains the `ExpandoTextCtrl` which is a multi-line
text control that will expand its height on the fly to be able to show
all the lines of the content of the control.
"""

import wx
import wx.lib.newevent


# This event class and binder object can be used to catch
# notifications that the ExpandoTextCtrl has resized itself and
# that layout adjustments may need to be made.
wxEVT_ETC_LAYOUT_NEEDED = wx.NewEventType()
EVT_ETC_LAYOUT_NEEDED = wx.PyEventBinder( wxEVT_ETC_LAYOUT_NEEDED, 1 )


#---------------------------------------------------------------------------

class ExpandoTextCtrl(wx.TextCtrl):
    """
    The ExpandoTextCtrl is a multi-line wx.TextCtrl that will
    adjust its height on the fly as needed to accomodate the number of
    lines needed to display the current content of the control.  It is
    assumed that the width of the control will be a fixed value and
    that only the height will be adjusted automatically.  If the
    control is used in a sizer then the width should be set as part of
    the initial or min size of the control.

    When the control resizes itself it will attempt to also make
    necessary adjustments in the sizer hierarchy it is a member of (if
    any) but if that is not suffiecient then the programmer can catch
    the EVT_ETC_LAYOUT_NEEDED event in the container and make any
    other layout adjustments that may be needed.
    """
    _defaultHeight = -1
    
    def __init__(self, parent, id=-1, value="",
                 pos=wx.DefaultPosition,  size=wx.DefaultSize,
                 style=0, validator=wx.DefaultValidator, name="expando"):
        # find the default height of a single line control
        self.defaultHeight = self._getDefaultHeight(parent)
        # make sure we default to that height if none was given
        w, h = size
        if h == -1:
            h = self.defaultHeight
        # always use the multi-line style
        style = style | wx.TE_MULTILINE | wx.TE_NO_VSCROLL | wx.TE_RICH2
        # init the base class
        wx.TextCtrl.__init__(self, parent, id, value, pos, (w, h),
                             style, validator, name)
        # save some basic metrics
        self.extraHeight = self.defaultHeight - self.GetCharHeight()
        self.numLines = 1
        self.maxHeight = -1
        if value:
            wx.CallAfter(self._adjustCtrl)
                        
        self.Bind(wx.EVT_TEXT, self.OnTextChanged)


    def SetMaxHeight(self, h):
        """
        Sets the max height that the control will expand to on its
        own, and adjusts it down if needed.
        """
        self.maxHeight = h
        if h != -1 and self.GetSize().height > h:
            self.SetSize((-1, h))
        
    def GetMaxHeight(self):
        """Sets the max height that the control will expand to on its own"""
        return self.maxHeight


    def SetFont(self, font):
        wx.TextCtrl.SetFont(self, font)
        self.numLines = -1
        self._adjustCtrl()

    def WriteText(self, text):
        # work around a bug of a lack of a EVT_TEXT when calling
        # WriteText on wxMac
        wx.TextCtrl.WriteText(self, text)
        self._adjustCtrl()

    def AppendText(self, text):
        # Instead of using wx.TextCtrl.AppendText append and set the
        # insertion point ourselves.  This works around a bug on wxMSW
        # where it scrolls the old text out of view, and since there
        # is no scrollbar there is no way to get back to it.
        self.SetValue(self.GetValue() + text)
        self.SetInsertionPointEnd()


    def OnTextChanged(self, evt):
        # check if any adjustments are needed on every text update
        self._adjustCtrl()
        evt.Skip()
        

    def _adjustCtrl(self):
        # if the current number of lines is different than before
        # then recalculate the size needed and readjust
        numLines = self.GetNumberOfLines()
        if numLines != self.numLines:
            self.numLines = numLines
            charHeight = self.GetCharHeight()
            height = numLines * charHeight + self.extraHeight
            if not (self.maxHeight != -1 and height > self.maxHeight):
                # The size is changing...  if the control is not in a
                # sizer then we just want to change the size and
                # that's it, the programmer will need to deal with
                # potential layout issues.  If it is being managed by
                # a sizer then we'll change the min size setting and
                # then try to do a layout.  In either case we'll also
                # send an event so the parent can handle any special
                # layout issues that it wants to deal with.
                if self.GetContainingSizer() is not None:
                    mw, mh = self.GetMinSize()
                    self.SetMinSize((mw, height))
                    if self.GetParent().GetSizer() is not None:
                        self.GetParent().Layout()
                    else:
                        self.GetContainingSizer().Layout()
                else:
                    self.SetSize((-1, height))
                # send notification that layout is needed
                evt = wx.PyCommandEvent(wxEVT_ETC_LAYOUT_NEEDED, self.GetId())
                evt.SetEventObject(self)
                evt.height = height
                evt.numLines = numLines
                self.GetEventHandler().ProcessEvent(evt)
                

    def _getDefaultHeight(self, parent):
        # checked for cached value
        if self.__class__._defaultHeight != -1:
            return self.__class__._defaultHeight
        # otherwise make a single line textctrl and find out its default height
        tc = wx.TextCtrl(parent)
        sz = tc.GetSize()
        tc.Destroy()
        self.__class__._defaultHeight = sz.height
        return sz.height


    if 'wxGTK' in wx.PlatformInfo: ## and wx.VERSION < (2,7):   it's broke again in 2.7.2...
        # the wxGTK version of GetNumberOfLines in 2.6 doesn't count
        # wrapped lines, so we need to implement our own.  This is
        # fixed in 2.7.
        def GetNumberOfLines(self):
            text = self.GetValue()
            width = self.GetSize().width
            dc = wx.ClientDC(self)
            dc.SetFont(self.GetFont())
            count = 0 
            for line in text.split('\n'):
                count += 1
                w, h = dc.GetTextExtent(line)
                if w > width:
                    # the width of the text is wider than the control,
                    # calc how many lines it will be wrapped to
                    count += self._wrapLine(line, dc, width)
                    
            if not count:
                count = 1
            return count


        def _wrapLine(self, line, dc, width):
            # Estimate where the control will wrap the lines and
            # return the count of extra lines needed.
            pte = dc.GetPartialTextExtents(line)            
            width -= wx.SystemSettings.GetMetric(wx.SYS_VSCROLL_X)
            idx = 0
            start = 0
            count = 0
            spc = -1
            while idx < len(pte):
                if line[idx] == ' ':
                    spc = idx
                if pte[idx] - start > width:
                    # we've reached the max width, add a new line
                    count += 1
                    # did we see a space? if so restart the count at that pos
                    if spc != -1:
                        idx = spc + 1
                        spc = -1
                    start = pte[idx]
                else:
                    idx += 1
            return count

#---------------------------------------------------------------------------
