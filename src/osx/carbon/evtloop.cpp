///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/evtloop.cpp
// Purpose:     implementation of wxEventLoop for wxMac
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxGUIEventLoop::wxGUIEventLoop()
{
    m_sleepTime = kEventDurationNoWait;
}

void wxGUIEventLoop::WakeUp()
{
    extern void wxMacWakeUp();

    wxMacWakeUp();
}

void wxGUIEventLoop::DispatchAndReleaseEvent(EventRef theEvent)
{
    if ( wxTheApp )
        wxTheApp->MacSetCurrentEvent( theEvent, NULL );

    OSStatus status = SendEventToEventTarget(theEvent, GetEventDispatcherTarget());
    if (status == eventNotHandledErr && wxTheApp)
        wxTheApp->MacHandleUnhandledEvent(theEvent);

    ReleaseEvent( theEvent );
}

bool wxGUIEventLoop::Pending() const
{
    EventRef theEvent;

    return ReceiveNextEvent
           (
            0,          // we want any event at all so we don't specify neither
            NULL,       // the number of event types nor the types themselves
            kEventDurationNoWait,
            false,      // don't remove the event from queue
            &theEvent
           ) == noErr;
}

bool wxGUIEventLoop::Dispatch()
{
    if ( !wxTheApp )
        return false;

    wxMacAutoreleasePool autoreleasepool;

    EventRef theEvent;

    OSStatus status = ReceiveNextEvent(0, NULL, m_sleepTime, true, &theEvent) ;

    switch (status)
    {
        case eventLoopTimedOutErr :
            if ( wxTheApp->ProcessIdle() )
                m_sleepTime = kEventDurationNoWait ;
            else
            {
                m_sleepTime = kEventDurationSecond;
#if wxUSE_THREADS
                wxMutexGuiLeave();
                wxMilliSleep(20);
                wxMutexGuiEnter();
#endif
            }
            break;

        case eventLoopQuitErr :
            // according to QA1061 this may also occur
            // when a WakeUp Process is executed
            break;

        default:
            DispatchAndReleaseEvent(theEvent);
            m_sleepTime = kEventDurationNoWait ;
            break;
    }

    return true;
}

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    EventRef event;
    OSStatus status = ReceiveNextEvent(0, NULL, timeout/1000, true, &event);
    switch ( status )
    {
        default:
            wxFAIL_MSG( "unexpected ReceiveNextEvent() error" );
            // fall through

        case eventLoopTimedOutErr:
            return -1;

        case eventLoopQuitErr:
            return 0;

        case noErr:
            DispatchAndReleaseEvent(event);
            return 1;
    }
}

bool wxGUIEventLoop::YieldFor(long eventsToProcess)
{
#if wxUSE_THREADS
    // Yielding from a non-gui thread needs to bail out, otherwise we end up
    // possibly sending events in the thread too.
    if ( !wxThread::IsMain() )
    {
        return true;
    }
#endif // wxUSE_THREADS

    m_isInsideYield = true;
    m_eventsToProcessInsideYield = eventsToProcess;

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();
#endif // wxUSE_LOG

    // process all pending events:
    while ( Pending() )
        Dispatch();

    // it's necessary to call ProcessIdle() to update the frames sizes which
    // might have been changed (it also will update other things set from
    // OnUpdateUI() which is a nice (and desired) side effect)
    while ( ProcessIdle() ) {}

#if wxUSE_LOG
    wxLog::Resume();
#endif // wxUSE_LOG
    m_isInsideYield = false;

    return true;
}
