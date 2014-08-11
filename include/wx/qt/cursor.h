/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CURSOR_H_
#define _WX_QT_CURSOR_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/image.h"

#include <QtGui/QCursor>

class WXDLLIMPEXP_CORE wxCursor : public wxGDIObject
{
public:
    wxCursor() { }
    wxCursor( const wxCursor & );
    wxCursor( const wxImage& image ) { InitFromImage(image); }
    wxCursor( const wxString& name,
              wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
              int hotSpotX = 0, int hotSpotY = 0 );
    wxCursor(wxStockCursor id) { InitFromStock(id); }
#if WXWIN_COMPATIBILITY_2_8
    wxCursor(int id) { InitFromStock((wxStockCursor)id); }
#endif

    QCursor m_qtCursor;

protected:
    void InitFromStock( wxStockCursor cursorId );
#if wxUSE_IMAGE
    void InitFromImage( const wxImage & image );
#endif

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
    
    DECLARE_DYNAMIC_CLASS( wxCursor )
};

#endif // _WX_QT_CURSOR_H_
