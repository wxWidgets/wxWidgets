///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/evtloopcmn.cpp
// Purpose:     common wxEventLoop-related stuff
// Author:      Vadim Zeitlin
// Created:     2006-01-12
// Copyright:   (c) 2006, 2013 Vadim Zeitlin <vadim@wxwindows.org>
//              (c) 2013 Rob Bresalier
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/scopeguard.h"
#include "wx/apptrait.h"
#include "wx/private/eventloopsourcesmanager.h"

// Counts currently existing event loops.
//
// As wxEventLoop can be only used from the main thread, there is no need to
// protect accesses to this variable.
static int gs_eventLoopCount = 0;

// ----------------------------------------------------------------------------
// wxEventLoopBase
// ----------------------------------------------------------------------------

wxEventLoopBase *wxEventLoopBase::ms_activeLoop = NULL;

wxEventLoopBase::wxEventLoopBase()
{
    gs_eventLoopCount++;

    m_isInsideRun = false;
    m_shouldExit = false;
    m_yieldLevel = 0;
    m_eventsToProcessInsideYield = wxEVT_CATEGORY_ALL;
}

wxEventLoopBase::~wxEventLoopBase()
{
    gs_eventLoopCount--;
}

bool wxEventLoopBase::IsMain() const
{
    if (wxTheApp)
        return wxTheApp->GetMainLoop() == this;
    return false;
}

/* static */
void wxEventLoopBase::SetActive(wxEventLoopBase* loop)
{
    ms_activeLoop = loop;

    if (wxTheApp)
        wxTheApp->OnEventLoopEnter(loop);
}

int wxEventLoopBase::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsInsideRun(), -1, wxT("can't reenter a message loop") );

    // ProcessIdle() and ProcessEvents() below may throw so the code here should
    // be exception-safe, hence we must use local objects for all actions we
    // should undo
    wxEventLoopActivator activate(this);

    // We might be called again, after a previous call to ScheduleExit(), so
    // reset this flag.
    m_shouldExit = false;

    // Set this variable to true for the duration of this method.
    m_isInsideRun = true;
    wxON_BLOCK_EXIT_SET(m_isInsideRun, false);

    // Finally really run the loop.
    return DoRun();
}

void wxEventLoopBase::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), wxS("Use ScheduleExit() on not running loop") );

    ScheduleExit(rc);
}
void wxEventLoopBase::OnExit()
{
    if (wxTheApp)
        wxTheApp->OnEventLoopExit(this);
}

bool wxEventLoopBase::ProcessIdle()
{
    return wxTheApp && wxTheApp->ProcessIdle();
}

bool wxEventLoopBase::Yield(bool onlyIfNeeded)
{
    if ( onlyIfNeeded && IsYielding() )
        return false;

    return YieldFor(wxEVT_CATEGORY_ALL);
}

bool wxEventLoopBase::YieldFor(long eventsToProcess)
{
#if wxUSE_THREADS
    if ( !wxThread::IsMain() )
    {
        // Don't ever dispatch events from non-main threads.
        return false;
    }
#endif // wxUSE_THREADS

    // set the flag and don't forget to reset it before returning
    const int yieldLevelOld = m_yieldLevel;
    const long eventsToProcessOld = m_eventsToProcessInsideYield;

    m_yieldLevel++;
    wxON_BLOCK_EXIT_SET(m_yieldLevel, yieldLevelOld);

    m_eventsToProcessInsideYield = eventsToProcess;
    wxON_BLOCK_EXIT_SET(m_eventsToProcessInsideYield, eventsToProcessOld);

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();

    // ensure the logs will be flashed again when we exit
    wxON_BLOCK_EXIT0(wxLog::Resume);
#endif

    DoYieldFor(eventsToProcess);

#if wxUSE_EXCEPTIONS
    // If any handlers called from inside DoYieldFor() threw exceptions, they
    // may have been stored for later rethrow as it's unsafe to let them escape
    // from inside DoYieldFor() itself, as it calls native functions through
    // which the exceptions can't propagate. But now that we're back to our own
    // code, we may rethrow them.
    if ( wxTheApp )
        wxTheApp->RethrowStoredException();
#endif // wxUSE_EXCEPTIONS

    return true;
}

void wxEventLoopBase::DoYieldFor(long eventsToProcess)
{
    // Normally yielding dispatches not only the pending native events, but
    // also the events pending in wxWidgets itself and idle events.
    //
    // Notice however that we must not do it if we're asked to process only the
    // events of specific kind, as pending events could be of any kind at all
    // (ideal would be to have a filtering version of ProcessPendingEvents()
    // too but we don't have this right now) and idle events are typically
    // unexpected when yielding for the specific event kinds only.
    if ( eventsToProcess == wxEVT_CATEGORY_ALL )
    {
        if ( wxTheApp )
            wxTheApp->ProcessPendingEvents();

        // We call it just once, even if it returns true, because we don't want
        // to get stuck inside wxYield() forever if the application does some
        // constant background processing in its idle handler, we do need to
        // get back to the main loop soon.
        ProcessIdle();
    }
}

#if wxUSE_EVENTLOOP_SOURCE

wxEventLoopSource*
wxEventLoopBase::AddSourceForFD(int fd,
                                wxEventLoopSourceHandler *handler,
                                int flags)
{
#if wxUSE_CONSOLE_EVENTLOOP
    // Delegate to the event loop sources manager defined by it.
    wxEventLoopSourcesManagerBase* const
        manager = wxApp::GetValidTraits().GetEventLoopSourcesManager();
    wxCHECK_MSG( manager, NULL, wxS("Must have wxEventLoopSourcesManager") );

    return manager->AddSourceForFD(fd, handler, flags);
#else // !wxUSE_CONSOLE_EVENTLOOP
    return NULL;
#endif // wxUSE_CONSOLE_EVENTLOOP/!wxUSE_CONSOLE_EVENTLOOP
}

#endif // wxUSE_EVENTLOOP_SOURCE
// wxEventLoopManual is unused in the other ports
#if defined(__WINDOWS__) || defined(__WXDFB__) || ( ( defined(__UNIX__) && !defined(__WXOSX__) ) && wxUSE_BASE)

// ============================================================================
// wxEventLoopManual implementation
// ============================================================================

wxEventLoopManual::wxEventLoopManual()
{
    m_exitcode = 0;
}

bool wxEventLoopManual::ProcessEvents()
{
    // process pending wx events first as they correspond to low-level events
    // which happened before, i.e. typically pending events were queued by a
    // previous call to Dispatch() and if we didn't process them now the next
    // call to it might enqueue them again (as happens with e.g. socket events
    // which would be generated as long as there is input available on socket
    // and this input is only removed from it when pending event handlers are
    // executed)
    if ( wxTheApp )
    {
        wxTheApp->ProcessPendingEvents();

        // One of the pending event handlers could have decided to exit the
        // loop so check for the flag before trying to dispatch more events
        // (which could block indefinitely if no more are coming).
        if ( m_shouldExit )
            return false;
    }

    const bool res = Dispatch();

#if wxUSE_EXCEPTIONS
    // Rethrow any exceptions which could have been produced by the handlers
    // ran by Dispatch().
    if ( wxTheApp )
        wxTheApp->RethrowStoredException();
#endif // wxUSE_EXCEPTIONS

    return res;
}

int wxEventLoopManual::DoRun()
{

    // we must ensure that OnExit() is called even if an exception is thrown
    // from inside ProcessEvents() but we must call it from Exit() in normal
    // situations because it is supposed to be called synchronously,
    // wxModalEventLoop depends on this (so we can't just use ON_BLOCK_EXIT or
    // something similar here)
#if wxUSE_EXCEPTIONS
    for ( ;; )
    {
        try
        {
#endif // wxUSE_EXCEPTIONS

            // this is the event loop itself
            for ( ;; )
            {
                // give them the possibility to do whatever they want
                OnNextIteration();

                // generate and process idle events for as long as we don't
                // have anything else to do, but stop doing this if Exit() is
                // called by one of the idle handlers
                while ( !m_shouldExit && !Pending() && ProcessIdle() )
                    ;

                // if Exit() was called, don't dispatch any more events here
                if ( m_shouldExit )
                    break;

                // a message came or no more idle processing to do, dispatch
                // all the pending events and call Dispatch() to wait for the
                // next message
                if ( !ProcessEvents() || m_shouldExit )
                    break;
            }

            // Process any still pending events.
            for ( ;; )
            {
                bool hasMoreEvents = false;

                // We always dispatch events pending at wx level: it may be
                // important to do it before the loop exits and e.g. the modal
                // dialog possibly referenced by these events handlers is
                // destroyed. It also shouldn't result in the problems
                // described below for the native events and while there is
                // still a risk of never existing the loop due to an endless
                // stream of events generated from the user-defined event
                // handlers, we consider that well-behaved programs shouldn't
                // do this -- and if they do, it's better to keep running the
                // loop than crashing after leaving it.
                if ( wxTheApp && wxTheApp->HasPendingEvents() )
                {
                    wxTheApp->ProcessPendingEvents();
                    hasMoreEvents = true;
                }

                // For the underlying toolkit events, we only handle them when
                // exiting the outermost event loop but not when exiting nested
                // loops. This is required at least under MSW where, in case of
                // a nested modal event loop, the modality has already been
                // undone as Exit() had been already called, so all UI elements
                // are re-enabled and if we dispatched events from them here,
                // we could end up reentering the same event handler that had
                // shown the modal dialog in the first place and showing the
                // dialog second time before its first instance was destroyed,
                // resulting in a lot of fun.
                //
                // Also, unlike wx events above, it should be fine to dispatch
                // the native events from the outer event loop, as any events
                // generated from outside the dialog itself (necessarily, as
                // the dialog is already hidden and about to be destroyed)
                // shouldn't reference the dialog. Which is one of the reasons
                // we still dispatch them in the outermost event loop, to
                // ensure they're still processed. Another reason is that if we
                // do have an endless stream of native events, e.g. because we
                // have a timer with a too short interval, it's arguably better
                // to keep handling them instead of exiting.
                if ( gs_eventLoopCount == 1 )
                {
                    if ( Pending() )
                    {
                        Dispatch();
                        hasMoreEvents = true;
                    }
                }

                if ( !hasMoreEvents )
                    break;
            }
#if wxUSE_EXCEPTIONS
            // exit the outer loop as well
            break;
        }
        catch ( ... )
        {
            try
            {
                if ( !wxTheApp || !wxTheApp->OnExceptionInMainLoop() )
                {
                    OnExit();
                    break;
                }
                //else: continue running the event loop
            }
            catch ( ... )
            {
                // OnException() throwed, possibly rethrowing the same
                // exception again: very good, but we still need OnExit() to
                // be called
                OnExit();
                throw;
            }
        }
    }
#endif // wxUSE_EXCEPTIONS

    return m_exitcode;
}

void wxEventLoopManual::ScheduleExit(int rc)
{
    wxCHECK_RET( IsInsideRun(), wxT("can't call ScheduleExit() if not running") );

    m_exitcode = rc;
    m_shouldExit = true;

    OnExit();

    // all we have to do to exit from the loop is to (maybe) wake it up so that
    // it can notice that Exit() had been called
    //
    // in particular, do *not* use here calls such as PostQuitMessage() (under
    // MSW) which terminate the current event loop here because we're not sure
    // that it is going to be processed by the correct event loop: it would be
    // possible that another one is started and terminated by mistake if we do
    // this
    WakeUp();
}

#endif // __WINDOWS__ || __WXMAC__ || __WXDFB__

