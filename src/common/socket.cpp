/////////////////////////////////////////////////////////////////////////////
// Name:       src/common/socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia
// Created:    April 1997
// Copyright:  (C) 1999-1997, Guilhem Lavaux
//             (C) 1999-2000, Guillermo Rodriguez Garcia
//             (C) 2008 Vadim Zeitlin
// RCS_ID:     $Id$
// License:    wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// Declarations
// ==========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include "wx/socket.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/timer.h"
    #include "wx/module.h"
#endif

#include "wx/apptrait.h"
#include "wx/sckaddr.h"
#include "wx/stopwatch.h"
#include "wx/thread.h"
#include "wx/evtloop.h"

#include "wx/private/fd.h"
#include "wx/private/socket.h"

#ifdef __UNIX__
    #include <errno.h>
#endif

// we use MSG_NOSIGNAL to avoid getting SIGPIPE when sending data to a remote
// host which closed the connection if it is available, otherwise we rely on
// SO_NOSIGPIPE existency
//
// this should cover all the current Unix systems (Windows never sends any
// signals anyhow) but if we find one that has neither we should explicitly
// ignore SIGPIPE for it
#ifdef MSG_NOSIGNAL
    #define wxSOCKET_MSG_NOSIGNAL MSG_NOSIGNAL
#else // MSG_NOSIGNAL not available (BSD including OS X)
    #if defined(__UNIX__) && !defined(SO_NOSIGPIPE)
        #error "Writing to socket could generate unhandled SIGPIPE."
        #error "Please post information about your system to wx-dev."
    #endif

    #define wxSOCKET_MSG_NOSIGNAL 0
#endif

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxNet")

// --------------------------------------------------------------------------
// macros and constants
// --------------------------------------------------------------------------

// discard buffer
#define MAX_DISCARD_SIZE (10 * 1024)

#define wxTRACE_Socket _T("wxSocket")

// --------------------------------------------------------------------------
// wxWin macros
// --------------------------------------------------------------------------

IMPLEMENT_CLASS(wxSocketBase, wxObject)
IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
IMPLEMENT_CLASS(wxDatagramSocket, wxSocketBase)
IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

namespace
{

void SetTimeValFromMS(timeval& tv, unsigned long ms)
{
    tv.tv_sec  = (ms / 1000);
    tv.tv_usec = (ms % 1000) * 1000;
}

} // anonymous namespace

// --------------------------------------------------------------------------
// private classes
// --------------------------------------------------------------------------

class wxSocketState : public wxObject
{
public:
    wxSocketFlags            m_flags;
    wxSocketEventFlags       m_eventmask;
    bool                     m_notify;
    void                    *m_clientData;

public:
    wxSocketState() : wxObject() {}

    DECLARE_NO_COPY_CLASS(wxSocketState)
};

// ============================================================================
// wxSocketManager
// ============================================================================

wxSocketManager *wxSocketManager::ms_manager = NULL;

/* static */
void wxSocketManager::Set(wxSocketManager *manager)
{
    wxASSERT_MSG( !ms_manager, "too late to set manager now" );

    ms_manager = manager;
}

/* static */
void wxSocketManager::Init()
{
    wxASSERT_MSG( !ms_manager, "shouldn't be initialized twice" );

    /*
        Details: Initialize() creates a hidden window as a sink for socket
        events, such as 'read completed'. wxMSW has only one message loop
        for the main thread. If Initialize is called in a secondary thread,
        the socket window will be created for the secondary thread, but
        since there is no message loop on this thread, it will never
        receive events and all socket operations will time out.
        BTW, the main thread must not be stopped using sleep or block
        on a semaphore (a bad idea in any case) or socket operations
        will time out.

        On the Mac side, Initialize() stores a pointer to the CFRunLoop for
        the main thread. Because secondary threads do not have run loops,
        adding event notifications to the "Current" loop would have no
        effect at all, events would never fire.
    */
    wxASSERT_MSG( wxIsMainThread(),
                    "sockets must be initialized from the main thread" );

    wxAppConsole * const app = wxAppConsole::GetInstance();
    wxCHECK_RET( app, "sockets can't be initialized without wxApp" );

    ms_manager = app->GetTraits()->GetSocketManager();
}

// ==========================================================================
// wxSocketImpl
// ==========================================================================

wxSocketImpl::wxSocketImpl(wxSocketBase& wxsocket)
    : m_wxsocket(&wxsocket)
{
    m_fd              = INVALID_SOCKET;
    m_error           = wxSOCKET_NOERROR;
    m_server          = false;
    m_stream          = true;

    SetTimeout(wxsocket.GetTimeout() * 1000);

    m_establishing    = false;
    m_reusable        = false;
    m_broadcast       = false;
    m_dobind          = true;
    m_initialRecvBufferSize = -1;
    m_initialSendBufferSize = -1;
}

wxSocketImpl::~wxSocketImpl()
{
    if ( m_fd != INVALID_SOCKET )
        Shutdown();
}

bool wxSocketImpl::PreCreateCheck(const wxSockAddressImpl& addr)
{
    if ( m_fd != INVALID_SOCKET )
    {
        m_error = wxSOCKET_INVSOCK;
        return false;
    }

    if ( !addr.IsOk() )
    {
        m_error = wxSOCKET_INVADDR;
        return false;
    }

    return true;
}

void wxSocketImpl::PostCreation()
{
    // FreeBSD variants can't use MSG_NOSIGNAL, and instead use a socket option
#ifdef SO_NOSIGPIPE
    EnableSocketOption(SO_NOSIGPIPE);
#endif

    if ( m_reusable )
        EnableSocketOption(SO_REUSEADDR);

    if ( m_broadcast )
    {
        wxASSERT_MSG( !m_stream, "broadcasting is for datagram sockets only" );

        EnableSocketOption(SO_BROADCAST);
    }

    if ( m_initialRecvBufferSize >= 0 )
        SetSocketOption(SO_RCVBUF, m_initialRecvBufferSize);
    if ( m_initialSendBufferSize >= 0 )
        SetSocketOption(SO_SNDBUF, m_initialSendBufferSize);

    // we always put our sockets in unblocked mode and handle blocking
    // ourselves in DoRead/Write() if wxSOCKET_WAITALL is specified
    UnblockAndRegisterWithEventLoop();
}

wxSocketError wxSocketImpl::UpdateLocalAddress()
{
    WX_SOCKLEN_T lenAddr = m_local.GetLen();
    if ( getsockname(m_fd, m_local.GetWritableAddr(), &lenAddr) != 0 )
    {
        Close();
        m_error = wxSOCKET_IOERR;
        return m_error;
    }

    return wxSOCKET_NOERROR;
}

wxSocketError wxSocketImpl::CreateServer()
{
    if ( !PreCreateCheck(m_local) )
        return m_error;

    m_server = true;
    m_stream = true;

    // do create the socket
    m_fd = socket(m_local.GetFamily(), SOCK_STREAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    PostCreation();

    // and then bind to and listen on it
    //
    // FIXME: should we test for m_dobind here?
    if ( bind(m_fd, m_local.GetAddr(), m_local.GetLen()) != 0 )
        m_error = wxSOCKET_IOERR;

    if ( IsOk() )
    {
        if ( listen(m_fd, 5) != 0 )
            m_error = wxSOCKET_IOERR;
    }

    if ( !IsOk() )
    {
        Close();
        return m_error;
    }

    // finally retrieve the address we effectively bound to
    return UpdateLocalAddress();
}

wxSocketError wxSocketImpl::CreateClient(bool wait)
{
    if ( !PreCreateCheck(m_peer) )
        return m_error;

    m_fd = socket(m_peer.GetFamily(), SOCK_STREAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    PostCreation();

    // If a local address has been set, then bind to it before calling connect
    if ( m_local.IsOk() )
    {
        if ( bind(m_fd, m_local.GetAddr(), m_local.GetLen()) != 0 )
        {
            Close();
            m_error = wxSOCKET_IOERR;
            return m_error;
        }
    }

    // Do connect now
    int rc = connect(m_fd, m_peer.GetAddr(), m_peer.GetLen());
    if ( rc == SOCKET_ERROR )
    {
        wxSocketError err = GetLastError();
        if ( err == wxSOCKET_WOULDBLOCK )
        {
            m_establishing = true;

            // block waiting for connection if we should (otherwise just return
            // wxSOCKET_WOULDBLOCK to the caller)
            if ( wait )
            {
                err = SelectWithTimeout(wxSOCKET_CONNECTION_FLAG)
                        ? wxSOCKET_NOERROR
                        : wxSOCKET_TIMEDOUT;
                m_establishing = false;
            }
        }

        m_error = err;
    }
    else // connected
    {
        m_error = wxSOCKET_NOERROR;
    }

    return m_error;
}


wxSocketError wxSocketImpl::CreateUDP()
{
    if ( !PreCreateCheck(m_local) )
        return m_error;

    m_stream = false;
    m_server = false;

    m_fd = socket(m_local.GetFamily(), SOCK_DGRAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    PostCreation();

    if ( m_dobind )
    {
        if ( bind(m_fd, m_local.GetAddr(), m_local.GetLen()) != 0 )
        {
            Close();
            m_error = wxSOCKET_IOERR;
            return m_error;
        }

        return UpdateLocalAddress();
    }

    return wxSOCKET_NOERROR;
}

wxSocketImpl *wxSocketImpl::Accept(wxSocketBase& wxsocket)
{
    wxSockAddressStorage from;
    WX_SOCKLEN_T fromlen = sizeof(from);
    const SOCKET fd = accept(m_fd, &from.addr, &fromlen);

    if ( fd == INVALID_SOCKET )
        return NULL;

    wxSocketImpl * const sock = Create(wxsocket);
    sock->m_fd = fd;
    sock->m_peer = wxSockAddressImpl(from.addr, fromlen);

    sock->UnblockAndRegisterWithEventLoop();

    return sock;
}


void wxSocketImpl::Close()
{
    if ( m_fd != INVALID_SOCKET )
    {
        DoClose();
        m_fd = INVALID_SOCKET;
    }
}

/*
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void wxSocketImpl::Shutdown()
{
    if ( m_fd != INVALID_SOCKET )
    {
        shutdown(m_fd, 1 /* SD_SEND */);
        Close();
    }
}

/*
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void wxSocketImpl::SetTimeout(unsigned long millis)
{
    SetTimeValFromMS(m_timeout, millis);
}

void wxSocketImpl::NotifyOnStateChange(wxSocketNotify event)
{
    m_wxsocket->OnRequest(event);
}

/* Address handling */
wxSocketError wxSocketImpl::SetLocal(const wxSockAddressImpl& local)
{
    /* the socket must be initialized, or it must be a server */
    if (m_fd != INVALID_SOCKET && !m_server)
    {
        m_error = wxSOCKET_INVSOCK;
        return wxSOCKET_INVSOCK;
    }

    if ( !local.IsOk() )
    {
        m_error = wxSOCKET_INVADDR;
        return wxSOCKET_INVADDR;
    }

    m_local = local;

    return wxSOCKET_NOERROR;
}

wxSocketError wxSocketImpl::SetPeer(const wxSockAddressImpl& peer)
{
    if ( !peer.IsOk() )
    {
        m_error = wxSOCKET_INVADDR;
        return wxSOCKET_INVADDR;
    }

    m_peer = peer;

    return wxSOCKET_NOERROR;
}

const wxSockAddressImpl& wxSocketImpl::GetLocal()
{
    if ( !m_local.IsOk() )
        UpdateLocalAddress();

    return m_local;
}

// ----------------------------------------------------------------------------
// wxSocketImpl IO
// ----------------------------------------------------------------------------

// this macro wraps the given expression (normally a syscall) in a loop which
// ignores any interruptions, i.e. reevaluates it again if it failed and errno
// is EINTR
#ifdef __UNIX__
    #define DO_WHILE_EINTR( rc, syscall ) \
        do { \
            rc = (syscall); \
        } \
        while ( rc == -1 && errno == EINTR )
#else
    #define DO_WHILE_EINTR( rc, syscall ) rc = (syscall)
#endif

int wxSocketImpl::RecvStream(void *buffer, int size)
{
    int ret;
    DO_WHILE_EINTR( ret, recv(m_fd, static_cast<char *>(buffer), size, 0) );

    if ( !ret )
    {
        // receiving 0 bytes for a TCP socket indicates that the connection was
        // closed by peer so shut down our end as well (for UDP sockets empty
        // datagrams are also possible)
        m_establishing = false;
        NotifyOnStateChange(wxSOCKET_LOST);

        Shutdown();

        // do not return an error in this case however
    }

    return ret;
}

int wxSocketImpl::SendStream(const void *buffer, int size)
{
    int ret;
    DO_WHILE_EINTR( ret, send(m_fd, static_cast<const char *>(buffer), size,
                              wxSOCKET_MSG_NOSIGNAL) );

    return ret;
}

int wxSocketImpl::RecvDgram(void *buffer, int size)
{
    wxSockAddressStorage from;
    WX_SOCKLEN_T fromlen = sizeof(from);

    int ret;
    DO_WHILE_EINTR( ret, recvfrom(m_fd, static_cast<char *>(buffer), size,
                                  0, &from.addr, &fromlen) );

    if ( ret == SOCKET_ERROR )
        return SOCKET_ERROR;

    m_peer = wxSockAddressImpl(from.addr, fromlen);
    if ( !m_peer.IsOk() )
        return -1;

    return ret;
}

int wxSocketImpl::SendDgram(const void *buffer, int size)
{
    if ( !m_peer.IsOk() )
    {
        m_error = wxSOCKET_INVADDR;
        return -1;
    }

    int ret;
    DO_WHILE_EINTR( ret, sendto(m_fd, static_cast<const char *>(buffer), size,
                                0, m_peer.GetAddr(), m_peer.GetLen()) );

    return ret;
}

int wxSocketImpl::Read(void *buffer, int size)
{
    // server sockets can't be used for IO, only to accept new connections
    if ( m_fd == INVALID_SOCKET || m_server )
    {
        m_error = wxSOCKET_INVSOCK;
        return -1;
    }

    int ret = m_stream ? RecvStream(buffer, size)
                       : RecvDgram(buffer, size);

    m_error = ret == SOCKET_ERROR ? GetLastError() : wxSOCKET_NOERROR;

    return ret;
}

int wxSocketImpl::Write(const void *buffer, int size)
{
    if ( m_fd == INVALID_SOCKET || m_server )
    {
        m_error = wxSOCKET_INVSOCK;
        return -1;
    }

    int ret = m_stream ? SendStream(buffer, size)
                       : SendDgram(buffer, size);

    m_error = ret == SOCKET_ERROR ? GetLastError() : wxSOCKET_NOERROR;

    return ret;
}

// ==========================================================================
// wxSocketBase
// ==========================================================================

// --------------------------------------------------------------------------
// Initialization and shutdown
// --------------------------------------------------------------------------

// FIXME-MT: all this is MT-unsafe, of course, we should protect all accesses
//           to m_countInit with a crit section
size_t wxSocketBase::m_countInit = 0;

bool wxSocketBase::IsInitialized()
{
    return m_countInit > 0;
}

bool wxSocketBase::Initialize()
{
    if ( !m_countInit++ )
    {
        wxSocketManager * const manager = wxSocketManager::Get();
        if ( !manager || !manager->OnInit() )
        {
            m_countInit--;

            return false;
        }
    }

    return true;
}

void wxSocketBase::Shutdown()
{
    // we should be initialized
    wxASSERT_MSG( m_countInit > 0, _T("extra call to Shutdown()") );
    if ( --m_countInit == 0 )
    {
        wxSocketManager * const manager = wxSocketManager::Get();
        wxCHECK_RET( manager, "should have a socket manager" );

        manager->OnExit();
    }
}

// --------------------------------------------------------------------------
// Ctor and dtor
// --------------------------------------------------------------------------

void wxSocketBase::Init()
{
    m_impl       = NULL;
    m_type         = wxSOCKET_UNINIT;

    // state
    m_flags        = 0;
    m_connected    =
    m_establishing =
    m_reading      =
    m_writing      =
    m_closed       = false;
    m_lcount       = 0;
    m_timeout      = 600;
    m_beingDeleted = false;

    // pushback buffer
    m_unread       = NULL;
    m_unrd_size    = 0;
    m_unrd_cur     = 0;

    // events
    m_id           = wxID_ANY;
    m_handler      = NULL;
    m_clientData   = NULL;
    m_notify       = false;
    m_eventmask    =
    m_eventsgot    = 0;

    if ( !IsInitialized() )
    {
        // this Initialize() will be undone by wxSocketModule::OnExit(), all
        // the other calls to it should be matched by a call to Shutdown()
        Initialize();
    }
}

wxSocketBase::wxSocketBase()
{
    Init();
}

wxSocketBase::wxSocketBase(wxSocketFlags flags, wxSocketType type)
{
    Init();

    SetFlags(flags);

    m_type = type;
}

wxSocketBase::~wxSocketBase()
{
    // Just in case the app called Destroy() *and* then deleted the socket
    // immediately: don't leave dangling pointers.
    wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
    if ( traits )
        traits->RemoveFromPendingDelete(this);

    // Shutdown and close the socket
    if (!m_beingDeleted)
        Close();

    // Destroy the implementation object
    delete m_impl;

    // Free the pushback buffer
    if (m_unread)
        free(m_unread);
}

bool wxSocketBase::Destroy()
{
    // Delayed destruction: the socket will be deleted during the next idle
    // loop iteration. This ensures that all pending events have been
    // processed.
    m_beingDeleted = true;

    // Shutdown and close the socket
    Close();

    // Suppress events from now on
    Notify(false);

    // schedule this object for deletion
    wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
    if ( traits )
    {
        // let the traits object decide what to do with us
        traits->ScheduleForDestroy(this);
    }
    else // no app or no traits
    {
        // in wxBase we might have no app object at all, don't leak memory
        delete this;
    }

    return true;
}

// ----------------------------------------------------------------------------
// simple accessors
// ----------------------------------------------------------------------------

void wxSocketBase::SetError(wxSocketError error)
{
    m_impl->m_error = error;
}

wxSocketError wxSocketBase::LastError() const
{
    return m_impl->GetError();
}

// --------------------------------------------------------------------------
// Basic IO calls
// --------------------------------------------------------------------------

// The following IO operations update m_lcount:
// {Read, Write, ReadMsg, WriteMsg, Peek, Unread, Discard}
bool wxSocketBase::Close()
{
    // Interrupt pending waits
    InterruptWait();

    ShutdownOutput();

    m_connected = false;
    m_establishing = false;
    return true;
}

void wxSocketBase::ShutdownOutput()
{
    if ( m_impl )
        m_impl->Shutdown();
}

wxSocketBase& wxSocketBase::Read(void* buffer, wxUint32 nbytes)
{
    // Mask read events
    m_reading = true;

    m_lcount = DoRead(buffer, nbytes);

    // Allow read events from now on
    m_reading = false;

    return *this;
}

wxUint32 wxSocketBase::DoRead(void* buffer_, wxUint32 nbytes)
{
    wxCHECK_MSG( m_impl, 0, "socket must be valid" );

    // We use pointer arithmetic here which doesn't work with void pointers.
    char *buffer = static_cast<char *>(buffer_);
    wxCHECK_MSG( buffer, 0, "NULL buffer" );

    // Try the push back buffer first, even before checking whether the socket
    // is valid to allow reading previously pushed back data from an already
    // closed socket.
    wxUint32 total = GetPushback(buffer, nbytes, false);
    nbytes -= total;
    buffer += total;

    while ( nbytes )
    {
        // our socket is non-blocking so Read() will return immediately if
        // there is nothing to read yet and it's more efficient to try it first
        // before entering WaitForRead() which is going to start dispatching
        // GUI events and, even more importantly, we must do this under Windows
        // where we're not going to get notifications about socket being ready
        // for reading before we read all the existing data from it
        const int ret = m_connected ? m_impl->Read(buffer, nbytes) : 0;
        if ( ret == -1 )
        {
            if ( m_impl->GetLastError() == wxSOCKET_WOULDBLOCK )
            {
                // if we don't want to wait, just return immediately
                if ( m_flags & wxSOCKET_NOWAIT )
                    break;

                // otherwise wait until the socket becomes ready for reading
                if ( !WaitForRead() )
                {
                    // and exit if the timeout elapsed before it did
                    SetError(wxSOCKET_TIMEDOUT);
                    break;
                }

                // retry reading
                continue;
            }
            else // "real" error
            {
                SetError(wxSOCKET_IOERR);
                break;
            }
        }
        else if ( ret == 0 )
        {
            // for connection-oriented (e.g. TCP) sockets we can only read
            // 0 bytes if the other end has been closed, and for connectionless
            // ones (UDP) this flag doesn't make sense anyhow so we can set it
            // to true too without doing any harm
            m_closed = true;

            // we're not going to read anything else and so if we haven't read
            // anything (or not everything in wxSOCKET_WAITALL case) already,
            // signal an error
            if ( (m_flags & wxSOCKET_WAITALL) || !total )
                SetError(wxSOCKET_IOERR);
            break;
        }

        total += ret;

        // if we are happy to read something and not the entire nbytes bytes,
        // then we're done
        if ( !(m_flags & wxSOCKET_WAITALL) )
            break;

        nbytes -= ret;
        buffer += ret;
    }

    return total;
}

wxSocketBase& wxSocketBase::ReadMsg(void* buffer, wxUint32 nbytes)
{
    struct
    {
        unsigned char sig[4];
        unsigned char len[4];
    } msg;

    // Mask read events
    m_reading = true;

    int old_flags = m_flags;
    SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

    bool ok = false;
    if ( DoRead(&msg, sizeof(msg)) == sizeof(msg) )
    {
        wxUint32 sig = (wxUint32)msg.sig[0];
        sig |= (wxUint32)(msg.sig[1] << 8);
        sig |= (wxUint32)(msg.sig[2] << 16);
        sig |= (wxUint32)(msg.sig[3] << 24);

        if ( sig == 0xfeeddead )
        {
            wxUint32 len = (wxUint32)msg.len[0];
            len |= (wxUint32)(msg.len[1] << 8);
            len |= (wxUint32)(msg.len[2] << 16);
            len |= (wxUint32)(msg.len[3] << 24);

            wxUint32 len2;
            if (len > nbytes)
            {
                len2 = len - nbytes;
                len = nbytes;
            }
            else
                len2 = 0;

            // Don't attempt to read if the msg was zero bytes long.
            m_lcount = len ? DoRead(buffer, len) : 0;

            if ( len2 )
            {
                char discard_buffer[MAX_DISCARD_SIZE];
                long discard_len;

                // NOTE: discarded bytes don't add to m_lcount.
                do
                {
                    discard_len = len2 > MAX_DISCARD_SIZE
                                    ? MAX_DISCARD_SIZE
                                    : len2;
                    discard_len = DoRead(discard_buffer, (wxUint32)discard_len);
                    len2 -= (wxUint32)discard_len;
                }
                while ((discard_len > 0) && len2);
            }

            if ( !len2 && DoRead(&msg, sizeof(msg)) == sizeof(msg) )
            {
                sig = (wxUint32)msg.sig[0];
                sig |= (wxUint32)(msg.sig[1] << 8);
                sig |= (wxUint32)(msg.sig[2] << 16);
                sig |= (wxUint32)(msg.sig[3] << 24);

                if ( sig == 0xdeadfeed )
                    ok = true;
            }
        }
    }

    if ( !ok )
        SetError(wxSOCKET_IOERR);

    m_reading = false;
    SetFlags(old_flags);

    return *this;
}

wxSocketBase& wxSocketBase::Peek(void* buffer, wxUint32 nbytes)
{
    // Mask read events
    m_reading = true;

    m_lcount = DoRead(buffer, nbytes);
    Pushback(buffer, m_lcount);

    // Allow read events again
    m_reading = false;

    return *this;
}

wxSocketBase& wxSocketBase::Write(const void *buffer, wxUint32 nbytes)
{
    // Mask write events
    m_writing = true;

    m_lcount = DoWrite(buffer, nbytes);

    // Allow write events again
    m_writing = false;

    return *this;
}

// This function is a mirror image of DoRead() except that it doesn't use the
// push back buffer and doesn't treat 0 return value specially (normally this
// shouldn't happen at all here), so please see comments there for explanations
wxUint32 wxSocketBase::DoWrite(const void *buffer_, wxUint32 nbytes)
{
    wxCHECK_MSG( m_impl, 0, "socket must be valid" );

    const char *buffer = static_cast<const char *>(buffer_);
    wxCHECK_MSG( buffer, 0, "NULL buffer" );

    wxUint32 total = 0;
    while ( nbytes )
    {
        if ( !m_connected )
        {
            if ( (m_flags & wxSOCKET_WAITALL) || !total )
                SetError(wxSOCKET_IOERR);
            break;
        }

        const int ret = m_impl->Write(buffer, nbytes);
        if ( ret == -1 )
        {
            if ( m_impl->GetLastError() == wxSOCKET_WOULDBLOCK )
            {
                if ( m_flags & wxSOCKET_NOWAIT )
                    break;

                if ( !WaitForWrite() )
                {
                    SetError(wxSOCKET_TIMEDOUT);
                    break;
                }

                continue;
            }
            else // "real" error
            {
                SetError(wxSOCKET_IOERR);
                break;
            }
        }

        total += ret;

        if ( !(m_flags & wxSOCKET_WAITALL) )
            break;

        nbytes -= ret;
        buffer += ret;
    }

    return total;
}

wxSocketBase& wxSocketBase::WriteMsg(const void *buffer, wxUint32 nbytes)
{
    struct
    {
        unsigned char sig[4];
        unsigned char len[4];
    } msg;

    // Mask write events
    m_writing = true;

    const int old_flags = m_flags;
    SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

    msg.sig[0] = (unsigned char) 0xad;
    msg.sig[1] = (unsigned char) 0xde;
    msg.sig[2] = (unsigned char) 0xed;
    msg.sig[3] = (unsigned char) 0xfe;

    msg.len[0] = (unsigned char) (nbytes & 0xff);
    msg.len[1] = (unsigned char) ((nbytes >> 8) & 0xff);
    msg.len[2] = (unsigned char) ((nbytes >> 16) & 0xff);
    msg.len[3] = (unsigned char) ((nbytes >> 24) & 0xff);

    bool ok = false;
    if ( DoWrite(&msg, sizeof(msg)) == sizeof(msg) )
    {
        m_lcount = DoWrite(buffer, nbytes);
        if ( m_lcount == nbytes )
        {
            msg.sig[0] = (unsigned char) 0xed;
            msg.sig[1] = (unsigned char) 0xfe;
            msg.sig[2] = (unsigned char) 0xad;
            msg.sig[3] = (unsigned char) 0xde;
            msg.len[0] =
            msg.len[1] =
            msg.len[2] =
            msg.len[3] = (char) 0;

            if ( DoWrite(&msg, sizeof(msg)) == sizeof(msg))
                ok = true;
        }
    }

    if ( !ok )
        SetError(wxSOCKET_IOERR);

    m_writing = false;
    SetFlags(old_flags);

    return *this;
}

wxSocketBase& wxSocketBase::Unread(const void *buffer, wxUint32 nbytes)
{
    if (nbytes != 0)
        Pushback(buffer, nbytes);

    SetError(wxSOCKET_NOERROR);
    m_lcount = nbytes;

    return *this;
}

wxSocketBase& wxSocketBase::Discard()
{
    char *buffer = new char[MAX_DISCARD_SIZE];
    wxUint32 ret;
    wxUint32 total = 0;

    // Mask read events
    m_reading = true;

    const int old_flags = m_flags;
    SetFlags(wxSOCKET_NOWAIT);

    do
    {
        ret = DoRead(buffer, MAX_DISCARD_SIZE);
        total += ret;
    }
    while (ret == MAX_DISCARD_SIZE);

    delete[] buffer;
    m_lcount = total;
    SetError(wxSOCKET_NOERROR);

    // Allow read events again
    m_reading = false;

    SetFlags(old_flags);

    return *this;
}

// --------------------------------------------------------------------------
// Wait functions
// --------------------------------------------------------------------------

/*
    This function will check for the events specified in the flags parameter,
    and it will return a mask indicating which operations can be performed.
 */
wxSocketEventFlags wxSocketImpl::Select(wxSocketEventFlags flags,
                                        const timeval *timeout)
{
    if ( m_fd == INVALID_SOCKET )
        return (wxSOCKET_LOST_FLAG & flags);

    struct timeval tv;
    if ( timeout )
        tv = *timeout;
    else
        tv.tv_sec = tv.tv_usec = 0;

    // prepare the FD sets, passing NULL for the one(s) we don't use
    fd_set
        readfds, *preadfds = NULL,
        writefds, *pwritefds = NULL,
        exceptfds;                      // always want to know about errors

    if ( flags & wxSOCKET_INPUT_FLAG )
    {
        preadfds = &readfds;
        wxFD_ZERO(preadfds);
        wxFD_SET(m_fd, preadfds);
    }

    // when using non-blocking connect() the socket becomes connected
    // (successfully or not) when it becomes writable
    if ( flags & (wxSOCKET_OUTPUT_FLAG | wxSOCKET_CONNECTION_FLAG) )
    {
        pwritefds = &writefds;
        wxFD_ZERO(pwritefds);
        wxFD_SET(m_fd, pwritefds);
    }

    wxFD_ZERO(&exceptfds);
    wxFD_SET(m_fd, &exceptfds);

    const int rc = select(m_fd + 1, preadfds, pwritefds, &exceptfds, &tv);

    // check for errors first
    if ( rc == -1 || wxFD_ISSET(m_fd, &exceptfds) )
    {
        m_establishing = false;

        return wxSOCKET_LOST_FLAG & flags;
    }

    if ( rc == 0 )
        return 0;

    wxASSERT_MSG( rc == 1, "unexpected select() return value" );

    wxSocketEventFlags detected = 0;
    if ( preadfds && wxFD_ISSET(m_fd, preadfds) )
        detected |= wxSOCKET_INPUT_FLAG;

    if ( pwritefds && wxFD_ISSET(m_fd, pwritefds) )
    {
        // check for the case of non-blocking connect()
        if ( m_establishing && !m_server )
        {
            int error;
            SOCKOPTLEN_T len = sizeof(error);
            m_establishing = false;
            getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

            if ( error )
                detected = wxSOCKET_LOST_FLAG;
            else
                detected |= wxSOCKET_CONNECTION_FLAG;
        }
        else // not called to get non-blocking connect() status
        {
            detected |= wxSOCKET_OUTPUT_FLAG;
        }
    }

    return detected & flags;
}

bool
wxSocketBase::DoWait(long seconds, long milliseconds, wxSocketEventFlags flags)
{
    wxCHECK_MSG( m_impl, false, "can't wait on invalid socket" );

    // we're never going to become ready if we're not connected (any more)
    if ( !m_connected && !m_establishing )
        return (flags & wxSOCKET_LOST_FLAG) != 0;

    // This can be set to true from Interrupt() to exit this function a.s.a.p.
    m_interrupt = false;


    // Use either the provided timeout or the default timeout value associated
    // with this socket.
    //
    // TODO: allow waiting forever, see #9443
    const long timeout = seconds == -1 ? m_timeout * 1000
                                       : seconds * 1000 + milliseconds;
    const wxMilliClock_t timeEnd = wxGetLocalTimeMillis() + timeout;

    // Get the active event loop which we'll use for the message dispatching
    // when running in the main thread unless this was explicitly disabled by
    // setting wxSOCKET_BLOCK flag
    wxEventLoopBase *eventLoop;
    if ( !(m_flags & wxSOCKET_BLOCK) && wxIsMainThread() )
    {
        eventLoop = wxEventLoop::GetActive();
    }
    else // in worker thread
    {
        // We never dispatch messages from threads other than the main one.
        eventLoop = NULL;
    }

    // Wait until we receive the event we're waiting for or the timeout expires
    // (but note that we always execute the loop at least once, even if timeout
    // is 0 as this is used for polling)
    bool gotEvent = false;
    for ( bool firstTime = true; !m_interrupt ; firstTime = false )
    {
        long timeLeft = wxMilliClockToLong(timeEnd - wxGetLocalTimeMillis());
        if ( timeLeft < 0 )
        {
            if ( !firstTime )
                break;   // timed out

            timeLeft = 0;
        }

        wxSocketEventFlags events;
        if ( eventLoop )
        {
            // reset them before starting to wait
            m_eventsgot = 0;

            eventLoop->DispatchTimeout(timeLeft);

            events = m_eventsgot;
        }
        else // no event loop or waiting in another thread
        {
            // as explained below, we should always check for wxSOCKET_LOST_FLAG
            timeval tv;
            SetTimeValFromMS(tv, timeLeft);
            events = m_impl->Select(flags | wxSOCKET_LOST_FLAG, &tv);
        }

        // always check for wxSOCKET_LOST_FLAG, even if flags doesn't include
        // it, as continuing to wait for anything else after getting it is
        // pointless
        if ( events & wxSOCKET_LOST_FLAG )
        {
            m_connected = false;
            m_establishing = false;
            if ( flags & wxSOCKET_LOST_FLAG )
                gotEvent = true;
            break;
        }

        // otherwise mask out the bits we're not interested in
        events &= flags;

        // Incoming connection (server) or connection established (client)?
        if ( events & wxSOCKET_CONNECTION_FLAG )
        {
            m_connected = true;
            m_establishing = false;
            gotEvent = true;
            break;
        }

        // Data available or output buffer ready?
        if ( (events & wxSOCKET_INPUT_FLAG) || (events & wxSOCKET_OUTPUT_FLAG) )
        {
            gotEvent = true;
            break;
        }
    }

    return gotEvent;
}

bool wxSocketBase::Wait(long seconds, long milliseconds)
{
    return DoWait(seconds, milliseconds,
            wxSOCKET_INPUT_FLAG |
            wxSOCKET_OUTPUT_FLAG |
            wxSOCKET_CONNECTION_FLAG |
            wxSOCKET_LOST_FLAG
        );
}

bool wxSocketBase::WaitForRead(long seconds, long milliseconds)
{
    // Check pushback buffer before entering DoWait
    if ( m_unread )
        return true;

    // Note that wxSOCKET_INPUT_LOST has to be explicitly passed to DoWait
    // because of the semantics of WaitForRead: a return value of true means
    // that a Read call will return immediately, not that there is
    // actually data to read.
    return DoWait(seconds, milliseconds, wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
}


bool wxSocketBase::WaitForWrite(long seconds, long milliseconds)
{
    return DoWait(seconds, milliseconds, wxSOCKET_OUTPUT_FLAG | wxSOCKET_LOST_FLAG);
}

bool wxSocketBase::WaitForLost(long seconds, long milliseconds)
{
    return DoWait(seconds, milliseconds, wxSOCKET_LOST_FLAG);
}

// --------------------------------------------------------------------------
// Miscellaneous
// --------------------------------------------------------------------------

//
// Get local or peer address
//

bool wxSocketBase::GetPeer(wxSockAddress& addr) const
{
    wxCHECK_MSG( m_impl, false, "invalid socket" );

    const wxSockAddressImpl& peer = m_impl->GetPeer();
    if ( !peer.IsOk() )
        return false;

    addr.SetAddress(peer);

    return true;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr) const
{
    wxCHECK_MSG( m_impl, false, "invalid socket" );

    const wxSockAddressImpl& local = m_impl->GetLocal();
    if ( !local.IsOk() )
        return false;

    addr.SetAddress(local);

    return true;
}

//
// Save and restore socket state
//

void wxSocketBase::SaveState()
{
    wxSocketState *state;

    state = new wxSocketState();

    state->m_flags      = m_flags;
    state->m_notify     = m_notify;
    state->m_eventmask  = m_eventmask;
    state->m_clientData = m_clientData;

    m_states.Append(state);
}

void wxSocketBase::RestoreState()
{
    wxList::compatibility_iterator node;
    wxSocketState *state;

    node = m_states.GetLast();
    if (!node)
        return;

    state = (wxSocketState *)node->GetData();

    m_flags      = state->m_flags;
    m_notify     = state->m_notify;
    m_eventmask  = state->m_eventmask;
    m_clientData = state->m_clientData;

    m_states.Erase(node);
    delete state;
}

//
// Timeout and flags
//

void wxSocketBase::SetTimeout(long seconds)
{
    m_timeout = seconds;

    if (m_impl)
        m_impl->SetTimeout(m_timeout * 1000);
}

void wxSocketBase::SetFlags(wxSocketFlags flags)
{
    // Do some sanity checking on the flags used: not all values can be used
    // together.
    wxASSERT_MSG( !(flags & wxSOCKET_NOWAIT) ||
                  !(flags & (wxSOCKET_WAITALL | wxSOCKET_BLOCK)),
                  "Using wxSOCKET_WAITALL or wxSOCKET_BLOCK with "
                  "wxSOCKET_NOWAIT doesn't make sense" );

    m_flags = flags;
}


// --------------------------------------------------------------------------
// Event handling
// --------------------------------------------------------------------------

void wxSocketBase::OnRequest(wxSocketNotify notification)
{
    wxSocketEventFlags flag = 0;
    switch ( notification )
    {
        case wxSOCKET_INPUT:
            flag = wxSOCKET_INPUT_FLAG;
            break;

        case wxSOCKET_OUTPUT:
            flag = wxSOCKET_OUTPUT_FLAG;
            break;

        case wxSOCKET_CONNECTION:
            flag = wxSOCKET_CONNECTION_FLAG;
            break;

        case wxSOCKET_LOST:
            flag = wxSOCKET_LOST_FLAG;
            break;

        default:
            wxFAIL_MSG( "unknown wxSocket notification" );
    }

    // if we lost the connection the socket is now closed
    if ( notification == wxSOCKET_LOST )
        m_closed = true;

    // remember the events which were generated for this socket, we're going to
    // use this in DoWait()
    m_eventsgot |= flag;

    // send the wx event if enabled and we're interested in it
    if ( m_notify && (m_eventmask & flag) && m_handler )
    {
        // If we are in the middle of a R/W operation, do not propagate events
        // to users. Also, filter 'late' events which are no longer valid.
        if ( notification == wxSOCKET_INPUT )
        {
            if ( m_reading || !m_impl->Select(wxSOCKET_INPUT_FLAG) )
                return;
        }
        else if ( notification == wxSOCKET_OUTPUT )
        {
            if ( m_writing || !m_impl->Select(wxSOCKET_OUTPUT_FLAG) )
                return;
        }

        wxSocketEvent event(m_id);
        event.m_event      = notification;
        event.m_clientData = m_clientData;
        event.SetEventObject(this);

        m_handler->AddPendingEvent(event);
    }
}

void wxSocketBase::Notify(bool notify)
{
    m_notify = notify;
}

void wxSocketBase::SetNotify(wxSocketEventFlags flags)
{
    m_eventmask = flags;
}

void wxSocketBase::SetEventHandler(wxEvtHandler& handler, int id)
{
    m_handler = &handler;
    m_id      = id;
}

// --------------------------------------------------------------------------
// Pushback buffer
// --------------------------------------------------------------------------

void wxSocketBase::Pushback(const void *buffer, wxUint32 size)
{
    if (!size) return;

    if (m_unread == NULL)
        m_unread = malloc(size);
    else
    {
        void *tmp;

        tmp = malloc(m_unrd_size + size);
        memcpy((char *)tmp + size, m_unread, m_unrd_size);
        free(m_unread);

        m_unread = tmp;
    }

    m_unrd_size += size;

    memcpy(m_unread, buffer, size);
}

wxUint32 wxSocketBase::GetPushback(void *buffer, wxUint32 size, bool peek)
{
    wxCHECK_MSG( buffer, 0, "NULL buffer" );

    if (!m_unrd_size)
        return 0;

    if (size > (m_unrd_size-m_unrd_cur))
        size = m_unrd_size-m_unrd_cur;

    memcpy(buffer, (char *)m_unread + m_unrd_cur, size);

    if (!peek)
    {
        m_unrd_cur += size;
        if (m_unrd_size == m_unrd_cur)
        {
            free(m_unread);
            m_unread = NULL;
            m_unrd_size = 0;
            m_unrd_cur  = 0;
        }
    }

    return size;
}


// ==========================================================================
// wxSocketServer
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor
// --------------------------------------------------------------------------

wxSocketServer::wxSocketServer(const wxSockAddress& addr,
                               wxSocketFlags flags)
              : wxSocketBase(flags, wxSOCKET_SERVER)
{
    wxLogTrace( wxTRACE_Socket, _T("Opening wxSocketServer") );

    m_impl = wxSocketImpl::Create(*this);

    if (!m_impl)
    {
        wxLogTrace( wxTRACE_Socket, _T("*** Failed to create m_impl") );
        return;
    }

    // Setup the socket as server
    m_impl->SetLocal(addr.GetAddress());

    if (GetFlags() & wxSOCKET_REUSEADDR) {
        m_impl->SetReusable();
    }
    if (GetFlags() & wxSOCKET_BROADCAST) {
        m_impl->SetBroadcast();
    }
    if (GetFlags() & wxSOCKET_NOBIND) {
        m_impl->DontDoBind();
    }

    if (m_impl->CreateServer() != wxSOCKET_NOERROR)
    {
        delete m_impl;
        m_impl = NULL;

        wxLogTrace( wxTRACE_Socket, _T("*** CreateServer() failed") );
        return;
    }

    wxLogTrace( wxTRACE_Socket, _T("wxSocketServer on fd %d"), m_impl->m_fd );
}

// --------------------------------------------------------------------------
// Accept
// --------------------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock, bool wait)
{
    if ( !m_impl || (m_impl->m_fd == INVALID_SOCKET) || !m_impl->IsServer() )
    {
        wxFAIL_MSG( "can only be called for a valid server socket" );

        SetError(wxSOCKET_INVSOCK);

        return false;
    }

    if ( wait )
    {
        // wait until we get a connection
        if ( !m_impl->SelectWithTimeout(wxSOCKET_INPUT_FLAG) )
        {
            SetError(wxSOCKET_TIMEDOUT);

            return false;
        }
    }

    sock.m_impl = m_impl->Accept(sock);

    if ( !sock.m_impl )
    {
        SetError(m_impl->GetLastError());

        return false;
    }

    sock.m_type = wxSOCKET_BASE;
    sock.m_connected = true;

    return true;
}

wxSocketBase *wxSocketServer::Accept(bool wait)
{
    wxSocketBase* sock = new wxSocketBase();

    sock->SetFlags(m_flags);

    if (!AcceptWith(*sock, wait))
    {
        sock->Destroy();
        sock = NULL;
    }

    return sock;
}

bool wxSocketServer::WaitForAccept(long seconds, long milliseconds)
{
    return DoWait(seconds, milliseconds, wxSOCKET_CONNECTION_FLAG);
}

bool wxSocketBase::GetOption(int level, int optname, void *optval, int *optlen)
{
    wxASSERT_MSG( m_impl, _T("Socket not initialised") );

    SOCKOPTLEN_T lenreal = *optlen;
    if ( getsockopt(m_impl->m_fd, level, optname,
                    static_cast<char *>(optval), &lenreal) != 0 )
        return false;

    *optlen = lenreal;

    return true;
}

bool
wxSocketBase::SetOption(int level, int optname, const void *optval, int optlen)
{
    wxASSERT_MSG( m_impl, _T("Socket not initialised") );

    return setsockopt(m_impl->m_fd, level, optname,
                      static_cast<const char *>(optval), optlen) == 0;
}

bool wxSocketBase::SetLocal(const wxIPV4address& local)
{
    m_localAddress = local;

    return true;
}

// ==========================================================================
// wxSocketClient
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor and dtor
// --------------------------------------------------------------------------

wxSocketClient::wxSocketClient(wxSocketFlags flags)
              : wxSocketBase(flags, wxSOCKET_CLIENT)
{
    m_initialRecvBufferSize =
    m_initialSendBufferSize = -1;
}

wxSocketClient::~wxSocketClient()
{
}

// --------------------------------------------------------------------------
// Connect
// --------------------------------------------------------------------------

bool wxSocketClient::DoConnect(const wxSockAddress& remote,
                               const wxSockAddress* local,
                               bool wait)
{
    if ( m_impl )
    {
        // Shutdown and destroy the old socket
        Close();
        delete m_impl;
    }

    m_connected = false;
    m_establishing = false;

    // Create and set up the new one
    m_impl = wxSocketImpl::Create(*this);
    if ( !m_impl )
        return false;

    // Reuse makes sense for clients too, if we are trying to rebind to the same port
    if (GetFlags() & wxSOCKET_REUSEADDR)
        m_impl->SetReusable();
    if (GetFlags() & wxSOCKET_BROADCAST)
        m_impl->SetBroadcast();
    if (GetFlags() & wxSOCKET_NOBIND)
        m_impl->DontDoBind();

    // Bind to the local IP address and port, when provided or if one had been
    // set before
    if ( !local && m_localAddress.GetAddress().IsOk() )
        local = &m_localAddress;

    if ( local )
        m_impl->SetLocal(local->GetAddress());

    m_impl->SetInitialSocketBuffers(m_initialRecvBufferSize, m_initialSendBufferSize);

    m_impl->SetPeer(remote.GetAddress());

    // Finally do create the socket and connect to the peer
    const wxSocketError err = m_impl->CreateClient(wait);

    if ( err != wxSOCKET_NOERROR )
    {
        if ( err == wxSOCKET_WOULDBLOCK )
        {
            wxASSERT_MSG( !wait, "shouldn't get this for blocking connect" );

            m_establishing = true;
        }

        return false;
    }

    m_connected = true;
    return true;
}

bool wxSocketClient::Connect(const wxSockAddress& remote, bool wait)
{
    return DoConnect(remote, NULL, wait);
}

bool wxSocketClient::Connect(const wxSockAddress& remote,
                             const wxSockAddress& local,
                             bool wait)
{
    return DoConnect(remote, &local, wait);
}

bool wxSocketClient::WaitOnConnect(long seconds, long milliseconds)
{
    if ( m_connected )
    {
        // this happens if the initial attempt to connect succeeded without
        // blocking
        return true;
    }

    wxCHECK_MSG( m_establishing && m_impl, false,
                 "No connection establishment attempt in progress" );

    // we must specify wxSOCKET_LOST_FLAG here explicitly because we must return
    // true if the connection establishment process is finished, whether it is
    // over because we successfully connected or because we were not able to
    // connect
    return DoWait(seconds, milliseconds,
        wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
}

// ==========================================================================
// wxDatagramSocket
// ==========================================================================

wxDatagramSocket::wxDatagramSocket( const wxSockAddress& addr,
                                    wxSocketFlags flags )
                : wxSocketBase( flags, wxSOCKET_DATAGRAM )
{
    // Create the socket
    m_impl = wxSocketImpl::Create(*this);

    if (!m_impl)
        return;

    // Setup the socket as non connection oriented
    m_impl->SetLocal(addr.GetAddress());
    if (flags & wxSOCKET_REUSEADDR)
    {
        m_impl->SetReusable();
    }
    if (GetFlags() & wxSOCKET_BROADCAST)
    {
        m_impl->SetBroadcast();
    }
    if (GetFlags() & wxSOCKET_NOBIND)
    {
        m_impl->DontDoBind();
    }

    if ( m_impl->CreateUDP() != wxSOCKET_NOERROR )
    {
        delete m_impl;
        m_impl = NULL;
        return;
    }

    // Initialize all stuff
    m_connected = false;
    m_establishing = false;
}

wxDatagramSocket& wxDatagramSocket::RecvFrom( wxSockAddress& addr,
                                              void* buf,
                                              wxUint32 nBytes )
{
    Read(buf, nBytes);
    GetPeer(addr);
    return (*this);
}

wxDatagramSocket& wxDatagramSocket::SendTo( const wxSockAddress& addr,
                                            const void* buf,
                                            wxUint32 nBytes )
{
    wxASSERT_MSG( m_impl, _T("Socket not initialised") );

    m_impl->SetPeer(addr.GetAddress());
    Write(buf, nBytes);
    return (*this);
}

// ==========================================================================
// wxSocketModule
// ==========================================================================

class wxSocketModule : public wxModule
{
public:
    virtual bool OnInit()
    {
        // wxSocketBase will call Initialize() itself only if sockets are
        // really used, don't do it from here
        return true;
    }

    virtual void OnExit()
    {
        if ( wxSocketBase::IsInitialized() )
            wxSocketBase::Shutdown();
    }

private:
    DECLARE_DYNAMIC_CLASS(wxSocketModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)

#endif // wxUSE_SOCKETS
