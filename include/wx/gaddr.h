/* -------------------------------------------------------------------------
 * Project:     GAddress (Generic Address)
 * Name:        gaddr.h
 * Author:      Angel Vidal Veiga <kry@amule.org> (mantainer)
 * Copyright:   (c) Angel Vidal Veiga
 * Licence:     wxWindows Licence
 * Purpose:     GSocket include file (system independent)
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GADDR_H
#define __GADDR_H

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/dlimpexp.h" /* for WXDLLIMPEXP_NET */
#include "wx/setup.h"
#include "wx/gsocket.h"

#include <stddef.h>

/*
   Including sys/types.h under cygwin results in the warnings about "fd_set
   having been defined in sys/types.h" when winsock.h is included later and
   doesn't seem to be necessary anyhow. It's not needed under Mac neither.
 */
#if !defined(__WXMAC__) && !defined(__CYGWIN__) && !defined(__WXWINCE__)
#include <sys/types.h>
#endif

typedef enum {
  GSOCK_NOFAMILY = 0,
  GSOCK_INET,
  GSOCK_INET6,
  GSOCK_UNIX
} GAddressType;

/* GAddress */

GAddress *GAddress_new(void);
GAddress *GAddress_copy(GAddress *address);
void GAddress_destroy(GAddress *address);

void GAddress_SetFamily(GAddress *address, GAddressType type);
GAddressType GAddress_GetFamily(GAddress *address);

/* The use of any of the next functions will set the address family to
 * the specific one. For example if you use GAddress_INET_SetHostName,
 * address family will be implicitly set to AF_INET.
 */

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname);
GSocketError GAddress_INET_SetAnyAddress(GAddress *address);
GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr);
GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol);
GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port);

GSocketError GAddress_INET_GetHostName(GAddress *address, char **hostname);
unsigned long GAddress_INET_GetHostAddress(GAddress *address);
unsigned short GAddress_INET_GetPort(GAddress *address);

/* TODO: Define specific parts (INET6, UNIX) */

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path);
GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf);

/* Definition of GAddress */
struct _GAddress
{
  struct sockaddr *m_addr;
  size_t m_len;

  GAddressType m_family;
  int m_realfamily;

  GSocketError m_error;
};

/* GAddress */

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len);
GSocketError _GAddress_translate_to  (GAddress *address,
                                      struct sockaddr **addr, int *len);
GSocketError _GAddress_Init_INET(GAddress *address);
GSocketError _GAddress_Init_UNIX(GAddress *address);


#endif  /* wxUSE_SOCKETS */

#endif    /* __GADDR_H */
