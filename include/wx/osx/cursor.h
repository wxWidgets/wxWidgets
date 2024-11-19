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

#if wxUSE_IMAGE
    wxCursor(const wxImage & image) ;
    wxCursor(const char* const* xpmData);
#endif // wxUSE_IMAGE
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
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    void InitFromStock(wxStockCursor);

#if wxUSE_IMAGE
    void InitFromImage(const wxImage & image) ;
#endif // wxUSE_IMAGE

    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif // _WX_CURSOR_H_
