/////////////////////////////////////////////////////////////////////////////
// Name:        sckaddr.cpp
// Purpose:     Network address manager
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "sckaddr.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/log.h"
    #include "wx/intl.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>

    #if !defined(__MWERKS__) && !defined(__SALFORDC__)
        #include <memory.h>
    #endif
#endif // !WX_PRECOMP

#include "wx/gsocket.h"
#include "wx/socket.h"
#include "wx/sckaddr.h"

IMPLEMENT_ABSTRACT_CLASS(wxSockAddress, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxIPV4address, wxSockAddress)
#ifdef ENABLE_IPV6
IMPLEMENT_DYNAMIC_CLASS(wxIPV6address, wxSockAddress)
#endif
#if defined(__UNIX__) && !defined(__WINE__) && (!defined(__WXMAC__) || defined(__DARWIN__))
IMPLEMENT_DYNAMIC_CLASS(wxUNIXaddress, wxSockAddress)
#endif

// ---------------------------------------------------------------------------
// wxIPV4address
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
  GAddress_destroy(m_address);
  m_address = GAddress_copy(address);
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
// wxIPV4address
// ---------------------------------------------------------------------------

wxIPV4address::wxIPV4address()
{
}

wxIPV4address::wxIPV4address(const wxIPV4address& other)
             : wxSockAddress(other)
{
}

wxIPV4address::~wxIPV4address()
{
}

bool wxIPV4address::Hostname(const wxString& name)
{
  // Some people are sometimes fool.
  if (name == wxT(""))
  {
    wxLogWarning( _("Trying to solve a NULL hostname: giving up") );
    return FALSE;
  }
  m_origHostname = name;
  return (GAddress_INET_SetHostName(m_address, name.mb_str()) == GSOCK_NOERROR);
}

bool wxIPV4address::Hostname(unsigned long addr)
{
  bool rv = (GAddress_INET_SetHostAddress(m_address, addr) == GSOCK_NOERROR);
  if (rv)
      m_origHostname = Hostname();
  else
      m_origHostname = wxEmptyString;
  return rv;
}

bool wxIPV4address::Service(const wxString& name)
{
  return (GAddress_INET_SetPortName(m_address, name.mb_str(), "tcp") == GSOCK_NOERROR);
}

bool wxIPV4address::Service(unsigned short port)
{
  return (GAddress_INET_SetPort(m_address, port) == GSOCK_NOERROR);
}

bool wxIPV4address::LocalHost()
{
  return (GAddress_INET_SetHostName(m_address, "localhost") == GSOCK_NOERROR);
}

bool wxIPV4address::AnyAddress()
{
  return (GAddress_INET_SetAnyAddress(m_address) == GSOCK_NOERROR);
}

wxString wxIPV4address::Hostname()
{
   char hostname[1024];

   hostname[0] = 0;
   GAddress_INET_GetHostName(m_address, hostname, 1024);
   return wxString::FromAscii(hostname);
}

unsigned short wxIPV4address::Service()
{
  return GAddress_INET_GetPort(m_address);
}

wxSockAddress *wxIPV4address::Clone() const
{
    wxIPV4address *addr = new wxIPV4address(*this);
    addr->m_origHostname = m_origHostname;
    return addr;
}

#if 0
// ---------------------------------------------------------------------------
// wxIPV6address
// ---------------------------------------------------------------------------

wxIPV6address::wxIPV6address()
  : wxSockAddress()
{
}

wxIPV6address::wxIPV6address(const wxIPV6address& other)
             : wxSockAddress(other)
{
}

wxIPV6address::~wxIPV6address()
{
}

bool wxIPV6address::Hostname(const wxString& name)
{
  return (GAddress_INET_SetHostName(m_address, name.fn_str()) == GSOCK_NOERROR);
}

bool wxIPV6address::Hostname(unsigned char addr[16])
{
  return TRUE;
}

bool wxIPV6address::Service(const char *name)
{
  return (GAddress_INET_SetPortName(m_address, name.fn_str()) == GSOCK_NOERROR);
}

bool wxIPV6address::Service(unsigned short port)
{
  return (GAddress_INET_SetPort(m_address, port) == GSOCK_NOERROR);
}

bool wxIPV6address::LocalHost()
{
  return (GAddress_INET_SetHostName(m_address, "localhost") == GSOCK_NOERROR);
}

const wxString& wxIPV6address::Hostname()
{
  return wxString(GAddress_INET_GetHostName(m_address));
}

unsigned short wxIPV6address::Service()
{
  return GAddress_INET_GetPort(m_address);
}

#endif // 0

#if defined(__UNIX__) && !defined(__WINE__) && (!defined(__WXMAC__) || defined(__DARWIN__))

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
