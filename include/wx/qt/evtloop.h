/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/evtloop.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_EVTLOOP_H_
#define _WX_QT_EVTLOOP_H_

class WXDLLIMPEXP_BASE wxQtEventLoopBase : public wxEventLoopBase
{
public:
    wxQtEventLoopBase();

    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual int DispatchTimeout(unsigned long timeout);
    virtual void WakeUp();
    virtual bool YieldFor(long eventsToProcess);

#if wxUSE_EVENTLOOP_SOURCE
    virtual wxEventLoopSource *AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags);
#endif // wxUSE_EVENTLOOP_SOURCE
protected:

private:
    wxDECLARE_NO_COPY_CLASS(wxQtEventLoopBase);
};


#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxQtEventLoopBase
{
public:
    wxGUIEventLoop();
};

#else // !wxUSE_GUI

#if wxUSE_CONSOLE_EVENTLOOP

class WXDLLIMPEXP_BASE wxConsoleEventLoop : public wxQtEventLoopBase
{
public:
    wxConsoleEventLoop();
};

#endif // wxUSE_CONSOLE_EVENTLOOP

#endif // wxUSE_GUI
                                        
#endif // _WX_QT_EVTLOOP_H_
