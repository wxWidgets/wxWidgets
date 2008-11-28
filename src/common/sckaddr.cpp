/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sckaddr.cpp
// Purpose:     Network address manager
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/log.h"
    #include "wx/intl.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>

    #if !defined(__MWERKS__)
        #include <memory.h>
    #endif
#endif // !WX_PRECOMP

#include "wx/private/gsocket.h"
#include "wx/socket.h"
#include "wx/sckaddr.h"

IMPLEMENT_ABSTRACT_CLASS(wxSockAddress, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxIPaddress, wxSockAddress)
IMPLEMENT_DYNAMIC_CLASS(wxIPV4address, wxIPaddress)
#if wxUSE_IPV6
IMPLEMENT_DYNAMIC_CLASS(wxIPV6address, wxIPaddress)
#endif
#if defined(__UNIX__) && !defined(__WINDOWS__) && !defined(__WINE__) 
IMPLEMENT_DYNAMIC_CLASS(wxUNIXaddress, wxSockAddress)
#endif

// ---------------------------------------------------------------------------
// wxSockAddress
// ---------------------------------------------------------------------------

void wxSockAddress::Init()
{
    if ( !wxSocketBase::IsInitialized() )
    {
        // we must do it before using GAddress_XXX functions
        (void)wxSocketBase::Initialize();
    }
}

wxSockAddress::wxSockAddress()
{
    Init();

    m_address = GAddress_new();
}

wxSockAddress::wxSockAddress(const wxSockAddress& other)
    : wxObject()
{
    Init();

    m_address = GAddress_copy(other.m_address);
}

wxSockAddress::~wxSockAddress()
{
  GAddress_destroy(m_address);
}

void wxSockAddress::SetAddress(GAddress *address)
{
    if ( address != m_address )
    {
        GAddress_destroy(m_address);
        m_address = GAddress_copy(address);
    }
}

wxSockAddress& wxSockAddress::operator=(const wxSockAddress& addr)
{
  SetAddress(addr.GetAddress());
  return *this;
}

void wxSockAddress::Clear()
{
  GAddress_destroy(m_address);
  m_address = GAddress_new();
}

// ---------------------------------------------------------------------------
// wxIPaddress
// ---------------------------------------------------------------------------

wxIPaddress::wxIPaddress()
            : wxSockAddress()
{
}

wxIPaddress::wxIPaddress(const wxIPaddress& other)
            : wxSockAddress(other)
{
}

wxIPaddress::~wxIPaddress()
{
}

// ---------------------------------------------------------------------------
// wxIPV4address
// ---------------------------------------------------------------------------

wxIPV4address::wxIPV4address()
             : wxIPaddress()
{
}

wxIPV4address::wxIPV4address(const wxIPV4address& other)
             : wxIPaddress(other)
{
}

wxIPV4address::~wxIPV4address()
{
}

bool wxIPV4address::Hostname(const wxString& name)
{
  // Some people are sometimes fool.
  if (name.empty())
  {
    wxLogWarning( _("Trying to solve a NULL hostname: giving up") );
    return false;
  }
  m_origHostname = name;
  return (GAddress_INET_SetHostName(m_address, name.mb_str()) == wxSOCKET_NOERROR);
}

bool wxIPV4address::Hostname(unsigned long addr)
{
  bool rv = (GAddress_INET_SetHostAddress(m_address, addr) == wxSOCKET_NOERROR);
  if (rv)
      m_origHostname = Hostname();
  else
      m_origHostname = wxEmptyString;
  return rv;
}

bool wxIPV4address::Service(const wxString& name)
{
  return (GAddress_INET_SetPortName(m_address, name.mb_str(), "tcp") == wxSOCKET_NOERROR);
}

bool wxIPV4address::Service(unsigned short port)
{
  return (GAddress_INET_SetPort(m_address, port) == wxSOCKET_NOERROR);
}

bool wxIPV4address::LocalHost()
{
  return (GAddress_INET_SetHostName(m_address, "localhost") == wxSOCKET_NOERROR);
}

bool wxIPV4address::IsLocalHost() const
{
  return (Hostname() == wxT("localhost") || IPAddress() == wxT("127.0.0.1"));
}

bool wxIPV4address::BroadcastAddress()
{
  return (GAddress_INET_SetBroadcastAddress(m_address) == wxSOCKET_NOERROR);
}

bool wxIPV4address::AnyAddress()
{
  return (GAddress_INET_SetAnyAddress(m_address) == wxSOCKET_NOERROR);
}

wxString wxIPV4address::Hostname() const
{
   char hostname[1024];

   hostname[0] = 0;
   GAddress_INET_GetHostName(m_address, hostname, 1024);
   return wxString::FromAscii(hostname);
}

unsigned short wxIPV4address::Service() const
{
  return GAddress_INET_GetPort(m_address);
}

wxSockAddress *wxIPV4address::Clone() const
{
    wxIPV4address *addr = new wxIPV4address(*this);
    addr->m_origHostname = m_origHostname;
    return addr;
}

wxString wxIPV4address::IPAddress() const
{
    unsigned long raw =  GAddress_INET_GetHostAddress(m_address);
    return wxString::Format(_T("%lu.%lu.%lu.%lu"),
                (raw>>24) & 0xff,
                (raw>>16) & 0xff,
                (raw>>8) & 0xff,
                raw & 0xff
        );
}

bool wxIPV4address::operator==(const wxIPV4address& addr) const
{
    return Hostname().Cmp(addr.Hostname()) == 0 &&
           Service() == addr.Service();
}

#if wxUSE_IPV6
// ---------------------------------------------------------------------------
// wxIPV6address
// ---------------------------------------------------------------------------

wxIPV6address::wxIPV6address()
  : wxIPaddress()
{
}

wxIPV6address::wxIPV6address(const wxIPV6address& other)
             : wxIPaddress(other), m_origHostname(other.m_origHostname)
{
}

wxIPV6address::~wxIPV6address()
{
}

bool wxIPV6address::Hostname(const wxString& name)
{
  if (name.empty())
  {
    wxLogWarning( _("Trying to solve a NULL hostname: giving up") );
    return false;
  }
  m_origHostname = name;
  return (GAddress_INET6_SetHostName(m_address, name.mb_str()) == wxSOCKET_NOERROR);
}

bool wxIPV6address::Hostname(unsigned char addr[16])
{
  wxString name;
  unsigned short wk[8];
  for ( int i = 0; i < 8; ++i )
  {
    wk[i] = addr[2*i];
    wk[i] <<= 8;
    wk[i] |= addr[2*i+1];
  }
  name.Printf("%x:%x:%x:%x:%x:%x:%x:%x",
              wk[0], wk[1], wk[2], wk[3], wk[4], wk[5], wk[6], wk[7]);
  return Hostname(name);
}

bool wxIPV6address::Service(const wxString& name)
{
  return (GAddress_INET6_SetPortName(m_address, name.mb_str(), "tcp") == wxSOCKET_NOERROR);
}

bool wxIPV6address::Service(unsigned short port)
{
  return (GAddress_INET6_SetPort(m_address, port) == wxSOCKET_NOERROR);
}

bool wxIPV6address::LocalHost()
{
  return (GAddress_INET6_SetHostName(m_address, "localhost") == wxSOCKET_NOERROR);
}

bool wxIPV6address::IsLocalHost() const
{
  if ( Hostname() == "localhost" )
      return true;

  wxString addr = IPAddress();
  return addr == wxT("::1") ||
            addr == wxT("0:0:0:0:0:0:0:1") ||
                addr == wxT("::ffff:127.0.0.1");
}

bool wxIPV6address::BroadcastAddress()
{
    wxFAIL_MSG( "not implemented" );

    return false;
}

bool wxIPV6address::AnyAddress()
{
  return (GAddress_INET6_SetAnyAddress(m_address) == wxSOCKET_NOERROR);
}

wxString wxIPV6address::IPAddress() const
{
    unsigned char addr[16];
    GAddress_INET6_GetHostAddress(m_address,(in6_addr*)addr);

    wxUint16 words[8];
    int i,
        prefix_zero_count = 0;
    for ( i = 0; i < 8; ++i )
    {
        words[i] = addr[i*2];
        words[i] <<= 8;
        words[i] |= addr[i*2+1];
        if ( i == prefix_zero_count && words[i] == 0 )
            ++prefix_zero_count;
    }

    wxString result;
    if ( prefix_zero_count == 8 )
    {
        result = wxT( "::" );
    }
    else if ( prefix_zero_count == 6 && words[5] == 0xFFFF )
    {
        // IPv4 mapped
        result.Printf("::ffff:%d.%d.%d.%d",
                      addr[12], addr[13], addr[14], addr[15]);
    }
    else // general case
    {
        result = ":";
        for ( i = prefix_zero_count; i < 8; ++i )
        {
            result += wxString::Format(":%x", words[i]);
        }
    }

    return result;
}

wxString wxIPV6address::Hostname() const
{
   char hostname[1024];
   hostname[0] = 0;

   if ( GAddress_INET6_GetHostName(m_address,
                                   hostname,
                                   WXSIZEOF(hostname)) != wxSOCKET_NOERROR )
       return wxEmptyString;

   return wxString::FromAscii(hostname);
}

unsigned short wxIPV6address::Service() const
{
  return GAddress_INET6_GetPort(m_address);
}

#endif // wxUSE_IPV6

#if defined(__UNIX__) && !defined(__WINDOWS__) && !defined(__WINE__) 

// ---------------------------------------------------------------------------
// wxUNIXaddress
// ---------------------------------------------------------------------------

wxUNIXaddress::wxUNIXaddress()
             : wxSockAddress()
{
}

wxUNIXaddress::wxUNIXaddress(const wxUNIXaddress& other)
             : wxSockAddress(other)
{
}

wxUNIXaddress::~wxUNIXaddress()
{
}

void wxUNIXaddress::Filename(const wxString& fname)
{
  GAddress_UNIX_SetPath(m_address, fname.fn_str());
}

wxString wxUNIXaddress::Filename()
{
  char path[1024];

  path[0] = 0;
  GAddress_UNIX_GetPath(m_address, path, 1024);

  return wxString::FromAscii(path);
}

#endif // __UNIX__

#endif
  // wxUSE_SOCKETS
