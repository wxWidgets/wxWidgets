/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsocket.c
 * Purpose: GSocket main Unix file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <assert.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#ifdef vms
#include <socket.h>
#else
#include <sys/socket.h>
#endif
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef sun
#    include <sys/filio.h>
#endif

#ifdef sgi
#    include <bstring.h>
#endif

#include <signal.h>

#include "wx/gsocket.h"
#include "gsockunx.h"

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

  socket->m_fd                  = -1;
  for (i=0;i<GSOCK_MAX_EVENT;i++) {
    socket->m_fbacks[i]         = NULL;
    socket->m_iocalls[i]        = FALSE;
  }
  socket->m_local               = NULL;
  socket->m_peer                = NULL;
  socket->m_error               = GSOCK_NOERROR;
  socket->m_server		= FALSE;
  socket->m_stream		= TRUE;
  socket->m_gui_dependent	= NULL;
  socket->m_blocking		= FALSE;

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
  if (socket->m_fd != -1) {
    shutdown(socket->m_fd, 2);
    close(socket->m_fd);
    socket->m_fd = -1;
  }

  /* We also disable GUI callbacks */
  for (evt=0;evt<GSOCK_MAX_EVENT;evt++)
    _GSocket_Uninstall_Fallback(socket, evt);
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
  _GAddress_translate_from(address, &addr, size);

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

/*
  GSocket_SetServer() setup the socket as a server. It uses the "Local" field
  of GSocket. "Local" must be set by GSocket_SetLocal() before
  GSocket_SetServer() is called. GSOCK_INVSOCK if socket has been initialized.
  In case, you haven't yet defined the local address, it returns GSOCK_INVADDR.
  In the other cases it returns GSOCK_IOERR.
*/
GSocketError GSocket_SetServer(GSocket *sck)
{
  int type;

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

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_STREAM, 0);

  if (sck->m_fd == -1) {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* Bind the socket to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) < 0) {
    close(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* Enable listening up to 5 connections */
  if (listen(sck->m_fd, 5) < 0) {
    close(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/*
  GSocket_WaitConnection() waits for an incoming client connection.
*/
GSocket *GSocket_WaitConnection(GSocket *socket)
{
  GSocket *connection;

  assert(socket != NULL);

  /* If the socket has already been created, we exit immediately */
  if (socket->m_fd == -1 || !socket->m_server) {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Reenable GSOCK_CONNECTION event */
  _GSocket_Enable(socket, GSOCK_CONNECTION);

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  /* Accept the incoming connection */
  connection->m_fd = accept(socket->m_fd, NULL, NULL);
  if (connection->m_fd == -1) {
    GSocket_destroy(connection);
    socket->m_error = GSOCK_IOERR;
    return NULL;
  }

  /* Initialize all fields */
  connection->m_stream   = TRUE;
  connection->m_server   = FALSE;
  connection->m_oriented = TRUE;

  return connection;
}

/* Non oriented connections */

GSocketError GSocket_SetNonOriented(GSocket *sck)
{
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

  /* Bind it to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) < 0) {
    close(sck->m_fd);
    sck->m_fd    = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Client specific parts */

/*
  GSocket_Connect() establishes a client connection to a server using the "Peer"
  field of GSocket. "Peer" must be set by GSocket_SetPeer() before
  GSocket_Connect() is called. In the other case, it returns GSOCK_INVADDR.
*/
GSocketError GSocket_Connect(GSocket *sck, GSocketStream stream)
{
  int type;

  assert(sck != NULL);

  if (sck->m_fd != -1) {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_peer) {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Test whether we want the socket to be a stream (e.g. TCP) */
  sck->m_stream = (stream == GSOCK_STREAMED);
  sck->m_oriented = TRUE;

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

  /* Connect it to the PEER address */
  if (connect(sck->m_fd, sck->m_peer->m_addr,
              sck->m_peer->m_len) != 0) {
    close(sck->m_fd);
    sck->m_fd = -1;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
  
  /* It is not a server */
  sck->m_server = FALSE;

  return GSOCK_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket_Read(GSocket *socket, char *buffer, int size)
{
  assert(socket != NULL);

  if (socket->m_fd == -1 || socket->m_server) {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* Reenable GSOCK_INPUT event */
  _GSocket_Enable(socket, GSOCK_INPUT);

  if (socket->m_stream)
    return _GSocket_Recv_Stream(socket, buffer, size);
  else
    return _GSocket_Recv_Dgram(socket, buffer, size);
}

int GSocket_Write(GSocket *socket, const char *buffer,
		  int size)
{
  assert(socket != NULL);

  if (socket->m_fd == -1 || socket->m_server) {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  _GSocket_Enable(socket, GSOCK_OUTPUT);

  if (socket->m_stream)
    return _GSocket_Send_Stream(socket, buffer, size);
  else
    return _GSocket_Send_Dgram(socket, buffer, size);
}

bool GSocket_DataAvailable(GSocket *socket)
{
  fd_set read_set;
  struct timeval tv;

  assert(socket != NULL);

  if (socket->m_fd == -1 || socket->m_server) {
    socket->m_error = GSOCK_INVSOCK;
    return FALSE;
  }

  FD_ZERO(&read_set);
  FD_SET(socket->m_fd, &read_set);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  select(socket->m_fd+1, &read_set, NULL, NULL, &tv);

  return FD_ISSET(socket->m_fd, &read_set);
}

/* Flags */

/*
  GSocket_SetBlocking() puts the socket in non-blocking mode. This is useful
  if we don't want to wait.
*/
void GSocket_SetBlocking(GSocket *socket, bool block)
{
  assert(socket != NULL);

  socket->m_blocking = block;

  if (socket->m_fd != -1)
    ioctl(socket->m_fd, FIONBIO, &block);
}

/*
  GSocket_GetError() returns the last error occured on the socket stream.
*/

GSocketError GSocket_GetError(GSocket *socket)
{
  assert(socket != NULL);

  return socket->m_error;
}

/* Callbacks */

/* 
   Only one fallback is possible for each event (INPUT, OUTPUT, CONNECTION)
   INPUT: The function is called when there is at least a byte in the 
          input buffer
   OUTPUT: The function is called when the system is sure the next write call
           will not block
   CONNECTION: Two cases is possible:
             Client socket -> the connection is established
	     Server socket -> a client request a connection
   LOST: the connection is lost

   SetFallback accepts a combination of these flags so a same callback can
   receive different events.

   An event is generated only once and its state is reseted when the relative
   IO call is requested.
   For example: INPUT -> GSocket_Read()
                CONNECTION -> GSocket_Accept()
*/
void GSocket_SetFallback(GSocket *socket, GSocketEventFlags event,
			 GSocketFallback fallback, char *cdata)
{
  int count;

  assert (socket != NULL);

  for (count=0;count<GSOCK_MAX_EVENT;count++) {
    /* We test each flag and, if it is enabled, we enable the corresponding
       event */
    if ((event & (1 << count)) != 0) {
      socket->m_fbacks[count] = fallback;
      socket->m_data[count] = cdata;

      _GSocket_Install_Fallback(socket, count);
      _GSocket_Enable(socket, count);
    }
  }
}

/*
  UnsetFallback will disables all fallbacks specified by "event".
  NOTE: event may be a combination of flags
*/
void GSocket_UnsetFallback(GSocket *socket, GSocketEventFlags event)
{
  int count = 0;

  assert(socket != NULL);

  for (count=0;count<GSOCK_MAX_EVENT;count++) {
    if ((event & (1 << count)) != 0) {
      _GSocket_Disable(socket, count);
      socket->m_fbacks[count] = NULL;
      _GSocket_Uninstall_Fallback(socket, count);
    }
  }
}

#define CALL_FALLBACK(socket, event) \
if (socket->m_iocalls[event] && \
    socket->m_fbacks[event]) {\
  _GSocket_Disable(socket, event); \
  socket->m_fbacks[event](socket, event, \
                   socket->m_data[event]); \
}

#define MASK_SIGNAL() \
{ \
  void (*old_handler)(int); \
\
  old_handler = signal(SIGPIPE, SIG_IGN);

#define UNMASK_SIGNAL() \
  signal(SIGPIPE, old_handler); \
}

void _GSocket_Enable(GSocket *socket, GSocketEvent event)
{
  socket->m_iocalls[event] = TRUE;
  if (socket->m_fbacks[event])
    _GSocket_Install_Fallback(socket, event);
}

void _GSocket_Disable(GSocket *socket, GSocketEvent event)
{
  socket->m_iocalls[event] = FALSE;
  if (socket->m_fbacks[event])
    _GSocket_Uninstall_Fallback(socket, event);
}

int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size)
{
  int ret;

  MASK_SIGNAL();
  ret = recv(socket->m_fd, buffer, size, 0);
  UNMASK_SIGNAL();
  if (ret == -1) {
    socket->m_error = GSOCK_IOERR;
    return -1;
  }
  return ret;
}

int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size)
{
  struct sockaddr from;
  SOCKLEN_T fromlen;
  int ret;

  fromlen = sizeof(from);

  MASK_SIGNAL();
  ret = recvfrom(socket->m_fd, buffer, size, 0, &from, &fromlen);
  UNMASK_SIGNAL();
  if (ret == -1) {
    socket->m_error = GSOCK_IOERR;
    return -1;
  }

  /* Translate a system address into a GSocket address */
  if (!socket->m_peer)
    socket->m_peer = GAddress_new();
  _GAddress_translate_from(socket->m_peer, &from, fromlen);

  return ret;
}

int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size)
{
  int ret;

  MASK_SIGNAL();
  ret = send(socket->m_fd, buffer, size, 0);
  UNMASK_SIGNAL();
  if (ret == -1) {
    socket->m_error = GSOCK_IOERR;
    return -1;
  }
  return ret;
}

int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;

  if (!socket->m_peer) {
    socket->m_error = GSOCK_INVADDR;
    return -1;
  }

  _GAddress_translate_to(socket->m_peer, &addr, &len);

  MASK_SIGNAL();
  ret = sendto(socket->m_fd, buffer, size, 0, addr, len);
  UNMASK_SIGNAL();
  if (ret == -1) {
    socket->m_error = GSOCK_IOERR;
    return -1;
  }

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void _GSocket_Detected_Read(GSocket *socket)
{
  char c;
  int ret;

  if (socket->m_stream) {
    ret = recv(socket->m_fd, &c, 1, MSG_PEEK);

    if (ret < 0 && socket->m_server) {
      CALL_FALLBACK(socket, GSOCK_CONNECTION);
      return;
    }

    if (ret > 0) {
      CALL_FALLBACK(socket, GSOCK_INPUT);
    } else {
      CALL_FALLBACK(socket, GSOCK_LOST);
    }
  }
}

void _GSocket_Detected_Write(GSocket *socket)
{
  CALL_FALLBACK(socket, GSOCK_OUTPUT);
}

#undef CALL_FALLBACK 
#undef MASK_SIGNAL
#undef UNMASK_SIGNAL

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
    _GAddress_Init_##family(address); \
  if (address->m_family != GSOCK_##family) {\
    address->m_error = GSOCK_INVADDR; \
    return retval; \
  } \
}

GAddress *GAddress_new()
{
  GAddress *address;

  address = (GAddress *)malloc(sizeof(GAddress));

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
  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr) {
    addr2->m_addr = (struct sockaddr *)malloc(addr2->m_len);
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

void _GAddress_translate_from(GAddress *address, struct sockaddr *addr, int len){
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
    /* TODO error */
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len  = len;
  address->m_addr = (struct sockaddr *)malloc(len);
  memcpy(address->m_addr, addr, len);
}

void _GAddress_translate_to(GAddress *address,
                            struct sockaddr **addr, int *len)
{
  if (!address->m_addr) {
    /* TODO error */
    return;
  }

  *len = address->m_len;
  *addr = (struct sockaddr *)malloc(address->m_len);
  memcpy(*addr, address->m_addr, address->m_len);
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

void _GAddress_Init_INET(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_in);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  address->m_family = GSOCK_INET;
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr   = INADDR_ANY;
}

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  /* If it is a numeric host name, convert it now */
  if (inet_aton(hostname, addr) == 0) {
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    if ((he = gethostbyname(hostname)) == NULL) {
      address->m_error = GSOCK_NOHOST;
      return GSOCK_NOHOST;
    }
    array_addr = (struct in_addr *) *(he->h_addr_list);
    addr->s_addr = array_addr[0].s_addr;
  }
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
    return GSOCK_INVOP;
  }
 
  se = getservbyname(port, protocol);
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

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

void _GAddress_Init_UNIX(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_un);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  address->m_family = GSOCK_UNIX;
  address->m_realfamily = PF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_family = AF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_path[0] = 0;
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

#endif // wxUSE_SOCKETS
