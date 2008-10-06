/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dcscreen.cpp
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
#include "wx/osx/dcscreen.h"

#include "wx/osx/private.h"
#include "wx/graphics.h"
#include "wx/osx/private/glgrab.h"

IMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWindowDCImpl)

// TODO : for the Screenshot use case, which doesn't work in Quartz
// we should do a GetAsBitmap using something like
// http://www.cocoabuilder.com/archive/message/cocoa/2005/8/13/144256

// Create a DC representing the whole screen
wxScreenDCImpl::wxScreenDCImpl( wxDC *owner ) :
   wxWindowDCImpl( owner )
{
#if wxOSX_USE_COCOA_OR_IPHONE
    m_graphicContext = NULL;
    m_ok = false ;
#else
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
    m_ok = true ;
#endif
}

wxScreenDCImpl::~wxScreenDCImpl()
{
    delete m_graphicContext;
    m_graphicContext = NULL;
#if wxOSX_USE_COCOA_OR_IPHONE
#else
    DisposeWindow((WindowRef) m_overlayWindow );
#endif
}

wxBitmap wxScreenDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
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
}
