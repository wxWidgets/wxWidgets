/////////////////////////////////////////////////////////////////////////////
// Name:        print.h
// Purpose:     wxPrinter, wxPrintPreview classes
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINT_H_
#define _WX_PRINT_H_

#include "wx/prntbase.h"

/*
 * Represents the printer: manages printing a wxPrintout object
 */

class WXDLLEXPORT wxPrinter: public wxPrinterBase
{
  DECLARE_DYNAMIC_CLASS(wxPrinter)

 public:
  wxPrinter(wxPrintData *data = NULL);
  ~wxPrinter();

  virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE);
  virtual wxDC* PrintDialog(wxWindow *parent);
  virtual bool Setup(wxWindow *parent);
private:
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */

class WXDLLEXPORT wxPrintPreview: public wxPrintPreviewBase
{
  DECLARE_CLASS(wxPrintPreview)

 public:
  wxPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxPrintPreview();

  virtual bool Print(bool interactive);
  virtual void DetermineScaling();
};

#endif
    // _WX_PRINT_H_
