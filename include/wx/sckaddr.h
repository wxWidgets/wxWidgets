/////////////////////////////////////////////////////////////////////////////
// Name:        sckaddr.h
// Purpose:     Network address classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NETWORK_ADDRESS_H
#define _WX_NETWORK_ADDRESS_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/string.h"
#include "wx/gsocket.h"


class WXDLLEXPORT wxSockAddress : public wxObject {
  DECLARE_ABSTRACT_CLASS(wxSockAddress)
public:
  typedef enum { IPV4=1, IPV6=2, UNIX=3 } Addr;

  wxSockAddress();
  virtual ~wxSockAddress();

  virtual void Clear();
  virtual int Type() = 0;

  GAddress *GetAddress() const { return m_address; }
  void SetAddress(GAddress *address);
  const wxSockAddress& operator =(const wxSockAddress& addr);

  void CopyObject(wxObject& dest) const;

protected:
  GAddress *m_address;
};

class WXDLLEXPORT wxIPV4address : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxIPV4address)
public:
  wxIPV4address();
  virtual ~wxIPV4address();

  bool Hostname(const wxString& name);
  bool Hostname(unsigned long addr);
  bool Service(const wxString& name);
  bool Service(unsigned short port);
  bool LocalHost();
  bool AnyAddress();

  wxString Hostname();
  unsigned short Service();

  inline int Type() { return wxSockAddress::IPV4; }
};

#ifdef ENABLE_IPV6
class WXDLLEXPORT wxIPV6address : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxIPV6address)
private:
  struct sockaddr_in6 *m_addr;
public:
  wxIPV6address();
  ~wxIPV6address();

  bool Hostname(const wxString& name);
  bool Hostname(unsigned char addr[16]);
  bool Service(const wxString& name);
  bool Service(unsigned short port);
  bool LocalHost();

  wxString Hostname() const;
  unsigned short Service() const;

  inline int Type() { return wxSockAddress::IPV6; }
};
#endif

#ifdef __UNIX__
#include <sys/socket.h>
#ifndef __VMS__
# include <sys/un.h>
#endif

class WXDLLEXPORT wxUNIXaddress : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxUNIXaddress)
private:
  struct sockaddr_un *m_addr;
public:
  wxUNIXaddress();
  ~wxUNIXaddress();

  void Filename(const wxString& name);
  wxString Filename();

  inline int Type() { return wxSockAddress::UNIX; }
};
#endif
  // __UNIX__

#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_ADDRESS_H
