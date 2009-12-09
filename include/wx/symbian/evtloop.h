///////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/evtloop.h
// Purpose:     wxEventLoop class for Symbian OS
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYMBIAN_EVTLOOP_H_
#define _WX_SYMBIAN_EVTLOOP_H_

class WXDLLIMPEXP_BASE wxConsoleEventLoop : public wxEventLoopManual
{
public:
    // initialize the event loop, use IsOk() to check if we were successful
    wxConsoleEventLoop();
    virtual ~wxConsoleEventLoop();

    // implement base class pure virtuals
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual int DispatchTimeout(unsigned long timeout);
    virtual void WakeUp();
    virtual bool IsOk() const { /*return m_dispatcher != NULL;*/ return true; }
    virtual bool YieldFor(long WXUNUSED(eventsToProcess)) { return true; }

protected:

private:
    // pipe used for wake up messages: when a child thread wants to wake up
    // the event loop in the main thread it writes to this pipe
    //wxPrivate::PipeIOHandler *m_wakeupPipe;

    // either wxSelectDispatcher or wxEpollDispatcher
    //wxFDIODispatcher *m_dispatcher;

    wxDECLARE_NO_COPY_CLASS(wxConsoleEventLoop);
};


#endif // _WX_SYMBIAN_EVTLOOP_H_

