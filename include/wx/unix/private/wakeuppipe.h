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

// This class is not MT-safe, see wxWakeUpPipeMT below for a wake up pipe
// usable from other threads.

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
    // As indicated by its name, this method does no locking and so can be
    // called only from the main thread.
    void WakeUpNoLock();

    // Same as WakeUp() but without locking.

    // Return the read end of the pipe.
    int GetReadFd() { return m_pipe[wxPipe::Read]; }


    // implement wxFDIOHandler pure virtual methods
    virtual void OnReadWaiting();
    virtual void OnWriteWaiting() { }
    virtual void OnExceptionWaiting() { }

private:
    wxPipe m_pipe;

    // This flag is set to true after writing to the pipe and reset to false
    // after reading from it in the main thread. Having it allows us to avoid
    // overflowing the pipe with too many writes if the main thread can't keep
    // up with reading from it.
    bool m_pipeIsEmpty;
};

// ----------------------------------------------------------------------------
// wxWakeUpPipeMT: thread-safe version of wxWakeUpPipe
// ----------------------------------------------------------------------------

// This class can be used from multiple threads, i.e. its WakeUp() can be
// called concurrently.

class wxWakeUpPipeMT : public wxWakeUpPipe
{
public:
    wxWakeUpPipeMT() { }

    // Thread-safe wrapper around WakeUpNoLock(): can be called from another
    // thread to wake up the main one.
    void WakeUp()
    {
        wxCriticalSectionLocker lock(m_pipeLock);

        WakeUpNoLock();
    }

    virtual void OnReadWaiting()
    {
        wxCriticalSectionLocker lock(m_pipeLock);

        wxWakeUpPipe::OnReadWaiting();
    }

private:
    // Protects access to m_pipeIsEmpty.
    wxCriticalSection m_pipeLock;
};

#endif // _WX_UNIX_PRIVATE_WAKEUPPIPE_H_
