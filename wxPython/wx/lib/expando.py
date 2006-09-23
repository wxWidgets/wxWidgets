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
        self.numLines = self.GetNumberOfLines()
        self.maxHeight = -1
        
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


    if wx.VERSION < (2,7) and 'wxGTK' in wx.PlatformInfo:
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
                if pte[idx] - start > width - _sbw:
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


if __name__ == '__main__':
    print wx.VERSION
    
    class TestFrame(wx.Frame):
        def __init__(self):
            wx.Frame.__init__(self, None, title="Testing...")
            self.pnl = p = wx.Panel(self)
            self.eom = ExpandoTextCtrl(p, pos=(25,25), size=(250,-1))
            self.Bind(EVT_ETC_LAYOUT_NEEDED, self.OnRefit, self.eom)

            # create some buttons and sizers to use in testing some
            # features and also the layout
            vBtnSizer = wx.BoxSizer(wx.VERTICAL)

            btn = wx.Button(p, -1, "Set MaxHeight")
            self.Bind(wx.EVT_BUTTON, self.OnSetMaxHeight, btn)
            vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

            btn = wx.Button(p, -1, "Set Font")
            self.Bind(wx.EVT_BUTTON, self.OnSetFont, btn)
            vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

            btn = wx.Button(p, -1, "Set Value")
            self.Bind(wx.EVT_BUTTON, self.OnSetValue, btn)
            vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

            for x in range(5):
                btn = wx.Button(p, -1, " ")
                vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

            hBtnSizer = wx.BoxSizer(wx.HORIZONTAL)
            for x in range(3):
                btn = wx.Button(p, -1, " ")
                hBtnSizer.Add(btn, 0, wx.ALL, 5)

            sizer = wx.BoxSizer(wx.HORIZONTAL)
            col1 = wx.BoxSizer(wx.VERTICAL)
            col1.Add(self.eom, 0, wx.ALL, 10)
            col1.Add(hBtnSizer)
            sizer.Add(col1)
            sizer.Add(vBtnSizer)
            p.SetSizer(sizer)

            # Put the panel in a sizer for the frame so we can use self.Fit()
            frameSizer = wx.BoxSizer()
            frameSizer.Add(p, 1, wx.EXPAND)
            self.SetSizer(frameSizer)
            
            self.Fit()


        def OnRefit(self, evt):
            # The Expando control will redo the layout of the
            # sizer it belongs to, but sometimes this may not be
            # enough, so it will send us this event so we can do any
            # other layout adjustments needed.  In this case we'll
            # just resize the frame to fit the new needs of the sizer.
            self.Fit()

        def OnSetMaxHeight(self, evt):
            mh = self.eom.GetMaxHeight()
            dlg = wx.NumberEntryDialog(self, "", "Enter new max height:",
                                       "MaxHeight", mh, -1, 1000)
            if dlg.ShowModal() == wx.ID_OK:
                self.eom.SetMaxHeight(dlg.GetValue())
            dlg.Destroy()
            
            
        def OnSetFont(self, evt):
            dlg = wx.FontDialog(self, wx.FontData())
            dlg.GetFontData().SetInitialFont(self.eom.GetFont())
            if dlg.ShowModal() == wx.ID_OK:
                self.eom.SetFont(dlg.GetFontData().GetChosenFont())
            dlg.Destroy()
            
            
        def OnSetValue(self, evt):
            self.eom.SetValue("This is a test...  Only a test.  If this had "
                              "been a real emergency you would have seen the "
                              "quick brown fox jump over the lazy dog.")
        

    app = wx.App(False)
    frm = TestFrame()
    frm.Show()
    #import wx.py
    #shell = wx.py.shell.ShellFrame(frm, size=(500,300),
    #                               locals={'wx':wx, 'frm':frm})
    #shell.Show()
    frm.Raise()
    app.MainLoop()


    
