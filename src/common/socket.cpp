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

// Conditionally make the socket non-blocking for the lifetime of this object.
class wxSocketUnblocker
{
public:
    wxSocketUnblocker(wxSocketImpl *socket, bool unblock = true)
        : m_impl(socket),
          m_unblock(unblock)
    {
        if ( m_unblock )
            m_impl->SetNonBlocking(true);
    }

    ~wxSocketUnblocker()
    {
        if ( m_unblock )
            m_impl->SetNonBlocking(false);
    }

private:
    wxSocketImpl * const m_impl;
    bool m_unblock;

    DECLARE_NO_COPY_CLASS(wxSocketUnblocker)
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
    m_detected        = 0;
    m_local           = NULL;
    m_peer            = NULL;
    m_error           = wxSOCKET_NOERROR;
    m_server          = false;
    m_stream          = true;
    m_non_blocking    = false;

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
    if (m_fd != INVALID_SOCKET)
        Shutdown();

    if (m_local)
        GAddress_destroy(m_local);

    if (m_peer)
        GAddress_destroy(m_peer);
}

bool wxSocketImpl::PreCreateCheck(GAddress *addr)
{
    if ( m_fd != INVALID_SOCKET )
    {
        m_error = wxSOCKET_INVSOCK;
        return false;
    }

    if ( !addr || !addr->m_addr )
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

    // FIXME: shouldn't we check for m_non_blocking here? as it is now, all our
    //        sockets are non-blocking
    UnblockAndRegisterWithEventLoop();
}

wxSocketError wxSocketImpl::UpdateLocalAddress()
{
    WX_SOCKLEN_T lenAddr;
    if ( getsockname(m_fd, m_local->m_addr, &lenAddr) != 0 )
    {
        Close();
        m_error = wxSOCKET_IOERR;
        return m_error;
    }

    m_local->m_len = lenAddr;

    return wxSOCKET_NOERROR;
}

wxSocketError wxSocketImpl::CreateServer()
{
    if ( !PreCreateCheck(m_local) )
        return m_error;

    m_server = true;
    m_stream = true;

    // do create the socket
    m_fd = socket(m_local->m_realfamily, SOCK_STREAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    PostCreation();

    // and then bind to and listen on it
    //
    // FIXME: should we test for m_dobind here?
    if ( bind(m_fd, m_local->m_addr, m_local->m_len) != 0 )
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

wxSocketError wxSocketImpl::CreateClient()
{
    if ( !PreCreateCheck(m_peer) )
        return m_error;

    m_fd = socket(m_peer->m_realfamily, SOCK_STREAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
      m_error = wxSOCKET_IOERR;
      return wxSOCKET_IOERR;
    }

    PostCreation();

    // If a local address has been set, then bind to it before calling connect
    if ( m_local && m_local->m_addr )
    {
        if ( bind(m_fd, m_local->m_addr, m_local->m_len) != 0 )
        {
            Close();
            m_error = wxSOCKET_IOERR;
            return m_error;
        }
    }

    // Connect to the peer and handle the EWOULDBLOCK return value in
    // platform-specific code
    return DoHandleConnect(connect(m_fd, m_peer->m_addr, m_peer->m_len));
}


wxSocketError wxSocketImpl::CreateUDP()
{
    if ( !PreCreateCheck(m_local) )
        return m_error;

    m_stream = false;
    m_server = false;

    m_fd = socket(m_local->m_realfamily, SOCK_DGRAM, 0);

    if ( m_fd == INVALID_SOCKET )
    {
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    PostCreation();

    if ( m_dobind )
    {
        if ( bind(m_fd, m_local->m_addr, m_local->m_len) != 0 )
        {
            Close();
            m_error = wxSOCKET_IOERR;
            return m_error;
        }

        return UpdateLocalAddress();
    }

    return wxSOCKET_NOERROR;
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

    m_detected = wxSOCKET_LOST_FLAG;
}

/*
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void wxSocketImpl::SetTimeout(unsigned long millis)
{
    m_timeout.tv_sec  = (millis / 1000);
    m_timeout.tv_usec = (millis % 1000) * 1000;
}

void wxSocketImpl::NotifyOnStateChange(wxSocketNotify event)
{
    m_wxsocket->OnRequest(event);
}

/* Address handling */

/*
 *  Set or get the local or peer address for this socket. The 'set'
 *  functions return wxSOCKET_NOERROR on success, an error code otherwise.
 *  The 'get' functions return a pointer to a GAddress object on success,
 *  or NULL otherwise, in which case they set the error code of the
 *  corresponding socket.
 *
 *  Error codes:
 *    wxSOCKET_INVSOCK - the socket is not valid.
 *    wxSOCKET_INVADDR - the address is not valid.
 */
wxSocketError wxSocketImpl::SetLocal(GAddress *address)
{
  /* the socket must be initialized, or it must be a server */
  if (m_fd != INVALID_SOCKET && !m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return wxSOCKET_INVSOCK;
  }

  /* check address */
  if (address == NULL || address->m_family == wxSOCKET_NOFAMILY)
  {
    m_error = wxSOCKET_INVADDR;
    return wxSOCKET_INVADDR;
  }

  if (m_local)
    GAddress_destroy(m_local);

  m_local = GAddress_copy(address);

  return wxSOCKET_NOERROR;
}

wxSocketError wxSocketImpl::SetPeer(GAddress *address)
{
  /* check address */
  if (address == NULL || address->m_family == wxSOCKET_NOFAMILY)
  {
    m_error = wxSOCKET_INVADDR;
    return wxSOCKET_INVADDR;
  }

  if (m_peer)
    GAddress_destroy(m_peer);

  m_peer = GAddress_copy(address);

  return wxSOCKET_NOERROR;
}

GAddress *wxSocketImpl::GetLocal()
{
  GAddress *address;
  wxSockAddr addr;
  WX_SOCKLEN_T size = sizeof(addr);
  wxSocketError err;

  /* try to get it from the m_local var first */
  if (m_local)
    return GAddress_copy(m_local);

  /* else, if the socket is initialized, try getsockname */
  if (m_fd == INVALID_SOCKET)
  {
    m_error = wxSOCKET_INVSOCK;
    return NULL;
  }

  if (getsockname(m_fd, (sockaddr*)&addr, &size) == SOCKET_ERROR)
  {
    m_error = wxSOCKET_IOERR;
    return NULL;
  }

  /* got a valid address from getsockname, create a GAddress object */
  if ((address = GAddress_new()) == NULL)
  {
     m_error = wxSOCKET_MEMERR;
     return NULL;
  }

  if ((err = _GAddress_translate_from(address, (sockaddr*)&addr, size)) != wxSOCKET_NOERROR)
  {
     GAddress_destroy(address);
     m_error = err;
     return NULL;
  }

  return address;
}

GAddress *wxSocketImpl::GetPeer()
{
  /* try to get it from the m_peer var */
  if (m_peer)
    return GAddress_copy(m_peer);

  return NULL;
}

bool wxSocketImpl::DoBlockWithTimeout(wxSocketEventFlags flags)
{
    if ( !m_non_blocking )
    {
        fd_set fds;
        wxFD_ZERO(&fds);
        wxFD_SET(m_fd, &fds);

        fd_set
            *readfds = flags & wxSOCKET_INPUT_FLAG ? &fds : NULL,
            *writefds = flags & wxSOCKET_OUTPUT_FLAG ? &fds : NULL;

        // make a copy as it can be modified by select()
        struct timeval tv = m_timeout;
        int ret = select(m_fd + 1, readfds, writefds, NULL, &tv);

        switch ( ret )
        {
            case 0:
                m_error = wxSOCKET_TIMEDOUT;
                return false;

            case -1:
                m_error = wxSOCKET_IOERR;
                return false;
        }
    }
    //else: we're non-blocking, never block

    return true;
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
    m_error        =
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
    m_eventmask    = 0;

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

    // Supress events from now on
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
// simply accessors
// ----------------------------------------------------------------------------

wxSocketError wxSocketBase::LastError() const
{
    return m_impl->GetError();
}

// --------------------------------------------------------------------------
// Basic IO calls
// --------------------------------------------------------------------------

// The following IO operations update m_error and m_lcount:
// {Read, Write, ReadMsg, WriteMsg, Peek, Unread, Discard}
//
// TODO: Should Connect, Accept and AcceptWith update m_error?

bool wxSocketBase::Close()
{
    // Interrupt pending waits
    InterruptWait();

    if (m_impl)
        m_impl->Shutdown();

    m_connected = false;
    m_establishing = false;
    return true;
}

wxSocketBase& wxSocketBase::Read(void* buffer, wxUint32 nbytes)
{
    // Mask read events
    m_reading = true;

    m_lcount = DoRead(buffer, nbytes);

    // If in wxSOCKET_WAITALL mode, all bytes should have been read.
    if (m_flags & wxSOCKET_WAITALL)
        m_error = (m_lcount != nbytes);
    else
        m_error = (m_lcount == 0);

    // Allow read events from now on
    m_reading = false;

    return *this;
}

wxUint32 wxSocketBase::DoRead(void* buffer_, wxUint32 nbytes)
{
    // We use pointer arithmetic here which doesn't work with void pointers.
    char *buffer = static_cast<char *>(buffer_);

    // Try the push back buffer first, even before checking whether the socket
    // is valid to allow reading previously pushed back data from an already
    // closed socket.
    wxUint32 total = GetPushback(buffer, nbytes, false);
    nbytes -= total;
    buffer += total;

    // If it's indeed closed or if read everything, there is nothing more to do.
    if ( !m_impl || !nbytes )
        return total;

    wxCHECK_MSG( buffer, 0, "NULL buffer" );


    // wxSOCKET_NOWAIT overrides all the other flags and means that we are
    // polling the socket and don't block at all.
    if ( m_flags & wxSOCKET_NOWAIT )
    {
        wxSocketUnblocker unblock(m_impl);
        int ret = m_impl->Read(buffer, nbytes);
        if ( ret < 0 )
            return 0;

        total += ret;
    }
    else // blocking socket
    {
        for ( ;; )
        {
            // Wait until socket becomes ready for reading dispatching the GUI
            // events in the meanwhile unless wxSOCKET_BLOCK was explicitly
            // specified to disable this.
            if ( !(m_flags & wxSOCKET_BLOCK) && !WaitForRead() )
                break;

            const int ret = m_impl->Read(buffer, nbytes);
            if ( ret == 0 )
            {
                // for connection-oriented (e.g. TCP) sockets we can only read
                // 0 bytes if the other end has been closed, and for
                // connectionless ones (UDP) this flag doesn't make sense
                // anyhow so we can set it to true too without doing any harm
                m_closed = true;
                break;
            }

            if ( ret < 0 )
            {
                // this will be always interpreted as error by Read()
                return 0;
            }

            total += ret;

            // If wxSOCKET_WAITALL is not set, we can leave now as we did read
            // something and we don't need to wait for all nbytes bytes to be
            // read.
            if ( !(m_flags & wxSOCKET_WAITALL) )
                break;

            // Otherwise continue reading until we do read everything.
            nbytes -= ret;
            if ( !nbytes )
                break;

            buffer += ret;
        }
    }

    return total;
}

wxSocketBase& wxSocketBase::ReadMsg(void* buffer, wxUint32 nbytes)
{
    wxUint32 len, len2, sig, total;
    bool error;
    int old_flags;
    struct
    {
        unsigned char sig[4];
        unsigned char len[4];
    } msg;

    // Mask read events
    m_reading = true;

    total = 0;
    error = true;
    old_flags = m_flags;
    SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

    if (DoRead(&msg, sizeof(msg)) != sizeof(msg))
        goto exit;

    sig = (wxUint32)msg.sig[0];
    sig |= (wxUint32)(msg.sig[1] << 8);
    sig |= (wxUint32)(msg.sig[2] << 16);
    sig |= (wxUint32)(msg.sig[3] << 24);

    if (sig != 0xfeeddead)
    {
        wxLogWarning(_("wxSocket: invalid signature in ReadMsg."));
        goto exit;
    }

    len = (wxUint32)msg.len[0];
    len |= (wxUint32)(msg.len[1] << 8);
    len |= (wxUint32)(msg.len[2] << 16);
    len |= (wxUint32)(msg.len[3] << 24);

    if (len > nbytes)
    {
        len2 = len - nbytes;
        len = nbytes;
    }
    else
        len2 = 0;

    // Don't attempt to read if the msg was zero bytes long.
    if (len)
    {
        total = DoRead(buffer, len);

        if (total != len)
            goto exit;
    }

    if (len2)
    {
        char *discard_buffer = new char[MAX_DISCARD_SIZE];
        long discard_len;

        // NOTE: discarded bytes don't add to m_lcount.
        do
        {
            discard_len = ((len2 > MAX_DISCARD_SIZE)? MAX_DISCARD_SIZE : len2);
            discard_len = DoRead(discard_buffer, (wxUint32)discard_len);
            len2 -= (wxUint32)discard_len;
        }
        while ((discard_len > 0) && len2);

        delete [] discard_buffer;

        if (len2 != 0)
            goto exit;
    }
    if (DoRead(&msg, sizeof(msg)) != sizeof(msg))
        goto exit;

    sig = (wxUint32)msg.sig[0];
    sig |= (wxUint32)(msg.sig[1] << 8);
    sig |= (wxUint32)(msg.sig[2] << 16);
    sig |= (wxUint32)(msg.sig[3] << 24);

    if (sig != 0xdeadfeed)
    {
        wxLogWarning(_("wxSocket: invalid signature in ReadMsg."));
        goto exit;
    }

    // everything was OK
    error = false;

exit:
    m_error = error;
    m_lcount = total;
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

    // If in wxSOCKET_WAITALL mode, all bytes should have been read.
    if (m_flags & wxSOCKET_WAITALL)
        m_error = (m_lcount != nbytes);
    else
        m_error = (m_lcount == 0);

    // Allow read events again
    m_reading = false;

    return *this;
}

wxSocketBase& wxSocketBase::Write(const void *buffer, wxUint32 nbytes)
{
    // Mask write events
    m_writing = true;

    m_lcount = DoWrite(buffer, nbytes);

    // If in wxSOCKET_WAITALL mode, all bytes should have been written.
    if (m_flags & wxSOCKET_WAITALL)
        m_error = (m_lcount != nbytes);
    else
        m_error = (m_lcount == 0);

    // Allow write events again
    m_writing = false;

    return *this;
}

// This function is a mirror image of DoRead() except that it doesn't use the
// push back buffer, please see comments there
wxUint32 wxSocketBase::DoWrite(const void *buffer_, wxUint32 nbytes)
{
    const char *buffer = static_cast<const char *>(buffer_);

    // Return if there is nothing to read or the socket is (already?) closed.
    if ( !m_impl || !nbytes )
        return 0;

    wxCHECK_MSG( buffer, 0, "NULL buffer" );

    wxUint32 total = 0;
    if ( m_flags & wxSOCKET_NOWAIT )
    {
        wxSocketUnblocker unblock(m_impl);
        const int ret = m_impl->Write(buffer, nbytes);
        if ( ret > 0 )
            total += ret;
    }
    else // blocking socket
    {
        for ( ;; )
        {
            if ( !(m_flags & wxSOCKET_BLOCK) && !WaitForWrite() )
                break;

            const int ret = m_impl->Write(buffer, nbytes);
            if ( ret == 0 )
            {
                m_closed = true;
                break;
            }

            if ( ret < 0 )
                return 0;

            total += ret;
            if ( !(m_flags & wxSOCKET_WAITALL) )
                break;

            nbytes -= ret;
            if ( !nbytes )
                break;

            buffer += ret;
        }
    }

    return total;
}

wxSocketBase& wxSocketBase::WriteMsg(const void *buffer, wxUint32 nbytes)
{
    wxUint32 total;
    bool error;
    struct
    {
        unsigned char sig[4];
        unsigned char len[4];
    } msg;

    // Mask write events
    m_writing = true;

    error = true;
    total = 0;
    SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

    msg.sig[0] = (unsigned char) 0xad;
    msg.sig[1] = (unsigned char) 0xde;
    msg.sig[2] = (unsigned char) 0xed;
    msg.sig[3] = (unsigned char) 0xfe;

    msg.len[0] = (unsigned char) (nbytes & 0xff);
    msg.len[1] = (unsigned char) ((nbytes >> 8) & 0xff);
    msg.len[2] = (unsigned char) ((nbytes >> 16) & 0xff);
    msg.len[3] = (unsigned char) ((nbytes >> 24) & 0xff);

    if (DoWrite(&msg, sizeof(msg)) < sizeof(msg))
        goto exit;

    total = DoWrite(buffer, nbytes);

    if (total < nbytes)
        goto exit;

    msg.sig[0] = (unsigned char) 0xed;
    msg.sig[1] = (unsigned char) 0xfe;
    msg.sig[2] = (unsigned char) 0xad;
    msg.sig[3] = (unsigned char) 0xde;
    msg.len[0] =
    msg.len[1] =
    msg.len[2] =
    msg.len[3] = (char) 0;

    if ((DoWrite(&msg, sizeof(msg))) < sizeof(msg))
        goto exit;

    // everything was OK
    error = false;

exit:
    m_error = error;
    m_lcount = total;
    m_writing = false;

    return *this;
}

wxSocketBase& wxSocketBase::Unread(const void *buffer, wxUint32 nbytes)
{
    if (nbytes != 0)
        Pushback(buffer, nbytes);

    m_error = false;
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

    SetFlags(wxSOCKET_NOWAIT);

    do
    {
        ret = DoRead(buffer, MAX_DISCARD_SIZE);
        total += ret;
    }
    while (ret == MAX_DISCARD_SIZE);

    delete[] buffer;
    m_lcount = total;
    m_error  = false;

    // Allow read events again
    m_reading = false;

    return *this;
}

// --------------------------------------------------------------------------
// Wait functions
// --------------------------------------------------------------------------

/*
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking | nonblocking) of the socket.
 */
wxSocketEventFlags wxSocketImpl::Select(wxSocketEventFlags flags)
{
  assert(this);

  wxSocketEventFlags result = 0;
  fd_set readfds;
  fd_set writefds;
  fd_set exceptfds;
  struct timeval tv;

  if (m_fd == -1)
    return (wxSOCKET_LOST_FLAG & flags);

  /* Do not use a static struct, Linux can garble it */
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  wxFD_ZERO(&readfds);
  wxFD_ZERO(&writefds);
  wxFD_ZERO(&exceptfds);
  wxFD_SET(m_fd, &readfds);
  if (flags & wxSOCKET_OUTPUT_FLAG || flags & wxSOCKET_CONNECTION_FLAG)
    wxFD_SET(m_fd, &writefds);
  wxFD_SET(m_fd, &exceptfds);

  /* Check 'sticky' CONNECTION flag first */
  result |= wxSOCKET_CONNECTION_FLAG & m_detected;

  /* If we have already detected a LOST event, then don't try
   * to do any further processing.
   */
  if ((m_detected & wxSOCKET_LOST_FLAG) != 0)
  {
    m_establishing = false;
    return (wxSOCKET_LOST_FLAG & flags);
  }

  /* Try select now */
  if (select(m_fd + 1, &readfds, &writefds, &exceptfds, &tv) < 0)
  {
    /* What to do here? */
    return (result & flags);
  }

  /* Check for exceptions and errors */
  if (wxFD_ISSET(m_fd, &exceptfds))
  {
    m_establishing = false;
    m_detected = wxSOCKET_LOST_FLAG;

    /* LOST event: Abort any further processing */
    return (wxSOCKET_LOST_FLAG & flags);
  }

  /* Check for readability */
  if (wxFD_ISSET(m_fd, &readfds))
  {
    result |= wxSOCKET_INPUT_FLAG;

    if (m_server && m_stream)
    {
      /* This is a TCP server socket that detected a connection.
         While the INPUT_FLAG is also set, it doesn't matter on
         this kind of  sockets, as we can only Accept() from them. */
      m_detected |= wxSOCKET_CONNECTION_FLAG;
    }
  }

  /* Check for writability */
  if (wxFD_ISSET(m_fd, &writefds))
  {
    if (m_establishing && !m_server)
    {
      int error;
      SOCKOPTLEN_T len = sizeof(error);
      m_establishing = false;
      getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

      if (error)
      {
        m_detected = wxSOCKET_LOST_FLAG;

        /* LOST event: Abort any further processing */
        return (wxSOCKET_LOST_FLAG & flags);
      }
      else
      {
        m_detected |= wxSOCKET_CONNECTION_FLAG;
      }
    }
    else
    {
      result |= wxSOCKET_OUTPUT_FLAG;
    }
  }

  return (result | m_detected) & flags;
}

// All Wait functions poll the socket using Select() to
// check for the specified combination of conditions, until one
// of these conditions become true, an error occurs, or the
// timeout elapses. The polling loop runs the event loop so that
// this won't block the GUI.

bool
wxSocketBase::DoWait(long seconds, long milliseconds, wxSocketEventFlags flags)
{
    wxCHECK_MSG( m_impl, false, "can't wait on invalid socket" );

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
    // when running in the main thread
    wxEventLoopBase *eventLoop;
    if ( wxIsMainThread() )
    {
        eventLoop = wxEventLoop::GetActive();
    }
    else // in worker thread
    {
        // We never dispatch messages from threads other than the main one.
        eventLoop = NULL;
    }

    // Wait in an active polling loop: notice that the loop is executed at
    // least once, even if timeout is 0 (i.e. polling).
    bool gotEvent = false;
    for ( ;; )
    {
        // We always stop waiting when the connection is lost as it doesn't
        // make sense to continue further, even if wxSOCKET_LOST_FLAG is not
        // specified in flags to wait for.
        const wxSocketEventFlags
            result = m_impl->Select(flags | wxSOCKET_LOST_FLAG);

        // Incoming connection (server) or connection established (client)?
        if ( result & wxSOCKET_CONNECTION_FLAG )
        {
            m_connected = true;
            m_establishing = false;
            gotEvent = true;
            break;
        }

        // Data available or output buffer ready?
        if ( (result & wxSOCKET_INPUT_FLAG) || (result & wxSOCKET_OUTPUT_FLAG) )
        {
            gotEvent = true;
            break;
        }

        // Connection lost
        if ( result & wxSOCKET_LOST_FLAG )
        {
            m_connected = false;
            m_establishing = false;
            if ( flags & wxSOCKET_LOST_FLAG )
                gotEvent = true;
            break;
        }

        if ( m_interrupt )
            break;

        // Wait more?
        const wxMilliClock_t timeNow = wxGetLocalTimeMillis();
        if ( timeNow >= timeEnd )
            break;

        if ( eventLoop )
        {
            // This function is only called if wxSOCKET_BLOCK flag was not used
            // and so we should dispatch the events if there is an event loop
            // capable of doing it.
            if ( eventLoop->Pending() )
                eventLoop->Dispatch();
        }
#if wxUSE_THREADS
        else // no event loop or waiting in another thread
        {
            // We're busy waiting but at least give up the rest of our current
            // time slice.
            wxThread::Yield();
        }
#endif // wxUSE_THREADS
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

bool wxSocketBase::GetPeer(wxSockAddress& addr_man) const
{
    GAddress *peer;

    if (!m_impl)
        return false;

    peer = m_impl->GetPeer();

    // copying a null address would just trigger an assert anyway

    if (!peer)
        return false;

    addr_man.SetAddress(peer);
    GAddress_destroy(peer);

    return true;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr_man) const
{
    GAddress *local;

    if (!m_impl)
        return false;

    local = m_impl->GetLocal();
    addr_man.SetAddress(local);
    GAddress_destroy(local);

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
    switch(notification)
    {
        case wxSOCKET_CONNECTION:
            m_establishing = false;
            m_connected = true;
            break;

            // If we are in the middle of a R/W operation, do not
            // propagate events to users. Also, filter 'late' events
            // which are no longer valid.

        case wxSOCKET_INPUT:
            if (m_reading || !m_impl->Select(wxSOCKET_INPUT_FLAG))
                return;
            break;

        case wxSOCKET_OUTPUT:
            if (m_writing || !m_impl->Select(wxSOCKET_OUTPUT_FLAG))
                return;
            break;

        case wxSOCKET_LOST:
            m_connected = false;
            m_establishing = false;
            break;

        case wxSOCKET_MAX_EVENT:
            wxFAIL_MSG( "unexpected notification" );
            return;
    }

    // Schedule the event

    wxSocketEventFlags flag = 0;
    wxUnusedVar(flag);
    switch (notification)
    {
        case wxSOCKET_INPUT:      flag = wxSOCKET_INPUT_FLAG; break;
        case wxSOCKET_OUTPUT:     flag = wxSOCKET_OUTPUT_FLAG; break;
        case wxSOCKET_CONNECTION: flag = wxSOCKET_CONNECTION_FLAG; break;
        case wxSOCKET_LOST:       flag = wxSOCKET_LOST_FLAG; break;
        default:
                               wxLogWarning(_("wxSocket: unknown event!."));
                               return;
    }

    if (((m_eventmask & flag) == flag) && m_notify)
    {
        if (m_handler)
        {
            wxSocketEvent event(m_id);
            event.m_event      = notification;
            event.m_clientData = m_clientData;
            event.SetEventObject(this);

            m_handler->AddPendingEvent(event);
        }
    }
}

void wxSocketBase::Notify(bool notify)
{
    m_notify = notify;
    if (m_impl)
        m_impl->Notify(notify);
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

wxSocketServer::wxSocketServer(const wxSockAddress& addr_man,
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
    m_impl->Notify(m_notify);
    m_impl->SetLocal(addr_man.GetAddress());

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
    if (!m_impl)
        return false;

    // If wait == false, then the call should be nonblocking.
    // When we are finished, we put the socket to blocking mode
    // again.
    wxSocketUnblocker unblock(m_impl, !wait);
    sock.m_impl = m_impl->WaitConnection(sock);

    if ( !sock.m_impl )
        return false;

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

    SOCKOPTLEN_T lenreal;
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
    GAddress* la = local.GetAddress();

    // If the address is valid, save it for use when we call Connect
    if (la && la->m_addr)
    {
        m_localAddress = local;

        return true;
    }

    return false;
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

bool wxSocketClient::DoConnect(const wxSockAddress& addr_man,
                               const wxSockAddress* local,
                               bool wait)
{
    if (m_impl)
    {
        // Shutdown and destroy the socket
        Close();
        delete m_impl;
    }

    m_impl = wxSocketImpl::Create(*this);
    m_connected = false;
    m_establishing = false;

    if (!m_impl)
        return false;

    // If wait == false, then the call should be nonblocking. When we are
    // finished, we put the socket to blocking mode again.
    wxSocketUnblocker unblock(m_impl, !wait);

    // Reuse makes sense for clients too, if we are trying to rebind to the same port
    if (GetFlags() & wxSOCKET_REUSEADDR)
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

    // If no local address was passed and one has been set, use the one that was Set
    if (!local && m_localAddress.GetAddress())
    {
        local = &m_localAddress;
    }

    // Bind to the local IP address and port, when provided
    if (local)
    {
        GAddress* la = local->GetAddress();

        if (la && la->m_addr)
            m_impl->SetLocal(la);
    }

    m_impl->SetInitialSocketBuffers(m_initialRecvBufferSize, m_initialSendBufferSize);

    m_impl->SetPeer(addr_man.GetAddress());
    const wxSocketError err = m_impl->CreateClient();

    //this will register for callbacks - must be called after m_impl->m_fd was initialized
    m_impl->Notify(m_notify);

    if (err != wxSOCKET_NOERROR)
    {
        if (err == wxSOCKET_WOULDBLOCK)
            m_establishing = true;

        return false;
    }

    m_connected = true;
    return true;
}

bool wxSocketClient::Connect(const wxSockAddress& addr_man, bool wait)
{
    return DoConnect(addr_man, NULL, wait);
}

bool wxSocketClient::Connect(const wxSockAddress& addr_man,
                             const wxSockAddress& local,
                             bool wait)
{
    return DoConnect(addr_man, &local, wait);
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

    m_impl->Notify(m_notify);
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
