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

class WXDLLEXPORT wxPrinterDC: public wxDC
{
 public:
  DECLARE_CLASS(wxPrinterDC)

  // Create a printer DC
  wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, bool interactive = TRUE, int orientation = wxPORTRAIT);
  wxPrinterDC(WXHDC theDC);

  ~wxPrinterDC(void);
};

// Gets an HDC for the default printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(int orientation);

#endif
    // _WX_DCPRINT_H_

