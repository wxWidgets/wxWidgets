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
#include "wx/msgdlg.h"
#include <math.h>
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

GrafPtr macPrintFormerPort = NULL ;

wxPrinterDC::wxPrinterDC(const wxPrintData& printdata)
{
	OSStatus err ;
	wxString message ;
	
	m_printData = printdata ;
	m_printData.ConvertToNative() ;

#if TARGET_CARBON && PM_USE_SESSION_APIS
	err = UMAPrOpen(&m_macPrintSession) ;
	if ( err != noErr || m_macPrintSession == kPMNoData )
#else
	err = UMAPrOpen() ;
	if ( err != noErr )
#endif
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
#if TARGET_CARBON && PM_USE_SESSION_APIS
		UMAPrClose(&m_macPrintSession) ;
#else
		UMAPrClose() ;
#endif
		m_ok = FALSE;
		return;
	}
	
#if !TARGET_CARBON
	if ( ::PrValidate( (THPrint) m_printData.m_macPrintInfo ) )
	{
		::PrStlDialog( (THPrint) m_printData.m_macPrintInfo ) ;
		// the driver has changed in the mean time, should we pop up a page setup dialog ?
	}
	err = PrError() ;
	if ( err != noErr )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		UMAPrClose() ;
		m_ok = FALSE;
		return;
	}
	::GetPort( &macPrintFormerPort ) ;
	m_macPrintPort = ::PrOpenDoc( (THPrint) m_printData.m_macPrintInfo , NULL , NULL ) ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		UMAPrClose() ;
		m_ok = FALSE;
		return;
	}
	// sets current port
	m_macPort = (GrafPtr ) m_macPrintPort ;
#else
  #if PM_USE_SESSION_APIS
    err = PMSessionBeginDocument(m_macPrintSession,
              m_printData.m_macPrintSettings, 
    	      m_printData.m_macPageFormat);
    if ( err != noErr )
  #else
	m_macPrintPort = kPMNoReference ;
    err = PMBeginDocument(
    	      m_printData.m_macPrintSettings, 
    	      m_printData.m_macPageFormat, 
    	      &m_macPrintPort);
    if ( err != noErr || m_macPrintPort == kPMNoReference )
  #endif
    {
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if TARGET_CARBON && PM_USE_SESSION_APIS
		UMAPrClose(&m_macPrintSession) ;
  #else
		UMAPrClose() ;
  #endif
		m_ok = FALSE;
		return;
    }
	// sets current port
	::GetPort( &m_macPort ) ;
#endif
	m_ok = TRUE ;
	m_minY = m_minX = 0 ;
#if TARGET_CARBON
	PMRect rPaper;
	
	err = PMGetAdjustedPaperRect(m_printData.m_macPageFormat, &rPaper);
    if ( err != noErr )
    {
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if TARGET_CARBON && PM_USE_SESSION_APIS
		UMAPrClose(&m_macPrintSession) ;
  #else
		UMAPrClose() ;
  #endif
		m_ok = FALSE;
		return;
    }
	m_maxX = rPaper.right - rPaper.left ;
	m_maxY = rPaper.bottom - rPaper.top ;
#else
	m_maxX = (**(THPrint)m_printData.m_macPrintInfo).rPaper.right - (**(THPrint)m_printData.m_macPrintInfo).rPaper.left ;
	m_maxY = (**(THPrint)m_printData.m_macPrintInfo).rPaper.bottom - (**(THPrint)m_printData.m_macPrintInfo).rPaper.top ;
#endif
}

wxPrinterDC::~wxPrinterDC(void)
{
	OSStatus err ;
	wxString message ;
#if !TARGET_CARBON
	if ( m_ok )
	{
		::PrCloseDoc( (TPPrPort) m_macPrintPort  ) ;
		err = PrError() ;
		
		if ( err == noErr )
		{
			if ( (**(THPrint)m_printData.m_macPrintInfo).prJob.bJDocLoop == bSpoolLoop )
			{
				TPrStatus status ;
				::PrPicFile( (THPrint) m_printData.m_macPrintInfo , NULL , NULL , NULL , &status ) ;
			}
		}
		else
		{
			message.Printf( "Print Error %d", err ) ;
			wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		    dialog.ShowModal();
		}
		::UMAPrClose() ;
//	  ::SetPort( macPrintFormerPort ) ;
		::SetPort( LMGetWMgrPort() ) ;
	}
#else
	if ( m_ok ) 
	{
  #if PM_USE_SESSION_APIS
	    err = PMSessionEndDocument(m_macPrintSession);
  #else
	    err = PMEndDocument(m_macPrintPort);
  #endif
     	if ( err != noErr )
     	{
			message.Printf( "Print Error %d", err ) ;
			wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		    dialog.ShowModal();
     	}
  #if TARGET_CARBON && PM_USE_SESSION_APIS
	    UMAPrClose(&m_macPrintSession) ;
  #else
	    UMAPrClose() ;
  #endif
	}
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

		
	OSStatus err ;
	wxString message ;

#if !TARGET_CARBON
  	PrOpenPage( (TPPrPort) m_macPrintPort , NULL ) ;
  	m_macLocalOrigin.x =  (**(THPrint)m_printData.m_macPrintInfo).rPaper.left ;
  	m_macLocalOrigin.y =  (**(THPrint)m_printData.m_macPrintInfo).rPaper.top ;
  	
	Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
	::ClipRect( &clip ) ;
	err = PrError() ;
	if ( err != noErr )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
   		::PrClosePage( (TPPrPort) m_macPrintPort ) ;
		::PrCloseDoc( (TPPrPort) m_macPrintPort ) ;
		::UMAPrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
  #if PM_USE_SESSION_APIS
	err = PMSessionBeginPage(m_macPrintSession,
				 m_printData.m_macPageFormat,
				 nil);
  #else
	err = PMBeginPage(m_macPrintPort, nil);
  #endif
	if ( err != noErr )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if PM_USE_SESSION_APIS
   		PMSessionEndPage(m_macPrintSession);
		PMSessionEndDocument(m_macPrintSession);
		UMAPrClose(&m_macPrintSession) ;
  #else
   		PMEndPage(m_macPrintPort);
		PMEndDocument(m_macPrintPort);
		UMAPrClose() ;
  #endif
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#endif
}

void wxPrinterDC::EndPage(void) 
{
	if ( !m_ok )
		return ;

	OSStatus err ;
	wxString message ;

#if !TARGET_CARBON
   	PrClosePage(  (TPPrPort) m_macPort ) ;
	err = PrError() ;
	if ( err != noErr )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		::PrCloseDoc( (TPPrPort) m_macPrintPort  ) ;
		::UMAPrClose() ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
  #if PM_USE_SESSION_APIS
	err = PMSessionEndPage(m_macPrintSession);
  #else
	err = PMEndPage(m_macPrintPort);
  #endif
	if ( err != noErr )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if PM_USE_SESSION_APIS
		PMSessionEndDocument(m_macPrintSession);
		UMAPrClose(&m_macPrintSession) ;
  #else
		PMEndDocument(m_macPrintPort);
		UMAPrClose() ;
  #endif
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#endif

}
