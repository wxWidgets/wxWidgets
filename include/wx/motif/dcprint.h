/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/dcprint.h
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#include "wx/dc.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
public:
    DECLARE_CLASS(wxPrinterDC)

    // Create a printer DC
    wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, bool interactive = true, int orientation = wxPORTRAIT);

    virtual ~wxPrinterDC();
};

#endif
// _WX_DCPRINT_H_
