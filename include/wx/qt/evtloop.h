/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/evtloop.h
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_EVTLOOP_H_
#define _WX_QT_EVTLOOP_H_

class wxQtIdleTimer;
class QEventLoop;

class WXDLLIMPEXP_CORE wxQtEventLoopBase : public wxEventLoopBase
{
public:
    wxQtEventLoopBase();
    ~wxQtEventLoopBase();

    virtual int DoRun() override;
    virtual void DoStop(int rc) override;
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual void DoYieldFor(long eventsToProcess) override;

    void ScheduleIdleCheck();

    // Non-blocking Dispatch() version:
    // Returns true if an event was processed, otherwise returns false.
    // This function is added to address code like:
    //
    //      while (evtloop->Pending())
    //          evtloop->Dispatch();
    //
    // which can simply replaced with:
    //
    //      while (evtloop->QtDispatch())
    //          ;
    bool QtDispatch() const;

private:
    QEventLoop *m_qtEventLoop;
    wxObjectDataPtr<wxQtIdleTimer> m_qtIdleTimer;

    wxDECLARE_NO_COPY_CLASS(wxQtEventLoopBase);
};


#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxQtEventLoopBase
{
public:
    wxGUIEventLoop();
};

#endif // wxUSE_GUI

#endif // _WX_QT_EVTLOOP_H_
