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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxGUIEventLoop::wxGUIEventLoop()
{
}

static void DispatchAndReleaseEvent(EventRef theEvent)
{
    if ( wxTheApp )
        wxTheApp->MacSetCurrentEvent( theEvent, NULL );

    OSStatus status = SendEventToEventTarget(theEvent, GetEventDispatcherTarget());
    if (status == eventNotHandledErr && wxTheApp)
        wxTheApp->MacHandleUnhandledEvent(theEvent);

    ReleaseEvent( theEvent );
}

int wxGUIEventLoop::DoDispatchTimeout(unsigned long timeout)
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
            // according to QA1061 this may also occur
            // when a WakeUp Process is executed
            return 0;

        case noErr:
            DispatchAndReleaseEvent(event);
            return 1;
    }
}
