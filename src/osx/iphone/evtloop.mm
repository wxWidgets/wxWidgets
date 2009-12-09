///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/evtloop.mm
// Purpose:     implementation of wxEventLoop for OS X
// Author:      Vadim Zeitlin, Stefan Csomor
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id: evtloop.cpp 54845 2008-07-30 14:52:41Z SC $
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

#include "wx/log.h"

#include "wx/osx/private.h"

// ============================================================================
// wxEventLoop implementation
// ============================================================================

/*
static int CalculateUIEventMaskFromEventCategory(wxEventCategory cat)
{
	NSLeftMouseDownMask	|
	NSLeftMouseUpMask |
	NSRightMouseDownMask |
	NSRightMouseUpMask		= 1 << NSRightMouseUp,
	NSMouseMovedMask		= 1 << NSMouseMoved,
	NSLeftMouseDraggedMask		= 1 << NSLeftMouseDragged,
	NSRightMouseDraggedMask		= 1 << NSRightMouseDragged,
	NSMouseEnteredMask		= 1 << NSMouseEntered,
	NSMouseExitedMask		= 1 << NSMouseExited,
        NSScrollWheelMask		= 1 << NSScrollWheel,
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
	NSTabletPointMask		= 1 << NSTabletPoint,
	NSTabletProximityMask		= 1 << NSTabletProximity,
#endif
	NSOtherMouseDownMask		= 1 << NSOtherMouseDown,
	NSOtherMouseUpMask		= 1 << NSOtherMouseUp,
	NSOtherMouseDraggedMask		= 1 << NSOtherMouseDragged,



	NSKeyDownMask			= 1 << NSKeyDown,
	NSKeyUpMask			= 1 << NSKeyUp,
	NSFlagsChangedMask		= 1 << NSFlagsChanged,

	NSAppKitDefinedMask		= 1 << NSAppKitDefined,
	NSSystemDefinedMask		= 1 << NSSystemDefined,
	UIApplicationDefinedMask	= 1 << UIApplicationDefined,
	NSPeriodicMask			= 1 << NSPeriodic,
	NSCursorUpdateMask		= 1 << NSCursorUpdate,

	NSAnyEventMask			= 0xffffffffU
}
*/

wxGUIEventLoop::wxGUIEventLoop()
{
    m_sleepTime = 0.0;
}

void wxGUIEventLoop::WakeUp()
{
    extern void wxMacWakeUp();

    wxMacWakeUp();
}

CFRunLoopRef wxGUIEventLoop::CFGetCurrentRunLoop() const
{
    return CFRunLoopGetCurrent();
}

bool wxGUIEventLoop::Pending() const
{
    wxMacAutoreleasePool autoreleasepool;
    // a pointer to the event is returned if there is one, or nil if not
    /*
    return [[UIApplication sharedApplication]
            nextEventMatchingMask: NSAnyEventMask
            untilDate: nil
            inMode: NSDefaultRunLoopMode
            dequeue: NO];
    */
    return false;
}

bool wxGUIEventLoop::Dispatch()
{
    if ( !wxTheApp )
        return false;

    wxMacAutoreleasePool autoreleasepool;

/*
    if(UIEvent *event = [[UIApplication sharedApplication]
                nextEventMatchingMask:NSAnyEventMask
                untilDate:[NSDate dateWithTimeIntervalSinceNow: m_sleepTime]
                inMode:NSDefaultRunLoopMode
                dequeue: YES])
    {
        m_sleepTime = 0.0;
        [[UIApplication sharedApplication] sendEvent: event];
    }
    else
*/
    {
        if ( wxTheApp->ProcessIdle() )
            m_sleepTime = 0.0 ;
        else
        {
            m_sleepTime = 1.0;
#if wxUSE_THREADS
            wxMutexGuiLeave();
            wxMilliSleep(20);
            wxMutexGuiEnter();
#endif
        }
    }

    return true;
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

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    wxMacAutoreleasePool autoreleasepool;
/*
    UIEvent *event = [[UIApplication sharedApplication]
                nextEventMatchingMask:NSAnyEventMask
                untilDate:[NSDate dateWithTimeIntervalSinceNow: timeout/1000]
                inMode:NSDefaultRunLoopMode
                dequeue: YES];
    if ( !event )
        return -1;

    [NSApp sendEvent: event];
*/
    return true;
}
