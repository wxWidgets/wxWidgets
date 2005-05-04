/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        gsockmsw.h
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Purpose:     GSocket MSW header
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GSOCK_MSW_H
#define __GSOCK_MSW_H

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__
#include "wx/gsocket.h"
#else
#include "gsocket.h"
#endif

#include <windows.h>
#include <winsock.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Definition of GSocket */
struct _GSocket
{
  SOCKET m_fd;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  /* Attributes */
  int m_non_blocking;
  int m_server;
  int m_stream;
  int m_oriented;
  int m_establishing;
  struct timeval m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];
  int m_msgnumber;
};

/* Definition of GAddress */
struct _GAddress
{
  struct sockaddr *m_addr;
  size_t m_len;

  GAddressType m_family;
  int m_realfamily;

  GSocketError m_error;
};

/* Input / output */

GSocketError _GSocket_Input_Timeout(GSocket *socket);
GSocketError _GSocket_Output_Timeout(GSocket *socket);
GSocketError _GSocket_Connect_Timeout(GSocket *socket);
int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size);
int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size);
int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size);
int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size);

/* Callbacks */

int  _GSocket_GUI_Init(GSocket *socket);
void _GSocket_GUI_Destroy(GSocket *socket);

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND, UINT, WPARAM, LPARAM);

void _GSocket_Enable_Events(GSocket *socket);
void _GSocket_Disable_Events(GSocket *socket);

/* GAddress */

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len);
GSocketError _GAddress_translate_to  (GAddress *address,
                                      struct sockaddr **addr, int *len);
GSocketError _GAddress_Init_INET(GAddress *address);
GSocketError _GAddress_Init_UNIX(GAddress *address);

#ifdef __cplusplus
}
#endif

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

#endif  /* __GSOCK_MSW_H */
