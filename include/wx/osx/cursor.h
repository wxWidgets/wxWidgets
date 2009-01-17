/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/cursor.h
// Purpose:     wxCursor class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#include "wx/bitmap.h"

// Cursor
class WXDLLIMPEXP_CORE wxCursor : public wxGDIObject
{
public:
    wxCursor();

    wxCursor(const wxImage & image) ;
    wxCursor(const char* const* bits);
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    wxCursor(wxStockCursor id) { InitFromStock(id); }
#if WXWIN_COMPATIBILITY_2_8
    wxCursor(int id) { InitFromStock((wxStockCursor)id); }
#endif
    virtual ~wxCursor();

    bool CreateFromXpm(const char* const* bits);

    void MacInstall() const ;

    void SetHCURSOR(WXHCURSOR cursor);
    WXHCURSOR GetHCURSOR() const;

private:
    void InitFromStock(wxStockCursor);

    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    void CreateFromImage(const wxImage & image) ;

    DECLARE_DYNAMIC_CLASS(wxCursor)
};

extern WXDLLIMPEXP_CORE void wxSetCursor(const wxCursor& cursor);

#endif // _WX_CURSOR_H_
