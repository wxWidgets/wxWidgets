/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcprint.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/dcprint.h"
#include "math.h"


IMPLEMENT_CLASS(wxPrinterDC, wxDC)

GrafPtr macPrintFormerPort = NULL ;

wxPrinterDC::wxPrinterDC(const wxPrintData& printdata)
{
	OSErr err ;
	wxString message ;
	
	m_printData = printdata ;
	m_printData.ConvertToNative() ;
	
	::PrOpen() ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		PrClose() ;
	}
	
	if ( ::PrValidate( m_printData.m_macPrintInfo ) )
	{
		// the driver has changed in the mean time, should we pop up a page setup dialog ?
	}
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		PrClose() ;
	}
	::GetPort( &macPrintFormerPort ) ;
	m_macPrintPort = ::PrOpenDoc( m_printData.m_macPrintInfo , NULL , NULL ) ;
	// sets current port
	m_macPort = (GrafPtr ) m_macPrintPort ;
	m_ok = TRUE ;
	m_minY = m_minX = 0 ;
	m_maxX = (**m_printData.m_macPrintInfo).rPaper.right - (**m_printData.m_macPrintInfo).rPaper.left ;
	m_maxY = (**m_printData.m_macPrintInfo).rPaper.bottom - (**m_printData.m_macPrintInfo).rPaper.top ;
}

wxPrinterDC::~wxPrinterDC(void)
{
	if ( m_ok )
	{
		OSErr err ;
		wxString message ;
	
		::PrCloseDoc( m_macPrintPort  ) ;
		err = PrError() ;
		
		if ( !err )
		{
			if ( (**m_printData.m_macPrintInfo).prJob.bJDocLoop == bSpoolLoop )
			{
				TPrStatus status ;
				::PrPicFile( m_printData.m_macPrintInfo , NULL , NULL , NULL , &status ) ;
			}
		}
		else
		{
			message.Printf( "Print Error %d", err ) ;
			wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
			PrClose() ;
		}
		::PrClose() ;
	  ::SetPort( macPrintFormerPort ) ;
	}
}

bool wxPrinterDC::StartDoc( const wxString& WXUNUSED(message) ) 
{
	return m_ok ;
}

void wxPrinterDC::EndDoc(void) 
{
}

void wxPrinterDC::StartPage(void) 
{
	if ( !m_ok )
		return ;
		
	OSErr err ;
	wxString message ;

  PrOpenPage( m_macPrintPort , NULL ) ;
	SetOrigin(  - (**m_printData.m_macPrintInfo).rPaper.left , - (**m_printData.m_macPrintInfo).rPaper.top ) ;
	Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
	::ClipRect( &clip ) ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
   		::PrClosePage(  m_macPrintPort) ;
		::PrCloseDoc( m_macPrintPort  ) ;
		::PrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
}

void wxPrinterDC::EndPage(void) 
{
	if ( !m_ok )
		return ;

	OSErr err ;
	wxString message ;

   	PrClosePage(  (TPrPort*) m_macPort ) ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		::PrCloseDoc( m_macPrintPort  ) ;
		::PrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
}
