/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.h
// Purpose:     wxPrintDialog, wxPageSetupDialog classes.
//              Use generic, PostScript version if no
//              platform-specific implementation.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTDLG_H_
#define _WX_PRINTDLG_H_

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
  wxPrintDialog();
    wxPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
  wxPrintDialog(wxWindow *parent, wxPrintData* data );
  virtual ~wxPrintDialog();

    bool Create(wxWindow *parent, wxPrintDialogData* data = NULL);
  virtual int ShowModal();

  wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
  wxPrintData& GetPrintData() { return m_printDialogData.GetPrintData(); }
  virtual wxDC *GetPrintDC();

 private:
  wxPrintDialogData   m_printDialogData;
  wxDC*             m_printerDC;
  bool              m_destroyDC;
  wxWindow*         m_dialogParent;
};

class WXDLLEXPORT wxPageSetupDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupDialog)

 public:
  wxPageSetupDialog();
  wxPageSetupDialog(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual ~wxPageSetupDialog();

  bool Create(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual int ShowModal();

  inline wxPageSetupData& GetPageSetupData() { return m_pageSetupData; }
 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
};

#endif
    // _WX_PRINTDLG_H_
