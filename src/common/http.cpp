/////////////////////////////////////////////////////////////////////////////
// Name:        http.cpp
// Purpose:     HTTP protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     August 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "http.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include <stdio.h>
#include <stdlib.h>
#include "wx/string.h"
#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "wx/protocol/protocol.h"
#include "wx/url.h"
#include "wx/protocol/http.h"
#include "wx/sckstrm.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxHTTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxHTTP, wxT("http"), wxT("80"), TRUE)
#endif

#define HTTP_BSIZE 2048

wxHTTP::wxHTTP()
  : wxProtocol(),
    m_headers(wxKEY_STRING)
{
  m_addr = NULL;
  m_read = FALSE;
  m_proxy_mode = FALSE;

  SetNotify(GSOCK_LOST_FLAG);
}

wxHTTP::~wxHTTP()
{
  // wxString isn't a wxObject
  wxNode *node = m_headers.First();
  wxString *string;

  while (node) {
    string = (wxString *)node->Data();
    delete string;
    node = node->Next();
  }
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
    m_headers.Clear();
    m_read = FALSE;
  }

  wxNode *node = m_headers.Find(header);

  if (!node)
    m_headers.Append(header, (wxObject *)(new wxString(h_data)));
  else {
    wxString *str = (wxString *)node->Data();
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

  return *((wxString *)node->Data());
}

void wxHTTP::SendHeaders()
{
  wxNode *head = m_headers.First();

  while (head)
  {
    wxString *str = (wxString *)head->Data();

    wxString buf;
    buf.Printf(wxT("%s: %s\n\r"), head->GetKeyString(), str->GetData());

    const wxWX2MBbuf cbuf = buf.mb_str();
    Write(cbuf, strlen(cbuf));

    head = head->Next();
  }
}

bool wxHTTP::ParseHeaders()
{
  wxString line;
  wxStringTokenizer tokenzr;

  m_headers.Clear();
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

    tokenzr.SetString(line, " :\t\n\r");
    if (!tokenzr.HasMoreTokens())
      return FALSE;

    wxString left_str = tokenzr.GetNextToken();
    wxString *str = new wxString(tokenzr.GetNextToken());

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

  return TRUE;
}

bool wxHTTP::Connect(wxSockAddress& addr, bool WXUNUSED(wait))
{
  if (m_addr) {
    delete m_addr;
    m_addr = NULL;
    Close();
  }

  m_addr = (wxSockAddress *) addr.Clone();
  return TRUE;
}

bool wxHTTP::BuildRequest(const wxString& path, wxHTTP_Req req)
{
  wxChar *tmp_buf;
  wxChar buf[200]; // 200 is arbitrary.
  wxString tmp_str = path;

  // If there is no User-Agent defined, define it.
  if (GetHeader(wxT("User-Agent")).IsNull())
    SetHeader(wxT("User-Agent"), wxT("wxWindows 2.x"));

  switch (req) {
  case wxHTTP_GET:
    tmp_buf = wxT("GET");
    break;
  default:
    return FALSE;
  }

  SaveState();
  SetFlags(NONE);
  Notify(FALSE);

  wxSprintf(buf, wxT("%s %s HTTP/1.0\n\r"), tmp_buf, tmp_str.GetData());
  const wxWX2MBbuf pathbuf = wxConvLibc.cWX2MB(buf);
  Write(pathbuf, strlen(wxMBSTRINGCAST pathbuf));
  SendHeaders();
  Write("\n\r", 2);

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

  switch (tmp_str2[(unsigned int) 0]) {
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

class wxHTTPStream : public wxSocketInputStream {
public:
  wxHTTP *m_http;
  size_t m_httpsize;
  unsigned long m_read_bytes;

  wxHTTPStream(wxHTTP *http) : wxSocketInputStream(*http), m_http(http) {}
  size_t GetSize() const { return m_httpsize; }
  virtual ~wxHTTPStream(void) { m_http->Abort(); }

protected:
  size_t OnSysRead(void *buffer, size_t bufsize);
};

size_t wxHTTPStream::OnSysRead(void *buffer, size_t bufsize)
{
  size_t ret;

  if (m_httpsize > 0 && m_read_bytes >= m_httpsize)
    return 0;

  ret = wxSocketInputStream::OnSysRead(buffer, bufsize);
  m_read_bytes += ret;

  return ret;
}

bool wxHTTP::Abort(void)
{
  bool ret;

  ret = wxSocketClient::Close();

  return ret;
}

wxInputStream *wxHTTP::GetInputStream(const wxString& path)
{
  wxHTTPStream *inp_stream = new wxHTTPStream(this);
  wxString new_path;

  m_perr = wxPROTO_CONNERR;
  if (!m_addr)
    return NULL;

  // We set m_connected back to FALSE so wxSocketBase will know what to do.
  if (!wxProtocol::Connect(*m_addr))
    return NULL;

  if (!BuildRequest(path, wxHTTP_GET))
    return NULL;

  if (!GetHeader(wxT("Content-Length")).IsEmpty())
    inp_stream->m_httpsize = wxAtoi(WXSTRINGCAST GetHeader(wxT("Content-Length")));
  else
    inp_stream->m_httpsize = (size_t)-1;

  inp_stream->m_read_bytes = 0;

  Notify(FALSE);
  SetFlags(SPEED | WAITALL);

  return inp_stream;
}

#endif
   // wxUSE_SOCKETS
