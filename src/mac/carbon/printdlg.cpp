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
#include "wx/mac/printdlg.h"
#include "wx/dcprint.h"

// Use generic page setup dialog: use your own native one if one exists.

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)
#endif

wxPrintDialog::wxPrintDialog():
 wxDialog()
{
    m_dialogParent = NULL;
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintData* data):
 wxDialog()
{
    Create(p, data);
}

bool wxPrintDialog::Create(wxWindow *p, wxPrintData* data)
{
    m_dialogParent = p;

    if ( data )
        m_printData = *data;

    return TRUE;
}

wxPrintDialog::~wxPrintDialog()
{
}

int wxPrintDialog::ShowModal()
{
	int result = wxID_CANCEL ;
	OSErr err ;
	wxString message ;
	::PrOpen() ;
	err = PrError() ;
	
	if ( !err )
	{
		m_printData.ConvertToNative() ;
		if  ( m_printData.macPrintInfo && ::PrJobDialog( m_printData.macPrintInfo ) )
		{
			m_printData.ConvertFromNative() ;
			result = wxID_OK ;
		}

	}
	else
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message  , "", wxICON_HAND | wxOK) ;
	}
	::PrClose() ;

	return result ;
}

wxDC *wxPrintDialog::GetPrintDC()
{
    return new wxPrinterDC( m_printData ) ;
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
	int result = wxID_CANCEL ;
	OSErr err ;
	wxString message ;
	::PrOpen() ;
	err = PrError() ;
	
	if ( !err )
	{
		m_pageSetupData.ConvertToNative() ;
		if  ( m_pageSetupData.m_macPageSetupInfo && ::PrStlDialog( m_pageSetupData.m_macPageSetupInfo ) )
		{
			m_pageSetupData.ConvertFromNative() ;
			result = wxID_OK ;
		}

	}
	else
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
	}
	::PrClose() ;

	return result ;
}

