///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/webrequest_winhttp.h
// Purpose:     wxWebRequest WinHTTP implementation
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST && wxUSE_WEBREQUEST_WINHTTP

#include "wx/mstream.h"
#include "wx/dynlib.h"
#include "wx/msw/private.h"
#include "wx/msw/private/webrequest_winhttp.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/translation.h"
#endif

// Buffer size used for writing/reading data to/from the network.
constexpr int wxWEBREQUEST_BUFFER_SIZE = 64 * 1024;

// Helper class used to dynamically load the required symbols from winhttp.dll
class wxWinHTTP
{
public:
    static bool LoadLibrary()
    {
        bool result = m_winhttp.Load("winhttp.dll", wxDL_VERBATIM | wxDL_QUIET);
        if ( !result )
            return result;

        #define wxLOAD_FUNC(name)               \
            wxDL_INIT_FUNC(, name, m_winhttp);  \
            result &= (name != nullptr);

        wxLOAD_FUNC(WinHttpQueryOption)
        wxLOAD_FUNC(WinHttpQueryHeaders)
        wxLOAD_FUNC(WinHttpSetOption)
        wxLOAD_FUNC(WinHttpWriteData)
        wxLOAD_FUNC(WinHttpCloseHandle)
        wxLOAD_FUNC(WinHttpReceiveResponse)
        wxLOAD_FUNC(WinHttpCrackUrl)
        wxLOAD_FUNC(WinHttpCreateUrl)
        wxLOAD_FUNC(WinHttpConnect)
        wxLOAD_FUNC(WinHttpOpenRequest)
        wxLOAD_FUNC(WinHttpSetStatusCallback)
        wxLOAD_FUNC(WinHttpSendRequest)
        wxLOAD_FUNC(WinHttpReadData)
        wxLOAD_FUNC(WinHttpQueryAuthSchemes)
        wxLOAD_FUNC(WinHttpSetCredentials)
        wxLOAD_FUNC(WinHttpOpen)

        if ( !result )
            m_winhttp.Unload();

        return result;
    }

    typedef BOOL(WINAPI* WinHttpQueryOption_t)(HINTERNET, DWORD, LPVOID, LPDWORD);
    static WinHttpQueryOption_t WinHttpQueryOption;
    typedef BOOL(WINAPI* WinHttpQueryHeaders_t)(HINTERNET, DWORD, LPCWSTR, LPVOID, LPDWORD, LPDWORD);
    static WinHttpQueryHeaders_t WinHttpQueryHeaders;
    typedef BOOL(WINAPI* WinHttpSetOption_t)(HINTERNET, DWORD, LPVOID, DWORD);
    static WinHttpSetOption_t WinHttpSetOption;
    typedef BOOL(WINAPI* WinHttpWriteData_t)(HINTERNET, LPCVOID, DWORD, LPDWORD);
    static WinHttpWriteData_t WinHttpWriteData;
    typedef BOOL(WINAPI* WinHttpCloseHandle_t)(HINTERNET);
    static WinHttpCloseHandle_t WinHttpCloseHandle;
    typedef BOOL(WINAPI* WinHttpReceiveResponse_t)(HINTERNET, LPVOID);
    static WinHttpReceiveResponse_t WinHttpReceiveResponse;
    typedef BOOL(WINAPI* WinHttpCrackUrl_t)(LPCWSTR, DWORD, DWORD, LPURL_COMPONENTS);
    static WinHttpCrackUrl_t WinHttpCrackUrl;
    typedef BOOL(WINAPI* WinHttpCreateUrl_t)(LPURL_COMPONENTS, DWORD, LPWSTR, LPDWORD);
    static WinHttpCreateUrl_t WinHttpCreateUrl;
    typedef HINTERNET(WINAPI* WinHttpConnect_t)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
    static WinHttpConnect_t WinHttpConnect;
    typedef HINTERNET(WINAPI* WinHttpOpenRequest_t)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
    static WinHttpOpenRequest_t WinHttpOpenRequest;
    typedef WINHTTP_STATUS_CALLBACK(WINAPI* WinHttpSetStatusCallback_t)(HINTERNET, WINHTTP_STATUS_CALLBACK, DWORD, DWORD_PTR);
    static WinHttpSetStatusCallback_t WinHttpSetStatusCallback;
    typedef BOOL(WINAPI* WinHttpSendRequest_t)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
    static WinHttpSendRequest_t WinHttpSendRequest;
    typedef BOOL(WINAPI* WinHttpReadData_t)(HINTERNET, LPVOID, DWORD, LPDWORD);
    static WinHttpReadData_t WinHttpReadData;
    typedef BOOL(WINAPI* WinHttpQueryAuthSchemes_t)(HINTERNET, LPDWORD, LPDWORD, LPDWORD);
    static WinHttpQueryAuthSchemes_t WinHttpQueryAuthSchemes;
    typedef BOOL(WINAPI* WinHttpSetCredentials_t)(HINTERNET, DWORD, DWORD, LPCWSTR, LPCWSTR, LPVOID);
    static WinHttpSetCredentials_t WinHttpSetCredentials;
    typedef HINTERNET(WINAPI* WinHttpOpen_t)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    static WinHttpOpen_t WinHttpOpen;

private:
    static wxDynamicLibrary m_winhttp;
};

wxDynamicLibrary wxWinHTTP::m_winhttp;
wxWinHTTP::WinHttpQueryOption_t wxWinHTTP::WinHttpQueryOption;
wxWinHTTP::WinHttpQueryHeaders_t wxWinHTTP::WinHttpQueryHeaders;
wxWinHTTP::WinHttpSetOption_t wxWinHTTP::WinHttpSetOption;
wxWinHTTP::WinHttpWriteData_t wxWinHTTP::WinHttpWriteData;
wxWinHTTP::WinHttpCloseHandle_t wxWinHTTP::WinHttpCloseHandle;
wxWinHTTP::WinHttpReceiveResponse_t wxWinHTTP::WinHttpReceiveResponse;
wxWinHTTP::WinHttpCrackUrl_t wxWinHTTP::WinHttpCrackUrl;
wxWinHTTP::WinHttpCreateUrl_t wxWinHTTP::WinHttpCreateUrl;
wxWinHTTP::WinHttpConnect_t wxWinHTTP::WinHttpConnect;
wxWinHTTP::WinHttpOpenRequest_t wxWinHTTP::WinHttpOpenRequest;
wxWinHTTP::WinHttpSetStatusCallback_t wxWinHTTP::WinHttpSetStatusCallback;
wxWinHTTP::WinHttpSendRequest_t wxWinHTTP::WinHttpSendRequest;
wxWinHTTP::WinHttpReadData_t wxWinHTTP::WinHttpReadData;
wxWinHTTP::WinHttpQueryAuthSchemes_t wxWinHTTP::WinHttpQueryAuthSchemes;
wxWinHTTP::WinHttpSetCredentials_t wxWinHTTP::WinHttpSetCredentials;
wxWinHTTP::WinHttpOpen_t wxWinHTTP::WinHttpOpen;


// Define constants potentially missing in old SDKs
#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif
#ifndef WINHTTP_PROTOCOL_FLAG_HTTP2
#define WINHTTP_PROTOCOL_FLAG_HTTP2 0x1
#endif
#ifndef WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL
#define WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL 133
#endif
#ifndef WINHTTP_DECOMPRESSION_FLAG_ALL
#define WINHTTP_DECOMPRESSION_FLAG_GZIP 0x00000001
#define WINHTTP_DECOMPRESSION_FLAG_DEFLATE 0x00000002
#define WINHTTP_DECOMPRESSION_FLAG_ALL ( \
    WINHTTP_DECOMPRESSION_FLAG_GZIP | \
    WINHTTP_DECOMPRESSION_FLAG_DEFLATE)
#endif
#ifndef WINHTTP_OPTION_DECOMPRESSION
#define WINHTTP_OPTION_DECOMPRESSION 118
#endif
#ifndef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1
#define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 0x00000200
#endif
#ifndef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2
#define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 0x00000800
#endif

namespace
{

// Wrapper initializing URL_COMPONENTS struct.
struct wxURLComponents : URL_COMPONENTS
{
    wxURLComponents()
    {
        wxZeroMemory(*this);
        dwStructSize = sizeof(URL_COMPONENTS);
        dwSchemeLength =
        dwHostNameLength =
        dwUserNameLength =
        dwPasswordLength =
        dwUrlPathLength =
        dwExtraInfoLength = (DWORD)-1;
    }

    bool HasCredentials() const
    {
        return dwUserNameLength > 0;
    }

    wxWebCredentials GetCredentials() const
    {
        return wxWebCredentials
               (
                wxString(lpszUserName, dwUserNameLength),
                wxSecretValue(wxString(lpszPassword, dwPasswordLength))
               );
    }
};

} // anonymous namespace

// Helper functions

static wxString wxWinHTTPQueryHeaderString(HINTERNET hRequest, DWORD dwInfoLevel,
    LPCWSTR pwszName = WINHTTP_HEADER_NAME_BY_INDEX)
{
    wxString result;
    DWORD bufferLen = 0;
    wxWinHTTP::WinHttpQueryHeaders(hRequest, dwInfoLevel, pwszName, nullptr, &bufferLen,
        WINHTTP_NO_HEADER_INDEX);
    if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
    {
        // Buffer length is in bytes, including the terminating (wide) NUL, but
        // wxWCharBuffer needs the size in characters and adds NUL itself.
        if ( !bufferLen || (bufferLen % sizeof(wchar_t)) )
        {
            wxLogDebug("Unexpected size of header %s: %lu", pwszName, bufferLen);
            return wxString();
        }

        wxWCharBuffer resBuf(bufferLen / sizeof(wchar_t) - 1);
        if ( wxWinHTTP::WinHttpQueryHeaders(hRequest, dwInfoLevel, pwszName,
                                   resBuf.data(), &bufferLen,
                                   WINHTTP_NO_HEADER_INDEX) )
        {
            result.assign(resBuf);
        }
    }

    return result;
}

static wxString wxWinHTTPQueryOptionString(HINTERNET hInternet, DWORD dwOption)
{
    wxString result;
    DWORD bufferLen = 0;
    wxWinHTTP::WinHttpQueryOption(hInternet, dwOption, nullptr, &bufferLen);
    if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
    {
        // Same as above: convert length in bytes into size in characters.
        if ( !bufferLen || (bufferLen % sizeof(wchar_t)) )
        {
            wxLogDebug("Unexpected size of option %lu: %lu", dwOption, bufferLen);
            return wxString();
        }

        wxWCharBuffer resBuf(bufferLen / sizeof(wchar_t) - 1);
        if ( wxWinHTTP::WinHttpQueryOption(hInternet, dwOption, resBuf.data(), &bufferLen) )
            result.assign(resBuf);
    }

    return result;
}

static inline
void wxWinHTTPSetOption(HINTERNET hInternet, DWORD dwOption, DWORD dwValue)
{
    wxWinHTTP::WinHttpSetOption(hInternet, dwOption, &dwValue, sizeof(dwValue));
}

static
void wxWinHTTPCloseHandle(HINTERNET hInternet)
{
    if ( !wxWinHTTP::WinHttpCloseHandle(hInternet) )
    {
        wxLogLastError("WinHttpCloseHandle");
    }
}

static void CALLBACK wxRequestStatusCallback(
    HINTERNET WXUNUSED(hInternet),
    DWORD_PTR dwContext,
    DWORD dwInternetStatus,
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
)
{
    if ( dwContext )
    {
        wxWebRequestWinHTTP* request =
            reinterpret_cast<wxWebRequestWinHTTP*>(dwContext);
        request->HandleCallback(dwInternetStatus, lpvStatusInformation,
            dwStatusInformationLength);
    }
}

//
// wxWebRequestWinHTTP
//

wxWebRequestWinHTTP::wxWebRequestWinHTTP(wxWebSession& session,
                                         wxWebSessionWinHTTP& sessionImpl,
                                         wxEvtHandler* handler,
                                         const wxString& url,
                                         int id):
    wxWebRequestImpl(session, sessionImpl, handler, id),
    m_sessionImpl(sessionImpl),
    m_url(url),
    m_tryProxyCredentials(sessionImpl.HasProxyCredentials())
{
}

wxWebRequestWinHTTP::wxWebRequestWinHTTP(wxWebSessionWinHTTP& sessionImpl,
                                         const wxString& url)
    : wxWebRequestImpl(sessionImpl),
      m_sessionImpl(sessionImpl),
      m_url(url),
      m_tryProxyCredentials(sessionImpl.HasProxyCredentials())
{
}

wxWebRequestWinHTTP::~wxWebRequestWinHTTP()
{
    if ( m_request )
        wxWinHTTPCloseHandle(m_request);
    if ( m_connect )
        wxWinHTTPCloseHandle(m_connect);
}

void
wxWebRequestWinHTTP::HandleCallback(DWORD dwInternetStatus,
                                    LPVOID lpvStatusInformation,
                                    DWORD dwStatusInformationLength)
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: callback %08x, len=%lu",
               this, dwInternetStatus, dwStatusInformationLength);

    switch ( dwInternetStatus )
    {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
            // If there is no data to write, this will call CreateResponse().
            WriteData();
            break;

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            if ( dwStatusInformationLength > 0 )
            {
                m_response->ReportDataReceived(dwStatusInformationLength);
                if ( !m_response->ReadData() && !WasCancelled() )
                    SetFailedWithLastError("Reading data");
            }
            else
            {
                SetFinalStateFromStatus();
            }
            break;

        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
        {
            DWORD written = *(reinterpret_cast<LPDWORD>(lpvStatusInformation));
            m_dataWritten += written;
            WriteData();
            break;
        }

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        {
            LPWINHTTP_ASYNC_RESULT
                asyncResult = reinterpret_cast<LPWINHTTP_ASYNC_RESULT>(lpvStatusInformation);

            // "Failing" with "cancelled" error is not actually an error if
            // we're expecting it, i.e. if our Cancel() had been called.
            if ( asyncResult->dwError == ERROR_WINHTTP_OPERATION_CANCELLED &&
                    WasCancelled() )
                SetState(wxWebRequest::State_Cancelled);
            else
                SetFailed("Async request", asyncResult->dwError);
            break;
        }
    }
}

void wxWebRequestWinHTTP::WriteData()
{
    if ( m_dataWritten == m_dataSize )
    {
        if ( !CheckResult(CreateResponse()) )
            return;

        const auto result = InitAuthIfNeeded();
        switch ( result.state )
        {
            case wxWebRequest::State_Unauthorized:
                switch ( m_authChallenge->GetSource() )
                {
                    case wxWebAuthChallenge::Source_Proxy:
                        if ( m_tryProxyCredentials )
                        {
                            m_tryProxyCredentials = false;

                            m_authChallenge->SetCredentials(
                                m_sessionImpl.GetProxyCredentials()
                            );
                            return;
                        }
                        break;

                    case wxWebAuthChallenge::Source_Server:
                        // Check if we can use the credentials from the URL.
                        if ( m_tryCredentialsFromURL )
                        {
                            m_tryCredentialsFromURL = false;

                            // We may need to retry proxy credentials if we get
                            // redirected, as we'd need to authenticate with
                            // the proxy again in this case.
                            if ( m_sessionImpl.HasProxyCredentials() )
                                m_tryProxyCredentials = true;

                            m_authChallenge->SetCredentials(m_credentialsFromURL);
                            return;
                        }
                        break;
                }

                // Otherwise just switch to this state and let the application
                // call SetCredentials() later.
                wxFALLTHROUGH;

            case wxWebRequest::State_Failed:
                // In case of any other error, we can't continue.
                HandleResult(result);
                return;

            case wxWebRequest::State_Active:
                // Continue normally.
                break;

            case wxWebRequest::State_Idle:
            case wxWebRequest::State_Completed:
            case wxWebRequest::State_Cancelled:
                wxFAIL_MSG("Unexpected state");
                break;
        }

        // Start reading the response, even in unauthorized case.
        if ( !m_response->ReadData() )
            SetFailedWithLastError("Reading data");

        return;
    }

    CheckResult(DoWriteData());
}

wxWebRequest::Result wxWebRequestWinHTTP::DoWriteData(DWORD* numWritten)
{
    wxASSERT( m_dataWritten < m_dataSize );

    int dataWriteSize = wxWEBREQUEST_BUFFER_SIZE;
    if ( m_dataWritten + dataWriteSize > m_dataSize )
        dataWriteSize = m_dataSize - m_dataWritten;

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: writing data [%llx; %x]",
               this, m_dataWritten, dataWriteSize);

    m_dataWriteBuffer.Clear();
    void* buffer = m_dataWriteBuffer.GetWriteBuf(dataWriteSize);
    m_dataStream->Read(buffer, dataWriteSize);

    if ( !wxWinHTTP::WinHttpWriteData
            (
                m_request,
                m_dataWriteBuffer.GetData(),
                dataWriteSize,
                numWritten // [out] bytes written, must be null in async mode
            ) )
    {
        return FailWithLastError("Writing data");
    }

    return Result::Ok();
}

wxWebRequest::Result wxWebRequestWinHTTP::CreateResponse()
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: creating response", this);

    if ( !wxWinHTTP::WinHttpReceiveResponse(m_request, nullptr) )
    {
        return FailWithLastError("Receiving response");
    }

    m_response.reset(new wxWebResponseWinHTTP(*this));

    return m_response->InitFileStorage();
}

wxWebRequest::Result wxWebRequestWinHTTP::InitAuthIfNeeded()
{
    int status = m_response->GetStatus();
    if ( status == HTTP_STATUS_DENIED || status == HTTP_STATUS_PROXY_AUTH_REQ )
    {
        m_authChallenge.reset(new wxWebAuthChallengeWinHTTP
            (
                status == HTTP_STATUS_PROXY_AUTH_REQ
                    ? wxWebAuthChallenge::Source_Proxy
                    : wxWebAuthChallenge::Source_Server,
                *this
            ));

        if ( !m_authChallenge->Init() )
            return FailWithLastError("Initializing authentication challenge");

        wxLogTrace(wxTRACE_WEBREQUEST,
                   "Request %p: authentication required (%s)",
                   this, m_response->GetStatusText());

        return Result::Unauthorized(m_response->GetStatusText());
    }

    return Result::Ok();
}

wxWebRequest::Result
wxWebRequestWinHTTP::Fail(const wxString& operation, DWORD errorCode)
{
    wxString failMessage = wxString::Format(
        "%s failed with error %08x (%s)",
        operation,
        errorCode,
        wxMSWFormatMessage(errorCode, GetModuleHandle(TEXT("WINHTTP")))
    );

    return Result::Error(failMessage);
}

wxWebRequest::Result wxWebRequestWinHTTP::Execute()
{
    Result result;

    result = DoPrepareRequest();
    if ( !result )
        return result;

    // This loop executes until we exhaust all authentication possibilities: we
    // may need to authenticate with the proxy first and then with the server
    // and we even may need to authenticate with the proxy again after failing
    // connecting to the server the first time.
    for ( ;; )
    {
        result = SendRequest();
        if ( !result )
            return result;

        // Write request data, if any.
        while ( m_dataWritten < m_dataSize )
        {
            DWORD written = 0;

            result = DoWriteData(&written);
            if ( !result )
                return result;

            if ( !written )
                break;

            m_dataWritten += written;
        }

        // Check the response.
        result = CreateResponse();
        if ( !result )
            return result;

        result = InitAuthIfNeeded();
        if ( !result )
            return result;

        if ( result.state != wxWebRequest::State_Unauthorized )
            break;

        switch ( m_authChallenge->GetSource() )
        {
            case wxWebAuthChallenge::Source_Proxy:
                if ( !m_tryProxyCredentials )
                    return result;

                // Don't try the same credentials again unless we manage to
                // connect to the server in the meanwhile (see below).
                m_tryProxyCredentials = false;

                result = m_authChallenge->DoSetCredentials(
                            m_sessionImpl.GetProxyCredentials()
                        );
                if ( !result )
                    return result;

                wxLogTrace(wxTRACE_WEBREQUEST,
                           "Request %p: retrying with proxy credentials",
                           this);
                break;

            case wxWebAuthChallenge::Source_Server:
                // We need to authenticate, but we can only do it if we had the
                // credentials in the URL and haven't tried using them yet.
                if ( !m_tryCredentialsFromURL )
                    return result;

                // Ensure we don't try them again, even if we fail.
                m_tryCredentialsFromURL = false;

                result = m_authChallenge->DoSetCredentials(m_credentialsFromURL);
                if ( !result )
                    return result;

                wxLogTrace(wxTRACE_WEBREQUEST,
                           "Request %p: retrying with credentials from URL",
                           this);

                // We have set the credentials successfully, so we can try
                // again, but we may need to re-authenticate with the proxy
                // now, so allow trying the proxy credentials again if we had
                // any in the first place.
                if ( m_sessionImpl.HasProxyCredentials() )
                    m_tryProxyCredentials = true;

                // Ensure that we write all the data again if we have any.
                if ( m_dataStream )
                {
                    m_dataStream->SeekI(0);
                    m_dataWritten = 0;
                }

                break;
        }

        continue;
    }

    // Read the response data.
    for ( ;; )
    {
        DWORD bytesRead = 0;
        if ( !m_response->ReadData(&bytesRead) )
            return FailWithLastError("Reading data");

        if ( !bytesRead )
            break;

        m_response->ReportDataReceived(bytesRead);
    }

    // We're done.
    return GetResultFromHTTPStatus(m_response);
}

wxWebRequest::Result wxWebRequestWinHTTP::DoPrepareRequest()
{
    const wxString method = GetHTTPMethod();

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: start \"%s %s\"",
               this, method, m_url);

    // Parse the URL
    wxURLComponents urlComps;
    if ( !wxWinHTTP::WinHttpCrackUrl(m_url.wc_str(), m_url.length(), 0, &urlComps) )
    {
        return FailWithLastError("Parsing URL");
    }

    // If we have auth in the URL, remember them but we can't use them yet
    // because we don't yet know which authentication scheme the server uses.
    if ( urlComps.HasCredentials() )
    {
        m_credentialsFromURL = urlComps.GetCredentials();
        m_tryCredentialsFromURL = true;
    }

    // Open a connection
    m_connect = wxWinHTTP::WinHttpConnect
                (
                     m_sessionImpl.GetHandle(),
                     wxString(urlComps.lpszHostName, urlComps.dwHostNameLength).wc_str(),
                     urlComps.nPort,
                     wxRESERVED_PARAM
                );
    if ( m_connect == nullptr )
    {
        return FailWithLastError("Connecting");
    }

    wxString objectName(urlComps.lpszUrlPath, urlComps.dwUrlPathLength);
    if ( urlComps.dwExtraInfoLength )
        objectName += wxString(urlComps.lpszExtraInfo, urlComps.dwExtraInfoLength);

    // Open a request
    static const wchar_t* acceptedTypes[] = { L"*/*", nullptr };
    m_request = wxWinHTTP::WinHttpOpenRequest
                  (
                    m_connect,
                    method.wc_str(), objectName.wc_str(),
                    nullptr,   // protocol version: use default, i.e. HTTP/1.1
                    WINHTTP_NO_REFERER,
                    acceptedTypes,
                    urlComps.nScheme == INTERNET_SCHEME_HTTPS
                        ? WINHTTP_FLAG_SECURE
                        : 0
                  );
    if ( m_request == nullptr )
    {
        return FailWithLastError("Opening request");
    }

    if ( int flags = GetSecurityFlags() )
    {
        DWORD optValue = 0;

        if ( flags & wxWebRequest::Ignore_Certificate )
            optValue |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                        SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                        SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
        if ( flags & wxWebRequest::Ignore_Host )
            optValue |= SECURITY_FLAG_IGNORE_CERT_CN_INVALID;

        wxWinHTTPSetOption(m_request, WINHTTP_OPTION_SECURITY_FLAGS, optValue);
    }

    return Result::Ok();
}

void wxWebRequestWinHTTP::Start()
{
    if ( !CheckResult(DoPrepareRequest()) )
        return;

    // Register callback
    if ( wxWinHTTP::WinHttpSetStatusCallback
           (
                m_request,
                wxRequestStatusCallback,
                WINHTTP_CALLBACK_FLAG_READ_COMPLETE |
                WINHTTP_CALLBACK_FLAG_WRITE_COMPLETE |
                WINHTTP_CALLBACK_FLAG_SENDREQUEST_COMPLETE |
                WINHTTP_CALLBACK_FLAG_REQUEST_ERROR,
                wxRESERVED_PARAM
            ) == WINHTTP_INVALID_STATUS_CALLBACK )
    {
        SetFailedWithLastError("Setting up callbacks");
        return;
    }

    SetState(wxWebRequest::State_Active);

    CheckResult(SendRequest());
}

wxWebRequest::Result wxWebRequestWinHTTP::SendRequest()
{
    // Combine all headers to a string
    wxString allHeaders;
    for ( wxWebRequestHeaderMap::const_iterator header = m_headers.begin();
          header != m_headers.end();
          ++header )
    {
        allHeaders.append(wxString::Format("%s: %s\n", header->first, header->second));
    }

    if ( m_dataSize )
        m_dataWritten = 0;

    // Send request
    if ( !wxWinHTTP::WinHttpSendRequest
            (
                m_request,
                allHeaders.wc_str(), allHeaders.length(),
                nullptr, 0,        // No extra optional data right now
                m_dataSize,
                (DWORD_PTR)this
            ) )
    {
        return FailWithLastError("Sending request");
    }

    return Result::Ok();
}

void wxWebRequestWinHTTP::DoCancel()
{
    wxWinHTTPCloseHandle(m_request);
    m_request = nullptr;
}

//
// wxWebResponseWinHTTP
//

wxWebResponseWinHTTP::wxWebResponseWinHTTP(wxWebRequestWinHTTP& request):
    wxWebResponseImpl(request),
    m_requestHandle(request.GetHandle())
{
    const wxString contentLengthStr =
        wxWinHTTPQueryHeaderString(m_requestHandle, WINHTTP_QUERY_CONTENT_LENGTH);
    if ( contentLengthStr.empty() ||
            !contentLengthStr.ToLongLong(&m_contentLength) )
    {
        m_contentLength = -1;

        wxLogTrace(wxTRACE_WEBREQUEST,
                   "Request %p: receiving response without content length",
                   &request);
    }
    else
    {
        wxLogTrace(wxTRACE_WEBREQUEST,
                   "Request %p: receiving %llu bytes",
                   &request, m_contentLength);
    }
}

wxString wxWebResponseWinHTTP::GetURL() const
{
    return wxWinHTTPQueryOptionString(m_requestHandle, WINHTTP_OPTION_URL);
}

wxString wxWebResponseWinHTTP::GetHeader(const wxString& name) const
{
    return wxWinHTTPQueryHeaderString(m_requestHandle, WINHTTP_QUERY_CUSTOM,
                                      name.wc_str());
}

int wxWebResponseWinHTTP::GetStatus() const
{
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if ( !wxWinHTTP::WinHttpQueryHeaders
            (
                m_requestHandle,
                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX,
                &status,
                &statusSize,
                0   // header index, unused with status code "header"
            ) )
    {
        wxLogLastError("WinHttpQueryHeaders/status");
    }

    return status;
}

wxString wxWebResponseWinHTTP::GetStatusText() const
{
    return wxWinHTTPQueryHeaderString(m_requestHandle, WINHTTP_QUERY_STATUS_TEXT);
}

bool wxWebResponseWinHTTP::ReadData(DWORD* bytesRead)
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: reading data", &m_request);

    return wxWinHTTP::WinHttpReadData
             (
                m_requestHandle,
                GetDataBuffer(wxWEBREQUEST_BUFFER_SIZE),
                wxWEBREQUEST_BUFFER_SIZE,
                bytesRead    // [out] bytes read, must be null in async mode
             ) == TRUE;
}

//
// wxWebAuthChallengeWinHTTP
//
wxWebAuthChallengeWinHTTP::wxWebAuthChallengeWinHTTP(wxWebAuthChallenge::Source source,
                                                     wxWebRequestWinHTTP & request):
    wxWebAuthChallengeImpl(source),
    m_request(request),
    m_target(0),
    m_selectedScheme(0)
{

}

bool wxWebAuthChallengeWinHTTP::Init()
{
    DWORD supportedSchemes;
    DWORD firstScheme;

    if ( !wxWinHTTP::WinHttpQueryAuthSchemes
            (
                m_request.GetHandle(),
                &supportedSchemes,
                &firstScheme,
                &m_target
            ) )
    {
        wxLogLastError("WinHttpQueryAuthSchemes");
        return false;
    }

    if ( supportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE )
        m_selectedScheme = WINHTTP_AUTH_SCHEME_NEGOTIATE;
    else if ( supportedSchemes & WINHTTP_AUTH_SCHEME_NTLM )
        m_selectedScheme = WINHTTP_AUTH_SCHEME_NTLM;
    else if ( supportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT )
        m_selectedScheme = WINHTTP_AUTH_SCHEME_PASSPORT;
    else if ( supportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST )
        m_selectedScheme = WINHTTP_AUTH_SCHEME_DIGEST;
    else if ( supportedSchemes & WINHTTP_AUTH_SCHEME_BASIC )
        m_selectedScheme = WINHTTP_AUTH_SCHEME_BASIC;
    else
        m_selectedScheme = 0;

    return m_selectedScheme != 0;
}

wxWebRequest::Result
wxWebAuthChallengeWinHTTP::DoSetCredentials(const wxWebCredentials& cred)
{
    if ( !wxWinHTTP::WinHttpSetCredentials
            (
                m_request.GetHandle(),
                m_target,
                m_selectedScheme,
                cred.GetUser().wc_str(),
                wxSecretString(cred.GetPassword()).wc_str(),
                wxRESERVED_PARAM
            ) )
    {
        return m_request.FailWithLastError("Setting credentials");
    }

    return wxWebRequest::Result::Ok();
}

void
wxWebAuthChallengeWinHTTP::SetCredentials(const wxWebCredentials& cred)
{
    if ( !m_request.CheckResult(DoSetCredentials(cred)) )
        return;

    // We can be called when the request is still active if we didn't switch to
    // unauthorized state because we're using the credentials from the URL.
    if ( m_request.GetState() != wxWebRequest::State_Active )
        m_request.SetState(wxWebRequest::State_Active);

    m_request.CheckResult(m_request.SendRequest());
}


//
// wxWebSessionWinHTTP
//

wxWebSessionWinHTTP::wxWebSessionWinHTTP(Mode mode)
    : wxWebSessionImpl(mode)
{
}

wxWebSessionWinHTTP::~wxWebSessionWinHTTP()
{
    if ( m_handle )
        wxWinHTTPCloseHandle(m_handle);
}

bool wxWebSessionWinHTTP::Initialize()
{
    return wxWinHTTP::LoadLibrary();
}

bool wxWebSessionWinHTTP::Open()
{
    DWORD accessType = 0;

    const wchar_t* proxyName = WINHTTP_NO_PROXY_NAME;

    const wxWebProxy& proxy = GetProxy();
    switch ( proxy.GetType() )
    {
        case wxWebProxy::Type::URL:
            accessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
            proxyName = m_proxyURLWithoutCredentials.wc_str();
            break;

        case wxWebProxy::Type::Disabled:
            accessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
            break;

        case wxWebProxy::Type::Default:
            if ( wxCheckOsVersion(6, 3) )
                accessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
            else
                accessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
            break;
    }

    DWORD flags = 0;
    if ( IsAsync() )
        flags |= WINHTTP_FLAG_ASYNC;

    m_handle = wxWinHTTP::WinHttpOpen
                 (
                    GetHeaders().find("User-Agent")->second.wc_str(),
                    accessType,
                    proxyName,
                    WINHTTP_NO_PROXY_BYPASS,
                    flags
                 );
    if ( !m_handle )
    {
        wxLogLastError("WinHttpOpen");
        return false;
    }

    // Try to enable HTTP/2 (available since Win 10 1607)
    wxWinHTTPSetOption(m_handle, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL,
                       WINHTTP_PROTOCOL_FLAG_HTTP2);

    // Try to enable GZIP and DEFLATE (available since Win 8.1)
    wxWinHTTPSetOption(m_handle, WINHTTP_OPTION_DECOMPRESSION,
                       WINHTTP_DECOMPRESSION_FLAG_ALL);

    // Try to enable modern TLS for older Windows versions
    if ( !wxCheckOsVersion(6, 3) )
    {
        wxWinHTTPSetOption(m_handle, WINHTTP_OPTION_SECURE_PROTOCOLS,
                           WINHTTP_FLAG_SECURE_PROTOCOL_SSL3 |
                           WINHTTP_FLAG_SECURE_PROTOCOL_TLS1 |
                           WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 |
                           WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2);
    }

    return true;
}

wxWebRequestImplPtr
wxWebSessionWinHTTP::CreateRequest(wxWebSession& session,
                                   wxEvtHandler* handler,
                                   const wxString& url,
                                   int id)
{
    wxCHECK_MSG( IsAsync(), wxWebRequestImplPtr(),
                 "This method should not be called for synchronous sessions" );

    if ( !m_handle )
    {
        if ( !Open() )
            return wxWebRequestImplPtr();
    }

    return wxWebRequestImplPtr(
        new wxWebRequestWinHTTP(session, *this, handler, url, id));
}

wxWebRequestImplPtr
wxWebSessionWinHTTP::CreateRequestSync(wxWebSessionSync& WXUNUSED(session),
                                       const wxString& url)
{
    wxCHECK_MSG( !IsAsync(), wxWebRequestImplPtr(),
                 "This method should not be called for asynchronous sessions" );

    if ( !m_handle )
    {
        if ( !Open() )
            return wxWebRequestImplPtr();
    }

    return wxWebRequestImplPtr{new wxWebRequestWinHTTP{*this, url}};
}

wxVersionInfo wxWebSessionWinHTTP::GetLibraryVersionInfo() const
{
    int verMaj, verMin, verMicro;
    wxGetOsVersion(&verMaj, &verMin, &verMicro);
    return wxVersionInfo("WinHTTP", verMaj, verMin, verMicro);
}

bool wxWebSessionWinHTTP::SetProxy(const wxWebProxy& proxy)
{
    wxCHECK_MSG( !m_handle, false,
                 "Proxy must be set before the first request is made" );

    // Extract proxy credentials if they are present in the URL as WinHTTP
    // doesn't handle them and we have to do everything ourselves.
    if ( proxy.GetType() == wxWebProxy::Type::URL )
    {
        const wxString& url = proxy.GetURL();
        wxURLComponents urlComps;
        if ( !wxWinHTTP::WinHttpCrackUrl(url.wc_str(), url.length(), 0, &urlComps) )
        {
            wxLogTrace(wxTRACE_WEBREQUEST, "Invalid proxy URL: \"%s\"", url);
            return false;
        }

        if ( urlComps.HasCredentials() )
        {
            // We're going to need these credentials later, if we have them.
            m_proxyCredentials = urlComps.GetCredentials();

            // Moreover, WinHttpOpen() doesn't accept credentials in the proxy
            // string, so we need to create an URL without them.
            urlComps.dwUserNameLength =
            urlComps.dwPasswordLength = 0;
            urlComps.lpszUserName =
            urlComps.lpszPassword = nullptr;

            wxWCharBuffer buf(url.length());
            DWORD len = buf.length();
            if ( !wxWinHTTP::WinHttpCreateUrl(&urlComps, 0, buf.data(), &len) )
            {
                wxLogTrace(wxTRACE_WEBREQUEST,
                           "Failed to recreate proxy URL for \"%s\"", url);
                return false;
            }

            // We need to shrink the buffer to its effective length to avoid
            // having NUL bytes at the end of the string.
            buf.shrink(len);

            m_proxyURLWithoutCredentials = buf;
        }
        else // No credentials in the proxy URL, just store it as is.
        {
            m_proxyURLWithoutCredentials = url;
        }

        // Final step: WinHttpOpen() doesn't accept trailing slashes in the URL
        // neither (it just fails with ERROR_INVALID_PARAMETER), so remove them.
        while ( m_proxyURLWithoutCredentials.Last() == '/' )
            m_proxyURLWithoutCredentials.RemoveLast();

        wxLogTrace(wxTRACE_WEBREQUEST, "Proxy URL: %s -> %s",
                   url, m_proxyURLWithoutCredentials);
    }

    return wxWebSessionImpl::SetProxy(proxy);
}

#endif // wxUSE_WEBREQUEST_WINHTTP
