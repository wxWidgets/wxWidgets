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

DEFINE_EVENT_TYPE(wxEVT_END_PROCESS)

IMPLEMENT_DYNAMIC_CLASS(wxProcess, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxProcessEvent, wxEvent)

void wxProcess::Init(wxEvtHandler *parent, int id, bool redirect)
{
    if ( parent )
        SetNextHandler(parent);

    m_id         = id;
    m_redirect   = redirect;

#if wxUSE_STREAMS
    m_inputStream  = NULL;
    m_errorStream  = NULL;
    m_outputStream = NULL;
#endif // wxUSE_STREAMS
}

wxProcess::~wxProcess()
{
#if wxUSE_STREAMS
    delete m_inputStream;
    delete m_errorStream;
    delete m_outputStream;
#endif // wxUSE_STREAMS
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

#if wxUSE_STREAMS

void wxProcess::SetPipeStreams(wxInputStream *inputSstream,
                               wxOutputStream *outputStream,
                               wxInputStream *errorStream)
{
    m_inputStream  = inputSstream;
    m_errorStream  = errorStream;
    m_outputStream = outputStream;
}

#endif // wxUSE_STREAMS
