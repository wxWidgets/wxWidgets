/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsocket.c
 * Authors: Guilhem Lavaux,
 *          Guillermo Rodriguez Garcia <guille@iies.es> (maintainer)
 * Purpose: GSocket main Unix file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

/*
 * PLEASE don't put C++ comments here - this is a C source file.
 */

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#include <assert.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>

#ifdef __VMS__
#include <socket.h>
struct sockaddr_un {
    u_char  sun_len;        /* sockaddr len including null */
    u_char  sun_family;     /* AF_UNIX */
    char    sun_path[108];  /* path name (gag) */
};
#else
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
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
#include <signal.h>

#ifndef SOCKLEN_T

#ifdef VMS
#  define SOCKLEN_T unsigned int
#else
#  ifdef __GLIBC__
#    if __GLIBC__ == 2
#       define SOCKLEN_T socklen_t
#    endif
#  else
#    define SOCKLEN_T int
#  endif
#endif

#endif /* SOCKLEN_T */


/*
 * INADDR_BROADCAST is identical to INADDR_NONE which is not defined
 * on all unices. INADDR_BROADCAST should be fine to indicate an error.
 */
#ifndef INADDR_NONE
#define INADDR_NONE INADDR_BROADCAST
#endif

#define MASK_SIGNAL()                       \
{                                           \
  void (*old_handler)(int);                 \
                                            \
  old_handler = signal(SIGPIPE, SIG_IGN);

#define UNMASK_SIGNAL()                     \
  signal(SIGPIPE, old_handler);             \
}


#ifndef __GSOCKET_STANDALONE__

#include "wx/unix/gsockunx.h"
#include "wx/gsocket.h"

#else

#include "gsockunx.h"
#include "gsocket.h"

#endif /* __GSOCKET_STANDALONE__ */

/* redefine some GUI-only functions to do nothing in console mode */
#if defined(wxUSE_GUI) && !wxUSE_GUI
    #define _GSocket_GUI_Init(socket)
    #define _GSocket_GUI_Destroy(socket)
    #define _GSocket_Enable_Events(socket)
    #define _GSocket_Disable_Events(socket)
    #define _GSocket_Install_Callback(socket, event)
    #define _GSocket_Uninstall_Callback(socket, event)
#endif /* wxUSE_GUI */

/* Global initialisers */

bool GSocket_Init()
{
  return TRUE;
}

void GSocket_Cleanup()
{
}

/* Constructors / Destructors for GSocket */

GSocket *GSocket_new()
{
  int i;
  GSocket *socket;

  socket = (GSocket *)malloc(sizeof(GSocket));

  if (socket == NULL)
    return NULL;

  socket->m_fd                  = -1;
  for (i=0;i<GSOCK_MAX_EVENT;i++)
  {
    socket->m_cbacks[i]         = NULL;
  }
  socket->m_detected            = 0;
  socket->m_local               = NULL;
  socket->m_peer                = NULL;
  socket->m_error               = GSOCK_NOERROR;
  socket->m_server              = FALSE;
  socket->m_stream              = TRUE;
  socket->m_gui_dependent       = NULL;
  socket->m_non_blocking        = FALSE;
  socket->m_timeout             = 10*60*1000;
                                /* 10 minutes * 60 sec * 1000 millisec */
  socket->m_establishing        = FALSE;

  /* We initialize the GUI specific entries here */
  _GSocket_GUI_Init(socket);

  return socket;
}

void GSocket_destroy(GSocket *socket)
{
  assert(socket != NULL);

  /* Check that the socket is really shutdowned */
  if (socket->m_fd != -1)
    GSocket_Shutdown(socket);

  /* We destroy GUI specific variables */
  _GSocket_GUI_Destroy(socket);

  /* Destroy private addresses */
  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  /* Destroy the socket itself */
  free(socket);
}

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket_Shutdown(GSocket *socket)
{
  int evt;

  assert(socket != NULL);

  /* If socket has been created, shutdown it */
  if (socket->m_fd != -1)
  {
    shutdown(socket->m_fd, 2);
    close(socket->m_fd);
    socket->m_fd = -1;
  }

  /* Disable GUI callbacks */
  for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
    socket->m_cbacks[evt] = NULL;

  socket->m_detected = GSOCK_LOST_FLAG;
  _GSocket_Disable_Events(socket);
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
GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address)
{
  assert(socket != NULL);

  /* the socket must be initialized, or it must be a server */
  if ((socket->m_fd != -1 && !socket->m_server))
  {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* check address */
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    socket->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  socket->m_local = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address)
{
  assert(socket != NULL);

  /* check address */
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    socket->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  socket->m_peer = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GAddress *GSocket_GetLocal(GSocket *socket)
{
  GAddress *address;
  struct sockaddr addr;
  SOCKLEN_T size = sizeof(addr);
  GSocketError err;

  assert(socket != NULL);

  /* try to get it from the m_local var first */
  if (socket->m_local)
    return GAddress_copy(socket->m_local);

  /* else, if the socket is initialized, try getsockname */
  if (socket->m_fd == -1)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  if (getsockname(socket->m_fd, &addr, (SOCKLEN_T *) &size) < 0)
  {
    socket->m_error = GSOCK_IOERR;
    return NULL;
  }

  /* got a valid address from getsockname, create a GAddress object */
  address = GAddress_new();
  if (address == NULL)
  {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }

  err = _GAddress_translate_from(address, &addr, size);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(address);
    socket->m_error = err;
    return NULL;
  }

  return address;
}

GAddress *GSocket_GetPeer(GSocket *socket)
{
  assert(socket != NULL);

  /* try to get it from the m_peer var */
  if (socket->m_peer)
    return GAddress_copy(socket->m_peer);

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
GSocketError GSocket_SetServer(GSocket *sck)
{
  int type;
  int arg = 1;

  assert(sck != NULL);

  /* must not be in use */
  if (sck->m_fd != -1)
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* the local addr must have been set */
  if (!sck->m_local)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  sck->m_stream   = TRUE;
  sck->m_server   = TRUE;
  sck->m_oriented = TRUE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_STREAM, 0);

  if (sck->m_fd == -1)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, &arg);
  _GSocket_Enable_Events(sck);

  /* Bind to the local address,
   * retrieve the actual address bound,
   * and listen up to 5 connections.
   */
  if ((bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) != 0) ||
      (getsockname(sck->m_fd,
                   sck->m_local->m_addr,
                   (SOCKLEN_T *) &sck->m_local->m_len) != 0) ||
      (listen(sck->m_fd, 5) != 0))
  {
    close(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
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
GSocket *GSocket_WaitConnection(GSocket *socket)
{
  struct sockaddr from;
  SOCKLEN_T fromlen = sizeof(from);
  GSocket *connection;
  GSocketError err;
  int arg = 1;

  assert(socket != NULL);

  /* Reenable CONNECTION events */
  _GSocket_Enable(socket, GSOCK_CONNECTION);

  /* If the socket has already been created, we exit immediately */
  if (socket->m_fd == -1 || !socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  if (!connection)
  {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
  {
    GSocket_destroy(connection);
    /* socket->m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

  connection->m_fd = accept(socket->m_fd, &from, (SOCKLEN_T *) &fromlen);

  if (connection->m_fd == -1)
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;

    GSocket_destroy(connection);
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = FALSE;
  connection->m_stream   = TRUE;
  connection->m_oriented = TRUE;

  /* Setup the peer address field */
  connection->m_peer = GAddress_new();
  if (!connection->m_peer)
  {
    GSocket_destroy(connection);
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }
  err = _GAddress_translate_from(connection->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(connection->m_peer);
    GSocket_destroy(connection);
    socket->m_error = err;
    return NULL;
  }

  ioctl(connection->m_fd, FIONBIO, &arg);
  _GSocket_Enable_Events(connection);

  return connection;
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
GSocketError GSocket_SetNonOriented(GSocket *sck)
{
  int arg = 1;

  assert(sck != NULL);

  if (sck->m_fd != -1)
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_local)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  sck->m_stream   = FALSE;
  sck->m_server   = FALSE;
  sck->m_oriented = FALSE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_DGRAM, 0);

  if (sck->m_fd < 0)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, &arg);
  _GSocket_Enable_Events(sck);

  /* Bind to the local address,
   * and retrieve the actual address bound.
   */
  if ((bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) != 0) ||
      (getsockname(sck->m_fd,
                   sck->m_local->m_addr,
                   (SOCKLEN_T *) &sck->m_local->m_len) != 0))
  {
    close(sck->m_fd);
    sck->m_fd    = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Client specific parts */

/* GSocket_Connect:
 *  For stream (connection oriented) sockets, GSocket_Connect() tries
 *  to establish a client connection to a server using the peer address
 *  as established with GSocket_SetPeer(). Returns GSOCK_NOERROR if the
 *  connection has been succesfully established, or one of the error
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
GSocketError GSocket_Connect(GSocket *sck, GSocketStream stream)
{
  int err, ret;
  int arg = 1;

  assert(sck != NULL);

  /* Enable CONNECTION events (needed for nonblocking connections) */
  _GSocket_Enable(sck, GSOCK_CONNECTION);

  if (sck->m_fd != -1)
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_peer)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Streamed or dgram socket? */
  sck->m_stream   = (stream == GSOCK_STREAMED);
  sck->m_oriented = TRUE;
  sck->m_server   = FALSE;
  sck->m_establishing = FALSE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_peer->m_realfamily,
                     sck->m_stream? SOCK_STREAM : SOCK_DGRAM, 0);

  if (sck->m_fd == -1)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, &arg);
  _GSocket_Enable_Events(sck);

  /* Connect it to the peer address, with a timeout (see below) */
  ret = connect(sck->m_fd, sck->m_peer->m_addr, sck->m_peer->m_len);

  if (ret == -1)
  {
    err = errno;

    /* If connect failed with EINPROGRESS and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */
    if ((err == EINPROGRESS) && (!sck->m_non_blocking))
    {
      if (_GSocket_Output_Timeout(sck) == GSOCK_TIMEDOUT)
      {
        close(sck->m_fd);
        sck->m_fd = -1;
        /* sck->m_error is set in _GSocket_Output_Timeout */
        return GSOCK_TIMEDOUT;
      }
      else
      {
        int error;
        SOCKLEN_T len = sizeof(error);

        getsockopt(sck->m_fd, SOL_SOCKET, SO_ERROR, (void*) &error, &len);

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
    if ((err == EINPROGRESS) && (sck->m_non_blocking))
    {
      sck->m_establishing = TRUE;
      sck->m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EINPROGRESS,
     * then the call to GSocket_Connect has failed.
     */
    close(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket_Read(GSocket *socket, char *buffer, int size)
{
  int ret;

  assert(socket != NULL);

  /* Reenable INPUT events */
  _GSocket_Enable(socket, GSOCK_INPUT);

  if (socket->m_fd == -1 || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for data (with a timeout) */
  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  /* Read the data */
  if (socket->m_stream)
    ret = _GSocket_Recv_Stream(socket, buffer, size);
  else
    ret = _GSocket_Recv_Dgram(socket, buffer, size);
    
  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;
  }
  
  return ret;
}

int GSocket_Write(GSocket *socket, const char *buffer, int size)
{                        
  int ret;

  assert(socket != NULL);

  if (socket->m_fd == -1 || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (_GSocket_Output_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  /* Write the data */
  if (socket->m_stream)
    ret = _GSocket_Send_Stream(socket, buffer, size);
  else
    ret = _GSocket_Send_Dgram(socket, buffer, size);
    
  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    _GSocket_Enable(socket, GSOCK_OUTPUT);
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
GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags)
{
  assert(socket != NULL);

  return flags & socket->m_detected;
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket_SetNonBlocking(GSocket *socket, bool non_block)
{
  assert(socket != NULL);

  socket->m_non_blocking = non_block;
}

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec)
{
  assert(socket != NULL);

  socket->m_timeout = millisec;
}

/* GSocket_GetError:
 *  Returns the last error occured for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError GSocket_GetError(GSocket *socket)
{
  assert(socket != NULL);

  return socket->m_error;
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
 *   Connection succesfully established, for client sockets, or incoming
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
void GSocket_SetCallback(GSocket *socket, GSocketEventFlags flags,
                         GSocketCallback callback, char *cdata)
{
  int count;

  assert(socket != NULL);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = callback;
      socket->m_data[count] = cdata;
    }
  }
}

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags)
{
  int count;

  assert(socket != NULL);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = NULL;
      socket->m_data[count] = NULL;
    }
  }
}


#define CALL_CALLBACK(socket, event) {                                  \
  _GSocket_Disable(socket, event);                                      \
  if (socket->m_cbacks[event])                                          \
    socket->m_cbacks[event](socket, event, socket->m_data[event]);      \
}


void _GSocket_Enable(GSocket *socket, GSocketEvent event)
{
  socket->m_detected &= ~(1 << event);
  _GSocket_Install_Callback(socket, event);
}

void _GSocket_Disable(GSocket *socket, GSocketEvent event)
{
  socket->m_detected |= (1 << event);
  _GSocket_Uninstall_Callback(socket, event);
}

/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError _GSocket_Input_Timeout(GSocket *socket)
{
  struct timeval tv;
  fd_set readfds;

  /* Linux select() will overwrite the struct on return */
  tv.tv_sec  = (socket->m_timeout / 1000);
  tv.tv_usec = (socket->m_timeout % 1000) * 1000;

  if (!socket->m_non_blocking)
  {
    FD_ZERO(&readfds);
    FD_SET(socket->m_fd, &readfds);
    if (select(socket->m_fd + 1, &readfds, NULL, NULL, &tv) == 0)
    {
      socket->m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }
  }
  return GSOCK_NOERROR;
}

/* _GSocket_Output_Timeout:
 *  For blocking sockets, wait until data can be sent without
 *  blocking or until timeout ellapses.
 */
GSocketError _GSocket_Output_Timeout(GSocket *socket)
{
  struct timeval tv;
  fd_set writefds;

  /* Linux select() will overwrite the struct on return */
  tv.tv_sec  = (socket->m_timeout / 1000);
  tv.tv_usec = (socket->m_timeout % 1000) * 1000;

  if (!socket->m_non_blocking)
  {
    FD_ZERO(&writefds);
    FD_SET(socket->m_fd, &writefds);
    if (select(socket->m_fd + 1, NULL, &writefds, NULL, &tv) == 0)
    {
      socket->m_error = GSOCK_TIMEDOUT;
      return GSOCK_TIMEDOUT;
    }
  }
  return GSOCK_NOERROR;
}

int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size)
{
  return recv(socket->m_fd, buffer, size, 0);
}

int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size)
{
  struct sockaddr from;
  SOCKLEN_T fromlen = sizeof(from);
  int ret;
  GSocketError err;

  fromlen = sizeof(from);

  ret = recvfrom(socket->m_fd, buffer, size, 0, &from, (SOCKLEN_T *) &fromlen);

  if (ret == -1)
    return -1;

  /* Translate a system address into a GSocket address */
  if (!socket->m_peer)
  {
    socket->m_peer = GAddress_new();
    if (!socket->m_peer)
    {
      socket->m_error = GSOCK_MEMERR;
      return -1;
    }
  }
  err = _GAddress_translate_from(socket->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(socket->m_peer);
    socket->m_peer  = NULL;
    socket->m_error = err;
    return -1;
  }

  return ret;
}

int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size)
{
  int ret;

  MASK_SIGNAL();
  ret = send(socket->m_fd, buffer, size, 0);
  UNMASK_SIGNAL();

  return ret;
}

int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;
  GSocketError err;

  if (!socket->m_peer)
  {
    socket->m_error = GSOCK_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(socket->m_peer, &addr, &len);
  if (err != GSOCK_NOERROR)
  {
    socket->m_error = err;
    return -1;
  }

  MASK_SIGNAL();
  ret = sendto(socket->m_fd, buffer, size, 0, addr, len);
  UNMASK_SIGNAL();

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void _GSocket_Detected_Read(GSocket *socket)
{
  char c;
  int ret;

  ret = recv(socket->m_fd, &c, 1, MSG_PEEK);

  if (socket->m_stream)
  {
    if (ret < 0 && socket->m_server)
    {
      CALL_CALLBACK(socket, GSOCK_CONNECTION);
      return;
    }
  }

  if (ret > 0)
  {
    CALL_CALLBACK(socket, GSOCK_INPUT);
  }
  else
  {
    CALL_CALLBACK(socket, GSOCK_LOST);
  }
}

void _GSocket_Detected_Write(GSocket *socket)
{
  if (socket->m_establishing && !socket->m_server)
  {
    int error;
    SOCKLEN_T len = sizeof(error);

    socket->m_establishing = FALSE;

    getsockopt(socket->m_fd, SOL_SOCKET, SO_ERROR, (void*) &error, &len);

    if (error)
    {
      CALL_CALLBACK(socket, GSOCK_LOST);
    }
    else
    {
      CALL_CALLBACK(socket, GSOCK_CONNECTION);
      /* We have to fire this event by hand because CONNECTION (for clients)
       * and OUTPUT are internally the same and we just disabled CONNECTION
       * events with the above macro.
       */
      CALL_CALLBACK(socket, GSOCK_OUTPUT);
    }
  }
  else
  {
    CALL_CALLBACK(socket, GSOCK_OUTPUT);
  }
}

/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current family is either GSOCK_NOFAMILY
 * or GSOCK_*family*, and if it is GSOCK_NOFAMILY, it initalizes address
 * to be a GSOCK_*family*. In other cases, it returns GSOCK_INVADDR.
 */
#define CHECK_ADDRESS(address, family, retval)                      \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}

GAddress *GAddress_new()
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

  if (address->m_addr)
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
#ifdef AF_INET6
    case AF_INET6:
      address->m_family = GSOCK_INET6;
      break;
#endif
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
  if (*addr == NULL) {
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

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  /* If it is a numeric host name, convert it now */
#if defined(HAVE_INET_ATON)
  if (inet_aton(hostname, addr) == 0)
  {
#elif defined(HAVE_INET_ADDR)
  if ( (addr->s_addr = inet_addr(hostname)) == -1 )
  {
#else
  /* Use gethostbyname by default */
  if (1)
  {
#endif
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    if ((he = gethostbyname(hostname)) == NULL)
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

GSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);
  addr->s_addr = hostaddr;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

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
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);
 
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
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin_addr), AF_INET);
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
  CHECK_ADDRESS(address, INET, 0); 

  addr = (struct sockaddr_in *)address->m_addr;

  return addr->sin_addr.s_addr;
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

  assert(address != NULL); 
  CHECK_ADDRESS(address, INET, 0); 

  addr = (struct sockaddr_in *)address->m_addr;
  return ntohs(addr->sin_port);
}

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

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

  return TRUE;
}

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
  struct sockaddr_un *addr;

  assert(address != NULL); 

  CHECK_ADDRESS(address, UNIX, GSOCK_INVADDR); 

  addr = ((struct sockaddr_un *)address->m_addr);
  memcpy(addr->sun_path, path, strlen(path));

  return GSOCK_NOERROR;
}

GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
  struct sockaddr_un *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, UNIX, GSOCK_INVADDR);

  addr = (struct sockaddr_un *)address->m_addr;

  strncpy(path, addr->sun_path, sbuf);

  return GSOCK_NOERROR;
}

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

