/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:
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
/*
#include "glib.h"
*/

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

/*
static GTimer *g_timer = (GTimer*) NULL;

void wxStartTimer()
{
    if (g_timer)
    {
        g_timer_rest( g_timer );
    }
    else
    {
        g_timer = g_timer_new();
        g_timer_start( g_timer );
    }
}

long wxGetElapsedTime( bool resetTimer )
{
    gulong res = 0;
    if (g_timer)
    {
        g_timer_elapsed( g_timer, &res );
        if (resetTimer) g_timer_reset( g_timer );
    }

    return res;
}

bool wxGetLocalTime( long *timeZone, int *dstObserved )
{
}

long wxGetCurrentTime()
{
}
*/


//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer,wxObject)

static gint timeout_callback( gpointer data )
{
    wxTimer *timer = (wxTimer*)data;

#if (GTK_MINOR_VERSION > 0)
    /* when getting called from GDK's timer handler we
       are no longer within GDK's grab on the GUI
       thread so we must lock it here ourselves */
    GDK_THREADS_ENTER ();
#endif

    timer->Notify();

#if (GTK_MINOR_VERSION > 0)
    /* release lock again */
    GDK_THREADS_LEAVE ();
#endif

    if ( timer->IsOneShot() )
        return FALSE;

    return TRUE;
}

wxTimer::wxTimer()
{
    m_tag = -1;
    m_milli = 1000;
    m_oneShot = FALSE;
}

wxTimer::~wxTimer()
{
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

