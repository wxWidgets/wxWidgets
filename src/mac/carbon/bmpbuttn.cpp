/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#include "wx/mac/uma.h"
#include "wx/bitmap.h"

bool wxBitmapButton::Create( wxWindow *parent,
                             wxWindowID id, const wxBitmap& bitmap,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name )
{
    m_macIsUserPane = false;

    // since bitmapbuttonbase is subclass of button calling wxBitmapButtonBase::Create
    // essentially creates an additional button
    if ( !wxControl::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    m_bmpNormal = bitmap;

    if ( style & wxBU_AUTODRAW )
    {
        m_marginX =
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }
    else
    {
        m_marginX =
        m_marginY = 0;
    }

    int width = size.x;
    int height = size.y;

    if ( bitmap.Ok() )
    {
        wxSize newSize = DoGetBestSize();
        if ( width == wxDefaultCoord )
            width = newSize.x;
        if ( height == wxDefaultCoord )
            height = newSize.y;
    }

    m_bmpNormal = bitmap;

    OSStatus err = noErr;
    ControlButtonContentInfo info;

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );
    m_peer = new wxMacControl( this );

#ifdef __WXMAC_OSX__
    if ( HasFlag( wxBORDER_NONE ) )
    {
        wxMacCreateBitmapButton( &info, m_bmpNormal, kControlContentIconRef );
        err = CreateIconControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
            &bounds, &info, false, m_peer->GetControlRefAddr() );
    }
    else
#endif
    {
        wxMacCreateBitmapButton( &info, m_bmpNormal );
        err = CreateBevelButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, CFSTR(""),
            ((style & wxBU_AUTODRAW) ? kControlBevelButtonSmallBevel : kControlBevelButtonNormalBevel ),
            kControlBehaviorOffsetContents, &info, 0, 0, 0, m_peer->GetControlRefAddr() );
    }

    verify_noerr( err );

    wxMacReleaseBitmapButton( &info );
    wxASSERT_MSG( m_peer != NULL && m_peer->Ok(), wxT("No valid native Mac control") );

    MacPostControlCreate( pos, size );

    return true;
}

void wxBitmapButton::SetBitmapLabel( const wxBitmap& bitmap )
{
    m_bmpNormal = bitmap;
    InvalidateBestSize();

    ControlButtonContentInfo info;

#ifdef __WXMAC_OSX__
    if ( HasFlag( wxBORDER_NONE ) )
    {
        wxMacCreateBitmapButton( &info, m_bmpNormal, kControlContentIconRef );
        if ( info.contentType != kControlNoContent )
            m_peer->SetData( kControlIconPart, kControlIconContentTag, info );
    }
    else
#endif
    {
        wxMacCreateBitmapButton( &info, m_bmpNormal );
        if ( info.contentType != kControlNoContent )
            m_peer->SetData( kControlButtonPart, kControlBevelButtonContentTag, info );
    }

    wxMacReleaseBitmapButton( &info );
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best;

    best.x = 2 * m_marginX;
    best.y = 2 * m_marginY;
    if ( m_bmpNormal.Ok() )
    {
        best.x += m_bmpNormal.GetWidth();
        best.y += m_bmpNormal.GetHeight();
    }

    return best;
}

#endif
