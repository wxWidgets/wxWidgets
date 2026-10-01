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

// Let the accessibility clients see the given elements as the children of this
// window, using the "static text" role.
//
// Calling this function again replaces the elements set by the previous call,
// but does nothing if they didn't change, so it's fine to call it whenever the
// elements might have changed. Passing an empty vector removes them.
//
// This function is currently implemented for wxOSX and wxGTK3 only and does
// nothing elsewhere.
#if defined(__WXOSX_COCOA__) || (defined(__WXGTK3__) && !defined(__WXGTK4__))

WXDLLIMPEXP_CORE void
SetAccessibleElements(wxWindow* win, const AccessibleElements& elements);

#else // !wxOSX && !wxGTK3

inline void
SetAccessibleElements(wxWindow* WXUNUSED(win),
                      const AccessibleElements& WXUNUSED(elements))
{
}

#endif // wxOSX || wxGTK3

} // namespace wxPrivate

#endif // _WX_PRIVATE_ACCESS_H_
