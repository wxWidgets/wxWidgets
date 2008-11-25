/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        gsocket.c
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Authors:     David Elliott (C++ conversion, maintainer)
 *              Guilhem Lavaux,
 *              Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose:     GSocket main Unix and OS/2 file
 * Licence:     The wxWindows licence
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include "wx/gsocket.h"

#include "wx/private/fd.h"
#include "wx/private/socket.h"
#include "wx/private/gsocketiohandler.h"

#if defined(__VISAGECPP__)
#define BSD_SELECT /* use Berkeley Sockets select */
#endif

#if defined(__WATCOMC__)
#include <errno.h>
#include <nerrno.h>
#endif

#include <assert.h>
#include <sys/types.h>
#ifdef __VISAGECPP__
#include <string.h>
#include <sys/time.h>
#include <types.h>
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <sys/ioctl.h>

#ifdef HAVE_SYS_SELECT_H
#   include <sys/select.h>
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
#include <sys/socket.h>
#include <sys/un.h>
#endif

#ifndef __VISAGECPP__
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

#if defined(__VISAGECPP__) || defined(__WATCOMC__)

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
#else /* MSG_NOSIGNAL not available (FreeBSD including OS X) */
#  define GSOCKET_MSG_NOSIGNAL 0
#endif /* MSG_NOSIGNAL */

#if wxUSE_THREADS && (defined(HAVE_GETHOSTBYNAME) || defined(HAVE_GETSERVBYNAME))
#  include "wx/thread.h"
#endif

#if defined(HAVE_GETHOSTBYNAME)
static struct hostent * deepCopyHostent(struct hostent *h,
					const struct hostent *he,
					char *buffer, int size, int *err)
{
  /* copy old structure */
  memcpy(h, he, sizeof(struct hostent));

  /* copy name */
  int len = strlen(h->h_name);
  if (len > size)
  {
    *err = ENOMEM;
    return NULL;
  }
  memcpy(buffer, h->h_name, len);
  buffer[len] = '\0';
  h->h_name = buffer;

  /* track position in the buffer */
  int pos = len + 1;

  /* reuse len to store address length */
  len = h->h_length;

  /* ensure pointer alignment */
  unsigned int misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  char **p = h->h_addr_list, **q;
  char **h_addr_list = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy addresses and fill new pointer list */
  for (p = h->h_addr_list, q = h_addr_list; *p != 0; p++, q++)
  {
    if (size < pos + len)
    {
      *err = ENOMEM;
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len;
  }
  *++q = 0; /* null terminate the pointer list */
  h->h_addr_list = h_addr_list; /* copy pointer to pointers */

  /* ensure word alignment of pointers */
  misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  p = h->h_aliases;
  char **h_aliases = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy aliases and fill new pointer list */
  for (p = h->h_aliases, q = h_aliases; *p != 0; p++, q++)
  {
    len = strlen(*p);
    if (size <= pos + len)
    {
      *err = ENOMEM;
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    buffer[pos + len] = '\0';
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len + 1;
  }
  *++q = 0; /* null terminate the pointer list */
  h->h_aliases = h_aliases; /* copy pointer to pointers */

  return h;
}
#endif

#if defined(HAVE_GETHOSTBYNAME) && wxUSE_THREADS
static wxMutex nameLock;
#endif
struct hostent * wxGethostbyname_r(const char *hostname, struct hostent *h,
				   void *buffer, int size, int *err)

{
  struct hostent *he = NULL;
  *err = 0;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
  if (gethostbyname_r(hostname, h, (char*)buffer, size, &he, err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
  he = gethostbyname_r(hostname, h, (char*)buffer, size, err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  if (gethostbyname_r(hostname, h, (struct hostent_data*) buffer))
  {
    he = NULL;
    *err = h_errno;
  }
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(nameLock);
#endif
  he = gethostbyname(hostname);
  if (!he)
    *err = h_errno;
  else
    he = deepCopyHostent(h, he, (char*)buffer, size, err);
#endif
  return he;
}

#if defined(HAVE_GETHOSTBYNAME) && wxUSE_THREADS
static wxMutex addrLock;
#endif
struct hostent * wxGethostbyaddr_r(const char *addr_buf, int buf_size,
				   int proto, struct hostent *h,
				   void *buffer, int size, int *err)
{
  struct hostent *he = NULL;
  *err = 0;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
  if (gethostbyaddr_r(addr_buf, buf_size, proto, h,
		      (char*)buffer, size, &he, err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
  he = gethostbyaddr_r(addr_buf, buf_size, proto, h, (char*)buffer, size, err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  if (gethostbyaddr_r(addr_buf, buf_size, proto, h,
			(struct hostent_data*) buffer))
  {
    he = NULL;
    *err = h_errno;
  }
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(addrLock);
#endif
  he = gethostbyaddr(addr_buf, buf_size, proto);
  if (!he)
    *err = h_errno;
  else
    he = deepCopyHostent(h, he, (char*)buffer, size, err);
#endif
  return he;
}

#if defined(HAVE_GETSERVBYNAME)
static struct servent * deepCopyServent(struct servent *s,
					const struct servent *se,
					char *buffer, int size)
{
  /* copy plain old structure */
  memcpy(s, se, sizeof(struct servent));

  /* copy name */
  int len = strlen(s->s_name);
  if (len >= size)
  {
    return NULL;
  }
  memcpy(buffer, s->s_name, len);
  buffer[len] = '\0';
  s->s_name = buffer;

  /* track position in the buffer */
  int pos = len + 1;

  /* copy protocol */
  len = strlen(s->s_proto);
  if (pos + len >= size)
  {
    return NULL;
  }
  memcpy(buffer + pos, s->s_proto, len);
  buffer[pos + len] = '\0';
  s->s_proto = buffer + pos;

  /* track position in the buffer */
  pos += len + 1;

  /* ensure pointer alignment */
  unsigned int misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  char **p = s->s_aliases, **q;
  char **s_aliases = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy addresses and fill new pointer list */
  for (p = s->s_aliases, q = s_aliases; *p != 0; p++, q++){
    len = strlen(*p);
    if (size <= pos + len)
    {
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    buffer[pos + len] = '\0';
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len + 1;
  }
  *++q = 0; /* null terminate the pointer list */
  s->s_aliases = s_aliases; /* copy pointer to pointers */
  return s;
}
#endif

#if defined(HAVE_GETSERVBYNAME) && wxUSE_THREADS
static wxMutex servLock;
#endif
struct servent *wxGetservbyname_r(const char *port, const char *protocol,
				  struct servent *serv, void *buffer, int size)
{
  struct servent *se = NULL;
#if defined(HAVE_FUNC_GETSERVBYNAME_R_6)
  if (getservbyname_r(port, protocol, serv, (char*)buffer, size, &se))
    se = NULL;
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_5)
  se = getservbyname_r(port, protocol, serv, (char*)buffer, size);
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_4)
  if (getservbyname_r(port, protocol, serv, (struct servent_data*) buffer))
    se = NULL;
  else
    se = serv;
#elif defined(HAVE_GETSERVBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(servLock);
#endif
  se = getservbyname(port, protocol);
  if (se)
    se = deepCopyServent(serv, se, (char*)buffer, size);
#endif
  return se;
}

/* debugging helpers */
#ifdef __GSOCKET_DEBUG__
#  define GSocket_Debug(args) printf args
#else
#  define GSocket_Debug(args)
#endif /* __GSOCKET_DEBUG__ */

/* Table of GUI-related functions. We must call them indirectly because
 * of wxBase and GUI separation: */

bool GSocket_Init()
{
    GSocketManager * const manager = GSocketManager::Get();
    return manager && manager->OnInit();
}

void GSocket_Cleanup()
{
    GSocketManager * const manager = GSocketManager::Get();
    if ( manager )
        manager->OnExit();
}

/* Constructors / Destructors for GSocket */

GSocket::GSocket(wxSocketBase& wxsocket)
       : GSocketBase(wxsocket)
{
  m_handler             = NULL;

  m_gui_dependent       = NULL;
  m_use_events          = false;
}

GSocket::~GSocket()
{
  delete m_handler;
}

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket::Shutdown()
{
    /* Don't allow events to fire after socket has been closed */
    DisableEvents();

    GSocketBase::Shutdown();
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
  int arg = 1;

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
  setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&arg, sizeof(arg));
#endif

  ioctl(m_fd, FIONBIO, &arg);
  EnableEvents();

  /* allow a socket to re-bind if the socket is in the TIME_WAIT
     state after being previously closed.
   */
  if (m_reusable)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(arg));
#ifdef SO_REUSEPORT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&arg, sizeof(arg));
#endif
  }

  /* Bind to the local address,
   * retrieve the actual address bound,
   * and listen up to 5 connections.
   */
  if ((bind(m_fd, m_local->m_addr, m_local->m_len) != 0) ||
      (getsockname(m_fd,
                   m_local->m_addr,
                   (WX_SOCKLEN_T *) &m_local->m_len) != 0) ||
      (listen(m_fd, 5) != 0))
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
GSocket *GSocket::WaitConnection(wxSocketBase& wxsocket)
{
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  GSocket *connection;
  GSocketError err;
  int arg = 1;

  assert(this);

  /* If the socket has already been created, we exit immediately */
  if (m_fd == INVALID_SOCKET || !m_server)
  {
    m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket::Create(wxsocket);

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

  connection->m_fd = accept(m_fd, (sockaddr*)&from, (WX_SOCKLEN_T *) &fromlen);

  /* Reenable CONNECTION events */
  EnableEvent(GSOCK_CONNECTION);

  if (connection->m_fd == INVALID_SOCKET)
  {
    if (errno == EWOULDBLOCK)
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

  err = _GAddress_translate_from(connection->m_peer, (sockaddr*)&from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    delete connection;
    m_error = err;
    return NULL;
  }

#if defined(__EMX__) || defined(__VISAGECPP__)
  ioctl(connection->m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  ioctl(connection->m_fd, FIONBIO, &arg);
#endif
  if (m_use_events)
    connection->Notify(true);

  return connection;
}

void GSocket::Notify(bool flag)
{
    if (flag == m_use_events)
        return;
    m_use_events = flag;
    DoEnableEvents(flag);
}

void GSocket::DoEnableEvents(bool flag)
{
    GSocketManager * const manager = GSocketManager::Get();
    if ( flag )
    {
        manager->Install_Callback(this, GSOCK_INPUT);
        manager->Install_Callback(this, GSOCK_OUTPUT);
    }
    else // off
    {
        manager->Uninstall_Callback(this, GSOCK_INPUT);
        manager->Uninstall_Callback(this, GSOCK_OUTPUT);
    }
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

bool GSocket::SetBroadcast()
{
    /* socket must not be in use/already bound */
    if (m_fd == INVALID_SOCKET) {
        m_broadcast = true;
        return true;
    }
    return false;
}

bool GSocket::DontDoBind()
{
    /* socket must not be in use/already bound */
    if (m_fd == INVALID_SOCKET) {
        m_dobind = false;
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
  int err, ret;
  int arg = 1;

  assert(this);

  /* Enable CONNECTION events (needed for nonblocking connections) */
  EnableEvent(GSOCK_CONNECTION);

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
  setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&arg, sizeof(arg));
#endif

#if defined(__EMX__) || defined(__VISAGECPP__)
  ioctl(m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  ioctl(m_fd, FIONBIO, &arg);
#endif

  // If the reuse flag is set, use the applicable socket reuse flags(s)
  if (m_reusable)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(arg));
#ifdef SO_REUSEPORT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&arg, sizeof(arg));
#endif
  }

  if (m_initialRecvBufferSize >= 0)
    setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, (const char*)&m_initialRecvBufferSize, sizeof(m_initialRecvBufferSize));
  if (m_initialSendBufferSize >= 0)
    setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, (const char*)&m_initialSendBufferSize, sizeof(m_initialSendBufferSize));

  // If a local address has been set, then we need to bind to it before calling connect
  if (m_local && m_local->m_addr)
  {
     bind(m_fd, m_local->m_addr, m_local->m_len);
  }

  /* Connect it to the peer address, with a timeout (see below) */
  ret = connect(m_fd, m_peer->m_addr, m_peer->m_len);

  /* We only call EnableEvents() if we know we aren't shutting down the socket.
   * NB: EnableEvents() needs to be called whether the socket is blocking or
   * non-blocking, it just shouldn't be called prior to knowing there is a
   * connection _if_ blocking sockets are being used.
   * If connect above returns 0, we are already connected and need to make the
   * call to EnableEvents() now.
   */
  if ( m_non_blocking || (ret == 0) )
    EnableEvents();

  if (ret == -1)
  {
    err = errno;

    /* If connect failed with EINPROGRESS and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */
    if ((err == EINPROGRESS) && (!m_non_blocking))
    {
      if (Output_Timeout() == GSOCK_TIMEDOUT)
      {
        Close();
        /* m_error is set in _GSocket_Output_Timeout */
        return GSOCK_TIMEDOUT;
      }
      else
      {
        int error;
        SOCKOPTLEN_T len = sizeof(error);

        getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*) &error, &len);
        EnableEvents();

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
    if ((err == EINPROGRESS) && (m_non_blocking))
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
  int arg = 1;

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
  ioctl(m_fd, FIONBIO, (char*)&arg, sizeof(arg));
#else
  ioctl(m_fd, FIONBIO, &arg);
#endif
  EnableEvents();

  if (m_reusable)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&arg, sizeof(arg));
#ifdef SO_REUSEPORT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&arg, sizeof(arg));
#endif
  }

  if (m_broadcast)
  {
    setsockopt(m_fd, SOL_SOCKET, SO_BROADCAST, (const char*)&arg, sizeof(arg));
  }
  if (m_dobind)
  {
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
  }
  return GSOCK_NOERROR;
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
  DisableEvent(GSOCK_INPUT);

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

    /*
     * If recv returned zero for a TCP socket (if m_stream == NULL, it's an UDP
     * socket and empty datagrams are possible), then the connection has been
     * gracefully closed.
     *
     * Otherwise, recv has returned an error (-1), in which case we have lost
     * the socket only if errno does _not_ indicate that there may be more data
     * to read.
     */
    if ((ret == 0) && m_stream)
    {
      /* Make sure wxSOCKET_LOST event gets sent and shut down the socket */
      if (m_use_events)
      {
        m_detected = GSOCK_LOST_FLAG;
        Detected_Read();
        return 0;
      }
    }
    else if (ret == -1)
    {
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
        m_error = GSOCK_WOULDBLOCK;
      else
        m_error = GSOCK_IOERR;
    }
  }

  /* Enable events again now that we are done processing */
  EnableEvent(GSOCK_INPUT);

  return ret;
}

int GSocket::Write(const char *buffer, int size)
{
  int ret;

  assert(this);

  GSocket_Debug(( "GSocket_Write #1, size %d\n", size ));

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = GSOCK_INVSOCK;
    return -1;
  }

  GSocket_Debug(( "GSocket_Write #2, size %d\n", size ));

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (Output_Timeout() == GSOCK_TIMEDOUT)
    return -1;

  GSocket_Debug(( "GSocket_Write #3, size %d\n", size ));

  /* Write the data */
  if (m_stream)
    ret = Send_Stream(buffer, size);
  else
    ret = Send_Dgram(buffer, size);

  GSocket_Debug(( "GSocket_Write #4, size %d\n", size ));

  if (ret == -1)
  {
    if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
    {
      m_error = GSOCK_WOULDBLOCK;
      GSocket_Debug(( "GSocket_Write error WOULDBLOCK\n" ));
    }
    else
    {
      m_error = GSOCK_IOERR;
      GSocket_Debug(( "GSocket_Write error IOERR\n" ));
    }

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    EnableEvent(GSOCK_OUTPUT);

    return -1;
  }

  GSocket_Debug(( "GSocket_Write #5, size %d ret %d\n", size, ret ));

  return ret;
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket::SetNonBlocking(bool non_block)
{
  assert(this);

  GSocket_Debug( ("GSocket_SetNonBlocking: %d\n", (int)non_block) );

  m_non_blocking = non_block;
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

void GSocket::EnableEvent(GSocketEvent event)
{
    if (m_use_events)
    {
        m_detected &= ~(1 << event);
        GSocketManager::Get()->Install_Callback(this, event);
    }
}

void GSocket::DisableEvent(GSocketEvent event)
{
    if (m_use_events)
    {
        m_detected |= (1 << event);
        GSocketManager::Get()->Uninstall_Callback(this, event);
    }
}

/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError GSocket::Input_Timeout()
{
  fd_set readfds;
  int ret;

  // Linux select() will overwrite the struct on return so make a copy
  struct timeval tv = m_timeout;

  if (!m_non_blocking)
  {
    wxFD_ZERO(&readfds);
    wxFD_SET(m_fd, &readfds);
    ret = select(m_fd + 1, &readfds, NULL, NULL, &tv);
    if (ret == 0)
    {
      GSocket_Debug(( "GSocket_Input_Timeout, select returned 0\n" ));
      m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }

    if (ret == -1)
    {
      GSocket_Debug(( "GSocket_Input_Timeout, select returned -1\n" ));
      if (errno == EBADF) { GSocket_Debug(( "Invalid file descriptor\n" )); }
      if (errno == EINTR) { GSocket_Debug(( "A non blocked signal was caught\n" )); }
      if (errno == EINVAL) { GSocket_Debug(( "The highest number descriptor is negative\n" )); }
      if (errno == ENOMEM) { GSocket_Debug(( "Not enough memory\n" )); }
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
  fd_set writefds;
  int ret;

  // Linux select() will overwrite the struct on return so make a copy
  struct timeval tv = m_timeout;

  GSocket_Debug( ("m_non_blocking has: %d\n", (int)m_non_blocking) );

  if (!m_non_blocking)
  {
    wxFD_ZERO(&writefds);
    wxFD_SET(m_fd, &writefds);
    ret = select(m_fd + 1, NULL, &writefds, NULL, &tv);
    if (ret == 0)
    {
      GSocket_Debug(( "GSocket_Output_Timeout, select returned 0\n" ));
      m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }

    if (ret == -1)
    {
      GSocket_Debug(( "GSocket_Output_Timeout, select returned -1\n" ));
      if (errno == EBADF) { GSocket_Debug(( "Invalid file descriptor\n" )); }
      if (errno == EINTR) { GSocket_Debug(( "A non blocked signal was caught\n" )); }
      if (errno == EINVAL) { GSocket_Debug(( "The highest number descriptor is negative\n" )); }
      if (errno == ENOMEM) { GSocket_Debug(( "Not enough memory\n" )); }
      m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }

    if ( ! wxFD_ISSET(m_fd, &writefds) )
    {
        GSocket_Debug(( "GSocket_Output_Timeout is buggy!\n" ));
    }
    else
    {
        GSocket_Debug(( "GSocket_Output_Timeout seems correct\n" ));
    }
  }
  else
  {
    GSocket_Debug(( "GSocket_Output_Timeout, didn't try select!\n" ));
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
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  return ret;
}

int GSocket::Recv_Dgram(char *buffer, int size)
{
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  int ret;
  GSocketError err;

  fromlen = sizeof(from);

  do
  {
    ret = recvfrom(m_fd, buffer, size, 0, (sockaddr*)&from, (WX_SOCKLEN_T *) &fromlen);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  if (ret == -1)
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

  err = _GAddress_translate_from(m_peer, (sockaddr*)&from, fromlen);
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
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

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
    ret = sendto(m_fd, (char *)buffer, size, 0, addr, len);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  UNMASK_SIGNAL();

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void GSocket::OnStateChange(GSocketEvent event)
{
    DisableEvent(event);
    NotifyOnStateChange(event);

    if ( event == GSOCK_LOST )
        Shutdown();
}

void GSocket::Detected_Read()
{
  char c;

  /* Safeguard against straggling call to Detected_Read */
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

    OnStateChange(GSOCK_LOST);
    return;
  }

  int num =  recv(m_fd, &c, 1, MSG_PEEK | GSOCKET_MSG_NOSIGNAL);

  if (num > 0)
  {
    OnStateChange(GSOCK_INPUT);
  }
  else
  {
    if (m_server && m_stream)
    {
      OnStateChange(GSOCK_CONNECTION);
    }
    else if (num == 0)
    {
      if (m_stream)
      {
        /* graceful shutdown */
        OnStateChange(GSOCK_LOST);
      }
      else
      {
        /* Empty datagram received */
        OnStateChange(GSOCK_INPUT);
      }
    }
    else
    {
      /* Do not throw a lost event in cases where the socket isn't really lost */
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN) || (errno == EINTR))
      {
        OnStateChange(GSOCK_INPUT);
      }
      else
      {
        OnStateChange(GSOCK_LOST);
      }
    }
  }
}

void GSocket::Detected_Write()
{
  /* If we have already detected a LOST event, then don't try
   * to do any further processing.
   */
  if ((m_detected & GSOCK_LOST_FLAG) != 0)
  {
    m_establishing = false;

    OnStateChange(GSOCK_LOST);
    return;
  }

  if (m_establishing && !m_server)
  {
    int error;
    SOCKOPTLEN_T len = sizeof(error);

    m_establishing = false;

    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

    if (error)
    {
      OnStateChange(GSOCK_LOST);
    }
    else
    {
      OnStateChange(GSOCK_CONNECTION);
      /* We have to fire this event by hand because CONNECTION (for clients)
       * and OUTPUT are internally the same and we just disabled CONNECTION
       * events with the above macro.
       */
      OnStateChange(GSOCK_OUTPUT);
    }
  }
  else
  {
    OnStateChange(GSOCK_OUTPUT);
  }
}

/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current address family is either
 * GSOCK_NOFAMILY or GSOCK_*family*, and if it is GSOCK_NOFAMILY, it
 * initalizes it to be a GSOCK_*family*. In other cases, it returns
 * an appropiate error code.
 *
 * CHECK_ADDRESS_RETVAL does the same but returning 'retval' on error.
 */
#define CHECK_ADDRESS(address, family)                              \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return GSOCK_INVADDR;                                           \
  }                                                                 \
}

#define CHECK_ADDRESS_RETVAL(address, family, retval)               \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return retval;                                                \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}


GAddress *GAddress_new(void)
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family  = GSOCK_NOFAMILY;
  address->m_addr    = NULL;
  address->m_len     = 0;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

  assert(address != NULL);

  if ((addr2 = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr && address->m_len > 0)
  {
    addr2->m_addr = (struct sockaddr *)malloc(addr2->m_len);
    if (addr2->m_addr == NULL)
    {
      free(addr2);
      return NULL;
    }
    memcpy(addr2->m_addr, address->m_addr, addr2->m_len);
  }

  return addr2;
}

void GAddress_destroy(GAddress *address)
{
  assert(address != NULL);

  if (address->m_addr)
    free(address->m_addr);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  assert(address != NULL);

  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
  assert(address != NULL);

  return address->m_family;
}

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len)
{
  address->m_realfamily = addr->sa_family;
  switch (addr->sa_family)
  {
    case AF_INET:
      address->m_family = GSOCK_INET;
      break;
    case AF_UNIX:
      address->m_family = GSOCK_UNIX;
      break;
#if wxUSE_IPV6
    case AF_INET6:
      address->m_family = GSOCK_INET6;
      break;
#endif // wxUSE_IPV6
    default:
    {
      address->m_error = GSOCK_INVOP;
      return GSOCK_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len  = len;
  address->m_addr = (struct sockaddr *)malloc(len);

  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(address->m_addr, addr, len);

  return GSOCK_NOERROR;
}

GSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len)
{
  if (!address->m_addr)
  {
    address->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  *len = address->m_len;
  *addr = (struct sockaddr *)malloc(address->m_len);
  if (*addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(*addr, address->m_addr, address->m_len);
  return GSOCK_NOERROR;
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_INET(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_in);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_family = GSOCK_INET;
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  /* If it is a numeric host name, convert it now */
#if defined(HAVE_INET_ATON)
  if (inet_aton(hostname, addr) == 0)
  {
#elif defined(HAVE_INET_ADDR)
  if ( (addr->s_addr = inet_addr(hostname)) == (unsigned)-1 )
  {
#else
  /* Use gethostbyname by default */
#ifndef __WXMAC__
  int val = 1;  /* VA doesn't like constants in conditional expressions */
  if (val)
#endif
  {
#endif
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    struct hostent h;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
    struct hostent_data buffer;
#else
    char buffer[1024];
#endif
    int err;
    he = wxGethostbyname_r(hostname, &h, (void*)&buffer, sizeof(buffer), &err);
    if (he == NULL)
    {
      /* Reset to invalid address */
      addr->s_addr = INADDR_NONE;
      address->m_error = GSOCK_NOHOST;
      return GSOCK_NOHOST;
    }

    array_addr = (struct in_addr *) *(he->h_addr_list);
    addr->s_addr = array_addr[0].s_addr;
  }

  return GSOCK_NOERROR;
}


GSocketError GAddress_INET_SetBroadcastAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_BROADCAST);
}

GSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);
  addr->s_addr = htonl(hostaddr);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

#if defined(HAVE_FUNC_GETSERVBYNAME_R_4)
    struct servent_data buffer;
#else
  char buffer[1024];
#endif
  struct servent serv;
  se = wxGetservbyname_r(port, protocol, &serv,
			 (void*)&buffer, sizeof(buffer));
  if (!se)
  {
    /* the cast to int suppresses compiler warnings about subscript having the
       type char */
    if (isdigit((int)port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in *)address->m_addr;
      addr->sin_port = htons(port_int);
      return GSOCK_NOERROR;
    }

    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = se->s_port;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = htons(port);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  struct hostent *he;
  char *addr_buf;
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  struct hostent temphost;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  struct hostent_data buffer;
#else
  char buffer[1024];
#endif
  int err;
  he = wxGethostbyaddr_r(addr_buf, sizeof(addr->sin_addr), AF_INET, &temphost,
			 (void*)&buffer, sizeof(buffer), &err);
  if (he == NULL)
  {
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }

  strncpy(hostname, he->h_name, sbuf);

  return GSOCK_NOERROR;
}

unsigned long GAddress_INET_GetHostAddress(GAddress *address)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohl(addr->sin_addr.s_addr);
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;
  return ntohs(addr->sin_port);
}

#if wxUSE_IPV6
/*
 * -------------------------------------------------------------------------
 * Internet IPv6 address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_INET6(GAddress *address)
{
  struct in6_addr any_address = IN6ADDR_ANY_INIT;
  address->m_len  = sizeof(struct sockaddr_in6);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }
  memset(address->m_addr,0,address->m_len);

  address->m_family = GSOCK_INET6;
  address->m_realfamily = AF_INET6;
  ((struct sockaddr_in6 *)address->m_addr)->sin6_family = AF_INET6;
  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = any_address;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_SetHostName(GAddress *address, const char *hostname)
{
  assert(address != NULL);
  CHECK_ADDRESS(address, INET6);

  addrinfo hints;
  memset( & hints, 0, sizeof( hints ) );
  hints.ai_family = AF_INET6;
  addrinfo * info = 0;
  if ( getaddrinfo( hostname, "0", & hints, & info ) || ! info )
  {
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }

  memcpy( address->m_addr, info->ai_addr, info->ai_addrlen );
  freeaddrinfo( info );
  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_SetAnyAddress(GAddress *address)
{
  assert(address != NULL);

  CHECK_ADDRESS(address, INET6);

  struct in6_addr addr;
  memset( & addr, 0, sizeof( addr ) );
  return GAddress_INET6_SetHostAddress(address, addr);
}
GSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr)
{
  assert(address != NULL);

  CHECK_ADDRESS(address, INET6);

  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = hostaddr;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in6 *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET6);

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  se = getservbyname(port, protocol);
  if (!se)
  {
    if (isdigit(port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in6 *)address->m_addr;
      addr->sin6_port = htons((u_short) port_int);
      return GSOCK_NOERROR;
    }

    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr->sin6_port = se->s_port;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in6 *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET6);

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr->sin6_port = htons(port);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  struct hostent *he;
  char *addr_buf;
  struct sockaddr_in6 *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET6);

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr_buf = (char *)&(addr->sin6_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin6_addr), AF_INET6);
  if (he == NULL)
  {
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }

  strncpy(hostname, he->h_name, sbuf);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET6_GetHostAddress(GAddress *address,struct in6_addr *hostaddr)
{
  assert(address != NULL);
  assert(hostaddr != NULL);
  CHECK_ADDRESS_RETVAL(address, INET6, GSOCK_INVADDR);
  *hostaddr = ( (struct sockaddr_in6 *)address->m_addr )->sin6_addr;
  return GSOCK_NOERROR;
}

unsigned short GAddress_INET6_GetPort(GAddress *address)
{
  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET6, 0);

  return ntohs( ((struct sockaddr_in6 *)address->m_addr)->sin6_port );
}

#endif // wxUSE_IPV6

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

#ifndef __VISAGECPP__
GSocketError _GAddress_Init_UNIX(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_un);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_family = GSOCK_UNIX;
  address->m_realfamily = PF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_family = AF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_path[0] = 0;

  return GSOCK_NOERROR;
}

#define UNIX_SOCK_PATHLEN (sizeof(addr->sun_path)/sizeof(addr->sun_path[0]))

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
  struct sockaddr_un *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, UNIX);

  addr = ((struct sockaddr_un *)address->m_addr);
  strncpy(addr->sun_path, path, UNIX_SOCK_PATHLEN);
  addr->sun_path[UNIX_SOCK_PATHLEN - 1] = '\0';

  return GSOCK_NOERROR;
}

GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
  struct sockaddr_un *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, UNIX);

  addr = (struct sockaddr_un *)address->m_addr;

  strncpy(path, addr->sun_path, sbuf);

  return GSOCK_NOERROR;
}
#endif  /* !defined(__VISAGECPP__) */
#endif  /* wxUSE_SOCKETS */
