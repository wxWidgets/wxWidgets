///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/evtloop.h
// Purpose:     wxGUIEventLoop for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_EVTLOOP_H_
#define _WX_RETRO68_EVTLOOP_H_

// ----------------------------------------------------------------------------
// wxGUIEventLoop: event loop using WaitNextEvent()
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopManual
{
public:
    wxGUIEventLoop() = default;

    // implement base class pure virtuals
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;

protected:
    virtual void DoYieldFor(long eventsToProcess) override;

private:
    // Wait for the next event for at most the given number of milliseconds
    // (or indefinitely if timeout is negative) and dispatch it if there is
    // one. Return true if an event was dispatched.
    bool DoDispatchTimeout(long timeout);

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_RETRO68_EVTLOOP_H_
