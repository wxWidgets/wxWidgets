/////////////////////////////////////////////////////////////////////////////
// Name:        _evtloop.i
// Purpose:     SWIG interface for wxEventLoop
//
// Author:      Robin Dunn
//
// Created:     18-Sept-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
// TODO: wxPyEventLoop that virtualizes all the methods...

//---------------------------------------------------------------------------
%newgroup

%{
#if 0   // #ifdef __WXMAC__

// A dummy class that raises an exception if used...    
class wxEventLoop
{
public:
    wxEventLoop() { wxPyRaiseNotImplemented(); }
    int Run() { return 0; }
    void Exit(int rc = 0) {}
    bool Pending() const { return false; }
    bool Dispatch() { return false; }
    bool IsRunning() const { return false; }
    static wxEventLoop *GetActive() { wxPyRaiseNotImplemented(); return NULL; }
    static void SetActive(wxEventLoop* loop) { wxPyRaiseNotImplemented(); }
};

#else
 
#include <wx/evtloop.h>

#endif
%}

class wxEventLoop
{
public:
    wxEventLoop();
    virtual ~wxEventLoop();

    // start the event loop, return the exit code when it is finished
    virtual int Run();

    // exit from the loop with the given exit code
    virtual void Exit(int rc = 0);

    // return true if any events are available
    virtual bool Pending() const;

    // dispatch a single event, return false if we should exit from the loop
    virtual bool Dispatch();

    // is the event loop running now?
    virtual bool IsRunning() const;

    // return currently active (running) event loop, may be NULL
    static wxEventLoop *GetActive();

    // set currently active (running) event loop
    static void SetActive(wxEventLoop* loop);
};



// This object sets the wxEventLoop given to the ctor as the currently active
// one and unsets it in its dtor, this is especially useful in presence of
// exceptions but is more tidy even when we don't use them
class wxEventLoopActivator
{
public:
    wxEventLoopActivator(wxEventLoop *evtLoop);
    ~wxEventLoopActivator();
};
 


//---------------------------------------------------------------------------
