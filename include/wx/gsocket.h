/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket)
 * Name:    gsocket.h
 * Purpose: GSocket include file (system independent)
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
#ifndef __GSOCKET_H
#define __GSOCKET_H

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
  GSOCK_INVPORT
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

typedef void (*GSocketFallback)(GSocket *socket, GSocketEvent event,
                                char *cdata);

#ifdef __cplusplus
extern "C" {
#endif

/* Global initialisers */

/* GSocket_Init() must be called at the beginning */
bool GSocket_Init();
/* GSocket_Cleanup() must be called at the ending */
void GSocket_Cleanup();

/* Constructors / Destructors */

GSocket *GSocket_new();
void GSocket_destroy(GSocket *socket);

/* This will disable all IO calls to this socket but errors are still available */
void GSocket_Shutdown(GSocket *socket);

/* Address handling */

GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address);
GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address);
GAddress *GSocket_GetLocal(GSocket *socket);
GAddress *GSocket_GetPeer(GSocket *socket);

/* Non-oriented connections handlers */

GSocketError GSocket_SetNonOriented(GSocket *socket);

/* Server specific parts */

/*
  GSocket_SetServer() setups the socket as a server. It uses the "Local" field
  of GSocket. "Local" must be set by GSocket_SetLocal() before
  GSocket_SetServer() is called. In the other case, it returns GSOCK_INVADDR.
*/
GSocketError GSocket_SetServer(GSocket *socket);

/*
  GSocket_WaitConnection() waits for an incoming client connection.
*/
GSocket *GSocket_WaitConnection(GSocket *socket);

/* Client specific parts */

/*
  GSocket_Connect() establishes a client connection to a server using the "Peer"
  field of GSocket. "Peer" must be set by GSocket_SetPeer() before
  GSocket_Connect() is called. In the other case, it returns GSOCK_INVADDR.
*/
GSocketError GSocket_Connect(GSocket *socket, GSocketStream stream);

/* Generic IO */

/* Like recv(), send(), ... */
/*
   NOTE: In case we read from a non-oriented connection, the incoming (outgoing)
   connection address is stored in the "Local" ("Peer") field.
*/
int GSocket_Read(GSocket *socket, char *buffer, int size);
int GSocket_Write(GSocket *socket, const char *buffer,
                  int size);
bool GSocket_DataAvailable(GSocket *socket);

/* Flags */

/*
  GSocket_SetBlocking() puts the socket in non-blocking mode. This is useful
  if we don't want to wait.
*/
void GSocket_SetBlocking(GSocket *socket, bool block);

/*
  GSocket_GetError() returns the last error occured on the socket stream.
*/

GSocketError GSocket_GetError(GSocket *socket);

/* Callbacks */

/*
   Only one fallback is possible for each event (INPUT, OUTPUT, CONNECTION, LOST)
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
                         GSocketFallback fallback, char *cdata);

/*
  UnsetFallback will disables all fallbacks specified by "event".
  NOTE: event may be a combination of flags
*/
void GSocket_UnsetFallback(GSocket *socket, GSocketEventFlags event);

/* GAddress */

GAddress *GAddress_new();
GAddress *GAddress_copy(GAddress *address);
void GAddress_destroy(GAddress *address);

void GAddress_SetFamily(GAddress *address, GAddressType type);
GAddressType GAddress_GetFamily(GAddress *address);

/*
   The use of any of the next functions will set the address family to the adapted
   one. For example if you use GAddress_INET_SetHostName, address family will be AF_INET
   implicitely
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

/*
 * System specific functions
 */

/* On systems needing an event id */
void GSocket_SetEventID(GSocket *socket, unsigned long evt_id);

/* On systems which don't have background refresh */
void GSocket_DoEvent(unsigned long evt_id);

#ifdef __cplusplus
};
#endif

#endif
 /* __GSOCKET_H */
