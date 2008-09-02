/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: spinbutt.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/osx/private.h"


IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent)


wxSpinButton::wxSpinButton()
   : wxSpinButtonBase()
{
}

bool wxSpinButton::Create( wxWindow *parent,
    wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name )
{
    m_macIsUserPane = false;

    if ( !wxSpinButtonBase::Create( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    m_min = 0;
    m_max = 100;

    if (!parent)
        return false;
    
    m_peer = wxWidgetImpl::CreateSpinButton( this , parent, id, 0, m_min, m_max, pos, size,
        style, GetExtraStyle() );
        
    MacPostControlCreate( pos, size );

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

void wxSpinButton::SetValue( int val )
{
    m_peer->SetValue( val );
}

int wxSpinButton::GetValue() const
{
    return m_peer->GetValue();
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    m_peer->SetMaximum( maxVal );
    m_peer->SetMinimum( minVal );
}   
    
void wxSpinButton::SendThumbTrackEvent() 
{
    wxSpinEvent event( wxEVT_SCROLL_THUMBTRACK, GetId() );
    event.SetPosition( GetValue() );
    event.SetEventObject( this );
    HandleWindowEvent( event );
}

bool wxSpinButton::HandleClicked( double timestampsec )
{
#if wxOSX_USE_CARBON
    // these have been handled by the live action proc already
#else
    SendThumbTrackEvent() ;
#endif

    return true;
}

wxSize wxSpinButton::DoGetBestSize() const
{
    return wxSize( 16, 24 );
}

#endif // wxUSE_SPINBTN
