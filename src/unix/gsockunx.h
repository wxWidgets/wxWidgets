#ifndef __GSOCK_UNX_H
#define __GSOCK_UNX_H

#include <wx/gsocket.h>

/* Definition of GSocket */
struct _GSocket {
  int m_fd;
  GAddress *m_local, *m_peer;
  GSocketError m_error;

  bool m_blocking, m_server, m_stream, m_oriented;

  /* Fallbacks */
  GSocketFallback m_fbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];

  /* IO calls associated */
  bool m_iocalls[GSOCK_MAX_EVENT];

  char *m_gui_dependent;
};

/* Definition of GAddress */
struct _GAddress {
  struct sockaddr *m_addr;
  size_t m_len;

  GAddressType m_family;
  int m_realfamily;

  GSocketError m_error;
};

void _GSocket_Enable(GSocket *socket, GSocketEvent event);
void _GSocket_Disable(GSocket *socket, GSocketEvent event);
int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size);
int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size);
int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size);
int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size);
void _GSocket_Install_Fallback(GSocket *socket, GSocketEvent count);
void _GSocket_Uninstall_Fallback(GSocket *socket, GSocketEvent count);
void _GSocket_Detected_Read(GSocket *socket);
void _GSocket_Detected_Write(GSocket *socket);
void _GSocket_GUI_Init(GSocket *socket);
void _GSocket_GUI_Destroy(GSocket *socket);
void _GAddress_translate_from(GAddress *address,
                              struct sockaddr *addr, int len);
void _GAddress_translate_to(GAddress *address,
                            struct sockaddr **addr, int *len);

/* Internet address family */
void _GAddress_Init_INET(GAddress *address);
/* Local address family */
void _GAddress_Init_UNIX(GAddress *address);

#endif
