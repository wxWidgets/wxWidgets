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
#include "wx/osx/private.h"


wxWidgetImplType* wxWidgetImpl::CreateSpinButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    Rect bounds = wxMacGetBoundsForControl( wxpeer , pos , size );

    wxMacControl* peer = new wxMacControl( wxpeer );
    OSStatus err = CreateLittleArrowsControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, value, 
        minimum, maximum, 1, peer->GetControlRefAddr() );
    verify_noerr( err );

    peer->SetActionProc( GetwxMacLiveScrollbarActionProc() );
    return peer ;
}

void wxSpinButton::MacHandleControlClick(WXWidget WXUNUSED(control),
                                         wxInt16 controlpart,
                                         bool WXUNUSED(mouseStillDown))
{
    int inc = 0;

    switch ( controlpart )
    {
    case kControlUpButtonPart :
        inc = 1;
        break;

    case kControlDownButtonPart :
        inc = -1;
        break;

    default:
        break;
    }
    
    // trigger scroll events
    
    wxEventType scrollEvent = wxEVT_NULL;
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

    if ( newValue - oldValue == -1 )
        scrollEvent = wxEVT_SCROLL_LINEDOWN;
    else if ( newValue - oldValue == 1 )
        scrollEvent = wxEVT_SCROLL_LINEUP;
    else
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;

    // Do not send an event if the value has not actually changed
    // (Also works for wxSpinCtrl)
    if ( newValue == oldValue )
        return;

    if ( scrollEvent != wxEVT_SCROLL_THUMBTRACK )
    {
        wxSpinEvent event( scrollEvent, m_windowId );

        event.SetPosition( newValue );
        event.SetEventObject( this );
        if ((HandleWindowEvent( event )) && !event.IsAllowed())
            newValue = oldValue;
    }

    m_peer->SetValue( newValue );

    // always send a thumbtrack event
    SendThumbTrackEvent() ;
}

#endif // wxUSE_SPINBTN
