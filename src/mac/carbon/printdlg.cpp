/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printdlg.h"
#endif

#include "wx/object.h"
#include "wx/printdlg.h"
#include "wx/dcprint.h"
#include "wx/msgdlg.h"
#include "wx/mac/uma.h"
#ifndef __DARWIN__
    #include "Printing.h"
#endif

#if defined(TARGET_CARBON) && !defined(__DARWIN__)
#  if PM_USE_SESSION_APIS
#    include <PMCore.h>
#  endif
#  include <PMApplication.h>
#endif

// Use generic page setup dialog: use your own native one if one exists.

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)
#endif

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

    return TRUE;
}

wxPrintDialog::~wxPrintDialog()
{
    if (m_destroyDC && m_printerDC) {
        delete m_printerDC;
        m_printerDC = NULL;
    }
}

int wxPrintDialog::ShowModal()
{
	int result = wxID_CANCEL ;
	OSErr err ;
	wxString message ;

#if !TARGET_CARBON	
	err = ::UMAPrOpen(NULL) ;
	if ( err == noErr )
	{
		m_printDialogData.ConvertToNative() ;
		if  ( ::PrJobDialog( (THPrint) m_printDialogData.GetPrintData().m_macPrintSettings ) )
		{
			m_printDialogData.ConvertFromNative() ;
			result = wxID_OK ;
		}

	}
	else
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message  , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
	}
	::UMAPrClose(NULL) ;
#else
  #if PM_USE_SESSION_APIS
    Boolean        accepted;
    
    {
        m_printDialogData.ConvertToNative() ;
        
        //  Set up a valid PageFormat object.
        if (m_printDialogData.GetPrintData().m_macPageFormat == kPMNoPageFormat)
        {
            err = PMCreatePageFormat((PMPageFormat *)&m_printDialogData.GetPrintData().m_macPageFormat);
            
            //  Note that PMPageFormat is not session-specific, but calling
            //  PMSessionDefaultPageFormat assigns values specific to the printer
            //  associated with the current printing session.
            if ((err == noErr) &&
                (m_printDialogData.GetPrintData().m_macPageFormat != kPMNoPageFormat))
            {
                err = PMSessionDefaultPageFormat((PMPrintSession)m_printDialogData.GetPrintData().m_macPrintSession,
                                                 (PMPageFormat)m_printDialogData.GetPrintData().m_macPageFormat);
            }
        }
        else
        {
            err = PMSessionValidatePageFormat((PMPrintSession)m_printDialogData.GetPrintData().m_macPrintSession,
                                              (PMPageFormat)m_printDialogData.GetPrintData().m_macPageFormat,
                                              kPMDontWantBoolean);
        }
	
        //  Set up a valid PrintSettings object.
        if (m_printDialogData.GetPrintData().m_macPrintSettings == kPMNoPrintSettings)
        {
            err = PMCreatePrintSettings((PMPrintSettings *)&m_printDialogData.GetPrintData().m_macPrintSettings);
            
            //  Note that PMPrintSettings is not session-specific, but calling
            //  PMSessionDefaultPrintSettings assigns values specific to the printer
            //  associated with the current printing session.
            if ((err == noErr) &&
                (m_printDialogData.GetPrintData().m_macPrintSettings != kPMNoPrintSettings))
            {
                err = PMSessionDefaultPrintSettings((PMPrintSession)m_printDialogData.GetPrintData().m_macPrintSession,
                                                    (PMPrintSettings)m_printDialogData.GetPrintData().m_macPrintSettings);
            }
        }
        else
        {
            err = PMSessionValidatePrintSettings((PMPrintSession)m_printDialogData.GetPrintData().m_macPrintSession,
                                                 (PMPrintSettings)m_printDialogData.GetPrintData().m_macPrintSettings,
                                                 kPMDontWantBoolean);
        }
        //  Set a valid page range before displaying the Print dialog
        if (err == noErr)
        {
            //    err = PMSetPageRange(m_printDialogData.GetPrintData().m_macPrintSettings,
            //              minPage, maxPage);
        }
        
        //  Display the Print dialog.
        if (err == noErr)
        {
            err = PMSessionPrintDialog((PMPrintSession)m_printDialogData.GetPrintData().m_macPrintSession,
                                       (PMPrintSettings)m_printDialogData.GetPrintData().m_macPrintSettings,
                                       (PMPageFormat)m_printDialogData.GetPrintData().m_macPageFormat,
                                       &accepted);
            if ((err == noErr) && !accepted)
            {
                err = kPMCancel; // user clicked Cancel button
            }
        }
        if  ( err == noErr )
        {
            m_printDialogData.ConvertFromNative() ;
            result = wxID_OK ;
        }
    }
    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( "Print Error %d", err ) ;
        wxMessageDialog dialog( NULL , message  , "", wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }
  #else
    #pragma warning "TODO: Printing for carbon without session apis"
  #endif
#endif
	return result ;
}

wxDC *wxPrintDialog::GetPrintDC()
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
	int      result = wxID_CANCEL ;
	OSErr    err ;
	wxString message ;

#if !TARGET_CARBON
	err = ::UMAPrOpen(NULL) ;
	if ( err == noErr )
	{
		m_pageSetupData.ConvertToNative() ;
		if  ( ::PrStlDialog(  (THPrint) m_pageSetupData.GetPrintData().m_macPrintSettings ) )
		{
			m_pageSetupData.ConvertFromNative() ;
			result = wxID_OK ;
		}

	}
	else
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
	}
	::UMAPrClose(NULL) ;
#else
  #if PM_USE_SESSION_APIS
    Boolean        accepted;
    
    {
        m_pageSetupData.ConvertToNative() ;
        
        //  Set up a valid PageFormat object.
        if (m_pageSetupData.GetPrintData().m_macPageFormat == kPMNoPageFormat)
        {
            err = PMCreatePageFormat((PMPageFormat *)&m_pageSetupData.GetPrintData().m_macPageFormat);
            
            //  Note that PMPageFormat is not session-specific, but calling
            //  PMSessionDefaultPageFormat assigns values specific to the printer
            //  associated with the current printing session.
            if ((err == noErr) &&
                (m_pageSetupData.GetPrintData().m_macPageFormat != kPMNoPageFormat))
            {
                err = PMSessionDefaultPageFormat((PMPrintSession)m_pageSetupData.GetPrintData().m_macPrintSession,
                                                 (PMPageFormat)m_pageSetupData.GetPrintData().m_macPageFormat);
            }
        }
        else
        {
            err = PMSessionValidatePageFormat((PMPrintSession)m_pageSetupData.GetPrintData().m_macPrintSession,
                                              (PMPageFormat)m_pageSetupData.GetPrintData().m_macPageFormat,
                                              kPMDontWantBoolean);
        }
        
        //  Display the Page Setup dialog.
        if (err == noErr)
        {
            err = PMSessionPageSetupDialog((PMPrintSession)m_pageSetupData.GetPrintData().m_macPrintSession,
                                           (PMPageFormat)m_pageSetupData.GetPrintData().m_macPageFormat,
                                           &accepted);
            if ((err == noErr) && !accepted)
            {
                err = kPMCancel; // user clicked Cancel button
            }
        }   
        
        //  If the user did not cancel, flatten and save the PageFormat object
        //  with our document.
        if (err == noErr) {
            //    err = FlattenAndSavePageFormat(m_pageSetupData.GetPrintData().m_macPageFormat);
            m_pageSetupData.ConvertFromNative() ;
            result = wxID_OK ;
        }
    }
    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( "Print Error %d", err ) ;
        wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }
  #else
    #pragma warning "TODO: Printing for carbon without session apis"
  #endif
#endif
	return result ;
}

