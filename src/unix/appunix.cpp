/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/appunix.cpp
// Purpose:     wxAppConsole with wxMainLoop implementation
// Author:      Lukasz Michalski
// Created:     28/01/2005
// RCS-ID:      $Id$
// Copyright:   (c) Lukasz Michalski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/log.h"
#include "wx/evtloop.h"

#include <signal.h>
#include <unistd.h>

bool wxAppConsoleUnix::Initialize(int& argc, wxChar** argv)
{
    if ( !wxAppConsole::Initialize(argc,argv) )
        return false;

    if ( !m_mainLoop->IsOk() )
        return false;

    sigemptyset(&m_signalsCaught);

    return true;
}

void wxAppConsoleUnix::HandleSignal(int signal)
{
    wxAppConsoleUnix * const app = wxTheApp;
    if ( !app )
        return;

    sigaddset(&(app->m_signalsCaught), signal);
    app->WakeUpIdle();
}

void wxAppConsoleUnix::CheckSignal()
{
    for ( SignalHandlerHash::iterator it = m_signalHandlerHash.begin();
          it != m_signalHandlerHash.end();
          ++it )
    {
        int sig = it->first;
        if ( sigismember(&m_signalsCaught, sig) )
        {
            sigdelset(&m_signalsCaught, sig);
            (it->second)(sig);
        }
    }
}

bool wxAppConsoleUnix::SetSignalHandler(int signal, SignalHandler handler)
{
    const bool install = handler != SIG_DFL && handler != SIG_IGN;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &wxAppConsoleUnix::HandleSignal;
    sa.sa_flags = SA_RESTART;
    int res = sigaction(signal, &sa, 0);
    if ( res != 0 )
    {
        wxLogSysError(_("Failed to install signal handler"));
        return false;
    }

    if ( install )
        m_signalHandlerHash[signal] = handler;
    else
        m_signalHandlerHash.erase(signal);

    return true;
}
