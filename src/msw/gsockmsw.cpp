/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket)
 * Name:    gsockmsw.cpp
 * Author:  Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose: GSocket GUI-specific MSW code
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

/*
 * TODO: for WinCE we need to replace WSAAsyncSelect
 * (Windows message-based notification of network events for a socket)
 * with another mechanism.
 * We may need to have a separate thread that polls for socket events
 * using select() and sends a message to the main thread.
 */

/* including rasasync.h (included from windows.h itself included from
 * wx/setup.h and/or winsock.h results in this warning for
 * RPCNOTIFICATION_ROUTINE
 */
#ifdef _MSC_VER
#   pragma warning(disable:4115) /* named type definition in parentheses */
#endif

/* This needs to be before the wx/defs/h inclusion
 * for some reason
 */

#ifdef __WXWINCE__
    /* windows.h results in tons of warnings at max warning level */
#   ifdef _MSC_VER
#       pragma warning(push, 1)
#   endif
#   include <windows.h>
#   ifdef _MSC_VER
#       pragma warning(pop)
#       pragma warning(disable:4514)
#   endif
#endif

#ifndef __GSOCKET_STANDALONE__
#   include "wx/platform.h"
#   include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__

#include "wx/msw/gsockmsw.h"
#include "wx/gsocket.h"

extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance(void);
#define INSTANCE wxGetInstance()

#else

#include "gsockmsw.h"
#include "gsocket.h"

/* If not using wxWidgets, a global var called hInst must
 * be available and it must contain the app's instance
 * handle.
 */
#define INSTANCE hInst

#endif /* __GSOCKET_STANDALONE__ */

#ifndef __WXWINCE__
#include <assert.h>
#else
#define assert(x)
#include <winsock.h>
#include "wx/msw/wince/net.h"
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

#define CLASSNAME  TEXT("_GSocket_Internal_Window_Class")

/* implemented in utils.cpp */
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

/* Maximum number of different GSocket objects at a given time.
 * This value can be modified at will, but it CANNOT be greater
 * than (0x7FFF - WM_USER + 1)
 */
#define MAXSOCKETS 1024

#if (MAXSOCKETS > (0x7FFF - WM_USER + 1))
#error "MAXSOCKETS is too big!"
#endif

typedef int (PASCAL *WSAAsyncSelectFunc)(SOCKET,HWND,u_int,long);

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND, UINT, WPARAM, LPARAM);

/* Global variables */

extern HINSTANCE INSTANCE;
static HWND hWin;
static CRITICAL_SECTION critical;
static GSocket* socketList[MAXSOCKETS];
static int firstAvailable;
static WSAAsyncSelectFunc gs_WSAAsyncSelect = NULL;
static HMODULE gs_wsock32dll = 0;

bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{   return true; }

/* Global initializers */

bool GSocketGUIFunctionsTableConcrete::OnInit()
{
  static LPCTSTR pclassname = NULL;
  int i;

  /* Create internal window for event notifications */
  hWin = wxCreateHiddenWindow(&pclassname, CLASSNAME, _GSocket_Internal_WinProc);
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
  gs_wsock32dll = LoadLibraryA("wsock32.dll");
  if (!gs_wsock32dll)
      return false;
  gs_WSAAsyncSelect =(WSAAsyncSelectFunc)GetProcAddress(gs_wsock32dll,
                                                        "WSAAsyncSelect");
  if (!gs_WSAAsyncSelect)
      return false;

  return true;
}

void GSocketGUIFunctionsTableConcrete::OnExit()
{
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
}

/* Per-socket GUI initialization / cleanup */

bool GSocketGUIFunctionsTableConcrete::Init_Socket(GSocket *socket)
{
  int i;

  /* Allocate a new message number for this socket */
  EnterCriticalSection(&critical);

  i = firstAvailable;
  while (socketList[i] != NULL)
  {
    i = (i + 1) % MAXSOCKETS;

    if (i == firstAvailable)    /* abort! */
    {
      LeaveCriticalSection(&critical);
      return false;
    }
  }
  socketList[i] = socket;
  firstAvailable = (i + 1) % MAXSOCKETS;
  socket->m_msgnumber = (i + WM_USER);

  LeaveCriticalSection(&critical);

  return true;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  /* Remove the socket from the list */
  EnterCriticalSection(&critical);
  socketList[(socket->m_msgnumber - WM_USER)] = NULL;
  LeaveCriticalSection(&critical);
}

/* Windows proc for asynchronous event handling */

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
  GSocket *socket;
  GSocketEvent event;
  GSocketCallback cback;
  char *data;

  if (uMsg >= WM_USER && uMsg <= (WM_USER + MAXSOCKETS - 1))
  {
    EnterCriticalSection(&critical);
    socket = socketList[(uMsg - WM_USER)];
    event = (GSocketEvent) -1;
    cback = NULL;
    data = NULL;

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
      {
        cback = socket->m_cbacks[event];
        data = socket->m_data[event];

        if (event == GSOCK_LOST)
          socket->m_detected = GSOCK_LOST_FLAG;
        else
          socket->m_detected |= (1 << event);
      }
    }

    /* OK, we can now leave the critical section because we have
     * already obtained the callback address (we make no further
     * accesses to socket->whatever). However, the app should
     * be prepared to handle events from a socket that has just
     * been closed!
     */
    LeaveCriticalSection(&critical);

    if (cback != NULL)
      (cback)(socket, event, data);

    return (LRESULT) 0;
  }
  else
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/* _GSocket_Enable_Events:
 *  Enable all event notifications; we need to be notified of all
 *  events for internal processing, but we will only notify users
 *  when an appropiate callback function has been installed.
 */
void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    /* We could probably just subscribe to all events regardless
     * of the socket type, but MS recommends to do it this way.
     */
    long lEvent = socket->m_server?
                  FD_ACCEPT : (FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);

    gs_WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, lEvent);
  }
}

/* _GSocket_Disable_Events:
 *  Disable event notifications (when shutdowning the socket)
 */
void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    gs_WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, 0);
  }
}

#else /* !wxUSE_SOCKETS */

/*
 * Translation unit shouldn't be empty, so include this typedef to make the
 * compiler (VC++ 6.0, for example) happy
 */
typedef void (*wxDummy)();

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */
