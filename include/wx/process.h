/////////////////////////////////////////////////////////////////////////////
// Name:        process.h
// Purpose:     wxProcess class
// Author:      Guilhem Lavaux
// Modified by:
// Created:     24/06/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROCESSH__
#define _WX_PROCESSH__

#ifdef __GNUG__
    #pragma interface "process.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/event.h"

// Process Event handling
class WXDLLEXPORT wxProcessEvent : public wxEvent
{
DECLARE_DYNAMIC_CLASS(wxProcessEvent)

public:
    wxProcessEvent(int id = 0, int pid = 0, int exitcode = 0) : wxEvent(id)
    {
        m_eventType = wxEVT_END_PROCESS;
        m_pid = pid;
        m_exitcode = exitcode;
    }

    // accessors
        // PID of process which terminated
    int GetPid() { return m_pid; }

        // the exit code
    int GetExitCode() { return m_exitcode; }

public:
    int m_pid, m_exitcode;
};

// A wxProcess object should be passed to wxExecute - than its OnTerminate()
// function will be called when the process terminates.
class WXDLLEXPORT wxProcess : public wxEvtHandler
{
DECLARE_DYNAMIC_CLASS(wxProcess)

public:
    wxProcess(wxEvtHandler *parent = (wxEvtHandler *) NULL, int id = -1);

    virtual void OnTerminate(int pid, int status);

protected:
    int m_id;
};

typedef void (wxObject::*wxProcessEventFunction)(wxProcessEvent&);

#define EVT_END_PROCESS(id, func) { wxEVT_END_PROCESS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxProcessEventFunction) & func, NULL},

#endif
    // _WX_PROCESSH__
