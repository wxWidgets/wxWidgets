/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockunx.h
 * Purpose: GSocket Unix header
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GSOCK_UNX_H
#define __GSOCK_UNX_H

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__
#include "wx/gsocket.h"
#else
#include "gsocket.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Definition of GSocket */
struct _GSocket
{
  int m_fd;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  bool m_non_blocking;
  bool m_server;
  bool m_stream;
  bool m_oriented;
  bool m_establishing;
  unsigned long m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];

  char *m_gui_dependent;
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

/* Input / Output */

GSocketError _GSocket_Input_Timeout(GSocket *socket);
GSocketError _GSocket_Output_Timeout(GSocket *socket);
int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size);
int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size);
int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size);
int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size);

/* Callbacks */

void _GSocket_Enable(GSocket *socket, GSocketEvent event);
void _GSocket_Disable(GSocket *socket, GSocketEvent event);
void _GSocket_Detected_Read(GSocket *socket);
void _GSocket_Detected_Write(GSocket *socket);

void _GSocket_GUI_Init(GSocket *socket);
void _GSocket_GUI_Destroy(GSocket *socket);
void _GSocket_Enable_Events(GSocket *socket);
void _GSocket_Disable_Events(GSocket *socket);
void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event);
void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event);

/* GAddress */

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len);
GSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len);

GSocketError _GAddress_Init_INET(GAddress *address);
GSocketError _GAddress_Init_UNIX(GAddress *address);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

#endif  /* __GSOCK_UNX_H */
