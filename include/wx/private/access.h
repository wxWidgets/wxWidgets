///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/access.h
// Purpose:     Private accessibility helpers used by the generic controls.
// Author:      Vadim Zeitlin
// Created:     2026-09-30
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ACCESS_H_
#define _WX_PRIVATE_ACCESS_H_

#include "wx/gdicmn.h"
#include "wx/string.h"

#include <vector>

class WXDLLIMPEXP_FWD_CORE wxWindow;

namespace wxPrivate
{

// Part of a custom drawn window, which is not a window itself, but should
// still be visible to the accessibility clients, e.g. a status bar field.
struct AccessibleElement
{
    AccessibleElement(const wxString& label_, const wxRect& rect_)
        : label(label_), rect(rect_)
    {
    }

    // The text read by the screen readers.
    wxString label;

    // Rectangle occupied by the element in the client coordinates of the
    // window it belongs to.
    wxRect rect;
};

using AccessibleElements = std::vector<AccessibleElement>;

// Row of a custom drawn control presented as a table, e.g. a list control item.
struct AccessibleRow
{
    AccessibleRow(long index_, const wxRect& rect_)
        : index(index_), rect(rect_)
    {
    }

    // Index of the row in the control, which is not the same as its index in
    // the rows passed to SetAccessibleTable() as only the shown rows are.
    long index;

    // Rectangle of the entire row in the client coordinates.
    wxRect rect;

    // Cells of this row: there must be one of them for each column, or just
    // one if the control doesn't have columns.
    AccessibleElements cells;

    // True if the row is currently selected.
    bool selected = false;
};

using AccessibleRows = std::vector<AccessibleRow>;

// The functions below are currently implemented for wxOSX and wxGTK3 only and
// do nothing elsewhere.
//
// SetAccessibleElements() lets the accessibility clients see the given
// elements as the children of this window, using the "static text" role.
//
// Calling it again replaces the elements set by the previous call, but does
// nothing if they didn't change, so it's fine to call it whenever the elements
// might have changed. Passing an empty vector removes them.
//
// SetAccessibleTable() lets the accessibility clients see this window as a
// table (or a list, if it has a single column) with the given rows. Only the
// rows which are currently shown should be given, as there may be too many of
// them otherwise, while numRows is the total number of rows. Just as
// SetAccessibleElements(), it updates the existing elements instead of
// recreating them when possible and can be called whenever the rows might
// have changed.
//
// SetAccessibleCurrentRow() tells the accessibility clients which row is the
// current one, which is necessary for the screen readers to announce it when
// it changes: they follow the focus, but have no way of knowing where it is
// inside a custom drawn control on their own. The row must be one of those
// passed to the last SetAccessibleTable() call or -1 if there is none.
#if defined(__WXOSX_COCOA__) || (defined(__WXGTK3__) && !defined(__WXGTK4__))

WXDLLIMPEXP_CORE void
SetAccessibleElements(wxWindow* win, const AccessibleElements& elements);

WXDLLIMPEXP_CORE void
SetAccessibleTable(wxWindow* win, const AccessibleRows& rows, long numRows);

WXDLLIMPEXP_CORE void
SetAccessibleCurrentRow(wxWindow* win, long row);

#else // !wxOSX && !wxGTK3

inline void
SetAccessibleElements(wxWindow* WXUNUSED(win),
                      const AccessibleElements& WXUNUSED(elements))
{
}

inline void
SetAccessibleTable(wxWindow* WXUNUSED(win),
                   const AccessibleRows& WXUNUSED(rows),
                   long WXUNUSED(numRows))
{
}

inline void
SetAccessibleCurrentRow(wxWindow* WXUNUSED(win), long WXUNUSED(row))
{
}

#endif // wxOSX || wxGTK3

} // namespace wxPrivate

#endif // _WX_PRIVATE_ACCESS_H_
