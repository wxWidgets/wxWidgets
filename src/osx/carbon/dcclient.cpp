/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dcclient.cpp
// Purpose:     wxClientDCImpl class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
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
#include "wx/osx/private.h"
#include "wx/osx/dcclient.h"

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

    m_ok = true ;

    m_window->GetSize( &m_width , &m_height);
    if ( !m_window->IsShownOnScreen() )
        m_width = m_height = 0;

    CGContextRef cg = (CGContextRef) window->MacGetCGContextRef();

    m_release = false;
    if ( cg == NULL )
    {
        SetGraphicsContext( wxGraphicsContext::Create( window ) ) ;
        m_contentScaleFactor = window->GetContentScaleFactor();
        SetDeviceOrigin(-window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize());
    }
    else
    {
        // determine content scale
        CGRect userrect = CGRectMake(0, 0, 10, 10);
        CGRect devicerect;
        devicerect = CGContextConvertRectToDeviceSpace(cg, userrect);
        m_contentScaleFactor = devicerect.size.height / userrect.size.height;

        CGContextSaveGState( cg );
        m_release = true ;
        // make sure the context is having its origin at the wx-window coordinates of the
        // view (read at the top of window.cpp about the differences)
        if ( window->MacGetLeftBorderSize() != 0 || window->MacGetTopBorderSize() != 0 )
            CGContextTranslateCTM( cg , -window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize() );

        wxGraphicsContext* context = wxGraphicsContext::CreateFromNative( cg );
        context->EnableOffset(true);
        SetGraphicsContext( context );
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

#if wxOSX_USE_CARBON
wxBitmap wxWindowDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    // wxScreenDC is derived from wxWindowDC, so a screen dc will
    // call this method when a Blit is performed with it as a source.
    if (!m_window)
        return wxNullBitmap;

    ControlRef handle = (ControlRef) m_window->GetHandle();
    if ( !handle )
        return wxNullBitmap;

    HIRect rect;
    CGImageRef image;
    CGContextRef context;

    HIViewCreateOffscreenImage( handle, 0, &rect, &image);


    int width = subrect != NULL ? subrect->width : (int)rect.size.width;
    int height = subrect !=  NULL ? subrect->height : (int)rect.size.height ;

    wxBitmap bmp = wxBitmap(width, height, 32);

    context = (CGContextRef)bmp.GetHBITMAP();

    CGContextSaveGState(context);

    CGContextTranslateCTM( context, 0,  height );
    CGContextScaleCTM( context, 1, -1 );

    if ( subrect )
        rect = CGRectOffset( rect, -subrect->x, -subrect->y ) ;
    CGContextDrawImage( context, rect, image );

    CGContextRestoreGState(context);
    return bmp;
}
#endif

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
    wxCHECK_RET( window, wxT("invalid window in wxClientDCImpl") );
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    if ( !m_window->IsShownOnScreen() )
        m_width = m_height = 0;
    
    int x0,y0;
    DoGetDeviceOrigin(&x0,&y0);
    SetDeviceOrigin( origin.x + x0, origin.y + y0 );
    
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;
}

wxClientDCImpl::~wxClientDCImpl()
{
    if( GetGraphicsContext() && GetGraphicsContext()->GetNativeContext() && !m_release )
        Flush();
}

/*
 * wxPaintDCImpl
 */

IMPLEMENT_ABSTRACT_CLASS(wxPaintDCImpl, wxWindowDCImpl)

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
 : wxWindowDCImpl( owner )
{
}

#if wxDEBUG_LEVEL
static bool IsGLCanvas( wxWindow * window )
{
    // If the wx gl library isn't loaded then ciGLCanvas will be NULL.
    static const wxClassInfo* const ciGLCanvas = wxClassInfo::FindClass("wxGLCanvas");
    return ciGLCanvas && window->IsKindOf(ciGLCanvas);
}
#endif

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *window ) :
    wxWindowDCImpl( owner, window )
{
    // With macOS 10.14, wxGLCanvas windows have a NULL CGContextRef.
    wxASSERT_MSG( window->MacGetCGContextRef() != NULL || IsGLCanvas(window), wxT("using wxPaintDC without being in a native paint event") );
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    SetDeviceOrigin( origin.x, origin.y );
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;
}

wxPaintDCImpl::~wxPaintDCImpl()
{
}
