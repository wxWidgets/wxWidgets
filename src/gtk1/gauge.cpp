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
  m_needParent = TRUE;
  
  wxSize newSize = size;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_rangeMax = range;
  m_gaugePos = 0;
  m_useProgressBar = TRUE;
  
  m_widget = gtk_progress_bar_new();
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};

void wxGauge::SetRange( int r )
{
  m_rangeMax = r;
  if (m_gaugePos > m_rangeMax) m_gaugePos = m_rangeMax;
  
  gtk_progress_bar_update( GTK_PROGRESS_BAR(m_widget), (float)(m_rangeMax/m_gaugePos) );
};

void wxGauge::SetValue( int pos )
{
  m_gaugePos = pos;
  if (m_gaugePos > m_rangeMax) m_gaugePos = m_rangeMax;
  
  gtk_progress_bar_update( GTK_PROGRESS_BAR(m_widget), (float)(m_rangeMax/m_gaugePos) );
};

int wxGauge::GetRange(void) const
{
  return m_rangeMax;
};

int wxGauge::GetValue(void) const
{
  return m_gaugePos;
};

