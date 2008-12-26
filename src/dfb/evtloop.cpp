/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/evtloop.cpp
// Purpose:     wxEventLoop implementation
// Author:      Vaclav Slavik
// Created:     2006-08-16
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/thread.h"
#include "wx/generic/private/timer.h"
#include "wx/private/fdiodispatcher.h"
#include "wx/dfb/private.h"
#include "wx/nonownedwnd.h"

#define TRACE_EVENTS "events"

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxEventLoop initialization
//-----------------------------------------------------------------------------

wxIDirectFBEventBufferPtr wxGUIEventLoop::ms_buffer;

wxGUIEventLoop::wxGUIEventLoop()
{
    if ( !ms_buffer )
        InitBuffer();
}

/* static */
void wxGUIEventLoop::InitBuffer()
{
    ms_buffer = wxIDirectFB::Get()->CreateEventBuffer();
}

/* static */
void wxGUIEventLoop::CleanUp()
{
    ms_buffer.Reset();
}

/* static */
wxIDirectFBEventBufferPtr wxGUIEventLoop::GetDirectFBEventBuffer()
{
    if ( !ms_buffer )
        InitBuffer();

    return ms_buffer;
}

//-----------------------------------------------------------------------------
// events dispatch and loop handling
//-----------------------------------------------------------------------------

bool wxGUIEventLoop::Pending() const
{
    wxCHECK_MSG( ms_buffer, false, "invalid event buffer" );

    return ms_buffer->HasEvent();
}

bool wxGUIEventLoop::Dispatch()
{
    // NB: we don't block indefinitely waiting for an event, but instead
    //     time out after a brief period in order to make sure that
    //     OnNextIteration() will be called frequently enough
    //
    // TODO: remove this hack, instead use CreateFileDescriptor() to properly
    //       multiplex GUI and socket input
    const int TIMEOUT = 100;

    // treat time out (-1 return value) as normal successful return so that
    // OnNextIteration() is called
    return !!DispatchTimeout(TIMEOUT);
}

int wxGUIEventLoop::DispatchTimeout(unsigned long timeout)
{
    wxCHECK_MSG( ms_buffer, 0, "invalid event buffer" );

    // release the GUI mutex so that other threads have a chance to post
    // events:
    wxMutexGuiLeave();

    bool rv = ms_buffer->WaitForEventWithTimeout(0, timeout);

    // and acquire it back before calling any event handlers:
    wxMutexGuiEnter();

    if ( rv )
    {
        switch ( ms_buffer->GetLastResult() )
        {
            case DFB_OK:
            {
                wxDFBEvent e;
                ms_buffer->GetEvent(e);
                HandleDFBEvent(e);
                break;
            }

            case DFB_TIMEOUT:
                return -1;

            default:
                // don't terminate the loop due to errors (they were reported
                // already by ms_buffer)
                break;
        }
    }

    return 1;
}

void wxGUIEventLoop::WakeUp()
{
    wxCHECK_RET( ms_buffer, "invalid event buffer" );

    ms_buffer->WakeUp();
}

void wxGUIEventLoop::OnNextIteration()
{
#if wxUSE_TIMER
    wxGenericTimerImpl::NotifyTimers();
#endif

#if wxUSE_SOCKETS
    // handle any pending socket events:
    wxFDIODispatcher::DispatchPending();
#endif
}

void wxGUIEventLoop::Yield()
{
    // process all pending events:
    while ( Pending() )
        Dispatch();

    // handle timers, sockets etc.
    OnNextIteration();
}


//-----------------------------------------------------------------------------
// DirectFB -> wxWidgets events translation
//-----------------------------------------------------------------------------

void wxGUIEventLoop::HandleDFBEvent(const wxDFBEvent& event)
{
    switch ( event.GetClass() )
    {
        case DFEC_WINDOW:
        {
            wxDFBWindowEvent winevent(((const DFBEvent&)event).window);
            wxNonOwnedWindow::HandleDFBWindowEvent(winevent);
            break;
        }

        case DFEC_NONE:
        case DFEC_INPUT:
        case DFEC_USER:
#if wxCHECK_DFB_VERSION(0,9,23)
        case DFEC_UNIVERSAL:
#endif
        {
            wxLogTrace(TRACE_EVENTS,
                       "ignoring event of unsupported class %i",
                       (int)event.GetClass());
        }
    }
}
