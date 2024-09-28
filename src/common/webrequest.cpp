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

#include "wx/private/webrequest.h"

#if wxUSE_WEBREQUEST_WINHTTP
#include "wx/msw/private/webrequest_winhttp.h"
#endif
#if wxUSE_WEBREQUEST_URLSESSION
#include "wx/osx/private/webrequest_urlsession.h"
#endif
#if wxUSE_WEBREQUEST_CURL
#include "wx/private/webrequest_curl.h"
#endif

#include <memory>
#include <unordered_map>

extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendWinHTTP[] = "WinHTTP";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendURLSession[] = "URLSession";
extern WXDLLIMPEXP_DATA_NET(const char) wxWebSessionBackendCURL[] = "CURL";

wxDEFINE_EVENT(wxEVT_WEBREQUEST_STATE, wxWebRequestEvent);
wxDEFINE_EVENT(wxEVT_WEBREQUEST_DATA, wxWebRequestEvent);

#ifdef __WXDEBUG__
static const wxStringCharType* wxNO_IMPL_MSG
    = wxS("can't be used with an invalid/uninitialized object");
#endif

#define wxCHECK_IMPL(rc) wxCHECK_MSG( m_impl, (rc), wxNO_IMPL_MSG )
#define wxCHECK_IMPL_VOID() wxCHECK_RET( m_impl, wxNO_IMPL_MSG )

//
// wxWebRequestImpl
//
wxWebRequestImpl::wxWebRequestImpl(wxWebSession& session,
                                   wxWebSessionImpl& sessionImpl,
                                   wxEvtHandler* handler,
                                   int id)
    : m_headers(sessionImpl.GetHeaders()),
      m_sessionImpl(sessionImpl),
      m_session(&session),
      m_handler(handler),
      m_id(id)
{
}

wxWebRequestImpl::wxWebRequestImpl(wxWebSessionImpl& sessionImpl)
    : m_headers(sessionImpl.GetHeaders()),
      m_sessionImpl(sessionImpl),
      m_session(nullptr),
      m_handler(nullptr),
      m_id(wxID_NONE)
{
}

void wxWebRequestImpl::Cancel()
{
    wxASSERT( IsAsync() );

    if ( m_cancelled )
    {
        // Nothing to do, don't even assert -- it's ok to call Cancel()
        // multiple times, but calling DoCancel() once is enough.
        return;
    }

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: cancelling", this);

    m_cancelled = true;
    DoCancel();
}

wxString wxWebRequestImpl::GetHTTPMethod() const
{
    if ( !m_method.empty() )
        return m_method.Upper();

    return m_dataSize ? wxASCII_STR("POST") : wxASCII_STR("GET");
}

// static
wxWebRequestSync::Result
wxWebRequestImpl::GetResultFromHTTPStatus(const wxWebResponseImplPtr& resp)
{
    wxCHECK( resp, wxWebRequest::Result::Error("No response object") );

    const int status = resp->GetStatus();

    if ( status == 401 || status == 407 )
    {
        return wxWebRequest::Result::Unauthorized(resp->GetStatusText());
    }
    else if ( status >= 400 )
    {
        return wxWebRequest::Result::Error(wxString::Format(_("Error: %s (%d)"), resp->GetStatusText(), status));
    }
    else
    {
        return wxWebRequest::Result::Ok(wxWebRequest::State_Completed);
    }
}

void wxWebRequestImpl::SetData(const wxString& text, const wxString& contentType, const wxMBConv& conv)
{
    m_dataText = text.mb_str(conv);

    std::unique_ptr<wxInputStream>
        stream(new wxMemoryInputStream(m_dataText, m_dataText.length()));
    SetData(std::move(stream), contentType);
}

bool
wxWebRequestImpl::SetData(std::unique_ptr<wxInputStream> dataStream,
                          const wxString& contentType,
                          wxFileOffset dataSize)
{
    m_dataStream = std::move(dataStream);

    if ( m_dataStream )
    {
        wxCHECK_MSG( m_dataStream->IsOk(), false, "can't use invalid stream" );

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

wxFileOffset wxWebRequestImpl::GetBytesReceived() const
{
    return m_bytesReceived;
}

wxFileOffset wxWebRequestImpl::GetBytesExpectedToReceive() const
{
    if ( GetResponse() )
        return GetResponse()->GetContentLength();
    else
        return -1;
}

namespace
{

#if wxUSE_LOG_TRACE

// Tiny helper to log states as strings rather than meaningless numbers.
wxString StateName(wxWebRequest::State state)
{
    switch ( state )
    {
        case wxWebRequest::State_Idle:            return wxS("IDLE");
        case wxWebRequest::State_Unauthorized:    return wxS("UNAUTHORIZED");
        case wxWebRequest::State_Active:          return wxS("ACTIVE");
        case wxWebRequest::State_Completed:       return wxS("COMPLETED");
        case wxWebRequest::State_Failed:          return wxS("FAILED");
        case wxWebRequest::State_Cancelled:       return wxS("CANCELLED");
    }

    return wxString::Format("invalid state %d", state);
}

#endif // wxUSE_LOG_TRACE

} // anonymous namespace

void wxWebRequestImpl::SetState(wxWebRequest::State state, const wxString & failMsg)
{
    wxCHECK_RET( state != m_state, "shouldn't switch to the same state" );

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: state %s => %s",
               this, StateName(m_state), StateName(state));

    if ( state == wxWebRequest::State_Active )
    {
        // The request is now in progress (maybe not for the first time if the
        // old state was State_Unauthorized and not State_Idle), ensure that it
        // stays alive until it terminates, even if wxWebRequest object itself
        // is deleted.
        //
        // Note that matching DecRef() is done by ProcessStateEvent() later.
        IncRef();
    }

    m_state = state;

    // Trigger the event in the main thread except when switching to the active
    // state because this always happens in the main thread anyhow and it's
    // important to process it synchronously, before the request actually
    // starts (this gives the possibility to modify the request using native
    // functions, for example, as its GetNativeHandle() is already valid).
    if ( state == wxWebRequest::State_Active )
    {
        wxASSERT( wxIsMainThread() );

        ProcessStateEvent(state, failMsg);
    }
    else
    {
        m_handler->CallAfter([this, state, failMsg]()
            {
                ProcessStateEvent(state, failMsg);
            });
    }
}

void wxWebRequestImpl::ReportDataReceived(size_t sizeReceived)
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

void wxWebRequestImpl::ProcessStateEvent(wxWebRequest::State state, const wxString& failMsg)
{
    wxString dataFile;

    // wxObjectDataPtr ctor takes ownership of raw pointer, so compensate
    // DecRef() that will be done in its dtor.
    IncRef();
    const wxWebRequestImplPtr request(this);

    const wxWebResponseImplPtr& response = GetResponse();

    wxWebRequestEvent evt(wxEVT_WEBREQUEST_STATE, GetId(), state,
                          wxWebRequest(request), wxWebResponse(response), failMsg);

    bool release = false;
    switch ( state )
    {
        case wxWebRequest::State_Idle:
            wxFAIL_MSG("unexpected");
            break;

        case wxWebRequest::State_Active:
            break;

        case wxWebRequest::State_Unauthorized:
            // This one is tricky: we might not be done with the request yet,
            // but we don't know if this is the case or not, i.e. if the
            // application will call wxWebAuthChallenge::SetCredentials() or
            // not later. So we release it now, as we assume that it still
            // keeps a reference to the original request if it intends to do it
            // anyhow, i.e. this won't actually destroy the request object in
            // this case.
            release = true;
            break;

        case wxWebRequest::State_Completed:
            if ( m_storage == wxWebRequest::Storage_File )
            {
                dataFile = response->GetDataFile();
                evt.SetDataFile(dataFile);
            }
            wxFALLTHROUGH;

        case wxWebRequest::State_Failed:
        case wxWebRequest::State_Cancelled:
            if ( response )
                response->Finalize();

            release = true;
            break;
    }

    m_handler->ProcessEvent(evt);

    // Remove temporary file if we're using one and if it still exists: it
    // could have been deleted or moved away by the event handler.
    if ( !dataFile.empty() && wxFileExists(dataFile) )
        wxRemoveFile(dataFile);

    // This may destroy this object if it's not used from elsewhere any longer.
    if ( release )
        DecRef();
}

//
// wxWebRequest
//

wxWebRequestBase::wxWebRequestBase() = default;

wxWebRequestBase::wxWebRequestBase(const wxWebRequestImplPtr& impl)
    : m_impl(impl)
{
}

wxWebRequestBase::wxWebRequestBase(const wxWebRequestBase&) = default;

wxWebRequestBase&
wxWebRequestBase::operator=(const wxWebRequestBase&) = default;

wxWebRequestBase::~wxWebRequestBase() = default;

void wxWebRequestBase::SetHeader(const wxString& name, const wxString& value)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetHeader(name, value);
}

void wxWebRequestBase::SetMethod(const wxString& method)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetMethod(method);
}

void
wxWebRequestBase::SetData(const wxString& text,
                          const wxString& contentType,
                          const wxMBConv& conv)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetData(text, contentType, conv);
}

bool
wxWebRequestBase::SetData(std::unique_ptr<wxInputStream> dataStream,
                          const wxString& contentType,
                          wxFileOffset dataSize)
{
    wxCHECK_IMPL( false );

    return m_impl->SetData(std::move(dataStream), contentType, dataSize);
}

void wxWebRequestBase::SetStorage(Storage storage)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetStorage(storage);
}

wxWebRequestBase::Storage wxWebRequestBase::GetStorage() const
{
    wxCHECK_IMPL( Storage_None );

    return m_impl->GetStorage();
}

wxWebRequestSync::Result wxWebRequestSync::Execute() const
{
    wxCHECK_IMPL( wxWebRequestSync::Result::Error("Invalid session object") );

    return m_impl->Execute();
}

void wxWebRequest::Start()
{
    wxCHECK_IMPL_VOID();

    wxCHECK_RET( m_impl->GetState() == wxWebRequest::State_Idle,
                 "Completed requests can not be restarted" );

    m_impl->Start();
}

void wxWebRequest::Cancel()
{
    wxCHECK_IMPL_VOID();

    wxCHECK_RET( m_impl->GetState() != wxWebRequest::State_Idle,
                 "Not yet started requests can't be cancelled" );

    m_impl->Cancel();
}

wxWebResponse wxWebRequestBase::GetResponse() const
{
    wxCHECK_IMPL( wxWebResponse() );

    return wxWebResponse(m_impl->GetResponse());
}

wxWebAuthChallenge wxWebRequest::GetAuthChallenge() const
{
    wxCHECK_IMPL( wxWebAuthChallenge() );

    return wxWebAuthChallenge(m_impl->GetAuthChallenge());
}

int wxWebRequest::GetId() const
{
    wxCHECK_IMPL( wxID_ANY );

    return m_impl->GetId();
}

wxWebSession& wxWebRequest::GetSession() const
{
    wxCHECK_IMPL( wxWebSession::GetDefault() );

    return m_impl->GetSession();
}

wxWebRequest::State wxWebRequest::GetState() const
{
    wxCHECK_IMPL( State_Failed );

    return m_impl->GetState();
}

wxFileOffset wxWebRequestBase::GetBytesSent() const
{
    wxCHECK_IMPL( wxInvalidOffset );

    return m_impl->GetBytesSent();
}

wxFileOffset wxWebRequestBase::GetBytesExpectedToSend() const
{
    wxCHECK_IMPL( wxInvalidOffset );

    return m_impl->GetBytesExpectedToSend();
}

wxFileOffset wxWebRequestBase::GetBytesReceived() const
{
    wxCHECK_IMPL( wxInvalidOffset );

    return m_impl->GetBytesReceived();
}

wxFileOffset wxWebRequestBase::GetBytesExpectedToReceive() const
{
    wxCHECK_IMPL( wxInvalidOffset );

    return m_impl->GetBytesExpectedToReceive();
}

wxWebRequestHandle wxWebRequestBase::GetNativeHandle() const
{
    return m_impl ? m_impl->GetNativeHandle() : nullptr;
}

void wxWebRequestBase::MakeInsecure(int flags)
{
    m_impl->MakeInsecure(flags);
}

int wxWebRequestBase::GetSecurityFlags() const
{
    return m_impl->GetSecurityFlags();
}




//
// wxWebAuthChallenge
//

wxWebAuthChallenge::wxWebAuthChallenge() = default;

wxWebAuthChallenge::wxWebAuthChallenge(const wxWebAuthChallengeImplPtr& impl)
    : m_impl(impl)
{
}

wxWebAuthChallenge::wxWebAuthChallenge(const wxWebAuthChallenge&) = default;

wxWebAuthChallenge&
wxWebAuthChallenge::operator=(const wxWebAuthChallenge&) = default;

wxWebAuthChallenge::~wxWebAuthChallenge() = default;

wxWebAuthChallenge::Source wxWebAuthChallenge::GetSource() const
{
    wxCHECK_IMPL( Source_Server );

    return m_impl->GetSource();
}

void
wxWebAuthChallenge::SetCredentials(const wxWebCredentials& cred)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetCredentials(cred);
}

//
// wxWebResponseImpl
//

wxWebResponseImpl::wxWebResponseImpl(wxWebRequestImpl& request) :
    m_request(request)
{
}

wxWebResponseImpl::~wxWebResponseImpl()
{
    if ( wxFileExists(m_file.GetName()) )
        wxRemoveFile(m_file.GetName());
}

wxWebRequest::Result wxWebResponseImpl::InitFileStorage()
{
    if ( m_request.GetStorage() == wxWebRequest::Storage_File )
    {
        wxFileName tmpPrefix;
        tmpPrefix.AssignDir(m_request.GetSessionImpl().GetTempDir());

        const auto neededSize = GetContentLength();
        if ( neededSize > 0 )
        {
            // Check available disk space
            wxLongLong freeSpace;
            if ( wxGetDiskSpace(tmpPrefix.GetFullPath(), nullptr, &freeSpace) &&
                    neededSize > freeSpace )
            {
                return wxWebRequest::Result::Error
                    (
                        wxString::Format
                        (
                         _("Not enough free disk space for download: %llu needed but only %llu available."),
                         neededSize, freeSpace
                        )
                    );
            }
        }

        tmpPrefix.SetName("wxd");
        if ( wxFileName::CreateTempFileName(tmpPrefix.GetFullPath(), &m_file).empty() )
        {
            return wxWebRequest::Result::Error
                (
                    wxString::Format
                    (
                     _("Failed to create temporary file in %s"),
                     tmpPrefix.GetFullPath()
                    )
                );
        }
    }

    return wxWebRequest::Result::Ok();
}

wxString wxWebResponseImpl::GetMimeType() const
{
    return GetContentType().BeforeFirst(';');
}

wxString wxWebResponseImpl::GetContentType() const
{
    return GetHeader("Content-Type");
}

wxInputStream * wxWebResponseImpl::GetStream() const
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

wxString wxWebResponseImpl::GetSuggestedFileName() const
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

wxString wxWebResponseImpl::AsString() const
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

void* wxWebResponseImpl::GetDataBuffer(size_t sizeNeeded)
{
    return m_readBuffer.GetAppendBuf(sizeNeeded);
}

void wxWebResponseImpl::PreAllocBuffer(size_t sizeNeeded)
{
    m_readBuffer.SetBufSize(sizeNeeded);
}

void wxWebResponseImpl::ReportDataReceived(size_t sizeReceived)
{
    m_readBuffer.UngetAppendBuf(sizeReceived);
    m_request.ReportDataReceived(sizeReceived);

    switch ( m_request.GetStorage() )
    {
        case wxWebRequest::Storage_Memory:
            // Nothing to do, just keep appending data to the buffer.
            break;

        case wxWebRequest::Storage_File:
            m_file.Write(m_readBuffer.GetData(), m_readBuffer.GetDataLen());
            m_readBuffer.Clear();
            break;

        case wxWebRequest::Storage_None:
            // We don't use events for synchronous requests, so just throw the
            // data away. This is not very useful, but what else can we do.
            if ( !m_request.IsAsync() )
                break;

            m_request.IncRef();
            const wxWebRequestImplPtr request(&m_request);

            IncRef();
            const wxWebResponseImplPtr response(this);

            wxWebRequestEvent* const evt = new wxWebRequestEvent
                                               (
                                                wxEVT_WEBREQUEST_DATA,
                                                m_request.GetId(),
                                                wxWebRequest::State_Active,
                                                wxWebRequest(request),
                                                wxWebResponse(response)
                                               );
            evt->SetDataBuffer(m_readBuffer);

            m_request.GetHandler()->QueueEvent(evt);

            // Make sure we switch to a different buffer instead of just
            // clearing the current one, which will be needed by the event
            // handler when it's finally called in the main thread.
            m_readBuffer = wxMemoryBuffer();
            break;
    }
}

wxString wxWebResponseImpl::GetDataFile() const
{
    return m_file.GetName();
}

void wxWebResponseImpl::Finalize()
{
    if ( m_request.GetStorage() == wxWebRequest::Storage_File )
        m_file.Close();
}

//
// wxWebResponse
//

wxWebResponse::wxWebResponse() = default;

wxWebResponse::wxWebResponse(const wxWebResponseImplPtr& impl)
    : m_impl(impl)
{
}

wxWebResponse::wxWebResponse(const wxWebResponse&) = default;

wxWebResponse& wxWebResponse::operator=(const wxWebResponse&) = default;

wxWebResponse::~wxWebResponse() = default;

wxFileOffset wxWebResponse::GetContentLength() const
{
    wxCHECK_IMPL( -1 );

    return m_impl->GetContentLength();
}

wxString wxWebResponse::GetURL() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetURL();
}

wxString wxWebResponse::GetHeader(const wxString& name) const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetHeader(name);
}

wxString wxWebResponse::GetMimeType() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetMimeType();
}

wxString wxWebResponse::GetContentType() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetContentType();
}

int wxWebResponse::GetStatus() const
{
    wxCHECK_IMPL( -1 );

    return m_impl->GetStatus();
}

wxString wxWebResponse::GetStatusText() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetStatusText();
}

wxInputStream* wxWebResponse::GetStream() const
{
    wxCHECK_IMPL( nullptr );

    return m_impl->GetStream();
}

wxString wxWebResponse::GetSuggestedFileName() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetSuggestedFileName();
}

wxString wxWebResponse::AsString() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->AsString();
}

wxString wxWebResponse::GetDataFile() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetDataFile();
}


//
// wxWebSessionImpl
//

namespace
{

wxWebSession gs_defaultSession;
wxWebSessionSync gs_defaultSessionSync;

std::unordered_map<wxString, std::unique_ptr<wxWebSessionFactory>> gs_factoryMap;

} // anonymous namespace

wxWebSessionImpl::wxWebSessionImpl(Mode mode)
    : m_mode(mode)
{
    // Initialize the user-Agent header with a reasonable default
    AddCommonHeader("User-Agent", wxString::Format("%s/1 wxWidgets/%d.%d.%d",
        wxTheApp->GetAppName(),
        wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER));
}

wxWebSessionImpl::~wxWebSessionImpl() = default;

bool wxWebSessionImpl::SetBaseURL(const wxString& url)
{
    // For things to work as expected, i.e. append relative URLs to the base
    // one without replacing its last component, it must end with a slash.
    wxString urlWithSlash = url;
    if ( !urlWithSlash.EndsWith("/") )
        urlWithSlash += '/';

    m_baseURL.reset(new wxURI());
    if ( !m_baseURL->Create(urlWithSlash) || m_baseURL->IsReference() )
    {
        m_baseURL.reset();
        return false;
    }

    wxLogTrace(wxTRACE_WEBREQUEST, "Using base URL: %s (%s)",
               url, m_baseURL->BuildURI());

    return true;
}

const wxURI* wxWebSessionImpl::GetBaseURL() const
{
    return m_baseURL ? m_baseURL.get() : nullptr;
}

wxString wxWebSessionImpl::GetTempDir() const
{
    if ( m_tempDir.empty() )
        return wxStandardPaths::Get().GetTempDir();
    else
        return m_tempDir;
}

//
// wxWebSessionBase and its derived wxWebSession and wxWebSessionSync classes
//

wxWebSessionBase::wxWebSessionBase() = default;

wxWebSessionBase::wxWebSessionBase(const wxWebSessionImplPtr& impl)
    : m_impl(impl)
{
}

wxWebSessionBase::wxWebSessionBase(const wxWebSessionBase&) = default;

wxWebSessionBase&
wxWebSessionBase::operator=(const wxWebSessionBase&) = default;

wxWebSessionBase::~wxWebSessionBase() = default;

// static
wxWebSession& wxWebSession::GetDefault()
{
    if ( !gs_defaultSession.IsOpened() )
        gs_defaultSession = wxWebSession::New();

    return gs_defaultSession;
}

// static
wxWebSessionSync& wxWebSessionSync::GetDefault()
{
    if ( !gs_defaultSessionSync.IsOpened() )
        gs_defaultSessionSync = wxWebSessionSync::New();

    return gs_defaultSessionSync;
}

// static
wxWebSessionFactory* wxWebSessionBase::FindFactory(const wxString& backendOrig)
{
    if ( gs_factoryMap.empty() )
        InitFactoryMap();

    wxString backend = backendOrig;
    if ( backend.empty() )
    {
        if ( !wxGetEnv("WXWEBREQUEST_BACKEND", &backend) )
        {
#if wxUSE_WEBREQUEST_WINHTTP
            backend = wxWebSessionBackendWinHTTP;
#elif wxUSE_WEBREQUEST_URLSESSION
            backend = wxWebSessionBackendURLSession;
#elif wxUSE_WEBREQUEST_CURL
            backend = wxWebSessionBackendCURL;
#endif
        }
    }

    const auto it = gs_factoryMap.find(backend);

    return it != gs_factoryMap.end() ? it->second.get() : nullptr;
}

// static
wxWebSession wxWebSession::New(const wxString& backend)
{
    wxWebSessionImplPtr impl;

    auto* const factory = FindFactory(backend);
    if ( factory )
        impl = factory->Create();

    return wxWebSession(impl);
}

// static
wxWebSessionSync wxWebSessionSync::New(const wxString& backend)
{
    wxWebSessionImplPtr impl;

    auto* const factory = FindFactory(backend);
    if ( factory )
        impl = factory->CreateSync();

    return wxWebSessionSync(impl);
}

// static
void
wxWebSessionBase::RegisterFactory(const wxString& backend,
                                  wxWebSessionFactory* factory)
{
    // Ensure that the pointer is always freed.
    std::unique_ptr<wxWebSessionFactory> ptr{factory};

    if ( !factory->Initialize() )
        return;

    gs_factoryMap[backend] = std::move(ptr);
}

// static
void wxWebSessionBase::InitFactoryMap()
{
#if wxUSE_WEBREQUEST_WINHTTP
    RegisterFactory(wxWebSessionBackendWinHTTP, new wxWebSessionFactoryWinHTTP());
#endif
#if wxUSE_WEBREQUEST_URLSESSION
    RegisterFactory(wxWebSessionBackendURLSession, new wxWebSessionFactoryURLSession());
#endif
#if wxUSE_WEBREQUEST_CURL
    RegisterFactory(wxWebSessionBackendCURL, new wxWebSessionFactoryCURL());
#endif
}

// static
bool wxWebSessionBase::IsBackendAvailable(const wxString& backend)
{
    if ( gs_factoryMap.empty() )
        InitFactoryMap();

    const auto factory = gs_factoryMap.find(backend);
    return factory != gs_factoryMap.end();
}

wxString wxWebSessionBase::GetFullURL(const wxString& url) const
{
    const wxURI* const baseURL = m_impl->GetBaseURL();

    // If we don't have a base URL, just return the original URL.
    if ( !baseURL )
        return url;

    wxURI absURL(url);
    absURL.Resolve(*baseURL);

    wxLogTrace(wxTRACE_WEBREQUEST, "Relative URL: %s -> %s",
               url, absURL.BuildURI());

    return absURL.BuildURI();
}

wxWebRequest
wxWebSession::CreateRequest(wxEvtHandler* handler, const wxString& url, int id)
{
    wxCHECK_IMPL( wxWebRequest() );

    return wxWebRequest(m_impl->CreateRequest(*this, handler, GetFullURL(url), id));
}

wxWebRequestSync
wxWebSessionSync::CreateRequest(const wxString& url)
{
    wxCHECK_IMPL( wxWebRequestSync() );

    return wxWebRequestSync(m_impl->CreateRequestSync(*this, GetFullURL(url)));
}

wxVersionInfo wxWebSessionBase::GetLibraryVersionInfo() const
{
    wxCHECK_IMPL( wxVersionInfo() );

    return m_impl->GetLibraryVersionInfo();
}

bool wxWebSessionBase::SetBaseURL(const wxString& url)
{
    wxCHECK_IMPL( false );

    return m_impl->SetBaseURL(url);
}

void wxWebSessionBase::AddCommonHeader(const wxString& name, const wxString& value)
{
    wxCHECK_IMPL_VOID();

    m_impl->AddCommonHeader(name, value);
}

void wxWebSessionBase::SetTempDir(const wxString& dir)
{
    wxCHECK_IMPL_VOID();

    m_impl->SetTempDir(dir);
}

wxString wxWebSessionBase::GetTempDir() const
{
    wxCHECK_IMPL( wxString() );

    return m_impl->GetTempDir();
}

bool wxWebSessionBase::SetProxy(const wxWebProxy& proxy)
{
    wxCHECK_IMPL( false );

    return m_impl->SetProxy(proxy);
}

bool wxWebSessionBase::IsOpened() const
{
    return m_impl.get() != nullptr;
}

void wxWebSessionBase::Close()
{
    m_impl.reset(nullptr);
}

wxWebSessionHandle wxWebSessionBase::GetNativeHandle() const
{
    return m_impl ? m_impl->GetNativeHandle() : nullptr;
}

bool wxWebSessionBase::EnablePersistentStorage(bool enable)
{
    wxCHECK_IMPL( false );

    return m_impl->EnablePersistentStorage(enable);
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

    virtual bool OnInit() override
    {
        return true;
    }

    virtual void OnExit() override
    {
        gs_factoryMap.clear();
        gs_defaultSession.Close();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(WebRequestModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(WebRequestModule, wxModule);

#endif // wxUSE_WEBREQUEST
