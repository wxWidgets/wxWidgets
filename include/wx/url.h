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
  
class WXDLLEXPORT wxURL : public wxObject
{
public:
    wxURL(const wxString& url);
    virtual ~wxURL();

    wxString GetProtocolName() const { return m_protoinfo->m_protoname; }
    wxString GetHostName() const     { return m_hostname; }
    wxString GetURL() const          { return m_url; }
    wxProtocol& GetProtocol()        { return *m_protocol; }
    wxURLError GetError() const      { return m_error; }
    wxString GetPath() const         { return m_path; }

    wxInputStream *GetInputStream();

#if wxUSE_SOCKETS
    static void SetDefaultProxy(const wxString& url_proxy);
    void SetProxy(const wxString& url_proxy);
#endif // wxUSE_SOCKETS

    static wxString ConvertToValidURI(
                        const wxString& uri,
                        const wxChar* delims = wxT(";/?:@&=+$,")
                    );
    static wxString ConvertFromURI(const wxString& uri);

protected:
    static wxProtoInfo *ms_protocols;

#if wxUSE_SOCKETS
    static wxHTTP *ms_proxyDefault;
    static bool ms_useDefaultProxy;
    wxHTTP *m_proxy;
#endif // wxUSE_SOCKETS

    wxProtoInfo *m_protoinfo;
    wxProtocol *m_protocol;

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
    friend class wxURLModule;

private:
    // VZ: can't use default copy ctor for this class, should write a correct
    //     one! (TODO)
    DECLARE_NO_COPY_CLASS(wxURL);

    DECLARE_DYNAMIC_CLASS(wxURL)
};

#endif
