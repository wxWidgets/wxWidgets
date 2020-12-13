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

#if wxUSE_WEBREQUEST

#include "wx/webrequest.h"
#include "wx/mstream.h"
#include "wx/module.h"
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
wxWebRequest::wxWebRequest(wxWebSession& session, int id)
    : m_storage(Storage_Memory),
      m_headers(session.m_headers),
      m_dataSize(0),
      m_session(session),
      m_id(id),
      m_state(State_Idle),
      m_ignoreServerErrorStatus(false),
      m_bytesReceived(0)
{
}

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

bool wxWebRequest::SetData(const wxSharedPtr<wxInputStream>& dataStream, const wxString& contentType, wxFileOffset dataSize)
{
    if ( !dataStream->IsOk() )
        return false;

    m_dataStream = dataStream;
    if ( m_dataStream.get() )
    {
        if ( dataSize == wxInvalidOffset )
        {
            // Determine data size
            m_dataSize = m_dataStream->SeekI(0, wxFromEnd);
            if ( m_dataSize == wxInvalidOffset )
                return false;

            m_dataStream->SeekI(0);
        }
        else
            m_dataSize = dataSize;
    }
    else
        m_dataSize = 0;

    SetHeader("Content-Type", contentType);

    return true;
}

wxFileOffset wxWebRequest::GetBytesReceived() const
{
    return m_bytesReceived;
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

// This function is used in a unit test, so define it inside wxPrivate
// namespace and an anonymous one.
namespace wxPrivate
{

WXDLLIMPEXP_NET wxString
SplitParameters(const wxString& s, wxWebRequestHeaderMap& parameters)
{
    wxString value;
    wxString::const_iterator it = s.begin();
    wxString::const_iterator end = s.end();
    while ( it != end && wxIsspace(*it) )
        ++it;
    while ( it != end && *it != ';' )
        value += *it++;
    value.Trim();
    if ( it != end )
        ++it;

    parameters.clear();
    wxString pname;
    wxString pvalue;
    pname.reserve(32);
    pvalue.reserve(64);
    while ( it != end )
    {
        pname.clear();
        pvalue.clear();
        while ( it != end && wxIsspace(*it) )
            ++it;
        while ( it != end && *it != '=' && *it != ';' )
            pname += *it++;
        pname.Trim();
        if ( it != end && *it != ';' )
            ++it;
        while ( it != end && wxIsspace(*it) )
            ++it;
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
                        if ( it != end )
                            pvalue += *it++;
                    }
                    else
                        pvalue += *it++;
                }
                if ( it != end )
                    ++it;
            }
            else if ( *it == '\\' )
            {
                ++it;
                if ( it != end )
                    pvalue += *it++;
            }
            else
                pvalue += *it++;
        }
        pvalue.Trim();
        if ( !pname.empty() )
            parameters[pname] = pvalue;
        if ( it != end )
            ++it;
    }

    return value;
}

} // namespace wxPrivate

void wxWebRequest::ProcessStateEvent(State state, const wxString& failMsg)
{
    if ( !IsActiveState(state) && GetResponse() )
        GetResponse()->Finalize();

    wxString responseFileName;

    wxWebRequestEvent evt(wxEVT_WEBREQUEST_STATE, GetId(), state,
        GetResponse(), failMsg);
    if ( state == State_Completed && m_storage == Storage_File )
    {
        responseFileName = GetResponse()->GetFileName();
        evt.SetResponseFileName(responseFileName);
    }

    ProcessEvent(evt);

    // Remove temporary file if it still exists
    if ( state == State_Completed && m_storage == Storage_File &&
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

void wxWebResponse::Init()
{
    if ( m_request.GetStorage() == wxWebRequest::Storage_File )
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
                return;
            }
        }

        tmpPrefix.SetName("wxd");
        wxFileName::CreateTempFileName(tmpPrefix.GetFullPath(), &m_file);
    }
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
    wxWebRequestHeaderMap params;
    const wxString disp = wxPrivate::SplitParameters(contentDisp, params);
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

wxString wxWebResponse::AsString() const
{
    if ( m_request.GetStorage() == wxWebRequest::Storage_Memory )
    {
        // TODO: try to determine encoding type from content-type header
        size_t outLen = 0;
        return wxConvWhateverWorks.cMB2WC((const char*)m_readBuffer.GetData(), m_readBuffer.GetDataLen(), &outLen);
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
    {
        m_file.Write(m_readBuffer.GetData(), m_readBuffer.GetDataLen());
    }
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

WX_DECLARE_STRING_HASH_MAP(wxSharedPtr<wxWebSessionFactory>, wxStringWebSessionFactoryMap);

namespace
{

wxScopedPtr<wxWebSession> gs_defaultSession;
wxStringWebSessionFactoryMap gs_factoryMap;

} // anonymous namespace

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
    if ( gs_defaultSession == NULL )
        gs_defaultSession.reset(wxWebSession::New());

    return *gs_defaultSession;
}

// static
wxWebSession* wxWebSession::New(const wxString& backend)
{
    if ( gs_factoryMap.empty() )
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = gs_factoryMap.find(backend);
    if ( factory != gs_factoryMap.end() )
        return factory->second->Create();
    else
        return NULL;
}

// static
void
wxWebSession::RegisterFactory(const wxString& backend,
                              const wxSharedPtr<wxWebSessionFactory>& factory)
{
    gs_factoryMap[backend] = factory;
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
    if ( gs_factoryMap.empty() )
        InitFactoryMap();

    wxStringWebSessionFactoryMap::iterator factory = gs_factoryMap.find(backend);
    return factory != gs_factoryMap.end();
}

// ----------------------------------------------------------------------------
// Module ensuring all global/singleton objects are destroyed on shutdown.
// ----------------------------------------------------------------------------

class WebRequestModule : public wxModule
{
public:
    WebRequestModule()
    {
    }

    virtual bool OnInit() wxOVERRIDE
    {
        return true;
    }

    virtual void OnExit() wxOVERRIDE
    {
        gs_factoryMap.clear();
        gs_defaultSession.reset();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(WebRequestModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(WebRequestModule, wxModule);

#endif // wxUSE_WEBREQUEST
