/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket)
 * Name:    gsockmsw.c
 * Author:  Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose: GSocket GUI-specific MSW code
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

/*
 * PLEASE don't put C++ comments here - this is a C source file.
 */

/* including rasasync.h (included from windows.h itself included from
 * wx/setup.h and/or winsock.h results in this warning for
 * RPCNOTIFICATION_ROUTINE
 */
#ifdef _MSC_VER
#  pragma warning(disable:4115) /* named type definition in parentheses */
#endif

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__

#include "wx/msw/gsockmsw.h"
#include "wx/gsocket.h"

#define INSTANCE wxGetInstance()

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

#ifdef _MSC_VER
#  pragma warning(default:4115) /* named type definition in parentheses */
#endif

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

int GSocket_Init(void)
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

void GSocket_Cleanup(void)
{
  /* Destroy internal window */
  DestroyWindow(hWin);
  UnregisterClass(CLASSNAME, INSTANCE);

  /* Delete critical section */
  DeleteCriticalSection(&critical);

  /* Cleanup WinSocket */
  WSACleanup();
}

/* Per-socket GUI initialization / cleanup */

bool _GSocket_GUI_Init(GSocket *socket)
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
      return FALSE;
    }
  }
  socketList[i] = socket;
  firstAvailable = (i + 1) % MAXSOCKETS;
  socket->m_msgnumber = (i + WM_USER);

  LeaveCriticalSection(&critical);

  return TRUE;
}

void _GSocket_GUI_Destroy(GSocket *socket)
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
    event = -1;
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
void _GSocket_Enable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    /* We could probably just subscribe to all events regardless
     * of the socket type, but MS recommends to do it this way.
     */
    long lEvent = socket->m_server?
                  FD_ACCEPT : (FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);

    WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, lEvent);
  }
}

/* _GSocket_Disable_Events:
 *  Disable event notifications (when shutdowning the socket)
 */
void _GSocket_Disable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    WSAAsyncSelect(socket->m_fd, hWin, socket->m_msgnumber, 0);
  }
}

#else /* !wxUSE_SOCKETS */

/* 
 * Translation unit shouldn't be empty, so include this typedef to make the
 * compiler (VC++ 6.0, for example) happy
 */
typedef (*wxDummy)();

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */
