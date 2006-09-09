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
// wxEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    wxEventLoopActivator activate(this);

    m_impl = new wxEventLoopImpl;

    gtk_main();

    int exitcode = m_impl->GetExitCode();
    delete m_impl;
    m_impl = NULL;

    return exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_impl->SetExitCode(rc);

    gtk_main_quit();
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    if (wxTheApp)
    {
        // We need to remove idle callbacks or gtk_events_pending will
        // never return false.
        wxTheApp->RemoveIdleSource();
    }

    return gtk_events_pending();
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), false, _T("can't call Dispatch() if not running") );

    gtk_main_iteration();

    return true;
}
