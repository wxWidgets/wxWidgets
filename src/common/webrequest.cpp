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

wxScopedPtr<wxWebSession> wxWebSession::ms_defaultSession;
wxStringWebSessionFactoryMap wxWebSession::ms_factoryMap;

// static
wxWebSession& wxWebSession::GetDefault()
{
    if (ms_defaultSession == NULL)
        ms_defaultSession.reset(wxWebSession::New());

    return *ms_defaultSession;
}

// static
wxWebSession* wxWebSession::New(const wxString& backend)
{
    if (ms_factoryMap.empty())
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = ms_factoryMap.find(backend);
    if (factory != ms_factoryMap.end())
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
    if (ms_factoryMap.empty())
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = ms_factoryMap.find(backend);
    return factory != ms_factoryMap.end();
}



#endif // wxUSE_WEBREQUEST
