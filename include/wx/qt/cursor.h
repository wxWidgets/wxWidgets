/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CURSOR_H_
#define _WX_QT_CURSOR_H_

#include "wx/image.h"

class QCursor;

class WXDLLIMPEXP_CORE wxCursor : public wxCursorBase
{
public:
    wxCursor() { }
    wxCursor(wxStockCursor id) { InitFromStock(id); }
#if WXWIN_COMPATIBILITY_2_8
    wxCursor(int id) { InitFromStock((wxStockCursor)id); }
#endif
#if wxUSE_IMAGE
    wxCursor( const wxImage & image );
    wxCursor(const char* const* xpmData);
#endif // wxUSE_IMAGE
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    virtual wxPoint GetHotSpot() const wxOVERRIDE;
    QCursor &GetHandle() const;

protected:
    void InitFromStock( wxStockCursor cursorId );
#if wxUSE_IMAGE
    void InitFromImage( const wxImage & image );
#endif

private:
    void Init();
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif // _WX_QT_CURSOR_H_
