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

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer,wxObject)

static gint timeout_callback( gpointer data )
{
    wxTimer *timer = (wxTimer*)data;

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

