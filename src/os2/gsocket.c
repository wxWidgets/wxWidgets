/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsocket.c
 * Purpose: GSocket main Unix-style file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#include "wx/setup.h"
#include "wx/defs.h"

#if wxUSE_SOCKETS

#define BSD_SELECT /* use Berkley Sockets select */

#include <assert.h>
#include <sys\types.h>
#ifdef __EMX__
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#define HAVE_INET_ADDR
#else
#include <utils.h>
#include <sys\time.h>
#include <in.h>
#include <netdb.h>
#include <nerrno.h>
#endif
#if defined(__VISAGECPP__) && __IBMCPP__ < 400
#include <socket.h>
#include <ioctl.h>
#include <select.h>
#else
#include <sys\socket.h>
#include <sys\ioctl.h>
#include <sys\select.h>
#ifndef __EMX__
#define select(a,b,c,d,e) bsdselect(a,b,c,d,e)
int _System bsdselect(int,
                      struct fd_set *,
                      struct fd_set *,
                      struct fd_set *,
                      struct timeval *);
int _System soclose(int);
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <signal.h>

#include "wx/gsocket.h"
#include "wx/os2/gsockos2.h"

#ifndef SOCKLEN_T

#ifdef __GLIBC__
#      if __GLIBC__ == 2
#         define SOCKLEN_T socklen_t
#      endif
#else
#      define SOCKLEN_T int
#endif

#endif

/* Global initialisers */

bool GSocket_Init()
{
  return TRUE;
}

void GSocket_Cleanup()
{
}

/* Constructors / Destructors */

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

  /* First, we check that the socket is really shutdowned */
  if (socket->m_fd != -1)
    GSocket_Shutdown(socket);

  /* We destroy GUI specific variables */
  _GSocket_GUI_Destroy(socket);

  /* We destroy private addresses */
  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  /* We destroy socket itself */
  free(socket);
}

void GSocket_Shutdown(GSocket *socket)
{
  int evt;

  assert(socket != NULL);

  /* If socket has been created, we shutdown it */
  if (socket->m_fd != -1)
  {
    shutdown(socket->m_fd, 2);
    soclose(socket->m_fd);
    socket->m_fd = -1;
  }

  /* We also disable GUI callbacks */
  for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
    socket->m_cbacks[evt] = NULL;

  socket->m_detected = 0;
  _GSocket_Disable_Events(socket);
}

/* Address handling */

GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address)
{
  assert(socket != NULL);

  if ((socket->m_fd != -1 && !socket->m_server)) {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (address == NULL || address->m_family == GSOCK_NOFAMILY) {
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

  if (address == NULL || address->m_family == GSOCK_NOFAMILY) {
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
  SOCKLEN_T size;
  GSocketError err;

  assert(socket != NULL);

  if (socket->m_local)
    return GAddress_copy(socket->m_local);

  if (socket->m_fd == -1) {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  size = sizeof(addr);

  if (getsockname(socket->m_fd, &addr, &size) < 0) {
    socket->m_error = GSOCK_IOERR;
    return NULL;
  }

  address = GAddress_new();
  if (address == NULL) {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }
  socket->m_error = _GAddress_translate_from(address, &addr, size);
  if (socket->m_error != GSOCK_NOERROR) {
    GAddress_destroy(address);
    return NULL;
  }

  return address;
}

GAddress *GSocket_GetPeer(GSocket *socket)
{
  assert(socket != NULL);

  if (socket->m_peer)
    return GAddress_copy(socket->m_peer);

  return NULL;
}

/* Server specific parts */

/* GSocket_SetServer:
 *  Sets up the socket as a server. It uses the "Local" field of GSocket.
 *  "Local" must be set by GSocket_SetLocal() before GSocket_SetServer()
 *  is called. Possible error codes are: GSOCK_INVSOCK if socket has not
 *  been initialized, GSOCK_INVADDR if the local address has not been
 *  defined and GSOCK_IOERR for other internal errors.
 */
GSocketError GSocket_SetServer(GSocket *sck)
{
  int type;
  int arg = 1;

  assert(sck != NULL);

  if (sck->m_fd != -1) {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_local) {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* We always have a stream here  */
  sck->m_stream = TRUE;
  sck->m_server = TRUE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_STREAM, 0);

  if (sck->m_fd == -1) {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, (char*)&arg, sizeof(int));
  _GSocket_Enable_Events(sck);

  /* Bind the socket to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) < 0) {
    soclose(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* Enable listening up to 5 connections */
  if (listen(sck->m_fd, 5) < 0) {
    soclose(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* GSocket_WaitConnection:
 *  Waits for an incoming client connection.
 */
GSocket *GSocket_WaitConnection(GSocket *socket)
{
  GSocket *connection;
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
    connection->m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Accept the incoming connection */
  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
  {
    GSocket_destroy(connection);
    /* socket->m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

  connection->m_fd = accept(socket->m_fd, NULL, NULL);

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

  ioctl(connection->m_fd, FIONBIO, (char*)&arg, sizeof(int));
  _GSocket_Enable_Events(connection);

  return connection;
}

/* Non oriented connections */

GSocketError GSocket_SetNonOriented(GSocket *sck)
{
  int arg = 1;

  assert(sck != NULL);

  if (sck->m_fd != -1) {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_local) {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  sck->m_stream   = FALSE;
  sck->m_server   = FALSE;
  sck->m_oriented = FALSE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_DGRAM, 0);

  if (sck->m_fd < 0) {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, (char*)&arg, sizeof(int));
  _GSocket_Enable_Events(sck);

  /* Bind it to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) < 0) {
    soclose(sck->m_fd);
    sck->m_fd    = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Client specific parts */

/* GSocket_Connect:
 *  Establishes a client connection to a server using the "Peer"
 *  field of GSocket. "Peer" must be set by GSocket_SetPeer() before
 *  GSocket_Connect() is called. Possible error codes are GSOCK_INVSOCK,
 *  GSOCK_INVADDR, GSOCK_TIMEDOUT, GSOCK_WOULDBLOCK and GSOCK_IOERR.
 *  If a socket is nonblocking and Connect() returns GSOCK_WOULDBLOCK,
 *  the connection request can be completed later. Use GSocket_Select()
 *  to check or wait for a GSOCK_CONNECTION event.
 */
GSocketError GSocket_Connect(GSocket *sck, GSocketStream stream)
{
  int type, err, ret;
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

  /* Test whether we want the socket to be a stream (e.g. TCP) */
  sck->m_stream   = (stream == GSOCK_STREAMED);
  sck->m_oriented = TRUE;
  sck->m_server   = FALSE;
  sck->m_establishing = FALSE;

  if (sck->m_stream)
    type = SOCK_STREAM;
  else
    type = SOCK_DGRAM;

  /* Create the socket */
  sck->m_fd = socket(sck->m_peer->m_realfamily, type, 0);

  if (sck->m_fd == -1) {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_fd, FIONBIO, (char*)&arg, sizeof(int));
  _GSocket_Enable_Events(sck);

  /* Connect it to the PEER address */
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
        soclose(sck->m_fd);
        sck->m_fd = -1;
        /* sck->m_error is set in _GSocket_Output_Timeout */
        fprintf(stderr, "Blocking connect timeouts\n");
        return GSOCK_TIMEDOUT;
      }
      else
      {
        fprintf(stderr, "Blocking connect OK\n");
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
      sck->m_error = GSOCK_WOULDBLOCK;
      sck->m_establishing = TRUE;
      fprintf(stderr, "Nonblocking connect in progress\n");

      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EINPROGRESS,
     * then the call to GSocket_Connect has failed.
     */
    soclose(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;

    fprintf(stderr, "Connect failed (generic err)\n");
    return GSOCK_IOERR;
  }

  fprintf(stderr, "Connect OK\n");
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

  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

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

int GSocket_Write(GSocket *socket, const char *buffer,
		  int size)
{
  int ret;

  assert(socket != NULL);

  if (socket->m_fd == -1 || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  if (_GSocket_Output_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

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
  }

  return ret;
}

/* GSocket_Select:
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking|nonblocking) of the socket.
 */
GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags)
{
  assert(socket != NULL);

  return (flags & socket->m_detected);
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. This is useful if
 *  we don't want to wait.
 */
void GSocket_SetNonBlocking(GSocket *socket, bool non_block)
{
  assert(socket != NULL);

  socket->m_non_blocking = non_block;
}

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is
 *  expressed in milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec)
{
  assert(socket != NULL);

  socket->m_timeout = millisec;
}

/* GSocket_GetError:
 *  Returns the last error occured for this socket.
 */
GSocketError GSocket_GetError(GSocket *socket)
{
  assert(socket != NULL);

  return socket->m_error;
}

/* Callbacks */

/* Only one callback is possible for each event (INPUT, OUTPUT, CONNECTION
 * and LOST). The callbacks are called in the following situations:
 *
 * INPUT: There is at least one byte in the input buffer
 * OUTPUT: The system is sure that the next write call will not block
 * CONNECTION: Two cases are possible:
 *           Client socket -> the connection is established
 *           Server socket -> a client requests a connection
 * LOST: The connection is lost
 *
 * An event is generated only once and its state is reseted when the
 * relative IO call is requested.
 * For example: INPUT -> GSocket_Read()
 *              CONNECTION -> GSocket_Accept()
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
  int ret;

  ret = recv(socket->m_fd, buffer, size, 0);

  return ret;
}

int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size)
{
  struct sockaddr from;
  SOCKLEN_T fromlen;
  int ret;
  GSocketError err;

  fromlen = sizeof(from);

  ret = recvfrom(socket->m_fd, buffer, size, 0, &from, &fromlen);

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
  GSocketError err;

  ret = send(socket->m_fd, (char*)buffer, size, 0);

  return ret;
}

int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;
  GSocketError err;

  if (!socket->m_peer) {
    socket->m_error = GSOCK_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(socket->m_peer, &addr, &len);
  if (err != GSOCK_NOERROR) {
    socket->m_error = err;
    return -1;
  }

  ret = sendto(socket->m_fd, (char*)buffer, size, 0, addr, len);

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void _GSocket_Detected_Read(GSocket *socket)
{
  char c;
  int ret;

  if (socket->m_stream)
  {
    ret = recv(socket->m_fd, &c, 1, MSG_PEEK);

    if (ret < 0 && socket->m_server)
    {
      CALL_CALLBACK(socket, GSOCK_CONNECTION);
      return;
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
}

void _GSocket_Detected_Write(GSocket *socket)
{
  if (socket->m_establishing && !socket->m_server)
  {
    int error, len;

    socket->m_establishing = FALSE;

    len = sizeof(error);
    getsockopt(socket->m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

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

/* CHECK_ADDRESS verifies that the current family is either GSOCK_NOFAMILY or
 * GSOCK_*family*. In case it is GSOCK_NOFAMILY, it initializes address to be
 * a GSOCK_*family*. In other cases, it returns GSOCK_INVADDR.
 */
#define CHECK_ADDRESS(address, family, retval) \
{ \
  if (address->m_family == GSOCK_NOFAMILY) \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR) {\
      return address->m_error; \
    }\
  if (address->m_family != GSOCK_##family) {\
    address->m_error = GSOCK_INVADDR; \
    return retval; \
  } \
}

GAddress *GAddress_new()
{
  GAddress *address;

  address = (GAddress *)malloc(sizeof(GAddress));

  if (address == NULL)
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

  addr2 = (GAddress *)malloc(sizeof(GAddress));

  if (addr2 == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr) {
    addr2->m_addr = (struct sockaddr *)malloc(addr2->m_len);
    if (addr2->m_addr == NULL) {
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

GSocketError _GAddress_translate_from(GAddress *address, struct sockaddr *addr, int len){
  address->m_realfamily = addr->sa_family;
  switch (addr->sa_family) {
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
  if (address->m_addr == NULL) {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }
  memcpy(address->m_addr, addr, len);

  return GSOCK_NOERROR;
}

GSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len)
{
  if (!address->m_addr) {
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
  address->m_addr = (struct sockaddr *)malloc(sizeof(struct sockaddr_in));
  if (address->m_addr == NULL) {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_len  = sizeof(struct sockaddr_in);

  address->m_family = GSOCK_INET;
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr   = INADDR_ANY;

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
  if (inet_aton(hostname, addr) == 0) {
#elif defined(HAVE_INET_ADDR)
  /* Fix from Guillermo Rodriguez Garcia <guille@iies.es> */
  if ( (addr->s_addr = inet_addr(hostname)) == -1 ) {
#endif
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    he = gethostbyname((char*)hostname);
    if (he == NULL) {
      address->m_error = GSOCK_NOHOST;
      return GSOCK_NOHOST;
    }
    array_addr = (struct in_addr *) *(he->h_addr_list);
    addr->s_addr = array_addr[0].s_addr;
#if defined(HAVE_INET_ATON)
  }
#elif defined(HAVE_INET_ADDR)
  }
#endif
  return GSOCK_NOERROR;
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

  if (!port) {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  se = getservbyname((char*)port, (char*)protocol);
  if (!se) {
    if (isdigit(port[0])) {
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

  he       = gethostbyaddr(addr_buf, sizeof(addr->sin_addr), AF_INET);
  if (he == NULL) {
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

#endif
  /* wxUSE_SOCKETS */
