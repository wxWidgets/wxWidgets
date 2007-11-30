/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:     wxScreenDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"
#include "wx/mac/carbon/dcclient.h"

class WXDLLEXPORT wxScreenDCImpl: public wxWindowDCImpl
{
public:
    wxScreenDCImpl( wxDC *owner );
    virtual ~wxScreenDCImpl();

private:
    void* m_overlayWindow;
        
private:
    DECLARE_CLASS(wxScreenDCImpl)
    DECLARE_NO_COPY_CLASS(wxScreenDCImpl)
};

#endif
    // _WX_DCSCREEN_H_

