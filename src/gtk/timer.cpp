/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTimer,wxObject)

gint timeout_callback( gpointer data )
{
  wxTimer *timer = (wxTimer*)data;
  timer->Notify();
  if (timer->OneShot()) timer->Stop();
  return TRUE;
};

wxTimer::wxTimer(void)
{
  m_tag = -1;
  m_time = 1000;
  m_oneShot = FALSE;
};

wxTimer::~wxTimer(void)
{
  Stop();
};

int wxTimer::Interval(void)
{
  return m_time;
};

bool wxTimer::OneShot(void)
{
  return m_oneShot;
};

void wxTimer::Notify(void)
{
};

void wxTimer::Start( int millisecs, bool oneShot )
{
  if (millisecs != -1) m_time = millisecs;
  m_oneShot = oneShot;
  m_tag = gtk_timeout_add( millisecs, timeout_callback, this );
};

void wxTimer::Stop(void)
{
  if (m_tag != -1)
    gtk_timeout_remove( m_tag );
  m_tag = -1;
};

