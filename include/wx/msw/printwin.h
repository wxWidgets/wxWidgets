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
 
class WXDLLEXPORT wxWindowsPrinter: public wxPrinterBase
{
  DECLARE_DYNAMIC_CLASS(wxWindowsPrinter)

 private:
  WXFARPROC lpAbortProc;
 public:
  wxWindowsPrinter(wxPrintData *data = NULL);
  ~wxWindowsPrinter(void);

  virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE);
  virtual bool PrintDialog(wxWindow *parent);
  virtual bool Setup(wxWindow *parent);
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */
 
class WXDLLEXPORT wxWindowsPrintPreview: public wxPrintPreviewBase
{
  DECLARE_CLASS(wxWindowsPrintPreview)

 public:
  wxWindowsPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxWindowsPrintPreview(void);

  virtual bool Print(bool interactive);
  virtual void DetermineScaling(void);
};

#endif
    // _WX_PRINTWIN_H_
