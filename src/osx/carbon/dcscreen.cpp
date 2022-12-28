/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/osx/dcscreen.h"

#include "wx/osx/private.h"
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
    
    image = CGDisplayCreateImage(kCGDirectMainDisplay);

    wxASSERT_MSG(image, wxT("wxScreenDC::GetAsBitmap - unable to get screenshot."));

    CGContextDrawImage(context, srcRect, image);

    CGImageRelease(image);

    CGContextRestoreGState(context);
#else
    // TODO implement using UIGetScreenImage, CGImageCreateWithImageInRect, CGContextDrawImage
#endif
    return bmp;
}
