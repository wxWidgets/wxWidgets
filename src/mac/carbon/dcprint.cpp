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
#ifndef __DARWIN__
#include "Printing.h"
#endif

#if defined(TARGET_CARBON) && !defined(__DARWIN__)
#  if PM_USE_SESSION_APIS
#    include <PMCore.h>
#  endif
#  include <PMApplication.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

GrafPtr macPrintFormerPort = NULL ;

wxPrinterDC::wxPrinterDC(const wxPrintData& printdata)
{
	OSStatus err = noErr ;
	wxString message ;
	
	m_printData = printdata ;
	m_printData.ConvertToNative() ;

#if TARGET_CARBON && PM_USE_SESSION_APIS
	m_macPrintSessionPort = printdata.m_macPrintSession ;
	PMRetain( m_macPrintSessionPort ) ;

	if ( err != noErr || m_macPrintSessionPort == kPMNoData )
#else
	err = UMAPrOpen(NULL) ;
	if ( err != noErr )
#endif
	{
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
#if TARGET_CARBON && PM_USE_SESSION_APIS
		PMRelease( m_macPrintSessionPort ) ;
#else
		UMAPrClose(NULL) ;
#endif
		m_ok = FALSE;
		return;
	}
	
#if !TARGET_CARBON
	if ( ::PrValidate( (THPrint) m_printData.m_macPrintSettings ) )
	{
		// the driver has changed in the mean time, should we pop up a page setup dialog ?
		if ( !::PrStlDialog( (THPrint) m_printData.m_macPrintSettings ) )
		{
      		UMAPrClose(NULL) ;
      		m_ok = FALSE;
      		return;
		}
	}
	err = PrError() ;
	if ( err != noErr )
	{
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		UMAPrClose(NULL) ;
		m_ok = FALSE;
		return;
	}
	::GetPort( &macPrintFormerPort ) ;
#endif
	m_ok = TRUE ;
	m_minY = m_minX = 0 ;
#if TARGET_CARBON
	PMRect rPaper;
	
	err = PMGetAdjustedPaperRect((PMPageFormat)m_printData.m_macPageFormat, &rPaper);
    if ( err != noErr )
    {
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if TARGET_CARBON && PM_USE_SESSION_APIS
		PMRelease(&m_macPrintSessionPort) ;
  #else
		UMAPrClose(NULL) ;
  #endif
		m_ok = FALSE;
		return;
    }
	m_maxX = wxCoord(rPaper.right - rPaper.left) ;
	m_maxY = wxCoord(rPaper.bottom - rPaper.top);
#else
	m_maxX = (**(THPrint)m_printData.m_macPrintSettings).rPaper.right - (**(THPrint)m_printData.m_macPrintSettings).rPaper.left ;
	m_maxY = (**(THPrint)m_printData.m_macPrintSettings).rPaper.bottom - (**(THPrint)m_printData.m_macPrintSettings).rPaper.top ;
#endif
}

wxPrinterDC::~wxPrinterDC(void)
{
//	OSStatus err ;
	wxString message ;
#if !TARGET_CARBON
	if ( m_ok )
	{
		::UMAPrClose(NULL) ;
		::SetPort( LMGetWMgrPort() ) ;
	}
#else
	if ( m_ok ) 
	{
  #if TARGET_CARBON && PM_USE_SESSION_APIS
	    PMRelease(&m_macPrintSessionPort) ;
  #else
	    UMAPrClose(NULL) ;
  #endif
	}
#endif
}

bool wxPrinterDC::StartDoc( const wxString& WXUNUSED(message) ) 
{
	OSStatus err = noErr ;
	wxString message ;
		
#if !TARGET_CARBON
	m_macPrintSessionPort = ::PrOpenDoc( (THPrint) m_printData.m_macPrintSettings , NULL , NULL ) ;
	err = PrError() ;
	if ( err )
	{
		message.Printf( "Print Error %d", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		UMAPrClose(NULL) ;
		m_ok = FALSE;
		return false ;
	}
	// sets current port
	m_macPort = (GrafPtr ) m_macPrintSessionPort ;
#else
  #if PM_USE_SESSION_APIS
    err = PMSessionBeginDocument((PMPrintSession)m_macPrintSessionPort,
              (PMPrintSettings)m_printData.m_macPrintSettings, 
    	      (PMPageFormat)m_printData.m_macPageFormat);
    if ( err != noErr )
  #else
		m_macPrintSessionPort = kPMNoReference ;
    	err = PMBeginDocument(
    	      m_printData.m_macPrintSettings, 
    	      m_printData.m_macPageFormat, 
    	      &m_macPrintSessionPort);
    if ( err != noErr || m_macPrintSessionPort == kPMNoReference )
  #endif
    {
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if TARGET_CARBON && PM_USE_SESSION_APIS
		PMRelease(&m_macPrintSessionPort) ;
  #else
		UMAPrClose(NULL) ;
  #endif
		m_ok = FALSE;
		return m_ok;
    }
	// sets current port
	::GetPort( (GrafPtr *)&m_macPort ) ;
#endif
	m_ok = TRUE ;
	m_minY = m_minX = 0 ;
#if TARGET_CARBON
	PMRect rPaper;
	
	err = PMGetAdjustedPaperRect((PMPageFormat)m_printData.m_macPageFormat, &rPaper);
    if ( err != noErr )
    {
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if TARGET_CARBON && PM_USE_SESSION_APIS
		PMRelease(&m_macPrintSessionPort) ;
  #else
		UMAPrClose(NULL) ;
  #endif
		m_ok = FALSE;
		return m_ok;
    }
	m_maxX = (wxCoord)(rPaper.right - rPaper.left) ;
	m_maxY = (wxCoord)(rPaper.bottom - rPaper.top) ;
#else
	m_maxX = (**(THPrint)m_printData.m_macPrintSettings).rPaper.right - (**(THPrint)m_printData.m_macPrintSettings).rPaper.left ;
	m_maxY = (**(THPrint)m_printData.m_macPrintSettings).rPaper.bottom - (**(THPrint)m_printData.m_macPrintSettings).rPaper.top ;
#endif
	return m_ok ;
}

void wxPrinterDC::EndDoc(void) 
{
	OSStatus err ;
	wxString message ;
#if !TARGET_CARBON
	if ( m_ok )
	{
		::PrCloseDoc( (TPrPort*) m_macPrintSessionPort  ) ;
		err = PrError() ;
		
		if ( !err )
		{
			if ( (** (THPrint) m_printData.m_macPrintSettings).prJob.bJDocLoop == bSpoolLoop )
			{
				TPrStatus status ;
				::PrPicFile(  (THPrint) m_printData.m_macPrintSettings , NULL , NULL , NULL , &status ) ;
			}
		}
		else
		{
			message.Printf( "Print Error %d", err ) ;
			wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
			UMAPrClose(NULL) ;
			m_ok = false ;
		}
	}
#else
	if ( m_ok ) 
	{
  #if PM_USE_SESSION_APIS
	    err = PMSessionEndDocument((PMPrintSession)m_macPrintSessionPort);
  #else
	    err = PMEndDocument(m_macPrintSessionPort);
  #endif
     	if ( err != noErr )
     	{
			message.Printf( "Print Error %ld", err ) ;
			wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		    dialog.ShowModal();
     	}
	}
#endif
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
  	PrOpenPage( (TPPrPort) m_macPrintSessionPort , NULL ) ;
  	m_macLocalOrigin.x =  (**(THPrint)m_printData.m_macPrintSettings).rPaper.left ;
  	m_macLocalOrigin.y =  (**(THPrint)m_printData.m_macPrintSettings).rPaper.top ;
  	
	Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
	::ClipRect( &clip ) ;
	err = PrError() ;
	if ( err != noErr )
	{
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
   		::PrClosePage( (TPPrPort) m_macPrintSessionPort ) ;
		::PrCloseDoc( (TPPrPort) m_macPrintSessionPort ) ;
		::UMAPrClose(NULL) ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
  #if PM_USE_SESSION_APIS
	err = PMSessionBeginPage((PMPrintSession)m_macPrintSessionPort,
				 (PMPageFormat)m_printData.m_macPageFormat,
				 nil);
  #else
	err = PMBeginPage(m_macPrintSessionPort, nil);
  #endif
	if ( err != noErr )
	{
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if PM_USE_SESSION_APIS
   		PMSessionEndPage((PMPrintSession)m_macPrintSessionPort);
		PMSessionEndDocument((PMPrintSession)m_macPrintSessionPort);
		UMAPrClose(&m_macPrintSessionPort) ;
  #else
   		PMEndPage(m_macPrintSessionPort);
		PMEndDocument(m_macPrintSessionPort);
		UMAPrClose(NULL) ;
  #endif
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
	PMRect rPaper;
	
	err = PMGetAdjustedPaperRect((PMPageFormat)m_printData.m_macPageFormat, &rPaper);
	if ( !err )
	{
  		m_macLocalOrigin.x = (int) (rPaper.left) ;
  		m_macLocalOrigin.y = (int) (rPaper.top) ;
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
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
		::PrCloseDoc( (TPPrPort) m_macPrintSessionPort  ) ;
		::UMAPrClose(NULL) ;
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#else
  #if PM_USE_SESSION_APIS
	err = PMSessionEndPage((PMPrintSession)m_macPrintSessionPort);
  #else
	err = PMEndPage(m_macPrintSessionPort);
  #endif
	if ( err != noErr )
	{
		message.Printf( "Print Error %ld", err ) ;
		wxMessageDialog dialog( NULL , message , "", wxICON_HAND | wxOK) ;
		dialog.ShowModal();
  #if PM_USE_SESSION_APIS
		PMSessionEndDocument((PMPrintSession)m_macPrintSessionPort);
		UMAPrClose(&m_macPrintSessionPort) ;
  #else
		PMEndDocument(m_macPrintSessionPort);
		UMAPrClose(NULL) ;
  #endif
	   	::SetPort( macPrintFormerPort ) ;
	   	m_ok = FALSE ;
	}
#endif

}
