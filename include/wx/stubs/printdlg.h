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
  wxPrintDialog(wxWindow *parent, wxPrintData* data = NULL);
  ~wxPrintDialog();

  bool Create(wxWindow *parent, wxPrintData* data = NULL);
  virtual int ShowModal();

  inline wxPrintData& GetPrintData() { return m_printData; }
  virtual wxDC *GetPrintDC();

 private:
  wxPrintData       m_printData;
  wxDC*             m_printerDC;
  wxWindow*         m_dialogParent;
};

class WXDLLEXPORT wxPageSetupDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupDialog)

 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
 public:
  wxPageSetupDialog();
  wxPageSetupDialog(wxWindow *parent, wxPageSetupData *data = NULL);
  ~wxPageSetupDialog();

  bool Create(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual int ShowModal();

  inline wxPageSetupData& GetPageSetupData() { return m_pageSetupData; }
};

#endif
    // _WX_PRINTDLG_H_
