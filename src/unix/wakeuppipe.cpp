///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/wakeuppipe.cpp
// Purpose:     Implementation of wxWakeUpPipe class.
// Author:      Vadim Zeitlin
// Created:     2013-06-09 (extracted from src/unix/evtloopunix.cpp)
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
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


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/unix/private/wakeuppipe.h"

#include <errno.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define TRACE_EVENTS wxT("events")

// ============================================================================
// wxWakeUpPipe implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization
// ----------------------------------------------------------------------------

wxWakeUpPipe::wxWakeUpPipe()
{
    if ( !m_pipe.Create() )
    {
        wxLogError(_("Failed to create wake up pipe used by event loop."));
        return;
    }


    // Make writes non-blocking too: WakeUp() doesn't check if the pipe is
    // full, and if it is, a wake up is pending anyhow.
    if ( !m_pipe.MakeNonBlocking(wxPipe::Read) ||
            !m_pipe.MakeNonBlocking(wxPipe::Write) )
    {
        wxLogSysError(_("Failed to switch wake up pipe to non-blocking mode"));
        return;
    }

    wxLogTrace(TRACE_EVENTS, wxT("Wake up pipe (%d, %d) created"),
               m_pipe[wxPipe::Read], m_pipe[wxPipe::Write]);
}

// ----------------------------------------------------------------------------
// wakeup handling
// ----------------------------------------------------------------------------

void wxWakeUpPipe::WakeUp()
{
    // This function can be called from a signal handler, so it must not change
    // errno and can't do anything except calling async-signal-safe functions.
    const int savedErrno = errno;

    if ( write(m_pipe[wxPipe::Write], "s", 1) != 1 )
    {
        // Nothing to do here: if the pipe is full (EAGAIN), we will be woken
        // up anyhow, and we can't report any other (unexpected) errors safely
        // from here.
    }

    errno = savedErrno;
}

void wxWakeUpPipe::OnReadWaiting()
{
    // got wakeup from another thread or a signal handler, remove all the data
    // written to the pipe by (possibly several) calls to WakeUp() from it

    char buf[256];
    for ( ;; )
    {
        const int size = read(GetReadFd(), buf, WXSIZEOF(buf));

        if ( size > 0 )
        {
            // Continue reading until the pipe becomes empty.
            continue;
        }

        if ( size == 0 || (size == -1 && errno == EAGAIN) )
        {
            // No (more) data available, not an error.
            break;
        }

        if ( errno == EINTR )
        {
            // We were interrupted, try again.
            continue;
        }

        wxLogSysError(_("Failed to read from wake-up pipe"));

        break;
    }
}
