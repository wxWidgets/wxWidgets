/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.cpp
// Purpose:     wxGauge class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gauge.h"
#endif

#include "wx/gauge.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)
#endif

#include <wx/mac/uma.h>

bool wxGauge::Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos,
           const wxSize& s,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
	wxSize size = s ;
	Rect bounds ;
	Str255 title ;
	m_rangeMax = range ;
	m_macHorizontalBorder = 2 ; // additional pixels around the real control
	m_macVerticalBorder = 2 ;
	
	if ( size.x == wxDefaultSize.x && size.y == wxDefaultSize.y)
	{
		size = wxSize( 200 , 16 ) ;
	}
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , range, 
	  	kControlProgressBarProc , (long) this ) ;
	
	MacPostControlCreate() ;

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
    ::SetControlMaximum( m_macControl , m_rangeMax ) ;
}

void wxGauge::SetValue(int pos)
{
    m_gaugePos = pos;
   ::SetControlValue( m_macControl , m_gaugePos ) ;
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

