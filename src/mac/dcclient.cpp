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
#include "wx/toplevel.h"
#include <math.h>
#include "wx/mac/private.h"

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

#include "wx/mac/uma.h"

wxWindowDC::wxWindowDC() 
{
}

wxWindowDC::wxWindowDC(wxWindow *the_canvas) 
{
	wxTopLevelWindowMac* rootwindow = the_canvas->MacGetTopLevelWindow() ;
	WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
	
	int x , y ;
	x = y = 0 ;
	the_canvas->MacWindowToRootWindow( &x , &y ) ;
	m_macLocalOrigin.x = x ;
	m_macLocalOrigin.y = y ;
	CopyRgn( (RgnHandle) the_canvas->MacGetVisibleRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
	CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
	m_macPort = UMAGetWindowPort( windowref ) ;
	m_minY = m_minX =  0;
	wxSize size = the_canvas->GetSize() ;
	m_maxX = size.x  ;
	m_maxY = size.y ; 

 	m_ok = TRUE ;
  SetBackground(the_canvas->MacGetBackgroundBrush());
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
	wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
        if (!rootwindow)
            return;
	WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
	wxPoint origin = window->GetClientAreaOrigin() ;
	wxSize size = window->GetClientSize() ;
	int x , y ;
	x = origin.x ;
	y = origin.y ;
	window->MacWindowToRootWindow( &x , &y ) ;
	m_macLocalOrigin.x = x ;
	m_macLocalOrigin.y = y ;
	SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , origin.x , origin.y , origin.x + size.x , origin.y + size.y ) ;
	SectRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) window->MacGetVisibleRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , -origin.x , -origin.y ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
	CopyRgn( (RgnHandle) m_macBoundaryClipRgn ,(RgnHandle)  m_macCurrentClipRgn ) ;
	m_macPort = UMAGetWindowPort( windowref ) ;
	m_minY = m_minX =  0;
	m_maxX = size.x  ;
	m_maxY = size.y ; 
 	m_ok = TRUE ;
  SetBackground(window->MacGetBackgroundBrush());
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
	wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
	WindowRef windowref = (WindowRef) rootwindow->MacGetWindowRef() ;
	wxPoint origin = window->GetClientAreaOrigin() ;
	wxSize size = window->GetClientSize() ;
	int x , y ;
	x = origin.x ;
	y = origin.y ;
	window->MacWindowToRootWindow( &x , &y ) ;
	m_macLocalOrigin.x = x ;
	m_macLocalOrigin.y = y ;
	SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , origin.x , origin.y , origin.x + size.x , origin.y + size.y ) ;
	SectRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) window->MacGetVisibleRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , -origin.x , -origin.y ) ;
  SectRgn( (RgnHandle) m_macBoundaryClipRgn  , (RgnHandle) window->GetUpdateRegion().GetWXHRGN() , (RgnHandle) m_macBoundaryClipRgn ) ;
	OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
	CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
	m_macPort = UMAGetWindowPort( windowref ) ;
	m_minY = m_minX =  0;
	m_maxX = size.x  ;
	m_maxY = size.y ; 
 	m_ok = TRUE ;
  SetBackground(window->MacGetBackgroundBrush());
	SetFont( window->GetFont() ) ;
}

wxPaintDC::~wxPaintDC()
{
}
