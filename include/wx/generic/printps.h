/////////////////////////////////////////////////////////////////////////////
// Name:        printps.h
// Purpose:     wxPostScriptPrinter, wxPostScriptPrintPreview
//              wxGenericPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PRINTPSH__
#define __PRINTPSH__

#ifdef __GNUG__
#pragma interface "printps.h"
#endif

#include "wx/prntbase.h"

/*
 * Represents the printer: manages printing a wxPrintout object
 */
 
class WXDLLEXPORT wxPostScriptPrinter: public wxPrinterBase
{
  DECLARE_DYNAMIC_CLASS(wxPostScriptPrinter)

 public:
  wxPostScriptPrinter(wxPrintData *data = NULL);
  ~wxPostScriptPrinter(void);

  virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE);
  virtual bool PrintDialog(wxWindow *parent);
  virtual bool Setup(wxWindow *parent);
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */
 
class WXDLLEXPORT wxPostScriptPrintPreview: public wxPrintPreviewBase
{
  DECLARE_CLASS(wxPostScriptPrintPreview)

 public:
  wxPostScriptPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxPostScriptPrintPreview(void);

  virtual bool Print(bool interactive);
  virtual void DetermineScaling(void);
};

#endif
    // __PRINTPSH__
