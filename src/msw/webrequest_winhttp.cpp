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

#if wxUSE_WEBREQUEST_WINHTTP

#include "wx/mstream.h"
#include "wx/msw/private.h"
#include "wx/msw/private/webrequest_winhttp.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/translation.h"
#endif

// For MSVC we can link in the required library explicitly, for the other
// compilers (e.g. MinGW) this needs to be done at makefiles level.
#ifdef __VISUALC__
#pragma comment(lib, "winhttp")
#endif

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

// Helper functions

static wxString wxWinHTTPQueryHeaderString(HINTERNET hRequest, DWORD dwInfoLevel,
    LPCWSTR pwszName = WINHTTP_HEADER_NAME_BY_INDEX)
{
    wxString result;
    DWORD bufferLen = 0;
    ::WinHttpQueryHeaders(hRequest, dwInfoLevel, pwszName, NULL, &bufferLen,
        WINHTTP_NO_HEADER_INDEX);
    if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
    {
        wxWCharBuffer resBuf(bufferLen);
        if ( ::WinHttpQueryHeaders(hRequest, dwInfoLevel, pwszName,
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
    ::WinHttpQueryOption(hInternet, dwOption, NULL, &bufferLen);
    if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
    {
        wxWCharBuffer resBuf(bufferLen);
        if ( ::WinHttpQueryOption(hInternet, dwOption, resBuf.data(), &bufferLen) )
            result.assign(resBuf);
    }

    return result;
}

static inline
void wxWinHTTPSetOption(HINTERNET hInternet, DWORD dwOption, DWORD dwValue)
{
    ::WinHttpSetOption(hInternet, dwOption, &dwValue, sizeof(dwValue));
}

static
void wxWinHTTPCloseHandle(HINTERNET hInternet)
{
    if ( !::WinHttpCloseHandle(hInternet) )
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
    m_connect(NULL),
    m_request(NULL),
    m_dataWritten(0)
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
            if ( m_dataSize )
                WriteData();
            else
                CreateResponse();
            break;

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            if ( dwStatusInformationLength > 0 )
            {
                if ( !m_response->ReportAvailableData(dwStatusInformationLength)
                        && !WasCancelled() )
                    SetFailedWithLastError();
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
                SetFailed(asyncResult->dwError);
            break;
        }
    }
}

void wxWebRequestWinHTTP::WriteData()
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: writing data", this);

    int dataWriteSize = wxWEBREQUEST_BUFFER_SIZE;
    if ( m_dataWritten + dataWriteSize > m_dataSize )
        dataWriteSize = m_dataSize - m_dataWritten;
    if ( !dataWriteSize )
    {
        CreateResponse();
        return;
    }

    m_dataWriteBuffer.Clear();
    void* buffer = m_dataWriteBuffer.GetWriteBuf(dataWriteSize);
    m_dataStream->Read(buffer, dataWriteSize);

    if ( !::WinHttpWriteData
            (
                m_request,
                m_dataWriteBuffer.GetData(),
                dataWriteSize,
                NULL    // [out] bytes written, must be null in async mode
            ) )
    {
        SetFailedWithLastError();
    }
}

void wxWebRequestWinHTTP::CreateResponse()
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: creating response", this);

    if ( !::WinHttpReceiveResponse(m_request, NULL) )
    {
        SetFailedWithLastError();
        return;
    }

    m_response.reset(new wxWebResponseWinHTTP(*this));
    // wxWebResponseWinHTTP ctor could have changed the state if its
    // initialization failed, so check for this.
    if ( GetState() == wxWebRequest::State_Failed )
        return;

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

        if ( m_authChallenge->Init() )
            SetState(wxWebRequest::State_Unauthorized, m_response->GetStatusText());
        else
            SetFailedWithLastError();
    }
    else
    {
        // Start reading the response
        if ( !m_response->ReadData() )
            SetFailedWithLastError();
    }
}

void wxWebRequestWinHTTP::SetFailed(DWORD errorCode)
{
    wxString failMessage = wxMSWFormatMessage(errorCode,
                                              GetModuleHandle(TEXT("WINHTTP")));
    SetState(wxWebRequest::State_Failed, failMessage);
}

void wxWebRequestWinHTTP::Start()
{
    wxString method;
    if ( !m_method.empty() )
        method = m_method;
    else if ( m_dataSize )
        method = "POST";
    else
        method = "GET";

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: start \"%s %s\"",
               this, method, m_url);

    // Parse the URL
    URL_COMPONENTS urlComps;
    wxZeroMemory(urlComps);
    urlComps.dwStructSize = sizeof(urlComps);
    urlComps.dwSchemeLength =
    urlComps.dwHostNameLength =
    urlComps.dwUrlPathLength =
    urlComps.dwExtraInfoLength = (DWORD)-1;

    if ( !::WinHttpCrackUrl(m_url.wc_str(), m_url.length(), 0, &urlComps) )
    {
        SetFailedWithLastError();
        return;
    }

    // Open a connection
    m_connect = ::WinHttpConnect
                (
                     m_sessionImpl.GetHandle(),
                     wxString(urlComps.lpszHostName, urlComps.dwHostNameLength).wc_str(),
                     urlComps.nPort,
                     wxRESERVED_PARAM
                );
    if ( m_connect == NULL )
    {
        SetFailedWithLastError();
        return;
    }

    wxString objectName(urlComps.lpszUrlPath, urlComps.dwUrlPathLength);
    if ( urlComps.dwExtraInfoLength )
        objectName += "?" + wxString(urlComps.lpszExtraInfo, urlComps.dwExtraInfoLength);

    // Open a request
    static const wchar_t* acceptedTypes[] = { L"*/*", NULL };
    m_request = ::WinHttpOpenRequest
                  (
                    m_connect,
                    method.wc_str(), objectName.wc_str(),
                    NULL,   // protocol version: use default, i.e. HTTP/1.1
                    WINHTTP_NO_REFERER,
                    acceptedTypes,
                    urlComps.nScheme == INTERNET_SCHEME_HTTPS
                        ? WINHTTP_FLAG_SECURE
                        : 0
                  );
    if ( m_request == NULL )
    {
        SetFailedWithLastError();
        return;
    }

    // Register callback
    if ( ::WinHttpSetStatusCallback
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
        SetFailedWithLastError();
        return;
    }

    if ( IsPeerVerifyDisabled() )
    {
        wxWinHTTPSetOption(m_request, WINHTTP_OPTION_SECURITY_FLAGS,
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
            SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
            SECURITY_FLAG_IGNORE_UNKNOWN_CA |
            SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE
        );
    }

    SendRequest();
}

void wxWebRequestWinHTTP::SendRequest()
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

    SetState(wxWebRequest::State_Active);

    // Send request
    if ( !::WinHttpSendRequest
            (
                m_request,
                allHeaders.wc_str(), allHeaders.length(),
                NULL, 0,        // No extra optional data right now
                m_dataSize,
                (DWORD_PTR)this
            ) )
    {
        SetFailedWithLastError();
        return;
    }
}

void wxWebRequestWinHTTP::DoCancel()
{
    wxWinHTTPCloseHandle(m_request);
    m_request = NULL;
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
        m_contentLength = -1;

    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: receiving %llu bytes",
               &request, m_contentLength);

    Init();
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
    if ( !::WinHttpQueryHeaders
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

bool wxWebResponseWinHTTP::ReadData()
{
    wxLogTrace(wxTRACE_WEBREQUEST, "Request %p: reading data", &m_request);

    return ::WinHttpReadData
             (
                m_requestHandle,
                GetDataBuffer(m_readSize),
                m_readSize,
                NULL    // [out] bytes read, must be null in async mode
             ) == TRUE;
}

bool wxWebResponseWinHTTP::ReportAvailableData(DWORD dataLen)
{
    ReportDataReceived(dataLen);
    return ReadData();
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

    if ( !::WinHttpQueryAuthSchemes
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

void
wxWebAuthChallengeWinHTTP::SetCredentials(const wxWebCredentials& cred)
{
    if ( !::WinHttpSetCredentials
            (
                m_request.GetHandle(),
                m_target,
                m_selectedScheme,
                cred.GetUser().wc_str(),
                wxSecretString(cred.GetPassword()).wc_str(),
                wxRESERVED_PARAM
            ) )
    {
        m_request.SetFailedWithLastError();
        return;
    }

    m_request.SendRequest();
}


//
// wxWebSessionWinHTTP
//

wxWebSessionWinHTTP::wxWebSessionWinHTTP():
    m_handle(NULL)
{
}

wxWebSessionWinHTTP::~wxWebSessionWinHTTP()
{
    if ( m_handle )
        wxWinHTTPCloseHandle(m_handle);
}

bool wxWebSessionWinHTTP::Open()
{
    DWORD accessType;
    if ( wxCheckOsVersion(6, 3) )
        accessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
    else
        accessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;

    m_handle = ::WinHttpOpen
                 (
                    GetHeaders().find("User-Agent")->second.wc_str(),
                    accessType,
                    WINHTTP_NO_PROXY_NAME,
                    WINHTTP_NO_PROXY_BYPASS,
                    WINHTTP_FLAG_ASYNC
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
    if ( !m_handle )
    {
        if ( !Open() )
            return wxWebRequestImplPtr();
    }

    return wxWebRequestImplPtr(
        new wxWebRequestWinHTTP(session, *this, handler, url, id));
}

wxVersionInfo wxWebSessionWinHTTP::GetLibraryVersionInfo()
{
    int verMaj, verMin, verMicro;
    wxGetOsVersion(&verMaj, &verMin, &verMicro);
    return wxVersionInfo("WinHTTP", verMaj, verMin, verMicro);
}


#endif // wxUSE_WEBREQUEST_WINHTTP
