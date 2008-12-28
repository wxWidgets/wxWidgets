///////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/evtloop.mm
// Purpose:     implements wxEventLoop for Cocoa
// Author:      David Elliott
// Modified by:
// Created:     2003/10/02
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott <dfe@cox.net>
// License:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <Foundation/NSRunLoop.h>

// ========================================================================
// wxGUIEventLoop
// ========================================================================

// ----------------------------------------------------------------------------
// wxGUIEventLoop running and exiting
// ----------------------------------------------------------------------------

int wxGUIEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    wxEventLoopActivator activate(this);

    [[NSApplication sharedApplication] run];

    OnExit();

    return m_exitcode;
}

void wxGUIEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_exitcode = rc;

    NSApplication *cocoaApp = [NSApplication sharedApplication];
    wxLogTrace(wxTRACE_COCOA,wxT("wxEventLoop::Exit isRunning=%d"), (int)[cocoaApp isRunning]);
    wxTheApp->WakeUpIdle();
    /* Notes:
    If we're being called from idle time (which occurs while checking the
    queue for new events) there may or may not be any events in the queue.
    In order to successfully stop the event loop, at least one event must
    be processed.  To ensure this always happens, WakeUpIdle is called.

    If the application was active when closed then this is unnecessary
    because it would receive a deactivate event anyway.  However, if the
    application was not active when closed, then no events would be
    added to the queue by Cocoa and thus the application would wait
    indefinitely for the next event.
    */
    [cocoaApp stop: cocoaApp];
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxGUIEventLoop::Pending() const
{
    // a pointer to the event is returned if there is one, or nil if not
    return [[NSApplication sharedApplication]
            nextEventMatchingMask: NSAnyEventMask
            untilDate: nil /* Equivalent to [NSDate distantPast] */
            inMode: NSDefaultRunLoopMode
            dequeue: NO];
}

bool wxGUIEventLoop::Dispatch()
{
    // This check is required by wxGTK but probably not really for wxCocoa
    // Keep it here to encourage developers to write cross-platform code
    wxCHECK_MSG( IsRunning(), false, _T("can't call Dispatch() if not running") );
    NSApplication *cocoaApp = [NSApplication sharedApplication];
    // Block to retrieve an event then send it
    if(NSEvent *event = [cocoaApp
                nextEventMatchingMask:NSAnyEventMask
                untilDate:[NSDate distantFuture]
                inMode:NSDefaultRunLoopMode
                dequeue: YES])
    {
        [cocoaApp sendEvent: event];
    }

    return true;
}

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    NSApplication *cocoaApp = [NSApplication sharedApplication];
    NSEvent *event = [cocoaApp
                nextEventMatchingMask:NSAnyEventMask
                untilDate:[[NSDate alloc] initWithTimeIntervalSinceNow:timeout/1000]
                inMode:NSDefaultRunLoopMode
                dequeue: YES];
    if ( !event )
        return -1;

    [cocoaApp sendEvent: event];

    return true;
}

