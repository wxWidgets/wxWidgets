/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gauge.h"
#endif

#include "wx/gauge.h"

//-----------------------------------------------------------------------------
// wxGauge
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGauge,wxControl)

bool wxGauge::Create( wxWindow *parent, wxWindowID id,  int range,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name )
{
  return TRUE;
};

void wxGauge::SetRange( int r )
{
  m_rangeMax = r;
  if (m_gaugePos > m_rangeMax) m_gaugePos = m_rangeMax;
};

void wxGauge::SetValue( int pos )
{
  m_gaugePos = pos;
  if (m_gaugePos > m_rangeMax) m_gaugePos = m_rangeMax;
};

int wxGauge::GetRange(void) const
{
  return m_rangeMax;
};

int wxGauge::GetValue(void) const
{
  return m_gaugePos;
};

