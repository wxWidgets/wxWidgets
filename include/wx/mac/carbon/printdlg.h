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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "printdlg.h"
#endif

#include "wx/dialog.h"
#include "wx/cmndata.h"
#include "wx/printdlg.h"

/*
 * wxPrinterDialog
 * The common dialog for printing.
 */

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxMacPrintDialog: public wxPrintDialogBase
{
public:
    wxMacPrintDialog();
    wxMacPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
    wxMacPrintDialog(wxWindow *parent, wxPrintData* data );
    ~wxMacPrintDialog();

    bool Create(wxWindow *parent, wxPrintDialogData* data = NULL);
    virtual int ShowModal();

    virtual wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
    virtual wxPrintData& GetPrintData() { return m_printDialogData.GetPrintData(); }
    virtual wxDC *GetPrintDC();

private:
    wxPrintDialogData   m_printDialogData;
    wxDC*               m_printerDC;
    bool                m_destroyDC;
    wxWindow*           m_dialogParent;
  
private:
    DECLARE_DYNAMIC_CLASS(wxPrintDialog)
};

class WXDLLEXPORT wxPageSetupDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupDialog)

 public:
  wxPageSetupDialog();
  wxPageSetupDialog(wxWindow *parent, wxPageSetupData *data = NULL);
  ~wxPageSetupDialog();

  bool Create(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual int ShowModal();

  inline wxPageSetupData& GetPageSetupData() { return m_pageSetupData; }
 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
};

#endif
    // _WX_PRINTDLG_H_
