/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/process.cpp
// Purpose:     Process termination classes
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin to check error codes, added Detach() method
// Created:     24/06/98
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/process.h"

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

wxDEFINE_EVENT( wxEVT_END_PROCESS, wxProcessEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxProcess, wxEvtHandler);
wxIMPLEMENT_DYNAMIC_CLASS(wxProcessEvent, wxEvent);

// ============================================================================
// wxProcess implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxProcess creation
// ----------------------------------------------------------------------------

void wxProcess::Init(wxEvtHandler *parent, int id, int flags)
{
    if ( parent )
        SetNextHandler(parent);

    m_id         = id;
    m_pid        = 0;
    m_priority   = wxPRIORITY_DEFAULT;
    m_redirect   = (flags & wxPROCESS_REDIRECT) != 0;

#if wxUSE_STREAMS
    m_inputStream  = NULL;
    m_errorStream  = NULL;
    m_outputStream = NULL;
#endif // wxUSE_STREAMS
}

/* static */
wxProcess *wxProcess::Open(const wxString& cmd, int flags)
{
    wxASSERT_MSG( !(flags & wxEXEC_SYNC), wxT("wxEXEC_SYNC should not be used." ));
    wxProcess *process = new wxProcess(wxPROCESS_REDIRECT);
    long pid = wxExecute(cmd, flags, process);
    if( !pid )
    {
        // couldn't launch the process
        delete process;
        return NULL;
    }

    process->SetPid(pid);

    return process;
}

// ----------------------------------------------------------------------------
// wxProcess termination
// ----------------------------------------------------------------------------

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
    // we just detach from the next handler of the chain (i.e. our "parent" -- see ctor)
    // not also from the previous handler like wxEvtHandler::Unlink() would do:

    if (m_nextHandler)
        m_nextHandler->SetPreviousHandler(m_previousHandler);

    m_nextHandler = NULL;
}

// ----------------------------------------------------------------------------
// process IO redirection
// ----------------------------------------------------------------------------

#if wxUSE_STREAMS

void wxProcess::SetPipeStreams(wxInputStream *inputSstream,
                               wxOutputStream *outputStream,
                               wxInputStream *errorStream)
{
    m_inputStream  = inputSstream;
    m_errorStream  = errorStream;
    m_outputStream = outputStream;
}

bool wxProcess::IsInputOpened() const
{
    return m_inputStream && m_inputStream->GetLastError() != wxSTREAM_EOF;
}

bool wxProcess::IsInputAvailable() const
{
    return m_inputStream && m_inputStream->CanRead();
}

bool wxProcess::IsErrorAvailable() const
{
    return m_errorStream && m_errorStream->CanRead();
}

#endif // wxUSE_STREAMS

// ----------------------------------------------------------------------------
// process killing
// ----------------------------------------------------------------------------

/* static */
wxKillError wxProcess::Kill(int pid, wxSignal sig, int flags)
{
    wxKillError rc;
    (void)wxKill(pid, sig, &rc, flags);

    return rc;
}

/* static */
bool wxProcess::Exists(int pid)
{
    switch ( Kill(pid, wxSIGNONE) )
    {
        case wxKILL_OK:
        case wxKILL_ACCESS_DENIED:
            return true;

        default:
        case wxKILL_ERROR:
        case wxKILL_BAD_SIGNAL:
            wxFAIL_MSG( wxT("unexpected wxProcess::Kill() return code") );
            wxFALLTHROUGH;

        case wxKILL_NO_PROCESS:
            return false;
    }
}

bool wxProcess::Activate() const
{
#ifdef __WINDOWS__
    // This function is defined in src/msw/utils.cpp.
    extern bool wxMSWActivatePID(long pid);

    return wxMSWActivatePID(m_pid);
#else
    return false;
#endif
}

void wxProcess::SetPriority(unsigned priority)
{
    wxCHECK_RET( priority <= wxPRIORITY_MAX,
                 wxS("Invalid process priority value.") );

    m_priority = priority;
}
