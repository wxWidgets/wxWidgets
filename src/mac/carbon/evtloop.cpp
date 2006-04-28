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

#ifdef __DARWIN__
    #include <Carbon/Carbon.h>
#else
    #include <Carbon.h>
#endif
// ============================================================================
// wxEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// high level functions for RunApplicationEventLoop() case
// ----------------------------------------------------------------------------

#if wxMAC_USE_RUN_APP_EVENT_LOOP

int wxEventLoop::Run()
{
    wxEventLoopActivator activate(this);

    RunApplicationEventLoop();

    return m_exitcode;
}

void wxEventLoop::Exit(int rc)
{
    m_exitcode = rc;

    QuitApplicationEventLoop();

    OnExit();
}

#else // manual event loop

// ----------------------------------------------------------------------------
// functions only used by wxEventLoopManual-based implementation
// ----------------------------------------------------------------------------

void wxEventLoop::WakeUp()
{
    extern void wxMacWakeUp();

    wxMacWakeUp();
}

#endif // high/low-level event loop

// ----------------------------------------------------------------------------
// low level functions used in both cases
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
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

bool wxEventLoop::Dispatch()
{
    // TODO: we probably should do the dispatching directly from here but for
    //       now it's easier to forward to wxApp which has all the code to do
    //       it
    if ( !wxTheApp )
        return false;

    wxTheApp->MacDoOneEvent();
    return true;
}
