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

#ifdef __GNUG__
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
#include "wx/string.h"
#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "wx/protocol/protocol.h"
#include "wx/url.h"
#include "wx/protocol/http.h"
#include "wx/sckstrm.h"

IMPLEMENT_DYNAMIC_CLASS(wxHTTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxHTTP, wxT("http"), wxT("80"), TRUE)

#define HTTP_BSIZE 2048

wxHTTP::wxHTTP()
  : wxProtocol(),
    m_headers(wxKEY_STRING)
{
  m_addr = NULL;
  m_read = FALSE;
  m_proxy_mode = FALSE;

  SetNotify(wxSOCKET_LOST_FLAG);
}

wxHTTP::~wxHTTP()
{
    ClearHeaders();

    delete m_addr;
}

void wxHTTP::ClearHeaders()
{
  // wxString isn't a wxObject
  wxNode *node = m_headers.GetFirst();
  wxString *string;

  while (node) {
    string = (wxString *)node->GetData();
    delete string;
    node = node->GetNext();
  }

  m_headers.Clear();
}

wxString wxHTTP::GetContentType()
{
  return GetHeader(wxT("Content-Type"));
}

void wxHTTP::SetProxyMode(bool on)
{
  m_proxy_mode = on;
}

void wxHTTP::SetHeader(const wxString& header, const wxString& h_data)
{
  if (m_read) {
    ClearHeaders();
    m_read = FALSE;
  }

  wxNode *node = m_headers.Find(header);

  if (!node)
    m_headers.Append(header.Upper(), (wxObject *)(new wxString(h_data)));
  else {
    wxString *str = (wxString *)node->GetData();
    (*str) = h_data;
  }
}

wxString wxHTTP::GetHeader(const wxString& header)
{
  wxNode *node;
  wxString upper_header;

  upper_header = header.Upper();

  node = m_headers.Find(upper_header);
  if (!node)
    return wxEmptyString;

  return *((wxString *)node->GetData());
}

void wxHTTP::SendHeaders()
{
  wxNode *head = m_headers.GetFirst();

  while (head)
  {
    wxString *str = (wxString *)head->GetData();

    wxString buf;
    buf.Printf(wxT("%s: %s\r\n"), head->GetKeyString(), str->GetData());

    const wxWX2MBbuf cbuf = buf.mb_str();
    Write(cbuf, strlen(cbuf));

    head = head->GetNext();
  }
}

bool wxHTTP::ParseHeaders()
{
  wxString line;
  wxStringTokenizer tokenzr;

  ClearHeaders();
  m_read = TRUE;

#if defined(__VISAGECPP__)
// VA just can't stand while(1)
    bool bOs2var = TRUE;
    while(bOs2var) {
#else
    while (1) {
#endif
    m_perr = GetLine(this, line);
    if (m_perr != wxPROTO_NOERR)
      return FALSE;

    if (line.Length() == 0)
      break;

    wxString left_str = line.BeforeFirst(':');
    wxString *str = new wxString(line.AfterFirst(':').Strip(wxString::both));
    left_str.MakeUpper();

    m_headers.Append(left_str, (wxObject *) str);
  }
  return TRUE;
}

bool wxHTTP::Connect(const wxString& host)
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
    return FALSE;
  }

  if (!addr->Service(wxT("http")))
    addr->Service(80);

  SetHeader(wxT("Host"), host);

  return TRUE;
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

  return TRUE;
}

bool wxHTTP::BuildRequest(const wxString& path, wxHTTP_Req req)
{
  const wxChar *request;

  switch (req) {
  case wxHTTP_GET:
    request = wxT("GET");
    break;
  default:
    return FALSE;
  }

  // If there is no User-Agent defined, define it.
  if (GetHeader(wxT("User-Agent")).IsNull())
    SetHeader(wxT("User-Agent"), wxT("wxWindows 2.x"));

  SaveState();
  SetFlags(wxSOCKET_NONE);
  Notify(FALSE);

  wxString buf;
  buf.Printf(wxT("%s %s HTTP/1.0\r\n"), request, path.c_str());
  const wxWX2MBbuf pathbuf = wxConvLocal.cWX2MB(buf);
  Write(pathbuf, strlen(wxMBSTRINGCAST pathbuf));
  SendHeaders();
  Write("\r\n", 2);

  wxString tmp_str;
  m_perr = GetLine(this, tmp_str);
  if (m_perr != wxPROTO_NOERR) {
    RestoreState();
    return FALSE;
  }

  if (!tmp_str.Contains(wxT("HTTP/"))) {
    // TODO: support HTTP v0.9 which can have no header.
    // FIXME: tmp_str is not put back in the in-queue of the socket.
    SetHeader(wxT("Content-Length"), wxT("-1"));
    SetHeader(wxT("Content-Type"), wxT("none/none"));
    RestoreState();
    return TRUE;
  }

  wxStringTokenizer token(tmp_str,wxT(' '));
  wxString tmp_str2;
  bool ret_value;

  token.NextToken();
  tmp_str2 = token.NextToken();

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
    return FALSE;
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

  // We set m_connected back to FALSE so wxSocketBase will know what to do.
#ifdef __WXMAC__
        wxSocketClient::Connect(*m_addr , FALSE );
        wxSocketClient::WaitOnConnect(10);

    if (!wxSocketClient::IsConnected())
        return NULL;
#else
  if (!wxProtocol::Connect(*m_addr))
    return NULL;
#endif

  if (!BuildRequest(path, wxHTTP_GET))
    return NULL;

  inp_stream = new wxHTTPStream(this);

  if (!GetHeader(wxT("Content-Length")).IsEmpty())
    inp_stream->m_httpsize = wxAtoi(WXSTRINGCAST GetHeader(wxT("Content-Length")));
  else
    inp_stream->m_httpsize = (size_t)-1;

  inp_stream->m_read_bytes = 0;

  Notify(FALSE);
  SetFlags(wxSOCKET_BLOCK | wxSOCKET_WAITALL);

  return inp_stream;
}

#endif // wxUSE_PROTOCOL_HTTP

