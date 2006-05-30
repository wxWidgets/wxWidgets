/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

BEGIN_EVENT_TABLE(wxScrollBar, wxControl)
END_EVENT_TABLE()


bool wxScrollBar::Create( wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    m_macIsUserPane = false;

    if ( !wxControl::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );

    m_peer = new wxMacControl( this );
    OSStatus err = CreateScrollBarControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds,
        0, 0, 100, 1, true /* liveTracking */,
        GetwxMacLiveScrollbarActionProc(),
        m_peer->GetControlRefAddr() );
    verify_noerr( err );

    MacPostControlCreate( pos, size );

    return true;
}

wxScrollBar::~wxScrollBar()
{
}

void wxScrollBar::SetThumbPosition( int viewStart )
{
    m_peer->SetValue( viewStart );
}

int wxScrollBar::GetThumbPosition() const
{
    return m_peer->GetValue();
}

void wxScrollBar::SetScrollbar( int position, int thumbSize, int range, int pageSize, bool refresh )
{
    m_pageSize = pageSize;
    m_viewSize = thumbSize;
    m_objectSize = range;

   int range1 = wxMax( (m_objectSize - m_viewSize), 0 );

    m_peer->SetMinimum( 0 );
    m_peer->SetMaximum( range1 );
    m_peer->SetValue( position );
    m_peer->SetViewSize( m_viewSize );
}

void wxScrollBar::Command( wxCommandEvent& event )
{
    SetThumbPosition( event.GetInt() );
    ProcessCommand( event );
}

void wxScrollBar::MacHandleControlClick( WXWidget control, wxInt16 controlpart, bool mouseStillDown )
{
    int position = m_peer->GetValue();
    int minPos = m_peer->GetMinimum();
    int maxPos = m_peer->GetMaximum();

    wxEventType scrollEvent = wxEVT_NULL;
    int nScrollInc = 0;

    // all events have already been reported during mouse down, except for THUMBRELEASE
    if ( !mouseStillDown && controlpart != kControlIndicatorPart )
        return;

    switch ( controlpart )
    {
    case kControlUpButtonPart:
        nScrollInc = -1;
        scrollEvent = wxEVT_SCROLL_LINEUP;
        break;

    case kControlDownButtonPart:
        nScrollInc = 1;
        scrollEvent = wxEVT_SCROLL_LINEDOWN;
        break;

    case kControlPageUpPart:
        nScrollInc = -m_pageSize;
        scrollEvent = wxEVT_SCROLL_PAGEUP;
        break;

    case kControlPageDownPart:
        nScrollInc = m_pageSize;
        scrollEvent = wxEVT_SCROLL_PAGEDOWN;
        break;

    case kControlIndicatorPart:
        nScrollInc = 0;
        if ( mouseStillDown )
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
        else
            scrollEvent = wxEVT_SCROLL_THUMBRELEASE;
        break;

    default:
        wxFAIL_MSG(wxT("unknown scrollbar selector"));
        break;
    }

    int new_pos = position + nScrollInc;

    if (new_pos < minPos)
        new_pos = minPos;
    else if (new_pos > maxPos)
        new_pos = maxPos;

    if ( nScrollInc )
        SetThumbPosition( new_pos );

    wxScrollEvent event( scrollEvent, m_windowId );
    if ( m_windowStyle & wxHORIZONTAL )
        event.SetOrientation( wxHORIZONTAL );
    else
        event.SetOrientation( wxVERTICAL );

    event.SetPosition( new_pos );
    event.SetEventObject( this );

    wxWindow* window = GetParent();
    if (window && window->MacIsWindowScrollbar( this ))
        // this is hardcoded
        window->MacOnScroll( event );
    else
        GetEventHandler()->ProcessEvent( event );
}

wxInt32 wxScrollBar::MacControlHit( WXEVENTHANDLERREF handler, WXEVENTREF mevent )
{
    int position = m_peer->GetValue();
    int minPos = m_peer->GetMinimum();
    int maxPos = m_peer->GetMaximum();

    wxEventType scrollEvent = wxEVT_NULL;
    int nScrollInc = 0;

    wxMacCarbonEvent cEvent( (EventRef)mevent );
    ControlPartCode controlpart = cEvent.GetParameter<ControlPartCode>(kEventParamControlPart, typeControlPartCode);

    // all events have already been reported during mouse down, except for THUMBRELEASE
    // NB: this may need to be reviewed in light of the fact that scroll wheel events
    // aren't being handled properly
    if ( controlpart != kControlIndicatorPart )
        return eventNotHandledErr;

    switch ( controlpart )
    {
    case kControlIndicatorPart:
        nScrollInc = 0;
        scrollEvent = wxEVT_SCROLL_THUMBRELEASE;
        break;

    default:
        wxFAIL_MSG(wxT("unknown scrollbar selector"));
        break;
    }

    int new_pos = position + nScrollInc;

    if (new_pos < minPos)
        new_pos = minPos;
    else if (new_pos > maxPos)
        new_pos = maxPos;

    if ( nScrollInc )
        SetThumbPosition( new_pos );

    wxScrollEvent event( scrollEvent, m_windowId );
    if ( m_windowStyle & wxHORIZONTAL )
        event.SetOrientation( wxHORIZONTAL );
    else
        event.SetOrientation( wxVERTICAL );

    event.SetPosition( new_pos );
    event.SetEventObject( this );
    wxWindow* window = GetParent();
    if (window && window->MacIsWindowScrollbar( this ))
        // this is hardcoded
        window->MacOnScroll( event );
    else
        GetEventHandler()->ProcessEvent( event );

    return noErr;
}


wxSize wxScrollBar::DoGetBestSize() const
{
    int w = 100;
    int h = 100;

    if ( IsVertical() )
    {
        w = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    }
    else
    {
        h = wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y);
    }

    wxSize best(w, h);
    CacheBestSize(best);
    return best;
}
