///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/wakeuppipe.h
// Purpose:     Helper class allowing to wake up the main thread.
// Author:      Vadim Zeitlin
// Created:     2013-06-09 (extracted from src/unix/evtloopunix.cpp)
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_WAKEUPPIPE_H_
#define _WX_UNIX_PRIVATE_WAKEUPPIPE_H_

#include "wx/unix/pipe.h"
#include "wx/evtloopsrc.h"

// ----------------------------------------------------------------------------
// wxWakeUpPipe: allows to wake up the event loop by writing to it
// ----------------------------------------------------------------------------

// This class is MT-safe and its WakeUp() can be called from any thread and
// even from a signal handler.

class wxWakeUpPipe : public wxEventLoopSourceHandler
{
public:
    // Create and initialize the pipe.
    //
    // It's the callers responsibility to add the read end of this pipe,
    // returned by GetReadFd(), to the code blocking on input.
    wxWakeUpPipe();

    // Wake up the blocking operation involving this pipe.
    //
    // It simply writes to the write end of the pipe, which is non-blocking,
    // and doesn't do anything else, so it can be called from any thread and
    // also from a signal handler (it also preserves errno for this reason).
    void WakeUp();

    // Return the read end of the pipe.
    int GetReadFd() { return m_pipe[wxPipe::Read]; }


    // Implement wxEventLoopSourceHandler pure virtual methods
    virtual void OnReadWaiting() override;
    virtual void OnWriteWaiting() override { }
    virtual void OnExceptionWaiting() override { }

private:
    wxPipe m_pipe;
};

#endif // _WX_UNIX_PRIVATE_WAKEUPPIPE_H_
