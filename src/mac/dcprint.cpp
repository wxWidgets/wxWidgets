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
#include "wx/mac/uma.h"


#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

GrafPtr macPrintFormerPort = NULL ;

wxPrinterDC::wxPrinterDC(const wxPrintData& printdata)
{
#if !TARGET_CARBON
	OSErr err ;
	wxString message ;
	
	m_printData = printdata ;
	m_printData.ConvertToNative() ;
	
	::UMAPrOpen() ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		UMAPrClose() ;
	}
	
	if ( ::PrValidate( m_printData.m_macPrintInfo ) )
	{
		::PrStlDialog( m_printData.m_macPrintInfo ) ;
		// the driver has changed in the mean time, should we pop up a page setup dialog ?
	}
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		UMAPrClose() ;
	}
	::GetPort( &macPrintFormerPort ) ;
	m_macPrintPort = ::PrOpenDoc( m_printData.m_macPrintInfo , NULL , NULL ) ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		UMAPrClose() ;
	}
	// sets current port
	m_macPort = (GrafPtr ) m_macPrintPort ;
	m_ok = TRUE ;
	m_minY = m_minX = 0 ;
	m_maxX = (**m_printData.m_macPrintInfo).rPaper.right - (**m_printData.m_macPrintInfo).rPaper.left ;
	m_maxY = (**m_printData.m_macPrintInfo).rPaper.bottom - (**m_printData.m_macPrintInfo).rPaper.top ;
#else
#pragma warning "TODO:printing support for carbon"
#endif
}

wxPrinterDC::~wxPrinterDC(void)
{
#if !TARGET_CARBON
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
			UMAPrClose() ;
		}
		::UMAPrClose() ;
//	  ::SetPort( macPrintFormerPort ) ;
		::SetPort( LMGetWMgrPort() ) ;
		m_macPortHelper.Clear() ;
	}
#else
#pragma warning "TODO:printing support for carbon"
#endif
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
#if !TARGET_CARBON
	if ( !m_ok )
		return ;

  	m_logicalFunction = wxCOPY;
//  m_textAlignment = wxALIGN_TOP_LEFT;
  	m_backgroundMode = wxTRANSPARENT;
  
  	m_textForegroundColour = *wxBLACK;
  	m_textBackgroundColour = *wxWHITE;
  	m_pen = *wxBLACK_PEN;
  	m_font = *wxNORMAL_FONT;
  	m_brush = *wxTRANSPARENT_BRUSH;
  	m_backgroundBrush = *wxWHITE_BRUSH;
    
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;

		
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
		::UMAPrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
#pragma warning "TODO:printing support for carbon"
#endif
}

void wxPrinterDC::EndPage(void) 
{
#if !TARGET_CARBON
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
		::UMAPrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
#pragma warning "TODO:printing support for carbon"
#endif

}
