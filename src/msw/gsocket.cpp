/* -------------------------------------------------------------------------
 * Project:     wxSocketImpl (Generic Socket)
 * Name:        src/msw/gsocket.cpp
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Author:      Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose:     wxSocketImpl main MSW file
 * Licence:     The wxWindows licence
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef _MSC_VER
   /* RPCNOTIFICATION_ROUTINE in rasasync.h (included from winsock.h),
    * warning: conditional expression is constant.
    */
#  pragma warning(disable:4115)
   /* FD_SET,
    * warning: named type definition in parentheses.
    */
#  pragma warning(disable:4127)
   /* GAddress_UNIX_GetPath,
    * warning: unreferenced formal parameter.
    */
#  pragma warning(disable:4100)

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

#endif /* _MSC_VER */

#if defined(__CYGWIN__)
    //CYGWIN gives annoying warning about runtime stuff if we don't do this
#   define USE_SYS_TYPES_FD_SET
#   include <sys/types.h>
#endif

#include <winsock.h>

#include "wx/platform.h"

#if wxUSE_SOCKETS

#include "wx/private/socket.h"
#include "wx/link.h"

wxFORCE_LINK_MODULE(gsockmsw)

#ifdef __WXWINCE__
#ifndef isdigit
#define isdigit(x) (x > 47 && x < 58)
#endif
#include "wx/msw/wince/net.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include "wx/private/socket.h"

void wxSocketImplMSW::DoClose()
{
    wxSocketManager::Get()->
        Uninstall_Callback(this, wxSOCKET_MAX_EVENT /* unused anyhow */);

    closesocket(m_fd);
}

/*
 *  Waits for an incoming client connection. Returns a pointer to
 *  a wxSocketImpl object, or NULL if there was an error, in which case
 *  the last error field will be updated for the calling wxSocketImpl.
 *
 *  Error codes (set in the calling wxSocketImpl)
 *    wxSOCKET_INVSOCK    - the socket is not valid or not a server.
 *    wxSOCKET_TIMEDOUT   - timeout, no incoming connections.
 *    wxSOCKET_WOULDBLOCK - the call would block and the socket is nonblocking.
 *    wxSOCKET_MEMERR     - couldn't allocate memory.
 *    wxSOCKET_IOERR      - low-level error.
 */
wxSocketImpl *wxSocketImplMSW::WaitConnection(wxSocketBase& wxsocket)
{
  wxSocketImpl *connection;
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  wxSocketError err;
  u_long arg = 1;

  /* Reenable CONNECTION events */
  m_detected &= ~wxSOCKET_CONNECTION_FLAG;

  /* If the socket has already been created, we exit immediately */
  if (m_fd == INVALID_SOCKET || !m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return NULL;
  }

  /* Create a wxSocketImpl object for the new connection */
  connection = wxSocketImplMSW::Create(wxsocket);

  if (!connection)
  {
    m_error = wxSOCKET_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (Input_Timeout() == wxSOCKET_TIMEDOUT)
  {
    delete connection;
    /* m_error set by Input_Timeout */
    return NULL;
  }

  connection->m_fd = accept(m_fd, (sockaddr*)&from, &fromlen);

  if (connection->m_fd == INVALID_SOCKET)
  {
    if (WSAGetLastError() == WSAEWOULDBLOCK)
      m_error = wxSOCKET_WOULDBLOCK;
    else
      m_error = wxSOCKET_IOERR;

    delete connection;
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = false;
  connection->m_stream   = true;

  /* Setup the peer address field */
  connection->m_peer = GAddress_new();
  if (!connection->m_peer)
  {
    delete connection;
    m_error = wxSOCKET_MEMERR;
    return NULL;
  }
  err = _GAddress_translate_from(connection->m_peer, (sockaddr*)&from, fromlen);
  if (err != wxSOCKET_NOERROR)
  {
    GAddress_destroy(connection->m_peer);
    delete connection;
    m_error = err;
    return NULL;
  }

  ioctlsocket(connection->m_fd, FIONBIO, (u_long FAR *) &arg);
  wxSocketManager::Get()->Install_Callback(connection);

  return connection;
}

wxSocketError wxSocketImplMSW::DoHandleConnect(int ret)
{
    // TODO: review this
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();

        /* If connect failed with EWOULDBLOCK and the wxSocketImpl object
         * is in blocking mode, we select() for the specified timeout
         * checking for writability to see if the connection request
         * completes.
         */
        if ((err == WSAEWOULDBLOCK) && (!m_non_blocking))
        {
            err = Connect_Timeout();

            if (err != wxSOCKET_NOERROR)
            {
                Close();
                /* m_error is set in Connect_Timeout */
            }

            return (wxSocketError) err;
        }

        /* If connect failed with EWOULDBLOCK and the wxSocketImpl object
         * is set to nonblocking, we set m_error to wxSOCKET_WOULDBLOCK
         * (and return wxSOCKET_WOULDBLOCK) but we don't close the socket;
         * this way if the connection completes, a wxSOCKET_CONNECTION
         * event will be generated, if enabled.
         */
        if ((err == WSAEWOULDBLOCK) && (m_non_blocking))
        {
            m_establishing = true;
            m_error = wxSOCKET_WOULDBLOCK;
            return wxSOCKET_WOULDBLOCK;
        }

        /* If connect failed with an error other than EWOULDBLOCK,
         * then the call to Connect() has failed.
         */
        Close();
        m_error = wxSOCKET_IOERR;
        return wxSOCKET_IOERR;
    }

    return wxSOCKET_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int wxSocketImplMSW::Read(char *buffer, int size)
{
  int ret;

  /* Reenable INPUT events */
  m_detected &= ~wxSOCKET_INPUT_FLAG;

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for data (with a timeout) */
  if (Input_Timeout() == wxSOCKET_TIMEDOUT)
  {
    m_error = wxSOCKET_TIMEDOUT;
    return -1;
  }

  /* Read the data */
  if (m_stream)
    ret = Recv_Stream(buffer, size);
  else
    ret = Recv_Dgram(buffer, size);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      m_error = wxSOCKET_IOERR;
    else
      m_error = wxSOCKET_WOULDBLOCK;
    return -1;
  }

  return ret;
}

int wxSocketImplMSW::Write(const char *buffer, int size)
{
  int ret;

  if (m_fd == INVALID_SOCKET || m_server)
  {
    m_error = wxSOCKET_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (Output_Timeout() == wxSOCKET_TIMEDOUT)
    return -1;

  /* Write the data */
  if (m_stream)
    ret = Send_Stream(buffer, size);
  else
    ret = Send_Dgram(buffer, size);

  if (ret == SOCKET_ERROR)
  {
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      m_error = wxSOCKET_IOERR;
    else
      m_error = wxSOCKET_WOULDBLOCK;

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * does). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    m_detected &= ~wxSOCKET_OUTPUT_FLAG;
    return -1;
  }

  return ret;
}

/* Internals (IO) */

/*
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
wxSocketError wxSocketImplMSW::Input_Timeout()
{
  fd_set readfds;

  if (!m_non_blocking)
  {
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);
    if (select(0, &readfds, NULL, NULL, &m_timeout) == 0)
    {
      m_error = wxSOCKET_TIMEDOUT;
      return wxSOCKET_TIMEDOUT;
    }
  }
  return wxSOCKET_NOERROR;
}

/*
 *  For blocking sockets, wait until data can be sent without
 *  blocking or until timeout ellapses.
 */
wxSocketError wxSocketImplMSW::Output_Timeout()
{
  fd_set writefds;

  if (!m_non_blocking)
  {
    FD_ZERO(&writefds);
    FD_SET(m_fd, &writefds);
    if (select(0, NULL, &writefds, NULL, &m_timeout) == 0)
    {
      m_error = wxSOCKET_TIMEDOUT;
      return wxSOCKET_TIMEDOUT;
    }
  }
  return wxSOCKET_NOERROR;
}

/*
 *  For blocking sockets, wait until the connection is
 *  established or fails, or until timeout ellapses.
 */
wxSocketError wxSocketImplMSW::Connect_Timeout()
{
  fd_set writefds;
  fd_set exceptfds;

  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_SET(m_fd, &writefds);
  FD_SET(m_fd, &exceptfds);
  if (select(0, NULL, &writefds, &exceptfds, &m_timeout) == 0)
  {
    m_error = wxSOCKET_TIMEDOUT;
    return wxSOCKET_TIMEDOUT;
  }
  if (!FD_ISSET(m_fd, &writefds))
  {
    m_error = wxSOCKET_IOERR;
    return wxSOCKET_IOERR;
  }

  return wxSOCKET_NOERROR;
}

int wxSocketImplMSW::Recv_Stream(char *buffer, int size)
{
  return recv(m_fd, buffer, size, 0);
}

int wxSocketImplMSW::Recv_Dgram(char *buffer, int size)
{
  wxSockAddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  int ret;
  wxSocketError err;

  ret = recvfrom(m_fd, buffer, size, 0, (sockaddr*)&from, &fromlen);

  if (ret == SOCKET_ERROR)
    return SOCKET_ERROR;

  /* Translate a system address into a wxSocketImpl address */
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

int wxSocketImplMSW::Send_Stream(const char *buffer, int size)
{
  return send(m_fd, buffer, size, 0);
}

int wxSocketImplMSW::Send_Dgram(const char *buffer, int size)
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

  ret = sendto(m_fd, buffer, size, 0, addr, len);

  /* Frees memory allocated by _GAddress_translate_to */
  free(addr);

  return ret;
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


GAddress *GAddress_new()
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family = wxSOCKET_NOFAMILY;
  address->m_addr   = NULL;
  address->m_len    = 0;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

  if ((addr2 = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr)
  {
    addr2->m_addr = (struct sockaddr *) malloc(addr2->m_len);
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
  if (address->m_addr)
    free(address->m_addr);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
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
#endif
    default:
    {
      address->m_error = wxSOCKET_INVOP;
      return wxSOCKET_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len = len;
  address->m_addr = (struct sockaddr *) malloc(len);

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
  *addr = (struct sockaddr *) malloc(address->m_len);
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
  address->m_realfamily = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  CHECK_ADDRESS(address, INET);

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  addr->s_addr = inet_addr(hostname);

  /* If it is a numeric host name, convert it now */
  if (addr->s_addr == INADDR_NONE)
  {
    struct in_addr *array_addr;

    /* It is a real name, we solve it */
    if ((he = gethostbyname(hostname)) == NULL)
    {
      /* addr->s_addr = INADDR_NONE just done by inet_addr() above */
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

  CHECK_ADDRESS(address, INET);

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
      addr = (struct sockaddr_in *)address->m_addr;
      addr->sin_port = htons((u_short) port_int);
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

  CHECK_ADDRESS(address, INET);

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin_addr), AF_INET);
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

  CHECK_ADDRESS_RETVAL(address, INET, 0);

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohl(addr->sin_addr.s_addr);
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

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
#include "ws2tcpip.h"

#ifdef __VISUALC__
    #pragma comment(lib,"ws2_32")
#endif // __VISUALC__

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
  CHECK_ADDRESS(address, INET6);

  struct in6_addr addr;
  memset( & addr, 0, sizeof( addr ) );
  return GAddress_INET6_SetHostAddress(address, addr);
}
wxSocketError GAddress_INET6_SetHostAddress(GAddress *address,
                                          struct in6_addr hostaddr)
{
  CHECK_ADDRESS(address, INET6);

  ((struct sockaddr_in6 *)address->m_addr)->sin6_addr = hostaddr;

  return wxSOCKET_NOERROR;
}

wxSocketError GAddress_INET6_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in6 *addr;

  CHECK_ADDRESS(address, INET6);

  if (!port)
  {
    address->m_error = wxSOCKET_INVPORT;
    return wxSOCKET_INVPORT;
  }

  se = getservbyname(port, protocol);
  if (!se)
  {
    if (isdigit((unsigned char) port[0]))
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
  CHECK_ADDRESS_RETVAL(address, INET6, wxSOCKET_INVADDR);
  *hostaddr = ( (struct sockaddr_in6 *)address->m_addr )->sin6_addr;
  return wxSOCKET_NOERROR;
}

unsigned short GAddress_INET6_GetPort(GAddress *address)
{
  CHECK_ADDRESS_RETVAL(address, INET6, 0);

  return ntohs( ((struct sockaddr_in6 *)address->m_addr)->sin6_port );
}

#endif // wxUSE_IPV6

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

wxSocketError _GAddress_Init_UNIX(GAddress *address)
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

wxSocketError GAddress_UNIX_SetPath(GAddress *address, const char *WXUNUSED(path))
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

wxSocketError GAddress_UNIX_GetPath(GAddress *address, char *WXUNUSED(path), size_t WXUNUSED(sbuf))
{
  address->m_error = wxSOCKET_INVADDR;
  return wxSOCKET_INVADDR;
}

#endif  // wxUSE_SOCKETS
