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
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/wfstream.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/translation.h"
    #include "wx/utils.h"
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

bool wxWebRequest::IsActiveState(State state)
{
    return (state == State_Active || state == State_Unauthorized);
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

wxFileOffset wxWebRequest::GetBytesReceived() const
{
    return m_bytesReceived;;
}

wxFileOffset wxWebRequest::GetBytesExpectedToReceive() const
{
    if ( GetResponse() )
        return GetResponse()->GetContentLength();
    else
        return -1;
}

void wxWebRequest::SetState(State state, const wxString & failMsg)
{
    // Add a reference while the request is active
    if ( IsActiveState(state) && !IsActiveState(m_state) )
        IncRef();

    m_state = state;

    // Trigger the event in the main thread
    CallAfter(&wxWebRequest::ProcessStateEvent, state, failMsg);
}

void wxWebRequest::ReportDataReceived(size_t sizeReceived)
{
    m_bytesReceived += sizeReceived;
}

// The SplitParamaters implementation is adapted to wxWidgets
// from Poco::Net::MessageHeader::splitParameters

void wxWebRequest::SplitParameters(const wxString& s, wxString& value,
    wxWebRequestHeaderMap& parameters)
{
    value.clear();
    parameters.clear();
    wxString::const_iterator it = s.begin();
    wxString::const_iterator end = s.end();
    while ( it != end && wxIsspace(*it) ) ++it;
    while ( it != end && *it != ';' ) value += *it++;
    value.Trim();
    if ( it != end ) ++it;
    SplitParameters(it, end, parameters);
}

void wxWebRequest::SplitParameters(const wxString::const_iterator& begin,
    const wxString::const_iterator& end, wxWebRequestHeaderMap& parameters)
{
    wxString pname;
    wxString pvalue;
    pname.reserve(32);
    pvalue.reserve(64);
    wxString::const_iterator it = begin;
    while ( it != end )
    {
        pname.clear();
        pvalue.clear();
        while ( it != end && wxIsspace(*it) ) ++it;
        while ( it != end && *it != '=' && *it != ';' ) pname += *it++;
        pname.Trim();
        if ( it != end && *it != ';' ) ++it;
        while ( it != end && wxIsspace(*it) ) ++it;
        while ( it != end && *it != ';' )
        {
            if ( *it == '"' )
            {
                ++it;
                while ( it != end && *it != '"' )
                {
                    if ( *it == '\\' )
                    {
                        ++it;
                        if ( it != end ) pvalue += *it++;
                    }
                    else pvalue += *it++;
                }
                if ( it != end ) ++it;
            }
            else if ( *it == '\\' )
            {
                ++it;
                if ( it != end ) pvalue += *it++;
            }
            else pvalue += *it++;
        }
        pvalue.Trim();
        if ( !pname.empty() ) parameters[pname] = pvalue;
        if ( it != end ) ++it;
    }
}

void wxWebRequest::ProcessStateEvent(State state, const wxString& failMsg)
{
    if (!IsActiveState(state) && GetResponse())
        GetResponse()->Finalize();

    wxString responseFileName;

    wxWebRequestEvent evt(wxEVT_WEBREQUEST_STATE, GetId(), state,
        GetResponse(), failMsg);
    if ( state == State_Completed && m_storage == Storage::Storage_File )
    {
        responseFileName = GetResponse()->GetFileName();
        evt.SetResponseFileName(responseFileName);
    }

    ProcessEvent(evt);

    // Remove temporary file if it still exists
    if ( state == State_Completed && m_storage == Storage::Storage_File &&
        wxFileExists(responseFileName) )
        wxRemoveFile(responseFileName);

    // Remove reference after the request is no longer active
    if ( !IsActiveState(state) )
        DecRef();
}

//
// wxWebResponse
//
wxWebResponse::wxWebResponse(wxWebRequest& request) :
    m_request(request),
    m_readSize(8 * 1024)
{
}

wxWebResponse::~wxWebResponse()
{
    if ( wxFileExists(m_file.GetName()) )
        wxRemoveFile(m_file.GetName());
}

bool wxWebResponse::Init()
{
    if (m_request.GetStorage() == wxWebRequest::Storage_File)
    {
        wxFileName tmpPrefix;
        tmpPrefix.AssignDir(m_request.GetSession().GetTempDir());
        if ( GetContentLength() > 0 )
        {
            // Check available disk space
            wxLongLong freeSpace;
            if ( wxGetDiskSpace(tmpPrefix.GetFullPath(), NULL, &freeSpace) &&
                GetContentLength() > freeSpace )
            {
                m_request.SetState(wxWebRequest::State_Failed, _("Not enough free disk space for download."));
                return false;
            }
        }

        tmpPrefix.SetName("wxd");
        wxFileName::CreateTempFileName(tmpPrefix.GetFullPath(), &m_file);
    }

    return true;
}

wxString wxWebResponse::GetMimeType() const
{
    return GetHeader("Mime-Type");
}

wxInputStream * wxWebResponse::GetStream() const
{
    if ( !m_stream.get() )
    {
        // Create stream
        switch ( m_request.GetStorage() )
        {
            case wxWebRequest::Storage_Memory:
                m_stream.reset(new wxMemoryInputStream(m_readBuffer.GetData(), m_readBuffer.GetDataLen()));
                break;
            case wxWebRequest::Storage_File:
                m_stream.reset(new wxFFileInputStream(m_file));
                m_stream->SeekI(0);
                break;
            case wxWebRequest::Storage_None:
                // No stream available
                break;
        }

    }

    return m_stream.get();
}

wxString wxWebResponse::GetSuggestedFileName() const
{
    wxString suggestedFilename;

    // Try to determine from Content-Disposition header
    wxString contentDisp = GetHeader("Content-Disposition");
    wxString disp;
    wxWebRequestHeaderMap params;
    wxWebRequest::SplitParameters(contentDisp, disp, params);
    if ( disp == "attachment" )
    {
        // Parse as filename to filter potential path names
        wxFileName fn(params["filename"]);
        suggestedFilename = fn.GetFullName();
    }

    if ( suggestedFilename.empty() )
    {
        wxURI uri(GetURL());
        if ( uri.HasPath() )
        {
            wxFileName fn(uri.GetPath());
            suggestedFilename = fn.GetFullName();
        }
        else
            suggestedFilename = uri.GetServer();
    }

    return suggestedFilename;
}

wxString wxWebResponse::AsString(wxMBConv * conv) const
{
    // TODO: try to determine encoding type from content-type header
    if ( !conv )
        conv = &wxConvUTF8;

    if ( m_request.GetStorage() == wxWebRequest::Storage_Memory )
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
    m_request.ReportDataReceived(sizeReceived);

    if ( m_request.GetStorage() == wxWebRequest::Storage_File )
        m_file.Write(m_readBuffer.GetData(), m_readBuffer.GetDataLen());
    else if ( m_request.GetStorage() == wxWebRequest::Storage_None )
    {
        wxWebRequestEvent evt(wxEVT_WEBREQUEST_DATA, m_request.GetId(), wxWebRequest::State_Active);
        evt.SetDataBuffer(m_readBuffer.GetData(), m_readBuffer.GetDataLen());
        m_request.ProcessEvent(evt);
    }

    if ( m_request.GetStorage() != wxWebRequest::Storage_Memory )
        m_readBuffer.Clear();
}

wxString wxWebResponse::GetFileName() const
{
    return m_file.GetName();
}

void wxWebResponse::Finalize()
{
    if ( m_request.GetStorage() == wxWebRequest::Storage_File )
        m_file.Close();
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

wxString wxWebSession::GetTempDir() const
{
    if ( m_tempDir.empty() )
        return wxStandardPaths::Get().GetTempDir();
    else
        return m_tempDir;
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
