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

    m_rangeMax = range ;
    m_gaugePos = 0 ;
    
    if ( size.x == wxDefaultSize.x && size.y == wxDefaultSize.y)
    {
        size = wxSize( 200 , 16 ) ;
    }
    
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    verify_noerr ( CreateProgressBarControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , 
     m_gaugePos , 0 , m_rangeMax , false /* not indeterminate */ , (ControlRef*) &m_macControl ) ) ;
       
    MacPostControlCreate(pos,size) ;
    
    return TRUE;
}

void wxGauge::SetShadowWidth(int w)
{
}

void wxGauge::SetBezelFace(int w)
{
}

void wxGauge::SetRange(int r)
{
    m_rangeMax = r;
    ::SetControl32BitMaximum( (ControlRef) m_macControl , m_rangeMax ) ;
}

void wxGauge::SetValue(int pos)
{
    m_gaugePos = pos;
       ::SetControl32BitValue( (ControlRef) m_macControl , m_gaugePos ) ;
}

int wxGauge::GetShadowWidth() const
{
    return 0;
}

int wxGauge::GetBezelFace() const
{
    return 0;
}

int wxGauge::GetRange() const
{
    return m_rangeMax;
}

int wxGauge::GetValue() const
{
    return m_gaugePos;
}

