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

wxWindowDC::wxWindowDC(void)
{
}

wxWindowDC::wxWindowDC(wxWindow *the_canvas)
{
	WindowRef windowref ;
	wxWindow* rootwindow ;
	
	// this time it is really the full window
	
	the_canvas->MacGetPortParams(&m_macLocalOrigin, &m_macClipRect , &windowref , &rootwindow );
	m_macPort = UMAGetWindowPort( windowref ) ;
	MacSetupPort() ;
 	m_ok = TRUE ;
}

wxWindowDC::~wxWindowDC(void)
{
}

/*
 * wxClientDC
 */

wxClientDC::wxClientDC(void)
{
}

wxClientDC::wxClientDC(wxWindow *window)
{
	WindowRef windowref ;
	wxWindow* rootwindow ;
	
	window->MacGetPortClientParams(&m_macLocalOrigin, &m_macClipRect , &windowref , &rootwindow );
	m_macPort = UMAGetWindowPort( windowref ) ;
	MacSetupPort() ;
 	m_ok = TRUE ;
  	SetBackground(wxBrush(window->GetBackgroundColour(), wxSOLID));
	SetFont( window->GetFont() ) ;
}

wxClientDC::~wxClientDC(void)
{
}

/*
 * wxPaintDC
 */

wxPaintDC::wxPaintDC(void)
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
	long x , y ,w , h ;
	window->GetUpdateRegion().GetBox( x , y , w , h ) ;
	SetClippingRegion( x , y , w , h ) ;
  	SetBackground(wxBrush(window->GetBackgroundColour(), wxSOLID));
  	SetFont(window->GetFont() ) ;
}

wxPaintDC::~wxPaintDC()
{
}
