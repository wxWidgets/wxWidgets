/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/osx/dcscreen.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"
#include "wx/graphics.h"

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWindowDCImpl);

// TODO : for the Screenshot use case, which doesn't work in Quartz
// we should do a GetAsBitmap using something like
// http://www.cocoabuilder.com/archive/message/cocoa/2005/8/13/144256

// Create a DC representing the whole screen
wxScreenDCImpl::wxScreenDCImpl( wxDC *owner ) :
   wxWindowDCImpl( owner )
{
#if !wxOSX_USE_IPHONE
    CGRect cgbounds ;
    cgbounds = CGDisplayBounds(CGMainDisplayID());
    m_width = (wxCoord)cgbounds.size.width;
    m_height = (wxCoord)cgbounds.size.height;
    SetGraphicsContext( wxGraphicsContext::Create() );
    m_ok = true ;
#endif
    m_contentScaleFactor = wxOSXGetMainScreenContentScaleFactor();
}

wxScreenDCImpl::~wxScreenDCImpl()
{
    wxDELETE(m_graphicContext);
}

#if wxOSX_USE_IPHONE
// Apple has allowed usage of this API as of 15th Dec 2009w
extern CGImageRef UIGetScreenImage();
#endif

// TODO Switch to CGWindowListCreateImage for 10.5 and above

wxBitmap wxScreenDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    wxRect rect = subrect ? *subrect : wxRect(0, 0, m_width, m_height);

    wxBitmap bmp(rect.GetSize(), 32);

#if !wxOSX_USE_IPHONE
    CGRect srcRect = CGRectMake(rect.x, rect.y, rect.width, rect.height);

    CGContextRef context = (CGContextRef)bmp.GetHBITMAP();

    CGContextSaveGState(context);

    CGContextTranslateCTM( context, 0,  m_height );
    CGContextScaleCTM( context, 1, -1 );

    if ( subrect )
        srcRect = CGRectOffset( srcRect, -subrect->x, -subrect->y ) ;

    CGImageRef image = nullptr;

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 140000
    if ( WX_IS_MACOS_AVAILABLE(14, 4) ) // errors on lower versions of macOS 14
    {
        // TODO add ScreenKit implementation
    }
    else
#endif // macOS 10.14+
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 150000
        image = CGDisplayCreateImage(kCGDirectMainDisplay);
#endif
    }

    if ( image != nullptr )
    {
        CGContextDrawImage(context, srcRect, image);
        CGImageRelease(image);
    }

    CGContextRestoreGState(context);
#else
    // TODO implement using UIGetScreenImage, CGImageCreateWithImageInRect, CGContextDrawImage
#endif
    return bmp;
}
