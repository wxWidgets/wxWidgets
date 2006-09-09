/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.cpp
// Purpose:     wxGauge class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GAUGE 

#include "wx/gauge.h"

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)

#include "wx/mac/uma.h"

bool wxGauge::Create( wxWindow *parent,
    wxWindowID id,
    int range,
    const wxPoint& pos,
    const wxSize& s,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    m_macIsUserPane = false;

    if ( !wxGaugeBase::Create( parent, id, range, pos, s, style & 0xE0FFFFFF, validator, name ) )
        return false;

    wxSize size = s;

#if 0
    if (size.x == wxDefaultCoord && size.y == wxDefaultCoord)
        size = wxSize( 200 , 16 );
#endif

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );
    m_peer = new wxMacControl( this );
    OSStatus err = CreateProgressBarControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds,
        GetValue(), 0, GetRange(), false /* not indeterminate */, m_peer->GetControlRefAddr() );
    verify_noerr( err );

    if ( GetValue() == 0 )
        m_peer->SetData<Boolean>( kControlEntireControl, kControlProgressBarAnimatingTag, (Boolean)false );

    MacPostControlCreate( pos, size );
    
    return true;
}

void wxGauge::SetRange(int r)
{
    // we are going via the base class in case there is 
    // some change behind the values by it
    wxGaugeBase::SetRange( r ) ;
    if ( m_peer && m_peer->Ok() )
        m_peer->SetMaximum( GetRange() ) ;
}

void wxGauge::SetValue(int pos)
{
    // we are going via the base class in case there is 
    // some change behind the values by it
    wxGaugeBase::SetValue( pos ) ;

    if ( m_peer && m_peer->Ok() )
    {
        m_peer->SetValue( GetValue() ) ;

        // turn off animation in the unnecessary situations as this is consuming a lot of CPU otherwise
        Boolean shouldAnimate = ( GetValue() > 0 && GetValue() < GetRange() ) ;
        if ( m_peer->GetData<Boolean>( kControlEntireControl, kControlProgressBarAnimatingTag ) != shouldAnimate )
        {
            m_peer->SetData<Boolean>( kControlEntireControl, kControlProgressBarAnimatingTag, shouldAnimate ) ;
            if ( !shouldAnimate )
                Refresh() ;
        }
    }
}

int wxGauge::GetValue() const
{
#if 0
    if ( m_peer && m_peer->Ok() )
        return m_peer->GetValue() ;
#endif

    return m_gaugePos ;
}

void wxGauge::Pulse()
{
    if ( m_peer && m_peer->Ok() )
    {
        // need to use the animate() method of NSProgressIndicator Class here
    }
}

#endif // wxUSE_GAUGE

