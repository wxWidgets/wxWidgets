/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cursor.h
// Purpose:     wxCursor class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/bitmap.h"

// Cursor
class WXDLLIMPEXP_CORE wxCursor : public wxCursorBase
{
public:
    wxCursor();

    wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot)
        : wxCursor(bitmap, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxBitmap& bitmap, int hotSpotX = 0, int hotSpotY = 0);
#if wxUSE_IMAGE
    wxCursor(const wxImage & image) ;
    wxCursor(const char* const* xpmData);
#endif // wxUSE_IMAGE
    wxCursor(const wxString& name, wxBitmapType type, const wxPoint& hotSpot)
        : wxCursor(name, type, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    wxCursor(wxStockCursor id) { InitFromStock(id); }

    void MacInstall() const ;

    void SetHCURSOR(WXHCURSOR cursor);
    WXHCURSOR GetHCURSOR() const;

    virtual wxPoint GetHotSpot() const override;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    void InitFromStock(wxStockCursor);
    void InitFromBitmap(const wxBitmap& bitmap, int hotSpotX, int hotSpotY);

#if wxUSE_IMAGE
    void InitFromImage(const wxImage & image) ;
#endif // wxUSE_IMAGE

    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif // _WX_CURSOR_H_
