/////////////////////////////////////////////////////////////////////////////
// Name:        printwin.h
// Purpose:     wxWindowsPrinter, wxWindowsPrintPreview classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTWIN_H_
#define _WX_PRINTWIN_H_

#ifdef __GNUG__
#pragma interface "printwin.h"
#endif

#include "wx/prntbase.h"

/*
 * Represents the printer: manages printing a wxPrintout object
 */
 
class WXDLLEXPORT wxMacPrinter: public wxPrinterBase
{
  DECLARE_DYNAMIC_CLASS(wxMacPrinter)

 public:
  wxMacPrinter(wxPrintData *data = NULL);
  ~wxMacPrinter(void);

  virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE);
  virtual bool PrintDialog(wxWindow *parent);
  virtual bool Setup(wxWindow *parent);

 private:
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */
 
class WXDLLEXPORT wxMacPrintPreview: public wxPrintPreviewBase
{
  DECLARE_CLASS(wxMacPrintPreview)

 public:
  wxMacPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxMacPrintPreview(void);

  virtual bool Print(bool interactive);
  virtual void DetermineScaling(void);
};

#endif
    // _WX_PRINTWIN_H_
