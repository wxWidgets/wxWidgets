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
#include "wx/graphics.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// TODO : for the Screenshot use case, which doesn't work in Quartz
// we should do a GetAsBitmap using something like
// http://www.cocoabuilder.com/archive/message/cocoa/2005/8/13/144256

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
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
}

wxScreenDC::~wxScreenDC()
{
    delete m_graphicContext;
    m_graphicContext = NULL;
    DisposeWindow((WindowRef) m_overlayWindow );
}
