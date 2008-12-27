/////////////////////////////////////////////////////////////////////////////
// Name:       src/msw/sockmsw.cpp
// Purpose:    MSW-specific socket code
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia
// Created:    April 1997
// Copyright:  (C) 1999-1997, Guilhem Lavaux
//             (C) 1999-2000, Guillermo Rodriguez Garcia
//             (C) 2008 Vadim Zeitlin
// RCS_ID:     $Id$
// License:    wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SOCKETS

/* including rasasync.h (included from windows.h itself included from
 * wx/setup.h and/or winsock.h results in this warning for
 * RPCNOTIFICATION_ROUTINE
 */
#ifdef _MSC_VER
#   pragma warning(disable:4115) /* named type definition in parentheses */
#endif

#include "wx/private/socket.h"
#include "wx/msw/private.h"     // for wxGetInstance()
#include "wx/apptrait.h"
#include "wx/thread.h"
#include "wx/dynlib.h"

#ifdef __WXWINCE__
/*
 * As WSAAsyncSelect is not present on WinCE, it now uses WSACreateEvent,
 * WSAEventSelect, WSAWaitForMultipleEvents and WSAEnumNetworkEvents. When
 * enabling eventhandling for a socket a new thread it created that keeps track
 * of the events and posts a messageto the hidden window to use the standard
 * message loop.
 */
#include "wx/msw/wince/net.h"
#include "wx/hashmap.h"
WX_DECLARE_HASH_MAP(int,bool,wxIntegerHash,wxIntegerEqual,SocketHash);

#ifndef isdigit
#define isdigit(x) (x > 47 && x < 58)
#endif
#include "wx/msw/wince/net.h"

#endif // __WXWINCE__

#ifdef _MSC_VER
#  pragma warning(default:4115) /* named type definition in parentheses */
#endif

#define CLASSNAME  TEXT("_wxSocket_Internal_Window_Class")

/* implemented in utils.cpp */
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

/* Maximum number of different wxSocket objects at a given time.
 * This value can be modified at will, but it CANNOT be greater
 * than (0x7FFF - WM_USER + 1)
 */
#define MAXSOCKETS 1024

#if (MAXSOCKETS > (0x7FFF - WM_USER + 1))
#error "MAXSOCKETS is too big!"
#endif

#ifndef __WXWINCE__
typedef int (PASCAL *WSAAsyncSelect_t)(SOCKET,HWND,u_int,long);
#else
/* Typedef the needed function prototypes and the WSANETWORKEVENTS structure
*/
typedef struct _WSANETWORKEVENTS {
       long lNetworkEvents;
       int iErrorCode[10];
} WSANETWORKEVENTS, FAR * LPWSANETWORKEVENTS;
typedef HANDLE (PASCAL *WSACreateEvent_t)();
typedef int (PASCAL *WSAEventSelect_t)(SOCKET,HANDLE,long);
typedef int (PASCAL *WSAWaitForMultipleEvents_t)(long,HANDLE,BOOL,long,BOOL);
typedef int (PASCAL *WSAEnumNetworkEvents_t)(SOCKET,HANDLE,LPWSANETWORKEVENTS);
#endif //__WXWINCE__

LRESULT CALLBACK wxSocket_Internal_WinProc(HWND, UINT, WPARAM, LPARAM);

/* Global variables */

static HWND hWin;
wxCRIT_SECT_DECLARE_MEMBER(gs_critical);
static wxSocketImplMSW *socketList[MAXSOCKETS];
static int firstAvailable;

#ifndef __WXWINCE__
static WSAAsyncSelect_t gs_WSAAsyncSelect = NULL;
#else
static SocketHash socketHash;
static unsigned int currSocket;
HANDLE hThread[MAXSOCKETS];
static WSACreateEvent_t gs_WSACreateEvent = NULL;
static WSAEventSelect_t gs_WSAEventSelect = NULL;
static WSAWaitForMultipleEvents_t gs_WSAWaitForMultipleEvents = NULL;
static WSAEnumNetworkEvents_t gs_WSAEnumNetworkEvents = NULL;
/* This structure will be used to pass data on to the thread that handles socket events.
*/
typedef struct thread_data{
    HWND hEvtWin;
    unsigned long msgnumber;
    unsigned long fd;
    unsigned long lEvent;
}thread_data;
#endif

#ifdef __WXWINCE__
/* This thread handles socket events on WinCE using WSAEventSelect() as
 * WSAAsyncSelect is not supported. When an event occurs for the socket, it is
 * checked what kind of event happend and the correct message gets posted so
 * that the hidden window can handle it as it would in other MSW builds.
*/
DWORD WINAPI SocketThread(LPVOID data)
{
    WSANETWORKEVENTS NetworkEvents;
    thread_data* d = (thread_data *)data;

    HANDLE NetworkEvent = gs_WSACreateEvent();
    gs_WSAEventSelect(d->fd, NetworkEvent, d->lEvent);

    while(socketHash[d->fd] == true)
    {
        if ((gs_WSAWaitForMultipleEvents(1, &NetworkEvent, FALSE,INFINITE, FALSE)) == WAIT_FAILED)
        {
            printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
            return 0;
        }
        if (gs_WSAEnumNetworkEvents(d->fd ,NetworkEvent, &NetworkEvents) == SOCKET_ERROR)
        {
            printf("WSAEnumNetworkEvents failed with error %d\n", WSAGetLastError());
            return 0;
        }

        long flags = NetworkEvents.lNetworkEvents;
        if (flags & FD_READ)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_READ);
        if (flags & FD_WRITE)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_WRITE);
        if (flags & FD_OOB)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_OOB);
        if (flags & FD_ACCEPT)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_ACCEPT);
        if (flags & FD_CONNECT)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_CONNECT);
        if (flags & FD_CLOSE)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_CLOSE);

    }
    gs_WSAEventSelect(d->fd, NetworkEvent, 0);
    ExitThread(0);
    return 0;
}
#endif

// ----------------------------------------------------------------------------
// MSW implementation of wxSocketManager
// ----------------------------------------------------------------------------

class wxSocketMSWManager : public wxSocketManager
{
public:
    virtual bool OnInit();
    virtual void OnExit();

    virtual wxSocketImpl *CreateSocket(wxSocketBase& wxsocket)
    {
        return new wxSocketImplMSW(wxsocket);
    }
    virtual void Install_Callback(wxSocketImpl *socket, wxSocketNotify event);
    virtual void Uninstall_Callback(wxSocketImpl *socket, wxSocketNotify event);

private:
    static wxDynamicLibrary gs_wsock32dll;
};

wxDynamicLibrary wxSocketMSWManager::gs_wsock32dll;

bool wxSocketMSWManager::OnInit()
{
  static LPCTSTR pclassname = NULL;
  int i;

  /* Create internal window for event notifications */
  hWin = wxCreateHiddenWindow(&pclassname, CLASSNAME, wxSocket_Internal_WinProc);
  if (!hWin)
      return false;

  /* Initialize socket list */
  for (i = 0; i < MAXSOCKETS; i++)
  {
    socketList[i] = NULL;
  }
  firstAvailable = 0;

  // we don't link with wsock32.dll (or ws2 in CE case) statically to avoid
  // dependencies on it for all the application using wx even if they don't use
  // sockets
#ifdef __WXWINCE__
    #define WINSOCK_DLL_NAME _T("ws2.dll")
#else
    #define WINSOCK_DLL_NAME _T("wsock32.dll")
#endif

    gs_wsock32dll.Load(WINSOCK_DLL_NAME, wxDL_VERBATIM | wxDL_QUIET);
    if ( !gs_wsock32dll.IsLoaded() )
        return false;

#ifndef __WXWINCE__
    wxDL_INIT_FUNC(gs_, WSAAsyncSelect, gs_wsock32dll);
    if ( !gs_WSAAsyncSelect )
        return false;
#else
    wxDL_INIT_FUNC(gs_, WSAEventSelect, gs_wsock32dll);
    if ( !gs_WSAEventSelect )
        return false;

    wxDL_INIT_FUNC(gs_, WSACreateEvent, gs_wsock32dll);
    if ( !gs_WSACreateEvent )
        return false;

    wxDL_INIT_FUNC(gs_, WSAWaitForMultipleEvents, gs_wsock32dll);
    if ( !gs_WSAWaitForMultipleEvents )
        return false;

    wxDL_INIT_FUNC(gs_, WSAEnumNetworkEvents, gs_wsock32dll);
    if ( !gs_WSAEnumNetworkEvents )
        return false;

    currSocket = 0;
#endif // !__WXWINCE__/__WXWINCE__

  // finally initialize WinSock
  WSADATA wsaData;
  return WSAStartup((1 << 8) | 1, &wsaData) == 0;
}

void wxSocketMSWManager::OnExit()
{
#ifdef __WXWINCE__
/* Delete the threads here */
    for(unsigned int i=0; i < currSocket; i++)
        CloseHandle(hThread[i]);
#endif
  /* Destroy internal window */
  DestroyWindow(hWin);
  UnregisterClass(CLASSNAME, wxGetInstance());

  WSACleanup();

  gs_wsock32dll.Unload();
}

/* Per-socket GUI initialization / cleanup */

wxSocketImplMSW::wxSocketImplMSW(wxSocketBase& wxsocket)
    : wxSocketImpl(wxsocket)
{
  /* Allocate a new message number for this socket */
  wxCRIT_SECT_LOCKER(lock, gs_critical);

  int i = firstAvailable;
  while (socketList[i] != NULL)
  {
    i = (i + 1) % MAXSOCKETS;

    if (i == firstAvailable)    /* abort! */
    {
      m_msgnumber = 0; // invalid
      return;
    }
  }
  socketList[i] = this;
  firstAvailable = (i + 1) % MAXSOCKETS;
  m_msgnumber = (i + WM_USER);
}

wxSocketImplMSW::~wxSocketImplMSW()
{
  /* Remove the socket from the list */
  wxCRIT_SECT_LOCKER(lock, gs_critical);

  if ( m_msgnumber )
  {
      // we need to remove any pending messages for this socket to avoid having
      // them sent to a new socket which could reuse the same message number as
      // soon as we destroy this one
      MSG msg;
      while ( ::PeekMessage(&msg, hWin, m_msgnumber, m_msgnumber, PM_REMOVE) )
          ;

      socketList[m_msgnumber - WM_USER] = NULL;
  }
  //else: the socket has never been created successfully
}

/* Windows proc for asynchronous event handling */

LRESULT CALLBACK wxSocket_Internal_WinProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
    if ( uMsg < WM_USER || uMsg > (WM_USER + MAXSOCKETS - 1))
        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    wxSocketImplMSW *socket;
    wxSocketNotify event;
    {
        wxCRIT_SECT_LOCKER(lock, gs_critical);

        socket = socketList[(uMsg - WM_USER)];
        event = (wxSocketNotify) -1;

        /* Check that the socket still exists (it has not been
         * destroyed) and for safety, check that the m_fd field
         * is what we expect it to be.
         */
        if ((socket != NULL) && ((WPARAM)socket->m_fd == wParam))
        {
            switch WSAGETSELECTEVENT(lParam)
            {
                case FD_READ:    event = wxSOCKET_INPUT; break;
                case FD_WRITE:   event = wxSOCKET_OUTPUT; break;
                case FD_ACCEPT:  event = wxSOCKET_CONNECTION; break;
                case FD_CONNECT:
                                 {
                                     if (WSAGETSELECTERROR(lParam) != 0)
                                         event = wxSOCKET_LOST;
                                     else
                                         event = wxSOCKET_CONNECTION;
                                     break;
                                 }
                case FD_CLOSE:   event = wxSOCKET_LOST; break;
            }

            if (event != -1)
            {
                if (event == wxSOCKET_LOST)
                    socket->m_detected = wxSOCKET_LOST_FLAG;
                else
                    socket->m_detected |= (1 << event);
            }
        }
    } // unlock gs_critical

    if ( socket )
        socket->NotifyOnStateChange(event);

    return (LRESULT) 0;
}

/*
 *  Enable all event notifications; we need to be notified of all
 *  events for internal processing, but we will only notify users
 *  when an appropriate callback function has been installed.
 */
void wxSocketMSWManager::Install_Callback(wxSocketImpl *socket_,
                                         wxSocketNotify WXUNUSED(event))
{
    wxSocketImplMSW * const socket = static_cast<wxSocketImplMSW *>(socket_);

  if (socket->m_fd != INVALID_SOCKET)
  {
    /* We could probably just subscribe to all events regardless
     * of the socket type, but MS recommends to do it this way.
     */
    long lEvent = socket->m_server?
                  FD_ACCEPT : (FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
#ifndef __WXWINCE__
    gs_WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, lEvent);
#else
/*
*  WinCE creates a thread for socket event handling.
*  All needed parameters get passed through the thread_data structure.
*/

    thread_data* d = new thread_data;
    d->lEvent = lEvent;
    d->hEvtWin = hWin;
    d->msgnumber = socket->m_msgnumber;
    d->fd = socket->m_fd;
    socketHash[socket->m_fd] = true;
    hThread[currSocket++] = CreateThread(NULL, 0, &SocketThread,(LPVOID)d, 0, NULL);
#endif
  }
}

/*
 *  Disable event notifications (used when shutting down the socket)
 */
void wxSocketMSWManager::Uninstall_Callback(wxSocketImpl *socket_,
                                           wxSocketNotify WXUNUSED(event))
{
    wxSocketImplMSW * const socket = static_cast<wxSocketImplMSW *>(socket_);

  if (socket->m_fd != INVALID_SOCKET)
  {
#ifndef __WXWINCE__
    gs_WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, 0);
#else
    //Destroy the thread
    socketHash[socket->m_fd] = false;
#endif
  }
}

// set the wxBase variable to point to our wxSocketManager implementation
//
// see comments in wx/apptrait.h for the explanation of why do we do it
// like this
static struct ManagerSetter
{
    ManagerSetter()
    {
        static wxSocketMSWManager s_manager;
        wxAppTraits::SetDefaultSocketManager(&s_manager);
    }
} gs_managerSetter;

// ============================================================================
// wxSocketImpl implementation
// ============================================================================

/* static */
wxSocketImpl *wxSocketImpl::Create(wxSocketBase& wxsocket)
{
    return new wxSocketImplMSW(wxsocket);
}

void wxSocketImplMSW::DoClose()
{
    wxSocketManager::Get()->
        Uninstall_Callback(this, wxSOCKET_MAX_EVENT /* unused anyhow */);

    closesocket(m_fd);
}

wxSocketError wxSocketImplMSW::GetLastError() const
{
    switch ( WSAGetLastError() )
    {
        case 0:
            return wxSOCKET_NOERROR;

        case WSAENOTSOCK:
            return wxSOCKET_INVSOCK;

        case WSAEWOULDBLOCK:
            return wxSOCKET_WOULDBLOCK;

        default:
            return wxSOCKET_IOERR;
    }
}

/* Generic IO */

/* Like recv(), send(), ... */
int wxSocketImplMSW::Read(void *buffer, int size)
{
  int ret;

  /* Reenable INPUT events */
  m_detected &= ~wxSOCKET_INPUT_FLAG;

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return -1;
  }

  /* Read the data */
  if (m_stream)
    ret = Recv_Stream(buffer, size);
  else
    ret = Recv_Dgram(buffer, size);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      m_error = wxSOCKET_IOERR;
    else
      m_error = wxSOCKET_WOULDBLOCK;
    return -1;
  }

  return ret;
}

int wxSocketImplMSW::Write(const void *buffer, int size)
{
  int ret;

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return -1;
  }

  /* Write the data */
  if (m_stream)
    ret = Send_Stream(buffer, size);
  else
    ret = Send_Dgram(buffer, size);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      m_error = wxSOCKET_IOERR;
    else
      m_error = wxSOCKET_WOULDBLOCK;

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * does). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    m_detected &= ~wxSOCKET_OUTPUT_FLAG;
    return -1;
  }

  return ret;
}

/* Internals (IO) */

/*
 *  For blocking sockets, wait until the connection is
 *  established or fails, or until timeout ellapses.
 */
wxSocketError wxSocketImplMSW::Connect_Timeout()
{
  fd_set writefds;
  fd_set exceptfds;

  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_SET(m_fd, &writefds);
  FD_SET(m_fd, &exceptfds);
  if (select(0, NULL, &writefds, &exceptfds, &m_timeout) == 0)
  {
    m_error = wxSOCKET_TIMEDOUT;
    return wxSOCKET_TIMEDOUT;
  }
  if (!FD_ISSET(m_fd, &writefds))
  {
    m_error = wxSOCKET_IOERR;
    return wxSOCKET_IOERR;
  }

  return wxSOCKET_NOERROR;
}

int wxSocketImplMSW::Recv_Stream(void *buffer, int size)
{
    return recv(m_fd, static_cast<char *>(buffer), size, 0);
}

int wxSocketImplMSW::Recv_Dgram(void *buffer, int size)
{
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  int ret;
  wxSocketError err;

  ret = recvfrom(m_fd, static_cast<char *>(buffer),
                 size, 0, &from, &fromlen);

  if (ret == SOCKET_ERROR)
    return SOCKET_ERROR;

  /* Translate a system address into a wxSocketImpl address */
  if (!m_peer)
  {
    m_peer = GAddress_new();
    if (!m_peer)
    {
      m_error = wxSOCKET_MEMERR;
      return -1;
    }
  }
  err = _GAddress_translate_from(m_peer, (sockaddr*)&from, fromlen);
  if (err != wxSOCKET_NOERROR)
  {
    GAddress_destroy(m_peer);
    m_peer  = NULL;
    m_error = err;
    return -1;
  }

  return ret;
}

int wxSocketImplMSW::Send_Stream(const void *buffer, int size)
{
  return send(m_fd, static_cast<const char *>(buffer), size, 0);
}

int wxSocketImplMSW::Send_Dgram(const void *buffer, int size)
{
  struct sockaddr *addr;
  int len, ret;
  wxSocketError err;

  if (!m_peer)
  {
    m_error = wxSOCKET_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(m_peer, &addr, &len);
  if (err != wxSOCKET_NOERROR)
  {
    m_error = err;
    return -1;
  }

  ret = sendto(m_fd, static_cast<const char *>(buffer), size, 0, addr, len);

  /* Frees memory allocated by _GAddress_translate_to */
  free(addr);

  return ret;
}

/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current address family is either
 * wxSOCKET_NOFAMILY or wxSOCKET_*family*, and if it is wxSOCKET_NOFAMILY, it
 * initalizes it to be a wxSOCKET_*family*. In other cases, it returns
 * an appropiate error code.
 *
 * CHECK_ADDRESS_RETVAL does the same but returning 'retval' on error.
 */
#define CHECK_ADDRESS(address, family)                              \
{                                                                   \
  if (address->m_family == wxSOCKET_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != wxSOCKET_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != wxSOCKET_##family)                          \
  {                                                                 \
    address->m_error = wxSOCKET_INVADDR;                               \
    return wxSOCKET_INVADDR;                                           \
  }                                                                 \
}

#define CHECK_ADDRESS_RETVAL(address, family, retval)               \
{                                                                   \
  if (address->m_family == wxSOCKET_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != wxSOCKET_NOERROR)          \
      return retval;                                                \
  if (address->m_family != wxSOCKET_##family)                          \
  {                                                                 \
    address->m_error = wxSOCKET_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}


GAddress *GAddress_new()
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family = wxSOCKET_NOFAMILY;
  address->m_addr   = NULL;
  address->m_len    = 0;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

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
  if (address->m_addr)
    free(address->m_addr);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
  return address->m_family;
}

wxSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len)
{
  address->m_realfamily = addr->sa_family;
  switch (addr->sa_family)
  {
    case AF_INET:
      address->m_family = wxSOCKET_INET;
      break;
    case AF_UNIX:
      address->m_family = wxSOCKET_UNIX;
      break;
#if wxUSE_IPV6
    case AF_INET6:
      address->m_family = wxSOCKET_INET6;
      break;
#endif
    default:
    {
      address->m_error = wxSOCKET_INVOP;
      return wxSOCKET_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len = len;
  address->m_addr = (struct sockaddr *) malloc(len);

  if (address->m_addr == NULL)
  {
    address->m_error = wxSOCKET_MEMERR;
    return wxSOCKET_MEMERR;
  }
  memcpy(address->m_addr, addr, len);

  return wxSOCKET_NOERROR;
}

wxSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len)
{
  if (!address->m_addr)
  {
    address->m_error = wxSOCKET_INVADDR;
    return wxSOCKET_INVADDR;
  }

  *len = address->m_len;
  *addr = (struct sockaddr *) malloc(address->m_len);
  if (*addr == NULL)
  {
    address->m_error = wxSOCKET_MEMERR;
    return wxSOCKET_MEMERR;
  }

  memcpy(*addr, address->m_addr, address->m_len);
  return wxSOCKET_NOERROR;
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

wxSocketError _GAddress_Init_INET(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_in);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = wxSOCKET_MEMERR;
    return wxSOCKET_MEMERR;
  }

  address->m_family = wxSOCKET_INET;
  address->m_realfamily = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  addr->s_addr = inet_addr(hostname);

  /* If it is a numeric host name, convert it now */
  if (addr->s_addr == INADDR_NONE)
  {
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    if ((he = gethostbyname(hostname)) == NULL)
    {
      /* addr->s_addr = INADDR_NONE just done by inet_addr() above */
      address->m_error = wxSOCKET_NOHOST;
      return wxSOCKET_NOHOST;
    }
    array_addr = (struct in_addr *) *(he->h_addr_list);
    addr->s_addr = array_addr[0].s_addr;
  }
  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetBroadcastAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_BROADCAST);
}

wxSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

wxSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  struct in_addr *addr;

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);
  addr->s_addr = htonl(hostaddr);

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in *addr;

  CHECK_ADDRESS(address, INET);

  if (!port)
  {
    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
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
      return wxSOCKET_NOERROR;
    }

    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
  }

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = se->s_port;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in *addr;

  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = htons(port);

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  struct hostent *he;
  char *addr_buf;
  struct sockaddr_in *addr;

  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin_addr), AF_INET);
  if (he == NULL)
  {
    address->m_error = wxSOCKET_NOHOST;
    return wxSOCKET_NOHOST;
  }

  strncpy(hostname, he->h_name, sbuf);

  return wxSOCKET_NOERROR;
}

unsigned long GAddress_INET_GetHostAddress(GAddress *address)
{
  struct sockaddr_in *addr;

  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohl(addr->sin_addr.s_addr);
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

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
#include "ws2tcpip.h"

#ifdef __VISUALC__
    #pragma comment(lib,"ws2_32")
#endif // __VISUALC__

wxSocketError _GAddress_Init_INET6(GAddress *address)
{
  struct in6_addr any_address = IN6ADDR_ANY_INIT;
  address->m_len  = sizeof(struct sockaddr_in6);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = wxSOCKET_MEMERR;
    return wxSOCKET_MEMERR;
  }
  memset(address->m_addr,0,address->m_len);

  address->m_family = wxSOCKET_INET6;
  address->m_realfamily = AF_INET6;
  ((struct sockaddr_in6 *)address->m_addr)->sin6_family = AF_INET6;
  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = any_address;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetHostName(GAddress *address, const char *hostname)
{
  CHECK_ADDRESS(address, INET6);

  addrinfo hints;
  memset( & hints, 0, sizeof( hints ) );
  hints.ai_family = AF_INET6;
  addrinfo * info = 0;
  if ( getaddrinfo( hostname, "0", & hints, & info ) || ! info )
  {
    address->m_error = wxSOCKET_NOHOST;
    return wxSOCKET_NOHOST;
  }

  memcpy( address->m_addr, info->ai_addr, info->ai_addrlen );
  freeaddrinfo( info );
  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetAnyAddress(GAddress *address)
{
  CHECK_ADDRESS(address, INET6);

  struct in6_addr addr;
  memset( & addr, 0, sizeof( addr ) );
  return GAddress_INET6_SetHostAddress(address, addr);
}
wxSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr)
{
  CHECK_ADDRESS(address, INET6);

  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = hostaddr;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in6 *addr;

  CHECK_ADDRESS(address, INET6);

  if (!port)
  {
    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
  }

  se = getservbyname(port, protocol);
  if (!se)
  {
    if (isdigit((unsigned char) port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in6 *)address->m_addr;
      addr->sin6_port = htons((u_short) port_int);
      return wxSOCKET_NOERROR;
    }

    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
  }

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr->sin6_port = se->s_port;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in6 *addr;

  CHECK_ADDRESS(address, INET6);

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr->sin6_port = htons(port);

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  struct hostent *he;
  char *addr_buf;
  struct sockaddr_in6 *addr;

  CHECK_ADDRESS(address, INET6);

  addr = (struct sockaddr_in6 *)address->m_addr;
  addr_buf = (char *)&(addr->sin6_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin6_addr), AF_INET6);
  if (he == NULL)
  {
    address->m_error = wxSOCKET_NOHOST;
    return wxSOCKET_NOHOST;
  }

  strncpy(hostname, he->h_name, sbuf);

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_GetHostAddress(GAddress *address,struct in6_addr *hostaddr)
{
  CHECK_ADDRESS_RETVAL(address, INET6, wxSOCKET_INVADDR);
  *hostaddr = ( (struct sockaddr_in6 *)address->m_addr )->sin6_addr;
  return wxSOCKET_NOERROR;
}

unsigned short GAddress_INET6_GetPort(GAddress *address)
{
  CHECK_ADDRESS_RETVAL(address, INET6, 0);

  return ntohs( ((struct sockaddr_in6 *)address->m_addr)->sin6_port );
}

#endif // wxUSE_IPV6

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

wxSocketError _GAddress_Init_UNIX(GAddress *address)
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

wxSocketError GAddress_UNIX_SetPath(GAddress *address, const char *WXUNUSED(path))
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

wxSocketError GAddress_UNIX_GetPath(GAddress *address, char *WXUNUSED(path), size_t WXUNUSED(sbuf))
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

#endif  // wxUSE_SOCKETS
