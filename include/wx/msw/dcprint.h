/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#ifdef __GNUG__
#pragma interface "dcprint.h"
#endif

#include "wx/dc.h"
#include "wx/cmndata.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
public:
DECLARE_CLASS(wxPrinterDC)

    // Create a printer DC (obsolete function: use wxPrintData version now)
    wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, bool interactive = TRUE, int orientation = wxPORTRAIT);

    // Create from print data
    wxPrinterDC(const wxPrintData& data);

    wxPrinterDC(WXHDC theDC);

    ~wxPrinterDC(void);

    bool StartDoc(const wxString& message);
    void EndDoc(void);
    void StartPage(void);
    void EndPage(void);

protected:
    wxPrintData m_printData;
};

// Gets an HDC for the default printer configuration
// WXHDC WXDLLEXPORT wxGetPrinterDC(int orientation);

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& data);

#endif
    // _WX_DCPRINT_H_

