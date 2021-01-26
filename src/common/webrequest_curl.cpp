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

#if wxUSE_WEBREQUEST_CURL

#include "wx/private/webrequest_curl.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/translation.h"
    #include "wx/utils.h"
#endif

#include "wx/uri.h"
#include "wx/socket.h"
#include "wx/msgqueue.h"
#include "wx/hashset.h"
#include "wx/hashmap.h"

#ifdef __WXMSW__
    #include <wx/msw/wrapwin.h>
#else
    #include <sys/socket.h>
    #include <sys/select.h>
#endif



// Define symbols that might be missing from older libcurl headers
#ifndef CURL_AT_LEAST_VERSION
#define CURL_VERSION_BITS(x,y,z) ((x)<<16|(y)<<8|z)
#define CURL_AT_LEAST_VERSION(x,y,z) \
  (LIBCURL_VERSION_NUM >= CURL_VERSION_BITS(x, y, z))
#endif

#if CURL_AT_LEAST_VERSION(7, 28, 0)
    #define wxCURL_HAVE_MULTI_WAIT 1
#else
    #define wxCURL_HAVE_MULTI_WAIT 0
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

wxWebResponseCURL::wxWebResponseCURL(wxWebRequestCURL& request) :
    wxWebResponseImpl(request)
{
    curl_easy_setopt(GetHandle(), CURLOPT_WRITEDATA, static_cast<void*>(this));
    curl_easy_setopt(GetHandle(), CURLOPT_HEADERDATA, static_cast<void*>(this));

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
    // Set this request in the private pointer
    curl_easy_setopt(m_handle, CURLOPT_PRIVATE, static_cast<void*>(this));
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

    curl_easy_cleanup(m_handle);
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


wxDECLARE_EVENT(wxSocketAction,wxThreadEvent);

class SocketPoller: public wxThreadHelper
{
public:
    enum PollAction
    {
        InvalidAction = 0,
        PollForRead = 1,
        PollForWrite = 2,
        PollForError = 4
    };

    enum Result
    {
        InvalidResult = 0,
        ReadyForRead = 1,
        ReadyForWrite = 2,
        HasError = 4
    };

    SocketPoller();
    ~SocketPoller();
    bool StartPolling(wxSOCKET_T, int, wxEvtHandler*);
    void StopPolling(wxSOCKET_T);
    void ResumePolling(wxSOCKET_T);

private:
    enum
    {
#ifdef INVALID_SOCKET
        SOCKET_POLLER_INVALID_SOCKET = INVALID_SOCKET
#else
        SOCKET_POLLER_INVALID_SOCKET = ((wxSOCKET_T)(~0))
#endif
    };

    class Message
    {
        public:
            enum MessageId
            {
                AddSocketAction,
                DeleteSocketAction,
                ResumePolling,
                Quit,
                LastMessageId
            };

            Message(MessageId i = LastMessageId,
                    wxSOCKET_T s = SOCKET_POLLER_INVALID_SOCKET,
                    int f = 0,
                    wxEvtHandler* h = NULL)
            {
                m_messageId = i;
                m_socket = s;
                m_flags = f;
                m_evtHandler = h;
            }

            MessageId GetMessageId() const { return m_messageId; }
            wxSOCKET_T GetSocket() const { return m_socket; }
            int GetFlags() const { return m_flags; }
            wxEvtHandler* GetEvtHandler() const { return m_evtHandler; }

        private:
            wxSOCKET_T m_socket;
            int m_flags;
            MessageId m_messageId;
            wxEvtHandler* m_evtHandler;
    };

    struct SocketData
    {
        int m_pollAction;
        void* m_event;
        wxEvtHandler* m_handler;
    };

    WX_DECLARE_HASH_SET(wxSOCKET_T, wxIntegerHash, wxIntegerEqual, SocketSet );
    WX_DECLARE_HASH_MAP(wxSOCKET_T, SocketData, wxIntegerHash, wxIntegerEqual, \
                        SocketDataMap );

    // Items used from main thread.
    void CreateSocketPair();
    void CloseSocket(wxSOCKET_T);
    void PostAndSignal(const Message&);

    SocketSet m_polledSockets;
    wxSOCKET_T m_writeEnd;


    // Items used in both threads.
    wxMessageQueue<Message> m_msgQueue;
    wxSOCKET_T m_readEnd;


    // Items used from worker thread.
    virtual wxThread::ExitCode Entry() wxOVERRIDE;

    void ThreadRemoveFromDL(wxSOCKET_T);
    void ThreadSetSocketAction(wxSOCKET_T, int,wxEvtHandler*);
    void ThreadDeleteSocketAction(wxSOCKET_T);
    void ThreadCheckSockets();

    SocketDataMap m_socketData;
    SocketSet m_disabledList;
};

wxDEFINE_EVENT(wxSocketAction,wxThreadEvent);


SocketPoller::SocketPoller()
{
    m_writeEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
    m_readEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
    CreateSocketPair();

    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogDebug("Could not create socket poller worker thread!");
        return;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogDebug("Could not run socket poller worker thread!");
        return;
    }
}

SocketPoller::~SocketPoller()
{
    SocketPoller::Message msg(Message::Quit);
    PostAndSignal(msg);

    GetThread()->Wait();

    CloseSocket(m_writeEnd);
    CloseSocket(m_readEnd);

#ifdef __WXMSW__
    WSACleanup();
#endif
}

void SocketPoller::CreateSocketPair()
{
#ifdef __WXMSW__
    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    m_writeEnd = socket(AF_INET, SOCK_DGRAM, 0);

    if ( m_writeEnd == INVALID_SOCKET )
    {
        wxLogDebug("Unable to create write end of socket pair.");
        m_writeEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
        return;
    }

    m_readEnd = socket(AF_INET, SOCK_DGRAM, 0);

    if ( m_readEnd == INVALID_SOCKET )
    {
        wxLogDebug("Unable to create read end of socket pair.");
        CloseSocket(m_writeEnd);
        m_writeEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
        m_readEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
        return;
    }

    // Bind the read end. This will assign it to an unused port.
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(0);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int retcode = bind(m_readEnd, (struct sockaddr *)&serverAddr,
                       sizeof(serverAddr));
    if ( retcode < 0 )
    {
        wxLogDebug("Unable bind socket to port.");
        CloseSocket(m_writeEnd);
        m_writeEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
        CloseSocket(m_readEnd);
        m_readEnd = SocketPoller::SOCKET_POLLER_INVALID_SOCKET;
        return;
    }

    // Get the ip address and port of the read end.
    struct sockaddr_in readEndAddr;
    ZeroMemory(&readEndAddr, sizeof(readEndAddr));
    int len = sizeof(readEndAddr);
    getsockname(m_readEnd, (struct sockaddr *) &readEndAddr, &len);

    // Unlike with stream sockets, this is a synchronous operation and just
    // sets location for the send function.
    connect(m_writeEnd, (struct sockaddr *)&readEndAddr, sizeof(readEndAddr));
#else
    int fd[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    m_writeEnd = fd[0];
    m_readEnd = fd[1];
#endif
}

void SocketPoller::CloseSocket(wxSOCKET_T s)
{
#ifdef __WXMSW__
    closesocket(s);
#else
    close(s);
#endif
}

bool SocketPoller::StartPolling(wxSOCKET_T s, int flags, wxEvtHandler* hndlr)
{
    SocketSet::iterator it = m_polledSockets.find(s);

    if ( m_polledSockets.size() > 1023 )
    {
        if ( it == m_polledSockets.end() )
        {
            return false;
        }
    }

    if ( it == m_polledSockets.end() )
    {
        m_polledSockets.insert(s);
    }

    SocketPoller::Message msg(Message::AddSocketAction, s, flags, hndlr);
    PostAndSignal(msg);

    return true;
}

void SocketPoller::StopPolling(wxSOCKET_T s)
{
    SocketPoller::Message msg(Message::DeleteSocketAction, s);
    PostAndSignal(msg);

    SocketSet::iterator it = m_polledSockets.find(s);

    if ( it != m_polledSockets.end() )
    {
        m_polledSockets.erase(it);
    }
}

void SocketPoller::ResumePolling(wxSOCKET_T s)
{
    SocketPoller::Message msg(Message::ResumePolling, s);
    PostAndSignal(msg);
}

void SocketPoller::PostAndSignal(const Message& msg)
{
    m_msgQueue.Post(msg);

    if ( m_writeEnd != SocketPoller::SOCKET_POLLER_INVALID_SOCKET )
    {
        char c = 32;
        send(m_writeEnd, &c, 1, 0);
    }
}

wxThread::ExitCode SocketPoller::Entry()
{
    wxMessageQueueError er;
    SocketPoller::Message msg;
    bool done = false;
    size_t socketsUnderConsideration = 0;
    int timeOut = 50;
    Message::MessageId id;

    while ( !done )
    {
        if ( GetThread()->TestDestroy() )
        {
            break;
        }

        // Process all messages in the message queue. If we currently have no
        // sockets, wait a little for a message to come in.
        timeOut = (socketsUnderConsideration == 0) ? 50 : 0;
        er = wxMSGQUEUE_NO_ERROR;

        while ( er == wxMSGQUEUE_NO_ERROR )
        {
            er = m_msgQueue.ReceiveTimeout(timeOut, msg);
            id = msg.GetMessageId();

            if ( er == wxMSGQUEUE_TIMEOUT )
            {
                break;
            }
            else if ( er == wxMSGQUEUE_MISC_ERROR )
            {
                wxLogDebug("Error with socket poller message queue.");
                done = true;
                break;
            }
            else if ( id == Message::Quit )
            {
                done = true;
                break;
            }
            else if ( id == Message::AddSocketAction )
            {
                wxSOCKET_T s = msg.GetSocket();
                int f = msg.GetFlags();
                wxEvtHandler* hndlr = msg.GetEvtHandler();
                ThreadSetSocketAction(s, f, hndlr);
            }
            else if ( id == Message::DeleteSocketAction )
            {
                wxSOCKET_T s = msg.GetSocket();
                ThreadDeleteSocketAction(s);
            }
            else if ( id == Message::ResumePolling )
            {
                wxSOCKET_T s = msg.GetSocket();
                ThreadRemoveFromDL(s);
            }

            timeOut = 0;
        }

        socketsUnderConsideration = m_socketData.size() - m_disabledList.size();

        if ( socketsUnderConsideration  > 0 && !done )
        {
            ThreadCheckSockets();
        }
    }

    return static_cast<wxThread::ExitCode>(0);
}

void SocketPoller::ThreadRemoveFromDL(wxSOCKET_T s)
{
    SocketSet::iterator it = m_disabledList.find(s);

    if ( it != m_disabledList.end() )
    {
        m_disabledList.erase(it);
    }
}

void SocketPoller::ThreadDeleteSocketAction(wxSOCKET_T sock)
{
    ThreadRemoveFromDL(sock);

    SocketDataMap::iterator it = m_socketData.find(sock);

    if ( it != m_socketData.end() )
    {
        m_socketData.erase(it);
    }
}

void SocketPoller::ThreadSetSocketAction(wxSOCKET_T sock, int flags,
                                         wxEvtHandler* hndlr)
{
    ThreadDeleteSocketAction(sock);

    SocketData data;
    data.m_pollAction = flags;
    data.m_event = NULL;
    data.m_handler = hndlr;

    m_socketData[sock] = data;
}

void SocketPoller::ThreadCheckSockets()
{
    fd_set readFds, writeFds, errorFds;
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    FD_ZERO(&errorFds);

    wxSOCKET_T maxSd = 0;

    if ( m_readEnd != SocketPoller::SOCKET_POLLER_INVALID_SOCKET )
    {
        FD_SET(m_readEnd, &readFds);
        maxSd = m_readEnd;
    }

    for ( SocketDataMap::iterator it = m_socketData.begin() ;
          it != m_socketData.end() ; ++it )
    {
        wxSOCKET_T sock = it->first;

        if ( m_disabledList.find(sock) != m_disabledList.end() )
        {
            continue;
        }

        int checkAction = it->second.m_pollAction;

        if ( checkAction & PollForRead )
        {
            FD_SET(sock, &readFds);
        }

        if ( checkAction & PollForWrite )
        {
            FD_SET(sock, &writeFds);
        }

        FD_SET(sock, &errorFds);

        if ( sock > maxSd )
        {
            maxSd = sock;
        }
    }

    struct timeval timeout;
    timeout.tv_sec = 0;  // 1s timeout
    timeout.tv_usec = 50*1000;

    int selectStatus = select(maxSd+1, &readFds, &writeFds, &errorFds,&timeout);

    if ( selectStatus < 0 )
    {
        // Massive error: do something
    }
    else if ( selectStatus == 0 )
    {
        ;// select timed out.  There is no need to do anything.
    }
    else
    {
        if ( (m_readEnd != SocketPoller::SOCKET_POLLER_INVALID_SOCKET)
              && FD_ISSET(m_readEnd, &readFds) )
        {
            char c;
            recv(m_readEnd, &c, 1, 0);
        }

        for ( SocketDataMap::iterator it = m_socketData.begin() ;
              it != m_socketData.end() ; ++it )
        {
            wxSOCKET_T sock = it->first;

            if ( m_disabledList.find(sock) != m_disabledList.end() )
            {
                continue;
            }

            int checkActions = it->second.m_pollAction;
            wxEvtHandler* hndlr = it->second.m_handler;
            int result = InvalidResult;

            if ( checkActions & PollForRead )
            {
                if ( FD_ISSET(sock, &readFds) )
                {
                    result |= ReadyForRead;
                }
            }

            if ( checkActions & ReadyForWrite )
            {
                if ( FD_ISSET(sock, &writeFds) )
                {
                    result |= ReadyForWrite;
                }
            }

            if ( FD_ISSET(sock, &errorFds) )
            {
                result |= HasError;
            }

            if ( result != InvalidResult && hndlr != NULL )
            {
                wxThreadEvent* event = new wxThreadEvent(wxSocketAction);
                event->SetPayload<wxSOCKET_T>(sock);
                event->SetInt(result);
                wxQueueEvent(hndlr,event);
                m_disabledList.insert(sock);
            }
        }
    }
}



//
// wxWebSessionCURL
//

int wxWebSessionCURL::ms_activeSessions = 0;

wxWebSessionCURL::wxWebSessionCURL() :
    m_handle(NULL)
{
    // Initialize CURL globally if no sessions are active
    if ( ms_activeSessions == 0 )
    {
        if ( curl_global_init(CURL_GLOBAL_ALL) )
            wxLogError(_("libcurl could not be initialized"));
    }

    ms_activeSessions++;

    m_socketPoller = new SocketPoller();
    m_timeoutTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &wxWebSessionCURL::TimeoutNotification, this);
    Bind(wxSocketAction, &wxWebSessionCURL::ProcessSocketPollerResult, this);
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
    curl_multi_add_handle(m_handle, curl);

    request.SetState(wxWebRequest::State_Active);

    // Report a timeout to curl to initiate this transfer.
    int runningHandles;
    curl_multi_socket_action(m_handle, CURL_SOCKET_TIMEOUT, 0, &runningHandles);

    return true;
}

void wxWebSessionCURL::CancelRequest(wxWebRequestCURL* request)
{
    curl_multi_remove_handle(m_handle, request->GetHandle());
    request->SetState(wxWebRequest::State_Cancelled);
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

int wxWebSessionCURL::SocketCallback(CURL* WXUNUSED(easy), curl_socket_t sock,
                                    int what, void* userp, void* WXUNUSED(sp))
{
    wxWebSessionCURL* session = reinterpret_cast<wxWebSessionCURL*>(userp);
    session->ProcessSocketCallback(sock, what);
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
    int pollAction = SocketPoller::InvalidAction;

    if ( what == CURL_POLL_IN )
    {
        pollAction = SocketPoller::PollForRead ;
    }
    else if ( what == CURL_POLL_OUT )
    {
        pollAction = SocketPoller::PollForWrite;
    }
    else if ( what == CURL_POLL_INOUT )
    {
        pollAction = SocketPoller::PollForRead | SocketPoller::PollForWrite;
    }

    return pollAction;
}

void wxWebSessionCURL::ProcessSocketCallback(curl_socket_t s, int what)
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
            m_socketPoller->StartPolling(s, CurlPoll2SocketPoller(what), this);
            break;
        case CURL_POLL_REMOVE:
            m_socketPoller->StopPolling(s);
            break;
        default:
            wxLogDebug("Unknown socket action in ProcessSocketCallback");
            break;
    }
}

static int SocketPollerResult2CurlSelect(int socketEventFlag)
{
    int curlSelect = 0;

    if ( socketEventFlag & SocketPoller::ReadyForRead )
    {
        curlSelect |= CURL_CSELECT_IN;
    }

    if ( socketEventFlag & SocketPoller::ReadyForWrite )
    {
        curlSelect |= CURL_CSELECT_OUT;
    }

    if ( socketEventFlag &  SocketPoller::HasError )
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
            wxWebRequestCURL* request;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &request);
            curl_multi_remove_handle(m_handle, msg->easy_handle);
            request->HandleCompletion();
        }
    }
}

#endif // wxUSE_WEBREQUEST_CURL
