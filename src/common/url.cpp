/////////////////////////////////////////////////////////////////////////////
// Name:        url.cpp
// Purpose:     URL parser
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "url.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_URL

#include "wx/string.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/module.h"
#include "wx/url.h"

#include <string.h>
#include <ctype.h>

IMPLEMENT_CLASS(wxProtoInfo, wxObject)
IMPLEMENT_CLASS(wxURL, wxURI)

// Protocols list
wxProtoInfo *wxURL::ms_protocols = NULL;

// Enforce linking of protocol classes:
USE_PROTOCOL(wxFileProto)

#if wxUSE_SOCKETS
USE_PROTOCOL(wxHTTP)
USE_PROTOCOL(wxFTP)

    wxHTTP *wxURL::ms_proxyDefault = NULL;
    bool wxURL::ms_useDefaultProxy = false;
#endif

// --------------------------------------------------------------
//
//                          wxURL
//
// --------------------------------------------------------------

// --------------------------------------------------------------
// Construction
// --------------------------------------------------------------

wxURL::wxURL(const wxString& url) : wxURI(url)
{
    Init(url);
    ParseURL();
}

wxURL::wxURL(const wxURI& url) : wxURI(url)
{
    Init(url.BuildURI());
    ParseURL();
}

void wxURL::Init(const wxString& url)
{
    m_protocol = NULL;
    m_error = wxURL_NOERR;
    m_url = url;
#if wxUSE_URL_NATIVE
    m_nativeImp = CreateNativeImpObject();
#endif

#if wxUSE_SOCKETS
    if ( ms_useDefaultProxy && !ms_proxyDefault )
    {
        SetDefaultProxy( wxGetenv(wxT("HTTP_PROXY")) );

        if ( !ms_proxyDefault )
        {
            // don't try again
            ms_useDefaultProxy = false;
        }
    }

    m_useProxy = ms_proxyDefault != NULL;
    m_proxy = ms_proxyDefault;
#endif // wxUSE_SOCKETS

}

// --------------------------------------------------------------
// Assignment
// --------------------------------------------------------------

wxURL& wxURL::operator = (const wxURI& url)
{
    wxURI::operator = (url);
    Init(url.BuildURI());
    ParseURL();
    return *this;
}
wxURL& wxURL::operator = (const wxString& url)
{
    wxURI::operator = (url);
    Init(url);
    ParseURL();
    return *this;
}

// --------------------------------------------------------------
// ParseURL
//
// Builds the URL and takes care of the old protocol stuff
// --------------------------------------------------------------

bool wxURL::ParseURL()
{
  // If the URL was already parsed (m_protocol != NULL), pass this section.
  if (!m_protocol)
  {
    // Clean up
    CleanData();

    // Make sure we have a protocol/scheme
    if (!HasScheme())
    {
      m_error = wxURL_SNTXERR;
      return false;
    }

    // Find and create the protocol object
    if (!FetchProtocol())
    {
      m_error = wxURL_NOPROTO;
      return false;
    }

    // Do we need a host name ?
    if (m_protoinfo->m_needhost)
    {
      //  Make sure we have one, then
      if (!HasServer())
      {
        m_error = wxURL_SNTXERR;
        return false;
      }
    }
  }

#if wxUSE_SOCKETS
  if (m_useProxy)
  {
    // destroy the previously created protocol as we'll be using m_proxy
    delete m_protocol;

    // Third, we rebuild the URL.
    m_url = m_scheme + wxT(":");
    if (m_protoinfo->m_needhost)
      m_url = m_url + wxT("//") + m_server;

    // We initialize specific variables.
    m_protocol = m_proxy; // FIXME: we should clone the protocol
  }
#endif

  m_error = wxURL_NOERR;
  return true;
}

// --------------------------------------------------------------
// Destruction/Cleanup
// --------------------------------------------------------------

void wxURL::CleanData()
{
#if wxUSE_SOCKETS
  if (!m_useProxy)
#endif
    delete m_protocol;
}

wxURL::~wxURL()
{
    CleanData();
#if wxUSE_SOCKETS
    if (m_proxy && m_proxy != ms_proxyDefault)
        delete m_proxy;
#endif
#if wxUSE_URL_NATIVE
    delete m_nativeImp;
#endif
}

// --------------------------------------------------------------
// FetchProtocol
// --------------------------------------------------------------

bool wxURL::FetchProtocol()
{
  wxProtoInfo *info = ms_protocols;

  while (info)
  {
    if (m_scheme == info->m_protoname)
    {
        if (m_port.IsNull())
            m_port = info->m_servname;
        m_protoinfo = info;
        m_protocol = (wxProtocol *)m_protoinfo->m_cinfo->CreateObject();
        return true;
    }
    info = info->next;
  }
  return false;
}

// --------------------------------------------------------------
// GetInputStream
// --------------------------------------------------------------

wxInputStream *wxURL::GetInputStream()
{
  if (!m_protocol)
  {
    m_error = wxURL_NOPROTO;
    return NULL;
  }

  m_error = wxURL_NOERR;
  if (HasUser())
  {
      size_t dwPasswordPos = m_user.find(':');

      if (dwPasswordPos == wxString::npos)
          m_protocol->SetUser(m_user);
      else
      {
          m_protocol->SetUser(m_user(0, dwPasswordPos));
          m_protocol->SetPassword(m_user(dwPasswordPos+1, m_user.length() + 1));
      }
  }

#if wxUSE_URL_NATIVE
  // give the native implementation to return a better stream
  // such as the native WinINet functionality under MS-Windows
  if (m_nativeImp)
  {
    wxInputStream *rc;
    rc = m_nativeImp->GetInputStream(this);
    if (rc != 0)
        return rc;
  }
  // else use the standard behaviour
#endif // wxUSE_URL_NATIVE

#if wxUSE_SOCKETS
    wxIPV4address addr;

  // m_protoinfo is NULL when we use a proxy
  if (!m_useProxy && m_protoinfo->m_needhost)
  {
    if (!addr.Hostname(m_server))
    {
      m_error = wxURL_NOHOST;
      return NULL;
    }

    addr.Service(m_port);

    if (!m_protocol->Connect(addr, true)) // Watcom needs the 2nd arg for some reason
    {
      m_error = wxURL_CONNERR;
      return NULL;
    }
  }
#endif

  // When we use a proxy, we have to pass the whole URL to it.
  wxInputStream *the_i_stream;
  
  if (!m_useProxy)
  {
      the_i_stream = m_protocol->GetInputStream(m_url);
  }
  else
  {
      wxString fullPath = m_path;

      if (HasQuery())
          fullPath += wxT("?") + m_query;
      
      if (HasFragment())
          fullPath += wxT("#") + m_fragment;
      
      the_i_stream = m_protocol->GetInputStream(fullPath);
  }

  if (!the_i_stream)
  {
    m_error = wxURL_PROTOERR;
    return NULL;
  }

  return the_i_stream;
}

#if wxUSE_SOCKETS
void wxURL::SetDefaultProxy(const wxString& url_proxy)
{
  if ( !url_proxy )
  {
      if ( ms_proxyDefault )
      {
          ms_proxyDefault->Close();
          delete ms_proxyDefault;
          ms_proxyDefault = NULL;
      }
  }
  else
  {
      wxString tmp_str = url_proxy;
      int pos = tmp_str.Find(wxT(':'));
      if (pos == wxNOT_FOUND)
          return;

      wxString hostname = tmp_str(0, pos),
      port = tmp_str(pos+1, tmp_str.Length()-pos);
      wxIPV4address addr;

      if (!addr.Hostname(hostname))
          return;
      if (!addr.Service(port))
          return;

      if (ms_proxyDefault)
          // Finally, when all is right, we connect the new proxy.
          ms_proxyDefault->Close();
      else
          ms_proxyDefault = new wxHTTP();
      ms_proxyDefault->Connect(addr, true); // Watcom needs the 2nd arg for some reason
  }
}

void wxURL::SetProxy(const wxString& url_proxy)
{
    if ( !url_proxy )
    {
        if ( m_proxy && m_proxy != ms_proxyDefault )
        {
            m_proxy->Close();
            delete m_proxy;
        }

        m_useProxy = false;
    }
    else
    {
        wxString tmp_str;
        wxString hostname, port;
        int pos;
        wxIPV4address addr;

        tmp_str = url_proxy;
        pos = tmp_str.Find(wxT(':'));
        // This is an invalid proxy name.
        if (pos == wxNOT_FOUND)
            return;

        hostname = tmp_str(0, pos);
        port = tmp_str(pos+1, tmp_str.Length()-pos);

        addr.Hostname(hostname);
        addr.Service(port);

        // Finally, create the whole stuff.
        if (m_proxy && m_proxy != ms_proxyDefault)
            delete m_proxy;
        m_proxy = new wxHTTP();
        m_proxy->Connect(addr, true); // Watcom needs the 2nd arg for some reason

        CleanData();
        // Reparse url.
        m_useProxy = true;
        ParseURL();
    }
}
#endif // wxUSE_SOCKETS

// ----------------------------------------------------------------------
// wxURLModule
//
// A module which deletes the default proxy if we created it
// ----------------------------------------------------------------------

#if wxUSE_SOCKETS

class wxURLModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxURLModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxURLModule, wxModule)

bool wxURLModule::OnInit()
{
    // env var HTTP_PROXY contains the address of the default proxy to use if
    // set, but don't try to create this proxy right now because it will slow
    // down the program startup (especially if there is no DNS server
    // available, in which case it may take up to 1 minute)

    if ( getenv("HTTP_PROXY") )
    {
        wxURL::ms_useDefaultProxy = true;
    }

    return true;
}

void wxURLModule::OnExit()
{
    delete wxURL::ms_proxyDefault;
    wxURL::ms_proxyDefault = NULL;
}

#endif // wxUSE_SOCKETS

#endif // wxUSE_URL

