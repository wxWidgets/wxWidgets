# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------
# Name:        wxPython.lib.rightalign
# Purpose:     A class derived from wxTextCtrl that aligns the text
#              on the right side of the control, (except when editing.)
#
# Author:      Josu Oyanguren
#
# Created:     19-October-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2001 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/11/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Added deprecation warning.
#
# 12/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxRightTextCtrl -> RightTextCtrl
#

"""
Some time ago, I asked about how to right-align
wxTextCtrls. Answer was that it is not supported. I forgot it.

Just a week ago, one of my clients asked me to have numbers right
aligned. (Indeed it was that numbers MUST be right aligned).

So the game begun. Hacking, hacking, ...

At last, i succeed. Here is some code that someone may find
useful. ubRightTextCtrl is right-aligned when you are not editing, but
left-aligned if it has focus.

Hope this can help someone, as much as this list helps me.

Josu Oyanguren
Ubera Servicios Informaticos.


P.S.  This only works well on wxMSW.
"""

import  warnings
import  wx

#----------------------------------------------------------------------

warningmsg = r"""\

##############################################################\
# THIS MODULE IS DEPRECATED                                   |
#                                                             |
# This control still functions, but it is deprecated because  |
# wx.TextCtrl now supports the wx.TE_RIGHT style flag         |
##############################################################/


"""

warnings.warn(warningmsg, DeprecationWarning, stacklevel=2)

#----------------------------------------------------------------------

class RightTextCtrl(wx.TextCtrl):
    def __init__(self, parent, id, *args, **kwargs):
        wx.TextCtrl.__init__(self, parent, id, *args, **kwargs)
        self.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        dc.SetFont(self.GetFont())
        dc.Clear()
        text = self.GetValue()
        textwidth, textheight = dc.GetTextExtent(text)
        dcwidth, dcheight = self.GetClientSize()

        y = (dcheight - textheight) / 2
        x = dcwidth - textwidth - 2

        if self.IsEnabled():
            fclr = self.GetForegroundColour()
        else:
            fclr = wx.SystemSettings_GetColour(wx.SYS_COLOUR_GRAYTEXT)

        dc.SetTextForeground(fclr)

        dc.SetClippingRegion((0, 0), (dcwidth, dcheight))
        dc.DrawText(text, x, y)

        if x < 0:
            toofat = '...'
            markwidth = dc.GetTextExtent(toofat)[0]
            dc.SetPen(wx.Pen(dc.GetBackground().GetColour(), 1, wx.SOLID ))
            dc.DrawRectangle(0,0, markwidth, dcheight)
            dc.SetPen(wx.Pen(wx.RED, 1, wx.SOLID ))
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.DrawRectangle(1, 1, dcwidth-2, dcheight-2)
            dc.DrawText(toofat, 1, y)


    def OnKillFocus(self, event):
        if not self.GetParent(): return
        self.Refresh()
        event.Skip()

