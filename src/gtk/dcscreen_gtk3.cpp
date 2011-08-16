/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: dcscreen.cpp 61724 2009-08-21 10:41:26Z VZ $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/gtk/dcscreen_gtk3.h"

#include "wx/gtk/private.h"
#include "wx/graphics.h"

IMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWindowDCImpl)

// Create a DC representing the whole screen
wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner ) :
   wxWindowDCImpl( owner )
{
    wxDisplaySize( &m_width, &m_height );
    // Rect bounds;
    // bounds.top = (short)cgbounds.origin.y;
    // bounds.left = (short)cgbounds.origin.x;
    // bounds.bottom = bounds.top + (short)cgbounds.size.height;
    // bounds.right = bounds.left  + (short)cgbounds.size.width;
    // WindowAttributes overlayAttributes  = kWindowIgnoreClicksAttribute;
    // CreateNewWindow( kOverlayWindowClass, overlayAttributes, &bounds, (WindowRef*) &m_overlayWindow );
    // ShowWindow((WindowRef)m_overlayWindow);
    // SetGraphicsContext( wxGraphicsContext::CreateFromNativeWindow( m_overlayWindow ) );
    m_ok = true ;
}

wxScreenDCImpl::~wxScreenDCImpl()
{
    wxDELETE(m_graphicContext);
    // DisposeWindow((WindowRef) m_overlayWindow );
}

wxBitmap wxScreenDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    wxBitmap bitmap;
    wxRect rect = subrect ? *subrect : wxRect(0, 0, m_width, m_height);
    GdkWindow* window = gdk_get_default_root_window();
    bitmap.SetPixbuf(gdk_pixbuf_get_from_window(
        window, rect.x, rect.y, rect.width, rect.height));
    return bitmap;
}
