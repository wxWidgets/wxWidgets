/////////////////////////////////////////////////////////////////////////////
// Name:        url.cpp
// Purpose:     URL parser
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "url.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

#ifndef WX_PRECOMP
#endif

#include <string.h>
#include <ctype.h>

// wxWindows headers
#include <wx/string.h>
#include <wx/list.h>
#include <wx/utils.h>

// wxSocket header
#include "wx/url.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxProtoInfo, wxObject)
IMPLEMENT_CLASS(wxURL, wxObject)
#endif

// Protocols list
wxProtoInfo *wxURL::g_protocols = NULL;
wxHTTP *wxURL::g_proxy;

// --------------------------------------------------------------
// wxURL
// --------------------------------------------------------------

// --------------------------------------------------------------
// --------- wxURL CONSTRUCTOR DESTRUCTOR -----------------------
// --------------------------------------------------------------

wxURL::wxURL(const wxString& url)
{
  m_protocol = NULL;
  if (g_proxy->IsConnected()) {
    m_protocol = g_proxy;
    m_protoname = "proxy";
    m_path = url;
    return;
  }
  m_url = url;
  m_error = wxURL_NOERR;
  ParseURL();
}

bool wxURL::ParseURL()
{
  wxString last_url = m_url;

  // Clean up
  CleanData();

  // Extract protocol name
  if (!PrepProto(last_url)) {
    m_error = wxURL_SNTXERR;
    return FALSE;
  }

  // Find and create the protocol object
  if (!FetchProtocol()) {
    m_error = wxURL_NOPROTO;
    return FALSE;
  }

  // Do we need a host name ?
  if (m_protoinfo->m_needhost) {
    // Extract it
    if (!PrepHost(last_url)) {
      m_error = wxURL_SNTXERR;
      return FALSE;
    }
  }

  // Extract full path
  if (!PrepPath(last_url)) {
    m_error = wxURL_NOPATH;
    return FALSE;
  }

  m_error = wxURL_NOERR;
  return TRUE;
}

void wxURL::CleanData()
{
  if (m_protoname != _T("proxy"))
    delete m_protocol;
}

wxURL::~wxURL()
{
  CleanData();
}

// --------------------------------------------------------------
// --------- wxURL urls decoders --------------------------------
// --------------------------------------------------------------

bool wxURL::PrepProto(wxString& url)
{
  int pos;

  // Find end
  pos = url.Find(':');
  if (pos == -1)
    return FALSE;

  m_protoname = url(0, pos);

  url = url(pos+1, url.Length());

  return TRUE;
}

bool wxURL::PrepHost(wxString& url)
{
  wxString temp_url;
  int pos, pos2;

  if ((url.GetChar(0) != '/') || (url.GetChar(1) != '/'))
    return FALSE;

  url = url(2, url.Length());

  pos = url.Find('/');
  if (pos == -1)
    pos = url.Length();

  if (pos == 0)
    return FALSE;

  temp_url = url(0, pos);
  url = url(url.Find('/'), url.Length());

  // Retrieve service number
  pos2 = temp_url.Find(':', TRUE);
  if (pos2 != -1 && pos2 < pos) {
    m_servname = temp_url(pos2+1, pos);
    if (!m_servname.IsNumber())
      return FALSE;
    temp_url = temp_url(0, pos2);
  }

  // Retrieve user and password.
  pos2 = temp_url.Find('@');
  // Even if pos2 equals -1, this code is right.
  m_hostname = temp_url(pos2+1, temp_url.Length());

  m_user = "";
  m_password = "";

  if (pos2 == -1)
    return TRUE;

  temp_url = temp_url(0, pos2);
  pos2 = temp_url.Find(':');

  if (pos2 == -1)
    return FALSE;

  m_user = temp_url(0, pos2);
  m_password = temp_url(pos2+1, url.Length());

  return TRUE;
}

bool wxURL::PrepPath(wxString& url)
{
  if (url.Length() != 0)
    m_path = url;
  else
    m_path = "/";
  return TRUE;
}

bool wxURL::FetchProtocol()
{
  wxProtoInfo *info = g_protocols;

  while (info) {
    if (m_protoname == info->m_protoname) {
      if (m_servname.IsNull())
        m_servname = info->m_servname;

      m_protoinfo = info;
      m_protocol = (wxProtocol *)m_protoinfo->m_cinfo->CreateObject();
      return TRUE;
    }
    info = info->next;
  }
  return FALSE;
}

// --------------------------------------------------------------
// --------- wxURL get ------------------------------------------
// --------------------------------------------------------------

wxInputStream *wxURL::GetInputStream(void)
{
  wxIPV4address addr;
  wxInputStream *the_i_stream = NULL;

  if (!m_protocol)
    if (!ParseURL())
      return NULL;

  if (!m_protocol) {
    m_error = wxURL_NOPROTO;
    return NULL;
  }

  m_error = wxURL_NOERR;
  if (m_user != _T("")) {
    m_protocol->SetUser(m_user);
    m_protocol->SetPassword(m_password);
  }

  if (m_protoinfo->m_needhost) {
    if (!addr.Hostname(m_hostname)) {
      m_error = wxURL_NOHOST;
      return NULL;
    }

    addr.Service(m_servname);

    if (!m_protocol->Connect(addr, TRUE)) // Watcom needs the 2nd arg for some reason
    {
      m_error = wxURL_CONNERR;
      return NULL;
    }
  }

  the_i_stream = m_protocol->GetInputStream(m_path);
  if (!the_i_stream) {
    m_error = wxURL_PROTOERR;
    return NULL;
  }

  return the_i_stream;
}

void wxURL::SetDefaultProxy(const wxString& url_proxy)
{
  g_proxy->Close();

  if (url_proxy.IsNull())
    return;

  wxString tmp_str = url_proxy;
  int pos = tmp_str.Find(':');
  wxString hostname = tmp_str(0, pos),
           port = tmp_str(pos, tmp_str.Length()-pos);
  wxIPV4address addr;

  addr.Hostname(hostname);
  addr.Service(port);
  
  g_proxy->Connect(addr, TRUE); // Watcom needs the 2nd arg for some reason
}

void wxURL::SetProxy(const wxString& url_proxy)
{
  if (url_proxy.IsNull()) {
    m_proxy.Close();
    return;
  }

  CleanData();

  wxString tmp_str;
  wxString hostname, port;
  int pos;
  wxIPV4address addr;

  tmp_str = url_proxy;
  pos = tmp_str.Find(':');
  hostname = tmp_str(0, pos);
  port = tmp_str(pos, tmp_str.Length()-pos);

  addr.Hostname(hostname);
  addr.Service(port);

  m_proxy.Connect(addr, TRUE); // Watcom needs the 2nd arg for some reason

  m_protocol = &m_proxy;
  m_protoname = "proxy";
  m_path = url_proxy;
}

wxString wxURL::ConvertToValidURI(const wxString& uri)
{
  wxString out_str;
  wxString hexa_code;
  size_t i;

  for (i=0;i<uri.Len();i++) {
    wxChar c = uri.GetChar(i);

    if (c == _T(' '))
      out_str += _T('+');
    else {
      if (!isalpha(c) && c != _T('.') && c != _T('+') && c != _T('.') && 
                         c != _T('/')) {
        hexa_code.Printf(_T("%%%02X"), c);
        out_str += hexa_code;
      } else
        out_str += c;
    }
  }
  
  return out_str;
}

#endif
  // wxUSE_SOCKETS
