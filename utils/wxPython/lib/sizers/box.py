#----------------------------------------------------------------------
# Name:        wxPython.lib.sizers.box
# Purpose:     A sizer/layout managers for wxPython that places items in
#              a stretchable box
#
# Author:      Robin Dunn and Dirk Holtwick
#
# Created:     17-May-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1998 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from sizer       import wxSizer
from wxPython.wx import wxVERTICAL, wxHORIZONTAL

#----------------------------------------------------------------------


class wxBoxSizer(wxSizer):
    """
    wxBoxSizer

    A Sizer that lays components out in a box, in the order they are
    added to the layout manager, with a given orientation.  The
    orientation is specified in the constructor with either wxVERTICAL
    or wxHORIZONTAL.

    The optional parameter to the Add method (for this sizer it's
    called the stretch flag) can be used to flag one or more components
    as stretchable, meaning that they will expand to fill available
    space in the given orientation.  The default is zero, or not
    stretchable.

    If the stretch flag is non-zero then the widget will stretch. If
    the sizer holds more than one item that is stretchable then they
    share the available space.

    If the strech flag is greater than 1 then it serves as a weighting
    factor.  Widgets with a flag of 2 will get twice as much space as
    widgets with 1, etc.
    """
    def __init__(self, orientation, size = None):
        wxSizer.__init__(self, size)
        self.orientation = orientation


    def CalcMin(self):
	self.stretchable = 0   # number of stretchable items
	self.minWidth    = 0   # minimal size
	self.minHeight   = 0
	self.fixedWidth  = 0   # size without stretched widgets
	self.fixedHeight = 0

	# iterate through children
	for (isSizer, widget, width, height, stretch) in self.children:
            weight = 1
            if stretch:
                weight = stretch

	    if isSizer:
		# let sub-sizers recalc their required space
		width, height = widget.CalcMin()

	    # minimal size
	    if self.orientation == wxVERTICAL:
		self.minHeight = self.minHeight + (height * weight)
		self.minWidth  = max(self.minWidth, width)
	    else:
		self.minWidth  = self.minWidth + (width * weight)
		self.minHeight = max(self.minHeight, height)

	    # stretchable items
	    if stretch:
                self.stretchable = self.stretchable + weight
	    else:
		if self.orientation == wxVERTICAL:
		    self.fixedHeight = self.fixedHeight + height
		    self.fixedWidth = max(self.fixedWidth, width)
		else:
		    self.fixedWidth = self.fixedWidth + width
		    self.fixedHeight = max(self.fixedHeight, height)

	return self.minWidth, self.minHeight



    def RecalcSizes(self):
        # get current dimensions, for performance
        myWidth  = self.size.width
        myHeight = self.size.height

	# relative recent positions & sizes
	px = self.origin.x
	py = self.origin.y
	newWidth = 0
	newHeight = 0

	# calculate space for one stretched item
        if self.stretchable:
            if self.orientation == wxHORIZONTAL:
                delta = (myWidth - self.fixedWidth) / self.stretchable
                extra = (myWidth - self.fixedWidth) % self.stretchable
            else:
                delta = (myHeight - self.fixedHeight) / self.stretchable
                extra = (myHeight - self.fixedHeight) % self.stretchable

	# iterate children ...
	for (isSizer, widget, width, height, stretch) in self.children:
            weight = 1
            if stretch:
                weight = stretch

	    if isSizer:
		width, height = widget.CalcMin()

	    # ... vertical
	    if self.orientation == wxVERTICAL:
                newHeight = height
		if stretch:
		    newHeight = (delta * weight) + extra  # first stretchable gets extra pixels
                    extra = 0
                widget.SetDimensions(px, py, myWidth, newHeight)

	    # ... horizontal
	    elif self.orientation == wxHORIZONTAL:
                newWidth = width
		if stretch:
		    newWidth = (delta * weight) + extra  # first stretchable gets extra pixels
                    extra = 0
		widget.SetDimensions(px, py, newWidth, myHeight)

	    px = px + newWidth
	    py = py + newHeight


#----------------------------------------------------------------------
