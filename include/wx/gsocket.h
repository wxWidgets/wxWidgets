/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket)
 * Name:    gsocket.h
 * Purpose: GSocket include file (system independent)
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
#ifndef __GSOCKET_H
#define __GSOCKET_H

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stddef.h>
#include <sys/types.h>

#if !defined(__cplusplus)
typedef int bool;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSocket GSocket;
typedef struct _GAddress GAddress;

typedef enum {
  GSOCK_NOFAMILY = 0,
  GSOCK_INET,
  GSOCK_INET6,
  GSOCK_UNIX
} GAddressType;

typedef enum {
  GSOCK_STREAMED,
  GSOCK_UNSTREAMED
} GSocketStream;

typedef enum {
  GSOCK_NOERROR = 0,
  GSOCK_INVOP,
  GSOCK_IOERR,
  GSOCK_INVADDR,
  GSOCK_INVSOCK,
  GSOCK_NOHOST,
  GSOCK_INVPORT,
  GSOCK_WOULDBLOCK,
  GSOCK_TIMEDOUT,
  GSOCK_MEMERR
} GSocketError;

typedef enum {
  GSOCK_INPUT  = 0,
  GSOCK_OUTPUT = 1,
  GSOCK_CONNECTION = 2,
  GSOCK_LOST = 3,
  GSOCK_MAX_EVENT = 4
} GSocketEvent;

enum {
  GSOCK_INPUT_FLAG = 1 << GSOCK_INPUT,
  GSOCK_OUTPUT_FLAG = 1 << GSOCK_OUTPUT,
  GSOCK_CONNECTION_FLAG = 1 << GSOCK_CONNECTION,
  GSOCK_LOST_FLAG = 1 << GSOCK_LOST
};

typedef int GSocketEventFlags;

typedef void (*GSocketCallback)(GSocket *socket, GSocketEvent event,
                                char *cdata);


/* Global initializers */

/* GSocket_Init() must be called at the beginning */
bool GSocket_Init();
/* GSocket_Cleanup() must be called at the ending */
void GSocket_Cleanup();

/* Constructors / Destructors */

GSocket *GSocket_new();
void GSocket_destroy(GSocket *socket);

/* This will disable all further IO calls to this socket */
void GSocket_Shutdown(GSocket *socket);

/* Address handling */

GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address);
GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address);
GAddress *GSocket_GetLocal(GSocket *socket);
GAddress *GSocket_GetPeer(GSocket *socket);

/* Non-oriented connections handlers */

GSocketError GSocket_SetNonOriented(GSocket *socket);

/* Server specific parts */

/* GSocket_SetServer:
 *  Sets up the socket as a server. It uses the "Local" field of GSocket.
 *  "Local" must be set by GSocket_SetLocal() before GSocket_SetServer()
 *  is called. Possible error codes are: GSOCK_INVSOCK if socket has not
 *  been initialized, GSOCK_INVADDR if the local address has not been
 *  defined and GSOCK_IOERR for other internal errors.
 */
GSocketError GSocket_SetServer(GSocket *socket);

/* GSocket_WaitConnection:
 *  Waits for an incoming client connection.
 */
GSocket *GSocket_WaitConnection(GSocket *socket);

/* Client specific parts */

/* GSocket_Connect:
 *  Establishes a client connection to a server using the "Peer"
 *  field of GSocket. "Peer" must be set by GSocket_SetPeer() before
 *  GSocket_Connect() is called. Possible error codes are GSOCK_INVSOCK,
 *  GSOCK_INVADDR, GSOCK_TIMEDOUT, GSOCK_WOULDBLOCK and GSOCK_IOERR.
 *  If a socket is nonblocking and Connect() returns GSOCK_WOULDBLOCK,
 *  the connection request can be completed later. Use GSocket_Select()
 *  to check it, or wait for a GSOCK_CONNECTION event.
 */
GSocketError GSocket_Connect(GSocket *socket, GSocketStream stream);

/* Generic IO */

/* Like recv(), send(), ... */

/* NOTE: In case we read from a non-oriented connection, the incoming
 * (outgoing) connection address is stored in the "Local" ("Peer")
 * field.
 */
int GSocket_Read(GSocket *socket, char *buffer, int size);
int GSocket_Write(GSocket *socket, const char *buffer,
                  int size);

/* GSocket_Select:
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking|nonblocking) of the socket.
 */
GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags);

/* Flags/Parameters */

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is
 *  expressed in milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec);

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. This is useful if
 *  we don't want to wait.
 */
void GSocket_SetNonBlocking(GSocket *socket, bool non_block);

/* GSocket_GetError:
 *  Returns the last error occured for this socket.
 */
GSocketError GSocket_GetError(GSocket *socket);

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
                         GSocketCallback fallback, char *cdata);

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags);

/* GAddress */

GAddress *GAddress_new();
GAddress *GAddress_copy(GAddress *address);
void GAddress_destroy(GAddress *address);

void GAddress_SetFamily(GAddress *address, GAddressType type);
GAddressType GAddress_GetFamily(GAddress *address);

/* The use of any of the next functions will set the address family to
 * the specific one. For example if you use GAddress_INET_SetHostName,
 * address family will be implicitly set to AF_INET.
 */

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname);
GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr);
GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol);
GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port);

GSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname,
                                       size_t sbuf);
unsigned long GAddress_INET_GetHostAddress(GAddress *address);
unsigned short GAddress_INET_GetPort(GAddress *address);

/* TODO: Define specific parts (INET6, UNIX) */

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path);
GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf);

#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif    /* wxUSE_SOCKETS */

#endif    /* __GSOCKET_H */
