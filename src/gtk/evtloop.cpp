///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/evtloop.cpp
// Purpose:     implements wxEventLoop for GTK+
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.07.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>

// ============================================================================
// wxEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxGUIEventLoop::wxGUIEventLoop()
{
    m_exitcode = 0;
}

int wxGUIEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, "can't reenter a message loop" );

    wxEventLoopActivator activate(this);

    gtk_main();

    OnExit();

    return m_exitcode;
}

void wxGUIEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), "can't call Exit() if not running" );

    m_exitcode = rc;

    gtk_main_quit();
}

void wxGUIEventLoop::WakeUp()
{
    // TODO: idle events handling should really be done by wxEventLoop itself
    //       but for now it's completely in gtk/app.cpp so just call there when
    //       we have wxTheApp and hope that it doesn't matter that we do
    //       nothing when we don't...
    if ( wxTheApp )
        wxTheApp->WakeUpIdle();
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxGUIEventLoop::Pending() const
{
    if ( wxTheApp )
    {
        // this avoids false positives from our idle source
        return wxTheApp->EventsPending();
    }

    return gtk_events_pending() != 0;
}

bool wxGUIEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), false, _T("can't call Dispatch() if not running") );

    // gtk_main_iteration() returns TRUE only if gtk_main_quit() was called
    return !gtk_main_iteration();
}

extern "C" {
static gboolean wx_event_loop_timeout(void* data)
{
    bool* expired = static_cast<bool*>(data);
    *expired = true;

    // return FALSE to remove this timeout
    return FALSE;
}
}

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    bool expired = false;
    const unsigned id = g_timeout_add(timeout, wx_event_loop_timeout, &expired);
    bool quit = gtk_main_iteration() != 0;

    if ( expired )
        return -1;

    g_source_remove(id);

    return !quit;
}


