/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/scrolbar.cpp
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

#include "wx/osx/private.h"

class wxOSXScrollBarCarbonImpl : public wxMacControl
{
public :
    wxOSXScrollBarCarbonImpl( wxWindowMac* peer) : wxMacControl( peer )
    {
    }
    
    void    SetScrollThumb( wxInt32 value, wxInt32 thumbSize ) 
    {
        SetValue( value );
        SetControlViewSize(m_controlRef , thumbSize );
    }
protected:
};

wxWidgetImplType* wxWidgetImpl::CreateScrollBar( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    Rect bounds = wxMacGetBoundsForControl( wxpeer, pos, size );

    wxMacControl* peer = new wxOSXScrollBarCarbonImpl( wxpeer );
    OSStatus err = CreateScrollBarControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds,
        0, 0, 100, 1, true /* liveTracking */,
        GetwxMacLiveScrollbarActionProc(),
        peer->GetControlRefAddr() );
    verify_noerr( err );
    return peer;
}

void wxScrollBar::MacHandleControlClick( WXWidget WXUNUSED(control), wxInt16 controlpart, bool mouseStillDown )
{
#if wxOSX_USE_CARBON

    int position = m_peer->GetValue();
    int minPos = 0 ;
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
        HandleWindowEvent( event );
#endif
}