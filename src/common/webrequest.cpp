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
#include "wx/mstream.h"
#include "wx/uri.h"
#include "wx/filename.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/translation.h"
#endif

#if wxUSE_WEBREQUEST_WINHTTP
#include "wx/msw/webrequest_winhttp.h"
#endif
#if wxUSE_WEBREQUEST_URLSESSION
#include "wx/osx/webrequest_urlsession.h"
#endif
#if wxUSE_WEBREQUEST_CURL
#include "wx/webrequest_curl.h"
#endif

extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendWinHTTP[] = "wxWebSessionBackendWinHTTP";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendURLSession[] = "wxWebSessionBackendURLSession";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendCURL[] = "wxWebSessionBackendCURL";

#if wxUSE_WEBREQUEST_WINHTTP
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendDefault[] = "wxWebSessionBackendWinHTTP";
#elif wxUSE_WEBREQUEST_URLSESSION
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendDefault[] = "wxWebSessionBackendURLSession";
#elif wxUSE_WEBREQUEST_CURL
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendDefault[] = "wxWebSessionBackendCURL";
#endif

wxDEFINE_EVENT(wxEVT_WEBREQUEST_STATE, wxWebRequestEvent);
wxDEFINE_EVENT(wxEVT_WEBREQUEST_DATA, wxWebRequestEvent);

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

void wxWebRequest::SetData(const wxString& text, const wxString& contentType, const wxMBConv& conv)
{
    m_dataText = text.mb_str(conv);
    SetData(wxSharedPtr<wxInputStream>(new wxMemoryInputStream(m_dataText, m_dataText.length())), contentType);
}

void wxWebRequest::SetData(wxSharedPtr<wxInputStream> dataStream, const wxString& contentType, wxFileOffset dataSize)
{
    m_dataStream = dataStream;
    if ( m_dataStream.get() )
    {
        if ( dataSize == wxInvalidOffset )
        {
            // Determine data size
            m_dataSize = m_dataStream->SeekI(0, wxFromEnd);
            m_dataStream->SeekI(0);
        }
        else
            m_dataSize = dataSize;
    }
    else
        m_dataSize = 0;

    SetHeader("Content-Type", contentType);
}

void wxWebRequest::SetState(State state, const wxString & failMsg)
{
    // Add a reference while the request is active
    if (state == State_Active && m_state != State_Active && m_state != State_Unauthorized)
        IncRef();

    // Trigger the event in the main thread
    CallAfter(&wxWebRequest::ProcessStateEvent, state, failMsg);
}

void wxWebRequest::ProcessStateEvent(State state, const wxString& failMsg)
{
    wxWebRequestEvent evt(wxEVT_WEBREQUEST_STATE, GetId(), state,
        GetResponse(), failMsg);
    ProcessEvent(evt);
    // Remove reference after the request is no longer active
    if (state == State_Completed || state == State_Failed ||
        state == State_Cancelled)
        DecRef();
    m_state = state;
}

//
// wxWebResponse
//
wxString wxWebResponse::GetMimeType() const
{
    return GetHeader("Mime-Type");
}

wxInputStream * wxWebResponse::GetStream() const
{
    if ( !m_stream.get() )
    {
        // Create stream
        m_stream.reset(new wxMemoryInputStream(m_readBuffer.GetData(), m_readBuffer.GetDataLen()));
    }

    return m_stream.get();
}

wxString wxWebResponse::GetSuggestedFileName() const
{
    wxString suggestedFilename;

    // TODO: get from Content-Disposition header

    wxURI uri(GetURL());
    if ( uri.HasPath() )
    {
        wxFileName fn(uri.GetPath());
        suggestedFilename = fn.GetFullName();
    }
    else
        suggestedFilename = uri.GetServer();

    return suggestedFilename;
}

wxString wxWebResponse::AsString(wxMBConv * conv) const
{
    // TODO: try to determine encoding type from content-type header
    if (!conv)
        conv = &wxConvUTF8;

    if (m_request.GetStorage() == wxWebRequest::Storage_Memory)
    {
        size_t outLen = 0;
        return conv->cMB2WC((const char*)m_readBuffer.GetData(), m_readBuffer.GetDataLen(), &outLen);
    }
    else
        return wxString();
}

void* wxWebResponse::GetDataBuffer(size_t sizeNeeded)
{
    return m_readBuffer.GetAppendBuf(sizeNeeded);
}

void wxWebResponse::ReportDataReceived(size_t sizeReceived)
{
    m_readBuffer.UngetAppendBuf(sizeReceived);
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
#if wxUSE_WEBREQUEST_WINHTTP
    RegisterFactory(wxWebSessionBackendWinHTTP,
        wxSharedPtr<wxWebSessionFactory>(new wxWebSessionFactoryWinHTTP()));
#endif
#if wxUSE_WEBREQUEST_URLSESSION
	RegisterFactory(wxWebSessionBackendURLSession,
					wxSharedPtr<wxWebSessionFactory>(new wxWebSessionFactoryURLSession()));
#endif
#if wxUSE_WEBREQUEST_CURL
	RegisterFactory(wxWebSessionBackendCURL,
					wxSharedPtr<wxWebSessionFactory>(new wxWebSessionFactoryCURL()));
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
