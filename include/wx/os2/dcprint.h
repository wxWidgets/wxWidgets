/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     wxPrinterDC class
// Author:      David Webster
// Modified by:
// Created:     09/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dc.h"
#include "wx/cmndata.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
 public:
  DECLARE_CLASS(wxPrinterDC)

   // Create a printer DC [obsolete]
   wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, bool interactive = TRUE, int orientation = wxPORTRAIT);

   // Create from print data
   wxPrinterDC(const wxPrintData& data);

   wxPrinterDC(WXHDC theDC);

   ~wxPrinterDC();

    bool StartDoc(const wxString& message);
    void EndDoc(void);
    void StartPage(void);
    void EndPage(void);

protected:
    wxPrintData         m_printData;
};

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& data);

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
    // _WX_DCPRINT_H_

