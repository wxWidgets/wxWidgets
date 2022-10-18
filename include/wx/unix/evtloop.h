///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/evtloop.h
// Purpose:     declares wxEventLoop class
// Author:      Lukasz Michalski (lm@zork.pl)
// Created:     2007-05-07
// Copyright:   (c) 2007 Lukasz Michalski
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_EVTLOOP_H_
#define _WX_UNIX_EVTLOOP_H_

#if wxUSE_CONSOLE_EVENTLOOP

// ----------------------------------------------------------------------------
// wxConsoleEventLoop
// ----------------------------------------------------------------------------

class wxEventLoopSource;
class wxFDIODispatcher;
class wxWakeUpPipeMT;

class WXDLLIMPEXP_BASE wxConsoleEventLoop
#ifdef __WXOSX__
: public wxCFEventLoop
#else
: public wxEventLoopManual
#endif
{
public:
    // initialize the event loop, use IsOk() to check if we were successful
    wxConsoleEventLoop();
    virtual ~wxConsoleEventLoop();

    // implement base class pure virtuals
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual bool IsOk() const override { return m_dispatcher != nullptr; }

protected:
    virtual void OnNextIteration() override;
    virtual void DoYieldFor(long eventsToProcess) override;

private:
    // pipe used for wake up messages: when a child thread wants to wake up
    // the event loop in the main thread it writes to this pipe
    wxWakeUpPipeMT *m_wakeupPipe;

    // the event loop source used to monitor this pipe
    wxEventLoopSource* m_wakeupSource;

    // either wxSelectDispatcher or wxEpollDispatcher
    wxFDIODispatcher *m_dispatcher;

    wxDECLARE_NO_COPY_CLASS(wxConsoleEventLoop);
};

#endif // wxUSE_CONSOLE_EVENTLOOP

#endif // _WX_UNIX_EVTLOOP_H_
