/////////////////////////////////////////////////////////////////////////////
// Name:        process.cpp
// Purpose:     Process termination classes
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin to check error codes, added Detach() method
// Created:     24/06/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
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

IMPLEMENT_DYNAMIC_CLASS(wxProcess, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxProcessEvent, wxEvent)

wxProcess::wxProcess(wxEvtHandler *parent, int id)
{
    if (parent)
        SetNextHandler(parent);

    m_id = id;
}

void wxProcess::OnTerminate(int pid, int status)
{
    wxProcessEvent event(m_id, pid, status);

    if ( !ProcessEvent(event) )
        delete this;
    //else: the object which processed the event is responsible for deleting
    //      us!
}

void wxProcess::Detach()
{
    SetNextHandler(NULL);
}
