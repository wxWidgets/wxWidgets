///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/evtloop.cpp
// Purpose:     implements wxEventLoop for Palm OS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10.14.04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "evtloop.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"

#include "wx/tooltip.h"
#include "wx/except.h"
#include "wx/ptr_scpd.h"

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the array of MSG strutures
    WX_DECLARE_OBJARRAY(MSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// helper class
// ----------------------------------------------------------------------------

// this object sets the wxEventLoop given to the ctor as the currently active
// one and unsets it in its dtor
class wxEventLoopActivator
{
public:
    wxEventLoopActivator(wxEventLoop **pActive,
                         wxEventLoop *evtLoop)
    {
        m_pActive = pActive;
        m_evtLoopOld = *pActive;
        *pActive = evtLoop;
    }

    ~wxEventLoopActivator()
    {
        // restore the previously active event loop
        *m_pActive = m_evtLoopOld;
    }

private:
    wxEventLoop *m_evtLoopOld;
    wxEventLoop **m_pActive;
};

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxEventLoop *wxEventLoopBase::ms_activeLoop = NULL;

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxEventLoop::wxEventLoop()
{
    m_shouldExit = false;
    m_exitcode = 0;
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing
// ----------------------------------------------------------------------------

void wxEventLoop::ProcessMessage(WXMSG *msg)
{
}

bool wxEventLoop::PreProcessMessage(WXMSG *msg)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

bool wxEventLoop::IsRunning() const
{
    return true;
}

int wxEventLoop::Run()
{
    status_t    error;
    EventType    event;

    do {
        wxTheApp && wxTheApp->ProcessIdle();

        EvtGetEvent(&event, evtWaitForever);

        if (SysHandleEvent(&event))
            continue;

        if (MenuHandleEvent(0, &event, &error))
            continue;

        FrmDispatchEvent(&event);

    } while (event.eType != appStopEvent);

    return 0;
}

void wxEventLoop::Exit(int rc)
{
    FrmCloseAllForms();

    EventType AppStop;
    AppStop.eType=appStopEvent;
    EvtAddEventToQueue(&AppStop);
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    return false;
}

bool wxEventLoop::Dispatch()
{
    return false;
}

