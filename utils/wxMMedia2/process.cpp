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

wxProcess::wxProcess(wxEvtHandler *parent, bool needPipe, int id)
{
    if (parent)
        SetNextHandler(parent);

    m_id         = id;
    m_needPipe   = needPipe;
    m_in_stream  = NULL;
    m_out_stream = NULL;
}

wxProcess::~wxProcess()
{
    if (m_in_stream)
      delete m_in_stream;
    if (m_out_stream)
      delete m_out_stream;
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

void wxProcess::SetPipeStreams(wxInputStream *in_stream, wxOutputStream *out_stream)
{
    m_in_stream  = in_stream;
    m_out_stream = out_stream;
}

wxInputStream *wxProcess::GetInputStream() const
{
    return m_in_stream;
}

wxOutputStream *wxProcess::GetOutputStream() const
{
    return m_out_stream;
}

bool wxProcess::NeedPipe() const
{
    return m_needPipe;
}
