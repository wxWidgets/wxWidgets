/////////////////////////////////////////////////////////////////////////////
// Name:        protocol.h
// Purpose:     Protocol base class
// Author:      Guilhem Lavaux
// Modified by:
// Created:     10/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_PROTOCOL_PROTOCOL_H
#define _WX_PROTOCOL_PROTOCOL_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/stream.h"
#include "wx/socket.h"

typedef enum {
 wxPROTO_NOERR = 0,
 wxPROTO_NETERR,
 wxPROTO_PROTERR,
 wxPROTO_CONNERR,
 wxPROTO_INVVAL,
 wxPROTO_NOHNDLR,
 wxPROTO_NOFILE,
 wxPROTO_ABRT,
 wxPROTO_RCNCT,
 wxPROTO_STREAMING 
} wxProtocolError;

// For protocols
#define DECLARE_PROTOCOL(class) \
public: \
  static wxProtoInfo g_proto_##class;

#define IMPLEMENT_PROTOCOL(class, name, serv, host) \
wxProtoInfo class::g_proto_##class(name, serv, host, CLASSINFO(class));

class WXDLLEXPORT wxProtoInfo : public wxObject {
  DECLARE_DYNAMIC_CLASS(wxProtoInfo)
protected:
  wxProtoInfo *next;
  wxString m_protoname;
  wxString prefix;
  wxString m_servname;
  wxClassInfo *m_cinfo;
  bool m_needhost;

  friend class wxURL;
public:
  wxProtoInfo(const char *name, const char *serv_name, const bool need_host1,
              wxClassInfo *info);
};

class WXDLLEXPORT wxProtocol : public wxSocketClient {
  DECLARE_ABSTRACT_CLASS(wxProtocol)
public:
  wxProtocol();

  bool Reconnect();
  virtual bool Connect( const wxString& WXUNUSED(host) ) { return FALSE; }
  virtual bool Connect( wxSockAddress& addr, bool WXUNUSED(wait) = TRUE) { return wxSocketClient::Connect(addr); }

  virtual bool Abort() = 0;
  virtual wxInputStream *GetInputStream(const wxString& path) = 0;
  virtual wxProtocolError GetError() = 0;
  virtual wxString GetContentType() { return wxEmptyString; }
  virtual void SetUser(const wxString& WXUNUSED(user)) {}
  virtual void SetPassword(const wxString& WXUNUSED(passwd) ) {}
};

wxProtocolError WXDLLEXPORT GetLine(wxSocketBase *sock, wxString& result);

#endif
