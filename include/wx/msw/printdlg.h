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

public:
  wxPrintDialog(void);
  wxPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
  ~wxPrintDialog(void);

  bool Create(wxWindow *parent, wxPrintDialogData* data = NULL);
  virtual int ShowModal(void);

  inline wxPrintDialogData& GetPrintDialogData(void) { return m_printDialogData; }
  inline wxPrintData& GetPrintData(void) { return m_printDialogData.GetPrintData(); }
  virtual wxDC *GetPrintDC(void);

private:
  wxPrintDialogData m_printDialogData;
  wxDC*             m_printerDC;
  bool              m_destroyDC;
  wxWindow*         m_dialogParent;
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
