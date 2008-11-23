/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket)
 * Name:        gsocket.h
 * Author:      Guilhem Lavaux
 *              Guillermo Rodriguez Garcia <guille@iies.es>
 * Copyright:   (c) Guilhem Lavaux
 *              (c) 2007,2008 Vadim Zeitlin <vadim@wxwidgets.org>
 * Licence:     wxWindows Licence
 * Purpose:     GSocket include file (system independent)
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef _WX_GSOCKET_H_
#define _WX_GSOCKET_H_

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/dlimpexp.h" /* for WXDLLIMPEXP_NET */

#include <stddef.h>

/*
   Including sys/types.h under Cygwin results in the warnings about "fd_set
   having been defined in sys/types.h" when winsock.h is included later and
   doesn't seem to be necessary anyhow. It's not needed under Mac neither.
 */
#if !defined(__WXMAC__) && !defined(__CYGWIN__) && !defined(__WXWINCE__)
#include <sys/types.h>
#endif

#ifdef __WXWINCE__
#include <stdlib.h>
#endif

enum GAddressType
{
  GSOCK_NOFAMILY = 0,
  GSOCK_INET,
  GSOCK_INET6,
  GSOCK_UNIX
};

enum GSocketStream
{
  GSOCK_STREAMED,
  GSOCK_UNSTREAMED
};

enum GSocketError
{
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
  GSOCK_OPTERR
};

/* See below for an explanation on how events work.
 */
enum GSocketEvent
{
  GSOCK_INPUT  = 0,
  GSOCK_OUTPUT = 1,
  GSOCK_CONNECTION = 2,
  GSOCK_LOST = 3,
  GSOCK_MAX_EVENT = 4
};

enum
{
  GSOCK_INPUT_FLAG = 1 << GSOCK_INPUT,
  GSOCK_OUTPUT_FLAG = 1 << GSOCK_OUTPUT,
  GSOCK_CONNECTION_FLAG = 1 << GSOCK_CONNECTION,
  GSOCK_LOST_FLAG = 1 << GSOCK_LOST
};

typedef int GSocketEventFlags;

struct GAddress;
class GSocket;

typedef void (*GSocketCallback)(GSocket *socket, GSocketEvent event,
                                char *cdata);

/*
   Class providing hooks abstracting the differences between console and GUI
   applications for socket code.

   We also have different implementations of this class for different platforms
   allowing us to keep more things in the common code but the main reason for
   its existence is that we want the same socket code work differently
   depending on whether it's used from a console or a GUI program. This is
   achieved by implementing the virtual methods of this class differently in
   the objects returned by wxConsoleAppTraits::GetSocketFunctionsTable() and
   the same method in wxGUIAppTraits.
 */
class GSocketManager
{
public:
    // set the manager to use, we don't take ownership of it
    //
    // this should be called before GSocket_Init(), i.e. before the first
    // wxSocket object is created, otherwise the manager returned by
    // wxAppTraits::GetSocketManager() will be used
    static void Set(GSocketManager *manager);

    // return the manager to use
    //
    // this initializes the manager at first use
    static GSocketManager *Get()
    {
        if ( !ms_manager )
            Init();

        return ms_manager;
    }

    // called before the first wxSocket is created and should do the
    // initializations needed in order to use the network
    //
    // return true if initialized successfully
    virtual bool OnInit() = 0;

    // undo the initializations of OnInit()
    virtual void OnExit() = 0;


    // do manager-specific socket initializations (and undo it): this is called
    // in the beginning/end of the socket initialization/destruction
    virtual bool Init_Socket(GSocket *socket) = 0;
    virtual void Destroy_Socket(GSocket *socket) = 0;

    virtual void Install_Callback(GSocket *socket, GSocketEvent event) = 0;
    virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event) = 0;

    virtual void Enable_Events(GSocket *socket) = 0;
    virtual void Disable_Events(GSocket *socket) = 0;

    virtual ~GSocketManager() { }

private:
    // get the manager to use if we don't have it yet
    static void Init();

    static GSocketManager *ms_manager;
};

/*
    Base class providing functionality common to BSD and Winsock sockets.

    TODO: merge this in wxSocket itself, there is no reason to maintain the
          separation between wxSocket and GSocket.
 */
class GSocketBase
{
public:
    // static factory function
    static GSocket *Create();

    virtual ~GSocketBase();

    GSocketEventFlags Select(GSocketEventFlags flags);

    virtual void Close() = 0;
    virtual void Shutdown();

#ifdef __WINDOWS__
    SOCKET m_fd;
#else
    int m_fd;
#endif

    int m_initialRecvBufferSize;
    int m_initialSendBufferSize;

    GAddress *m_local;
    GAddress *m_peer;
    GSocketError m_error;

    bool m_non_blocking;
    bool m_server;
    bool m_stream;
    bool m_establishing;
    bool m_reusable;
    bool m_broadcast;
    bool m_dobind;

#ifdef __WINDOWS__
    struct timeval m_timeout;
#else
    unsigned long m_timeout;
#endif

    GSocketEventFlags m_detected;
    GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
    char *m_data[GSOCK_MAX_EVENT];

protected:
    GSocketBase();
};

#if defined(__WINDOWS__)
    #include "wx/msw/gsockmsw.h"
#else
    #include "wx/unix/gsockunx.h"
#endif

/* Global initializers */

/* GSocket_Init() must be called at the beginning (but after calling
 * GSocketManager::Set() if a custom manager should be used) */
bool GSocket_Init();

/* GSocket_Cleanup() must be called at the end */
void GSocket_Cleanup();


/* GAddress */

// Represents a socket endpoint, i.e. -- in spite of its name -- not an address
// but an (address, port) pair
struct GAddress
{
    struct sockaddr *m_addr;
    size_t m_len;

    GAddressType m_family;
    int m_realfamily;

    GSocketError m_error;
};

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
GSocketError GAddress_INET_SetBroadcastAddress(GAddress *address);
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

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len);
GSocketError _GAddress_translate_to  (GAddress *address,
                                      struct sockaddr **addr, int *len);
GSocketError _GAddress_Init_INET(GAddress *address);

#if wxUSE_IPV6

GSocketError GAddress_INET6_SetHostName(GAddress *address, const char *hostname);
GSocketError GAddress_INET6_SetAnyAddress(GAddress *address);
GSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr);
GSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol);
GSocketError GAddress_INET6_SetPort(GAddress *address, unsigned short port);

GSocketError GAddress_INET6_GetHostName(GAddress *address, char *hostname,
                                       size_t sbuf);
GSocketError GAddress_INET6_GetHostAddress(GAddress *address,struct in6_addr *hostaddr);
unsigned short GAddress_INET6_GetPort(GAddress *address);

#endif // wxUSE_IPV6

// these functions are available under all platforms but only implemented under
// Unix ones, elsewhere they just return GSOCK_INVADDR
GSocketError _GAddress_Init_UNIX(GAddress *address);
GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path);
GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf);

// standard linux headers produce many warnings when used with icc
#if defined(__INTELC__) && defined(__LINUX__)
    inline void wxFD_ZERO(fd_set *fds)
    {
        #pragma warning(push)
        #pragma warning(disable:593)
        FD_ZERO(fds);
        #pragma warning(pop)
    }

    inline void wxFD_SET(int fd, fd_set *fds)
    {
        #pragma warning(push, 1)
        #pragma warning(disable:1469)
        FD_SET(fd, fds);
        #pragma warning(pop)
    }

    inline bool wxFD_ISSET(int fd, fd_set *fds)
    {
        #pragma warning(push, 1)
        #pragma warning(disable:1469)
        return FD_ISSET(fd, fds);
        #pragma warning(pop)
    }
    inline bool wxFD_CLR(int fd, fd_set *fds)
    {
        #pragma warning(push, 1)
        #pragma warning(disable:1469)
        return FD_CLR(fd, fds);
        #pragma warning(pop)
    }
#else // !__INTELC__
    #define wxFD_ZERO(fds) FD_ZERO(fds)
    #define wxFD_SET(fd, fds) FD_SET(fd, fds)
    #define wxFD_ISSET(fd, fds) FD_ISSET(fd, fds)
    #define wxFD_CLR(fd, fds) FD_CLR(fd, fds)
#endif // __INTELC__/!__INTELC__

// this is for Windows where configure doesn't define this
#ifndef SOCKOPTLEN_T
    #define SOCKOPTLEN_T int
#endif

/*
 * MSW defines this, Unices don't.
 */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#endif /* wxUSE_SOCKETS */

#endif /* _WX_GSOCKET_H_ */
