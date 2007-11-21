/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/mac/uma.h"


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

    Rect bounds = wxMacGetBoundsForControl( this , pos , size );

    m_peer = new wxMacControl( this );
    OSStatus err = CreateLittleArrowsControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, 0, m_min, m_max, 1,
        m_peer->GetControlRefAddr() );
    verify_noerr( err );

    m_peer->SetActionProc( GetwxMacLiveScrollbarActionProc() );
    MacPostControlCreate( pos, size );

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

int wxSpinButton::GetMin() const
{
    return m_min;
}

int wxSpinButton::GetMax() const
{
    return m_max;
}

int wxSpinButton::GetValue() const
{
    int n = m_value;

    if (n < m_min)
        n = m_min;
    else if (n > m_max)
        n = m_max;

    return n;
}

void wxSpinButton::SetValue(int val)
{
    m_value = val;
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    m_peer->SetMaximum( maxVal );
    m_peer->SetMinimum( minVal );
}

void wxSpinButton::MacHandleValueChanged( int inc )
{
    wxEventType scrollEvent = wxEVT_NULL;
    int oldValue = m_value;

    m_value = oldValue + inc;

    if (m_value < m_min)
    {
        if ( m_windowStyle & wxSP_WRAP )
            m_value = m_max;
        else
            m_value = m_min;
    }

    if (m_value > m_max)
    {
        if ( m_windowStyle & wxSP_WRAP )
            m_value = m_min;
        else
            m_value = m_max;
    }

    if ( m_value - oldValue == -1 )
        scrollEvent = wxEVT_SCROLL_LINEDOWN;
    else if ( m_value - oldValue == 1 )
        scrollEvent = wxEVT_SCROLL_LINEUP;
    else
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;

    // Do not send an event if the value has not actually changed
    // (Also works for wxSpinCtrl)
    if ( m_value == oldValue )
        return;

    wxSpinEvent event( scrollEvent, m_windowId );

    event.SetPosition( m_value );
    event.SetEventObject( this );
    if ((GetEventHandler()->ProcessEvent( event )) && !event.IsAllowed())
        m_value = oldValue;

    m_peer->SetValue( m_value );

    // always send a thumbtrack event
    if (scrollEvent != wxEVT_SCROLL_THUMBTRACK)
    {
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
        wxSpinEvent event2( scrollEvent, GetId() );
        event2.SetPosition( m_value );
        event2.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event2 );
    }
}

void wxSpinButton::MacHandleControlClick( WXWidget control, wxInt16 controlpart, bool mouseStillDown )
{
    int nScrollInc = 0;

    switch ( controlpart )
    {
    case kControlUpButtonPart :
        nScrollInc = 1;
        break;

    case kControlDownButtonPart :
        nScrollInc = -1;
        break;

    default:
        break;
    }

    MacHandleValueChanged( nScrollInc ) ;
}

wxInt32 wxSpinButton::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF event )
{
#if 0
    // these have been handled by the live action proc already
    int nScrollInc = 0;
    wxMacCarbonEvent cEvent( (EventRef)event );

    switch ( cEvent.GetParameter<ControlPartCode>(kEventParamControlPart, typeControlPartCode) )
    {
    case kControlUpButtonPart :
        nScrollInc = 1;
        break;

    case kControlDownButtonPart :
        nScrollInc = -1;
        break;

    default :
        break;
    }

    MacHandleValueChanged( nScrollInc ) ;
#endif

    return noErr;
}

wxSize wxSpinButton::DoGetBestSize() const
{
    return wxSize( 16, 24 );
}

#endif // wxUSE_SPINBTN
