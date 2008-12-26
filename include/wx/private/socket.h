/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/socket.h
// Purpose:     wxSocketImpl nd related declarations
// Authors:     Guilhem Lavaux, Vadim Zeitlin
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Guilhem Lavaux
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    Brief overview of different socket classes:

    - wxSocketBase is the public class representing a socket ("Base" here
      refers to the fact that wxSocketClient and wxSocketServer are derived
      from it and predates the convention of using "Base" for common base
      classes for platform-specific classes in wxWidgets) with implementation
      common to all platforms and forwarding methods whose implementation
      differs between platforms to wxSocketImpl which it contains.

    - wxSocketImpl is actually just an abstract base class having only code
      common to all platforms, the concrete implementation classes derive from
      it and are created by wxSocketImpl::Create().

    - Some socket operations have different implementations in console-mode and
      GUI applications. wxSocketManager class exists to abstract this in such
      way that console applications (using wxBase) don't depend on wxNet. An
      object of this class is made available via wxApp and GUI applications set
      up a different kind of global socket manager from console ones.

      TODO: it looks like wxSocketManager could be eliminated by providing
            methods for registering/unregistering sockets directly in
            wxEventLoop.
 */

#ifndef _WX_PRIVATE_SOCKET_H_
#define _WX_PRIVATE_SOCKET_H_

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/socket.h"

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

// include the header defining timeval: under Windows this struct is used only
// with sockets so we need to include winsock.h which we do via windows.h
#ifdef __WXMSW__
    #include "wx/msw/wrapwin.h"
#else
    #include <sys/time.h>   // for timeval
#endif

// these definitions are for MSW when we don't use configure, otherwise these
// symbols are defined by configure
#ifndef WX_SOCKLEN_T
    #define WX_SOCKLEN_T int
#endif

#ifndef SOCKOPTLEN_T
    #define SOCKOPTLEN_T int
#endif

// define some symbols which winsock.h defines but traditional BSD headers
// don't
#ifndef __WXMSW__
    #define SOCKET int
#endif

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
#endif

#if wxUSE_IPV6
    typedef struct sockaddr_storage wxSockAddr;
#else
    typedef struct sockaddr wxSockAddr;
#endif

enum GAddressType
{
    wxSOCKET_NOFAMILY = 0,
    wxSOCKET_INET,
    wxSOCKET_INET6,
    wxSOCKET_UNIX
};

typedef int wxSocketEventFlags;

struct GAddress;
class wxSocketImpl;

/*
   Class providing hooks abstracting the differences between console and GUI
   applications for socket code.

   We also have different implementations of this class for different platforms
   allowing us to keep more things in the common code but the main reason for
   its existence is that we want the same socket code work differently
   depending on whether it's used from a console or a GUI program. This is
   achieved by implementing the virtual methods of this class differently in
   the objects returned by wxConsoleAppTraits::GetSocketManager() and the same
   method in wxGUIAppTraits.
 */
class wxSocketManager
{
public:
    // set the manager to use, we don't take ownership of it
    //
    // this should be called before creating the first wxSocket object,
    // otherwise the manager returned by wxAppTraits::GetSocketManager() will
    // be used
    static void Set(wxSocketManager *manager);

    // return the manager to use
    //
    // this initializes the manager at first use
    static wxSocketManager *Get()
    {
        if ( !ms_manager )
            Init();

        return ms_manager;
    }

    // called before the first wxSocket is created and should do the
    // initializations needed in order to use the network
    //
    // return true if initialized successfully; if this returns false sockets
    // can't be used at all
    virtual bool OnInit() = 0;

    // undo the initializations of OnInit()
    virtual void OnExit() = 0;


    // these functions enable or disable monitoring of the given socket for the
    // specified events inside the currently running event loop (but notice
    // that both BSD and Winsock implementations actually use socket->m_server
    // value to determine what exactly should be monitored so it needs to be
    // set before calling these functions)
    virtual void Install_Callback(wxSocketImpl *socket,
                                  wxSocketNotify event = wxSOCKET_MAX_EVENT) = 0;
    virtual void Uninstall_Callback(wxSocketImpl *socket,
                                    wxSocketNotify event = wxSOCKET_MAX_EVENT) = 0;

    virtual ~wxSocketManager() { }

private:
    // get the manager to use if we don't have it yet
    static void Init();

    static wxSocketManager *ms_manager;
};

/*
    Base class for all socket implementations providing functionality common to
    BSD and Winsock sockets.

    Objects of this class are not created directly but only via its static
    Create() method which is implemented in port-specific code.
 */
class wxSocketImpl
{
public:
    // static factory function: creates the low-level socket associated with
    // the given wxSocket (and inherits its attributes such as timeout)
    static wxSocketImpl *Create(wxSocketBase& wxsocket);

    virtual ~wxSocketImpl();

    // set various socket properties: all of those can only be called before
    // creating the socket
    void SetTimeout(unsigned long millisec);
    void SetNonBlocking(bool non_blocking) { m_non_blocking = non_blocking; }
    void SetReusable() { m_reusable = true; }
    void SetBroadcast() { m_broadcast = true; }
    void DontDoBind() { m_dobind = false; }
    void SetInitialSocketBuffers(int recv, int send)
    {
        m_initialRecvBufferSize = recv;
        m_initialSendBufferSize = send;
    }

    wxSocketError SetLocal(GAddress *address);
    wxSocketError SetPeer(GAddress *address);

    // accessors
    // ---------

    GAddress *GetLocal();
    GAddress *GetPeer();

    wxSocketError GetError() const { return m_error; }
    bool IsOk() const { return m_error == wxSOCKET_NOERROR; }


    // creating/closing the socket
    // --------------------------

    // notice that SetLocal() must be called before creating the socket using
    // any of the functions below
    //
    // all of Create() functions return wxSOCKET_NOERROR if the operation
    // completed successfully or one of:
    //  wxSOCKET_INVSOCK - the socket is in use.
    //  wxSOCKET_INVADDR - the local (server) or peer (client) address has not
    //                     been set.
    //  wxSOCKET_IOERR   - any other error.

    // create a socket listening on the local address specified by SetLocal()
    // (notice that DontDoBind() is ignored by this function)
    wxSocketError CreateServer();

    // create a socket connected to the peer address specified by SetPeer()
    // (notice that DontDoBind() is ignored by this function)
    //
    // this function may return wxSOCKET_WOULDBLOCK in addition to the return
    // values listed above
    wxSocketError CreateClient();

    // create (and bind unless DontDoBind() had been called) an UDP socket
    // associated with the given local address
    wxSocketError CreateUDP();

    // may be called whether the socket was created or not, calls DoClose() if
    // it was indeed created
    void Close();

    virtual void Shutdown();


    // IO operations
    // -------------

    virtual int Read(void *buffer, int size) = 0;
    virtual int Write(const void *buffer, int size) = 0;

    wxSocketEventFlags Select(wxSocketEventFlags flags);

    virtual wxSocketImpl *WaitConnection(wxSocketBase& wxsocket) = 0;


    // notifications
    // -------------

    // notify m_wxsocket about the given socket event by calling its (inaptly
    // named) OnRequest() method
    void NotifyOnStateChange(wxSocketNotify event);

    // TODO: make these fields protected and provide accessors for those of
    //       them that wxSocketBase really needs
//protected:
    SOCKET m_fd;

    int m_initialRecvBufferSize;
    int m_initialSendBufferSize;

    GAddress *m_local;
    GAddress *m_peer;
    wxSocketError m_error;

    bool m_non_blocking;
    bool m_server;
    bool m_stream;
    bool m_establishing;
    bool m_reusable;
    bool m_broadcast;
    bool m_dobind;

    struct timeval m_timeout;

    wxSocketEventFlags m_detected;

protected:
    wxSocketImpl(wxSocketBase& wxsocket);

    // wait until input/output becomes available or m_timeout expires
    //
    // returns true if we do have input/output or false on timeout or error
    // (also sets m_error accordingly)
    bool BlockForInputWithTimeout()
        { return DoBlockWithTimeout(wxSOCKET_INPUT_FLAG); }
    bool BlockForOutputWithTimeout()
        { return DoBlockWithTimeout(wxSOCKET_OUTPUT_FLAG); }

private:
    // handle the given connect() return value (which may be 0 or EWOULDBLOCK
    // or something else)
    virtual wxSocketError DoHandleConnect(int ret) = 0;

    // called by Close() if we have a valid m_fd
    virtual void DoClose() = 0;

    // put this socket into non-blocking mode and enable monitoring this socket
    // as part of the event loop
    virtual void UnblockAndRegisterWithEventLoop() = 0;

    // check that the socket wasn't created yet and that the given address
    // (either m_local or m_peer depending on the socket kind) is valid and
    // set m_error and return false if this is not the case
    bool PreCreateCheck(GAddress *addr);

    // set the given socket option: this just wraps setsockopt(SOL_SOCKET)
    int SetSocketOption(int optname, int optval)
    {
        // although modern Unix systems use "const void *" for the 4th
        // parameter here, old systems and Winsock still use "const char *"
        return setsockopt(m_fd, SOL_SOCKET, optname,
                          reinterpret_cast<const char *>(&optval),
                          sizeof(optval));
    }

    // set the given socket option to true value: this is an even simpler
    // wrapper for setsockopt(SOL_SOCKET) for boolean options
    int EnableSocketOption(int optname)
    {
        return SetSocketOption(optname, 1);
    }

    // apply the options to the (just created) socket and register it with the
    // event loop by calling UnblockAndRegisterWithEventLoop()
    void PostCreation();

    // update local address after binding/connecting
    wxSocketError UpdateLocalAddress();

    // wait for IO on the socket or until timeout expires
    //
    // the parameter can be one of wxSOCKET_INPUT/OUTPUT_FLAG (but could be
    // their combination in the future, hence we take wxSocketEventFlags)
    bool DoBlockWithTimeout(wxSocketEventFlags flags);


    // set in ctor and never changed except that it's reset to NULL when the
    // socket is shut down
    wxSocketBase *m_wxsocket;

    DECLARE_NO_COPY_CLASS(wxSocketImpl)
};

#if defined(__WXMSW__)
    #include "wx/msw/private/sockmsw.h"
#else
    #include "wx/unix/private/sockunix.h"
#endif


/* GAddress */

// TODO: make GAddress a real class instead of this mix of C and C++

// Represents a socket endpoint, i.e. -- in spite of its name -- not an address
// but an (address, port) pair
struct GAddress
{
    struct sockaddr *m_addr;
    size_t m_len;

    GAddressType m_family;
    int m_realfamily;

    wxSocketError m_error;
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

wxSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname);
wxSocketError GAddress_INET_SetBroadcastAddress(GAddress *address);
wxSocketError GAddress_INET_SetAnyAddress(GAddress *address);
wxSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr);
wxSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol);
wxSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port);

wxSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname,
                                       size_t sbuf);
unsigned long GAddress_INET_GetHostAddress(GAddress *address);
unsigned short GAddress_INET_GetPort(GAddress *address);

wxSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len);
wxSocketError _GAddress_translate_to  (GAddress *address,
                                      struct sockaddr **addr, int *len);
wxSocketError _GAddress_Init_INET(GAddress *address);

#if wxUSE_IPV6

wxSocketError GAddress_INET6_SetHostName(GAddress *address, const char *hostname);
wxSocketError GAddress_INET6_SetAnyAddress(GAddress *address);
wxSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr);
wxSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol);
wxSocketError GAddress_INET6_SetPort(GAddress *address, unsigned short port);

wxSocketError GAddress_INET6_GetHostName(GAddress *address, char *hostname,
                                       size_t sbuf);
wxSocketError GAddress_INET6_GetHostAddress(GAddress *address,struct in6_addr *hostaddr);
unsigned short GAddress_INET6_GetPort(GAddress *address);

#endif // wxUSE_IPV6

// these functions are available under all platforms but only implemented under
// Unix ones, elsewhere they just return wxSOCKET_INVADDR
wxSocketError _GAddress_Init_UNIX(GAddress *address);
wxSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path);
wxSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf);

#endif /* wxUSE_SOCKETS */

#endif /* _WX_PRIVATE_SOCKET_H_ */
