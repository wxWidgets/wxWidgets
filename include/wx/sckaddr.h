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

#if defined(__WINDOWS__) && defined(WXSOCK_INTERNAL)
#include <winsock.h>

#elif defined(__UNIX__) && defined(WXSOCK_INTERNAL)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef __GNUG__
#pragma interface
#endif

#ifdef WXPREC
#include <wx/wxprec.h>
#else
#include <wx/wx.h>
#endif

class WXDLLEXPORT wxSockAddress : public wxObject {
  DECLARE_ABSTRACT_CLASS(wxSockAddress)
public:
  typedef enum { IPV4=1, IPV6=2, UNIX=3 } Addr;

  wxSockAddress() {};
  virtual ~wxSockAddress() {};

  virtual void Clear() = 0;

  virtual void Build(struct sockaddr*& addr, size_t& len) = 0;
  virtual void Disassemble(struct sockaddr *addr, size_t len) = 0;
  virtual int SockAddrLen() = 0;

  virtual int GetFamily() = 0;
  virtual int Type() = 0;
};

class WXDLLEXPORT wxIPV4address : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxIPV4address)
private:
  struct sockaddr_in *m_addr;
public:
  wxIPV4address();
  virtual ~wxIPV4address();

  virtual void Clear();
//  const wxSockAddress& operator =(const wxSockAddress& addr);

  virtual bool Hostname(const wxString& name);
  virtual bool Hostname(unsigned long addr);
  virtual bool Service(const wxString& name);
  virtual bool Service(unsigned short port);
  virtual bool LocalHost();

  wxString Hostname();
  unsigned short Service();

  void Build(struct sockaddr*& addr, size_t& len);
  void Disassemble(struct sockaddr *addr, size_t len);

  inline int SockAddrLen();
  inline int GetFamily();
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

  void Clear();
//  const wxSockAddress& operator =(const wxSockAddress& addr);

  bool Hostname(const wxString& name);
  bool Hostname(unsigned char addr[16]);
  bool Service(const wxString& name);
  bool Service(unsigned short port);
  bool LocalHost();

  wxString Hostname() const;
  unsigned short Service() const;

  void Build(struct sockaddr*& addr, size_t& len);
  void Disassemble(struct sockaddr *addr, size_t len);

  inline int SockAddrLen();
  inline int GetFamily();
  inline int Type() { return wxSockAddress::IPV6; }
};
#endif

#ifdef __UNIX__
#include <sys/un.h>

class WXDLLEXPORT wxUNIXaddress : public wxSockAddress {
  DECLARE_DYNAMIC_CLASS(wxUNIXaddress)
private:
  struct sockaddr_un *m_addr;
public:
  wxUNIXaddress();
  ~wxUNIXaddress();

  void Clear();
//  const wxSockAddress& operator =(const wxSockAddress& addr);

  void Filename(const wxString& name);
  wxString Filename();

  void Build(struct sockaddr*& addr, size_t& len);
  void Disassemble(struct sockaddr *addr, size_t len);

  inline int SockAddrLen();
  inline int GetFamily();
  inline int Type() { return wxSockAddress::UNIX; }
};
#endif

#endif
