/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: dcscreen.h 50547 2007-12-06 16:22:00Z PC $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"
#include "wx/gtk/dcclient.h"

class WXDLLIMPEXP_CORE wxScreenDCImpl: public wxWindowDCImpl
{
public:
    wxScreenDCImpl( wxDC *owner );
    virtual ~wxScreenDCImpl();

    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const;
private:
    void* m_overlayWindow;

private:
    DECLARE_CLASS(wxScreenDCImpl)
    wxDECLARE_NO_COPY_CLASS(wxScreenDCImpl);
};

#endif
    // _WX_DCSCREEN_H_

