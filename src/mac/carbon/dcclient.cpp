/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dcclient.cpp
// Purpose:     wxClientDCImpl class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
    #include "wx/toplevel.h"
    #include "wx/math.h"
    #include "wx/region.h"
#endif

#include "wx/graphics.h"
#include "wx/rawbmp.h"
#include "wx/mac/private.h"

//-----------------------------------------------------------------------------
// wxWindowDCImpl
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxWindowDCImpl, wxGCDCImpl)

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
   : wxGCDCImpl( owner )
{
    m_release = false;
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *window )
   : wxGCDCImpl( owner )
{
    m_window = window;
    
    wxTopLevelWindowMac* rootwindow = window->MacGetTopLevelWindow() ;
    if (!rootwindow)
        return;

    m_ok = true ;

    m_window->GetSize( &m_width , &m_height);
    CGContextRef cg = (CGContextRef) window->MacGetCGContextRef();
    m_release = false;
    if ( cg == NULL )
    {
        SetGraphicsContext( wxGraphicsContext::Create( window ) ) ;
    }
    else
    {
        CGContextSaveGState( cg );
        m_release = true ;
        // make sure the context is having its origin at the wx-window coordinates of the
        // view (read at the top of window.cpp about the differences)
        if ( window->MacGetLeftBorderSize() != 0 || window->MacGetTopBorderSize() != 0 )
            CGContextTranslateCTM( cg , -window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize() );

        SetGraphicsContext( wxGraphicsContext::CreateFromNative( cg ) );
    }
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;

    SetBackground(wxBrush(window->GetBackgroundColour(),wxSOLID));

    SetFont( window->GetFont() ) ;
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    if ( m_release )
    {
        // this must not necessarily be the current context, we must restore the state of the
        // cg we started with above (before the CGContextTranslateCTM call)
        CGContextRef cg = (CGContextRef) m_window->MacGetCGContextRef();
        CGContextRestoreGState(cg);
    }
}

void wxWindowDCImpl::DoGetSize( int* width, int* height ) const
{
    if ( width )
        *width = m_width;
    if ( height )
        *height = m_height;
}

wxBitmap wxWindowDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    // wxScreenDC is derived from wxWindowDC, so a screen dc will
    // call this method when a Blit is performed with it as a source.
    if (!m_window)
        return wxNullBitmap;

#ifdef __LP64__
    return wxNullBitmap;
#else
    ControlRef handle = (ControlRef) m_window->GetHandle();
    if ( !handle )
        return wxNullBitmap;
    
    HIRect rect;
    CGImageRef image;
    CGContextRef context;
    void* data;

    size_t bytesPerRow;

    HIViewCreateOffscreenImage( handle, 0, &rect, &image);


    int width = subrect != NULL ? subrect->width : (int)rect.size.width;
    int height = subrect !=  NULL ? subrect->height : (int)rect.size.height ;

    bytesPerRow = ( ( width * 8 * 4 + 7 ) / 8 );

    data = calloc( 1, bytesPerRow * height );
    context = CGBitmapContextCreate( data, width, height, 8, bytesPerRow, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedFirst );

    if ( subrect )
        rect = CGRectOffset( rect, -subrect->x, -subrect->y ) ;
    CGContextDrawImage( context, rect, image );

    unsigned char* buffer = (unsigned char*) data;
    wxBitmap bmp = wxBitmap(width, height, 32);
    wxAlphaPixelData pixData(bmp, wxPoint(0,0), wxSize(width, height));

    wxAlphaPixelData::Iterator p(pixData);
    for (int y=0; y<height; y++) {
        wxAlphaPixelData::Iterator rowStart = p;
        for (int x=0; x<width; x++) {
            unsigned char a = buffer[3];
            p.Red()   = a; buffer++;
            p.Green() = a; buffer++;
            p.Blue()  = a; buffer++;
            p.Alpha() = a; buffer++;
            ++p;
        }
        p = rowStart;
        p.OffsetY(pixData, 1);
    }

    return bmp;
#endif
}

/*
 * wxClientDCImpl
 */

IMPLEMENT_ABSTRACT_CLASS(wxClientDCImpl, wxWindowDCImpl)

wxClientDCImpl::wxClientDCImpl( wxDC *owner )
 : wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *window ) :
    wxWindowDCImpl( owner, window )
{
    wxCHECK_RET( window, _T("invalid window in wxClientDCImpl") );
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    SetDeviceOrigin( origin.x, origin.y );
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;
}

wxClientDCImpl::~wxClientDCImpl()
{
}

/*
 * wxPaintDCImpl
 */

IMPLEMENT_ABSTRACT_CLASS(wxPaintDCImpl, wxWindowDCImpl)

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
 : wxWindowDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *window ) :
    wxWindowDCImpl( owner, window )
{
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    SetDeviceOrigin( origin.x, origin.y );
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;
}

wxPaintDCImpl::~wxPaintDCImpl()
{
}
