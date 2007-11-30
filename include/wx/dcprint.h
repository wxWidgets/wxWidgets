/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcprint.h
// Purpose:     wxPrinterDC base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_BASE_
#define _WX_DCPRINT_H_BASE_

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dc.h"

//-----------------------------------------------------------------------------
// wxPrinterDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPrinterDC: public wxDC
{
public:
    wxPrinterDC();
    wxPrinterDC( const wxPrintData& data );
    ~wxPrinterDC();
    
    wxRect GetPaperRect();
    int GetResolution();
    
private:
    DECLARE_DYNAMIC_CLASS()
};


#endif 
    // wxUSE_PRINTING_ARCHITECTURE
    
#endif
    // _WX_DCPRINT_H_BASE_
