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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "evtloop.h"
#endif

class WXDLLEXPORT wxEventLoopImpl;

// ----------------------------------------------------------------------------
// wxEventLoop: a GUI event loop
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoop
{
public:
    // ctor
    wxEventLoop() { m_impl = NULL; }

    // dtor
    virtual ~wxEventLoop();

    // start the event loop, return the exit code when it is finished
    virtual int Run();

    // exit from the loop with the given exit code
    virtual void Exit(int rc = 0);

    // return TRUE if any events are available
    virtual bool Pending() const;

    // dispatch a single event, return FALSE if we should exit from the loop
    virtual bool Dispatch();

    // is the event loop running now?
    virtual bool IsRunning() const;

    // return currently active (running) event loop, may be NULL
    static wxEventLoop *GetActive() { return ms_activeLoop; }

    // set currently active (running) event loop
    static void SetActive(wxEventLoop* loop) { ms_activeLoop = loop; }

protected:
    // this function should be called before the event loop terminates, whether
    // this happens normally (because of Exit() call) or abnormally (because of
    // an exception thrown from inside the loop)
    virtual void OnExit() { }


    // the pointer to currently active loop
    static wxEventLoop *ms_activeLoop;

    // the pointer to the port specific implementation class
    wxEventLoopImpl *m_impl;

    DECLARE_NO_COPY_CLASS(wxEventLoop)
};

// ----------------------------------------------------------------------------
// wxModalEventLoop
// ----------------------------------------------------------------------------

// this is a naive generic implementation which uses wxWindowDisabler to
// implement modality, we will surely need platform-specific implementations
// too, this generic implementation is here only temporarily to see how it
// works
class WXDLLEXPORT wxModalEventLoop : public wxEventLoop
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

        wxEventLoop::OnExit();
    }

private:
    wxWindowDisabler *m_windowDisabler;
};

#endif // _WX_EVTLOOP_H_
