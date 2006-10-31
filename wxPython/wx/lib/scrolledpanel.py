#----------------------------------------------------------------------------
# Name:         scrolledpanel.py
# Author:       Will Sadkin
# Created:      03/21/2003
# Copyright:    (c) 2003 by Will Sadkin
# RCS-ID:       $Id$
# License:      wxWindows license
#----------------------------------------------------------------------------
# 12/11/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxScrolledPanel -> ScrolledPanel
#

import  wx


class ScrolledPanel( wx.PyScrolledWindow ):

    """ ScrolledPanel fills a "hole" in the implementation of
    wx.ScrolledWindow, providing automatic scrollbar and scrolling
    behavior and the tab traversal management that wxScrolledWindow
    lacks.  This code was based on the original demo code showing how
    to do this, but is now available for general use as a proper class
    (and the demo is now converted to just use it.)

    It is assumed that the ScrolledPanel will have a sizer, as it is
    used to calculate the minimal virtual size of the panel and etc.
    """
    
    def __init__(self, parent, id=-1, pos = wx.DefaultPosition,
                 size = wx.DefaultSize, style = wx.TAB_TRAVERSAL,
                 name = "scrolledpanel"):

        wx.PyScrolledWindow.__init__(self, parent, id,
                                     pos=pos, size=size,
                                     style=style, name=name)
        self.SetInitialSize(size)
        self.Bind(wx.EVT_CHILD_FOCUS, self.OnChildFocus)


    def SetupScrolling(self, scroll_x=True, scroll_y=True, rate_x=20, rate_y=20):
        """
        This function sets up the event handling necessary to handle
        scrolling properly. It should be called within the __init__
        function of any class that is derived from ScrolledPanel,
        once the controls on the panel have been constructed and
        thus the size of the scrolling area can be determined.

        """
        # The following is all that is needed to integrate the sizer and the
        # scrolled window.
        if not scroll_x: rate_x = 0
        if not scroll_y: rate_y = 0

        # Round up the virtual size to be a multiple of the scroll rate
        sizer = self.GetSizer()
        if sizer:
            w, h = sizer.GetMinSize()
            if rate_x:
                w += rate_x - (w % rate_x)
            if rate_y:
                h += rate_y - (h % rate_y)
            self.SetVirtualSize( (w, h) )
        self.SetScrollRate(rate_x, rate_y)        
        wx.CallAfter(self._SetupAfter) # scroll back to top after initial events


    def _SetupAfter(self):
        self.SetVirtualSize(self.GetBestVirtualSize())
        self.Scroll(0,0)


    def OnChildFocus(self, evt):
        # If the child window that gets the focus is not visible,
        # this handler will try to scroll enough to see it.
        evt.Skip()
        child = evt.GetWindow()

        sppu_x, sppu_y = self.GetScrollPixelsPerUnit()
        vs_x, vs_y   = self.GetViewStart()
        cr = child.GetRect()
        clntsz = self.GetClientSize()
        new_vs_x, new_vs_y = -1, -1

        # is it before the left edge?
        if cr.x < 0 and sppu_x > 0:
            new_vs_x = vs_x + (cr.x / sppu_x)

        # is it above the top?
        if cr.y < 0 and sppu_y > 0:
            new_vs_y = vs_y + (cr.y / sppu_y)

        # For the right and bottom edges, scroll enough to show the
        # whole control if possible, but if not just scroll such that
        # the top/left edges are still visible

        # is it past the right edge ?
        if cr.right > clntsz.width and sppu_x > 0:
            diff = (cr.right - clntsz.width) / sppu_x
            if cr.x - diff * sppu_x > 0:
                new_vs_x = vs_x + diff + 1
            else:
                new_vs_x = vs_x + (cr.x / sppu_x)
                
        # is it below the bottom ?
        if cr.bottom > clntsz.height and sppu_y > 0:
            diff = (cr.bottom - clntsz.height) / sppu_y
            if cr.y - diff * sppu_y > 0:
                new_vs_y = vs_y + diff + 1
            else:
                new_vs_y = vs_y + (cr.y / sppu_y)

        # if we need to adjust
        if new_vs_x != -1 or new_vs_y != -1:
            #print "%s: (%s, %s)" % (self.GetName(), new_vs_x, new_vs_y)
            self.Scroll(new_vs_x, new_vs_y)
