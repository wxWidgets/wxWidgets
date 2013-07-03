///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/wakeuppipe.h
// Purpose:     Helper class allowing to wake up the main thread.
// Author:      Vadim Zeitlin
// Created:     2013-06-09 (extracted from src/unix/evtloopunix.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_WAKEUPPIPE_H_
#define _WX_UNIX_PRIVATE_WAKEUPPIPE_H_

#include "wx/private/fdiohandler.h"

#include "wx/unix/pipe.h"

// ----------------------------------------------------------------------------
// wxWakeUpPipe: allows to wake up the event loop by writing to it
// ----------------------------------------------------------------------------

class wxWakeUpPipe : public wxFDIOHandler
{
public:
    // Create and initialize the pipe.
    //
    // It's the callers responsibility to add the read end of this pipe,
    // returned by GetReadFd(), to the code blocking on input.
    wxWakeUpPipe();

    // Wake up the blocking operation involving this pipe.
    //
    // It simply writes to the write end of the pipe.
    //
    // Notice that this method can be, and often is, called from another
    // thread.
    void WakeUp();

    // Return the read end of the pipe.
    int GetReadFd() { return m_pipe[wxPipe::Read]; }


    // implement wxFDIOHandler pure virtual methods
    virtual void OnReadWaiting();
    virtual void OnWriteWaiting() { }
    virtual void OnExceptionWaiting() { }

private:
    wxPipe m_pipe;

    // Protects access to m_pipeIsEmpty.
    wxCriticalSection m_pipeLock;

    // This flag is set to true after writing to the pipe and reset to false
    // after reading from it in the main thread. Having it allows us to avoid
    // overflowing the pipe with too many writes if the main thread can't keep
    // up with reading from it.
    bool m_pipeIsEmpty;
};

#endif // _WX_UNIX_PRIVATE_WAKEUPPIPE_H_
