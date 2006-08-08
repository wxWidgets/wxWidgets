/* -------------------------------------------------------------------------
 * Project:     GAddress (Generic Address) for WX
 * Name:        gaddr.cpp
 * Copyright:   (c) Angel Vidal Veiga (kry@amule.org, mantainer)
 * Licence:     wxWindows Licence
 * Authors:     Angel Vidal Veiga (kry@amule.org, mantainer)
 * Purpose:     GAddr generic file
 * Licence:     The wxWindows licence
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#if defined(__WATCOMC__)
#include "wx/wxprec.h"
#include <errno.h>
#include <nerrno.h>
#endif

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include <assert.h>
#include <sys/types.h>
#ifdef __VISAGECPP__
#include <string.h>
#include <sys/time.h>
#include <types.h>
#include <netinet/in.h>
#endif

#ifndef __WINDOWS__
#include <netdb.h>
#include <sys/ioctl.h>
#endif

#ifdef __VMS__
#include <socket.h>
struct sockaddr_un
{
    u_char  sun_len;        /* sockaddr len including null */
    u_char  sun_family;     /* AF_UNIX */
    char    sun_path[108];  /* path name (gag) */
};
#else
#ifndef __WINDOWS__
#include <sys/socket.h>
#include <sys/un.h>
#endif
#endif

#ifndef __VISAGECPP__
#include <sys/time.h>
#ifndef __WINDOWS__
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <string.h>
#include <unistd.h>
#else
#include <nerrno.h>
#  if __IBMCPP__ < 400
#include <machine/endian.h>
#include <socket.h>
#include <ioctl.h>
#include <select.h>
#include <unistd.h>

#    ifdef min
#    undef min
#    endif
#  else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#  endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#ifdef sun
#  include <sys/filio.h>
#endif
#ifdef sgi
#  include <bstring.h>
#endif
#ifdef _AIX
#  include <strings.h>
#endif
#include <signal.h>

/*
 * INADDR_BROADCAST is identical to INADDR_NONE which is not defined
 * on all systems. INADDR_BROADCAST should be fine to indicate an error.
 */
#ifndef INADDR_NONE
#define INADDR_NONE INADDR_BROADCAST
#endif

#include "wx/gaddr.h"
#include "wx/thread.h" // Needed for wxMutex

struct in_addr* wxGethostbyname_r(const char *hostname)

{
  struct hostent *he = NULL;
  struct in_addr* address;

#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
  struct hostent h;
  char buffer[1024];
  int err;
  if (gethostbyname_r(hostname, &h, buffer, 1024, &he, &err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5) 
  struct hostent h;
  char buffer[1024];
  int err;
  he = gethostbyname_r(hostname, &h, (char*)buffer, 1024, &err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3) 
  struct hostent h;
  struct hostent_data buffer;
  if (gethostbyname_r(hostname, &h, &buffer))
    he = NULL;
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  static wxMutex nameLock;
  wxMutexLocker locker(nameLock);
#endif
  he = gethostbyname(hostname);
#endif

  if (!he)
    address = NULL;
  else
  {	
    if (he->h_addr_list[0] == NULL)
      address = NULL;
    else
    {
      address =(struct in_addr*)  malloc(sizeof(struct in_addr));
      memcpy(address,&(((struct in_addr *) *(he->h_addr_list))[0]),sizeof(struct in_addr));
    }
  }

  return address;
}

char* wxGethostbyaddr_r(struct in_addr addr)
{
  struct hostent *he = NULL;
  char* name = NULL;

#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6) 
  struct hostent h;
  char buffer[1024];
  int err;
  if (gethostbyaddr_r(&addr, sizeof(struct in_addr), AF_INET, &h, buffer, 1024, &he, &err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
  struct hostent h;
  char buffer[1024];
  int err;
  he = gethostbyaddr_r(&addr, sizeof(struct in_addr), AF_INET, &h, buffer, 1024, &err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3) 
  struct hostent h;
  struct hostent_data buffer;
  if (gethostbyaddr_r(&addr, sizeof(struct in_addr), AF_INET, &h, &buffer))
    he = NULL;
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  static wxMutex addrLock;
  wxMutexLocker locker(addrLock);
#endif
  // Some versions of gethostbyaddr (specially MacOS 10.3 and *BSD flavours
  // have (char*) as the first parameter of the funtion, as oposed to 
  // (void*) used in MacOS 10.4 and linux. So the cast is needed here,
  // and the (void*) versions will just handle it the same way.
  he = gethostbyaddr((char*)&addr, sizeof(struct in_addr), AF_INET);
#endif

  if (he && he->h_name)
  {
    name = strdup(he->h_name);
  }

  return name;
}

struct servent *wxGetservbyname_r(const char *port, const char *protocol)
{
  struct servent *se = NULL;
  struct servent *result = NULL;
#if defined(HAVE_FUNC_GETSERVBYNAME_R_6)
  struct servent serv;
  char buffer[1024];
  if (getservbyname_r(port, protocol, &serv, buffer, 1024, &se))
    se = NULL;
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_5)
  struct servent serv;
  char buffer[1024];
  se = getservbyname_r(port, protocol, &serv, buffer, 1024);
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_4)
  struct servent serv;
  struct servent_data  buffer;
  if (getservbyname_r(port, protocol, &serv, &buffer))
    se = NULL;
  else
    se = &serv;
#elif defined(HAVE_GETSERVBYNAME)
#if wxUSE_THREADS
  static wxMutex servLock;
  wxMutexLocker locker(servLock);
#endif
  se = getservbyname(port, protocol);
#endif

  if (se)
  {
    result = (struct servent*) malloc(sizeof(struct servent));
    memcpy(result,se,sizeof(struct servent));
  }

  return result;
}

/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current address family is either
 * GSOCK_NOFAMILY or GSOCK_*family*, and if it is GSOCK_NOFAMILY, it
 * initalizes it to be a GSOCK_*family*. In other cases, it returns
 * an appropiate error code.
 *
 * CHECK_ADDRESS_RETVAL does the same but returning 'retval' on error.
 */
#define CHECK_ADDRESS(address, family)                              \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return GSOCK_INVADDR;                                           \
  }                                                                 \
}

#define CHECK_ADDRESS_RETVAL(address, family, retval)               \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return retval;                                                \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}


GAddress *GAddress_new(void)
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family  = GSOCK_NOFAMILY;
  address->m_addr    = NULL;
  address->m_len     = 0;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

  assert(address != NULL);

  if ((addr2 = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr && address->m_len > 0)
  {
    addr2->m_addr = (struct sockaddr *)malloc(addr2->m_len);
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
  assert(address != NULL);

  if (address->m_addr)
    free(address->m_addr);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  assert(address != NULL);

  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
  assert(address != NULL);

  return address->m_family;
}

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len)
{
  address->m_realfamily = addr->sa_family;
  switch (addr->sa_family)
  {
    case AF_INET:
      address->m_family = GSOCK_INET;
      break;
    case AF_UNIX:
      address->m_family = GSOCK_UNIX;
      break;
#ifdef AF_INET6
    case AF_INET6:
      address->m_family = GSOCK_INET6;
      break;
#endif
    default:
    {
      address->m_error = GSOCK_INVOP;
      return GSOCK_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len  = len;
  address->m_addr = (struct sockaddr *)malloc(len);

  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(address->m_addr, addr, len);

  return GSOCK_NOERROR;
}

GSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len)
{
  if (!address->m_addr)
  {
    address->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  *len = address->m_len;
  *addr = (struct sockaddr *)malloc(address->m_len);
  if (*addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(*addr, address->m_addr, address->m_len);
  return GSOCK_NOERROR;
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_INET(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_in);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_family = GSOCK_INET;
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{  
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  /* If it is a numeric host name, convert it now */
#if defined(HAVE_INET_ATON)
  if (inet_aton(hostname, addr) == 0)
  {
#elif defined(HAVE_INET_ADDR) || defined (__WXMSW__)
  if ( (addr->s_addr = inet_addr(hostname)) == INADDR_NONE )
  {
#else
  /* Use gethostbyname by default */
#ifndef __WXMAC__
  int val = 1;  /* VA doesn't like constants in conditional expressions */
  if (val)
#endif
  {
#endif    

    /* It is a real name, we solve it */
    
    struct in_addr* newaddr = wxGethostbyname_r(hostname);
    if (!newaddr)
    {
      /* Reset to invalid address */
      addr->s_addr = INADDR_NONE;
      address->m_error = GSOCK_NOHOST;
      return GSOCK_NOHOST;
    }
    else
    {
      addr->s_addr = newaddr->s_addr;
      free(newaddr);
    }
  }

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);
  addr->s_addr = htonl(hostaddr);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  se = wxGetservbyname_r(port, protocol);
  if (!se)
  {
    /* the cast to int suppresses compiler warnings about subscript having the
       type char */
    if (isdigit((int)port[0]))
    {
      int port_int;

      port_int = atoi(port);
      ((struct sockaddr_in *)address->m_addr)->sin_port = htons(port_int);
      return GSOCK_NOERROR;
    }

    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }
  else
  {
    ((struct sockaddr_in *)address->m_addr)->sin_port = se->s_port;
    free(se);
  }

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = htons(port);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_GetHostName(GAddress *address, char **hostname)
{

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  *hostname = wxGethostbyaddr_r(((struct sockaddr_in *)address->m_addr)->sin_addr);
  if (*hostname == NULL)
  {
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }

  return GSOCK_NOERROR;
}

unsigned long GAddress_INET_GetHostAddress(GAddress *address)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohl(addr->sin_addr.s_addr);
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;
  return ntohs(addr->sin_port);
}

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

#ifndef __VISAGECPP__
GSocketError _GAddress_Init_UNIX(GAddress *address)
{
#ifdef __WINDOWS__
  assert(address);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
#else
  address->m_len  = sizeof(struct sockaddr_un);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_family = GSOCK_UNIX;
  address->m_realfamily = PF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_family = AF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_path[0] = 0;

  return GSOCK_NOERROR;
#endif
}

#define UNIX_SOCK_PATHLEN (sizeof(addr->sun_path)/sizeof(addr->sun_path[0]))

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
#ifdef __WINDOWS__
  assert(address);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
#else
  struct sockaddr_un *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, UNIX);

  addr = ((struct sockaddr_un *)address->m_addr);
  strncpy(addr->sun_path, path, UNIX_SOCK_PATHLEN);
  addr->sun_path[UNIX_SOCK_PATHLEN - 1] = '\0';

  return GSOCK_NOERROR;
#endif
}

GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
#ifdef __WINDOWS__
  assert(address);
  address->m_error = GSOCK_INVADDR;
  return GSOCK_INVADDR;
#else
  struct sockaddr_un *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, UNIX);

  addr = (struct sockaddr_un *)address->m_addr;

  strncpy(path, addr->sun_path, sbuf);

  return GSOCK_NOERROR;
#endif
}

#endif /* __VISAGECPP__ */

#endif  /* wxUSE_SOCKETS */
