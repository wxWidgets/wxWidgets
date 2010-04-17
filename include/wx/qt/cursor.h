/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:
// Author:      Peter Most
// Id:          $Id: cursor.h 60648 2009-05-16 10:42:48Z FM $
// Copyright:   (c) 2009 Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CURSOR_H_
#define _WX_QT_CURSOR_H_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"
#include <QtGui/QCursor>

class WXDLLIMPEXP_CORE wxCursor : public wxGDIObject
{
public:
    wxCursor();
    wxCursor( const wxCursor & );
    wxCursor(wxStockCursor id)
        { InitFromStock(id); }

protected:
    void InitFromStock( wxStockCursor );
    
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
    
private:
    QCursor m_qtCursor;
    
    DECLARE_DYNAMIC_CLASS( wxCursor )
};

#endif // _WX_QT_CURSOR_H_
