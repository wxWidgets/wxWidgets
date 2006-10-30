/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/object.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxImage;

// Cursor
class WXDLLEXPORT wxCursor: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxCursor)

public:
    wxCursor();

    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = NULL);

    wxCursor(const wxString& name, long flags = wxBITMAP_TYPE_XBM,
        int hotSpotX = 0, int hotSpotY = 0);

#if wxUSE_IMAGE
    wxCursor(const wxImage& image);
#endif

    wxCursor(wxStockCursor id);
    virtual ~wxCursor();

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk() const;

    // Motif-specific.
    // Create/get a cursor for the current display
    WXCursor GetXCursor(WXDisplay* display) const;
private:
    void Create(const char bits[], int width, int height,
                int hotSpotX = -1, int hotSpotY = -1,
                const char maskBits[] = NULL);
    void Create(WXPixmap cursor, WXPixmap mask, int hotSpotX, int hotSpotY);

    // Make a cursor from standard id
    WXCursor MakeCursor(WXDisplay* display, wxStockCursor id) const;
};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif
// _WX_CURSOR_H_
