///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/webrequest.cpp
// Purpose:     wxWebRequest base class implementations
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#if wxUSE_WEBREQUEST

#include "wx/webrequest.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/translation.h"
#endif

#if defined(__WINDOWS__)
#include "wx/msw/webrequest_winhttp.h"
#endif

extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendWinHTTP[] = "wxWebSessionBackendWinHTTP";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendURLSession[] = "wxWebSessionBackendURLSession";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendCURL[] = "wxWebSessionBackendCURL";

#if defined(__WINDOWS__)
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendDefault[] = "wxWebSessionBackendWinHTTP";
#endif

wxDEFINE_EVENT(wxEVT_WEBREQUEST_READY, wxWebRequestEvent);
wxDEFINE_EVENT(wxEVT_WEBREQUEST_FAILED, wxWebRequestEvent);
wxDEFINE_EVENT(wxEVT_WEBREQUEST_AUTH_REQUIRED, wxWebRequestEvent);

//
// wxWebRequest
//
bool wxWebRequest::CheckServerStatus()
{
    const wxWebResponse* resp = GetResponse();
    if ( resp && resp->GetStatus() >= 400 && !m_ignoreServerErrorStatus )
    {
        SetState(State_Failed, wxString::Format(_("Error: %s (%d)"),
            resp->GetStatusText(), resp->GetStatus()));
        return false;
    }
    else
        return true;
}

void wxWebRequest::SetState(State state, const wxString & failMsg)
{
    switch (state)
    {
        case State_Active:
            // Add a reference while the request is active
            if ( m_state != State_Active )
            {
                IncRef();
                m_state = state;
            }
            break;
        case State_Ready:
            // Trigger the ready event in main thread
            CallAfter(&wxWebRequest::ProcessReadyEvent);
            break;
        case State_Failed:
            m_failMessage = failMsg;
            // Trigger the failed event in main thread
            CallAfter(&wxWebRequest::ProcessFailedEvent);
            break;
    }
}

void wxWebRequest::ProcessReadyEvent()
{
    wxWebRequestEvent evt(wxEVT_WEBREQUEST_READY, GetId(), GetResponse());
    ProcessEvent(evt);
    // Remove reference after the request is no longer active
    if ( m_state == State_Active )
        DecRef();
    m_state = State_Ready;
}

void wxWebRequest::ProcessFailedEvent()
{
    wxWebRequestEvent evt(wxEVT_WEBREQUEST_FAILED, GetId(), NULL,
        m_failMessage);
    ProcessEvent(evt);
    // Remove reference after the request is no longer active
    if ( m_state == State_Active )
        DecRef();
    m_state = State_Failed;
}

//
// wxWebSession
//

wxScopedPtr<wxWebSession> wxWebSession::ms_defaultSession;
wxStringWebSessionFactoryMap wxWebSession::ms_factoryMap;

wxWebSession::wxWebSession()
{
    // Initialize the user-Agent header with a reasonable default
    SetHeader("User-Agent", wxString::Format("%s/1 wxWidgets/%d.%d.%d",
        wxTheApp->GetAppName(),
        wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER));
}

// static
wxWebSession& wxWebSession::GetDefault()
{
    if ( ms_defaultSession == NULL )
        ms_defaultSession.reset(wxWebSession::New());

    return *ms_defaultSession;
}

// static
wxWebSession* wxWebSession::New(const wxString& backend)
{
    if ( ms_factoryMap.empty() )
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = ms_factoryMap.find(backend);
    if ( factory != ms_factoryMap.end() )
        return factory->second->Create();
    else
        return NULL;
}

// static
void wxWebSession::RegisterFactory(const wxString& backend, wxSharedPtr<wxWebSessionFactory> factory)
{
    ms_factoryMap[backend] = factory;
}

// static
void wxWebSession::InitFactoryMap()
{
#if defined(__WINDOWS__)
    RegisterFactory(wxWebSessionBackendWinHTTP,
        wxSharedPtr<wxWebSessionFactory>(new wxWebSessionFactoryWinHTTP()));
#endif
}

// static
bool wxWebSession::IsBackendAvailable(const wxString& backend)
{
    if ( ms_factoryMap.empty() )
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = ms_factoryMap.find(backend);
    return factory != ms_factoryMap.end();
}

#endif // wxUSE_WEBREQUEST
