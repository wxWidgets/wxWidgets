/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "printdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Don't use the Windows print dialog if we're in wxUniv mode and using
// the PostScript architecture
#if wxUSE_PRINTING_ARCHITECTURE && (!defined(__WXUNIVERSAL__) || !wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/cmndata.h"
#include "wx/printdlg.h"
#include "wx/dcprint.h"

#include <stdlib.h>

#include "wx/msw/private.h"

#include <commdlg.h>

#ifndef __WIN32__
    #include <print.h>
#endif

// ---------------------------------------------------------------------------
// wxPrintDialog
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxWindowsPrintDialog, wxPrintDialogBase)

wxWindowsPrintDialog::wxWindowsPrintDialog(wxWindow *p, wxPrintDialogData* data)
{
    Create(p, data);
}

wxWindowsPrintDialog::wxWindowsPrintDialog(wxWindow *p, wxPrintData* data)
{
    wxPrintDialogData data2;
    if ( data )
        data2 = *data;

    Create(p, &data2);
}

bool wxWindowsPrintDialog::Create(wxWindow *p, wxPrintDialogData* data)
{
    m_dialogParent = p;
    m_printerDC = NULL;
    m_destroyDC = true;

    if ( data )
        m_printDialogData = *data;

    m_printDialogData.SetOwnerWindow(p);

    return true;
}

wxWindowsPrintDialog::~wxWindowsPrintDialog()
{
    if (m_destroyDC && m_printerDC)
        delete m_printerDC;
}

int wxWindowsPrintDialog::ShowModal()
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

    if ( ret != false && ((PRINTDLG *)m_printDialogData.GetNativeData())->hDC)
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

wxDC *wxWindowsPrintDialog::GetPrintDC()
{
    if (m_printerDC)
    {
        m_destroyDC = false;
        return m_printerDC;
    }
    else
        return (wxDC*) NULL;
}

// ---------------------------------------------------------------------------
// wxPageSetupDialog
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)

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
    return true;
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
    genericPageSetupDialog->Close(true);
    return ret;
#endif
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE
