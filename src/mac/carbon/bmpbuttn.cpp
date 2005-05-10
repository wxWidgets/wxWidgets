/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/window.h"
#include "wx/bmpbuttn.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#include "wx/mac/uma.h"
#include "wx/bitmap.h"

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    // since bitmapbuttonbase is subclass of button calling wxBitmapButtonBase::Create
    // essentially creates an additional button
    if ( !wxControl::Create(parent, id, pos, size,
                                     style, validator, name) )
        return false;

    m_bmpNormal = bitmap;
 
    if (style & wxBU_AUTODRAW)
    {
        m_marginX = wxDEFAULT_BUTTON_MARGIN;
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }
    else
    {
        m_marginX = 0;
        m_marginY = 0;
    }

    int width = size.x;
    int height = size.y;

    if ( bitmap.Ok() )
    {
        wxSize newSize = DoGetBestSize();
        if ( width == -1 )
            width = newSize.x;
        if ( height == -1 )
            height = newSize.y;
    }

    m_bmpNormal = bitmap;
        
    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , m_bmpNormal ) ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_peer = new wxMacControl( this ) ;
    verify_noerr ( CreateBevelButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") , 
        (( style & wxBU_AUTODRAW ) ? kControlBevelButtonSmallBevel : kControlBevelButtonNormalBevel )  , 
        kControlBehaviorOffsetContents , &info , 0 , 0 , 0 , m_peer->GetControlRefAddr() ) );
    
    wxMacReleaseBitmapButton( &info ) ;
    wxASSERT_MSG( m_peer != NULL && m_peer->Ok() , wxT("No valid mac control") ) ;
    
    MacPostControlCreate(pos,size) ;

    return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_bmpNormal = bitmap;
    InvalidateBestSize();

    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
    if ( info.contentType != kControlNoContent )
    {
        m_peer->SetData( kControlButtonPart , kControlBevelButtonContentTag , info ) ;
    }
    wxMacReleaseBitmapButton( &info ) ;
}


wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best;
    if (m_bmpNormal.Ok())
    {
        best.x = m_bmpNormal.GetWidth() + 2*m_marginX;
        best.y = m_bmpNormal.GetHeight() + 2*m_marginY;
    }
    return best;
}

#endif
