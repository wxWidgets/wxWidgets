/////////////////////////////////////////////////////////////////////////////
// Name:        sckaddr.cpp
// Purpose:     Network address manager
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "sckaddr.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef __MWERKS__
#include <memory.h>
#endif

#include "wx/defs.h"
#include "wx/object.h"

#if defined(__WINDOWS__)
#include <winsock.h>
#endif // __WINDOWS__

#if defined(__UNIX__)

#ifdef VMS
#include <socket.h>
#include <in.h>
#else
#if defined(__FreeBSD__) || defined (__NetBSD__)
#include <sys/types.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <unistd.h>
#include <netdb.h>

#endif // __UNIX__

#include "wx/sckaddr.h"

#define CHECK_ADDRTYPE(var, type)

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxSockAddress, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxIPV4address, wxSockAddress)
#ifdef ENABLE_IPV6
IMPLEMENT_DYNAMIC_CLASS(wxIPV6address, wxSockAddress)
#endif
#ifdef __UNIX__
IMPLEMENT_DYNAMIC_CLASS(wxUNIXaddress, wxSockAddress)
#endif
#endif

wxIPV4address::wxIPV4address()
{
  m_addr = new sockaddr_in;
  Clear();
}

wxIPV4address::~wxIPV4address()
{
}

int wxIPV4address::SockAddrLen()
{
  return sizeof(*m_addr);
}

int wxIPV4address::GetFamily()
{
  return AF_INET;
}

void wxIPV4address::Clear()
{
  memset(m_addr, 0, sizeof(*m_addr));
  m_addr->sin_family = AF_INET;
  m_addr->sin_addr.s_addr = INADDR_ANY;
}

/*
const wxSockAddress& wxIPV4address::operator =(const wxSockAddress& addr)
{
  wxIPV4address *ip_addr = (wxIPV4address *)&addr;
  CHECK_ADDRTYPE(addr, wxIPV4address);
  m_addr = ip_addr->m_addr;
  return *this;
}
*/

bool wxIPV4address::Hostname(const wxString& name)
{
  struct hostent *hostent;
  struct in_addr *addr;
  
  if (name.IsNull())
    return FALSE;
  
  if (!name.IsNumber()) {
    if ((hostent = gethostbyname(name.GetData())) == 0) {
      return FALSE;
    }
  } else {
    long len_addr = inet_addr(name.GetData());
    if (len_addr == -1)
      return FALSE;
    m_addr->sin_addr.s_addr = len_addr;
    return TRUE;
  }

  addr = (struct in_addr *) *(hostent->h_addr_list);

  m_addr->sin_addr.s_addr = addr[0].s_addr;
  return TRUE;
}

bool wxIPV4address::Hostname(unsigned long addr)
{
  m_addr->sin_addr.s_addr = htonl(addr);
  return TRUE; 
}

bool wxIPV4address::Service(const wxString& name)
{
  struct servent *servent;
  
  if (name.IsNull())
    return FALSE;
  
  if (!name.IsNumber()) {
    if ((servent = getservbyname(name, "tcp")) == 0)
      return FALSE;
  } else {
    if ((servent = getservbyport(atoi(name), "tcp")) == 0) {
      m_addr->sin_port = htons(atoi(name));
      return TRUE;
    }
  }
  
  m_addr->sin_port = servent->s_port;
  return TRUE;
}

bool wxIPV4address::Service(unsigned short port)
{
  m_addr->sin_port = htons(port);
  return TRUE;
}

bool wxIPV4address::LocalHost()
{
  static char buf[256];
  
  if (gethostname(buf, sizeof(buf)) < 0)
    return Hostname("localhost");
  else
    return Hostname(buf);
}

wxString wxIPV4address::Hostname()
{
  struct hostent *h_ent;

  h_ent = gethostbyaddr((char *)&(m_addr->sin_addr), sizeof(m_addr->sin_addr),
			GetFamily());
  return wxString(h_ent->h_name);
}

unsigned short wxIPV4address::Service()
{
  return ntohs(m_addr->sin_port); 
}

void wxIPV4address::Build(struct sockaddr *&addr, size_t& len)
{
  addr = (struct sockaddr *)m_addr;
  len = sizeof(*m_addr);
}

void wxIPV4address::Disassemble(struct sockaddr *addr, size_t len)
{
  if (len != sizeof(*m_addr))
    return;
  *m_addr = *(struct sockaddr_in *)addr;
}

#ifdef IPV6_ENABLE

wxIPV6address::wxIPV6address()
{
  m_addr = new sockaddr_in6;
  Clear();
}

wxIPV6address::~wxIPV6address()
{
}

int wxIPV6address::SockAddrLen()
{
  return sizeof(*m_addr);
}

int wxIPV6address::GetFamily()
{
  return AF_INET6;
}

void wxIPV6address::Clear()
{ 
  memset(m_addr, 0, sizeof(*m_addr));
  m_addr->sin6_family = AF_INET6;
  m_addr->sin6_addr.s_addr = INADDR_ANY;
}

/*
const wxSockAddress& wxIPV6address::operator =(const wxSockAddress& addr)
{
  wxIPV6address *ip_addr = (wxIPV6address *)&addr;

  CHECK_ADDRTYPE(addr, wxIPV6address);
  m_addr = ip_addr->m_addr;
  return *this;
}
*/

bool wxIPV6address::Hostname(const wxString& name)
{
  struct hostent *hostent;
  struct in_addr *addr;
  
  if (name.IsNull())
    return FALSE;
  
  if (!name.IsNumber()) {
    hostent = gethostbyname2((char*) name, AF_INET6);
    if (!hostent)
      return FALSE;
  } else {
    // Don't how to do
    return FALSE;
  }

  addr = (struct in6_addr *) *(hostent->h_addr_list);

  m_addr->sin6_addr.s6_addr = addr[0].s6_addr;
  return TRUE;
}

bool wxIPV6address::Hostname(unsigned char addr[16])
{
  m_addr->sin6_addr.s6_addr = addr;
  return TRUE;
}

bool wxIPV6address::Service(const char *name)
{
  struct servent *servent;
  
  if (!name || !strlen(name))
    return FALSE;
  
  if (!isdigit(*name)) {
    if ((servent = getservbyname((char*) name, "tcp")) == 0)
      return FALSE;
  } else {
    if ((servent = getservbyport(atoi(name), "tcp")) == 0) {
      m_addr->sin_port = htons(atoi(name));
      return TRUE;
    }
  }
  
  m_addr->sin_port = servent->s_port;
  return TRUE;
}

bool wxIPV6address::Service(unsigned short port)
{
  m_addr->sin_port = htons(port);
  return TRUE;
}

bool wxIPV6address::LocalHost()
{
  static char buf[256];
  
  if (gethostname(buf, sizeof(buf)) < 0)
    return Hostname("localhost");
  else
    return Hostname(buf);
}

const wxString& wxIPV6address::Hostname()
{
  struct hostent *h_ent;

  h_ent = gethostbyaddr((char *)&(m_addr->sin_addr), sizeof(m_addr->sin_addr),
			GetFamily());
  return wxString(h_ent->h_name);
}

unsigned short wxIPV6address::Service()
{
  return ntohs(m_addr->sin_port); 
}

void wxIPV6address::Build(struct sockaddr& *addr, size_t& len)
{
  len = sizeof(*m_addr);
  addr = m_addr;
}

void wxIPV6address::Disassemble(struct sockaddr& *addr, size_t len)
{
  if (len != sizeof(*m_addr))
    return;
  *m_addr = *(struct sockaddr_in6 *)addr;
}

#endif

#ifdef __UNIX__
#include <sys/un.h>

wxUNIXaddress::wxUNIXaddress()
{
  m_addr = new sockaddr_un;
  Clear();
}

wxUNIXaddress::~wxUNIXaddress()
{
}

int wxUNIXaddress::SockAddrLen()
{
  return sizeof(*m_addr);
}

int wxUNIXaddress::GetFamily()
{
  return AF_UNIX;
}

void wxUNIXaddress::Clear()
{
  memset(m_addr, 0, sizeof(m_addr));
  m_addr->sun_family = AF_UNIX;
}

/*
const wxSockAddress& wxUNIXaddress::operator =(const wxSockAddress& addr)
{
  wxUNIXaddress *unx_addr = (wxUNIXaddress *)&addr;
  CHECK_ADDRTYPE(addr, wxUNIXaddress);
  m_addr = unx_addr->m_addr;
  return *this;
}
*/

void wxUNIXaddress::Filename(const wxString& fname)
{
  sprintf(m_addr->sun_path, "%s", WXSTRINGCAST fname);
}

wxString wxUNIXaddress::Filename()
{
  return wxString(m_addr->sun_path);
}

void wxUNIXaddress::Build(struct sockaddr*& addr, size_t& len)
{
  addr = (struct sockaddr *)m_addr;
  len = sizeof(*m_addr);
}

void wxUNIXaddress::Disassemble(struct sockaddr *addr, size_t len)
{
  if (len != sizeof(*m_addr))
    return;
  *m_addr = *(struct sockaddr_un *)addr;
}
#endif
