#----------------------------------------------------------------------
# Name:        wxPython.lib.sizers.shape
# Purpose:     A Sizer that preserves the shape (proportions)
#              of the managed window
#
# Created:     7-October-1999
# RCS-ID:      $Id$
# Copyright:   SIA "ANK"
# Licence:     wxWindows license
#----------------------------------------------------------------------

from sizer       import wxSizer

wxANCHOR_NONE  = 0
wxANCHOR_NORTH = 1
wxANCHOR_SOUTH = 2
wxANCHOR_EAST  = 4
wxANCHOR_WEST  = 8
wxANCHOR_NW = wxANCHOR_NORTH | wxANCHOR_WEST
wxANCHOR_NE = wxANCHOR_NORTH | wxANCHOR_EAST
wxANCHOR_SW = wxANCHOR_SOUTH | wxANCHOR_WEST
wxANCHOR_SE = wxANCHOR_SOUTH | wxANCHOR_EAST

#----------------------------------------------------------------------

class wxShapeSizer(wxSizer):
    """
    wxShapeSizer

    This sizer preserves the proportional dimensions of the managed
    window, leaving empty space either in horizontal or vertical
    dimension.

    By default, the managed window is centered within allowed size.
    You may specify an anchor parameter to leave all of the extra
    space on one side: wxANCHOR_NORTH and wxANCHOR_SOUTH manage
    vertical dimension, leaving extra space on the bottom or top side,
    respectively; wxANCHOR_EAST and wxANCHOR_WEST do the same in
    horizontal dimension.  wxANCHOR_NW, wxANCHOR_NE, wxANCHOR_SW
    and wxANCHOR_SE are short-cut names for combinations north+west,
    north+east, south+west, south+east.

    If both anchors are specified in either direction, south and east
    take precedence over north and west, respectively.  (Because of
    gravity, widgets tend to fall down.)
    """
    def __init__(self, anchor =wxANCHOR_NONE):
        wxSizer.__init__(self)
        self.anchor =anchor

    def Add(self, widget):
        if self.children:
            raise ValueError("wxShapeSizer can only contain one child.")

        wxSizer.Add(self, widget)

    def CalcMin(self):
        isSizer, widget, width, height, borderSize = self.children[0]

        if isSizer:
            width, height = widget.CalcMin()

        return width, height

    def RecalcSizes(self):
        isSizer, widget, width, height, borderSize = self.children[0]
        width =self.size.width
        height =self.size.height
	px =self.origin.x
	py =self.origin.y
        anchor =self.anchor
        # get current dimensions of the managed window
        w, h =self.CalcMin()
        ratio =float(w) /h
        # in what direction space should be added:
        #   -1: horisontal
        #    1: vertical
        #    0: shape is ok
        dir =cmp(ratio /width *height, 1)
        if dir <0:
            # recalculate width
            old_width =width
            width =height *ratio
            if anchor & wxANCHOR_EAST:
                px =px +old_width -width
            elif not (anchor & wxANCHOR_WEST):
                px =px +(old_width -width) /2
        elif dir >0:
            # recalculate height
            old_height =height
            height =width /ratio
            if anchor & wxANCHOR_SOUTH:
                py =py +old_height -height
            elif not (anchor & wxANCHOR_NORTH):
                py =py +(old_height -height) /2

        widget.SetDimensions(px, py, width, height)
