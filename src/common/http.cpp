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

#ifndef WX_PRECOMP
#endif

#include <stdio.h>
#include <stdlib.h>
#include "wx/string.h"
#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "wx/protocol/protocol.h"
#include "wx/protocol/http.h"
#include "wx/sckstrm.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxHTTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxHTTP, "http", "80", TRUE)
#endif

#define HTTP_BSIZE 2048

wxHTTP::wxHTTP()
  : wxProtocol(),
    m_headers(wxKEY_STRING)
{
  m_addr = NULL;
  m_read = FALSE;

  SetNotify(REQ_LOST);
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
  return GetHeader("Content-Type");
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
  wxNode *node = m_headers.Find(header);
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
    buf.Printf("%s: %s\n\r", head->GetKeyString(), str->GetData());

    Write(buf, buf.Len());

    head = head->Next();
  }
}

bool wxHTTP::ParseHeaders()
{
  wxString line;

  m_headers.Clear();
  m_read = TRUE;

  while (1) {
    m_error = GetLine(this, line);
    if (m_error != wxPROTO_NOERR)
      return FALSE;

    if (line.Length() == 0)
      break;

    printf("Header: %s\n", WXSTRINGCAST line);
    int pos = line.Find(':');
    if (pos == -1)
      return FALSE;

    wxString left_str = line(0, pos);
    wxString right_str = line(pos+1, line.Length());

    right_str = right_str.Strip(wxString::leading);

    wxString *str = new wxString(right_str);

    m_headers.Append(left_str, (wxObject *) str);
  }
  return TRUE;
}

bool wxHTTP::Connect(const wxString& host)
{
  wxIPV4address *addr;

  if (m_connected) {
    delete m_addr;
    m_addr = NULL;
    Close();
  }

  m_addr = addr = new wxIPV4address();

  if (!addr->Hostname(host)) {
    delete m_addr;
    m_addr = NULL;
    m_error = wxPROTO_NETERR;
    return FALSE;
  }

  if (!addr->Service("http"))
    addr->Service(80);

  return TRUE;
}

bool wxHTTP::Connect(wxSockAddress& addr, bool WXUNUSED(wait))
{
  struct sockaddr *raw_addr;
  size_t len;

  m_addr = (wxSockAddress *)(addr.GetClassInfo()->CreateObject());

  addr.Build(raw_addr, len);
  m_addr->Disassemble(raw_addr, len);

  return TRUE;
}

bool wxHTTP::BuildRequest(const wxString& path, wxHTTP_Req req)
{
  char *tmp_buf;
  char buf[HTTP_BSIZE];

  switch (req) {
  case wxHTTP_GET:
    tmp_buf = "GET";
    break;
  default:
    return FALSE;
  }

  SaveState();
  Notify(FALSE);
  SetFlags(WAITALL);

  sprintf(buf, "%s %s HTTP/1.0\n\r", tmp_buf, (const char *)path);
  Write(buf, strlen(buf));
  SendHeaders();
  sprintf(buf, "\n\r");
  Write(buf, strlen(buf));

  wxString tmp_str;

  m_error = GetLine(this, tmp_str);
  if (m_error != wxPROTO_NOERR) {
    RestoreState();
    return FALSE;
  }

  if (!tmp_str.Contains("HTTP/")) {
    // TODO: support HTTP v0.9 which can have no header.
    SetHeader("Content-Length", "-1");
    SetHeader("Content-Type", "none/none");
    RestoreState();
    return TRUE;
  }

  wxStringTokenizer token(tmp_str,' ');
  wxString tmp_str2;
  bool ret_value;

  token.NextToken();
  tmp_str2 = token.NextToken();

  switch (atoi(tmp_str2)) {
  case 200:
    break;
  default:
    m_error = wxPROTO_NOFILE;
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

  wxHTTPStream(wxHTTP *http) : wxSocketInputStream(*http), m_http(http) {}
  size_t StreamSize() const { return m_httpsize; }
  virtual ~wxHTTPStream(void) { m_http->Abort(); }
};

bool wxHTTP::Abort(void)
{
  return wxSocketClient::Close();
}

wxInputStream *wxHTTP::GetInputStream(const wxString& path)
{
  wxHTTPStream *inp_stream = new wxHTTPStream(this);

  if (!m_addr || m_connected) {
    m_error = wxPROTO_CONNERR;
    return NULL;
  }

  if (!wxProtocol::Connect(*m_addr))
    return NULL;

  if (!BuildRequest(path, wxHTTP_GET))
    return NULL;

  printf("Len = %s\n", WXSTRINGCAST GetHeader("Content-Length"));
  if (!GetHeader("Content-Length").IsEmpty())
    inp_stream->m_httpsize = atoi(WXSTRINGCAST GetHeader("Content-Length"));

  return inp_stream;
}
