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

#ifdef __GNUG__
#pragma implementation "gauge.h"
#endif

#include "wx/gauge.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)
#endif

#include "wx/mac/uma.h"

bool wxGauge::Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos,
           const wxSize& s,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;

    if ( !wxGaugeBase::Create(parent, id, range, pos, s, style & 0xE0FFFFFF, validator, name) )
        return false;

    wxSize size = s ;
    /*
    if ( size.x == wxDefaultCoord && size.y == wxDefaultCoord)
    {
        size = wxSize( 200 , 16 ) ;
    }
    */
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    m_peer = new wxMacControl() ;
    verify_noerr ( CreateProgressBarControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , 
     GetValue() , 0 , GetRange() , false /* not indeterminate */ , m_peer->GetControlRefAddr() ) );
    
       
    MacPostControlCreate(pos,size) ;
    
    return TRUE;
}

void wxGauge::SetRange(int r)
{
    // we are going via the base class in case there is 
    // some change behind the values by it
    wxGaugeBase::SetRange(r) ;
    if ( m_peer && m_peer->Ok() )
        m_peer->SetMaximum( GetRange() ) ;
}

void wxGauge::SetValue(int pos)
{
    // we are going via the base class in case there is 
    // some change behind the values by it
    wxGaugeBase::SetValue(pos) ;
    if ( m_peer && m_peer->Ok() )
        m_peer->SetValue( GetValue() ) ;
}

int wxGauge::GetValue() const
{
/*
    if ( m_peer && m_peer->Ok() )
        return m_peer->GetValue() ;
*/
    return m_gaugePos ;
}

