///////////////////////////////////////////////////////////////////////////////
// Name:        wx/evtloop.h
// Purpose:     declares wxEventLoop class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EVTLOOP_H_
#define _WX_EVTLOOP_H_

#include "wx/utils.h"

// ----------------------------------------------------------------------------
// wxEventLoopBase: interface for wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxEventLoopBase
{
public:
    // trivial, but needed (because of wxEventLoopBase) ctor
    wxEventLoopBase() { }

    // dtor
    virtual ~wxEventLoopBase() { }

    // use this to check whether the event loop was successfully created before
    // using it
    virtual bool IsOk() const { return true; }


    // start the event loop, return the exit code when it is finished
    virtual int Run() = 0;

    // exit from the loop with the given exit code
    virtual void Exit(int rc = 0) = 0;

    // return true if any events are available
    virtual bool Pending() const = 0;

    // dispatch a single event, return false if we should exit from the loop
    virtual bool Dispatch() = 0;

    // same as Dispatch() but doesn't wait for longer than the specified (in
    // ms) timeout, return true if an event was processed, false if we should
    // exit the loop or -1 if timeout expired
    virtual int DispatchTimeout(unsigned long timeout) = 0;


    // return currently active (running) event loop, may be NULL
    static wxEventLoopBase *GetActive() { return ms_activeLoop; }

    // set currently active (running) event loop
    static void SetActive(wxEventLoopBase* loop) { ms_activeLoop = loop; }

    // is this event loop running now?
    //
    // notice that even if this event loop hasn't terminated yet but has just
    // spawned a nested (e.g. modal) event loop, this would return false
    bool IsRunning() const;

    // implement this to wake up the loop: usually done by posting a dummy event
    // to it (can be called from non main thread)
    virtual void WakeUp() = 0;

protected:
    // this function should be called before the event loop terminates, whether
    // this happens normally (because of Exit() call) or abnormally (because of
    // an exception thrown from inside the loop)
    virtual void OnExit() { }


    // the pointer to currently active loop
    static wxEventLoopBase *ms_activeLoop;

    DECLARE_NO_COPY_CLASS(wxEventLoopBase)
};

#if defined(__WXMSW__) || defined(__WXMAC__) || defined(__WXDFB__) || defined(__UNIX__)

// this class can be used to implement a standard event loop logic using
// Pending() and Dispatch()
//
// it also handles idle processing automatically
class WXDLLIMPEXP_BASE wxEventLoopManual : public wxEventLoopBase
{
public:
    wxEventLoopManual();

    // enters a loop calling OnNextIteration(), Pending() and Dispatch() and
    // terminating when Exit() is called
    virtual int Run();

    // sets the "should exit" flag and wakes up the loop so that it terminates
    // soon
    virtual void Exit(int rc = 0);

protected:
    // may be overridden to perform some action at the start of each new event
    // loop iteration
    virtual void OnNextIteration() { }


    // the loop exit code
    int m_exitcode;

    // should we exit the loop?
    bool m_shouldExit;
};

#endif // platforms using "manual" loop

// we're moving away from old m_impl wxEventLoop model as otherwise the user
// code doesn't have access to platform-specific wxEventLoop methods and this
// can sometimes be very useful (e.g. under MSW this is necessary for
// integration with MFC) but currently this is done for MSW only, other ports
// should follow a.s.a.p.
#if defined(__WXPALMOS__)
    #include "wx/palmos/evtloop.h"
#elif defined(__WXMSW__)
    #include "wx/msw/evtloop.h"
#elif defined(__WXMAC__)
    #include "wx/osx/evtloop.h"
#elif defined(__WXDFB__)
    #include "wx/dfb/evtloop.h"
#else // other platform

#define wxNEEDS_GENERIC_DISPATCH_TIMEOUT

class WXDLLIMPEXP_FWD_CORE wxEventLoopImpl;

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopBase
{
public:
    wxGUIEventLoop() { m_impl = NULL; }
    virtual ~wxGUIEventLoop();

    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual int DispatchTimeout(unsigned long timeout);
    virtual void WakeUp() { }

protected:
    // the pointer to the port specific implementation class
    wxEventLoopImpl *m_impl;

    DECLARE_NO_COPY_CLASS(wxGUIEventLoop)
};

#endif // platforms

// also include the header defining wxConsoleEventLoop for Unix systems
#if defined(__UNIX__)
    #include "wx/unix/evtloop.h"
#endif

// we use a class rather than a typedef because wxEventLoop is forward-declared
// in many places
#if wxUSE_GUI
    class wxEventLoop : public wxGUIEventLoop { };
#else // !GUI
    // we can't define wxEventLoop differently in GUI and base libraries so use
    // a #define to still allow writing wxEventLoop in the user code
    #if wxUSE_CONSOLE_EVENTLOOP && (defined(__WXMSW__) || defined(__UNIX__))
        #define wxEventLoop wxConsoleEventLoop
    #else // we still must define it somehow for the code below...
        #define wxEventLoop wxEventLoopBase
    #endif
#endif

inline bool wxEventLoopBase::IsRunning() const { return GetActive() == this; }

#if wxUSE_GUI
// ----------------------------------------------------------------------------
// wxModalEventLoop
// ----------------------------------------------------------------------------

// this is a naive generic implementation which uses wxWindowDisabler to
// implement modality, we will surely need platform-specific implementations
// too, this generic implementation is here only temporarily to see how it
// works
class WXDLLIMPEXP_CORE wxModalEventLoop : public wxGUIEventLoop
{
public:
    wxModalEventLoop(wxWindow *winModal)
    {
        m_windowDisabler = new wxWindowDisabler(winModal);
    }

protected:
    virtual void OnExit()
    {
        delete m_windowDisabler;
        m_windowDisabler = NULL;

        wxGUIEventLoop::OnExit();
    }

private:
    wxWindowDisabler *m_windowDisabler;
};

#endif //wxUSE_GUI

// ----------------------------------------------------------------------------
// wxEventLoopActivator: helper class for wxEventLoop implementations
// ----------------------------------------------------------------------------

// this object sets the wxEventLoop given to the ctor as the currently active
// one and unsets it in its dtor, this is especially useful in presence of
// exceptions but is more tidy even when we don't use them
class wxEventLoopActivator
{
public:
    wxEventLoopActivator(wxEventLoopBase *evtLoop)
    {
        m_evtLoopOld = wxEventLoopBase::GetActive();
        wxEventLoopBase::SetActive(evtLoop);
    }

    ~wxEventLoopActivator()
    {
        // restore the previously active event loop
        wxEventLoopBase::SetActive(m_evtLoopOld);
    }

private:
    wxEventLoopBase *m_evtLoopOld;
};

#if wxUSE_CONSOLE_EVENTLOOP

class wxEventLoopGuarantor
{
public:
    wxEventLoopGuarantor()
    {
        m_evtLoopNew = NULL;
        if (!wxEventLoop::GetActive())
        {
            m_evtLoopNew = new wxEventLoop;
            wxEventLoop::SetActive(m_evtLoopNew);
        }
    }

    ~wxEventLoopGuarantor()
    {
        if (m_evtLoopNew)
        {
            wxEventLoop::SetActive(NULL);
            delete m_evtLoopNew;
        }
    }

private:
    wxEventLoop *m_evtLoopNew;
};

#endif // wxUSE_CONSOLE_EVENTLOOP

#endif // _WX_EVTLOOP_H_
