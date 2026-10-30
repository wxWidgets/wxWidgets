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
    m_pipeIsEmpty = true;

    if ( !m_pipe.Create() )
    {
        wxLogError(_("Failed to create wake up pipe used by event loop."));
        return;
    }


    // Non-blocking writes: WakeUpFromSignal() doesn't check if the pipe is
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

void wxWakeUpPipe::WakeUpNoLock()
{
    // No need to do anything if the pipe already contains something.
    if ( !m_pipeIsEmpty )
      return;

    if ( write(m_pipe[wxPipe::Write], "s", 1) != 1 )
    {
        // don't use wxLog here, we can be in another thread and this could
        // result in dead locks
        perror("write(wake up pipe)");
    }
    else
    {
        // We just wrote to it, so it's not empty any more.
        m_pipeIsEmpty = false;
    }
}

void wxWakeUpPipe::WakeUpFromSignal()
{
    const int savedErrno = errno;

    if ( write(m_pipe[wxPipe::Write], "s", 1) != 1 )
    {
        // The pipe is full, so we will be woken up anyhow.
    }

    errno = savedErrno;
}

void wxWakeUpPipe::OnReadWaiting()
{
    // got wakeup from child thread, remove the data that provoked it from the
    // pipe

    char buf[4];
    for ( ;; )
    {
        const int size = read(GetReadFd(), buf, WXSIZEOF(buf));

        if ( size > 0 )
        {
            // Read everything, there may be more than one byte if
            // WakeUpFromSignal() was called several times.
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

        return;
    }

    // The pipe is empty now, so future calls to WakeUp() would need to write
    // to it again.
    m_pipeIsEmpty = true;
}
