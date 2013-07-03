/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/evtloopunix.cpp
// Purpose:     wxEventLoop implementation
// Author:      Lukasz Michalski (lm@zork.pl)
// Created:     2007-05-07
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Zork Lukasz Michalski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CONSOLE_EVENTLOOP

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif

#include "wx/apptrait.h"
#include "wx/scopedptr.h"
#include "wx/thread.h"
#include "wx/module.h"
#include "wx/unix/private/timer.h"
#include "wx/unix/private/epolldispatcher.h"
#include "wx/unix/private/wakeuppipe.h"
#include "wx/private/selectdispatcher.h"
#include "wx/private/fdioeventloopsourcehandler.h"

#if wxUSE_EVENTLOOP_SOURCE
    #include "wx/evtloopsrc.h"
#endif // wxUSE_EVENTLOOP_SOURCE

// ===========================================================================
// wxEventLoop implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

wxConsoleEventLoop::wxConsoleEventLoop()
{
    m_wakeupPipe = new wxWakeUpPipeMT;
    const int pipeFD = m_wakeupPipe->GetReadFd();
    if ( pipeFD == wxPipe::INVALID_FD )
    {
        wxDELETE(m_wakeupPipe);
        m_dispatcher = NULL;
        return;
    }

    m_dispatcher = wxFDIODispatcher::Get();
    if ( !m_dispatcher )
        return;

    m_dispatcher->RegisterFD(pipeFD, m_wakeupPipe, wxFDIO_INPUT);
}

wxConsoleEventLoop::~wxConsoleEventLoop()
{
    if ( m_wakeupPipe )
    {
        if ( m_dispatcher )
        {
            m_dispatcher->UnregisterFD(m_wakeupPipe->GetReadFd());
        }

        delete m_wakeupPipe;
    }
}

//-----------------------------------------------------------------------------
// adding & removing sources
//-----------------------------------------------------------------------------

#if wxUSE_EVENTLOOP_SOURCE

wxEventLoopSource *
wxConsoleEventLoop::AddSourceForFD(int fd,
                                   wxEventLoopSourceHandler *handler,
                                   int flags)
{
    wxCHECK_MSG( fd != -1, NULL, "can't monitor invalid fd" );

    wxLogTrace(wxTRACE_EVT_SOURCE,
                "Adding event loop source for fd=%d", fd);

    // we need a bridge to wxFDIODispatcher
    //
    // TODO: refactor the code so that only wxEventLoopSourceHandler is used
    wxScopedPtr<wxFDIOHandler>
        fdioHandler(new wxFDIOEventLoopSourceHandler(handler));

    if ( !m_dispatcher->RegisterFD(fd, fdioHandler.get(), flags) )
        return NULL;

    return new wxUnixEventLoopSource(m_dispatcher, fdioHandler.release(),
                                     fd, handler, flags);
}

wxUnixEventLoopSource::~wxUnixEventLoopSource()
{
    wxLogTrace(wxTRACE_EVT_SOURCE,
               "Removing event loop source for fd=%d", m_fd);

    m_dispatcher->UnregisterFD(m_fd);

    delete m_fdioHandler;
}

#endif // wxUSE_EVENTLOOP_SOURCE

//-----------------------------------------------------------------------------
// events dispatch and loop handling
//-----------------------------------------------------------------------------

bool wxConsoleEventLoop::Pending() const
{
    if ( m_dispatcher->HasPending() )
        return true;

#if wxUSE_TIMER
    wxUsecClock_t nextTimer;
    if ( wxTimerScheduler::Get().GetNext(&nextTimer) &&
            !wxMilliClockToLong(nextTimer) )
        return true;
#endif // wxUSE_TIMER

    return false;
}

bool wxConsoleEventLoop::Dispatch()
{
    DispatchTimeout(static_cast<unsigned long>(
        wxFDIODispatcher::TIMEOUT_INFINITE));

    return true;
}

int wxConsoleEventLoop::DispatchTimeout(unsigned long timeout)
{
#if wxUSE_TIMER
    // check if we need to decrease the timeout to account for a timer
    wxUsecClock_t nextTimer;
    if ( wxTimerScheduler::Get().GetNext(&nextTimer) )
    {
        unsigned long timeUntilNextTimer = wxMilliClockToLong(nextTimer / 1000);
        if ( timeUntilNextTimer < timeout )
            timeout = timeUntilNextTimer;
    }
#endif // wxUSE_TIMER

    bool hadEvent = m_dispatcher->Dispatch(timeout) > 0;

#if wxUSE_TIMER
    if ( wxTimerScheduler::Get().NotifyExpired() )
        hadEvent = true;
#endif // wxUSE_TIMER

    return hadEvent ? 1 : -1;
}

void wxConsoleEventLoop::WakeUp()
{
    m_wakeupPipe->WakeUp();
}

void wxConsoleEventLoop::OnNextIteration()
{
    // call the signal handlers for any signals we caught recently
    wxTheApp->CheckSignal();
}


wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

#endif // wxUSE_CONSOLE_EVENTLOOP
