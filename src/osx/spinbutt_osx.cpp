/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/spinbutt_osx.cpp
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/osx/private.h"


wxSpinButton::wxSpinButton()
   : wxSpinButtonBase()
{
}

bool wxSpinButton::Create( wxWindow *parent,
    wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name )
{    
    DontCreatePeer();
    if ( !wxSpinButtonBase::Create( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    m_min = 0;
    m_max = 100;

    if (!parent)
        return false;

    SetPeer(wxWidgetImpl::CreateSpinButton( this , parent, id, 0, m_min, m_max, pos, size,
        style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

void wxSpinButton::SetValue( int val )
{
    GetPeer()->SetValue( val );
}

int wxSpinButton::GetValue() const
{
    return GetPeer()->GetValue();
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    GetPeer()->SetMaximum( maxVal );
    GetPeer()->SetMinimum( minVal );
}

void wxSpinButton::SendThumbTrackEvent()
{
    wxSpinEvent event( wxEVT_SCROLL_THUMBTRACK, GetId() );
    event.SetPosition( GetValue() );
    event.SetEventObject( this );
    HandleWindowEvent( event );
}

bool wxSpinButton::OSXHandleClicked( double WXUNUSED(timestampsec) )
{
    // all events have already been processed
    return true;
}

wxSize wxSpinButton::DoGetBestSize() const
{
    return wxSize( 16, 24 );
}

void wxSpinButton::TriggerScrollEvent(wxEventType scrollEvent)
{
    int inc = 0;

    if ( scrollEvent == wxEVT_SCROLL_LINEUP )
    {
        inc = 1;
    }
    else if ( scrollEvent == wxEVT_SCROLL_LINEDOWN )
    {
        inc = -1;
    }
    else
    {
        wxFAIL_MSG( "Unexpected event" );
    }

    // trigger scroll events

    int oldValue = GetValue() ;

    int newValue = oldValue + inc;

    if (newValue < m_min)
    {
        if ( m_windowStyle & wxSP_WRAP )
            newValue = m_max;
        else
            newValue = m_min;
    }

    if (newValue > m_max)
    {
        if ( m_windowStyle & wxSP_WRAP )
            newValue = m_min;
        else
            newValue = m_max;
    }

    // Do not send an event if the value has not actually changed
    // (Also works for wxSpinCtrl)
    if ( newValue == oldValue )
        return;

    wxSpinEvent event( scrollEvent, m_windowId );

    event.SetPosition( newValue );
    event.SetEventObject( this );
    if ((HandleWindowEvent( event )) && !event.IsAllowed())
        newValue = oldValue;

    SetValue( newValue );

    // send a thumbtrack event if EVT_SPIN_UP/DOWN wasn't vetoed 
    if ( newValue != oldValue )
        SendThumbTrackEvent() ;
}

void wxSpinButton::SetIncrement(int value)
{
    GetPeer()->SetIncrement( value );
}

int wxSpinButton::GetIncrement() const
{
    return GetPeer()->GetIncrement();
}

#endif // wxUSE_SPINBTN
