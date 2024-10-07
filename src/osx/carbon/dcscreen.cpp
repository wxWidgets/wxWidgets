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
#include "wx/graphics.h"

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWindowDCImpl);

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

wxBitmap wxScreenDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    wxRect rect = subrect ? *subrect : wxRect(0, 0, m_width, m_height);

    wxBitmap bmp(rect.GetSize(), 32);

    // TODO: Implement this using ScreenCaptureKit.

    return bmp;
}
