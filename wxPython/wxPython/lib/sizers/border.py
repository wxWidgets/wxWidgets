#----------------------------------------------------------------------
# Name:        wxPython.lib.sizers.border
# Purpose:     A Sizer that wraps an empty border around its contents
#
# Author:      Robin Dunn
#
# Created:     9-June-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1998 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from sizer       import wxSizer

wxNORTH = 1
wxSOUTH = 2
wxEAST  = 4
wxWEST  = 8
wxALL   = wxNORTH | wxSOUTH | wxEAST | wxWEST

#----------------------------------------------------------------------

class wxBorderSizer(wxSizer):
    """
    wxBorderSizer

    This sizer provides an empty buffer on one or more sides of it's
    contents.  It can only hold a single widget, but that can be a
    sizer containing other items if you wish.

    The sizer is constructed with a parameter specifying which sides
    should have the border.  You can use a logical OR of the following
    values to specify the sides:

        wxNORTH  -- the top side
        wxSOUTH  -- the bottom side
        wxEAST   -- the right side
        wxWEST   -- the left side
        wxALL    -- all sides

    The width in pixels of the border is specified when the child
    widget is Added to the sizer.

    """
    def __init__(self, sides = wxALL):
        wxSizer.__init__(self)
        self.sides = sides


    def Add(self, widget, borderSize):
        if self.children:
            raise ValueError("wxBorderSizer can only contain one child.")

        wxSizer.Add(self, widget, borderSize)


    def CalcMin(self):
        isSizer, widget, width, height, borderSize = self.children[0]

        if isSizer:
            width, height = widget.CalcMin()

        if self.sides & wxEAST:
            width = width + borderSize

        if self.sides & wxWEST:
            width = width + borderSize

        if self.sides & wxNORTH:
            height = height + borderSize

        if self.sides & wxSOUTH:
            height = height + borderSize

        return width, height


    def RecalcSizes(self):
        isSizer, widget, width, height, borderSize = self.children[0]
        width  = self.size.width
        height = self.size.height
	px = self.origin.x
	py = self.origin.y

        if self.sides & wxWEST:
            width = width - borderSize
            px = px + borderSize
        if self.sides & wxEAST:
            width = width - borderSize

        if self.sides & wxNORTH:
            height = height - borderSize
            py = py + borderSize
        if self.sides & wxSOUTH:
            height = height - borderSize

        widget.SetDimensions(px, py, width, height)


#----------------------------------------------------------------------
#
# TODO...  Make an abstract class wxBorder whose decendants can be added to
#          a wxBorderSizer to provide drawing for the buffer area.  Ideas are
#          to provide a color border, beveled borders, rounded borders, etc.





