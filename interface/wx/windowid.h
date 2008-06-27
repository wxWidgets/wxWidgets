/////////////////////////////////////////////////////////////////////////////
// Name:        windowid.h
// Purpose:     interface of wxIdManager
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxIdManager

    wxIdManager is responsible for allocating and releasing window IDs.  It
    is used by wxWindow::NewControlId and
    wxWindow::UnreserveControlId, and can also
    be used be used directly.

    @library{wxcore}
    @category{FIXME}

    @see wxWindow::NewControlId, wxWindow::UnreserveControlId, @ref
    overview_windowidsoverview "Window IDs overview"
*/
class wxIdManager
{
public:
    /**
        Called directly by wxWindow::NewControlId,
        this function will create a new ID or range of IDs.  The IDs will be
        reserved until assigned to a wxWindowIDRef()
        or unreserved with UnreserveControlId().
        Only ID values that are not assigned to a wxWindowIDRef()
        need to be unreserved.

        @param count
            The number of sequential IDs to reserve.

        @return The value of the first ID in the sequence, or wxID_NONE.
    */
    static wxWindowID ReserveControlId(int count = 1);
};

