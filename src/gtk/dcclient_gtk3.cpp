/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dcclient.cpp
// Purpose:     wxWindowDCImpl implementation
// Author:      Robert Roebling
// RCS-ID:      $Id: dcclient.cpp 65846 2010-10-18 23:43:20Z VZ $
// Copyright:   (c) 1998 Robert Roebling, Chris Breeze
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
#include "wx/gtk/private.h"
#include "wx/gtk/dcclient_gtk3.h"

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

    // CGContextRef cg = (CGContextRef) window->MacGetCGContextRef();

    // m_release = false;
    // if ( cg == NULL )
    // {
    if (window->GTKGetDrawingWindow())
    {
        SetGraphicsContext( wxGraphicsContext::Create( window ) ) ;
    // }
    // else
    // {
    //     CGContextSaveGState( cg );
    //     m_release = true ;
    //     // make sure the context is having its origin at the wx-window coordinates of the
    //     // view (read at the top of window.cpp about the differences)
    //     if ( window->MacGetLeftBorderSize() != 0 || window->MacGetTopBorderSize() != 0 )
    //         CGContextTranslateCTM( cg , -window->MacGetLeftBorderSize() , -window->MacGetTopBorderSize() );

    //     SetGraphicsContext( wxGraphicsContext::CreateFromNative( cg ) );
    // }
        DoSetClippingRegion(0, 0, m_width, m_height);

        SetBackground(wxBrush(window->GetBackgroundColour()));
    }
    SetFont( window->GetFont() ) ;
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    // if ( m_release )
    // {
    //     // this must not necessarily be the current context, we must restore the state of the
    //     // cg we started with above (before the CGContextTranslateCTM call)
    //     CGContextRef cg = (CGContextRef) m_window->MacGetCGContextRef();
    //     CGContextRestoreGState(cg);
    // }
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
    wxBitmap bitmap;
    if (m_window)
    {
        GdkWindow* window = m_window->GTKGetDrawingWindow();
        if (window)
        {
            wxRect rect = subrect ? *subrect : wxRect(0, 0, m_width, m_height);
            bitmap.SetPixbuf(gdk_pixbuf_get_from_window(
                window, rect.x, rect.y, rect.width, rect.height));
        }
    }
    return bitmap;
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
    wxCHECK_RET( window, wxT("invalid window in wxClientDCImpl") );
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    if ( !m_window->IsShownOnScreen() )
        m_width = m_height = 0;
    if (window->GTKGetDrawingWindow())
    {
        SetDeviceOrigin(origin.x, origin.y);
        DoSetClippingRegion(0, 0, m_width, m_height);
    }
}

wxClientDCImpl::~wxClientDCImpl()
{
    if( GetGraphicsContext() && GetGraphicsContext()->GetNativeContext() )
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

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *window ) :
    wxWindowDCImpl( owner, window )
{
    // wxASSERT_MSG( window->MacGetCGContextRef() != NULL, wxT("using wxPaintDC without being in a native paint event") );
    wxPoint origin = window->GetClientAreaOrigin() ;
    m_window->GetClientSize( &m_width , &m_height);
    SetDeviceOrigin( origin.x, origin.y );
    DoSetClippingRegion( 0 , 0 , m_width , m_height ) ;
}

wxPaintDCImpl::~wxPaintDCImpl()
{
}
