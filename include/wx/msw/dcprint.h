/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DCPRINTH__
#define __DCPRINTH__

#ifdef __GNUG__
#pragma interface "dcprint.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
 public:
  DECLARE_CLASS(wxPrinterDC)

  // Create a printer DC
  wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, const bool interactive = TRUE, const int orientation = wxPORTRAIT);
  wxPrinterDC(WXHDC theDC);

  ~wxPrinterDC(void);
};

// Gets an HDC for the default printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(int orientation);

#endif
    // __DCPRINTH__

