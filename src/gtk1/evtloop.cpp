///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/evtloop.cpp
// Purpose:     implements wxEventLoop for GTK+
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.07.01
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
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
#include "wx/private/eventloopsourcesmanager.h"
#include "wx/apptrait.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl() { SetExitCode(0); }

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

private:
    // the exit code of the event loop
    int m_exitcode;
};

// ============================================================================
// wxGUIEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGUIEventLoop running and exiting
// ----------------------------------------------------------------------------

wxGUIEventLoop::~wxGUIEventLoop()
{
    wxASSERT_MSG( !m_impl, wxT("should have been deleted in Run()") );
}

int wxGUIEventLoop::DoRun()
{
    m_impl = new wxEventLoopImpl;

    guint loopLevel = gtk_main_level();

    // This is placed inside of a loop to take into account nested
    // event loops.  For example, inside this event loop, we may recieve
    // Exit() for a different event loop (which we are currently inside of)
    // That Exit() will cause this gtk_main() to exit so we need to re-enter it.
#if 0
   // changed by JJ
   // this code was intended to support nested event loops. However,
   // exiting a dialog will result in a application hang (because
   // gtk_main_quit is called when closing the dialog????)
   // So for the moment this code is disabled and nested event loops
   // probably fail for wxGTK1
   while ( !m_shouldExit )
    {
#endif
       gtk_main();
#if 0
    }

    // Force the enclosing event loop to also exit to see if it is done
    // in case that event loop ended inside of this one.  If it is not time
    // yet for that event loop to exit, it will be executed again due to
    // the while() loop on m_shouldExit().
    //
    // This is unnecessary if we are the top level loop, i.e. loop of level 0.
    if ( loopLevel )
    {
        gtk_main_quit();
    }
#endif

    OnExit();

    int exitcode = m_impl->GetExitCode();
    wxDELETE(m_impl);

    return exitcode;
}

void wxGUIEventLoop::ScheduleExit(int rc)
{
    wxCHECK_RET( IsInsideRun(), wxT("can't call ScheduleExit() if not started") );

    m_impl->SetExitCode(rc);

    m_shouldExit = true;

    gtk_main_quit();
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxGUIEventLoop::Pending() const
{
    if (wxTheApp)
    {
        // We need to remove idle callbacks or gtk_events_pending will
        // never return false.
        wxTheApp->RemoveIdleTag();
    }

    return gtk_events_pending();
}

bool wxGUIEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), false, wxT("can't call Dispatch() if not running") );

    gtk_main_iteration();

    return true;
}

//-----------------------------------------------------------------------------
// wxYield
//-----------------------------------------------------------------------------

void wxGUIEventLoop::DoYieldFor(long eventsToProcess)
{
    // We need to remove idle callbacks or the loop will
    // never finish.
    wxTheApp->RemoveIdleTag();

    // TODO: implement event filtering using the eventsToProcess mask
    while (gtk_events_pending())
        gtk_main_iteration();

    wxEventLoopBase::DoYieldFor(eventsToProcess);
}

class wxGUIEventLoopSourcesManager : public wxEventLoopSourcesManagerBase
{
 public:
    wxEventLoopSource *
    AddSourceForFD(int WXUNUSED(fd),
                   wxEventLoopSourceHandler* WXUNUSED(handler),
                   int WXUNUSED(flags))
    {
        wxFAIL_MSG("Monitoring FDs in the main loop is not implemented in wxGTK1");

        return NULL;
    }
};

wxEventLoopSourcesManagerBase* wxGUIAppTraits::GetEventLoopSourcesManager()
{
    static wxGUIEventLoopSourcesManager s_eventLoopSourcesManager;

    return &s_eventLoopSourcesManager;
}
