/////////////////////////////////////////////////////////////////////////////
// Name:        sckaddr.h
// Purpose:     Network address classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NETWORK_ADDRESS_H
#define _WX_NETWORK_ADDRESS_H

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "sckaddr.h"
#endif

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/string.h"
#include "wx/gsocket.h"


class WXDLLIMPEXP_BASE wxSockAddress : public wxObject {
  DECLARE_ABSTRACT_CLASS(wxSockAddress)
public:
  typedef enum { IPV4=1, IPV6=2, UNIX=3 } Addr;

  wxSockAddress();
  wxSockAddress(const wxSockAddress& other);
  virtual ~wxSockAddress();

  wxSockAddress& operator=(const wxSockAddress& other);

  virtual void Clear();
  virtual int Type() = 0;

  GAddress *GetAddress() const { return m_address; }
  void SetAddress(GAddress *address);

  // we need to be able to create copies of the addresses polymorphically (i.e.
  // without knowing the exact address class)
  virtual wxSockAddress *Clone() const = 0;

protected:
  GAddress *m_address;

private:
  void Init();
};

class WXDLLIMPEXP_BASE wxIPV4address : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxIPV4address)
public:
  wxIPV4address();
  wxIPV4address(const wxIPV4address& other);
  virtual ~wxIPV4address();

  bool Hostname(const wxString& name);
  bool Hostname(unsigned long addr);
  bool Service(const wxString& name);
  bool Service(unsigned short port);
  bool LocalHost();
  bool AnyAddress();

  wxString Hostname();
  wxString OrigHostname() { return m_origHostname; }
  unsigned short Service();
  wxString IPAddress() const;

  virtual int Type() { return wxSockAddress::IPV4; }
  virtual wxSockAddress *Clone() const;

private:
  wxString m_origHostname;
};

#ifdef ENABLE_IPV6
class WXDLLIMPEXP_BASE wxIPV6address : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxIPV6address)
private:
  struct sockaddr_in6 *m_addr;
public:
  wxIPV6address();
  wxIPV6address(const wxIPV6address& other);
  virtual ~wxIPV6address();

  bool Hostname(const wxString& name);
  bool Hostname(unsigned char addr[16]);
  bool Service(const wxString& name);
  bool Service(unsigned short port);
  bool LocalHost();

  wxString Hostname() const;
  unsigned short Service() const;

  virtual int Type() { return wxSockAddress::IPV6; }
  virtual wxSockAddress *Clone() const { return new wxIPV6address(*this); }
};
#endif

#if defined(__UNIX__) && !defined(__WINE__) && (!defined(__WXMAC__) || defined(__DARWIN__))
#include <sys/socket.h>
#ifndef __VMS__
# include <sys/un.h>
#endif

class WXDLLIMPEXP_BASE wxUNIXaddress : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxUNIXaddress)
private:
  struct sockaddr_un *m_addr;
public:
  wxUNIXaddress();
  wxUNIXaddress(const wxUNIXaddress& other);
  virtual ~wxUNIXaddress();

  void Filename(const wxString& name);
  wxString Filename();

  virtual int Type() { return wxSockAddress::UNIX; }
  virtual wxSockAddress *Clone() const { return new wxUNIXaddress(*this); }
};
#endif
  // __UNIX__

#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_ADDRESS_H
