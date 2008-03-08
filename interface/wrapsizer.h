/////////////////////////////////////////////////////////////////////////////
// Name:        wrapsizer.h
// Purpose:     documentation for wxWrapSizer class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWrapSizer
    @wxheader{wrapsizer.h}

    A wrap sizer lays out its items in a single line, like a box sizer -- as long
    as there is space available in that direction. Once all available space in
    the primary direction has been used, a new line is added and items are added
    there.

    So a wrap sizer has a primary orientation for adding items, and adds lines
    as needed in the secondary direction.

    @library{wxcore}
    @category{winlayout}

    @seealso
    wxBoxSizer, wxSizer, @ref overview_sizeroverview "Sizer overview"
*/
class wxWrapSizer : public wxBoxSizer
{
public:
    /**
        Constructor for a wxWrapSizer. @e orient determines the primary direction of
        the sizer (the most common case being @c wxHORIZONTAL). The flags
        parameter may have the value @c wxEXTEND_LAST_ON_EACH_LINE which will
        cause the last item on each line to use any remaining space on that line.
    */
    wxWrapSizer(int orient, int flags);

    /**
        Not used by an application. This is the mechanism by which sizers can inform
        sub-items of the first determined size component. The sub-item can then better
        determine its size requirements.
        
        Returns @true if the information was used (and the sub-item min size was
        updated).
    */
    bool InformFirstDirection(int direction, int size,
                              int availableOtherDir);
};
