/////////////////////////////////////////////////////////////////////////////
// Name:        process.cpp
// Purpose:     Process termination classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     24/06/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "process.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/process.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxProcess, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxProcessEvent, wxEvent)
#endif

wxProcess::wxProcess(wxEvtHandler *parent, int id)
{
  if (parent)
    SetPreviousHandler(parent);

  m_id = id;
}

wxProcess::~wxProcess()
{
}

void wxProcess::OnTerminate(int pid)
{
  wxProcessEvent event(m_id, pid);

  ProcessEvent(event);
}

wxProcessEvent::wxProcessEvent(int id, int pid):
  wxEvent(id), m_pid(pid)
{
}

