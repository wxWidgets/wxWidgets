/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer,wxObject)

gint timeout_callback( gpointer data )
{
  wxTimer *timer = (wxTimer*)data;
  timer->Notify();

  if ( timer->OneShot() )
    timer->Stop();

  return TRUE;
}

wxTimer::wxTimer()
{
  m_tag = -1;
  m_time = 1000;
  m_oneShot = FALSE;
}

wxTimer::~wxTimer()
{
  Stop();
}

bool wxTimer::Start( int millisecs, bool oneShot )
{
  if ( millisecs != -1 )
    m_time = millisecs;

  m_oneShot = oneShot;

  m_tag = gtk_timeout_add( millisecs, timeout_callback, this );

  return TRUE;
}

void wxTimer::Stop()
{
  if ( m_tag != -1 )
  {
    gtk_timeout_remove( m_tag );

    m_tag = -1;
  }
}

