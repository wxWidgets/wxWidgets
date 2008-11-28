/////////////////////////////////////////////////////////////////////////////
// Name:       src/msw/gsockmsw.cpp
// Purpose:    MSW-specific socket support
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

/*
 * DONE: for WinCE we need to replace WSAAsyncSelect
 * (Windows message-based notification of network events for a socket)
 * with another mechanism.
 * As WSAAsyncSelect is not present on WinCE, it now uses
 * WSACreateEvent, WSAEventSelect, WSAWaitForMultipleEvents and WSAEnumNetworkEvents.
 * When enabling eventhandling for a socket a new thread it created that keeps track of the events
 * and posts a messageto the hidden window to use the standard message loop.
 */

/* including rasasync.h (included from windows.h itself included from
 * wx/setup.h and/or winsock.h results in this warning for
 * RPCNOTIFICATION_ROUTINE
 */
#ifdef _MSC_VER
#   pragma warning(disable:4115) /* named type definition in parentheses */
#endif

#include "wx/private/gsocket.h"
#include "wx/apptrait.h"
#include "wx/link.h"

wxFORCE_LINK_THIS_MODULE(gsockmsw)

extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance();
#define INSTANCE wxGetInstance()

#ifdef __WXWINCE__
#include <winsock.h>
#include "wx/msw/wince/net.h"
#include "wx/hashmap.h"
WX_DECLARE_HASH_MAP(int,bool,wxIntegerHash,wxIntegerEqual,SocketHash);
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <winsock.h>

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
typedef int (PASCAL *WSAAsyncSelectFunc)(SOCKET,HWND,u_int,long);
#else
/* Typedef the needed function prototypes and the WSANETWORKEVENTS structure
*/
typedef struct _WSANETWORKEVENTS {
       long lNetworkEvents;
       int iErrorCode[10];
} WSANETWORKEVENTS, FAR * LPWSANETWORKEVENTS;
typedef HANDLE (PASCAL *WSACreateEventFunc)();
typedef int (PASCAL *WSAEventSelectFunc)(SOCKET,HANDLE,long);
typedef int (PASCAL *WSAWaitForMultipleEventsFunc)(long,HANDLE,BOOL,long,BOOL);
typedef int (PASCAL *WSAEnumNetworkEventsFunc)(SOCKET,HANDLE,LPWSANETWORKEVENTS);
#endif //__WXWINCE__

LRESULT CALLBACK wxSocket_Internal_WinProc(HWND, UINT, WPARAM, LPARAM);

/* Global variables */

static HWND hWin;
static CRITICAL_SECTION critical;
static wxSocketImplMSW *socketList[MAXSOCKETS];
static int firstAvailable;

#ifndef __WXWINCE__
static WSAAsyncSelectFunc gs_WSAAsyncSelect = NULL;
#else
static SocketHash socketHash;
static unsigned int currSocket;
HANDLE hThread[MAXSOCKETS];
static WSACreateEventFunc gs_WSACreateEvent = NULL;
static WSAEventSelectFunc gs_WSAEventSelect = NULL;
static WSAWaitForMultipleEventsFunc gs_WSAWaitForMultipleEvents = NULL;
static WSAEnumNetworkEventsFunc gs_WSAEnumNetworkEvents = NULL;
/* This structure will be used to pass data on to the thread that handles socket events.
*/
typedef struct thread_data{
    HWND hEvtWin;
    unsigned long msgnumber;
    unsigned long fd;
    unsigned long lEvent;
}thread_data;
#endif

static HMODULE gs_wsock32dll = 0;


#ifdef __WXWINCE__
/* This thread handles socket events on WinCE using WSAEventSelect() as WSAAsyncSelect is not supported.
*  When an event occures for the socket, it is checked what kind of event happend and the correct message gets posted
*  so that the hidden window can handle it as it would in other MSW builds.
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
};

/* Global initializers */

bool wxSocketMSWManager::OnInit()
{
  static LPCTSTR pclassname = NULL;
  int i;

  /* Create internal window for event notifications */
  hWin = wxCreateHiddenWindow(&pclassname, CLASSNAME, wxSocket_Internal_WinProc);
  if (!hWin)
      return false;

  /* Initialize socket list */
  InitializeCriticalSection(&critical);

  for (i = 0; i < MAXSOCKETS; i++)
  {
    socketList[i] = NULL;
  }
  firstAvailable = 0;

  /* Load WSAAsyncSelect from wsock32.dll (we don't link against it
     statically to avoid dependency on wsock32.dll for apps that don't use
     sockets): */
#ifndef __WXWINCE__
  gs_wsock32dll = LoadLibrary(wxT("wsock32.dll"));
  if (!gs_wsock32dll)
      return false;
  gs_WSAAsyncSelect =(WSAAsyncSelectFunc)GetProcAddress(gs_wsock32dll,
                                                        "WSAAsyncSelect");
  if (!gs_WSAAsyncSelect)
      return false;
#else
/*  On WinCE we load ws2.dll which will provide the needed functions.
*/
  gs_wsock32dll = LoadLibrary(wxT("ws2.dll"));
  if (!gs_wsock32dll)
      return false;
  gs_WSAEventSelect =(WSAEventSelectFunc)GetProcAddress(gs_wsock32dll,
                                                        wxT("WSAEventSelect"));
  if (!gs_WSAEventSelect)
      return false;

  gs_WSACreateEvent =(WSACreateEventFunc)GetProcAddress(gs_wsock32dll,
                                                        wxT("WSACreateEvent"));
  if (!gs_WSACreateEvent)
      return false;

  gs_WSAWaitForMultipleEvents =(WSAWaitForMultipleEventsFunc)GetProcAddress(gs_wsock32dll,
                                                                            wxT("WSAWaitForMultipleEvents"));
  if (!gs_WSAWaitForMultipleEvents)
      return false;

  gs_WSAEnumNetworkEvents =(WSAEnumNetworkEventsFunc)GetProcAddress(gs_wsock32dll,
                                                                    wxT("WSAEnumNetworkEvents"));
  if (!gs_WSAEnumNetworkEvents)
      return false;

  currSocket = 0;
#endif

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
  UnregisterClass(CLASSNAME, INSTANCE);

  /* Unlock wsock32.dll */
  if (gs_wsock32dll)
  {
      FreeLibrary(gs_wsock32dll);
      gs_wsock32dll = 0;
  }

  /* Delete critical section */
  DeleteCriticalSection(&critical);

  WSACleanup();
}

/* Per-socket GUI initialization / cleanup */

wxSocketImplMSW::wxSocketImplMSW(wxSocketBase& wxsocket)
    : wxSocketImpl(wxsocket)
{
  /* Allocate a new message number for this socket */
  EnterCriticalSection(&critical);

  int i = firstAvailable;
  while (socketList[i] != NULL)
  {
    i = (i + 1) % MAXSOCKETS;

    if (i == firstAvailable)    /* abort! */
    {
      LeaveCriticalSection(&critical);
      m_msgnumber = 0; // invalid
      return;
    }
  }
  socketList[i] = this;
  firstAvailable = (i + 1) % MAXSOCKETS;
  m_msgnumber = (i + WM_USER);

  LeaveCriticalSection(&critical);
}

wxSocketImplMSW::~wxSocketImplMSW()
{
  /* Remove the socket from the list */
  EnterCriticalSection(&critical);

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

  LeaveCriticalSection(&critical);
}

/* Windows proc for asynchronous event handling */

LRESULT CALLBACK wxSocket_Internal_WinProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
  wxSocketImplMSW *socket;
  wxSocketNotify event;

  if (uMsg >= WM_USER && uMsg <= (WM_USER + MAXSOCKETS - 1))
  {
    EnterCriticalSection(&critical);
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

    LeaveCriticalSection(&critical);

    if ( socket )
        socket->NotifyOnStateChange(event);

    return (LRESULT) 0;
  }
  else
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

#endif  // wxUSE_SOCKETS
