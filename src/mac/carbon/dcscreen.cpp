/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
#if wxMAC_USE_CORE_GRAPHICS
	CGDirectDisplayID display = CGMainDisplayID();
	m_displayId = (UInt32) display;
	CGError err = CGDisplayCaptureWithOptions(display,kCGCaptureNoFill);
	wxASSERT( err == kCGErrorSuccess ); 
	CGContextRef cg = CGDisplayGetDrawingContext(display);
	CGRect bounds ;
    bounds = CGDisplayBounds(display);
	/*
    m_macLocalOrigin.x = 0;
    m_macLocalOrigin.y = 0;
	*/ // TODO
    SInt16 height;
    GetThemeMenuBarHeight( &height );
    m_minY = height;
	m_minX = 0;
    m_maxX = bounds.size.width;
    m_maxY = bounds.size.height - height;
    SetGraphicsContext( wxGraphicsContext::CreateFromNative( cg ) );
	m_width = bounds.size.width;
	m_height = bounds.size.height - height;
#else
    m_macPort = CreateNewPort() ;
    GrafPtr port ;
    GetPort( &port ) ;
    SetPort( (GrafPtr) m_macPort ) ;
    Point pt = { 0,0 } ;
    LocalToGlobal( &pt ) ;
    SetPort( port ) ;
    m_macLocalOrigin.x = -pt.h ;
    m_macLocalOrigin.y = -pt.v ;

    BitMap screenBits;
    GetQDGlobalsScreenBits( &screenBits );
    m_minX = screenBits.bounds.left ;

    SInt16 height ;
    GetThemeMenuBarHeight( &height ) ;
    m_minY = screenBits.bounds.top + height ;

    m_maxX = screenBits.bounds.right  ;
    m_maxY = screenBits.bounds.bottom ;

    MacSetRectRgn( (RgnHandle) m_macBoundaryClipRgn , m_minX , m_minY , m_maxX , m_maxY ) ;
    OffsetRgn( (RgnHandle) m_macBoundaryClipRgn , m_macLocalOrigin.x , m_macLocalOrigin.y ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
#endif
	m_ok = true ;
}

wxScreenDC::~wxScreenDC()
{
#if wxMAC_USE_CORE_GRAPHICS
    delete m_graphicContext;
    m_graphicContext = NULL;
	CGDirectDisplayID display = (CGDirectDisplayID) m_displayId;
	CGDisplayRelease( display );
#else
    if ( m_macPort )
        DisposePort( (CGrafPtr) m_macPort ) ;
#endif
}
