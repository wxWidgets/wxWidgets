/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/sockunix.cpp
// Purpose:     wxSocketImpl implementation for Unix systems
// Authors:     Guilhem Lavaux, Guillermo Rodriguez Garcia, David Elliott,
//              Vadim Zeitlin
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Guilhem Lavaux
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include "wx/private/fd.h"
#include "wx/private/socket.h"
#include "wx/unix/private/sockunix.h"

#if defined(__VISAGECPP__)
#define BSD_SELECT /* use Berkeley Sockets select */
#endif

#if defined(__WATCOMC__)
#include <errno.h>
#include <nerrno.h>
#endif

#include <assert.h>
#include <sys/types.h>
#ifdef __VISAGECPP__
#include <string.h>
#include <sys/time.h>
#include <types.h>
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <sys/ioctl.h>

#ifdef HAVE_SYS_SELECT_H
#   include <sys/select.h>
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
#include <sys/socket.h>
#include <sys/un.h>
#endif

#ifndef __VISAGECPP__
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

#define EBADF   SOCEBADF

#    ifdef min
#    undef min
#    endif
#  else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#define close(a) soclose(a)
#define select(a,b,c,d,e) bsdselect(a,b,c,d,e)
int _System bsdselect(int,
                      struct fd_set *,
                      struct fd_set *,
                      struct fd_set *,
                      struct timeval *);
int _System soclose(int);
#  endif
#endif
#ifdef __EMX__
#include <sys/select.h>
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

#ifndef WX_SOCKLEN_T

#ifdef VMS
#  define WX_SOCKLEN_T unsigned int
#else
#  ifdef __GLIBC__
#    if __GLIBC__ == 2
#      define WX_SOCKLEN_T socklen_t
#    endif
#  elif defined(__WXMAC__)
#    define WX_SOCKLEN_T socklen_t
#  else
#    define WX_SOCKLEN_T int
#  endif
#endif

#endif /* SOCKLEN_T */

#ifndef SOCKOPTLEN_T
#define SOCKOPTLEN_T WX_SOCKLEN_T
#endif

/* UnixWare reportedly needs this for FIONBIO definition */
#ifdef __UNIXWARE__
#include <sys/filio.h>
#endif

/*
 * INADDR_BROADCAST is identical to INADDR_NONE which is not defined
 * on all systems. INADDR_BROADCAST should be fine to indicate an error.
 */
#ifndef INADDR_NONE
#define INADDR_NONE INADDR_BROADCAST
#endif

#if defined(__VISAGECPP__) || defined(__WATCOMC__)

    #define MASK_SIGNAL() {
    #define UNMASK_SIGNAL() }

#else
    extern "C" { typedef void (*wxSigHandler)(int); }

    #define MASK_SIGNAL()                       \
    {                                           \
        wxSigHandler old_handler = signal(SIGPIPE, SIG_IGN);

    #define UNMASK_SIGNAL()                     \
        signal(SIGPIPE, old_handler);           \
    }

#endif

/* If a SIGPIPE is issued by a socket call on a remotely closed socket,
   the program will "crash" unless it explicitly handles the SIGPIPE.
   By using MSG_NOSIGNAL, the SIGPIPE is suppressed. Later, we will
   use SO_NOSIGPIPE (if available), the BSD equivalent. */
#ifdef MSG_NOSIGNAL
#  define GSOCKET_MSG_NOSIGNAL MSG_NOSIGNAL
#else /* MSG_NOSIGNAL not available (FreeBSD including OS X) */
#  define GSOCKET_MSG_NOSIGNAL 0
#endif /* MSG_NOSIGNAL */

#if wxUSE_THREADS && (defined(HAVE_GETHOSTBYNAME) || defined(HAVE_GETSERVBYNAME))
#  include "wx/thread.h"
#endif

#if defined(HAVE_GETHOSTBYNAME)
static struct hostent * deepCopyHostent(struct hostent *h,
					const struct hostent *he,
					char *buffer, int size, int *err)
{
  /* copy old structure */
  memcpy(h, he, sizeof(struct hostent));

  /* copy name */
  int len = strlen(h->h_name);
  if (len > size)
  {
    *err = ENOMEM;
    return NULL;
  }
  memcpy(buffer, h->h_name, len);
  buffer[len] = '\0';
  h->h_name = buffer;

  /* track position in the buffer */
  int pos = len + 1;

  /* reuse len to store address length */
  len = h->h_length;

  /* ensure pointer alignment */
  unsigned int misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  char **p = h->h_addr_list, **q;
  char **h_addr_list = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy addresses and fill new pointer list */
  for (p = h->h_addr_list, q = h_addr_list; *p != 0; p++, q++)
  {
    if (size < pos + len)
    {
      *err = ENOMEM;
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len;
  }
  *++q = 0; /* null terminate the pointer list */
  h->h_addr_list = h_addr_list; /* copy pointer to pointers */

  /* ensure word alignment of pointers */
  misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  p = h->h_aliases;
  char **h_aliases = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy aliases and fill new pointer list */
  for (p = h->h_aliases, q = h_aliases; *p != 0; p++, q++)
  {
    len = strlen(*p);
    if (size <= pos + len)
    {
      *err = ENOMEM;
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    buffer[pos + len] = '\0';
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len + 1;
  }
  *++q = 0; /* null terminate the pointer list */
  h->h_aliases = h_aliases; /* copy pointer to pointers */

  return h;
}
#endif

#if defined(HAVE_GETHOSTBYNAME) && wxUSE_THREADS
static wxMutex nameLock;
#endif
struct hostent * wxGethostbyname_r(const char *hostname, struct hostent *h,
				   void *buffer, int size, int *err)

{
  struct hostent *he = NULL;
  *err = 0;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
  if (gethostbyname_r(hostname, h, (char*)buffer, size, &he, err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
  he = gethostbyname_r(hostname, h, (char*)buffer, size, err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  if (gethostbyname_r(hostname, h, (struct hostent_data*) buffer))
  {
    he = NULL;
    *err = h_errno;
  }
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(nameLock);
#endif
  he = gethostbyname(hostname);
  if (!he)
    *err = h_errno;
  else
    he = deepCopyHostent(h, he, (char*)buffer, size, err);
#endif
  return he;
}

#if defined(HAVE_GETHOSTBYNAME) && wxUSE_THREADS
static wxMutex addrLock;
#endif
struct hostent * wxGethostbyaddr_r(const char *addr_buf, int buf_size,
				   int proto, struct hostent *h,
				   void *buffer, int size, int *err)
{
  struct hostent *he = NULL;
  *err = 0;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
  if (gethostbyaddr_r(addr_buf, buf_size, proto, h,
		      (char*)buffer, size, &he, err))
    he = NULL;
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
  he = gethostbyaddr_r(addr_buf, buf_size, proto, h, (char*)buffer, size, err);
#elif defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  if (gethostbyaddr_r(addr_buf, buf_size, proto, h,
			(struct hostent_data*) buffer))
  {
    he = NULL;
    *err = h_errno;
  }
  else
    he = h;
#elif defined(HAVE_GETHOSTBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(addrLock);
#endif
  he = gethostbyaddr(addr_buf, buf_size, proto);
  if (!he)
    *err = h_errno;
  else
    he = deepCopyHostent(h, he, (char*)buffer, size, err);
#endif
  return he;
}

#if defined(HAVE_GETSERVBYNAME)
static struct servent * deepCopyServent(struct servent *s,
					const struct servent *se,
					char *buffer, int size)
{
  /* copy plain old structure */
  memcpy(s, se, sizeof(struct servent));

  /* copy name */
  int len = strlen(s->s_name);
  if (len >= size)
  {
    return NULL;
  }
  memcpy(buffer, s->s_name, len);
  buffer[len] = '\0';
  s->s_name = buffer;

  /* track position in the buffer */
  int pos = len + 1;

  /* copy protocol */
  len = strlen(s->s_proto);
  if (pos + len >= size)
  {
    return NULL;
  }
  memcpy(buffer + pos, s->s_proto, len);
  buffer[pos + len] = '\0';
  s->s_proto = buffer + pos;

  /* track position in the buffer */
  pos += len + 1;

  /* ensure pointer alignment */
  unsigned int misalign = sizeof(char *) - pos%sizeof(char *);
  if(misalign < sizeof(char *))
    pos += misalign;

  /* leave space for pointer list */
  char **p = s->s_aliases, **q;
  char **s_aliases = (char **)(buffer + pos);
  while(*(p++) != 0)
    pos += sizeof(char *);

  /* copy addresses and fill new pointer list */
  for (p = s->s_aliases, q = s_aliases; *p != 0; p++, q++){
    len = strlen(*p);
    if (size <= pos + len)
    {
      return NULL;
    }
    memcpy(buffer + pos, *p, len); /* copy content */
    buffer[pos + len] = '\0';
    *q = buffer + pos; /* set copied pointer to copied content */
    pos += len + 1;
  }
  *++q = 0; /* null terminate the pointer list */
  s->s_aliases = s_aliases; /* copy pointer to pointers */
  return s;
}
#endif

#if defined(HAVE_GETSERVBYNAME) && wxUSE_THREADS
static wxMutex servLock;
#endif
struct servent *wxGetservbyname_r(const char *port, const char *protocol,
				  struct servent *serv, void *buffer, int size)
{
  struct servent *se = NULL;
#if defined(HAVE_FUNC_GETSERVBYNAME_R_6)
  if (getservbyname_r(port, protocol, serv, (char*)buffer, size, &se))
    se = NULL;
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_5)
  se = getservbyname_r(port, protocol, serv, (char*)buffer, size);
#elif defined(HAVE_FUNC_GETSERVBYNAME_R_4)
  if (getservbyname_r(port, protocol, serv, (struct servent_data*) buffer))
    se = NULL;
  else
    se = serv;
#elif defined(HAVE_GETSERVBYNAME)
#if wxUSE_THREADS
  wxMutexLocker locker(servLock);
#endif
  se = getservbyname(port, protocol);
  if (se)
    se = deepCopyServent(serv, se, (char*)buffer, size);
#endif
  return se;
}

/* static */
wxSocketImpl *wxSocketImpl::Create(wxSocketBase& wxsocket)
{
    return new wxSocketImplUnix(wxsocket);
}


/*
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void wxSocketImplUnix::Shutdown()
{
    /* Don't allow events to fire after socket has been closed */
    DisableEvents();

    wxSocketImpl::Shutdown();
}

wxSocketError wxSocketImplUnix::GetLastError() const
{
    switch ( errno )
    {
        case 0:
            return wxSOCKET_NOERROR;

        case ENOTSOCK:
            return wxSOCKET_INVSOCK;

        // unfortunately EAGAIN only has the "would block" meaning for read(),
        // not for connect() for which it means something rather different but
        // we can't distinguish between these two situations currently...
        case EAGAIN:
        case EINPROGRESS:
            return wxSOCKET_WOULDBLOCK;

        default:
            return wxSOCKET_IOERR;
    }
}

void wxSocketImplUnix::DoEnableEvents(bool flag)
{
    wxSocketManager * const manager = wxSocketManager::Get();
    if ( flag )
    {
        manager->Install_Callback(this, wxSOCKET_INPUT);
        manager->Install_Callback(this, wxSOCKET_OUTPUT);
    }
    else // off
    {
        manager->Uninstall_Callback(this, wxSOCKET_INPUT);
        manager->Uninstall_Callback(this, wxSOCKET_OUTPUT);
    }
}

/* Generic IO */

/* Like recv(), send(), ... */
int wxSocketImplUnix::Read(void *buffer, int size)
{
  int ret;

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return -1;
  }

  /* Disable events during query of socket status */
  DisableEvent(wxSOCKET_INPUT);

  /* Read the data */
  if (m_stream)
      ret = Recv_Stream(buffer, size);
  else
      ret = Recv_Dgram(buffer, size);

  /*
   * If recv returned zero for a TCP socket (if m_stream == NULL, it's an UDP
   * socket and empty datagrams are possible), then the connection has been
   * gracefully closed.
   *
   * Otherwise, recv has returned an error (-1), in which case we have lost
   * the socket only if errno does _not_ indicate that there may be more data
   * to read.
   */
  if ((ret == 0) && m_stream)
  {
      m_establishing = false;
      OnStateChange(wxSOCKET_LOST);
      return 0;
  }
  else if (ret == -1)
  {
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
          m_error = wxSOCKET_WOULDBLOCK;
      else
          m_error = wxSOCKET_IOERR;
  }

  /* Enable events again now that we are done processing */
  EnableEvent(wxSOCKET_INPUT);

  return ret;
}

int wxSocketImplUnix::Write(const void *buffer, int size)
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

  if (ret == -1)
  {
    if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
    {
      m_error = wxSOCKET_WOULDBLOCK;
    }
    else
    {
      m_error = wxSOCKET_IOERR;
    }

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    EnableEvent(wxSOCKET_OUTPUT);

    return -1;
  }

  return ret;
}

/* Flags */

void wxSocketImplUnix::EnableEvent(wxSocketNotify event)
{
    wxSocketManager::Get()->Install_Callback(this, event);
}

void wxSocketImplUnix::DisableEvent(wxSocketNotify event)
{
    wxSocketManager::Get()->Uninstall_Callback(this, event);
}

int wxSocketImplUnix::Recv_Stream(void *buffer, int size)
{
  int ret;
  do
  {
    ret = recv(m_fd, buffer, size, GSOCKET_MSG_NOSIGNAL);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  return ret;
}

int wxSocketImplUnix::Recv_Dgram(void *buffer, int size)
{
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  int ret;
  wxSocketError err;

  fromlen = sizeof(from);

  do
  {
    ret = recvfrom(m_fd, buffer, size, 0, (sockaddr*)&from, (WX_SOCKLEN_T *) &fromlen);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  if (ret == -1)
    return -1;

  /* Translate a system address into a wxSocketImplUnix address */
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

int wxSocketImplUnix::Send_Stream(const void *buffer, int size)
{
  int ret;

  MASK_SIGNAL();

  do
  {
    ret = send(m_fd, buffer, size, GSOCKET_MSG_NOSIGNAL);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  UNMASK_SIGNAL();

  return ret;
}

int wxSocketImplUnix::Send_Dgram(const void *buffer, int size)
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

  MASK_SIGNAL();

  do
  {
    ret = sendto(m_fd, buffer, size, 0, addr, len);
  }
  while (ret == -1 && errno == EINTR); /* Loop until not interrupted */

  UNMASK_SIGNAL();

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);

  return ret;
}

void wxSocketImplUnix::OnStateChange(wxSocketNotify event)
{
    DisableEvent(event);
    NotifyOnStateChange(event);

    if ( event == wxSOCKET_LOST )
        Shutdown();
}

void wxSocketImplUnix::OnReadWaiting()
{
  char c;

  if (m_fd == INVALID_SOCKET)
  {
    return;
  }

  int num =  recv(m_fd, &c, 1, MSG_PEEK | GSOCKET_MSG_NOSIGNAL);

  if (num > 0)
  {
    OnStateChange(wxSOCKET_INPUT);
  }
  else
  {
    if (m_server && m_stream)
    {
      OnStateChange(wxSOCKET_CONNECTION);
    }
    else if (num == 0)
    {
      if (m_stream)
      {
        /* graceful shutdown */
        OnStateChange(wxSOCKET_LOST);
      }
      else
      {
        /* Empty datagram received */
        OnStateChange(wxSOCKET_INPUT);
      }
    }
    else
    {
      /* Do not throw a lost event in cases where the socket isn't really lost */
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN) || (errno == EINTR))
      {
        OnStateChange(wxSOCKET_INPUT);
      }
      else
      {
        OnStateChange(wxSOCKET_LOST);
      }
    }
  }
}

void wxSocketImplUnix::OnWriteWaiting()
{
  if (m_establishing && !m_server)
  {
    int error;
    SOCKOPTLEN_T len = sizeof(error);

    m_establishing = false;

    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

    if (error)
    {
      OnStateChange(wxSOCKET_LOST);
    }
    else
    {
      OnStateChange(wxSOCKET_CONNECTION);
      /* We have to fire this event by hand because CONNECTION (for clients)
       * and OUTPUT are internally the same and we just disabled CONNECTION
       * events with the above macro.
       */
      OnStateChange(wxSOCKET_OUTPUT);
    }
  }
  else
  {
    OnStateChange(wxSOCKET_OUTPUT);
  }
}

void wxSocketImplUnix::OnExceptionWaiting()
{
    wxFAIL_MSG( "not supposed to be called" );
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


GAddress *GAddress_new(void)
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family  = wxSOCKET_NOFAMILY;
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
#endif // wxUSE_IPV6
    default:
    {
      address->m_error = wxSOCKET_INVOP;
      return wxSOCKET_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len  = len;
  address->m_addr = (struct sockaddr *)malloc(len);

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
  *addr = (struct sockaddr *)malloc(address->m_len);
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
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  /* If it is a numeric host name, convert it now */
#if defined(HAVE_INET_ATON)
  if (inet_aton(hostname, addr) == 0)
  {
#elif defined(HAVE_INET_ADDR)
  if ( (addr->s_addr = inet_addr(hostname)) == (unsigned)-1 )
  {
#else
  /* Use gethostbyname by default */
#ifndef __WXMAC__
  int val = 1;  /* VA doesn't like constants in conditional expressions */
  if (val)
#endif
  {
#endif
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    struct hostent h;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
    struct hostent_data buffer;
#else
    char buffer[1024];
#endif
    int err;
    he = wxGethostbyname_r(hostname, &h, (void*)&buffer, sizeof(buffer), &err);
    if (he == NULL)
    {
      /* Reset to invalid address */
      addr->s_addr = INADDR_NONE;
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

  assert(address != NULL);

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

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  if (!port)
  {
    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
  }

#if defined(HAVE_FUNC_GETSERVBYNAME_R_4)
    struct servent_data buffer;
#else
  char buffer[1024];
#endif
  struct servent serv;
  se = wxGetservbyname_r(port, protocol, &serv,
			 (void*)&buffer, sizeof(buffer));
  if (!se)
  {
    /* the cast to int suppresses compiler warnings about subscript having the
       type char */
    if (isdigit((int)port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in *)address->m_addr;
      addr->sin_port = htons(port_int);
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

  assert(address != NULL);
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

  assert(address != NULL);
  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  struct hostent temphost;
#if defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
  struct hostent_data buffer;
#else
  char buffer[1024];
#endif
  int err;
  he = wxGethostbyaddr_r(addr_buf, sizeof(addr->sin_addr), AF_INET, &temphost,
			 (void*)&buffer, sizeof(buffer), &err);
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

#if wxUSE_IPV6
/*
 * -------------------------------------------------------------------------
 * Internet IPv6 address family
 * -------------------------------------------------------------------------
 */

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
  assert(address != NULL);
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
  assert(address != NULL);

  CHECK_ADDRESS(address, INET6);

  struct in6_addr addr;
  memset( & addr, 0, sizeof( addr ) );
  return GAddress_INET6_SetHostAddress(address, addr);
}
wxSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr)
{
  assert(address != NULL);

  CHECK_ADDRESS(address, INET6);

  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = hostaddr;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in6 *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET6);

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

  assert(address != NULL);
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

  assert(address != NULL);
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
  assert(address != NULL);
  assert(hostaddr != NULL);
  CHECK_ADDRESS_RETVAL(address, INET6, wxSOCKET_INVADDR);
  *hostaddr = ( (struct sockaddr_in6 *)address->m_addr )->sin6_addr;
  return wxSOCKET_NOERROR;
}

unsigned short GAddress_INET6_GetPort(GAddress *address)
{
  assert(address != NULL);
  CHECK_ADDRESS_RETVAL(address, INET6, 0);

  return ntohs( ((struct sockaddr_in6 *)address->m_addr)->sin6_port );
}

#endif // wxUSE_IPV6

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

#ifndef __VISAGECPP__
wxSocketError _GAddress_Init_UNIX(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_un);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = wxSOCKET_MEMERR;
    return wxSOCKET_MEMERR;
  }

  address->m_family = wxSOCKET_UNIX;
  address->m_realfamily = PF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_family = AF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_path[0] = 0;

  return wxSOCKET_NOERROR;
}

#define UNIX_SOCK_PATHLEN (sizeof(addr->sun_path)/sizeof(addr->sun_path[0]))

wxSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
  struct sockaddr_un *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, UNIX);

  addr = ((struct sockaddr_un *)address->m_addr);
  strncpy(addr->sun_path, path, UNIX_SOCK_PATHLEN);
  addr->sun_path[UNIX_SOCK_PATHLEN - 1] = '\0';

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
  struct sockaddr_un *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, UNIX);

  addr = (struct sockaddr_un *)address->m_addr;

  strncpy(path, addr->sun_path, sbuf);

  return wxSOCKET_NOERROR;
}
#endif  /* !defined(__VISAGECPP__) */

#endif  /* wxUSE_SOCKETS */
