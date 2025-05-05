/////////////////////////////////////////////////////////////////////////////
// Name:        wx/x11/cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/colour.h"

class WXDLLIMPEXP_FWD_CORE wxImage;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCursor : public wxCursorBase
{
public:
    wxCursor();
    wxCursor(wxStockCursor id) { InitFromStock(id); }
    wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot)
        : wxCursor(bitmap, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxBitmap& bitmap, int hotSpotX = 0, int hotSpotY = 0);
#if wxUSE_IMAGE
    wxCursor( const wxImage & image );
    wxCursor(const char* const* xpmData);
#endif

    wxCursor(const wxString& name, wxBitmapType type, const wxPoint& hotSpot)
        : wxCursor(name, type, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    // implementation

    WXCursor GetCursor() const;

protected:
    void InitFromStock(wxStockCursor);

    virtual wxGDIRefData *CreateGDIRefData() const;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif // _WX_CURSOR_H_
