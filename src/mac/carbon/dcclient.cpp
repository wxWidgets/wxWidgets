/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/region.h"
#include "wx/window.h"
#include <math.h>

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define RAD2DEG 57.2957795131

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)
#endif

/*
 * wxWindowDC
 */

#include <wx/mac/uma.h>

wxWindowDC::wxWindowDC() 
{
}

wxWindowDC::wxWindowDC(wxWindow *the_canvas) 
{
	WindowRef windowref ;
	wxWindow* rootwindow ;
	
	// this time it is really the full window
	
	the_canvas->MacGetPortParams(&m_macLocalOrigin, &m_macClipRect , &windowref , &rootwindow );
	m_macPort = UMAGetWindowPort( windowref ) ;
	m_minY = m_minX =  0;
	wxSize size = the_canvas->GetSize() ;
	m_maxX = size.x  ;
	m_maxY = size.y ; 

	MacSetupPort() ;
 	m_ok = TRUE ;
}

wxWindowDC::~wxWindowDC()
{
}

/*
 * wxClientDC
 */

wxClientDC::wxClientDC()
{
}

wxClientDC::wxClientDC(wxWindow *window)
{
	WindowRef windowref ;
	wxWindow* rootwindow ;
	
	window->MacGetPortClientParams(&m_macLocalOrigin, &m_macClipRect , &windowref , &rootwindow );
	m_macPort = UMAGetWindowPort( windowref ) ;
	MacSetupPort() ;
	m_minY = m_minX =  0;
	wxSize size = window->GetSize() ;
	m_maxX = size.x  ;
	m_maxY = size.y ; 
 	m_ok = TRUE ;
  	SetBackground(wxBrush(window->GetBackgroundColour(), wxSOLID));
	SetFont( window->GetFont() ) ;
}

wxClientDC::~wxClientDC()
{
}

/*
 * wxPaintDC
 */

wxPaintDC::wxPaintDC()
{
}

wxPaintDC::wxPaintDC(wxWindow *window)
{
	WindowRef windowref ;
	wxWindow* rootwindow ;
	
	window->MacGetPortClientParams(&m_macLocalOrigin, &m_macClipRect , &windowref , &rootwindow );

	m_macPort = UMAGetWindowPort( windowref ) ;
	MacSetupPort() ;
 	m_ok = TRUE ;
	wxCoord x , y ,w , h ;
	window->GetUpdateRegion().GetBox( x , y , w , h ) ;
	m_minY = m_minX =  0;
	wxSize size = window->GetSize() ;
	m_maxX = size.x  ;
	m_maxY = size.y ; 
	SetClippingRegion( x , y , w , h ) ;
  	SetBackground(wxBrush(window->GetBackgroundColour(), wxSOLID));
  	SetFont(window->GetFont() ) ;
}

wxPaintDC::~wxPaintDC()
{
}
