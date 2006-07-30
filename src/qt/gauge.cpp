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

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)

bool wxGauge::Create(wxWindow *parent, wxWindowID id,
           int range,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_rangeMax = range;
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;


    // TODO
    return FALSE;
}

void wxGauge::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxGauge::SetShadowWidth(int w)
{
    // TODO optional
}

void wxGauge::SetBezelFace(int w)
{
    // TODO optional
}

void wxGauge::SetRange(int r)
{
    m_rangeMax = r;
    // TODO
}

void wxGauge::SetValue(int pos)
{
    m_gaugePos = pos;
    // TODO
}

int wxGauge::GetShadowWidth() const
{
    // TODO optional
    return 0;
}

int wxGauge::GetBezelFace() const
{
    // TODO optional
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

void wxGauge::SetForegroundColour(const wxColour& col)
{
    m_foregroundColour = col ;
}

void wxGauge::SetBackgroundColour(const wxColour& col)
{
    m_backgroundColour = col ;
}

