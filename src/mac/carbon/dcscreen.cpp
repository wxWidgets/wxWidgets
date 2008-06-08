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
#include "wx/mac/common/glgrab.h"
#include "wx/graphics.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
#if wxMAC_USE_CORE_GRAPHICS
    CGRect cgbounds ;
    cgbounds = CGDisplayBounds(CGMainDisplayID());
    Rect bounds;
    bounds.top = (short)cgbounds.origin.y;
    bounds.left = (short)cgbounds.origin.x;
    bounds.bottom = bounds.top + (short)cgbounds.size.height;
    bounds.right = bounds.left  + (short)cgbounds.size.width;
    WindowAttributes overlayAttributes  = kWindowIgnoreClicksAttribute;
    CreateNewWindow( kOverlayWindowClass, overlayAttributes, &bounds, (WindowRef*) &m_overlayWindow );
    ShowWindow((WindowRef)m_overlayWindow);
    SetGraphicsContext( wxGraphicsContext::CreateFromNativeWindow( m_overlayWindow ) );
    m_width = (wxCoord)cgbounds.size.width;
    m_height = (wxCoord)cgbounds.size.height;
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
    DisposeWindow((WindowRef) m_overlayWindow );
#else
    if ( m_macPort )
        DisposePort( (CGrafPtr) m_macPort ) ;
#endif
}

wxBitmap wxScreenDC::DoGetAsBitmap(const wxRect *subrect) const
{
#if wxMAC_USE_CORE_GRAPHICS
    CGRect srcRect = CGRectMake(0, 0, m_width, m_height);
    if (subrect)
    {
        srcRect.origin.x = subrect->GetX();
        srcRect.origin.y = subrect->GetY();
        srcRect.size.width = subrect->GetWidth();
        srcRect.size.height = subrect->GetHeight();
    }
    
    wxBitmap bmp = wxBitmap(srcRect.size.width, srcRect.size.height, 32);
    
    CGContextRef context = (CGContextRef)bmp.GetHBITMAP();
    
    CGContextSaveGState(context);
    
    CGContextTranslateCTM( context, 0,  m_height );
    CGContextScaleCTM( context, 1, -1 );
    
    if ( subrect )
        srcRect = CGRectOffset( srcRect, -subrect->x, -subrect->y ) ;
    
    CGImageRef image = grabViaOpenGL(kCGNullDirectDisplay, srcRect);
    
    wxASSERT_MSG(image, wxT("wxScreenDC::GetAsBitmap - unable to get screenshot."));
    
    CGContextDrawImage(context, srcRect, image);
    
    CGContextRestoreGState(context);

    return bmp;
#else
    return wxNullBitmap;
#endif    
}
