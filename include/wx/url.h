/////////////////////////////////////////////////////////////////////////////
// Name:        url.h
// Purpose:     URL parser
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_URL_H
#define _WX_URL_H

#ifdef __GNUG__
#pragma interface
#endif

// wxWindows header
#include "wx/object.h"

// wxSocket headers
#include "wx/protocol/protocol.h"

#if wxUSE_SOCKETS
  #include "wx/protocol/http.h"
#endif

typedef enum {
  wxURL_NOERR = 0,
  wxURL_SNTXERR,
  wxURL_NOPROTO,
  wxURL_NOHOST,
  wxURL_NOPATH,
  wxURL_CONNERR,
  wxURL_PROTOERR
} wxURLError;
  
class WXDLLEXPORT wxURL : public wxObject {
  DECLARE_DYNAMIC_CLASS(wxURL)
protected:
  static wxProtoInfo *g_protocols;
#if wxUSE_SOCKETS
  static wxHTTP *g_proxy;
#endif
  wxProtoInfo *m_protoinfo;
  wxProtocol *m_protocol;
#if wxUSE_SOCKETS
  wxHTTP *m_proxy;
#endif
  wxURLError m_error;
  wxString m_protoname, m_hostname, m_servname, m_path, m_url;
  wxString m_user, m_password;
  bool m_useProxy;

  bool PrepProto(wxString& url);
  bool PrepHost(wxString& url);
  bool PrepPath(wxString& url);
  bool ParseURL();
  void CleanData();
  bool FetchProtocol();

  friend class wxProtoInfo;
  friend class wxProtocolModule;
public:

  wxURL(const wxString& url);
  virtual ~wxURL();

  inline wxString GetProtocolName() const
        { return m_protoinfo->m_protoname; }
  inline wxString GetHostName() const { return m_hostname; }
  inline wxString GetURL() const { return m_url; }
  inline wxProtocol& GetProtocol() { return *m_protocol; }
  inline wxURLError GetError() const { return m_error; }
  inline wxString GetPath() const { return m_path; }

  wxInputStream *GetInputStream();

#if wxUSE_SOCKETS
  static void SetDefaultProxy(const wxString& url_proxy);
  void SetProxy(const wxString& url_proxy);
#endif

  static wxString ConvertToValidURI(const wxString& uri);
};

#endif
