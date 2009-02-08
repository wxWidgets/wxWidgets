/////////////////////////////////////////////////////////////////////////////
// Name:        http.h
// Purpose:     HTTP protocol
// Author:      Guilhem Lavaux
// Modified by: Simo Virokannas (authentication, Dec 2005)
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

class WXDLLIMPEXP_NET wxHTTP : public wxProtocol
{
public:
    wxHTTP();
    virtual ~wxHTTP();

    virtual bool Connect(const wxString& host, unsigned short port);
    virtual bool Connect(const wxString& host) { return Connect(host, 0); }
    virtual bool Connect(const wxSockAddress& addr, bool wait);
    bool Abort();

    wxInputStream *GetInputStream(const wxString& path);

    wxString GetContentType() const;
    wxString GetHeader(const wxString& header) const;
    int GetResponse() const { return m_http_response; }

    void SetHeader(const wxString& header, const wxString& h_data);
    void SetPostBuffer(const wxString& post_buf);
    void SetProxyMode(bool on);

protected:
    enum wxHTTP_Req
    {
        wxHTTP_GET,
        wxHTTP_POST,
        wxHTTP_HEAD
    };

    typedef wxStringToStringHashMap::iterator wxHeaderIterator;
    typedef wxStringToStringHashMap::const_iterator wxHeaderConstIterator;

    bool BuildRequest(const wxString& path, wxHTTP_Req req);
    void SendHeaders();
    bool ParseHeaders();

    wxString GenerateAuthString(const wxString& user, const wxString& pass) const;

    // find the header in m_headers
    wxHeaderIterator FindHeader(const wxString& header);
    wxHeaderConstIterator FindHeader(const wxString& header) const;

    // deletes the header value strings
    void ClearHeaders();


    // internal variables:

    wxStringToStringHashMap m_headers;
    bool m_read,
         m_proxy_mode;
    wxSockAddress *m_addr;
    wxString m_post_buf;
    int m_http_response;

    DECLARE_DYNAMIC_CLASS(wxHTTP)
    DECLARE_PROTOCOL(wxHTTP)
    wxDECLARE_NO_COPY_CLASS(wxHTTP);
};

#endif // wxUSE_PROTOCOL_HTTP

#endif // _WX_HTTP_H

