/////////////////////////////////////////////////////////////////////////////
// Name:        protocol.cpp
// Purpose:     Implement protocol base class
// Author:      Guilhem Lavaux
// Modified by:
// Created:     07/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "protocol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/protocol/protocol.h"
#include "wx/url.h"
#include "wx/module.h"

#include <stdlib.h>

/////////////////////////////////////////////////////////////////
// wxProtoInfo
/////////////////////////////////////////////////////////////////

/*
 * --------------------------------------------------------------
 * --------- wxProtoInfo CONSTRUCTOR ----------------------------
 * --------------------------------------------------------------
 */

wxProtoInfo::wxProtoInfo(const wxChar *name, const wxChar *serv,
                         const bool need_host1, wxClassInfo *info)
{
  m_protoname = name;
  m_servname = serv;
  m_cinfo = info;
  m_needhost = need_host1;
  next = wxURL::g_protocols;
  wxURL::g_protocols = this;
}

/////////////////////////////////////////////////////////////////
// wxProtocol ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#if wxUSE_SOCKETS
IMPLEMENT_ABSTRACT_CLASS(wxProtocol, wxSocketClient)
#else
IMPLEMENT_ABSTRACT_CLASS(wxProtocol, wxObject)
#endif

wxProtocol::wxProtocol()
#if wxUSE_SOCKETS
 : wxSocketClient()
#endif
{
}

#if wxUSE_SOCKETS
bool wxProtocol::Reconnect()
{
  wxIPV4address addr;

  if (!GetPeer(addr))
  {
    Close();
    return FALSE;
  }
  if (!Close())
    return FALSE;
  if (!Connect(addr))
    return FALSE;

  return TRUE;
}

wxProtocolError GetLine(wxSocketBase *sock, wxString& result) {
#define PROTO_BSIZE 2048
  size_t avail, size;
  char tmp_buf[PROTO_BSIZE], tmp_str[PROTO_BSIZE];
  char *ret;
  bool found;

  avail = sock->Read(tmp_buf, PROTO_BSIZE).LastCount();
  if (sock->Error() || avail == 0)
    return wxPROTO_NETERR;

  memcpy(tmp_str, tmp_buf, avail);

// Not implemented on all systems
// ret = (char *)memccpy(tmp_str, tmp_buf, '\n', avail);
  found = FALSE;
  for (ret=tmp_str;ret < (tmp_str+avail); ret++)
     if (*ret == '\n') {
       found = TRUE;
       break;
     }

  if (!found)
    return wxPROTO_PROTERR;
  *ret = 0;

  result = tmp_str;
  result = result.Left(result.Length()-1);

  size = ret-tmp_str+1;
  sock->Unread(&tmp_buf[size], avail-size);
  return wxPROTO_NOERR;
#undef PROTO_BSIZE
}
#endif

// ----------------------------------------------------------------------
// Module
// ----------------------------------------------------------------------

class wxProtocolModule: public wxModule {
  DECLARE_DYNAMIC_CLASS(wxProtocolModule)
public:
  wxProtocolModule() {}
  bool OnInit();
  void OnExit();
};

IMPLEMENT_DYNAMIC_CLASS(wxProtocolModule, wxModule)

bool wxProtocolModule::OnInit()
{
#if wxUSE_SOCKETS
  char *env_http_prox;

  wxURL::g_proxy = NULL;
  // Initialize the proxy when HTTP_PROXY is defined
  env_http_prox = getenv("HTTP_PROXY");
  if (env_http_prox)
    wxURL::SetDefaultProxy(env_http_prox);
#endif
  
  return TRUE;
}

void wxProtocolModule::OnExit()
{
#if wxUSE_SOCKETS
  if (wxURL::g_proxy)
    delete wxURL::g_proxy;
  wxURL::g_proxy = NULL;
#endif
}

