/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "cursor.h"
#endif

#include "wx/msw/gdiimage.h"

class WXDLLEXPORT wxImage;

// Cursor
class WXDLLEXPORT wxCursor : public wxGDIImage
{
public:
    // constructors
    wxCursor();
    wxCursor(const wxCursor& cursor) : wxGDIImage(cursor) { Ref(cursor); }
    wxCursor(const wxImage& image);
    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = NULL);
    wxCursor(const wxString& name,
             long flags = wxBITMAP_TYPE_CUR_RESOURCE,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(int idCursor);
    virtual ~wxCursor();

    wxCursor& operator=(const wxCursor& cursor)
        { if (*this == cursor) return (*this); Ref(cursor); return *this; }

    bool operator==(const wxCursor& cursor) const;
    bool operator!=(const wxCursor& cursor) const
        { return !(*this == cursor); }

    // implementation only
    void SetHCURSOR(WXHCURSOR cursor) { SetHandle((WXHANDLE)cursor); }
    WXHCURSOR GetHCURSOR() const { return (WXHCURSOR)GetHandle(); }

protected:
    virtual wxGDIImageRefData *CreateData() const;

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif
    // _WX_CURSOR_H_
