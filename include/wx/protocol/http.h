/////////////////////////////////////////////////////////////////////////////
// Name:        http.h
// Purpose:     HTTP protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     August 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_HTTP_H
#define _WX_HTTP_H

#include "wx/defs.h"

#if wxUSE_PROTOCOL_HTTP

#include "wx/hashmap.h"
#include "wx/protocol/protocol.h"

WX_DECLARE_EXPORTED_STRING_HASH_MAP( wxString, wxStringToStringHashMap );

class WXDLLIMPEXP_BASE wxHTTP : public wxProtocol {
  DECLARE_DYNAMIC_CLASS(wxHTTP)
  DECLARE_PROTOCOL(wxHTTP)
protected:
  wxProtocolError m_perr;
  wxStringToStringHashMap m_headers;
  bool m_read, m_proxy_mode;
  wxSockAddress *m_addr;
  wxString m_post_buf;
public:
  wxHTTP();
  ~wxHTTP();

  bool Connect(const wxString& host, unsigned short port = 0);
  bool Connect(wxSockAddress& addr, bool wait);
  bool Abort();
  wxInputStream *GetInputStream(const wxString& path);
  inline wxProtocolError GetError() { return m_perr; }
  wxString GetContentType();

  void SetHeader(const wxString& header, const wxString& h_data);
  wxString GetHeader(const wxString& header);
  void SetPostBuffer(const wxString& post_buf);

  void SetProxyMode(bool on);

protected:
  typedef enum {
    wxHTTP_GET,
    wxHTTP_POST,
    wxHTTP_HEAD
  } wxHTTP_Req;
  bool BuildRequest(const wxString& path, wxHTTP_Req req);
  void SendHeaders();
  bool ParseHeaders();

  // deletes the header value strings
  void ClearHeaders();

    DECLARE_NO_COPY_CLASS(wxHTTP)
};

#endif // wxUSE_PROTOCOL_HTTP

#endif // _WX_HTTP_H

