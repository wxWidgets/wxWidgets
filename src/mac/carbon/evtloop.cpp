///////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/evtloop.cpp
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

#include <Carbon/Carbon.h>

// ============================================================================
// implementation
// ============================================================================

wxEventLoop::wxEventLoop()
{
    m_exitcode = 0;
#if !wxMAC_USE_RUN_APP_EVENT_LOOP
    m_shouldExit = false;
#endif
}

int wxEventLoop::Run()
{
    wxEventLoopActivator activate(this);

#if wxMAC_USE_RUN_APP_EVENT_LOOP
    RunApplicationEventLoop();
#else // manual event loop
    while ( !m_shouldExit )
    {
        Dispatch();
    }
#endif // auto/manual event loop

    return m_exitcode;
}

void wxEventLoop::Exit(int rc)
{
    m_exitcode = rc;

#if wxMAC_USE_RUN_APP_EVENT_LOOP
    QuitApplicationEventLoop();
#else // manual event loop
    m_shouldExit = true;
#endif // auto/manual event loop
}

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

