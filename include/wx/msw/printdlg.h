/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.h
// Purpose:     wxPrintDialog, wxPageSetupDialog classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTDLG_H_
#define _WX_PRINTDLG_H_

#ifdef __GNUG__
#pragma interface "printdlg.h"
#endif

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * wxPrinterDialog
 * The common dialog for printing.
 */

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxPrintDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(wxPrintDialog)

 private:
  wxPrintData printData;
  wxDC *printerDC;
  bool destroyDC;
  char *deviceName;
  char *driverName;
  char *portName;
  wxWindow *dialogParent;
 public:
  wxPrintDialog(void);
  wxPrintDialog(wxWindow *parent, wxPrintData* data = NULL);
  ~wxPrintDialog(void);

  bool Create(wxWindow *parent, wxPrintData* data = NULL);
  virtual int ShowModal(void);

  inline wxPrintData& GetPrintData(void) { return printData; }
  virtual wxDC *GetPrintDC(void);
};

class WXDLLEXPORT wxPageSetupDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupDialog)

 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
 public:
  wxPageSetupDialog(void);
  wxPageSetupDialog(wxWindow *parent, wxPageSetupData *data = NULL);
  ~wxPageSetupDialog(void);

  bool Create(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual int ShowModal(void);

  inline wxPageSetupData& GetPageSetupData(void) { return m_pageSetupData; }
};

#endif
    // _WX_PRINTDLG_H_
