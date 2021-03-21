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
    #include "wx/hashset.h"
    #include "wx/msw/wrapwin.h"
#else
    #include "wx/evtloopsrc.h"
    #include "wx/evtloop.h"
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

wxWebResponseCURL::wxWebResponseCURL(wxWebRequestCURL& request) :
    wxWebResponseImpl(request)
{
    m_knownDownloadSize = 0;

    curl_easy_setopt(GetHandle(), CURLOPT_WRITEDATA, static_cast<void*>(this));
    curl_easy_setopt(GetHandle(), CURLOPT_HEADERDATA, static_cast<void*>(this));

 // Set the progress callback.
    #if CURL_AT_LEAST_VERSION(7, 32, 0)
        if ( wxWebSessionCURL::CurlRuntimeAtLeastVersion(7, 32, 0) )
        {
            curl_easy_setopt(GetHandle(), CURLOPT_XFERINFOFUNCTION,
                             wxCURLXferInfo);
            curl_easy_setopt(GetHandle(), CURLOPT_XFERINFODATA,
                             static_cast<void*>(this));
        }
        else
    #endif
        {
            curl_easy_setopt(GetHandle(), CURLOPT_PROGRESSFUNCTION,
                             wxCURLProgress);
            curl_easy_setopt(GetHandle(), CURLOPT_PROGRESSDATA,
                             static_cast<void*>(this));
        }

    // Have curl call the progress callback.
    curl_easy_setopt(GetHandle(), CURLOPT_NOPROGRESS, 0L);

    Init();
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
    char* urlp = NULL;
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
    m_sessionImpl(sessionImpl)
{
    m_headerList = NULL;

    m_handle = curl_easy_init();
    if ( !m_handle )
    {
        wxStrlcpy(m_errorBuffer, "libcurl initialization failed", CURL_ERROR_SIZE);
        return;
    }

    // Set error buffer to get more detailed CURL status
    m_errorBuffer[0] = '\0';
    curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, m_errorBuffer);
    // Set URL to handle: note that we must use wxURI to escape characters not
    // allowed in the URLs correctly (URL API is only available in libcurl
    // since the relatively recent v7.62.0, so we don't want to rely on it).
    curl_easy_setopt(m_handle, CURLOPT_URL, wxURI(url).BuildURI().utf8_str().data());
    // Set callback functions
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, wxCURLWriteData);
    curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, wxCURLHeader);
    curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, wxCURLRead);
    curl_easy_setopt(m_handle, CURLOPT_READDATA, static_cast<void*>(this));
    // Enable gzip, etc decompression
    curl_easy_setopt(m_handle, CURLOPT_ACCEPT_ENCODING, "");
    // Enable redirection handling
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
    // Limit redirect to HTTP
    curl_easy_setopt(m_handle, CURLOPT_REDIR_PROTOCOLS,
        CURLPROTO_HTTP | CURLPROTO_HTTPS);
    // Enable all supported authentication methods
    curl_easy_setopt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(m_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
}

wxWebRequestCURL::~wxWebRequestCURL()
{
    DestroyHeaderList();
    m_sessionImpl.RequestHasTerminated(this);
}

void wxWebRequestCURL::Start()
{
    m_response.reset(new wxWebResponseCURL(*this));

    if ( m_dataSize )
    {
        if ( m_method.empty() || m_method.CmpNoCase("POST") == 0 )
        {
            curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE_LARGE,
                static_cast<curl_off_t>(m_dataSize));
            curl_easy_setopt(m_handle, CURLOPT_POST, 1L);
        }
        else if ( m_method.CmpNoCase("PUT") == 0 )
        {
            curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(m_handle, CURLOPT_INFILESIZE_LARGE,
                static_cast<curl_off_t>(m_dataSize));
        }
        else
        {
            wxFAIL_MSG(wxString::Format(
                "Supplied data is ignored when using method %s", m_method
            ));
        }
    }

    if ( m_method.CmpNoCase("HEAD") == 0 )
    {
        curl_easy_setopt(m_handle, CURLOPT_NOBODY, 1L);
    }
    else if ( !m_method.empty() )
    {
        curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST,
            static_cast<const char*>(m_method.mb_str()));
    }

    for ( wxWebRequestHeaderMap::const_iterator it = m_headers.begin();
        it != m_headers.end(); ++it )
    {
        // TODO: We need to implement RFC 2047 encoding here instead of blindly
        //       sending UTF-8 which is against the standard.
        wxString hdrStr = wxString::Format("%s: %s", it->first, it->second);
        m_headerList = curl_slist_append(m_headerList, hdrStr.utf8_str());
    }
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_headerList);

    if ( IsPeerVerifyDisabled() )
        curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0);

    StartRequest();
}

bool wxWebRequestCURL::StartRequest()
{
    m_bytesSent = 0;

    if ( !m_sessionImpl.StartRequest(*this) )
    {
        SetState(wxWebRequest::State_Failed);
        return false;
    }

    return true;
}

void wxWebRequestCURL::DoCancel()
{
    m_sessionImpl.CancelRequest(this);
}

void wxWebRequestCURL::HandleCompletion()
{
    int status = m_response ? m_response->GetStatus() : 0;

    if ( status == 0 )
    {
        SetState(wxWebRequest::State_Failed, GetError());
    }
    else if ( status == 401 || status == 407 )
    {
        m_authChallenge.reset(new wxWebAuthChallengeCURL(
            (status == 407) ? wxWebAuthChallenge::Source_Proxy : wxWebAuthChallenge::Source_Server, *this));
        SetState(wxWebRequest::State_Unauthorized, m_response->GetStatusText());
    }
    else
    {
        SetFinalStateFromStatus();
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

void wxWebRequestCURL::DestroyHeaderList()
{
    if ( m_headerList )
    {
        curl_slist_free_all(m_headerList);
        m_headerList = NULL;
    }
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
    curl_easy_setopt(m_request.GetHandle(),
        (GetSource() == wxWebAuthChallenge::Source_Proxy) ? CURLOPT_PROXYUSERPWD : CURLOPT_USERPWD,
        authStr.utf8_str().data());
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
    bool StartPolling(wxSOCKET_T, int);
    void StopPolling(wxSOCKET_T);
    void ResumePolling(wxSOCKET_T);

private:
    SocketPollerImpl* m_impl;
};

wxDEFINE_EVENT(wxEVT_SOCKET_POLLER_RESULT, wxThreadEvent);

class SocketPollerImpl
{
public:
    virtual ~SocketPollerImpl(){}
    virtual bool StartPolling(wxSOCKET_T, int) = 0;
    virtual void StopPolling(wxSOCKET_T) = 0;
    virtual void ResumePolling(wxSOCKET_T) = 0;

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

bool SocketPoller::StartPolling(wxSOCKET_T sock, int pollAction)
{
    return m_impl->StartPolling(sock, pollAction);
}
void SocketPoller::StopPolling(wxSOCKET_T sock)
{
    m_impl->StopPolling(sock);
}

void SocketPoller::ResumePolling(wxSOCKET_T sock)
{
    m_impl->ResumePolling(sock);
}

#ifdef __WINDOWS__

class WinSock1SocketPoller: public SocketPollerImpl
{
public:
    WinSock1SocketPoller(wxEvtHandler*);
    virtual ~WinSock1SocketPoller();
    virtual bool StartPolling(wxSOCKET_T, int) wxOVERRIDE;
    virtual void StopPolling(wxSOCKET_T) wxOVERRIDE;
    virtual void ResumePolling(wxSOCKET_T) wxOVERRIDE;

private:
    static LRESULT CALLBACK MsgProc(HWND hwnd, WXUINT uMsg, WXWPARAM wParam,
                                    WXLPARAM lParam);
    static const WXUINT SOCKET_MESSAGE;

    WX_DECLARE_HASH_SET(wxSOCKET_T, wxIntegerHash, wxIntegerEqual, SocketSet);

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
        NULL,           //LPCSTR    lpWindowName,
        0,              //DWORD     dwStyle,
        0,              //int       X,
        0,              //int       Y,
        0,              //int       nWidth,
        0,              //int       nHeight,
        HWND_MESSAGE,   //HWND      hWndParent,
        NULL,           //HMENU     hMenu,
        NULL,           //HINSTANCE hInstance,
        NULL            //LPVOID    lpParam
    );

    if ( m_hwnd == NULL )
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
        CloseWindow(m_hwnd);
    }

    // Cleanup winsock.
    WSACleanup();
}

bool WinSock1SocketPoller::StartPolling(wxSOCKET_T sock, int pollAction)
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

void WinSock1SocketPoller::StopPolling(wxSOCKET_T sock)
{
    SocketSet::iterator it = m_polledSockets.find(sock);

    if ( it != m_polledSockets.end() )
    {
        // Stop sending messages when there is activity on the socket.
        WSAAsyncSelect(sock, m_hwnd, 0, 0);
        m_polledSockets.erase(it);
    }
}

void WinSock1SocketPoller::ResumePolling(wxSOCKET_T WXUNUSED(sock))
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
            wxSOCKET_T sock = wParam;

            wxThreadEvent* event =
                new wxThreadEvent(wxEVT_SOCKET_POLLER_RESULT);
            event->SetPayload<wxSOCKET_T>(sock);
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
    SocketPollerSourceHandler(wxSOCKET_T, wxEvtHandler*);

    void OnReadWaiting() wxOVERRIDE;
    void OnWriteWaiting() wxOVERRIDE;
    void OnExceptionWaiting() wxOVERRIDE;
    ~SocketPollerSourceHandler(){}
private:
    void SendEvent(int);
    wxSOCKET_T m_socket;
    wxEvtHandler* m_handler;
};

SocketPollerSourceHandler::SocketPollerSourceHandler(wxSOCKET_T sock,
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
    event.SetPayload<wxSOCKET_T>(m_socket);
    event.SetInt(result);
    m_handler->ProcessEvent(event);
}

// SourceSocketPoller - a SocketPollerImpl based on event loop sources.

class SourceSocketPoller: public SocketPollerImpl
{
public:
    SourceSocketPoller(wxEvtHandler*);
    ~SourceSocketPoller();
    bool StartPolling(wxSOCKET_T, int) wxOVERRIDE;
    void StopPolling(wxSOCKET_T) wxOVERRIDE;
    void ResumePolling(wxSOCKET_T) wxOVERRIDE;

private:
    WX_DECLARE_HASH_MAP(wxSOCKET_T, wxEventLoopSource*, wxIntegerHash,\
                        wxIntegerEqual, SocketDataMap);

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

bool SourceSocketPoller::StartPolling(wxSOCKET_T sock, int pollAction)
{
    SocketDataMap::iterator it = m_socketData.find(sock);
    wxEventLoopSourceHandler* srcHandler = NULL;

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

    if ( newSrc == NULL )
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

void SourceSocketPoller::StopPolling(wxSOCKET_T sock)
{
    SocketDataMap::iterator it = m_socketData.find(sock);

    if ( it != m_socketData.end() )
    {
        CleanUpSocketSource(it->second);
        m_socketData.erase(it);
    }
}

void SourceSocketPoller::ResumePolling(wxSOCKET_T WXUNUSED(sock))
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
// wxWebSessionCURL
//

int wxWebSessionCURL::ms_activeSessions = 0;
unsigned int wxWebSessionCURL::ms_runtimeVersion = 0;

wxWebSessionCURL::wxWebSessionCURL() :
    m_handle(NULL)
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

    // Global CURL cleanup if this is the last session
    --ms_activeSessions;
    if ( ms_activeSessions == 0 )
        curl_global_cleanup();
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

    curl_easy_cleanup(curl);
}

wxVersionInfo  wxWebSessionCURL::GetLibraryVersionInfo()
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

bool wxWebSessionCURL::CurlRuntimeAtLeastVersion(unsigned int major,
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
