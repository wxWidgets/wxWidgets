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

#ifdef __GNUG__
    #pragma interface "cursor.h"
#endif

// compatible (even if incorrect) behaviour by default: derive wxCursor from
// wxBitmap
#ifndef wxICON_IS_BITMAP
    #define wxICON_IS_BITMAP 1
#endif

#if wxICON_IS_BITMAP
    #include "wx/bitmap.h"

    #define wxCursorRefDataBase   wxBitmapRefData
    #define wxCursorBase          wxBitmap
#else
    #include "wx/msw/gdiimage.h"

    #define wxCursorRefDataBase   wxGDIImageRefData
    #define wxCursorBase          wxGDIImage
#endif

class WXDLLEXPORT wxCursorRefData : public wxCursorRefDataBase
{
public:
    wxCursorRefData();
    virtual ~wxCursorRefData() { Free(); }

    virtual void Free();

    // for compatibility
public:
    bool m_destroyCursor;
};

// Cursor
class WXDLLEXPORT wxCursor : public wxCursorBase
{
public:
    wxCursor();

    // Copy constructors
    wxCursor(const wxCursor& cursor) { Ref(cursor); }

    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = NULL);
    wxCursor(const wxString& name,
             long flags = wxBITMAP_TYPE_CUR_RESOURCE,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(int cursor_type);
    virtual ~wxCursor();

    wxCursor& operator = (const wxCursor& cursor) { if (*this == cursor) return (*this); Ref(cursor); return *this; }
    bool operator == (const wxCursor& cursor) const { return m_refData == cursor.m_refData; }
    bool operator != (const wxCursor& cursor) const { return m_refData != cursor.m_refData; }

    void SetHCURSOR(WXHCURSOR cursor) { SetHandle((WXHANDLE)cursor); }
    WXHCURSOR GetHCURSOR() const { return (WXHCURSOR)GetHandle(); }

protected:
    virtual wxGDIImageRefData *CreateData() const { return new wxCursorRefData; }

private:
    DECLARE_DYNAMIC_CLASS(wxCursor)
};

#endif
    // _WX_CURSOR_H_
