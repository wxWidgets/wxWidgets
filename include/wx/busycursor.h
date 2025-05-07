///////////////////////////////////////////////////////////////////////////////
// Name:        wx/busycursor.h
// Purpose:     Functions and class for showing/hiding busy cursor.
// Author:      Vadim Zeitlin
// Created:     2025-05-08 (extracted from wx/utils.h)
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUSYCURSOR_H_
#define _WX_BUSYCURSOR_H_

#include "wx/gdicmn.h"

// Set the cursor to the busy cursor for all windows
WXDLLIMPEXP_CORE void wxBeginBusyCursor(const wxCursor *cursor = wxHOURGLASS_CURSOR);
WXDLLIMPEXP_CORE void wxBeginBusyCursor(const wxCursorBundle& cursors);

// Restore cursor to normal
WXDLLIMPEXP_CORE void wxEndBusyCursor();

// true if we're between the above two calls
WXDLLIMPEXP_CORE bool wxIsBusy();

// Convenience class so we can just create a wxBusyCursor object on the stack
class WXDLLIMPEXP_CORE wxBusyCursor
{
public:
    explicit wxBusyCursor(const wxCursor* cursor = wxHOURGLASS_CURSOR)
        { wxBeginBusyCursor(cursor); }
    explicit wxBusyCursor(const wxCursorBundle& cursors)
        { wxBeginBusyCursor(cursors); }
    ~wxBusyCursor()
        { wxEndBusyCursor(); }

#if WXWIN_COMPATIBILITY_3_2
    // These functions are kept only for compatibility, they were never
    // available in all ports and never did anything really useful.
    wxDEPRECATED_MSG("Returns invalid cursor, don't use it")
    static wxCursor GetStoredCursor();

    wxDEPRECATED_MSG("Returns invalid cursor, don't use it")
    static wxCursor GetBusyCursor();
#endif // WXWIN_COMPATIBILITY_3_2
};

// This class only exists for compatibility, it never did anything really
// useful and now doesn't do anything at all.
class wxDEPRECATED_MSG("Does nothing, stop using it") wxBusyCursorSuspender
{
public:
    wxBusyCursorSuspender() = default;
    ~wxBusyCursorSuspender() = default;
};

#endif // _WX_BUSYCURSOR_H_
