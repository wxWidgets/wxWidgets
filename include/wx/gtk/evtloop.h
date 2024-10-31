///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/evtloop.h
// Purpose:     wxGTK event loop implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-27
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_EVTLOOP_H_
#define _WX_GTK_EVTLOOP_H_

// ----------------------------------------------------------------------------
// wxGUIEventLoop for wxGTK
// ----------------------------------------------------------------------------

typedef union  _GdkEvent        GdkEvent;

#include <vector>

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopBase
{
public:
    wxGUIEventLoop();
    virtual ~wxGUIEventLoop();

    virtual void ScheduleExit(int rc = 0) override;
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;

    // implementation only from now on

    void StoreGdkEventForLaterProcessing(GdkEvent* ev)
        { m_queuedGdkEvents.push_back(ev); }

    // Check if this event is the same as the last event passed to this
    // function and store it for future checks.
    bool GTKIsSameAsLastEvent(const GdkEvent* ev, size_t size);

protected:
    virtual int DoRun() override;
    virtual void DoYieldFor(long eventsToProcess) override;

private:
    // the exit code of this event loop
    int m_exitcode;

    // used to temporarily store events processed in DoYieldFor()
    std::vector<GdkEvent*> m_queuedGdkEvents;

    // last event passed to GTKIsSameAsLastEvent()
    GdkEvent* m_lastEvent;

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_GTK_EVTLOOP_H_
