/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsocket.c
 * Purpose: GSocket main MSW file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */


#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)


#ifndef __GSOCKET_STANDALONE__

#include "wx/msw/gsockmsw.h"
#include "wx/gsocket.h"

#define INSTANCE wxhInstance

#else

#include "gsockmsw.h"
#include "gsocket.h"

/* If not using wxWindows, a global var called hInst must
 * be available and it must containt the app's instance
 * handle.
 */
#define INSTANCE hInst

#endif /* __GSOCKET_STANDALONE__ */


#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <winsock.h>

#define SOCKLEN_T  int
#define CLASSNAME  "_GSocket_Internal_Window_Class"
#define WINDOWNAME "_GSocket_Internal_Window_Name"

/* Maximum number of different GSocket objects at a given time.
 * This value can be modified at will, but it CANNOT be greater
 * than (0x7FFF - WM_USER + 1)
 */
#define MAXSOCKETS 1024

#if (MAXSOCKETS > (0x7FFF - WM_USER + 1))
#error "MAXSOCKETS is too big!"
#endif


/* Global variables */

extern HINSTANCE INSTANCE;
static HWND hWin;
static CRITICAL_SECTION critical;
static GSocket* socketList[MAXSOCKETS];
static int firstAvailable;

/* Global initializers */

bool GSocket_Init()
{
  WSADATA wsaData;
  WNDCLASS winClass;
  int i;

  /* Create internal window for event notifications */
  winClass.style         = 0;
  winClass.lpfnWndProc   = _GSocket_Internal_WinProc;
  winClass.cbClsExtra    = 0;
  winClass.cbWndExtra    = 0;
  winClass.hInstance     = INSTANCE;
  winClass.hIcon         = (HICON) NULL;
  winClass.hCursor       = (HCURSOR) NULL;
  winClass.hbrBackground = (HBRUSH) NULL;
  winClass.lpszMenuName  = (LPCTSTR) NULL;
  winClass.lpszClassName = CLASSNAME;

  RegisterClass(&winClass);
  hWin = CreateWindow(CLASSNAME,
                      WINDOWNAME,
                      0, 0, 0, 0, 0,
                      (HWND) NULL, (HMENU) NULL, INSTANCE, (LPVOID) NULL);

  if (!hWin) return FALSE;

  /* Initialize socket list */
  InitializeCriticalSection(&critical);

  for (i = 0; i < MAXSOCKETS; i++)
  {
    socketList[i] = NULL;
  }
  firstAvailable = 0;

  /* Initialize WinSocket */
  return (WSAStartup((1 << 8) | 1, &wsaData) == 0);
}

void GSocket_Cleanup()
{
  /* Destroy internal window */
  DestroyWindow(hWin);
  UnregisterClass(CLASSNAME, INSTANCE);

  /* Delete critical section */
  DeleteCriticalSection(&critical);

  /* Cleanup WinSocket */
  WSACleanup();
}

/* Constructors / Destructors */

GSocket *GSocket_new()
{
  int i;
  GSocket *socket;

  if ((socket = (GSocket *) malloc(sizeof(GSocket))) == NULL)
    return NULL;

  socket->m_fd              = INVALID_SOCKET;
  for (i = 0; i < GSOCK_MAX_EVENT; i++)
  {
    socket->m_cbacks[i]     = NULL;
  }
  socket->m_local           = NULL;
  socket->m_peer            = NULL;
  socket->m_error           = GSOCK_NOERROR;
  socket->m_server          = FALSE;
  socket->m_stream          = TRUE;
  socket->m_non_blocking    = FALSE;
  socket->m_timeout.tv_sec  = 10 * 60;  /* 10 minutes */
  socket->m_timeout.tv_usec = 0;

  /* Allocate a new message number for this socket */
  EnterCriticalSection(&critical);

  i = firstAvailable;
  while (socketList[i] != NULL)
  {
    i = (i + 1) % MAXSOCKETS;

    if (i == firstAvailable)    /* abort! */
    {
      free(socket);
      LeaveCriticalSection(&critical);
      return NULL;
    }
  }
  socketList[i] = socket;
  firstAvailable = (i + 1) % MAXSOCKETS;
  socket->m_msgnumber = (i + WM_USER);

  LeaveCriticalSection(&critical);

  return socket;
}

void GSocket_destroy(GSocket *socket)
{
  assert(socket != NULL);

  /* We don't want more event notifications; so first of all we
   * remove the socket from the list (NOTE: we won't get a CLOSE
   * event for this socket if it wasn't closed before).
   */
  EnterCriticalSection(&critical);
  socketList[(socket->m_msgnumber - WM_USER)] = NULL;
  LeaveCriticalSection(&critical);

  /* We check that the socket is really shutdowned */
  if (socket->m_fd != INVALID_SOCKET)
    GSocket_Shutdown(socket);

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
  if (socket->m_fd != INVALID_SOCKET)
  {
    shutdown(socket->m_fd, 2);
    closesocket(socket->m_fd);
    socket->m_fd = INVALID_SOCKET;
  }

  /* We disable GUI callbacks */
  for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
  {
    socket->m_cbacks[evt] = NULL;
  }

  _GSocket_Configure_Callbacks(socket);
}

/* Address handling */

GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address)
{
  assert(socket != NULL);

  if (socket->m_fd != INVALID_SOCKET && !socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

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
  SOCKLEN_T size;

  assert(socket != NULL);

  if (socket->m_local)
    return GAddress_copy(socket->m_local);

  if (socket->m_fd == INVALID_SOCKET)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  size = sizeof(addr);

  if (getsockname(socket->m_fd, &addr, &size) == SOCKET_ERROR)
  {
    socket->m_error = GSOCK_IOERR;
    return NULL;
  }

  address = GAddress_new();
  if (address == NULL)
  {
     socket->m_error = GSOCK_MEMERR;
     return NULL;
  }
  if (_GAddress_translate_from(address, &addr, size) != GSOCK_NOERROR)
  {
     socket->m_error = GSOCK_MEMERR;
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
  u_long arg = 1;

  assert(sck != NULL);

  if (sck->m_fd != INVALID_SOCKET)
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
  sck->m_server   = TRUE;
  sck->m_stream   = TRUE;
  sck->m_oriented = TRUE;

  /* Create the socket */
  sck->m_fd = socket(sck->m_local->m_realfamily, SOCK_STREAM, 0);

  if (sck->m_fd == INVALID_SOCKET)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctlsocket(sck->m_fd, FIONBIO, (u_long FAR *) &arg);
  _GSocket_Configure_Callbacks(sck);

  /* Bind the socket to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) != 0)
  {
    closesocket(sck->m_fd);
    sck->m_fd = INVALID_SOCKET;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  /* Enable listening up to 5 connections */
  if (listen(sck->m_fd, 5) != 0)
  {
    closesocket(sck->m_fd);
    sck->m_fd = INVALID_SOCKET;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* GSocket_WaitConnection:
 *  Waits for an incoming client connection.
 */
GSocket *GSocket_WaitConnection(GSocket *sck)
{
  GSocket *connection;
  u_long arg = 1;

  assert(sck != NULL);

  /* If the socket has already been created, we exit immediately */
  if (sck->m_fd == INVALID_SOCKET || !sck->m_server)
  {
    sck->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  if (!connection)
  {
    sck->m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (_GSocket_Input_Timeout(sck) == GSOCK_TIMEDOUT)
  {
    GSocket_destroy(connection);
    /* sck->m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

  connection->m_fd = accept(sck->m_fd, NULL, NULL);

  if (connection->m_fd == INVALID_SOCKET)
  {
    if (WSAGetLastError() == WSAEWOULDBLOCK)
      sck->m_error = GSOCK_WOULDBLOCK;
    else
      sck->m_error = GSOCK_IOERR;

    GSocket_destroy(connection);
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = FALSE;
  connection->m_stream   = TRUE;
  connection->m_oriented = TRUE;

  ioctlsocket(connection->m_fd, FIONBIO, (u_long FAR *) &arg);
  _GSocket_Configure_Callbacks(connection);

  return connection;
}

/* Non oriented connections */

GSocketError GSocket_SetNonOriented(GSocket *sck)
{
  u_long arg = 1;

  assert(sck != NULL);

  if (sck->m_fd != INVALID_SOCKET)
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

  if (sck->m_fd == INVALID_SOCKET)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctlsocket(sck->m_fd, FIONBIO, (u_long FAR *) &arg);
  _GSocket_Configure_Callbacks(sck);

  /* Bind it to the LOCAL address */
  if (bind(sck->m_fd, sck->m_local->m_addr, sck->m_local->m_len) != 0)
  {
    closesocket(sck->m_fd);
    sck->m_fd    = INVALID_SOCKET;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

GSocketError GSocket_SetBroadcast(GSocket *sck)
{
  BOOL b;

  assert(sck != NULL);

  if (GSocket_SetNonOriented(sck) != GSOCK_NOERROR)
    return sck->m_error;

  b = TRUE;
  setsockopt(sck->m_fd, SOL_SOCKET, SO_BROADCAST, (const char FAR *) &b, sizeof(b));

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
  u_long arg = 1;
  int type, ret, err;

  assert(sck != NULL);

  if (sck->m_fd != INVALID_SOCKET)
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

  if (sck->m_stream)
    type = SOCK_STREAM;
  else
    type = SOCK_DGRAM;

  /* Create the socket */
  sck->m_fd = socket(sck->m_peer->m_realfamily, type, 0);

  if (sck->m_fd == INVALID_SOCKET)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctlsocket(sck->m_fd, FIONBIO, (u_long FAR *) &arg);
  _GSocket_Configure_Callbacks(sck);

  /* Connect it to the PEER address, with a timeout (see below) */
  ret = connect(sck->m_fd, sck->m_peer->m_addr, sck->m_peer->m_len);

  if (ret == SOCKET_ERROR)
  {
    err = WSAGetLastError();

    /* If connect failed with EWOULDBLOCK and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */
    if ((err == WSAEWOULDBLOCK) && (!sck->m_non_blocking))
    {
      if (_GSocket_Output_Timeout(sck) == GSOCK_TIMEDOUT)
      {
        closesocket(sck->m_fd);
        sck->m_fd = INVALID_SOCKET;
        /* sck->m_error is set in _GSocket_Output_Timeout */
        return GSOCK_TIMEDOUT;
      }
      else
        return GSOCK_NOERROR;
    }

    /* If connect failed with EWOULDBLOCK and the GSocket object
     * is set to nonblocking, we set m_error to GSOCK_WOULDBLOCK
     * (and return GSOCK_WOULDBLOCK) but we don't close the socket;
     * this way if the connection completes, a GSOCK_CONNECTION
     * event will be generated, if enabled.
     */
    if ((err == WSAEWOULDBLOCK) && (sck->m_non_blocking))
    {
      sck->m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EWOULDBLOCK,
     * then the call to GSocket_Connect has failed.
     */
    closesocket(sck->m_fd);
    sck->m_fd = INVALID_SOCKET;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket_Read(GSocket *socket, char *buffer, int size)
{
  assert(socket != NULL);

  if (socket->m_fd == INVALID_SOCKET || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  if (socket->m_stream)
    return _GSocket_Recv_Stream(socket, buffer, size);
  else
    return _GSocket_Recv_Dgram(socket, buffer, size);
}

int GSocket_Write(GSocket *socket, const char *buffer, int size)
{
  assert(socket != NULL);

  if (socket->m_fd == INVALID_SOCKET || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  if (_GSocket_Output_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  if (socket->m_stream)
    return _GSocket_Send_Stream(socket, buffer, size);
  else
    return _GSocket_Send_Dgram(socket, buffer, size);
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
  fd_set readfds, writefds, exceptfds;
  struct timeval tv;
  GSocketEventFlags mask;

  assert(socket != NULL);

  if (socket->m_fd == INVALID_SOCKET)
  {
    socket->m_error = GSOCK_INVSOCK;
    return FALSE;
  }

  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_SET(socket->m_fd, &readfds);
  FD_SET(socket->m_fd, &writefds);
  FD_SET(socket->m_fd, &exceptfds);

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(socket->m_fd + 1, &readfds, &writefds, &exceptfds, &tv);

  mask = 0;

  /* If select() says that the socket is readable, then we have
   * no way to distinguish if that means 'data available' (to
   * recv) or 'incoming connection' (to accept). The same goes
   * for writability: we cannot distinguish between 'you can
   * send data' and 'connection request completed'. So we will
   * assume the following: if the flag was set upon entry,
   * that means that the event was possible.
   */
  if (FD_ISSET(socket->m_fd, &readfds))
  {
    mask |= (flags & GSOCK_CONNECTION_FLAG);
    mask |= (flags & GSOCK_INPUT_FLAG);
  }
  if (FD_ISSET(socket->m_fd, &writefds))
  {
    mask |= (flags & GSOCK_CONNECTION_FLAG);
    mask |= (flags & GSOCK_OUTPUT_FLAG);
  }
  if (FD_ISSET(socket->m_fd, &exceptfds))
    mask |= (flags & GSOCK_LOST_FLAG);

  return mask;
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
void GSocket_SetTimeout(GSocket *socket, unsigned long millisecs)
{
  assert(socket != NULL);

  socket->m_timeout.tv_sec  = (millisecs / 1000);
  socket->m_timeout.tv_usec = (millisecs % 1000) * 1000;
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

  assert (socket != NULL);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    /* We test each flag and enable the corresponding events */
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = callback;
      socket->m_data[count] = cdata;
    }
  }

  _GSocket_Configure_Callbacks(socket);
}

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags)
{
  int count = 0;

  assert(socket != NULL);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    /* We test each flag and disable the corresponding events */
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = NULL;
    }
  }

  _GSocket_Configure_Callbacks(socket);
}


/* Internals */

void _GSocket_Configure_Callbacks(GSocket *socket)
{
  long mask = 0;
  int count;

  if (socket->m_fd == INVALID_SOCKET)
    return;

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if (socket->m_cbacks[count] != NULL)
    {
      switch (count)
      {
        case GSOCK_INPUT:      mask |= FD_READ; break;
        case GSOCK_OUTPUT:     mask |= FD_WRITE; break;
        case GSOCK_CONNECTION: mask |= (FD_ACCEPT | FD_CONNECT); break;
        case GSOCK_LOST:       mask |= FD_CLOSE; break;
      }
    }
  }

  WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, mask);
}

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
  GSocket *socket;
  GSocketEvent event;
  GSocketCallback cback;

  if (uMsg >= WM_USER && uMsg <= (WM_USER + MAXSOCKETS - 1))
  {
    EnterCriticalSection(&critical);
    socket = socketList[(uMsg - WM_USER)];
    event = -1;
    cback = NULL;

    /* Check that the socket still exists (it has not been
     * destroyed) and for safety, check that the m_fd field
     * is what we expect it to be.
     */
    if ((socket != NULL) && (socket->m_fd == wParam))
    {
      switch WSAGETSELECTEVENT(lParam)
      {
        case FD_READ:    event = GSOCK_INPUT; break;
        case FD_WRITE:   event = GSOCK_OUTPUT; break;
        case FD_ACCEPT:  event = GSOCK_CONNECTION; break;
        case FD_CONNECT:
        {
          if (WSAGETSELECTERROR(lParam) != 0)
            event = GSOCK_LOST;
          else
            event = GSOCK_CONNECTION;
          break;
        }
        case FD_CLOSE:   event = GSOCK_LOST; break;
      }

      if (event != -1)
        cback = socket->m_cbacks[event];
    }

    /* OK, we can now leave the critical section because we have
     * already obtained the callback address (we make no further
     * accesses to socket->whatever)
     */
    LeaveCriticalSection(&critical);

    if (cback != NULL)
      (cback)(socket, event, socket->m_data[event]);

    return (LRESULT) 0;
  }
  else
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError _GSocket_Input_Timeout(GSocket *socket)
{
  fd_set readfds;

  if (!socket->m_non_blocking)
  {
    FD_ZERO(&readfds);
    FD_SET(socket->m_fd, &readfds);
    if (select(0, &readfds, NULL, NULL, &socket->m_timeout) == 0)
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
  fd_set writefds;

  if (!socket->m_non_blocking)
  {
    FD_ZERO(&writefds);
    FD_SET(socket->m_fd, &writefds);
    if (select(0, NULL, &writefds, NULL, &socket->m_timeout) == 0)
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

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      socket->m_error = GSOCK_IOERR;
    else
      socket->m_error = GSOCK_WOULDBLOCK;

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

  ret = recvfrom(socket->m_fd, buffer, size, 0, &from, &fromlen);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      socket->m_error = GSOCK_IOERR;
    else
      socket->m_error = GSOCK_WOULDBLOCK;

    return -1;
  }

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
  if (_GAddress_translate_from(socket->m_peer, &from, fromlen) != GSOCK_NOERROR)
  {
    socket->m_error = GSOCK_MEMERR;
    GAddress_destroy(socket->m_peer);
    return -1;
  }

  return ret;
}

int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size)
{
  int ret;

  ret = send(socket->m_fd, buffer, size, 0);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      socket->m_error = GSOCK_IOERR;
    else
      socket->m_error = GSOCK_WOULDBLOCK;

    return -1;
  }
  return ret;
}

int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;

  if (!socket->m_peer)
  {
    socket->m_error = GSOCK_INVADDR;
    return -1;
  }

  if (!_GAddress_translate_to(socket->m_peer, &addr, &len))
  {
    socket->m_error = GSOCK_MEMERR;
    return -1;
  }

  ret = sendto(socket->m_fd, buffer, size, 0, addr, len);

  /* Frees memory allocated by _GAddress_translate_to */
  free(addr);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      socket->m_error = GSOCK_IOERR;
    else
      socket->m_error = GSOCK_WOULDBLOCK;

    return -1;
  }
  return ret;
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

  address->m_family = GSOCK_NOFAMILY;
  address->m_addr   = NULL;
  address->m_len    = 0;

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
    addr2->m_addr = (struct sockaddr *) malloc(addr2->m_len);
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

  address->m_len = len;
  address->m_addr = (struct sockaddr *) malloc(len);

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
  *addr = (struct sockaddr *) malloc(address->m_len);
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
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_len = sizeof(struct sockaddr_in);
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

  addr->s_addr = inet_addr(hostname);

  /* If it is a numeric host name, convert it now */
  if (addr->s_addr == INADDR_NONE)
  {
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    if ((he = gethostbyname(hostname)) == NULL)
    {
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

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVOP;
  }

  se = getservbyname(port, protocol);
  if (!se)
  {
    if (isdigit(port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in *)address->m_addr;
      addr->sin_port = htons((u_short) port_int);
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
  assert (address != NULL);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
}

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
  assert (address != NULL);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
}

GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
  assert (address != NULL);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
}


#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */




/* Diferencias con la version Unix:
 *  - El descriptor es SOCKET y no int
 *  - Constantes -1 pasan a INVALID_SOCKET
 *  - Errores en muchas funciones pasan de -1 o <0 a SOCKET_ERROR
 *  - ioctl y close pasan a ioctlsocket y closesocket
 *  - inet_addr en lugar de inet_aton
 *  - Codigo de inicializacion y terminacion para inicializar y
 *    terminar WinSocket y para la ventana interna.
 *  - SetTimeout en la version MSW simplemente guarda el valor en
 *    socket.m_timeout, por tanto no hay necesidad de llamar a
 *    SetTimeout cada vez que se crea realmente un socket (no
 *    un gsocket).
 *  - Lo mismo para SetNonBlocking.
 */
