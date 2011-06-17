/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
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
    wxScreenDCImpl( wxScreenDC *owner );
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

