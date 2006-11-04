/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/dcprint.h
// Purpose:     wxPrinterDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#include "wx/dc.h"
#include "wx/cmndata.h"

class wxNativePrinterDC ;

class WXDLLEXPORT wxPrinterDC: public wxDC
{
 public:
#if wxUSE_PRINTING_ARCHITECTURE
  DECLARE_CLASS(wxPrinterDC)

  // Create a printer DC
  wxPrinterDC(const wxPrintData& printdata );
  virtual ~wxPrinterDC();

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) ;
    virtual void EndDoc(void) ;
    virtual void StartPage(void) ;
    virtual void EndPage(void) ;

    wxRect GetPaperRect();

    wxPrintData& GetPrintData() { return m_printData; }
    virtual wxSize GetPPI() const;
 protected:
    virtual void DoGetSize( int *width, int *height ) const;
    wxPrintData   m_printData ;
    wxNativePrinterDC* m_nativePrinterDC ;
#endif // wxUSE_PRINTING_ARCHITECTURE
};

#endif
    // _WX_DCPRINT_H_

