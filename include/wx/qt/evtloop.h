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

    virtual int DoRun() wxOVERRIDE;
    virtual void ScheduleExit(int rc = 0) wxOVERRIDE;
    virtual bool Pending() const wxOVERRIDE;
    virtual bool Dispatch() wxOVERRIDE;
    virtual int DispatchTimeout(unsigned long timeout) wxOVERRIDE;
    virtual void WakeUp() wxOVERRIDE;
    virtual void DoYieldFor(long eventsToProcess) wxOVERRIDE;

    void ScheduleIdleCheck();

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
