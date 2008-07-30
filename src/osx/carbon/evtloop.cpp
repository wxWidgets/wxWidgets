///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/evtloop.cpp
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

#if wxOSX_USE_CARBON
    #include <Carbon/Carbon.h>
#else
    #include <CoreFoundation/CoreFoundation.h>
#endif
// ============================================================================
// wxEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// high level functions for RunApplicationEventLoop() case
// ----------------------------------------------------------------------------



#if wxOSX_USE_RUN_APP_EVENT_LOOP

int wxGUIEventLoop::Run()
{
    wxEventLoopActivator activate(this);

    RunApplicationEventLoop();

    return m_exitcode;
}

void wxGUIEventLoop::Exit(int rc)
{
    m_exitcode = rc;

    QuitApplicationEventLoop();

    OnExit();
}

#else // manual event loop

// ----------------------------------------------------------------------------
// functions only used by wxEventLoopManual-based implementation
// ----------------------------------------------------------------------------

void wxGUIEventLoop::WakeUp()
{
    extern void wxMacWakeUp();

    wxMacWakeUp();
}

#endif // high/low-level event loop

// ----------------------------------------------------------------------------
// low level functions used in both cases
// ----------------------------------------------------------------------------

bool wxGUIEventLoop::Pending() const
{
#if wxOSX_USE_CARBON
    EventRef theEvent;

    return ReceiveNextEvent
           (
            0,          // we want any event at all so we don't specify neither
            NULL,       // the number of event types nor the types themselves
            kEventDurationNoWait,
            false,      // don't remove the event from queue
            &theEvent
           ) == noErr;
#else
    return true; // TODO
#endif
}

bool wxGUIEventLoop::Dispatch()
{
    if ( !wxTheApp )
        return false;

#if wxOSX_USE_CARBON
    // TODO: we probably should do the dispatching directly from here but for
    //       now it's easier to forward to wxApp which has all the code to do
    //       it
    wxTheApp->MacDoOneEvent();
#else
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, 0);
#endif
    return true;
}
