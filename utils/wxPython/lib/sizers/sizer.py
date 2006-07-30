#----------------------------------------------------------------------
# Name:        wxPython.lib.sizers.sizer
# Purpose:     General purpose sizer/layout managers for wxPython
#
# Author:      Robin Dunn and Dirk Holtwick
#
# Created:     17-May-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1998 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from wxPython.wx import wxPoint, wxSize

#----------------------------------------------------------------------

class wxSizer:
    """
    wxSizer

    An abstract base sizer class.  A sizer is able to manage the size and
    layout of windows and/or child sizers.

    Derived classes should implement CalcMin, and RecalcSizes.

    A window or sizer is added to this sizer with the Add method:

        def Add(self, widget, opt=0)

    The meaning of the opt parameter is different for each type of
    sizer.  It may be a single value or a collection of values.
    """
    def __init__(self, size = None):
        self.children = []
        self.origin = wxPoint(0, 0)
        if not size:
            size = wxSize(0,0)
        self.size   = size

    def Add(self, widget, opt=0):
        """
        Add a window or a sizer to this sizer.  The meaning of the opt
        parameter is different for each type of sizer.  It may be a single
        value or a collection of values.
        """
        size = widget.GetSize()
        isSizer = isinstance(widget, wxSizer)
        self.children.append( (isSizer, widget, size.width, size.height, opt) )


    def AddMany(self, widgets):
        """
        Add a sequence (list, tuple, etc.) of widgets to this sizer.  The
        items in the sequence should be tuples containing valid args for
        the Add method.
        """
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            apply(self.Add, childinfo)


    def SetDimensions(self, x, y, width, height):
        self.origin = wxPoint(x, y)
        self.size   = wxSize(width, height)
        self.RecalcSizes()

    def GetSize(self):
        return self.size

    def GetPosition(self):
        return self.origin

    def CalcMin(self):
        raise NotImplementedError("Derived class should implement CalcMin")

    def RecalcSizes(self):
        raise NotImplementedError("Derived class should implement RecalcSizes")



    def __getMinWindowSize(self, win):
        """
        Calculate the best size window to hold this sizer, taking into
        account the difference between client size and window size.
        """
	min = self.GetMinSize()
	a1,a2 = win.GetSizeTuple()
	b1,b2 = win.GetClientSizeTuple()
	w = min.width  + (a1 - b1)
	h = min.height + (a2 - b2)
        return (w, h)


    def GetMinSize(self):
        minWidth, minHeight = self.CalcMin()
        return wxSize(minWidth, minHeight)

    def SetWindowSizeHints(self, win):
        w, h = self.__getMinWindowSize(win)
	win.SetSizeHints(w,h)

    def FitWindow(self, win):
        w, h = self.__getMinWindowSize(win)
	win.SetSize(wxSize(w,h))

    def Layout(self, size):
        self.CalcMin()
        self.SetDimensions(self.origin.x, self.origin.y,
                           size.width, size.height)

#----------------------------------------------------------------------
