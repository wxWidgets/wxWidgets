/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockunx.h
 * Purpose: GSocket Macintosh header
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

typedef GSocket* GSocketPtr ;

/* Definition of GSocket */
struct _GSocket
{
  wxMacNotifierTableRef m_mac_events ;
  EndpointRef m_endpoint;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  int m_non_blocking;
  int m_server;
  int m_stream;
  int m_oriented;
  unsigned long m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];
  int m_takesEvents ;
};

/* Definition of GAddress */

struct _GAddress
{
  UInt32 		m_host ;
  UInt16 		m_port ;
  GAddressType m_family;
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

void _GSocket_Enable_Events(GSocket *socket);
void _GSocket_Disable_Events(GSocket *socket);
void _GSocket_Internal_Proc(unsigned long e , void* data ) ;

/* GAddress */

GSocketError _GAddress_translate_from(GAddress *address,
                                      InetAddress *addr );
GSocketError _GAddress_translate_to(GAddress *address,
                                    InetAddress *addr);

GSocketError _GAddress_Init_INET(GAddress *address);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

#endif  /* __GSOCK_UNX_H */
