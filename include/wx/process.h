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

#ifndef __PROCESSH__
#define __PROCESSH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/event.h"

class WXDLLEXPORT wxProcess: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxProcess)
 public:

  wxProcess(wxEvtHandler *parent = NULL, int id = -1);
  virtual ~wxProcess();

  virtual void OnTerminate(int pid);

 protected:
  int m_id;
};

// Process Event handling

class WXDLLEXPORT wxProcessEvent: public wxEvent
{
  DECLARE_DYNAMIC_CLASS(wxProcessEvent)
 public:

  wxProcessEvent(int id = 0, int pid = -1);
  
  inline int GetPid() { return m_pid; }
  inline void SetPid(int pid) { m_pid = pid; }

 public:
  int m_pid;
};

typedef void (wxObject::*wxProcessEventFunction)(wxProcessEvent&);

#define EVT_END_PROCESS(id, func) { wxEVT_END_TERMINATE, id, -1, (wxObjectEvent) (wxEventFunction) (wxProcessEventFunction) & fn, NULL},

#endif
    // __PROCESSH__
