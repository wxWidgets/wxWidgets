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
Ubera Servicios Informáticos.
"""

from wxPython.wx import *

#----------------------------------------------------------------------

class wxRightTextCtrl(wxTextCtrl):
    def __init__(self, parent, id, *args, **kwargs):
        wxTextCtrl.__init__(self, parent, id, *args, **kwargs)
        EVT_KILL_FOCUS(self, self.OnKillFocus)
        EVT_PAINT(self, self.OnPaint)

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        dc.SetFont(self.GetFont())
        dc.Clear()
        text = self.GetValue()
        textwidth, textheight = dc.GetTextExtent(text)
        dcwidth, dcheight = self.GetClientSizeTuple()

        y = (dcheight - textheight) / 2
        x = dcwidth - textwidth - 2

        dc.SetClippingRegion(0, 0, dcwidth, dcheight)
        dc.DrawText(text, x, y)

        if x < 0:
            toofat = '...'
            markwidth = dc.GetTextExtent(toofat)[0]
            dc.SetPen(wxPen(dc.GetBackground().GetColour(), 1, wxSOLID ))
            dc.DrawRectangle(0,0, markwidth, dcheight)
            dc.SetPen(wxPen(wxRED, 1, wxSOLID ))
            dc.SetBrush(wxTRANSPARENT_BRUSH)
            dc.DrawRectangle(1, 1, dcwidth-2, dcheight-2)
            dc.DrawText(toofat, 1, y)


    def OnKillFocus(self, event):
        if not self.GetParent(): return
        self.Refresh()
        event.Skip()

