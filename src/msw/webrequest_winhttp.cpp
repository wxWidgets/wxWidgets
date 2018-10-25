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

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST_WINHTTP

#include "wx/mstream.h"
#include "wx/uri.h"
#include "wx/msw/webrequest_winhttp.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/translation.h"
#endif

// For MSVC we can link in the required library explicitly, for the other
// compilers (e.g. MinGW) this needs to be done at makefiles level.
#ifdef __VISUALC__
#pragma comment(lib, "Winhttp")
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

static wxString wxWinHTTPErrorToString(DWORD errorCode)
{
    wxString errorString;

    LPVOID msgBuf;
    if ( FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandle(TEXT("WINHTTP")),
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuf,
        0, NULL) )
    {
        errorString.assign((LPWSTR)msgBuf);
        LocalFree(msgBuf);
        // Truncate trailing \n\r
        if ( errorString.size() > 2 )
            errorString.resize(errorString.size());
    }
    return errorString;
}

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
            resBuf.data(), &bufferLen, WINHTTP_NO_HEADER_INDEX) )
            result.assign(resBuf);
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

wxWebRequestWinHTTP::wxWebRequestWinHTTP(int id, wxWebSessionWinHTTP& session, const wxString& url):
    wxWebRequest(id),
    m_session(session),
    m_url(url),
    m_connect(NULL),
    m_request(NULL)
{
    m_headers = session.GetHeaders();
}

wxWebRequestWinHTTP::~wxWebRequestWinHTTP()
{
    if ( m_request )
        ::WinHttpCloseHandle(m_request);
    if ( m_connect )
        ::WinHttpCloseHandle(m_connect);
}

void wxWebRequestWinHTTP::HandleCallback(DWORD dwInternetStatus,
    LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
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
                if ( !m_response->ReportAvailableData(dwStatusInformationLength) )
                    SetFailedWithLastError();
            }
            else
            {
                m_response->ReportDataComplete();
                SetState(State_Completed);
            }
            break;
        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
            WriteData();
            break;
        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        {
            LPWINHTTP_ASYNC_RESULT asyncResult = reinterpret_cast<LPWINHTTP_ASYNC_RESULT>(lpvStatusInformation);
            SetState(State_Failed, wxWinHTTPErrorToString(asyncResult->dwError));
            break;
        }
    }
}

void wxWebRequestWinHTTP::WriteData()
{
    int dataWriteSize = 8 * 1024;
    if ( m_dataWritten + dataWriteSize > m_dataSize )
        dataWriteSize = m_dataSize - m_dataWritten;
    if ( dataWriteSize )
    {
        m_dataWriteBuffer.Clear();
        m_dataWriteBuffer.GetWriteBuf(dataWriteSize);
        m_dataStream->Read(m_dataWriteBuffer.GetData(), dataWriteSize);

        if ( !::WinHttpWriteData(m_request, m_dataWriteBuffer.GetData(), dataWriteSize, NULL) )
            SetFailedWithLastError();
        m_dataWritten += dataWriteSize;
    }
    else
        CreateResponse();
}

void wxWebRequestWinHTTP::CreateResponse()
{
    if (::WinHttpReceiveResponse(m_request, NULL))
    {
        m_response.reset(new wxWebResponseWinHTTP(*this));
        if (CheckServerStatus())
        {
            // Start reading the response
            if (!m_response->ReadData())
                SetFailedWithLastError();
        }
    }
    else
        SetFailedWithLastError();
}

void wxWebRequestWinHTTP::SetFailedWithLastError()
{
    wxString failMessage = wxWinHTTPErrorToString(::GetLastError());
    SetState(State_Failed, failMessage);
}

void wxWebRequestWinHTTP::Start()
{
    if ( GetState() != State_Idle ) // Completed requests can not be restarted
        return;

    // Parse the URL
    wxURI uri(m_url);
    bool isSecure = uri.GetScheme().IsSameAs("HTTPS", false);

    int port;
    if ( !uri.HasPort() )
        port = (isSecure) ? 443 : 80;
    else
        port = wxAtoi(uri.GetPort());

    // Open a connction
    m_connect = ::WinHttpConnect(m_session.GetHandle(), uri.GetServer().wc_str(),
        port, 0);
    if ( m_connect == NULL )
    {
        SetFailedWithLastError();
        return;
    }

    wxString method;
    if ( !m_method.empty() )
        method = m_method;
    else if ( m_dataSize )
        method = "POST";
    else
        method = "GET";

    wxString objectName = uri.GetPath();
    if ( uri.HasQuery() )
        objectName += "?" + uri.GetQuery();

    // Open a request
    m_request = ::WinHttpOpenRequest(m_connect,
        method.wc_str(), objectName.wc_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (isSecure) ? WINHTTP_FLAG_SECURE : 0);
    if ( m_request == NULL )
    {
        SetFailedWithLastError();
        return;
    }

    // Register callback
    if ( ::WinHttpSetStatusCallback(m_request,
        (WINHTTP_STATUS_CALLBACK)wxRequestStatusCallback,
        WINHTTP_CALLBACK_FLAG_READ_COMPLETE |
        WINHTTP_CALLBACK_FLAG_WRITE_COMPLETE |
        WINHTTP_CALLBACK_FLAG_SENDREQUEST_COMPLETE |
        WINHTTP_CALLBACK_FLAG_REQUEST_ERROR,
        0) == WINHTTP_INVALID_STATUS_CALLBACK )
    {
        SetFailedWithLastError();
        return;
    }

    // Combine all headers to a string
    wxString allHeaders;
    for (wxWebRequestHeaderMap::const_iterator header = m_headers.begin(); header != m_headers.end(); ++header)
        allHeaders.append(wxString::Format("%s: %s\n", header->first, header->second));

    if ( m_dataSize )
        m_dataWritten = 0;

    // Send request
    if ( WinHttpSendRequest(m_request,
        allHeaders.wc_str(), allHeaders.length(),
        NULL, 0, m_dataSize,
        (DWORD_PTR)this) )
    {
        SetState(State_Active);
    }
    else
        SetFailedWithLastError();
}

void wxWebRequestWinHTTP::Cancel()
{
    wxFAIL_MSG("not implemented");
}

wxWebResponse* wxWebRequestWinHTTP::GetResponse()
{
    return m_response.get();
}

//
// wxWebResponseWinHTTP
//

wxWebResponseWinHTTP::wxWebResponseWinHTTP(wxWebRequestWinHTTP& request):
    m_request(request),
    m_readSize(8 * 1024)
{
    wxString contentLengthStr = wxWinHTTPQueryHeaderString(m_request.GetHandle(),
        WINHTTP_QUERY_CONTENT_LENGTH);
    if ( contentLengthStr.empty() ||
        !contentLengthStr.ToLongLong(&m_contentLength) )
        m_contentLength = -1;
}

wxString wxWebResponseWinHTTP::GetURL() const
{
    return wxWinHTTPQueryOptionString(m_request.GetHandle(), WINHTTP_OPTION_URL);
}

wxString wxWebResponseWinHTTP::GetHeader(const wxString& name) const
{
    return wxWinHTTPQueryHeaderString(m_request.GetHandle(),
        WINHTTP_QUERY_CUSTOM, name.wc_str());
}

int wxWebResponseWinHTTP::GetStatus() const
{
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if ( !::WinHttpQueryHeaders(m_request.GetHandle(),
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, 0) )
    {
        status = 0;
        wxLogLastError("WinHttpQueryHeaders/status");
    }

    return status;
}

wxString wxWebResponseWinHTTP::GetStatusText() const
{
    return wxWinHTTPQueryHeaderString(m_request.GetHandle(), WINHTTP_QUERY_STATUS_TEXT);
}

wxInputStream* wxWebResponseWinHTTP::GetStream() const
{
    return m_stream.get();
}

wxString wxWebResponseWinHTTP::AsString(wxMBConv* conv) const
{
    // TODO: try to determine encoding type from content-type header
    if ( !conv )
        conv = &wxConvUTF8;

    size_t outLen = 0;
    return conv->cMB2WC((const char*) m_readBuffer.GetData(), m_readBuffer.GetDataLen(), &outLen);
}

bool wxWebResponseWinHTTP::ReadData()
{
    if ( ::WinHttpReadData(m_request.GetHandle(),
        m_readBuffer.GetAppendBuf(m_readSize), m_readSize, NULL) )
        return true;
    else
        return false;
}

bool wxWebResponseWinHTTP::ReportAvailableData(DWORD dataLen)
{
    m_readBuffer.UngetAppendBuf(dataLen);
    return ReadData();
}

void wxWebResponseWinHTTP::ReportDataComplete()
{
    m_stream.reset(new wxMemoryInputStream(m_readBuffer.GetData(), m_readBuffer.GetDataLen()));
}

//
// wxWebSessionWinHTTP
//

wxWebSessionWinHTTP::wxWebSessionWinHTTP():
    m_initialized(false),
    m_handle(NULL)
{
}

wxWebSessionWinHTTP::~wxWebSessionWinHTTP()
{
    if ( m_handle != INVALID_HANDLE_VALUE )
        ::WinHttpCloseHandle(m_handle);
}

void wxWebSessionWinHTTP::Init()
{
    DWORD accessType;
    if ( wxCheckOsVersion(6, 3) )
        accessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
    else
        accessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;

    m_handle = ::WinHttpOpen(GetHeaders().find("User-Agent")->second.wc_str(), accessType,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC);
    if ( m_handle != NULL )
    {
        // Try to enable HTTP/2 (available since Win 10 1607)
        DWORD protFlags = WINHTTP_PROTOCOL_FLAG_HTTP2;
        ::WinHttpSetOption(m_handle, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL,
            &protFlags, sizeof(protFlags));

        // Try to enable GZIP and DEFLATE (available since Win 8.1)
        DWORD decompressFlags = WINHTTP_DECOMPRESSION_FLAG_ALL;
        ::WinHttpSetOption(m_handle, WINHTTP_OPTION_DECOMPRESSION,
            &decompressFlags, sizeof(decompressFlags));

        // Try to enable modern TLS for older Windows versions
        if ( !wxCheckOsVersion(6, 3) )
        {
            DWORD securityFlags = WINHTTP_FLAG_SECURE_PROTOCOL_SSL3 |
                WINHTTP_FLAG_SECURE_PROTOCOL_TLS1 |
                WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 |
                WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
            ::WinHttpSetOption(m_handle, WINHTTP_OPTION_SECURE_PROTOCOLS,
                &securityFlags, sizeof(securityFlags));
        }
    }
    else
        wxLogLastError("WinHttpOpen");

    m_initialized = true;
}

wxWebRequest* wxWebSessionWinHTTP::CreateRequest(const wxString& url, int id)
{
    if ( !m_initialized )
        Init();

    return new wxWebRequestWinHTTP(id, *this, url);
}

#endif // wxUSE_WEBREQUEST_WINHTTP
