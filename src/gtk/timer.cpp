/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

#include "gtk/gtk.h"

// ----------------------------------------------------------------------------
// This is a hack to work around a crash in wxGTK which happens (sometimes)
// when the timer is deleted from the GTK event handler. In this case even
// though the timer is stopped in its dtor, it is apparently too late to
// prevent the dead timer from being called from g_timeout_dispatch() during
// the same iteration of g_main_dispatch(). A better solution must be found,
// possibly by deferring the timer deletion until later, this is only a
// temporary hack! (VZ)
// ----------------------------------------------------------------------------

static wxTimer *gs_timerDead = (wxTimer *)NULL;

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer,wxObject)

static gint timeout_callback( gpointer data )
{
    wxTimer *timer = (wxTimer*)data;
    if ( timer == gs_timerDead )
    {
        // shouldn't be called more than once for the dead timer anyhow
        gs_timerDead = NULL;

        return FALSE;
    }

    /* when getting called from GDK's timer handler we
       are no longer within GDK's grab on the GUI
       thread so we must lock it here ourselves */
    gdk_threads_enter();

    timer->Notify();

    /* release lock again */
    gdk_threads_leave();

    if ( timer->IsOneShot() )
        return FALSE;

    return TRUE;
}

void wxTimer::Init()
{
    m_tag = -1;
    m_milli = 1000;
}

wxTimer::~wxTimer()
{
    gs_timerDead = this;

    wxTimer::Stop();
}

bool wxTimer::Start( int millisecs, bool oneShot )
{
    (void)wxTimerBase::Start(millisecs, oneShot);

    m_tag = gtk_timeout_add( m_milli, timeout_callback, this );

    return TRUE;
}

void wxTimer::Stop()
{
    if (m_tag != -1)
    {
        gtk_timeout_remove( m_tag );
        m_tag = -1;
    }
}

