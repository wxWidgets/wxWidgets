/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printdlg.h"
#endif

#include "wx/object.h"
#include "wx/printdlg.h"
#include "wx/dcprint.h"
#include "wx/msgdlg.h"
#include "wx/mac/private/print.h"

// Use generic page setup dialog: use your own native one if one exists.

IMPLEMENT_DYNAMIC_CLASS(wxMacPrintDialog, wxPrintDialogBase)
IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)

wxMacPrintDialog::wxMacPrintDialog()
{
    m_dialogParent = NULL;
    m_printerDC = NULL;
    m_destroyDC = TRUE;
}

wxMacPrintDialog::wxMacPrintDialog(wxWindow *p, wxPrintDialogData* data)
{
    Create(p, data);
}

wxMacPrintDialog::wxMacPrintDialog(wxWindow *p, wxPrintData* data)
{
    wxPrintDialogData data2;
    if ( data )
        data2 = *data;
    
    Create(p, &data2);
}

bool wxMacPrintDialog::Create(wxWindow *p, wxPrintDialogData* data)
{
    m_dialogParent = p;
    m_printerDC = NULL;
    m_destroyDC = TRUE;
    
    if ( data )
        m_printDialogData = *data;
    
    return TRUE;
}

wxMacPrintDialog::~wxMacPrintDialog()
{
    if (m_destroyDC && m_printerDC) {
        delete m_printerDC;
        m_printerDC = NULL;
    }
}

int wxMacPrintDialog::ShowModal()
{
    m_printDialogData.ConvertToNative() ;
    int result = m_printDialogData.GetPrintData().m_nativePrintData->ShowPrintDialog() ;
    if ( result == wxID_OK )
        m_printDialogData.ConvertFromNative() ;
    
    return result ;
}

wxDC *wxMacPrintDialog::GetPrintDC()
{
    return new wxPrinterDC( m_printDialogData.GetPrintData() ) ;
}

/*
* wxPageSetupDialog
*/

wxPageSetupDialog::wxPageSetupDialog():
wxDialog()
{
    m_dialogParent = NULL;
}

wxPageSetupDialog::wxPageSetupDialog(wxWindow *p, wxPageSetupData *data):
wxDialog()
{
    Create(p, data);
}

bool wxPageSetupDialog::Create(wxWindow *p, wxPageSetupData *data)
{
    m_dialogParent = p;
    
    if (data)
        m_pageSetupData = (*data);
    
    return TRUE;
}

wxPageSetupDialog::~wxPageSetupDialog()
{
}

int wxPageSetupDialog::ShowModal()
{
    m_pageSetupData.ConvertToNative() ;
    int result = m_pageSetupData.GetPrintData().m_nativePrintData->ShowPageSetupDialog() ;
    if (result == wxID_OK )
        m_pageSetupData.ConvertFromNative() ;
        
    return result ;
}

