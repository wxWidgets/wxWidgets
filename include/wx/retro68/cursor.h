/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/cursor.h
// Purpose:     wxCursor class declaration
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_CURSOR_H_
#define _WX_RETRO68_CURSOR_H_

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxImage;

// Toolbox cursor structure, declared in Quickdraw.h
struct Cursor;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

// Classic Mac OS supports only 16x16 monochrome cursors with a mask.
class WXDLLIMPEXP_CORE wxCursor : public wxCursorBase
{
public:
    wxCursor() = default;
    wxCursor(wxStockCursor id) { InitFromStock(id); }
#if wxUSE_IMAGE
    wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot)
        : wxCursor(bitmap, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxBitmap& bitmap, int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(const wxImage& image);
    wxCursor(const char* const* xpmData);
#endif // wxUSE_IMAGE
    wxCursor(const wxString& name, wxBitmapType type, const wxPoint& hotSpot)
        : wxCursor(name, type, hotSpot.x, hotSpot.y) { }
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    // implementation
    virtual wxPoint GetHotSpot() const override;

    // Return the native cursor.
    const Cursor* GetMacCursor() const;

    // Make this cursor current.
    void MacInstall() const;

protected:
    void InitFromStock(wxStockCursor);

#if wxUSE_IMAGE
    void InitFromImage(const wxImage& image);
#endif // wxUSE_IMAGE

    // ref counting code
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif // _WX_RETRO68_CURSOR_H_
