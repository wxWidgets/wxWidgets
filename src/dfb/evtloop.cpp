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

#include "wx/timer.h"
#include "wx/dfb/private.h"

#define TRACE_EVENTS _T("events")

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxEventLoop initialization
//-----------------------------------------------------------------------------

IDirectFBEventBufferPtr wxEventLoop::ms_buffer;

wxEventLoop::wxEventLoop()
{
    if ( !ms_buffer )
        InitBuffer();
}

/* static */
void wxEventLoop::InitBuffer()
{
    IDirectFBPtr dfb(wxTheApp->GetDirectFBInterface());
    DFB_CALL( dfb->CreateEventBuffer(dfb, &ms_buffer) );
}

/* static */
IDirectFBEventBufferPtr wxEventLoop::GetDirectFBEventBuffer()
{
    if ( !ms_buffer )
        InitBuffer();

    return ms_buffer;
}

//-----------------------------------------------------------------------------
// events dispatch and loop handling
//-----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    wxCHECK_MSG( ms_buffer, false, _T("invalid event buffer") );

    // returns DFB_OK if there is >=1 event, DFB_BUFFER_EMPTY otherwise
    return ms_buffer->HasEvent(ms_buffer) == DFB_OK;
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( ms_buffer, false, _T("invalid event buffer") );

    // NB: we don't block indefinitely waiting for an event, but instead
    //     time out after a brief period in order to make sure that
    //     OnNextIteration() will be called frequently enough
    //
    //     FIXME: call NotifyTimers() from here (and loop) instead?
    const int TIMEOUT = 100;

    DFBResult ret = ms_buffer->WaitForEventWithTimeout(ms_buffer, 0, TIMEOUT);

    switch ( ret )
    {
        case DFB_OK:
        {
            wxDFBEvent e;
            ms_buffer->GetEvent(ms_buffer, &e);
            HandleDFBEvent(e);
            break;
        }

        case DFB_TIMEOUT:
            // timed out, pretend we processed an event so that OnNextIteration
            // is called
            break;

        default:
            // report any errors, but don't terminate the loop due to them
            wxDfbCheckReturn(ret);
            break;
    }

    return true;
}

void wxEventLoop::WakeUp()
{
    wxCHECK_RET( ms_buffer, _T("invalid event buffer") );

    DFB_CALL( ms_buffer->WakeUp(ms_buffer) );
}

void wxEventLoop::OnNextIteration()
{
#if wxUSE_TIMER
    // see the comment in Dispatch
    wxTimer::NotifyTimers();
#endif
}

#warning "FIXME: cleanup wxEventLoop::ms_buffer before exiting"


//-----------------------------------------------------------------------------
// DirectFB -> wxWidgets events translation
//-----------------------------------------------------------------------------

void wxEventLoop::HandleDFBEvent(const wxDFBEvent& event)
{
    switch ( event.GetClass() )
    {
        case DFEC_WINDOW:
        {
            wxDFBWindowEvent winevent(((const DFBEvent&)event).window);
            wxTopLevelWindowDFB::HandleDFBWindowEvent(winevent);
            break;
        }

        case DFEC_NONE:
        case DFEC_INPUT:
        case DFEC_USER:
        case DFEC_UNIVERSAL:
        {
            wxLogTrace(TRACE_EVENTS,
                       _T("ignoring event of unsupported class %i"),
                       (int)event.GetClass());
        }
    }
}
