/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket)
 * Name:    gsocket.h
 * Author:  Guilhem Lavaux
 *          Guillermo Rodriguez Garcia <guille@iies.es> (maintainer)
 * Purpose: GSocket include file (system independent)
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GSOCKET_H
#define __GSOCKET_H

#ifdef __WINDOWS__
/* #define wxUSE_GSOCKET_CPLUSPLUS 1 */
#undef wxUSE_GSOCKET_CPLUSPLUS
#else
/* DFE: Define this and compile gsocket.cpp instead of gsocket.c and
   compile existing GUI gsock*.c as C++ to try out the new GSocket. */
#undef wxUSE_GSOCKET_CPLUSPLUS
#endif
#if !defined(__cplusplus) && defined(wxUSE_GSOCKET_CPLUSPLUS)
#error "You need to compile this file (probably a GUI gsock peice) as C++"
#endif

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#include "wx/platform.h"

#include "wx/dlimpexp.h" /* for WXDLLIMPEXP_NET */

#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#include <stddef.h>

/*
   Including sys/types.h under cygwin results in the warnings about "fd_set
   having been defined in sys/types.h" when winsock.h is included later and
   doesn't seem to be necessary anyhow. It's not needed under Mac neither.
 */
#if !defined(__WXMAC__) && !defined(__CYGWIN__) && !defined(__WXWINCE__)
#include <sys/types.h>
#endif

#ifdef __WXWINCE__
#include <stdlib.h>
#endif

#ifdef wxUSE_GSOCKET_CPLUSPLUS
# ifdef __WINDOWS__
class GSocket;
# else
typedef class GSocketBSD GSocket;
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef wxUSE_GSOCKET_CPLUSPLUS
typedef struct _GSocket GSocket;
#endif
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
  GSOCK_MEMERR,
  GSOCK_OPTERR,
} GSocketError;

/* See below for an explanation on how events work.
 */
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


/* Functions tables for internal use by GSocket code: */

#ifndef __WINDOWS__
struct GSocketBaseFunctionsTable
{
    void (*Detected_Read)(GSocket *socket);
    void (*Detected_Write)(GSocket *socket);
};
#endif

#if defined(__WINDOWS__) && defined(wxUSE_GSOCKET_CPLUSPLUS)
/* Actually this is a misnomer now, but reusing this name means I don't
   have to ifdef app traits or common socket code */
class GSocketGUIFunctionsTable
{
public:
    virtual bool OnInit() = 0;
    virtual void OnExit() = 0;
    virtual bool CanUseEventLoop() = 0;
    virtual bool Init_Socket(GSocket *socket) = 0;
    virtual void Destroy_Socket(GSocket *socket) = 0;
#ifndef __WINDOWS__
    virtual void Install_Callback(GSocket *socket, GSocketEvent event) = 0;
    virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event) = 0;
#endif
    virtual void Enable_Events(GSocket *socket) = 0;
    virtual void Disable_Events(GSocket *socket) = 0;
};

#else
struct GSocketGUIFunctionsTable
{
    int  (*GUI_Init)(void);
    void (*GUI_Cleanup)(void);
    int  (*GUI_Init_Socket)(GSocket *socket);
    void (*GUI_Destroy_Socket)(GSocket *socket);
#ifndef __WINDOWS__
    void (*Install_Callback)(GSocket *socket, GSocketEvent event);
    void (*Uninstall_Callback)(GSocket *socket, GSocketEvent event);
#endif
    void (*Enable_Events)(GSocket *socket);
    void (*Disable_Events)(GSocket *socket);
};
#endif /* defined(__WINDOWS__) && defined(wxUSE_GSOCKET_CPLUSPLUS) */


/* Global initializers */

/* Sets GUI functions callbacks. Must be called *before* GSocket_Init
   if the app uses async sockets. */
void GSocket_SetGUIFunctions(struct GSocketGUIFunctionsTable *guifunc);

/* GSocket_Init() must be called at the beginning */
int GSocket_Init(void);

/* GSocket_Cleanup() must be called at the end */
void GSocket_Cleanup(void);


/* Constructors / Destructors */

GSocket *GSocket_new(void);
#if !defined(__WINDOWS__) || !defined(wxUSE_GSOCKET_CPLUSPLUS)
void GSocket_destroy(GSocket *socket);
#endif


#ifndef wxUSE_GSOCKET_CPLUSPLUS

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket_Shutdown(GSocket *socket);

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
GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address);
GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address);
GAddress *GSocket_GetLocal(GSocket *socket);
GAddress *GSocket_GetPeer(GSocket *socket);

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
GSocketError GSocket_SetServer(GSocket *socket);

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
GSocket *GSocket_WaitConnection(GSocket *socket);


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
GSocketError GSocket_Connect(GSocket *socket, GSocketStream stream);

/* GSocket_SetReusable:
*  Simply sets the m_resuable flag on the socket. GSocket_SetServer will
*  make the appropriate setsockopt() call.
*  Implemented as a GSocket function because clients (ie, wxSocketServer)
*  don't have access to the GSocket struct information.
*  Returns TRUE if the flag was set correctly, FALSE if an error occured
*  (ie, if the parameter was NULL)
*/
int GSocket_SetReusable(GSocket *socket);

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
GSocketError GSocket_SetNonOriented(GSocket *socket);


/* Generic IO */

/* Like recv(), send(), ... */

/* For datagram sockets, the incoming / outgoing addresses
 * are stored as / read from the 'peer' address field.
 */
int GSocket_Read(GSocket *socket, char *buffer, int size);
int GSocket_Write(GSocket *socket, const char *buffer,
                  int size);

/* GSocket_Select:
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking | nonblocking) of the socket.
 */
GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags);

GSocketError GSocket_GetSockOpt(GSocket *socket, int level, int optname,
                                void *optval, int *optlen);

GSocketError GSocket_SetSockOpt(GSocket *socket, int level, int optname, 
                                const void *optval, int optlen);

/* Attributes */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket_SetNonBlocking(GSocket *socket, int non_block);

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec);

#endif /* ndef wxUSE_GSOCKET_CPLUSPLUS */

/* GSocket_GetError:
 *  Returns the last error occured for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError WXDLLIMPEXP_NET GSocket_GetError(GSocket *socket);

#ifndef wxUSE_GSOCKET_CPLUSPLUS

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
                         GSocketCallback fallback, char *cdata);

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags);

#endif /* ndef wxUSE_GSOCKET_CPLUSPLUS */


/* GAddress */

GAddress *GAddress_new(void);
GAddress *GAddress_copy(GAddress *address);
void GAddress_destroy(GAddress *address);

void GAddress_SetFamily(GAddress *address, GAddressType type);
GAddressType GAddress_GetFamily(GAddress *address);

/* The use of any of the next functions will set the address family to
 * the specific one. For example if you use GAddress_INET_SetHostName,
 * address family will be implicitly set to AF_INET.
 */

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname);
GSocketError GAddress_INET_SetAnyAddress(GAddress *address);
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
}
#endif /* __cplusplus */

#ifdef wxUSE_GSOCKET_CPLUSPLUS
# ifdef __WINDOWS__
#  include "wx/msw/gsockmsw.h"
# else
#  include "wx/unix/gsockunx.h"
# endif
#endif

#endif    /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

#endif    /* __GSOCKET_H */
