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

#if wxUSE_SOCKETS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if !defined(__MWERKS__) && !defined(__SALFORDC__)
#include <memory.h>
#endif

#include <wx/defs.h>
#include <wx/object.h>
#include <wx/log.h>
#include <wx/gsocket.h>
#include <wx/sckaddr.h>

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

// ---------------------------------------------------------------------------
// wxIPV4address
// ---------------------------------------------------------------------------

wxSockAddress::wxSockAddress()
{
  m_address = GAddress_new();
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

const wxSockAddress& wxSockAddress::operator=(const wxSockAddress& addr)
{
  SetAddress(addr.GetAddress());
  return *this;
}

void wxSockAddress::CopyObject(wxObject& dest) const
{
  wxSockAddress *addr = (wxSockAddress *)&dest;

  wxObject::CopyObject(dest);
  addr->SetAddress(GetAddress());
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
  : wxSockAddress()
{
}

wxIPV4address::~wxIPV4address()
{
}

bool wxIPV4address::Hostname(const wxString& name)
{
  // Some people are sometimes fool.
  if (name == wxT("")) {
    wxLogWarning( wxT("Trying to solve a NULL hostname: giving up") );
    return FALSE;
  }

  return (GAddress_INET_SetHostName(m_address, name.fn_str()) == GSOCK_NOERROR);
}

bool wxIPV4address::Hostname(unsigned long addr)
{
  return (GAddress_INET_SetHostAddress(m_address, addr) == GSOCK_NOERROR);
}

bool wxIPV4address::Service(const wxString& name)
{
  return (GAddress_INET_SetPortName(m_address, name.fn_str(), "tcp") == GSOCK_NOERROR);
}

bool wxIPV4address::Service(unsigned short port)
{
  return (GAddress_INET_SetPort(m_address, port) == GSOCK_NOERROR);
}

bool wxIPV4address::LocalHost()
{
  return (GAddress_INET_SetHostName(m_address, "localhost") == GSOCK_NOERROR);
}

wxString wxIPV4address::Hostname()
{
   char hostname[1024];

   hostname[0] = 0;
   GAddress_INET_GetHostName(m_address, hostname, 1024);
   return wxString(hostname);
}

unsigned short wxIPV4address::Service()
{
  return GAddress_INET_GetPort(m_address); 
}

#if 0
// ---------------------------------------------------------------------------
// wxIPV6address
// ---------------------------------------------------------------------------

wxIPV6address::wxIPV6address()
  : wxSockAddress()
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

#endif

#ifdef __UNIX__
// ---------------------------------------------------------------------------
// wxUNIXaddress
// ---------------------------------------------------------------------------

wxUNIXaddress::wxUNIXaddress()
  : wxSockAddress()
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
  return wxString(path);
}

#endif

#endif 
  // wxUSE_SOCKETS
