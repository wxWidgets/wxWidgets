#----------------------------------------------------------------------------
# Name:         wxScrolledPanel.py
# Author:       Will Sadkin
# Created:      03/21/2003
# Copyright:    (c) 2003 by Will Sadkin
# RCS-ID:       $Id$
# License:      wxWindows license
#----------------------------------------------------------------------------
#

from wxPython.wx import *


class wxScrolledPanel( wxScrolledWindow ):
    """
wxScrolledPanel fills a "hole" in the implementation of wxScrolledWindow,
providing automatic scrollbar and scrolling behavior and the tab traversal
management that wxScrolledWindow lacks.  This code was based on the original
demo code showing how to do this, but is now available for general use
as a proper class (and the demo is now converted to just use it.)
"""
    def __init__(self, parent, id=-1,
                 pos = wxDefaultPosition, size = wxDefaultSize,
                 style = wxTAB_TRAVERSAL, name = "scrolledpanel"):

        wxScrolledWindow.__init__(self, parent, -1,
                                  pos=pos, size=size,
                                  style=style, name=name)


    def SetupScrolling(self, scroll_x=True, scroll_y=True, rate_x=1, rate_y=1):
        """
        This function sets up the event handling necessary to handle
        scrolling properly. It should be called within the __init__
        function of any class that is derived from wxScrolledPanel,
        once the controls on the panel have been constructed and
        thus the size of the scrolling area can be determined.

        """
        # The following is all that is needed to integrate the sizer and the
        # scrolled window.
        if not scroll_x: rate_x = 0
        if not scroll_y: rate_y = 0

        self.SetScrollRate(rate_x, rate_y)

        self.GetSizer().SetVirtualSizeHints(self)
        EVT_CHILD_FOCUS(self, self.OnChildFocus)
        wxCallAfter(self.Scroll, 0, 0) # scroll back to top after initial events


    def OnChildFocus(self, evt):
        # If the child window that gets the focus is not visible,
        # this handler will try to scroll enough to see it.
        evt.Skip()
        child = evt.GetWindow()

        sppu_x, sppu_y = self.GetScrollPixelsPerUnit()
        vs_x, vs_y   = self.GetViewStart()
        cpos = child.GetPosition()
        csz  = child.GetSize()
        new_vs_x, new_vs_y = -1, -1

        # is it before the left edge?
        if cpos.x < 0 and sppu_x > 0:
                new_vs_x = cpos.x / sppu_x

        # is it above the top?
        if cpos.y < 0 and sppu_y > 0:
            new_vs_y = cpos.y / sppu_y


        # is it past the right edge ?
        if cpos.x + csz.width > self.GetClientSize().width and sppu_x > 0:
            diff = (cpos.x + csz.width - self.GetClientSize().width) / sppu_x
            new_vs_x = vs_x + diff + 1

        # is it below the bottom ?
        if cpos.y + csz.height > self.GetClientSize().height and sppu_y > 0:
            diff = (cpos.y + csz.height - self.GetClientSize().height) / sppu_y
            new_vs_y = vs_y + diff + 1

        # if we need to adjust
        if new_vs_x != -1 or new_vs_y != -1:
            self.Scroll(new_vs_x, new_vs_y)
