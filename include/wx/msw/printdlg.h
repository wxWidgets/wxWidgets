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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "printdlg.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dialog.h"
#include "wx/cmndata.h"
#include "wx/prntbase.h"
#include "wx/printdlg.h"

class WXDLLEXPORT wxDC;

//----------------------------------------------------------------------------
// wxWindowsPrintNativeData
//----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowsPrintNativeData: public wxPrintNativeDataBase
{
public:
    wxWindowsPrintNativeData();
    virtual ~wxWindowsPrintNativeData();
    
    virtual bool TransferTo( wxPrintData &data );
    virtual bool TransferFrom( const wxPrintData &data );
    
    virtual bool Ok() const;
    
    void* GetDevMode() const { return m_devMode; }
    void SetDevMode(void* data) { m_devMode = data; }
    void* GetDevNames() const { return m_devNames; }
    void SetDevNames(void* data) { m_devNames = data; }
    
private:
    void* m_devMode;
    void* m_devNames;

private:
    DECLARE_DYNAMIC_CLASS(wxWindowsPrintNativeData)
};
    
// ---------------------------------------------------------------------------
// wxPrinterDialog: the common dialog for printing.
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindowsPrintDialog : public wxPrintDialogBase
{
public:
    wxWindowsPrintDialog(wxWindow *parent, wxPrintDialogData* data = NULL);
    wxWindowsPrintDialog(wxWindow *parent, wxPrintData* data);
    virtual ~wxWindowsPrintDialog();

    bool Create(wxWindow *parent, wxPrintDialogData* data = NULL);
    virtual int ShowModal();

    wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
    wxPrintData& GetPrintData() { return m_printDialogData.GetPrintData(); }
    virtual wxDC *GetPrintDC();

    wxPrintDialogData m_printDialogData;
    wxDC*             m_printerDC;
    bool              m_destroyDC;
    wxWindow*         m_dialogParent;

private:
    DECLARE_NO_COPY_CLASS(wxWindowsPrintDialog)
    DECLARE_CLASS(wxWindowsPrintDialog)
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

    DECLARE_NO_COPY_CLASS(wxPageSetupDialog)
};

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
    // _WX_PRINTDLG_H_
