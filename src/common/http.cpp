/////////////////////////////////////////////////////////////////////////////
// Name:        http.cpp
// Purpose:     HTTP protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     August 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma implementation "http.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_PROTOCOL_HTTP

#include <stdio.h>
#include <stdlib.h>

#ifndef WX_PRECOMP
#include "wx/string.h"
#include "wx/app.h"
#endif

#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "wx/protocol/protocol.h"
#include "wx/url.h"
#include "wx/protocol/http.h"
#include "wx/sckstrm.h"

IMPLEMENT_DYNAMIC_CLASS(wxHTTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxHTTP, wxT("http"), wxT("80"), true)

#define HTTP_BSIZE 2048

wxHTTP::wxHTTP()
  : wxProtocol()
{
  m_addr = NULL;
  m_read = false;
  m_proxy_mode = false;
  m_post_buf = wxEmptyString;
  m_http_response = 0;

  SetNotify(wxSOCKET_LOST_FLAG);
}

wxHTTP::~wxHTTP()
{
    ClearHeaders();

    delete m_addr;
}

void wxHTTP::ClearHeaders()
{
  m_headers.clear();
}

wxString wxHTTP::GetContentType()
{
  return GetHeader(wxT("Content-Type"));
}

void wxHTTP::SetProxyMode(bool on)
{
  m_proxy_mode = on;
}

wxHTTP::wxHeaderIterator wxHTTP::FindHeader(const wxString& header)
{
    wxHeaderIterator it = m_headers.begin();
    for ( wxHeaderIterator en = m_headers.end(); it != en; ++it )
    {
        if ( wxStricmp(it->first, header) == 0 )
            break;
    }

    return it;
}

wxHTTP::wxHeaderConstIterator wxHTTP::FindHeader(const wxString& header) const
{
    wxHeaderConstIterator it = m_headers.begin();
    for ( wxHeaderConstIterator en = m_headers.end(); it != en; ++it )
    {
        if ( wxStricmp(it->first, header) == 0 )
            break;
    }

    return it;
}

void wxHTTP::SetHeader(const wxString& header, const wxString& h_data)
{
  if (m_read) {
    ClearHeaders();
    m_read = false;
  }

  wxHeaderIterator it = FindHeader(header);
  if (it != m_headers.end())
    it->second = h_data;
  else
    m_headers[header] = h_data;
}

wxString wxHTTP::GetHeader(const wxString& header) const
{
    wxHeaderConstIterator it = FindHeader(header);

    return it == m_headers.end() ? wxGetEmptyString() : it->second;
}

void wxHTTP::SetPostBuffer(const wxString& post_buf)
{
    m_post_buf = post_buf;
}

void wxHTTP::SendHeaders()
{
  typedef wxStringToStringHashMap::iterator iterator;
  wxString buf;

  for (iterator it = m_headers.begin(), en = m_headers.end(); it != en; ++it )
  {
    buf.Printf(wxT("%s: %s\r\n"), it->first.c_str(), it->second.c_str());

    const wxWX2MBbuf cbuf = buf.mb_str();
    Write(cbuf, strlen(cbuf));
  }
}

bool wxHTTP::ParseHeaders()
{
  wxString line;
  wxStringTokenizer tokenzr;

  ClearHeaders();
  m_read = true;

#if defined(__VISAGECPP__)
// VA just can't stand while(1)
    bool bOs2var = true;
    while(bOs2var)
#else
  while (1)
#endif
  {
    m_perr = GetLine(this, line);
    if (m_perr != wxPROTO_NOERR)
      return false;

    if (line.Length() == 0)
      break;

    wxString left_str = line.BeforeFirst(':');
    m_headers[left_str] = line.AfterFirst(':').Strip(wxString::both);
  }
  return true;
}

bool wxHTTP::Connect(const wxString& host, unsigned short port)
{
  wxIPV4address *addr;

  if (m_addr) {
    delete m_addr;
    m_addr = NULL;
    Close();
  }

  m_addr = addr = new wxIPV4address();

  if (!addr->Hostname(host)) {
    delete m_addr;
    m_addr = NULL;
    m_perr = wxPROTO_NETERR;
    return false;
  }

  if ( port ) addr->Service(port);
  else if (!addr->Service(wxT("http")))
    addr->Service(80);

  SetHeader(wxT("Host"), host);

  return true;
}

bool wxHTTP::Connect(wxSockAddress& addr, bool WXUNUSED(wait))
{
  if (m_addr) {
    delete m_addr;
    Close();
  }

  m_addr = addr.Clone();

  wxIPV4address *ipv4addr = wxDynamicCast(&addr, wxIPV4address);
  if (ipv4addr)
      SetHeader(wxT("Host"), ipv4addr->OrigHostname());

  return true;
}

bool wxHTTP::BuildRequest(const wxString& path, wxHTTP_Req req)
{
  const wxChar *request;

  switch (req) {
  case wxHTTP_GET:
    request = wxT("GET");
    break;
  case wxHTTP_POST:
    request = wxT("POST");
    if ( GetHeader( wxT("Content-Length") ).IsNull() )
      SetHeader( wxT("Content-Length"), wxString::Format( wxT("%lu"), (unsigned long)m_post_buf.Len() ) );
    break;
  default:
    return false;
  }

  m_http_response = 0;

  // If there is no User-Agent defined, define it.
  if (GetHeader(wxT("User-Agent")).IsNull())
    SetHeader(wxT("User-Agent"), wxT("wxWidgets 2.x"));

  SaveState();

  // we may use non blocking sockets only if we can dispatch events from them
  SetFlags( wxIsMainThread() && wxApp::IsMainLoopRunning() ? wxSOCKET_NONE
                                                           : wxSOCKET_BLOCK );
  Notify(false);

  wxString buf;
  buf.Printf(wxT("%s %s HTTP/1.0\r\n"), request, path.c_str());
  const wxWX2MBbuf pathbuf = wxConvLocal.cWX2MB(buf);
  Write(pathbuf, strlen(wxMBSTRINGCAST pathbuf));
  SendHeaders();
  Write("\r\n", 2);

  if ( req == wxHTTP_POST ) {
    Write(m_post_buf.mbc_str(), m_post_buf.Len());
    m_post_buf = wxEmptyString;
  }

  wxString tmp_str;
  m_perr = GetLine(this, tmp_str);
  if (m_perr != wxPROTO_NOERR) {
    RestoreState();
    return false;
  }

  if (!tmp_str.Contains(wxT("HTTP/"))) {
    // TODO: support HTTP v0.9 which can have no header.
    // FIXME: tmp_str is not put back in the in-queue of the socket.
    SetHeader(wxT("Content-Length"), wxT("-1"));
    SetHeader(wxT("Content-Type"), wxT("none/none"));
    RestoreState();
    return true;
  }

  wxStringTokenizer token(tmp_str,wxT(' '));
  wxString tmp_str2;
  bool ret_value;

  token.NextToken();
  tmp_str2 = token.NextToken();

  m_http_response = wxAtoi(tmp_str2);

  switch (tmp_str2[0u]) {
  case wxT('1'):
    /* INFORMATION / SUCCESS */
    break;
  case wxT('2'):
    /* SUCCESS */
    break;
  case wxT('3'):
    /* REDIRECTION */
    break;
  default:
    m_perr = wxPROTO_NOFILE;
    RestoreState();
    return false;
  }

  ret_value = ParseHeaders();
  RestoreState();
  return ret_value;
}

class wxHTTPStream : public wxSocketInputStream
{
public:
  wxHTTP *m_http;
  size_t m_httpsize;
  unsigned long m_read_bytes;

  wxHTTPStream(wxHTTP *http) : wxSocketInputStream(*http), m_http(http) {}
  size_t GetSize() const { return m_httpsize; }
  virtual ~wxHTTPStream(void) { m_http->Abort(); }

protected:
  size_t OnSysRead(void *buffer, size_t bufsize);

    DECLARE_NO_COPY_CLASS(wxHTTPStream)
};

size_t wxHTTPStream::OnSysRead(void *buffer, size_t bufsize)
{
    if (m_httpsize > 0 && m_read_bytes >= m_httpsize)
    {
        m_lasterror = wxSTREAM_EOF;
        return 0;
    }

    size_t ret = wxSocketInputStream::OnSysRead(buffer, bufsize);
    m_read_bytes += ret;

    return ret;
}

bool wxHTTP::Abort(void)
{
  return wxSocketClient::Close();
}

wxInputStream *wxHTTP::GetInputStream(const wxString& path)
{
  wxHTTPStream *inp_stream;

  wxString new_path;

  m_perr = wxPROTO_CONNERR;
  if (!m_addr)
    return NULL;

  // We set m_connected back to false so wxSocketBase will know what to do.
#ifdef __WXMAC__
        wxSocketClient::Connect(*m_addr , false );
        wxSocketClient::WaitOnConnect(10);

    if (!wxSocketClient::IsConnected())
        return NULL;
#else
  if (!wxProtocol::Connect(*m_addr))
    return NULL;
#endif

  if (!BuildRequest(path, m_post_buf.IsEmpty() ? wxHTTP_GET : wxHTTP_POST))
    return NULL;

  inp_stream = new wxHTTPStream(this);

  if (!GetHeader(wxT("Content-Length")).IsEmpty())
    inp_stream->m_httpsize = wxAtoi(WXSTRINGCAST GetHeader(wxT("Content-Length")));
  else
    inp_stream->m_httpsize = (size_t)-1;

  inp_stream->m_read_bytes = 0;

  Notify(false);
  SetFlags(wxSOCKET_BLOCK | wxSOCKET_WAITALL);

  return inp_stream;
}

#endif // wxUSE_PROTOCOL_HTTP

