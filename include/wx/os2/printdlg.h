/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.h
// Purpose:     wxPrintDialog, wxPageSetupDialog classes.
//              Use generic, PostScript version if no
//              platform-specific implementation.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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
  wxPrintDialog();
  wxPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
  wxPrintDialog(wxWindow *parent, wxPrintData* data);
  ~wxPrintDialog();

  bool Create(wxWindow *parent, wxPrintData* data = NULL);
  virtual int ShowModal();

  wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
  inline wxPrintData& GetPrintData() { return m_printDialogData.GetPrintData(); }
  virtual wxDC *GetPrintDC();

 private:
  wxPrintDialogData       m_printDialogData;
  wxDC*                   m_printerDC;
  bool                    m_destroyDC;
  wxWindow*               m_dialogParent;
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
