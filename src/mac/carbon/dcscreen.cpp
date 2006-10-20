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
	CGrafPtr grafptr = CreateNewPort() ;
	m_displayId = (UInt32) grafptr;
	CGContextRef cg = NULL;
	OSStatus status = QDBeginCGContext( grafptr , &cg );

	CGRect bounds ;
    bounds = CGDisplayBounds(CGMainDisplayID());

    SInt16 height;
    GetThemeMenuBarHeight( &height );
    m_minY = height;
	m_minX = 0;
    m_maxX = bounds.size.width;
    m_maxY = bounds.size.height - height;
	CGContextTranslateCTM( cg , 0 , bounds.size.height );
	CGContextScaleCTM( cg , 1 , -1 );

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
	CGrafPtr grafptr = (CGrafPtr) m_displayId;
	m_displayId = (UInt32) grafptr;
	CGContextRef cg = (CGContextRef) m_graphicContext->GetNativeContext();
	QDEndCGContext(grafptr, &cg );

    delete m_graphicContext;
    m_graphicContext = NULL;
#else
    if ( m_macPort )
        DisposePort( (CGrafPtr) m_macPort ) ;
#endif
}
