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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "printdlg.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/object.h"
#include "wx/printdlg.h"
#include "wx/mac/printdlg.h"
#include "wx/dcprint.h"
#include "wx/msgdlg.h"
#include "wx/mac/private/print.h"

// Use generic page setup dialog: use your own native one if one exists.

IMPLEMENT_DYNAMIC_CLASS(wxMacPrintDialog, wxPrintDialogBase)

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
    m_printDialogData.GetPrintData().ConvertToNative();
    ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->TransferFrom( &m_printDialogData ) ;
    
    int result = wxID_CANCEL ;
    OSErr err = noErr ;
    wxString message ;
    
    Boolean        accepted;
    
    err = PMSessionPrintDialog( ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPrintSession,
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPrintSettings,
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPageFormat,
        &accepted);
    if ((err == noErr) && !accepted)
    {
        err = kPMCancel; // user clicked Cancel button
    }

    if  ( err == noErr )
    {
        result = wxID_OK ;
    }

    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message  , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }

    if ( result == wxID_OK )
    {
        m_printDialogData.GetPrintData().ConvertFromNative();
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->TransferTo( &m_printDialogData ) ;
    }
    
    return result ;
}

wxDC *wxMacPrintDialog::GetPrintDC()
{
    return new wxPrinterDC( m_printDialogData.GetPrintData() ) ;
}

/*
* wxMacPageSetupDialog
*/

IMPLEMENT_CLASS(wxMacPageSetupDialog, wxPageSetupDialogBase)

wxMacPageSetupDialog::wxMacPageSetupDialog(wxWindow *p, wxPageSetupDialogData *data) :
wxPageSetupDialogBase()
{
    Create(p, data);
}

bool wxMacPageSetupDialog::Create(wxWindow *p, wxPageSetupDialogData *data)
{
    m_dialogParent = p;
    
    if (data)
        m_pageSetupData = (*data);
    
    return TRUE;
}

wxMacPageSetupDialog::~wxMacPageSetupDialog()
{
}

wxPageSetupData& wxMacPageSetupDialog::GetPageSetupDialogData()
{ 
    return m_pageSetupData;
}

int wxMacPageSetupDialog::ShowModal()
{
    m_pageSetupData.GetPrintData().ConvertToNative();
    ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->TransferFrom( &m_pageSetupData ) ;

    int      result = wxID_CANCEL ;
    OSErr    err = noErr ;
    wxString message ;
    
    Boolean        accepted;

    err = PMSessionPageSetupDialog( ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->m_macPrintSession,
        ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->m_macPageFormat,
        &accepted);
    if ((err == noErr) && !accepted)
    {
        err = kPMCancel; // user clicked Cancel button
    }

    //  If the user did not cancel, flatten and save the PageFormat object
    //  with our document.
    if (err == noErr) 
    {
        result = wxID_OK ;
    }
    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }

    if (result == wxID_OK )
    {
        m_pageSetupData.GetPrintData().ConvertFromNative();
        m_pageSetupData.SetPaperSize( m_pageSetupData.GetPrintData().GetPaperSize() ) ;
        ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->TransferTo( &m_pageSetupData ) ;
    }   
    return result;
}

#endif

