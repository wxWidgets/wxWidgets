///////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/evtloop.cpp
// Purpose:     implements wxEventLoop for Symbian
// Author:      Andrei Matuk <Veon.UA@gmail.com>
// RCS-ID:      $Id$
// Copyright:   (c) Andrei Matuk
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"
#include "wx/apptrait.h"
#include "wx/except.h"
#include "wx/ptr_scpd.h"

// ============================================================================
// wxEventLoop implementation
// ============================================================================

#if wxUSE_CONSOLE_EVENTLOOP
// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxConsoleEventLoop::wxConsoleEventLoop()
{
}

wxConsoleEventLoop::~wxConsoleEventLoop()
{
}

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------
/*
bool wxEventLoopBase::IsRunning() const
{
    // Is isn't really running as such in Symbian, but is called as if it is through
    // OS idle events.
    return true;
}

int wxConsoleEventLoop::Run()
{
    return 0;
}

void wxConsoleEventLoop::Exit(int rc)
{
}
*/
// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxConsoleEventLoop::Pending() const
{
    return false;
}

bool wxConsoleEventLoop::Dispatch()
{
    return false;
}

int wxConsoleEventLoop::DispatchTimeout(unsigned long timeout)
{
    return 0;
}

void wxConsoleEventLoop::WakeUp()
{
}

wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
    return new wxConsoleEventLoop();
}

#endif // wxUSE_CONSOLE_EVENTLOOP
