/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TIMER

#include "wx/timer.h"

#include <gtk/gtk.h>

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

extern "C" {
static gboolean timeout_callback(gpointer data)
{
    wxTimer *timer = (wxTimer*)data;

    // Don't change the order of anything in this callback!

    if (timer->IsOneShot())
    {
        // This sets m_tag to -1
        timer->Stop();
    }

    // When getting called from GDK's timer handler we
    // are no longer within GDK's grab on the GUI
    // thread so we must lock it here ourselves.
    gdk_threads_enter();

    timer->Notify();

    // Release lock again.
    gdk_threads_leave();

    return !timer->IsOneShot();
}
}

void wxTimer::Init()
{
    m_sourceId = 0;
    m_milli = 1000;
}

wxTimer::~wxTimer()
{
    Stop();
}

bool wxTimer::Start( int millisecs, bool oneShot )
{
    (void)wxTimerBase::Start(millisecs, oneShot);

    if (m_sourceId != 0)
        g_source_remove(m_sourceId);

    m_sourceId = g_timeout_add(m_milli, timeout_callback, this);

    return true;
}

void wxTimer::Stop()
{
    if (m_sourceId != 0)
    {
        g_source_remove(m_sourceId);
        m_sourceId = 0;
    }
}

#endif // wxUSE_TIMER

