///////////////////////////////////////////////////////////////////////////////
// Name:        x11/evtloop.cpp
// Purpose:     implements wxEventLoop for X11
// Author:      Julian Smart
// Modified by:
// Created:     01.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Julian Smart
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "evtloop.h"
#endif

#include "wx/window.h"
#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/tooltip.h"
#if wxUSE_THREADS
#include "wx/thread.h"
#endif
#include "wx/timer.h"
#include "wx/x11/private.h"
#include "X11/Xlib.h"

#include <sys/time.h>
#include <unistd.h>

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl() { SetExitCode(0); m_keepGoing = FALSE; }

    // process an XEvent, return TRUE if it was processed
    bool ProcessEvent(XEvent* event);

    // generate an idle message, return TRUE if more idle time requested
    bool SendIdleEvent();

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

public:
    // preprocess an event, return TRUE if processed (i.e. no further
    // dispatching required)
    bool PreProcessEvent(XEvent* event);

    // the exit code of the event loop
    int m_exitcode;

    bool m_keepGoing;
};

// ============================================================================
// wxEventLoopImpl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoopImpl message processing
// ----------------------------------------------------------------------------

bool wxEventLoopImpl::ProcessEvent(XEvent *event)
{
    // give us the chance to preprocess the message first
    if ( PreProcessEvent(event) )
        return TRUE;
    
    // if it wasn't done, dispatch it to the corresponding window
    if (wxTheApp)
        return wxTheApp->ProcessXEvent((WXEvent*) event);

    return FALSE;
}

bool wxEventLoopImpl::PreProcessEvent(XEvent *event)
{
    // TODO
#if 0
    HWND hWnd = msg->hwnd;
    wxWindow *wndThis = wxGetWindowFromHWND((WXHWND)hWnd);


    // try translations first; find the youngest window with a translation
    // table.
    wxWindow *wnd;
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWTranslateMessage((WXMSG *)msg) )
            return TRUE;
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWProcessMessage((WXMSG *)msg) )
            return TRUE;
    }
#endif

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxEventLoopImpl idle event processing
// ----------------------------------------------------------------------------

bool wxEventLoopImpl::SendIdleEvent()
{
    wxIdleEvent event;
    event.SetEventObject(wxTheApp);

    return wxTheApp->ProcessEvent(event) && event.MoreRequested();
}

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxEventLoop *wxEventLoop::ms_activeLoop = NULL;

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

bool wxEventLoop::IsRunning() const
{
    return m_impl != NULL;
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    m_impl = new wxEventLoopImpl;
    
    wxEventLoop *oldLoop = ms_activeLoop;
    ms_activeLoop = this;

    m_impl->m_keepGoing = TRUE;
    while ( m_impl->m_keepGoing )
    {
#if 0 // wxUSE_THREADS
	wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        // generate and process idle events for as long as we don't have
        // anything else to do
        while ( ! Pending() )
        {
#if wxUSE_TIMER
            wxTimer::NotifyTimers(); // TODO: is this the correct place for it?
#endif
            if (!m_impl->SendIdleEvent())
            {
#if wxUSE_THREADS
                // leave the main loop to give other threads a chance to
                // perform their GUI work
                wxMutexGuiLeave();
                wxUsleep(20);
                wxMutexGuiEnter();
#endif
                // Break out of while loop
                break;
            }
        }

        // a message came or no more idle processing to do, sit in Dispatch()
        // waiting for the next message
        if ( !Dispatch() )
        {
            break;
        }
    }

    int exitcode = m_impl->GetExitCode();
    delete m_impl;
    m_impl = NULL;

    ms_activeLoop = oldLoop;

    return exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_impl->SetExitCode(rc);
    m_impl->m_keepGoing = FALSE;
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    XFlush((Display*) wxGetDisplay());
    return (XPending((Display*) wxGetDisplay()) > 0);
}

bool wxEventLoop::Dispatch()
{
    XEvent event;

    // TODO allowing for threads, as per e.g. wxMSW

#if 0
    XNextEvent((Display*) wxGetDisplay(), & event);
#endif

    // This now waits until either an X event is received,
    // or the select times out. So we should now process
    // wxTimers in a reasonably timely fashion. However it
    // does also mean that idle processing will happen more
    // often, so we should probably limit idle processing to
    // not be repeated more than every N milliseconds.
    
    if (XPending((Display*) wxGetDisplay()) == 0)
    {
#if wxUSE_NANOX
        GR_TIMEOUT timeout = 10; // Milliseconds
        // Wait for next event, or timeout
        GrGetNextEventTimeout(& event, timeout);

        // Fall through to ProcessEvent.
        // we'll assume that ProcessEvent will just ignore
        // the event if there was a timeout and no event.
            
#else
        struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=10000; // TODO make this configurable
        int fd = ConnectionNumber((Display*) wxGetDisplay());
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(fd, &readset);
        if (select(fd+1, &readset, NULL, NULL, & tv) == 0)
        {
            // Timed out, so no event to process
            return TRUE;
        }
        else
        {
            // An event was pending, so get it
            XNextEvent((Display*) wxGetDisplay(), & event);
        }
#endif
    } else
    {
       XNextEvent((Display*) wxGetDisplay(), & event);
    }
    
    (void) m_impl->ProcessEvent(& event);
    return TRUE;
}

