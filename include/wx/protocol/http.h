/////////////////////////////////////////////////////////////////////////////
// Name:        http.h
// Purpose:     HTTP protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     August 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_HTTP_H
#define _WX_HTTP_H

#include "wx/list.h"
#include "wx/protocol/protocol.h"

class WXDLLEXPORT wxHTTP : public wxProtocol {
  DECLARE_DYNAMIC_CLASS(wxHTTP)
  DECLARE_PROTOCOL(wxHTTP)
protected:
  wxProtocolError m_perr;
  wxList m_headers;
  bool m_read;
  wxSockAddress *m_addr;
public:
  wxHTTP();
  ~wxHTTP();

  bool Connect(const wxString& host);
  bool Connect(wxSockAddress& addr);
  bool Abort();
  wxInputStream *GetInputStream(const wxString& path);
  inline wxProtocolError GetError() { return m_perr; }
  wxString GetContentType();

  void SetHeader(const wxString& header, const wxString& h_data);
  wxString GetHeader(const wxString& header);

protected:
  typedef enum {
    wxHTTP_GET
  } wxHTTP_Req;
  bool BuildRequest(const wxString& path, wxHTTP_Req req);
  void SendHeaders();
  bool ParseHeaders();
};

#endif
