///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/evtloop.h
// Purpose:     declares wxEventLoop class
// Author:      Lukasz Michalski (lm@zork.pl)
// Created:     2007-05-07
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Lukasz Michalski
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_EVTLOOP_H_
#define _WX_UNIX_EVTLOOP_H_

#include "wx/private/fdiodispatcher.h"
#include "wx/unix/private/timer.h"
#include "wx/unix/pipe.h"

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConsoleEventLoop : public wxEventLoopManual
{
public:
    // initialize the event loop, use IsOk() to check if we were successful
    wxConsoleEventLoop();

    // implement base class pure virtuals
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual void WakeUp();
    virtual bool IsOk() const { return m_dispatcher != NULL; }

protected:
    virtual void OnNextIteration();

private:
    // pipe used for wake up messages: when a child thread wants to wake up
    // the event loop in the main thread it writes to this pipe
    class PipeIOHandler : public wxFDIOHandler
    {
    public:
        // default ctor does nothing, call Create() to really initialize the
        // object
        PipeIOHandler() { }

        bool Create();

        // this method can be, and normally is, called from another thread
        void WakeUp();

        int GetReadFd() { return m_pipe[wxPipe::Read]; }

        // implement wxFDIOHandler pure virtual methods
        virtual void OnReadWaiting();
        virtual void OnWriteWaiting() { }
        virtual void OnExceptionWaiting() { }

    private:
        wxPipe m_pipe;
    };

    PipeIOHandler m_wakeupPipe;

    // either wxSelectDispatcher or wxEpollDispatcher
    wxFDIODispatcher *m_dispatcher;

    DECLARE_NO_COPY_CLASS(wxConsoleEventLoop)
};

#endif // _WX_UNIX_EVTLOOP_H_
