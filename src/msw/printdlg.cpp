/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "printdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PRINTING_ARCHITECTURE

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/printdlg.h"
#include "wx/dcprint.h"

// Have to emulate page setup dialog for Win16
#if !defined(__WIN95__)
    #include "wx/generic/prntdlgg.h"
#endif

#include <stdlib.h>

#include "wx/msw/private.h"

#include <commdlg.h>

#ifndef __WIN32__
    #include <print.h>
#endif

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
    IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxPrintDialog
// ---------------------------------------------------------------------------

wxPrintDialog::wxPrintDialog()
{
    m_dialogParent = NULL;
    m_printerDC = NULL;
    m_destroyDC = TRUE;
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintDialogData* data)
{
    Create(p, data);
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintData* data)
{
    wxPrintDialogData data2;
    if ( data )
        data2 = *data;

    Create(p, &data2);
}

bool wxPrintDialog::Create(wxWindow *p, wxPrintDialogData* data)
{
    m_dialogParent = p;
    m_printerDC = NULL;
    m_destroyDC = TRUE;

    if ( data )
        m_printDialogData = *data;

    m_printDialogData.SetOwnerWindow(p);

    return TRUE;
}

wxPrintDialog::~wxPrintDialog()
{
    if (m_destroyDC && m_printerDC)
        delete m_printerDC;
}

int wxPrintDialog::ShowModal()
{
    m_printDialogData.ConvertToNative();

    PRINTDLG* p = (PRINTDLG *)m_printDialogData.GetNativeData() ;
    if (m_dialogParent)
        p->hwndOwner = (HWND) m_dialogParent->GetHWND();
    else if (wxTheApp->GetTopWindow())
        p->hwndOwner = (HWND) wxTheApp->GetTopWindow()->GetHWND();
    else
        p->hwndOwner = 0;

    bool ret = (PrintDlg( p ) != 0);

    p->hwndOwner = 0;

    if ( ret != FALSE && ((PRINTDLG *)m_printDialogData.GetNativeData())->hDC)
    {
        wxPrinterDC *pdc = new wxPrinterDC((WXHDC) ((PRINTDLG *)m_printDialogData.GetNativeData())->hDC);
        m_printerDC = pdc;
        m_printDialogData.ConvertFromNative();
        return wxID_OK;
    }
    else
    {
        return wxID_CANCEL;
    }
}

wxDC *wxPrintDialog::GetPrintDC()
{
    if (m_printerDC)
    {
        m_destroyDC = FALSE;
        return m_printerDC;
    }
    else
        return (wxDC*) NULL;
}

// ---------------------------------------------------------------------------
// wxPageSetupDialog
// ---------------------------------------------------------------------------

wxPageSetupDialog::wxPageSetupDialog()
{
    m_dialogParent = NULL;
}

wxPageSetupDialog::wxPageSetupDialog(wxWindow *p, wxPageSetupData *data)
{
    Create(p, data);
}

bool wxPageSetupDialog::Create(wxWindow *p, wxPageSetupData *data)
{
    m_dialogParent = p;

    if (data)
        m_pageSetupData = (*data);

#if defined(__WIN95__)
    m_pageSetupData.SetOwnerWindow(p);
#endif
    return TRUE;
}

wxPageSetupDialog::~wxPageSetupDialog()
{
}

int wxPageSetupDialog::ShowModal()
{
#ifdef __WIN95__
    m_pageSetupData.ConvertToNative();
    PAGESETUPDLG *p = (PAGESETUPDLG *)m_pageSetupData.GetNativeData();
    if (m_dialogParent)
        p->hwndOwner = (HWND) m_dialogParent->GetHWND();
    else if (wxTheApp->GetTopWindow())
        p->hwndOwner = (HWND) wxTheApp->GetTopWindow()->GetHWND();
    else
        p->hwndOwner = 0;
    BOOL retVal = PageSetupDlg( p ) ;
    p->hwndOwner = 0;
    if (retVal)
    {
        m_pageSetupData.ConvertFromNative();
        return wxID_OK;
    }
    else
        return wxID_CANCEL;
#else
    wxGenericPageSetupDialog *genericPageSetupDialog = new wxGenericPageSetupDialog(GetParent(), & m_pageSetupData);
    int ret = genericPageSetupDialog->ShowModal();
    m_pageSetupData = genericPageSetupDialog->GetPageSetupData();
    genericPageSetupDialog->Close(TRUE);
    return ret;
#endif
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE
