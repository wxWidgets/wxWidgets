/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.h
// Purpose:     wxPrintDialog, wxPageSetupDialog classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTDLG_H_
#define _WX_PRINTDLG_H_

#ifdef __GNUG__
#pragma interface "printdlg.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dialog.h"
#include "wx/cmndata.h"

class WXDLLEXPORT wxDC;

// ---------------------------------------------------------------------------
// wxPrinterDialog: the common dialog for printing.
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxPrintDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxPrintDialog)

public:
    wxPrintDialog();
    wxPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
    wxPrintDialog(wxWindow *parent, wxPrintData* data);
    virtual ~wxPrintDialog();

    bool Create(wxWindow *parent, wxPrintDialogData* data = NULL);
    virtual int ShowModal();

    wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
    wxPrintData& GetPrintData() { return m_printDialogData.GetPrintData(); }
    virtual wxDC *GetPrintDC();

private:
    wxPrintDialogData m_printDialogData;
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

    wxPageSetupData& GetPageSetupData() { return m_pageSetupData; }

private:
    wxPageSetupData   m_pageSetupData;
    wxWindow*         m_dialogParent;
};

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
    // _WX_PRINTDLG_H_
