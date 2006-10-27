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

  virtual wxPageSetupData& GetPageSetupDialogData() { return m_pageSetupData; }
 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
};


/*
* wxMacPageMarginsDialog
* A Mac dialog for setting the page margins separately from page setup since
* (native) wxMacPageSetupDialog doesn't let you set margins.
*/

class WXDLLEXPORT wxMacPageMarginsDialog : public wxDialog
{
public:
    wxMacPageMarginsDialog(wxFrame* parent, wxPageSetupData* data);
    bool TransferToWindow();
    bool TransferDataFromWindow();
    virtual wxPageSetupData& GetPageSetupDialogData() { return *m_pageSetupDialogData; }

private:
    wxPageSetupData* m_pageSetupDialogData;
    
    wxPoint m_MinMarginTopLeft;
    wxPoint m_MinMarginBottomRight;
    wxTextCtrl *m_LeftMargin;
    wxTextCtrl *m_TopMargin;
    wxTextCtrl *m_RightMargin;
    wxTextCtrl *m_BottomMargin;
    
    void GetMinMargins();
    bool CheckValue(wxTextCtrl* textCtrl, int *value, int minValue, const wxString& name);
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacPageMarginsDialog)
};


#endif    // _WX_PRINTDLG_H_
