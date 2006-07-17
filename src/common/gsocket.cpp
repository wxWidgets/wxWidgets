/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        gsocket.cpp
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Authors:     David Elliott (C++ conversion, maintainer)
 *              Guilhem Lavaux,
 *              Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose:     GSocket Generic File
 * Licence:     The wxWindows licence
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#if defined(__WATCOMC__)
#include "wx/wxprec.h"
#include <errno.h>
#include <nerrno.h>
#endif

#include "wx/defs.h"

#if defined(__VISAGECPP__)
#define BSD_SELECT /* use Berkeley Sockets select */
#endif

#if wxUSE_SOCKETS

#include <assert.h>
#include <sys/types.h>
#ifdef __VISAGECPP__
#include <string.h>
#include <sys/time.h>
#include <types.h>
#include <netinet/in.h>
#endif

#ifndef __WINDOWS__
#include <netdb.h>
#include <sys/ioctl.h>
#endif

#ifdef __VMS__
#include <socket.h>
struct sockaddr_un
{
    u_char  sun_len;        /* sockaddr len including null */
    u_char  sun_family;     /* AF_UNIX */
    char    sun_path[108];  /* path name (gag) */
};
#else
#ifndef __WINDOWS__
#include <sys/socket.h>
#include <sys/un.h>
#endif
#endif

#ifndef __VISAGECPP__
#include <sys/time.h>
#ifndef __WINDOWS__
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <string.h>
#include <unistd.h>
#else
#include <nerrno.h>
#  if __IBMCPP__ < 400
#include <machine/endian.h>
#include <socket.h>
#include <ioctl.h>
#include <select.h>
#include <unistd.h>

#define EBADF   SOCEBADF

#    ifdef min
#    undef min
#    endif
#  else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#define close(a) soclose(a)
#define select(a,b,c,d,e) bsdselect(a,b,c,d,e)
int _System bsdselect(int,
                      struct fd_set *,
                      struct fd_set *,
                      struct fd_set *,
                      struct timeval *);
int _System soclose(int);
#  endif
#endif
#ifdef __EMX__
#include <sys/select.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#ifdef sun
#  include <sys/filio.h>
#endif
#ifdef sgi
#  include <bstring.h>
#endif
#ifdef _AIX
#  include <strings.h>
#endif
#include <signal.h>

#ifndef WX_SOCKLEN_T

#ifdef VMS
#  define WX_SOCKLEN_T unsigned int
#else
#  ifdef __GLIBC__
#    if __GLIBC__ == 2
#      define WX_SOCKLEN_T socklen_t
#    endif
#  elif defined(__WXMAC__)
#    define WX_SOCKLEN_T socklen_t
#  else
#    define WX_SOCKLEN_T int
#  endif
#endif

#endif /* SOCKLEN_T */

#ifndef SOCKOPTLEN_T
#define SOCKOPTLEN_T WX_SOCKLEN_T
#endif

/*
 * MSW defines this, Unices don't.
 */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifdef __WINDOWS__
#define CLOSE_SOCKET closesocket
#define IOCTL_SOCKET ioctlsocket
#define IOCTL_TYPE u_long
/* This should never, ever happen in windows as the only call that can cause it has been removed on winsock2 */
#define IS_INTERRUPTED (WSAGetLastError() == WSAEINTR)
//#define IN_PROGRESS ((WSAGetLastError() == WSAEWOULDBLOCK) || (WSAGetLastError() == WSAEAGAIN) || (WSAGetLastError() == WSAEINPROGRESS))
#define IN_PROGRESS ((WSAGetLastError() == WSAEWOULDBLOCK) || (WSAGetLastError() == WSAEINPROGRESS))
#define HAVE_GETHOSTBYNAME
#else
#define CLOSE_SOCKET close
#define IOCTL_SOCKET ioctl
#define IOCTL_TYPE int
#define IS_INTERRUPTED (errno == EINTR)
#define IN_PROGRESS ((errno == EINPROGRESS) || (errno == EWOULDBLOCK) || (errno == EAGAIN))
#endif

/* UnixWare reportedly needs this for FIONBIO definition */
#ifdef __UNIXWARE__
#include <sys/filio.h>
#endif

/*
 * INADDR_BROADCAST is identical to INADDR_NONE which is not defined
 * on all systems. INADDR_BROADCAST should be fine to indicate an error.
 */
#ifndef INADDR_NONE
#define INADDR_NONE INADDR_BROADCAST
#endif

#if defined(__VISAGECPP__) || defined(__WATCOMC__) || defined(__WINDOWS__)

    #define MASK_SIGNAL() {
    #define UNMASK_SIGNAL() }

#else
    extern "C" { typedef void (*wxSigHandler)(int); }

    #define MASK_SIGNAL()                       \
    {                                           \
        wxSigHandler old_handler = signal(SIGPIPE, SIG_IGN);

    #define UNMASK_SIGNAL()                     \
        signal(SIGPIPE, old_handler);           \
    }

#endif

/* If a SIGPIPE is issued by a socket call on a remotely closed socket,
   the program will "crash" unless it explicitly handles the SIGPIPE.
   By using MSG_NOSIGNAL, the SIGPIPE is suppressed. Later, we will
   use SO_NOSIGPIPE (if available), the BSD equivalent. */
#ifdef MSG_NOSIGNAL
#  define GSOCKET_MSG_NOSIGNAL MSG_NOSIGNAL
#else /* MSG_NOSIGNAL not available (FreeBSD including OS X), MSW */
#  define GSOCKET_MSG_NOSIGNAL 0
#endif /* MSG_NOSIGNAL */

#include "wx/gsocket.h"
#include "wx/gaddr.h"
#include "wx/unix/private.h"
#if wxUSE_THREADS && (defined(HAVE_GETHOSTBYNAME) || defined(HAVE_GETSERVBYNAME))
#  include "wx/thread.h"
#endif

/* Table of GUI-related functions. We must call them indirectly because
 * of wxBase and GUI separation: */

static GSocketGUIFunctionsTable *gs_gui_functions;

class GSocketGUIFunctionsTableNull: public GSocketGUIFunctionsTable
{
public:
    virtual bool OnInit();
    virtual void OnExit();
    virtual bool CanUseEventLoop();
    virtual bool Init_Socket(GSocket *socket);
    virtual void Destroy_Socket(GSocket *socket);
    virtual void Install_Callback(GSocket *socket, GSocketEvent event);
    virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event);
    virtual void Enable_Events(GSocket *socket);
    virtual void Disable_Events(GSocket *socket);
};

bool GSocketGUIFunctionsTableNull::OnInit()
{   return true; }
void GSocketGUIFunctionsTableNull::OnExit()
{}
bool GSocketGUIFunctionsTableNull::CanUseEventLoop()
{   return false; }
bool GSocketGUIFunctionsTableNull::Init_Socket(GSocket *WXUNUSED(socket))
{   return true; }
void GSocketGUIFunctionsTableNull::Destroy_Socket(GSocket *WXUNUSED(socket))
{}
void GSocketGUIFunctionsTableNull::Install_Callback(GSocket *WXUNUSED(socket), GSocketEvent WXUNUSED(event))
{}
void GSocketGUIFunctionsTableNull::Uninstall_Callback(GSocket *WXUNUSED(socket), GSocketEvent WXUNUSED(event))
{}
void GSocketGUIFunctionsTableNull::Enable_Events(GSocket *WXUNUSED(socket))
{}
void GSocketGUIFunctionsTableNull::Disable_Events(GSocket *WXUNUSED(socket))
{}
/* Global initialisers */

void GSocket_SetGUIFunctions(GSocketGUIFunctionsTable *guifunc)
{
  gs_gui_functions = guifunc;
}

int GSocket_Init(void)
{
  if (!gs_gui_functions)
  {
    static GSocketGUIFunctionsTableNull table;
    gs_gui_functions = &table;
  }
  if ( !gs_gui_functions->OnInit() )
    return 0;

#ifdef __WINDOWS__
  WSADATA wsaData;
  return (WSAStartup((1 << 8) | 1, &wsaData) == 0);
#else   
  return 1;
#endif
}

void GSocket_Cleanup(void)
{
  if (gs_gui_functions)
  {
      gs_gui_functions->OnExit();
  }
#ifdef __WINDOWS__
  WSACleanup();
#endif
}

/* Constructors / Destructors for GSocket */

GSocket::GSocket()
{
  int i;

  m_fd                  = INVALID_SOCKET;
  for (i=0;i<GSOCK_MAX_EVENT;i++)
  {
    m_cbacks[i]         = NULL;
  }
  m_detected            = 0;
  m_local               = NULL;
  m_peer                = NULL;
  m_error               = GSOCK_NOERROR;
  m_server              = false;
  m_stream              = true;
  m_non_blocking        = false;
  m_reusable            = false;
  m_udpconnected        = false;
#ifndef __WINDOWS__
  m_gui_dependent       = NULL;
#endif

  /* Default timeout is 10 minutes * 60 sec*/
  m_timeout.tv_sec      = 10*60;
  m_timeout.tv_usec     = 0;

  m_establishing        = false;

  assert(gs_gui_functions);
  /* Per-socket GUI-specific initialization */
  m_ok = gs_gui_functions->Init_Socket(this);
}

void GSocket::Close()
{
    gs_gui_functions->Disable_Events(this);
#warning Kry - Actually, does unix need also a close() here? Or is it redundant? What does Disable_Events do?.Defined INVALID_SOCKEt too? 
    if (m_fd != INVALID_SOCKET)
    {
      CLOSE_SOCKET(m_fd);
      m_fd = INVALID_SOCKET;
    }
}

GSocket::~GSocket()
{
  assert(this);

  /* Check that the socket is really shutdowned */


  if (m_fd != INVALID_SOCKET)
    Shutdown();

  /* Per-socket GUI-specific cleanup */
  gs_gui_functions->Destroy_Socket(this);

  /* Destroy private addresses */
  if (m_local)
    GAddress_destroy(m_local);

  if (m_peer)
    GAddress_destroy(m_peer);
}

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket::Shutdown()
{
  int evt;

  assert(this);

  /* Don't allow events to fire after socket has been closed */
  gs_gui_functions->Disable_Events(this);

  /* If socket has been created, shutdown it */
  if (m_fd != INVALID_SOCKET)
  {
    shutdown(m_fd, 2);
    Close();
  }

  /* Disable GUI callbacks */
  for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
    m_cbacks[evt] = NULL;

  m_detected = GSOCK_LOST_FLAG;
}

/* Address handling */

/* GSocket_SetLocal:
 * GSocket_GetLocal:
 * GSocket_SetPeer:
 * GSocket_GetPeer:
 *  Set or get the local or peer address for this socket. The 'set'
 *  functions return GSOCK_NOERROR on success, an error code otherwise.
 *  The 'get' functions return a pointer to a GAddress object on success,
 *  or NULL otherwise, in which case they set the error code of the
 *  corresponding GSocket.
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is not valid.
 *    GSOCK_INVADDR - the address is not valid.
 */
GSocketError GSocket::SetLocal(GAddress *address)
{
  assert(this);

  /* the socket must be initialized, or it must be a server */
  if ((m_fd != INVALID_SOCKET && !m_server))
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* check address */
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (m_local)
    GAddress_destroy(m_local);

  m_local = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GSocketError GSocket::SetPeer(GAddress *address)
{
  assert(this);

  /* check address */
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (m_peer)
    GAddress_destroy(m_peer);

  m_peer = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GAddress *GSocket::GetLocal()
{
  GAddress *address;
  struct sockaddr addr;
  WX_SOCKLEN_T size = sizeof(addr);
  GSocketError err;

  assert(this);

  /* try to get it from the m_local var first */
  if (m_local)
    return GAddress_copy(m_local);

  /* else, if the socket is initialized, try getsockname */
  if (m_fd == INVALID_SOCKET)
  {
    m_error = GSOCK_INVSOCK;
    return NULL;
  }

  if (getsockname(m_fd, &addr, (WX_SOCKLEN_T *) &size) == SOCKET_ERROR)
  {
    m_error = GSOCK_IOERR;
    return NULL;
  }

  /* got a valid address from getsockname, create a GAddress object */
  address = GAddress_new();
  if (address == NULL)
  {
    m_error = GSOCK_MEMERR;
    return NULL;
  }

  err = _GAddress_translate_from(address, &addr, size);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(address);
    m_error = err;
    return NULL;
  }

  return address;
}

GAddress *GSocket::GetPeer()
{
  assert(this);

  /* try to get it from the m_peer var */
  if (m_peer)
    return GAddress_copy(m_peer);

  return NULL;
}

/* Server specific parts */

/* GSocket_SetServer:
 *  Sets up this socket as a server. The local address must have been
 *  set with GSocket_SetLocal() before GSocket_SetServer() is called.
 *  Returns GSOCK_NOERROR on success, one of the following otherwise:
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is in use.
 *    GSOCK_INVADDR - the local address has not been set.
 *    GSOCK_IOERR   - low-level error.
 */
GSocketError GSocket::SetServer()
{
  IOCTL_TYPE arg = 1;

  assert(this);

  /* must not be in use */
  if (m_fd != INVALID_SOCKET)
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* the local addr must have been set */
  if (!m_local)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  m_stream   = true;
  m_server   = true;

  /* Create the socket */
  m_fd = socket(m_local->m_realfamily, SOCK_STREAM, 0);

  if (m_fd == INVALID_SOCKET)
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* FreeBSD variants can't use MSG_NOSIGNAL, and instead use a socket option */
#ifdef SO_NOSIGPIPE
  setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&arg, sizeof(u_long));
#endif

  IOCTL_SOCKET(m_fd, FIONBIO, &arg);
  gs_gui_functions->Enable_Events(this);

  /* allow a socket to re-bind if the socket is in the TIME_WAIT
     state after being previously closed.
   */
  if (m_reusable)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(u_long));
#ifdef SO_REUSEPORT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&arg, sizeof(u_long));
#endif
  }

  /* Bind to the local address,
   * retrieve the actual address bound,
   * and listen up to 5 connections.
   */

  if ( (bind(m_fd, m_local->m_addr, m_local->m_len) != 0) || (getsockname(m_fd, m_local->m_addr, (WX_SOCKLEN_T *) &m_local->m_len) != 0) || (listen(m_fd, 5) != 0) )
  {
    Close();
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* GSocket_WaitConnection:
 *  Waits for an incoming client connection. Returns a pointer to
 *  a GSocket object, or NULL if there was an error, in which case
 *  the last error field will be updated for the calling GSocket.
 *
 *  Error codes (set in the calling GSocket)
 *    GSOCK_INVSOCK    - the socket is not valid or not a server.
 *    GSOCK_TIMEDOUT   - timeout, no incoming connections.
 *    GSOCK_WOULDBLOCK - the call would block and the socket is nonblocking.
 *    GSOCK_MEMERR     - couldn't allocate memory.
 *    GSOCK_IOERR      - low-level error.
 */
GSocket *GSocket::WaitConnection()
{
  struct sockaddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  GSocket *connection;
  GSocketError err;
  IOCTL_TYPE arg = 1;

  assert(this);

  /* If the socket has already been created, we exit immediately */
  if (m_fd == INVALID_SOCKET || !m_server)
  {
    m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  if (!connection)
  {
    m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (Input_Timeout() == GSOCK_TIMEDOUT)
  {
    delete connection;
    /* m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

  connection->m_fd = accept(m_fd, &from, (WX_SOCKLEN_T *) &fromlen);

  /* Reenable CONNECTION events */
  Enable(GSOCK_CONNECTION);

  if (connection->m_fd == INVALID_SOCKET)
  {
    if (IN_PROGRESS)
      m_error = GSOCK_WOULDBLOCK;
    else
      m_error = GSOCK_IOERR;

    delete connection;
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = false;
  connection->m_stream   = true;

  /* Setup the peer address field */
  connection->m_peer = GAddress_new();
  if (!connection->m_peer)
  {
    delete connection;
    m_error = GSOCK_MEMERR;
    return NULL;
  }

  err = _GAddress_translate_from(connection->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    delete connection;
    m_error = err;
    return NULL;
  }

#if defined(__EMX__) || defined(__VISAGECPP__)
  IOCTL_SOCKET(connection->m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  IOCTL_SOCKET(connection->m_fd, FIONBIO, &arg);
#endif
  gs_gui_functions->Enable_Events(connection);

  return connection;
}

bool GSocket::SetReusable()
{
    /* socket must not be null, and must not be in use/already bound */
    if (this && m_fd == INVALID_SOCKET)
    {
        m_reusable = true;

        return true;
    }

    return false;
}

/* Client specific parts */

/* GSocket_Connect:
 *  For stream (connection oriented) sockets, GSocket_Connect() tries
 *  to establish a client connection to a server using the peer address
 *  as established with GSocket_SetPeer(). Returns GSOCK_NOERROR if the
 *  connection has been successfully established, or one of the error
 *  codes listed below. Note that for nonblocking sockets, a return
 *  value of GSOCK_WOULDBLOCK doesn't mean a failure. The connection
 *  request can be completed later; you should use GSocket_Select()
 *  to poll for GSOCK_CONNECTION | GSOCK_LOST, or wait for the
 *  corresponding asynchronous events.
 *
 *  For datagram (non connection oriented) sockets, GSocket_Connect()
 *  just sets the peer address established with GSocket_SetPeer() as
 *  default destination.
 *
 *  Error codes:
 *    GSOCK_INVSOCK    - the socket is in use or not valid.
 *    GSOCK_INVADDR    - the peer address has not been established.
 *    GSOCK_TIMEDOUT   - timeout, the connection failed.
 *    GSOCK_WOULDBLOCK - connection in progress (nonblocking sockets only)
 *    GSOCK_MEMERR     - couldn't allocate memory.
 *    GSOCK_IOERR      - low-level error.
 */
GSocketError GSocket::Connect(GSocketStream stream)
{
  int ret;
  IOCTL_TYPE arg = 1;

  assert(this);

  if (m_fd != INVALID_SOCKET)
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!m_peer)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Streamed or dgram socket? */
  m_stream   = (stream == GSOCK_STREAMED);

  if (!m_stream)
  {
    // This is datagram socket. For connecting, nothing new must be created
    // As it's all on the wxDatagramSocket constructor. Just call connect()
    // and return any error.
    ret = connect(m_fd, m_peer->m_addr, m_peer->m_len);

    if (ret == -1)
    {
      m_error = GSOCK_IOERR;
      return GSOCK_IOERR;
    }

    m_udpconnected = true;

    return GSOCK_NOERROR;

  }

  // TCP sockets have a lot of stuff to do before connecting...

  /* Enable CONNECTION events (needed for nonblocking connections) */
  Enable(GSOCK_CONNECTION);

  m_server   = false;
  m_establishing = false;

  /* Create the socket */
  m_fd = socket(m_peer->m_realfamily,
                     m_stream? SOCK_STREAM : SOCK_DGRAM, 0);

  if (m_fd == INVALID_SOCKET)
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* FreeBSD variants can't use MSG_NOSIGNAL, and instead use a socket option */
#ifdef SO_NOSIGPIPE
  setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&arg, sizeof(u_long));
#endif

#if defined(__EMX__) || defined(__VISAGECPP__)
  IOCTL_SOCKET(m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  IOCTL_SOCKET(m_fd, FIONBIO, &arg);
#endif

#ifdef __WXMSW__
  gs_gui_functions->Enable_Events(this);
#endif

  // If the reuse flag is set, use the applicable socket reuse flags(s)
  if (m_reusable)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(u_long));
#ifdef SO_REUSEPORT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&arg, sizeof(u_long));
#endif
  }

  // If a local address has been set, then we need to bind to it before calling connect
  if (m_local && m_local->m_addr)
  {
     bind(m_fd, m_local->m_addr, m_local->m_len);
  }

  /* Connect it to the peer address, with a timeout (see below) */
  ret = connect(m_fd, m_peer->m_addr, m_peer->m_len);

#ifndef __WXMSW__

  /* We only call Enable_Events if we know we aren't shutting down the socket.
   * NB: Enable_Events needs to be called whether the socket is blocking or
   * non-blocking, it just shouldn't be called prior to knowing there is a
   * connection _if_ blocking sockets are being used.
   * If connect above returns 0, we are already connected and need to make the
   * call to Enable_Events now.
   */

  if (m_non_blocking || ret == 0)
    gs_gui_functions->Enable_Events(this);

#endif

  if (ret == SOCKET_ERROR)
  {
    /* If connect failed with EINPROGRESS and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */

    if (IN_PROGRESS && (!m_non_blocking))
    {
      if (Output_Timeout() == GSOCK_TIMEDOUT)
      {
        Close();
        // m_error is set in _GSocket_Output_Timeout 
        return GSOCK_TIMEDOUT;
      }
      else
      {
       int error;
        SOCKOPTLEN_T len = sizeof(error);

        getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*) &error, &len);

        gs_gui_functions->Enable_Events(this);

        if (!error)
          return GSOCK_NOERROR;
      }
    }

    /* If connect failed with EINPROGRESS and the GSocket object
     * is set to nonblocking, we set m_error to GSOCK_WOULDBLOCK
     * (and return GSOCK_WOULDBLOCK) but we don't close the socket;
     * this way if the connection completes, a GSOCK_CONNECTION
     * event will be generated, if enabled.
     */
    if (IN_PROGRESS && (m_non_blocking))
    {
      m_establishing = true;
      m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EINPROGRESS,
     * then the call to GSocket_Connect has failed.
     */
    Close();
    m_error = GSOCK_IOERR;

    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Datagram sockets */

/* GSocket_SetNonOriented:
 *  Sets up this socket as a non-connection oriented (datagram) socket.
 *  Before using this function, the local address must have been set
 *  with GSocket_SetLocal(), or the call will fail. Returns GSOCK_NOERROR
 *  on success, or one of the following otherwise.
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is in use.
 *    GSOCK_INVADDR - the local address has not been set.
 *    GSOCK_IOERR   - low-level error.
 */
GSocketError GSocket::SetNonOriented()
{
  IOCTL_TYPE arg = 1;

  assert(this);

  if (m_fd != INVALID_SOCKET)
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!m_local)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  m_stream   = false;
  m_server   = false;

  /* Create the socket */
  m_fd = socket(m_local->m_realfamily, SOCK_DGRAM, 0);

  if (m_fd == INVALID_SOCKET)
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
#if defined(__EMX__) || defined(__VISAGECPP__)
  IOCTL_SOCKET(m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  IOCTL_SOCKET(m_fd, FIONBIO, &arg);
#endif
  gs_gui_functions->Enable_Events(this);

  /* Bind to the local address,
   * and retrieve the actual address bound.
   */
  if ((bind(m_fd, m_local->m_addr, m_local->m_len) != 0) ||
      (getsockname(m_fd,
                   m_local->m_addr,
                   (WX_SOCKLEN_T *) &m_local->m_len) != 0))
  {
    Close();
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* GSocket::UDPDisconnect:
 *  Removes a current UDP attachment to an address issued via Connect()
 */
bool GSocket::UDPDisconnect()
{
  struct sockaddr temp;
  // Acording to man connect, this will remove the connection.
  temp.sa_family = AF_UNSPEC;
  int ret = connect(m_fd,&temp,sizeof(temp));
  if (ret != -1)
    m_udpconnected = false;
  return (!m_udpconnected);
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket::Read(char *buffer, int size)
{
  int ret;

  assert(this);

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* Disable events during query of socket status */
  Disable(GSOCK_INPUT);

  /* If the socket is blocking, wait for data (with a timeout) */
  if (Input_Timeout() == GSOCK_TIMEDOUT) {
    m_error = GSOCK_TIMEDOUT;
    /* Don't return here immediately, otherwise socket events would not be
     * re-enabled! */
    ret = -1;
  }
  else
  {
    /* Read the data */
    if (m_stream)
      ret = Recv_Stream(buffer, size);
    else
      ret = Recv_Dgram(buffer, size);

    /* If recv returned zero, then the connection is lost, and errno is not set.
     * Otherwise, recv has returned an error (-1), in which case we have lost the
     * socket only if errno does _not_ indicate that there may be more data to read.
     */
    if (ret == 0) {
      m_error = GSOCK_IOERR;
      m_detected = GSOCK_LOST_FLAG;
      Close();
      // Signal an error for return;
      return -1;
    }
    else if (ret == -1)
    {
      if (IN_PROGRESS)
        m_error = GSOCK_WOULDBLOCK;
      else
        m_error = GSOCK_IOERR;
    }
  }

  /* Enable events again now that we are done processing */
  Enable(GSOCK_INPUT);

  return ret;
}

int GSocket::Write(const char *buffer, int size)
{
  int ret;

  assert(this);

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (Output_Timeout() == GSOCK_TIMEDOUT)
    return -1;

  /* Write the data */
  if (m_stream)
    ret = Send_Stream(buffer, size);
  else
    ret = Send_Dgram(buffer, size);

  if (ret == -1)
  {
    if (IN_PROGRESS)
    {
      m_error = GSOCK_WOULDBLOCK;
    }
    else
    {
      m_error = GSOCK_IOERR;
    }

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    Enable(GSOCK_OUTPUT);

    return -1;
  }

  return ret;
}

/* GSocket_Select:
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking | nonblocking) of the socket.
 */
GSocketEventFlags GSocket::Select(GSocketEventFlags flags)
{
  if (!gs_gui_functions->CanUseEventLoop())
  {

    GSocketEventFlags result = 0;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    struct timeval tv;

    assert(this);

    if (m_fd == INVALID_SOCKET)
        return (GSOCK_LOST_FLAG & flags);

    /* Linux may update the timeout value, so let's backup it */
    /* I'm not sure how MSW behaves here, so let's be safe */
    tv.tv_sec = m_timeout.tv_sec;
    tv.tv_usec = m_timeout.tv_usec;

    wxFD_ZERO(&readfds);
    wxFD_ZERO(&writefds);
    wxFD_ZERO(&exceptfds);
    wxFD_SET(m_fd, &readfds);
    if (flags & GSOCK_OUTPUT_FLAG || flags & GSOCK_CONNECTION_FLAG)
      wxFD_SET(m_fd, &writefds);
    wxFD_SET(m_fd, &exceptfds);

    /* Check 'sticky' CONNECTION flag first */
    result |= (GSOCK_CONNECTION_FLAG & m_detected);

    /* If we have already detected a LOST event, then don't try
     * to do any further processing.
     */
    if ((m_detected & GSOCK_LOST_FLAG) != 0)
    {
      m_establishing = false;

      return (GSOCK_LOST_FLAG & flags);
    }

    /* Try select now */
    if (select(m_fd + 1, &readfds, &writefds, &exceptfds, &tv) <= 0)
    {
      /* What to do here? */
      return (result & flags);
    }

    /* Check for exceptions and errors */
    if (wxFD_ISSET(m_fd, &exceptfds))
    {
      m_establishing = false;
      m_detected = GSOCK_LOST_FLAG;

      /* LOST event: Abort any further processing */
      return (GSOCK_LOST_FLAG & flags);
    }

    /* Check for readability */
    if (wxFD_ISSET(m_fd, &readfds))
    {
      result |= GSOCK_INPUT_FLAG;

      if (m_server && m_stream)
      {
        /* This is a TCP server socket that detected a connection.
          While the INPUT_FLAG is also set, it doesn't matter on
          this kind of  sockets, as we can only Accept() from them. */        
        result |= GSOCK_CONNECTION_FLAG;
        m_detected |= GSOCK_CONNECTION_FLAG;
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
          m_detected = GSOCK_LOST_FLAG;

          /* LOST event: Abort any further processing */
          return (GSOCK_LOST_FLAG & flags);
        }
        else
        {
          result |= GSOCK_CONNECTION_FLAG;
          m_detected |= GSOCK_CONNECTION_FLAG;
        }
      }
      else
      {
        result |= GSOCK_OUTPUT_FLAG;
      }
    }

    return (result & flags);

  }
  else
  {
    assert(this);
    return flags & m_detected;
  }
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket::SetNonBlocking(bool non_block)
{
  assert(this);

  m_non_blocking = non_block;
}

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket::SetTimeout(unsigned long millisec)
{
  assert(this);

  m_timeout.tv_sec = (millisec / 1000);
  m_timeout.tv_usec = (millisec % 1000) * 1000;
}

/* GSocket_GetError:
 *  Returns the last error occurred for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError WXDLLIMPEXP_NET GSocket::GetError()
{
  assert(this);

  return m_error;
}

/* Callbacks */

/* GSOCK_INPUT:
 *   There is data to be read in the input buffer. If, after a read
 *   operation, there is still data available, the callback function will
 *   be called again.
 * GSOCK_OUTPUT:
 *   The socket is available for writing. That is, the next write call
 *   won't block. This event is generated only once, when the connection is
 *   first established, and then only if a call failed with GSOCK_WOULDBLOCK,
 *   when the output buffer empties again. This means that the app should
 *   assume that it can write since the first OUTPUT event, and no more
 *   OUTPUT events will be generated unless an error occurs.
 * GSOCK_CONNECTION:
 *   Connection successfully established, for client sockets, or incoming
 *   client connection, for server sockets. Wait for this event (also watch
 *   out for GSOCK_LOST) after you issue a nonblocking GSocket_Connect() call.
 * GSOCK_LOST:
 *   The connection is lost (or a connection request failed); this could
 *   be due to a failure, or due to the peer closing it gracefully.
 */

/* GSocket_SetCallback:
 *  Enables the callbacks specified by 'flags'. Note that 'flags'
 *  may be a combination of flags OR'ed toghether, so the same
 *  callback function can be made to accept different events.
 *  The callback function must have the following prototype:
 *
 *  void function(GSocket *socket, GSocketEvent event, char *cdata)
 */
void GSocket::SetCallback(GSocketEventFlags flags,
                         GSocketCallback callback, char *cdata)
{
  int count;

  assert(this);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      m_cbacks[count] = callback;
      m_data[count] = cdata;
    }
  }
}

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket::UnsetCallback(GSocketEventFlags flags)
{
  int count;

  assert(this);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      m_cbacks[count] = NULL;
      m_data[count] = NULL;
    }
  }
}

GSocketError GSocket::GetSockOpt(int level, int optname,
                                void *optval, int *optlen)
{
    if (getsockopt(m_fd, level, optname, (char*)optval, (SOCKOPTLEN_T*)optlen) == 0)
        return GSOCK_NOERROR;

    return GSOCK_OPTERR;
}

GSocketError GSocket::SetSockOpt(int level, int optname,
                                const void *optval, int optlen)
{
    if (setsockopt(m_fd, level, optname, (const char*)optval, optlen) == 0)
        return GSOCK_NOERROR;

    return GSOCK_OPTERR;
}

#define CALL_CALLBACK(socket, event) {                                  \
  socket->Disable(event);                                               \
  if (socket->m_cbacks[event])                                          \
    socket->m_cbacks[event](socket, event, socket->m_data[event]);      \
}

void GSocket::Enable(GSocketEvent event)
{
  m_detected &= ~(1 << event);
#ifndef __WINDOWS__
  gs_gui_functions->Install_Callback(this, event);
#endif
}

void GSocket::Disable(GSocketEvent event)
{
  m_detected |= (1 << event);
#ifndef __WINDOWS__
  gs_gui_functions->Uninstall_Callback(this, event);
#endif
}

/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError GSocket::Input_Timeout()
{
  struct timeval tv;
  fd_set readfds;

  /* Linux select() will overwrite the struct on return */
  /* I'm not sure how MSW behaves here, so let's be safe */
  tv.tv_sec  = m_timeout.tv_sec;
  tv.tv_usec = m_timeout.tv_usec;;

  if (!m_non_blocking)
  {
    wxFD_ZERO(&readfds);
    wxFD_SET(m_fd, &readfds);
    if (select(m_fd + 1, &readfds, NULL, NULL, &tv) <= 0)
    {
      m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }
  }

  return GSOCK_NOERROR;
}

/* _GSocket_Output_Timeout:
 *  For blocking sockets, wait until data can be sent without
 *  blocking or until timeout ellapses.
 */
GSocketError GSocket::Output_Timeout()
{
  struct timeval tv;
  fd_set writefds;

  /* Linux select() will overwrite the struct on return */
  /* I'm not sure how MSW behaves here, so let's be safe */
  tv.tv_sec  = m_timeout.tv_sec;
  tv.tv_usec = m_timeout.tv_usec;

  if (!m_non_blocking)
  {
    wxFD_ZERO(&writefds);
    wxFD_SET(m_fd, &writefds);
    if (select(m_fd + 1, NULL, &writefds, NULL, &tv) <= 0)
    {
      m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }
  }

  return GSOCK_NOERROR;
}

/* _GSocket_Connect_Timeout:
 *  For blocking sockets, wait until the connection is
 *  established or fails, or until timeout ellapses.
 */
GSocketError GSocket::Connect_Timeout()
{
  fd_set writefds;
  fd_set exceptfds;

  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_SET(m_fd, &writefds);
  FD_SET(m_fd, &exceptfds);
  if (select(0, NULL, &writefds, &exceptfds, &m_timeout) <= 0)
  {
    m_error = GSOCK_TIMEDOUT;
    return GSOCK_TIMEDOUT;
  }

  if (!FD_ISSET(m_fd, &writefds))
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

int GSocket::Recv_Stream(char *buffer, int size)
{
  int ret;
  do
  {
    ret = recv(m_fd, buffer, size, GSOCKET_MSG_NOSIGNAL);
  }
  while (ret == SOCKET_ERROR && IS_INTERRUPTED); /* Loop until not interrupted */

  return ret;
}

int GSocket::Recv_Dgram(char *buffer, int size)
{
  struct sockaddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  int ret;
  GSocketError err;

  fromlen = sizeof(from);

  do
  {
    if (m_udpconnected)
      ret = recvfrom(m_fd, buffer, size, 0, NULL, (WX_SOCKLEN_T *) &fromlen);
    else 
      ret = recvfrom(m_fd, buffer, size, 0, &from, (WX_SOCKLEN_T *) &fromlen);      
  }
  while (ret == SOCKET_ERROR && IS_INTERRUPTED); /* Loop until not interrupted */

  if (ret == SOCKET_ERROR)
    return -1;

  /* Translate a system address into a GSocket address */
  if (!m_peer)
  {
    m_peer = GAddress_new();
    if (!m_peer)
    {
      m_error = GSOCK_MEMERR;
      return -1;
    }
  }

  err = _GAddress_translate_from(m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(m_peer);
    m_peer  = NULL;
    m_error = err;
    return -1;
  }

  return ret;
}

int GSocket::Send_Stream(const char *buffer, int size)
{
  int ret;

  MASK_SIGNAL();

  do
  {
    ret = send(m_fd, (char *)buffer, size, GSOCKET_MSG_NOSIGNAL);
  }
  while (ret == -1 && IS_INTERRUPTED); /* Loop until not interrupted */

  UNMASK_SIGNAL();

  return ret;
}

int GSocket::Send_Dgram(const char *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;
  GSocketError err;

  if (!m_peer)
  {
    m_error = GSOCK_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(m_peer, &addr, &len);
  if (err != GSOCK_NOERROR)
  {
    m_error = err;
    return -1;
  }

  MASK_SIGNAL();

  do
  {
    if (m_udpconnected)
      ret = sendto(m_fd, (char *)buffer, size, 0, NULL, len);
    else
      ret = sendto(m_fd, (char *)buffer, size, 0, addr, len);
  }
  while (ret == -1 && IS_INTERRUPTED); /* Loop until not interrupted */

  UNMASK_SIGNAL();

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void GSocket::Detected_Read()
{
#ifndef __WINDOWS__

  /* Safeguard against straggling call to Detectedw_Read */
  if (m_fd == INVALID_SOCKET)
  {
    return;
  }

  /* If we have already detected a LOST event, then don't try
   * to do any further processing.
   */
  if ((m_detected & GSOCK_LOST_FLAG) != 0)
  {
    m_establishing = false;

    CALL_CALLBACK(this, GSOCK_LOST);
    Shutdown();
    return;
  }


  if (m_server && m_stream)
  {
    CALL_CALLBACK(this, GSOCK_CONNECTION);
  }
  else
  {
    CALL_CALLBACK(this, GSOCK_INPUT);
  }
#endif
}

void GSocket::Detected_Write()
{
#ifndef __WINDOWS__
  /* If we have already detected a LOST event, then don't try
   * to do any further processing.
   */
  if ((m_detected & GSOCK_LOST_FLAG) != 0)
  {
    m_establishing = false;

    CALL_CALLBACK(this, GSOCK_LOST);
    Shutdown();
    return;
  }

  if (m_establishing && !m_server)
  {
    CALL_CALLBACK(this, GSOCK_CONNECTION);
    /* We have to fire this event by hand because CONNECTION (for clients)
     * and OUTPUT are internally the same and we just disabled CONNECTION
     * events with the above macro.
     */
    CALL_CALLBACK(this, GSOCK_OUTPUT);
  }
  else
  {
    CALL_CALLBACK(this, GSOCK_OUTPUT);
  }
#endif
}

/* Compatibility functions for GSocket */
GSocket *GSocket_new(void)
{
    GSocket *newsocket = new GSocket();
    if (newsocket->IsOk())
        return newsocket;

    delete newsocket;

    return NULL;
}

#endif  /* wxUSE_SOCKETS */
