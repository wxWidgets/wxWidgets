///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/webrequest_curl.h
// Purpose:     wxWebRequest implementation using libcurl
// Author:      Tobias Taschner
// Created:     2018-10-25
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/webrequest.h"

#if wxUSE_WEBREQUEST && wxUSE_WEBREQUEST_CURL

#include "wx/private/webrequest_curl.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/translation.h"
    #include "wx/utils.h"
#endif

#include "wx/uri.h"
#include "wx/private/socket.h"
#include "wx/evtloop.h"

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"

    #include <unordered_set>
#else
    #include "wx/evtloopsrc.h"
    #include "wx/evtloop.h"

    #include <unordered_map>
#endif


// Define symbols that might be missing from older libcurl headers
#ifndef CURL_AT_LEAST_VERSION
#define CURL_VERSION_BITS(x,y,z) ((x)<<16|(y)<<8|z)
#define CURL_AT_LEAST_VERSION(x,y,z) \
  (LIBCURL_VERSION_NUM >= CURL_VERSION_BITS(x, y, z))
#endif

// The new name was introduced in curl 7.21.6.
#ifndef CURLOPT_ACCEPT_ENCODING
    #define CURLOPT_ACCEPT_ENCODING CURLOPT_ENCODING
#endif

//
// wxWebResponseCURL
//

static size_t wxCURLWriteData(void* buffer, size_t size, size_t nmemb, void* userdata)
{
    wxCHECK_MSG( userdata, 0, "invalid curl write callback data" );

    return static_cast<wxWebResponseCURL*>(userdata)->CURLOnWrite(buffer, size * nmemb);
}

static size_t wxCURLHeader(char *buffer, size_t size, size_t nitems, void *userdata)
{
    wxCHECK_MSG( userdata, 0, "invalid curl header callback data" );

    return static_cast<wxWebResponseCURL*>(userdata)->CURLOnHeader(buffer, size * nitems);
}

static int wxCURLXferInfo(void* clientp, curl_off_t dltotal,
                          curl_off_t WXUNUSED(dlnow),
                          curl_off_t WXUNUSED(ultotal),
                          curl_off_t WXUNUSED(ulnow))
{
    wxCHECK_MSG( clientp, 0, "invalid curl progress callback data" );

    wxWebResponseCURL* response = reinterpret_cast<wxWebResponseCURL*>(clientp);
    return response->CURLOnProgress(dltotal);
}

static int wxCURLProgress(void* clientp, double dltotal, double dlnow,
                          double ultotal, double ulnow)
{
    return wxCURLXferInfo(clientp, static_cast<curl_off_t>(dltotal),
                          static_cast<curl_off_t>(dlnow),
                          static_cast<curl_off_t>(ultotal),
                          static_cast<curl_off_t>(ulnow));
}

// Replace dangerous variadic curl_easy_setopt() with safe overloads.
namespace
{

void wxCURLSetOpt(CURL* handle, CURLoption option, long long value)
{
    CURLcode res = curl_easy_setopt(handle, option, value);
    if ( res != CURLE_OK )
    {
        wxLogDebug("curl_easy_setopt(%d, %lld) failed: %s",
                   static_cast<int>(option), value, curl_easy_strerror(res));
    }
}

void wxCURLSetOpt(CURL* handle, CURLoption option, unsigned long value)
{
    CURLcode res = curl_easy_setopt(handle, option, value);
    if ( res != CURLE_OK )
    {
        wxLogDebug("curl_easy_setopt(%d, %lx) failed: %s",
                   static_cast<int>(option), value, curl_easy_strerror(res));
    }
}

void wxCURLSetOpt(CURL* handle, CURLoption option, long value)
{
    CURLcode res = curl_easy_setopt(handle, option, value);
    if ( res != CURLE_OK )
    {
        wxLogDebug("curl_easy_setopt(%d, %ld) failed: %s",
                   static_cast<int>(option), value, curl_easy_strerror(res));
    }
}

void wxCURLSetOpt(CURL* handle, CURLoption option, int value)
{
    wxCURLSetOpt(handle, option, static_cast<long>(value));
}

void wxCURLSetOpt(CURL* handle, CURLoption option, const void* value)
{
    CURLcode res = curl_easy_setopt(handle, option, value);
    if ( res != CURLE_OK )
    {
        wxLogDebug("curl_easy_setopt(%d, %p) failed: %s",
                   static_cast<int>(option), value, curl_easy_strerror(res));
    }
}

// Overload for function pointers used for various callbacks.
template <typename R, typename... Args>
void wxCURLSetOpt(CURL* handle, CURLoption option, R (*func)(Args...))
{
    wxCURLSetOpt(handle, option, reinterpret_cast<void*>(func));
}

void wxCURLSetOpt(CURL* handle, CURLoption option, const char* value)
{
    CURLcode res = curl_easy_setopt(handle, option, value);
    if ( res != CURLE_OK )
    {
        wxLogDebug("curl_easy_setopt(%d, \"%s\") failed: %s",
                   static_cast<int>(option), value, curl_easy_strerror(res));
    }
}

void wxCURLSetOpt(CURL* handle, CURLoption option, const wxString& value)
{
    wxCURLSetOpt(handle, option, value.utf8_str().data());
}

} // anonymous namespace

wxWebResponseCURL::wxWebResponseCURL(wxWebRequestCURL& request) :
    wxWebResponseImpl(request)
{
    m_knownDownloadSize = 0;

    wxCURLSetOpt(GetHandle(), CURLOPT_WRITEDATA, this);
    wxCURLSetOpt(GetHandle(), CURLOPT_HEADERDATA, this);

 // Set the progress callback.
    #if CURL_AT_LEAST_VERSION(7, 32, 0)
        if ( wxWebSessionCURL::CurlRuntimeAtLeastVersion(7, 32, 0) )
        {
            wxCURLSetOpt(GetHandle(), CURLOPT_XFERINFOFUNCTION, wxCURLXferInfo);
            wxCURLSetOpt(GetHandle(), CURLOPT_XFERINFODATA, this);
        }
        else
    #endif
        {
            // We know that these constants are deprecated, but we still need
            // to use them with this old version.
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)

            wxCURLSetOpt(GetHandle(), CURLOPT_PROGRESSFUNCTION, wxCURLProgress);
            wxCURLSetOpt(GetHandle(), CURLOPT_PROGRESSDATA, this);

            wxGCC_WARNING_RESTORE(deprecated-declarations)
        }

    // Have curl call the progress callback.
    wxCURLSetOpt(GetHandle(), CURLOPT_NOPROGRESS, 0L);
}

size_t wxWebResponseCURL::CURLOnWrite(void* buffer, size_t size)
{
    void* buf = GetDataBuffer(size);
    memcpy(buf, buffer, size);
    ReportDataReceived(size);
    return size;
}

size_t wxWebResponseCURL::CURLOnHeader(const char * buffer, size_t size)
{
    // HTTP headers are supposed to only contain ASCII data, so any encoding
    // should work here, but use Latin-1 for compatibility with some servers
    // that send it directly and to at least avoid losing data entirely when
    // the current encoding is UTF-8 but the input doesn't decode correctly.
    wxString hdr = wxString::From8BitData(buffer, size);
    hdr.Trim();

    if ( hdr.StartsWith("HTTP/") )
    {
        // First line of the headers contains status text after
        // version and status
        m_statusText = hdr.AfterFirst(' ').AfterFirst(' ');
        m_headers.clear();
    }
    else if ( !hdr.empty() )
    {
        wxString hdrValue;
        wxString hdrName = hdr.BeforeFirst(':', &hdrValue).Strip(wxString::trailing);
        hdrName.MakeUpper();
        m_headers[hdrName] = hdrValue.Strip(wxString::leading);
    }

    return size;
}

int wxWebResponseCURL::CURLOnProgress(curl_off_t total)
{
    if ( m_knownDownloadSize != total )
    {
        if ( m_request.GetStorage() == wxWebRequest::Storage_Memory )
        {
            PreAllocBuffer(static_cast<size_t>(total));
        }
        m_knownDownloadSize = total;
    }

    return 0;
}

wxFileOffset wxWebResponseCURL::GetContentLength() const
{
#if CURL_AT_LEAST_VERSION(7, 55, 0)
    curl_off_t len = 0;
    curl_easy_getinfo(GetHandle(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &len);
    return len;
#else
    double len = 0;
    curl_easy_getinfo(GetHandle(), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len);
    return (wxFileOffset)len;
#endif
}

wxString wxWebResponseCURL::GetURL() const
{
    char* urlp = nullptr;
    curl_easy_getinfo(GetHandle(), CURLINFO_EFFECTIVE_URL, &urlp);

    // While URLs should contain ASCII characters only as per
    // https://tools.ietf.org/html/rfc3986#section-2 we still want to avoid
    // losing data if they somehow contain something else but are not in UTF-8
    // by interpreting it as Latin-1.
    return wxString::From8BitData(urlp);
}

wxString wxWebResponseCURL::GetHeader(const wxString& name) const
{
    wxWebRequestHeaderMap::const_iterator it = m_headers.find(name.Upper());
    if ( it != m_headers.end() )
        return it->second;
    else
        return wxString();
}

int wxWebResponseCURL::GetStatus() const
{
    long status = 0;
    curl_easy_getinfo(GetHandle(), CURLINFO_RESPONSE_CODE, &status);
    return status;
}

//
// wxWebRequestCURL
//

static size_t wxCURLRead(char *buffer, size_t size, size_t nitems, void *userdata)
{
    wxCHECK_MSG( userdata, 0, "invalid curl read callback data" );

    return static_cast<wxWebRequestCURL*>(userdata)->CURLOnRead(buffer, size * nitems);
}

wxWebRequestCURL::wxWebRequestCURL(wxWebSession & session,
                                   wxWebSessionCURL& sessionImpl,
                                   wxEvtHandler* handler,
                                   const wxString & url,
                                   int id):
    wxWebRequestImpl(session, sessionImpl, handler, id),
    m_sessionCURL(&sessionImpl),
    m_handle(curl_easy_init())
{

    DoStartPrepare(url);
}

wxWebRequestCURL::wxWebRequestCURL(wxWebSessionSyncCURL& sessionImpl,
                                   const wxString& url) :
    wxWebRequestImpl(sessionImpl),
    m_sessionCURL(nullptr),
    m_handle(sessionImpl.GetHandle())
{
    DoStartPrepare(url);
}

void wxWebRequestCURL::DoStartPrepare(const wxString& url)
{
    if ( !m_handle )
    {
        wxStrlcpy(m_errorBuffer, "libcurl initialization failed", CURL_ERROR_SIZE);
        return;
    }

    // Set error buffer to get more detailed CURL status
    m_errorBuffer[0] = '\0';
    wxCURLSetOpt(m_handle, CURLOPT_ERRORBUFFER, m_errorBuffer);
    // Set URL to handle: note that we must use wxURI to escape characters not
    // allowed in the URLs correctly (URL API is only available in libcurl
    // since the relatively recent v7.62.0, so we don't want to rely on it).
    wxCURLSetOpt(m_handle, CURLOPT_URL, wxURI(url).BuildURI());
    // Set callback functions
    wxCURLSetOpt(m_handle, CURLOPT_WRITEFUNCTION, wxCURLWriteData);
    wxCURLSetOpt(m_handle, CURLOPT_HEADERFUNCTION, wxCURLHeader);
    wxCURLSetOpt(m_handle, CURLOPT_READFUNCTION, wxCURLRead);
    wxCURLSetOpt(m_handle, CURLOPT_READDATA, this);
    // Enable gzip, etc decompression
    wxCURLSetOpt(m_handle, CURLOPT_ACCEPT_ENCODING, "");
    // Enable redirection handling
    wxCURLSetOpt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
    // Limit redirect to HTTP
    #if CURL_AT_LEAST_VERSION(7, 85, 0)
    if ( wxWebSessionCURL::CurlRuntimeAtLeastVersion(7, 85, 0) )
    {
        wxCURLSetOpt(m_handle, CURLOPT_REDIR_PROTOCOLS_STR, "http,https");
    }
    else
    #endif // curl >= 7.85
    {
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)

        wxCURLSetOpt(m_handle, CURLOPT_REDIR_PROTOCOLS,
            CURLPROTO_HTTP | CURLPROTO_HTTPS);

        wxGCC_WARNING_RESTORE(deprecated-declarations)
    }

    // Configure proxy settings.
    const wxWebProxy& proxy = GetSessionImpl().GetProxy();
    bool usingProxy = true;
    switch ( proxy.GetType() )
    {
        case wxWebProxy::Type::URL:
            wxCURLSetOpt(m_handle, CURLOPT_PROXY, proxy.GetURL());
            break;

        case wxWebProxy::Type::Disabled:
            // This is a special value disabling use of proxy.
            wxCURLSetOpt(m_handle, CURLOPT_PROXY, "");
            usingProxy = false;
            break;

        case wxWebProxy::Type::Default:
            // Nothing to do, libcurl will use the standard http_proxy and
            // other similar environment variables by default.
            break;
    }

    // Enable all supported authentication methods
    wxCURLSetOpt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    if ( usingProxy )
        wxCURLSetOpt(m_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
}

wxWebRequestCURL::~wxWebRequestCURL()
{
    if ( m_headerList )
        curl_slist_free_all(m_headerList);

    if ( IsAsync() )
    {
        m_sessionCURL->RequestHasTerminated(this);

        curl_easy_cleanup(m_handle);
    }
}

wxWebRequest::Result wxWebRequestCURL::DoFinishPrepare()
{
    m_response.reset(new wxWebResponseCURL(*this));

    const auto result = m_response->InitFileStorage();
    if ( !result )
        return result;

    const wxString method = GetHTTPMethod();

    if ( method == "GET" )
    {
        // Nothing to do, libcurl defaults to GET. We could explicitly set
        // CURLOPT_HTTPGET option to 1, but this would be useless, as we always
        // reset the handle after making a request anyhow and curl_easy_reset()
        // already resets the method to GET.
    }
    else if ( method == "POST" )
    {
        wxCURLSetOpt(m_handle, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<long long>(m_dataSize));
        wxCURLSetOpt(m_handle, CURLOPT_POST, 1L);
    }
    else if ( method == "HEAD" )
    {
        wxCURLSetOpt(m_handle, CURLOPT_NOBODY, 1L);
    }
    else if ( method != "PUT" || m_dataSize == 0 )
    {
        wxCURLSetOpt(m_handle, CURLOPT_CUSTOMREQUEST, method);
    }
    //else: PUT will be used by default if we have any data to send (and if we
    //      don't, which should never happen but is nevertheless formally
    //      allowed, we've set it as custom request above).

    // POST is handled specially by libcurl, but for everything else, including
    // PUT as well as any other method, such as e.g. DELETE, we need to
    // explicitly request uploading the data, if any.
    if ( m_dataSize && method != "POST" )
    {
        wxCURLSetOpt(m_handle, CURLOPT_UPLOAD, 1L);
        wxCURLSetOpt(m_handle, CURLOPT_INFILESIZE_LARGE,
                     static_cast<long long>(m_dataSize));
    }

    for ( wxWebRequestHeaderMap::const_iterator it = m_headers.begin();
        it != m_headers.end(); ++it )
    {
        // TODO: We need to implement RFC 2047 encoding here instead of blindly
        //       sending UTF-8 which is against the standard.
        wxString hdrStr = wxString::Format("%s: %s", it->first, it->second);
        m_headerList = curl_slist_append(m_headerList, hdrStr.utf8_str());
    }
    wxCURLSetOpt(m_handle, CURLOPT_HTTPHEADER, m_headerList);

    const int securityFlags = GetSecurityFlags();
    if ( securityFlags & wxWebRequest::Ignore_Certificate )
        wxCURLSetOpt(m_handle, CURLOPT_SSL_VERIFYPEER, 0);
    if ( securityFlags & wxWebRequest::Ignore_Host )
        wxCURLSetOpt(m_handle, CURLOPT_SSL_VERIFYHOST, 0);

    return Result::Ok();
}

wxWebRequest::Result wxWebRequestCURL::Execute()
{
    const auto result = DoFinishPrepare();
    if ( result.state == wxWebRequest::State_Failed )
        return result;

    const CURLcode err = curl_easy_perform(m_handle);
    if ( err != CURLE_OK )
    {
        // This ensures that DoHandleCompletion() returns failure and uses
        // libcurl error message.
        m_response.reset(nullptr);
    }

    return DoHandleCompletion();
}

void wxWebRequestCURL::Start()
{
    if ( !CheckResult(DoFinishPrepare()) )
        return;

    StartRequest();
}

bool wxWebRequestCURL::StartRequest()
{
    m_bytesSent = 0;

    if ( !m_sessionCURL->StartRequest(*this) )
    {
        SetState(wxWebRequest::State_Failed);
        return false;
    }

    return true;
}

void wxWebRequestCURL::DoCancel()
{
    m_sessionCURL->CancelRequest(this);
}

wxWebRequest::Result wxWebRequestCURL::DoHandleCompletion()
{
    // This is a special case, we want to use libcurl error message if there is
    // no response at all.
    if ( !m_response || m_response->GetStatus() == 0 )
        return Result::Error(GetError());

    return GetResultFromHTTPStatus(m_response);
}

void wxWebRequestCURL::HandleCompletion()
{
    HandleResult(DoHandleCompletion());

    if ( GetState() == wxWebRequest::State_Unauthorized )
    {
        m_authChallenge.reset(
            new wxWebAuthChallengeCURL(
                m_response->GetStatus() == 407
                    ? wxWebAuthChallenge::Source_Proxy
                    : wxWebAuthChallenge::Source_Server,
                    *this
            )
        );
    }
}

wxString wxWebRequestCURL::GetError() const
{
    // We don't know what encoding is used for libcurl errors, so do whatever
    // is needed in order to interpret this data at least somehow.
    return wxString(m_errorBuffer, wxConvWhateverWorks);
}

size_t wxWebRequestCURL::CURLOnRead(char* buffer, size_t size)
{
    if ( m_dataStream )
    {
        m_dataStream->Read(buffer, size);
        size_t readSize = m_dataStream->LastRead();
        m_bytesSent += readSize;
        return readSize;
    }
    else
        return 0;
}

wxFileOffset wxWebRequestCURL::GetBytesSent() const
{
    return m_bytesSent;
}

wxFileOffset wxWebRequestCURL::GetBytesExpectedToSend() const
{
    return m_dataSize;
}

//
// wxWebAuthChallengeCURL
//

wxWebAuthChallengeCURL::wxWebAuthChallengeCURL(wxWebAuthChallenge::Source source,
                                               wxWebRequestCURL& request) :
    wxWebAuthChallengeImpl(source),
    m_request(request)
{
}

void wxWebAuthChallengeCURL::SetCredentials(const wxWebCredentials& cred)
{
    const wxSecretString authStr =
        wxString::Format
        (
            "%s:%s",
            cred.GetUser(),
            static_cast<const wxString&>(wxSecretString(cred.GetPassword()))
        );
    wxCURLSetOpt(m_request.GetHandle(),
        (GetSource() == wxWebAuthChallenge::Source_Proxy) ? CURLOPT_PROXYUSERPWD : CURLOPT_USERPWD,
        authStr);

    m_request.StartRequest();
}

//
// SocketPoller - a helper class for wxWebSessionCURL
//

wxDECLARE_EVENT(wxEVT_SOCKET_POLLER_RESULT, wxThreadEvent);

class SocketPollerImpl;

class SocketPoller
{
public:
    enum PollAction
    {
        INVALID_ACTION = 0x00,
        POLL_FOR_READ = 0x01,
        POLL_FOR_WRITE = 0x02
    };

    enum Result
    {
        INVALID_RESULT = 0x00,
        READY_FOR_READ = 0x01,
        READY_FOR_WRITE = 0x02,
        HAS_ERROR = 0x04
    };

    SocketPoller(wxEvtHandler*);
    ~SocketPoller();
    bool StartPolling(curl_socket_t, int);
    void StopPolling(curl_socket_t);
    void ResumePolling(curl_socket_t);

private:
    SocketPollerImpl* m_impl;
};

wxDEFINE_EVENT(wxEVT_SOCKET_POLLER_RESULT, wxThreadEvent);

class SocketPollerImpl
{
public:
    virtual ~SocketPollerImpl(){}
    virtual bool StartPolling(curl_socket_t, int) = 0;
    virtual void StopPolling(curl_socket_t) = 0;
    virtual void ResumePolling(curl_socket_t) = 0;

    static SocketPollerImpl* Create(wxEvtHandler*);
};

SocketPoller::SocketPoller(wxEvtHandler* hndlr)
{
    m_impl = SocketPollerImpl::Create(hndlr);
}

SocketPoller::~SocketPoller()
{
    delete m_impl;
}

bool SocketPoller::StartPolling(curl_socket_t sock, int pollAction)
{
    return m_impl->StartPolling(sock, pollAction);
}
void SocketPoller::StopPolling(curl_socket_t sock)
{
    m_impl->StopPolling(sock);
}

void SocketPoller::ResumePolling(curl_socket_t sock)
{
    m_impl->ResumePolling(sock);
}

#ifdef __WINDOWS__

class WinSock1SocketPoller: public SocketPollerImpl
{
public:
    WinSock1SocketPoller(wxEvtHandler*);
    virtual ~WinSock1SocketPoller();
    virtual bool StartPolling(curl_socket_t, int) override;
    virtual void StopPolling(curl_socket_t) override;
    virtual void ResumePolling(curl_socket_t) override;

private:
    static LRESULT CALLBACK MsgProc(HWND hwnd, WXUINT uMsg, WXWPARAM wParam,
                                    WXLPARAM lParam);
    static const WXUINT SOCKET_MESSAGE;

    using SocketSet = std::unordered_set<curl_socket_t>;

    SocketSet m_polledSockets;
    WXHWND m_hwnd;
};

const WXUINT WinSock1SocketPoller::SOCKET_MESSAGE = WM_USER + 1;

WinSock1SocketPoller::WinSock1SocketPoller(wxEvtHandler* hndlr)
{
    // Initialize winsock in case it's not already done.
    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    // Create a dummy message only window.
    m_hwnd = CreateWindowEx(
        0,              //DWORD     dwExStyle,
        TEXT("STATIC"), //LPCSTR    lpClassName,
        nullptr,        //LPCSTR    lpWindowName,
        0,              //DWORD     dwStyle,
        0,              //int       X,
        0,              //int       Y,
        0,              //int       nWidth,
        0,              //int       nHeight,
        HWND_MESSAGE,   //HWND      hWndParent,
        nullptr,        //HMENU     hMenu,
        nullptr,        //HINSTANCE hInstance,
        nullptr         //LPVOID    lpParam
    );

    if ( m_hwnd == nullptr )
    {
        wxLogError("Unable to create message window for WinSock1SocketPoller");
        return;
    }

    // Set the event handler to be the message window's user data. Also set the
    // message window to use our MsgProc to process messages it receives.
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(hndlr));
    SetWindowLongPtr(m_hwnd, GWLP_WNDPROC,
                     reinterpret_cast<LONG_PTR>(WinSock1SocketPoller::MsgProc));
}

WinSock1SocketPoller::~WinSock1SocketPoller()
{
    // Stop monitoring any leftover sockets.
    for ( SocketSet::iterator it = m_polledSockets.begin() ;
          it != m_polledSockets.end() ; ++it )
    {
        WSAAsyncSelect(*it, m_hwnd, 0, 0);
    }

    // Close the message window.
    if ( m_hwnd )
    {
        DestroyWindow(m_hwnd);
    }

    // Cleanup winsock.
    WSACleanup();
}

bool WinSock1SocketPoller::StartPolling(curl_socket_t sock, int pollAction)
{
    StopPolling(sock);

    // Convert pollAction to a flag that can be used by winsock.
    int winActions = 0;

    if ( pollAction & SocketPoller::POLL_FOR_READ )
    {
        winActions |= FD_READ;
    }

    if ( pollAction & SocketPoller::POLL_FOR_WRITE )
    {
        winActions |= FD_WRITE;
    }

    // Have winsock send a message to our window whenever activity is
    // detected on the socket.
    WSAAsyncSelect(sock, m_hwnd, SOCKET_MESSAGE, winActions);

    m_polledSockets.insert(sock);
    return true;
}

void WinSock1SocketPoller::StopPolling(curl_socket_t sock)
{
    SocketSet::iterator it = m_polledSockets.find(sock);

    if ( it != m_polledSockets.end() )
    {
        // Stop sending messages when there is activity on the socket.
        WSAAsyncSelect(sock, m_hwnd, 0, 0);
        m_polledSockets.erase(it);
    }
}

void WinSock1SocketPoller::ResumePolling(curl_socket_t WXUNUSED(sock))
{
}

LRESULT CALLBACK WinSock1SocketPoller::MsgProc(WXHWND hwnd, WXUINT uMsg,
                                               WXWPARAM wParam, WXLPARAM lParam)
{
    // We only handle 1 message - the message we told winsock to send when
    // it notices activity on sockets we are monitoring.

    if ( uMsg == SOCKET_MESSAGE )
    {
        // Extract the result any any errors from lParam.
        int winResult = LOWORD(lParam);
        int error = HIWORD(lParam);

        // Convert the result/errors to a SocketPoller::Result flag.
        int pollResult = 0;

        if ( winResult & FD_READ )
        {
            pollResult |= SocketPoller::READY_FOR_READ;
        }

        if ( winResult & FD_WRITE )
        {
            pollResult |= SocketPoller::READY_FOR_WRITE;
        }

        if ( error != 0 )
        {
            pollResult |= SocketPoller::HAS_ERROR;
        }

        // If there is a significant result, send an event.
        if ( pollResult != 0 )
        {
            // The event handler is stored in the window's user data and the
            // socket with activity is given by wParam.
            LONG_PTR userData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
            wxEvtHandler* hndlr = reinterpret_cast<wxEvtHandler*>(userData);
            curl_socket_t sock = wParam;

            wxThreadEvent* event =
                new wxThreadEvent(wxEVT_SOCKET_POLLER_RESULT);
            event->SetPayload<curl_socket_t>(sock);
            event->SetInt(pollResult);

            if ( wxThread::IsMain() )
            {
                hndlr->ProcessEvent(*event);
                delete event;
            }
            else
            {
                wxQueueEvent(hndlr, event);
            }
        }

        return 0;
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

SocketPollerImpl* SocketPollerImpl::Create(wxEvtHandler* hndlr)
{
    return new WinSock1SocketPoller(hndlr);
}

#else

// SocketPollerSourceHandler - a source handler used by the SocketPoller class.

class SocketPollerSourceHandler: public wxEventLoopSourceHandler
{
public:
    SocketPollerSourceHandler(curl_socket_t, wxEvtHandler*);

    void OnReadWaiting() override;
    void OnWriteWaiting() override;
    void OnExceptionWaiting() override;
    ~SocketPollerSourceHandler(){}
private:
    void SendEvent(int);
    curl_socket_t m_socket;
    wxEvtHandler* m_handler;
};

SocketPollerSourceHandler::SocketPollerSourceHandler(curl_socket_t sock,
                                                     wxEvtHandler* hndlr)
{
    m_socket = sock;
    m_handler = hndlr;
}

void SocketPollerSourceHandler::OnReadWaiting()
{
    SendEvent(SocketPoller::READY_FOR_READ);
}

void SocketPollerSourceHandler::OnWriteWaiting()
{
    SendEvent(SocketPoller::READY_FOR_WRITE);
}

void SocketPollerSourceHandler::OnExceptionWaiting()
{
    SendEvent(SocketPoller::HAS_ERROR);
}

void SocketPollerSourceHandler::SendEvent(int result)
{
    wxThreadEvent event(wxEVT_SOCKET_POLLER_RESULT);
    event.SetPayload<curl_socket_t>(m_socket);
    event.SetInt(result);
    m_handler->ProcessEvent(event);
}

// SourceSocketPoller - a SocketPollerImpl based on event loop sources.

class SourceSocketPoller: public SocketPollerImpl
{
public:
    SourceSocketPoller(wxEvtHandler*);
    ~SourceSocketPoller();
    bool StartPolling(curl_socket_t, int) override;
    void StopPolling(curl_socket_t) override;
    void ResumePolling(curl_socket_t) override;

private:
    using SocketDataMap = std::unordered_map<curl_socket_t, wxEventLoopSource*>;

    void CleanUpSocketSource(wxEventLoopSource*);

    SocketDataMap m_socketData;
    wxEvtHandler* m_handler;
};

SourceSocketPoller::SourceSocketPoller(wxEvtHandler* hndlr)
{
    m_handler = hndlr;
}

SourceSocketPoller::~SourceSocketPoller()
{
    // Clean up any leftover socket data.
    for ( SocketDataMap::iterator it = m_socketData.begin() ;
          it != m_socketData.end() ; ++it )
    {
        CleanUpSocketSource(it->second);
    }
}

static int SocketPoller2EventSource(int pollAction)
{
    // Convert the SocketPoller::PollAction value to a flag that can be used
    // by wxEventLoopSource.

    // Always check for errors.
    int eventSourceFlag = wxEVENT_SOURCE_EXCEPTION;

    if ( pollAction & SocketPoller::POLL_FOR_READ )
    {
        eventSourceFlag |= wxEVENT_SOURCE_INPUT;
    }

    if ( pollAction & SocketPoller::POLL_FOR_WRITE )
    {
        eventSourceFlag |= wxEVENT_SOURCE_OUTPUT;
    }

    return eventSourceFlag;
}

bool SourceSocketPoller::StartPolling(curl_socket_t sock, int pollAction)
{
    SocketDataMap::iterator it = m_socketData.find(sock);
    wxEventLoopSourceHandler* srcHandler = nullptr;

    if ( it != m_socketData.end() )
    {
        // If this socket is already being polled, reuse the old handler. Also
        // delete the old source object to stop the old polling operations.
        wxEventLoopSource* oldSrc = it->second;
        srcHandler = oldSrc->GetHandler();

        delete oldSrc;
    }
    else
    {
        // Otherwise create a new source handler.
        srcHandler =
            new SocketPollerSourceHandler(sock, m_handler);
    }

    // Get a new source object for these polling checks.
    bool socketIsPolled = true;
    int eventSourceFlag = SocketPoller2EventSource(pollAction);
    wxEventLoopSource* newSrc =
        wxEventLoopBase::AddSourceForFD(sock, srcHandler, eventSourceFlag);

    if ( newSrc == nullptr )
    {
        // We were not able to add a source for this socket.
        wxLogDebug(wxString::Format(
                       "Unable to create event loop source for %d",
                       static_cast<int>(sock)));

        delete srcHandler;
        socketIsPolled = false;

        if ( it != m_socketData.end() )
        {
            m_socketData.erase(it);
        }
    }
    else
    {
        m_socketData[sock] = newSrc;
    }

    return socketIsPolled;
}

void SourceSocketPoller::StopPolling(curl_socket_t sock)
{
    SocketDataMap::iterator it = m_socketData.find(sock);

    if ( it != m_socketData.end() )
    {
        CleanUpSocketSource(it->second);
        m_socketData.erase(it);
    }
}

void SourceSocketPoller::ResumePolling(curl_socket_t WXUNUSED(sock))
{
}

void SourceSocketPoller::CleanUpSocketSource(wxEventLoopSource* source)
{
    wxEventLoopSourceHandler* srcHandler = source->GetHandler();
    delete source;
    delete srcHandler;
}

SocketPollerImpl* SocketPollerImpl::Create(wxEvtHandler* hndlr)
{
    return new SourceSocketPoller(hndlr);
}

#endif

//
// wxWebSessionBaseCURL
//

int wxWebSessionBaseCURL::ms_activeSessions = 0;
unsigned int wxWebSessionBaseCURL::ms_runtimeVersion = 0;

wxWebSessionBaseCURL::wxWebSessionBaseCURL(Mode mode)
    : wxWebSessionImpl(mode)
{
    // Initialize CURL globally if no sessions are active
    if ( ms_activeSessions == 0 )
    {
        if ( curl_global_init(CURL_GLOBAL_ALL) )
        {
            wxLogError(_("libcurl could not be initialized"));
        }
        else
        {
            curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
            ms_runtimeVersion = data->version_num;
        }
    }

    ms_activeSessions++;
}

wxWebSessionBaseCURL::~wxWebSessionBaseCURL()
{
    // Global CURL cleanup if this is the last session
    --ms_activeSessions;
    if ( ms_activeSessions == 0 )
        curl_global_cleanup();
}

//
// wxWebSessionSyncCURL
//

wxWebSessionSyncCURL::wxWebSessionSyncCURL()
    : wxWebSessionBaseCURL(Mode::Sync)
{
}

wxWebSessionSyncCURL::~wxWebSessionSyncCURL()
{
    if ( m_handle )
        curl_easy_cleanup(m_handle);
}

wxWebRequestImplPtr
wxWebSessionSyncCURL::CreateRequestSync(wxWebSessionSync& WXUNUSED(session),
                                        const wxString& url)
{
    if ( !m_handle )
    {
        // Allocate it the first time we need it and keep it later.
        m_handle = curl_easy_init();
    }
    else
    {
        // But when reusing it subsequently, we must reset all the previously
        // set options to prevent the settings from one request from applying
        // to the subsequent ones.
        curl_easy_reset(m_handle);
    }

    return wxWebRequestImplPtr(new wxWebRequestCURL(*this, url));
}

//
// wxWebSessionCURL
//

wxWebSessionCURL::wxWebSessionCURL()
    : wxWebSessionBaseCURL(Mode::Async)
{
    m_socketPoller = new SocketPoller(this);
    m_timeoutTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &wxWebSessionCURL::TimeoutNotification, this);
    Bind(wxEVT_SOCKET_POLLER_RESULT,
         &wxWebSessionCURL::ProcessSocketPollerResult, this);
}

wxWebSessionCURL::~wxWebSessionCURL()
{
    delete m_socketPoller;

    if ( m_handle )
        curl_multi_cleanup(m_handle);
}

wxWebRequestImplPtr
wxWebSessionCURL::CreateRequest(wxWebSession& session,
                                wxEvtHandler* handler,
                                const wxString& url,
                                int id)
{
    // Allocate our handle on demand.
    if ( !m_handle )
    {
        m_handle = curl_multi_init();
        if ( !m_handle )
        {
            wxLogDebug("curl_multi_init() failed");
            return wxWebRequestImplPtr();
        }
        else
        {
            curl_multi_setopt(m_handle, CURLMOPT_SOCKETDATA, this);
            curl_multi_setopt(m_handle, CURLMOPT_SOCKETFUNCTION, SocketCallback);
            curl_multi_setopt(m_handle, CURLMOPT_TIMERDATA, this);
            curl_multi_setopt(m_handle, CURLMOPT_TIMERFUNCTION, TimerCallback);
        }
    }

    return wxWebRequestImplPtr(new wxWebRequestCURL(session, *this, handler, url, id));
}

bool wxWebSessionCURL::StartRequest(wxWebRequestCURL & request)
{
    // Add request easy handle to multi handle
    CURL* curl = request.GetHandle();
    int code = curl_multi_add_handle(m_handle, curl);

    if ( code == CURLM_OK )
    {
        request.SetState(wxWebRequest::State_Active);
        m_activeTransfers[curl] = &request;

        // Report a timeout to curl to initiate this transfer.
        int runningHandles;
        curl_multi_socket_action(m_handle, CURL_SOCKET_TIMEOUT, 0,
                                 &runningHandles);

        return true;
    }
    else
    {
        return false;
    }
}

void wxWebSessionCURL::CancelRequest(wxWebRequestCURL* request)
{
    // If this transfer is currently active, stop it.
    CURL* curl = request->GetHandle();
    StopActiveTransfer(curl);

    request->SetState(wxWebRequest::State_Cancelled);
}

void wxWebSessionCURL::RequestHasTerminated(wxWebRequestCURL* request)
{
    // If this transfer is currently active, stop it.
    CURL* curl = request->GetHandle();
    StopActiveTransfer(curl);
}

wxVersionInfo wxWebSessionBaseCURL::GetLibraryVersionInfo() const
{
    const curl_version_info_data* vi = curl_version_info(CURLVERSION_NOW);
    wxString desc = wxString::Format("libcurl/%s", vi->version);
    if (vi->ssl_version[0])
        desc += " " + wxString(vi->ssl_version);
    return wxVersionInfo("libcurl",
        vi->version_num >> 16 & 0xff,
        vi->version_num >> 8 & 0xff,
        vi->version_num & 0xff,
        desc);
}

bool wxWebSessionBaseCURL::CurlRuntimeAtLeastVersion(unsigned int major,
                                                     unsigned int minor,
                                                     unsigned int patch)
{
    return (ms_runtimeVersion >= CURL_VERSION_BITS(major, minor, patch));
}

// curl interacts with the wxWebSessionCURL class through 2 callback functions
// 1) TimerCallback is called whenever curl wants us to start or stop a timer.
// 2) SocketCallback is called when curl wants us to start monitoring a socket
//     for activity.
//
// curl accomplishes the network transfers by calling the
// curl_multi_socket_action function to move pieces of the transfer to or from
// the system's network services. Consequently we call this function when a
// timeout occurs or when activity is detected on a socket.

int wxWebSessionCURL::TimerCallback(CURLM* WXUNUSED(multi), long timeoutms,
                                    void *userp)
{
    wxWebSessionCURL* session = reinterpret_cast<wxWebSessionCURL*>(userp);
    session->ProcessTimerCallback(timeoutms);
    return 0;
}

int wxWebSessionCURL::SocketCallback(CURL* curl, curl_socket_t sock, int what,
                                     void* userp, void* WXUNUSED(sp))
{
    wxWebSessionCURL* session = reinterpret_cast<wxWebSessionCURL*>(userp);
    session->ProcessSocketCallback(curl, sock, what);
    return CURLM_OK;
};

void wxWebSessionCURL::ProcessTimerCallback(long timeoutms)
{
    // When this callback is called, curl wants us to start or stop a timer.
    // If timeoutms = -1, we should stop the timer. If timeoutms > 0, we should
    // start a oneshot timer and when that timer expires, we should call
    // curl_multi_socket_action(m_handle, CURL_SOCKET_TIMEOUT,...
    //
    // In the special case that timeoutms = 0, we should signal a timeout as
    // soon as possible (as above by calling curl_multi_socket_action). But
    // according to the curl documentation, we can't do that from this callback
    // or we might cause an infinite loop. So use CallAfter to report the
    // timeout at a slightly later time.

    if ( timeoutms > 0)
    {
        m_timeoutTimer.StartOnce(timeoutms);
    }
    else if ( timeoutms < 0 )
    {
        m_timeoutTimer.Stop();
    }
    else // timeoutms == 0
    {
        CallAfter(&wxWebSessionCURL::ProcessTimeoutNotification);
    }
}

void wxWebSessionCURL::TimeoutNotification(wxTimerEvent& WXUNUSED(event))
{
    ProcessTimeoutNotification();
}

void wxWebSessionCURL::ProcessTimeoutNotification()
{
    int runningHandles;
    curl_multi_socket_action(m_handle, CURL_SOCKET_TIMEOUT, 0, &runningHandles);

    CheckForCompletedTransfers();
}

static int CurlPoll2SocketPoller(int what)
{
    int pollAction = SocketPoller::INVALID_ACTION;

    if ( what == CURL_POLL_IN )
    {
        pollAction = SocketPoller::POLL_FOR_READ ;
    }
    else if ( what == CURL_POLL_OUT )
    {
        pollAction = SocketPoller::POLL_FOR_WRITE;
    }
    else if ( what == CURL_POLL_INOUT )
    {
        pollAction =
            SocketPoller::POLL_FOR_READ | SocketPoller::POLL_FOR_WRITE;
    }

    return pollAction;
}

void wxWebSessionCURL::ProcessSocketCallback(CURL* curl, curl_socket_t s,
                                             int what)
{
    // Have the socket poller start or stop monitoring a socket depending of
    // the value of what.

    switch ( what )
    {
        case CURL_POLL_IN:
            wxFALLTHROUGH;
        case CURL_POLL_OUT:
            wxFALLTHROUGH;
        case CURL_POLL_INOUT:
            {
                m_activeSockets[curl] = s;

                int pollAction = CurlPoll2SocketPoller(what);
                bool socketIsMonitored = m_socketPoller->StartPolling(s,
                                                                      pollAction);

                if ( !socketIsMonitored )
                {
                    TransferSet::iterator it = m_activeTransfers.find(curl);

                    if ( it != m_activeTransfers.end() )
                    {
                        FailRequest(curl,
                            "wxWebSession failed to monitor a socket for this "
                            "transfer");
                    }
                }
            }
            break;
        case CURL_POLL_REMOVE:
            m_socketPoller->StopPolling(s);
            RemoveActiveSocket(curl);
            break;
        default:
            wxLogDebug("Unknown socket action in ProcessSocketCallback");
            break;
    }
}

static int SocketPollerResult2CurlSelect(int socketEventFlag)
{
    int curlSelect = 0;

    if ( socketEventFlag & SocketPoller::READY_FOR_READ )
    {
        curlSelect |= CURL_CSELECT_IN;
    }

    if ( socketEventFlag & SocketPoller::READY_FOR_WRITE )
    {
        curlSelect |= CURL_CSELECT_OUT;
    }

    if ( socketEventFlag &  SocketPoller::HAS_ERROR )
    {
        curlSelect |= CURL_CSELECT_ERR;
    }

    return curlSelect;
}

void wxWebSessionCURL::ProcessSocketPollerResult(wxThreadEvent& event)
{
    // Convert the socket poller result to an action flag needed by curl.
    // Then call curl_multi_socket_action.
    curl_socket_t sock = event.GetPayload<curl_socket_t>();
    int action = SocketPollerResult2CurlSelect(event.GetInt());
    int runningHandles;
    curl_multi_socket_action(m_handle, sock, action, &runningHandles);

    CheckForCompletedTransfers();
    m_socketPoller->ResumePolling(sock);
}

void wxWebSessionCURL::CheckForCompletedTransfers()
{
    // Process CURL message queue
    int msgQueueCount;
    while ( CURLMsg* msg = curl_multi_info_read(m_handle, &msgQueueCount) )
    {
        if ( msg->msg == CURLMSG_DONE )
        {
            CURL* curl = msg->easy_handle;
            TransferSet::iterator it = m_activeTransfers.find(curl);

            if ( it != m_activeTransfers.end() )
            {
                wxWebRequestCURL* request = it->second;
                curl_multi_remove_handle(m_handle, curl);
                request->HandleCompletion();
                m_activeTransfers.erase(it);
                RemoveActiveSocket(curl);
            }
        }
    }
}

void wxWebSessionCURL::FailRequest(CURL* curl,const wxString& msg)
{
    TransferSet::iterator it = m_activeTransfers.find(curl);

    if ( it != m_activeTransfers.end() )
    {
        wxWebRequestCURL* request = it->second;
        StopActiveTransfer(curl);

        request->SetState(wxWebRequest::State_Failed, msg);
    }
}

void wxWebSessionCURL::StopActiveTransfer(CURL* curl)
{
    TransferSet::iterator it = m_activeTransfers.find(curl);

    if ( it != m_activeTransfers.end() )
    {
        // Record the current active socket now since it should be removed from
        // the m_activeSockets map when we call curl_multi_remove_handle.
        curl_socket_t activeSocket = CURL_SOCKET_BAD;
        CurlSocketMap::iterator it2 = m_activeSockets.find(curl);

        if ( it2 != m_activeSockets.end() )
        {
            activeSocket = it2->second;
        }

        // Remove the CURL easy handle from the CURLM multi handle.
        curl_multi_remove_handle(m_handle, curl);

        // If the transfer was active, close its socket.
        if ( activeSocket != CURL_SOCKET_BAD )
        {
            wxCloseSocket(activeSocket);
        }

        // Clean up the maps.
        RemoveActiveSocket(curl);
        m_activeTransfers.erase(it);
    }
}

void wxWebSessionCURL::RemoveActiveSocket(CURL* curl)
{
    CurlSocketMap::iterator it = m_activeSockets.find(curl);

    if ( it != m_activeSockets.end() )
    {
        m_activeSockets.erase(it);
    }
}

#endif // wxUSE_WEBREQUEST_CURL
