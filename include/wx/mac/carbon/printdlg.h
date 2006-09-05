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
#include "wx/printdlg.h"
#include "wx/prntbase.h"
    
/*
 * wxMacPrintDialog
 * The Mac dialog for printing
 */

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxMacPrintDialog: public wxPrintDialogBase
{
public:
    wxMacPrintDialog();
    wxMacPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
    wxMacPrintDialog(wxWindow *parent, wxPrintData* data );
    virtual ~wxMacPrintDialog();

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

/*
 * wxMacPageSetupDialog
 * The Mac page setup dialog
 */

class WXDLLEXPORT wxMacPageSetupDialog: public wxPageSetupDialogBase
{
public:
    wxMacPageSetupDialog(wxWindow *parent, wxPageSetupDialogData *data = NULL);
    virtual ~wxMacPageSetupDialog();

    virtual wxPageSetupData& GetPageSetupDialogData();
    
    bool Create(wxWindow *parent, wxPageSetupDialogData *data = NULL);
    virtual int ShowModal();
  
private:
    wxPageSetupDialogData   m_pageSetupData;
    wxWindow*               m_dialogParent;
  
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacPageSetupDialog)
};

#endif
    // _WX_PRINTDLG_H_
