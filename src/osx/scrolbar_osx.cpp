/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: scrolbar.cpp 54129 2008-06-11 19:30:52Z SC $
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
        
    m_peer = wxWidgetImpl::CreateScrollBar( this, parent, id, pos, size, style, GetExtraStyle() );

    MacPostControlCreate( pos, size );

    return true;
}

wxScrollBar::~wxScrollBar()
{
}

void wxScrollBar::SetThumbPosition( int viewStart )
{
    m_peer->SetScrollThumb( viewStart, m_viewSize );
}

int wxScrollBar::GetThumbPosition() const
{
    return m_peer->GetValue();
}

void wxScrollBar::SetScrollbar( int position,
                                int thumbSize,
                                int range,
                                int pageSize,
                                bool WXUNUSED(refresh) )
{
    m_pageSize = pageSize;
    m_viewSize = thumbSize;
    m_objectSize = range;

   int range1 = wxMax( (m_objectSize - m_viewSize), 0 );
   
   m_peer->SetMaximum( range1 );
   m_peer->SetScrollThumb( position, m_viewSize );
}

void wxScrollBar::Command( wxCommandEvent& event )
{
    SetThumbPosition( event.GetInt() );
    ProcessCommand( event );
}

bool wxScrollBar::HandleClicked( double timestampsec )
{
    int new_pos = m_peer->GetValue();

    wxScrollEvent event( wxEVT_SCROLL_THUMBRELEASE, m_windowId );
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

    return true;
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
