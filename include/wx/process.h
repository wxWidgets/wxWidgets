/////////////////////////////////////////////////////////////////////////////
// Name:        process.h
// Purpose:     wxProcess class
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin to check error codes, added Detach() method
// Created:     24/06/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROCESSH__
#define _WX_PROCESSH__

#ifdef __GNUG__
    #pragma interface "process.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/event.h"

#if wxUSE_STREAMS
#include "wx/stream.h"
#endif

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
    wxProcess(wxEvtHandler *parent = (wxEvtHandler *) NULL, int id = -1)
        { Init(parent, id, FALSE); }
    wxProcess(wxEvtHandler *parent, bool redirect)
        { Init(parent, -1, redirect); }

    virtual ~wxProcess();

    // may be overridden to be notified about process termination
    virtual void OnTerminate(int pid, int status);

    // call this before passing the object to wxExecute() to redirect the
    // launched process stdin/stdout, then use GetInputStream() and
    // GetOutputStream() to get access to them
    void Redirect() { m_redirect = TRUE; }
    bool IsRedirected() const { return m_redirect; }

    // detach from the parent - should be called by the parent if it's deleted
    // before the process it started terminates
    void Detach();

#if wxUSE_STREAMS
    // Pipe handling
    wxInputStream *GetInputStream() const { return m_inputStream; }
    wxInputStream *GetErrorStream() const { return m_errorStream; }
    wxOutputStream *GetOutputStream() const { return m_outputStream; }

    // implementation only (for wxExecute)
    void SetPipeStreams(wxInputStream *inStream,
                        wxOutputStream *outStream,
                        wxInputStream *errStream);
#endif // wxUSE_STREAMS

protected:
    void Init(wxEvtHandler *parent, int id, bool redirect);

    int m_id;

#if wxUSE_STREAMS
    wxInputStream  *m_inputStream,
                   *m_errorStream;
    wxOutputStream *m_outputStream;
#endif // wxUSE_STREAMS

    bool m_redirect;
};

typedef void (wxObject::*wxProcessEventFunction)(wxProcessEvent&);

#define EVT_END_PROCESS(id, func) { wxEVT_END_PROCESS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxProcessEventFunction) & func, NULL},

#endif
    // _WX_PROCESSH__
