/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     wxPrinterDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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

  // Create a printer DC
  wxPrinterDC(const wxPrintData& printdata );
  ~wxPrinterDC();

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) ;
    virtual void EndDoc(void) ;
    virtual void StartPage(void) ;
    virtual void EndPage(void) ;
 protected :
 #if TARGET_CARBON
 	PMPrintContext  m_macPrintPort ;
 #else
 	TPPrPort 	m_macPrintPort ;
 #endif
 	wxPrintData m_printData ;
};

#endif
    // _WX_DCPRINT_H_

