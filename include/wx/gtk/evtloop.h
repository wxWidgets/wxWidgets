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

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopBase
{
public:
    wxGUIEventLoop();

    virtual void ScheduleExit(int rc = 0) override;
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;

    void StoreGdkEventForLaterProcessing(GdkEvent* ev)
        { m_arrGdkEvents.Add(ev); }

protected:
    virtual int DoRun() override;
    virtual void DoYieldFor(long eventsToProcess) override;

private:
    // the exit code of this event loop
    int m_exitcode;

    // used to temporarily store events in DoYield()
    wxArrayPtrVoid m_arrGdkEvents;

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_GTK_EVTLOOP_H_
