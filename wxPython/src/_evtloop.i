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
#include <wx/evtloop.h>
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


//---------------------------------------------------------------------------
