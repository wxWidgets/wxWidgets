/////////////////////////////////////////////////////////////////////////////
// Name:        ftp.cpp
// Purpose:     FTP protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     07/07/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "ftp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#ifndef __MWERKS__
#include <memory.h>
#endif
#if defined(__WXMAC__)
#include "/wx/mac/macsock.h"
#endif

#include <stdlib.h>
#include "wx/string.h"
#include "wx/utils.h"
// #include "wx/data.h"
#define WXSOCK_INTERNAL
#include "wx/sckaddr.h"
#undef WXSOCK_INTERNAL
#include "wx/socket.h"
#include "wx/url.h"
#include "wx/sckstrm.h"
#include "wx/protocol/protocol.h"
#include "wx/protocol/ftp.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#define FTP_BSIZE 1024

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxFTP, _T("ftp"), _T("ftp"), TRUE)
#endif

////////////////////////////////////////////////////////////////
////// wxFTP constructor and destructor ////////////////////////
////////////////////////////////////////////////////////////////

wxFTP::wxFTP()
  : wxProtocol()
{
  m_lastError = wxPROTO_NOERR;
  m_streaming = FALSE;

  m_user = _T("anonymous");
  m_passwd = wxGetUserId();
  m_passwd += '@';
  m_passwd += wxGetHostName();

  SetNotify(0);
  SetFlags(NONE);
}

wxFTP::~wxFTP()
{
  SendCommand("QUIT", '2');
}

////////////////////////////////////////////////////////////////
////// wxFTP connect and login methods /////////////////////////
////////////////////////////////////////////////////////////////
bool wxFTP::Connect(wxSockAddress& addr, bool WXUNUSED(wait))
{
  if (!wxProtocol::Connect(addr)) {
    m_lastError = wxPROTO_NETERR;
    return FALSE;
  }

  if (!m_user || !m_passwd) {
    m_lastError = wxPROTO_CONNERR;
    return FALSE;
  }

  wxString command;

  if (!GetResult('2')) {
    Close();
    return FALSE;
  }

  command.sprintf(_T("USER %s"), (const wxChar *)m_user);
  if (!SendCommand(command, '3')) {
    Close();
    return FALSE;
  }

  command.sprintf(_T("PASS %s"), (const wxChar *)m_passwd);
  if (!SendCommand(command, '2')) {
    Close();
    return FALSE;
  }

  return TRUE;
}

bool wxFTP::Connect(const wxString& host)
{
  wxIPV4address addr;
  wxString my_host = host;

  addr.Hostname(my_host);
  addr.Service(_T("ftp"));

  return Connect(addr);
}

bool wxFTP::Close()
{
  if (m_streaming) {
    m_lastError = wxPROTO_STREAMING;
    return FALSE;
  }
  if (m_connected)
    SendCommand(wxString(_T("QUIT")), '2');
  return wxSocketClient::Close();
}

////////////////////////////////////////////////////////////////
////// wxFTP low-level methods /////////////////////////////////
////////////////////////////////////////////////////////////////
bool wxFTP::SendCommand(const wxString& command, char exp_ret)
{
  wxString tmp_str;

  if (m_streaming) {
    m_lastError = wxPROTO_STREAMING;
    return FALSE;
  }
  tmp_str = command + _T("\r\n");
  const wxWX2MBbuf tmp_buf = tmp_str.mb_str();
  if (Write(MBSTRINGCAST tmp_buf, strlen(tmp_buf)).Error()) {
    m_lastError = wxPROTO_NETERR;
    return FALSE;
  }
  return GetResult(exp_ret);
}

bool wxFTP::GetResult(char exp)
{
  m_lastError = GetLine(this, m_lastResult);
  if ( m_lastError )
    return FALSE;
  if (m_lastResult.GetChar(0) != exp) {
    m_lastError = wxPROTO_PROTERR;
    return FALSE;
  }

  if (m_lastResult.GetChar(3) == '-') {
    wxString key = m_lastResult.Left((size_t)3);

    key += _T(' ');

    while (m_lastResult.Index(key) != 0) {
      m_lastError = GetLine(this, m_lastResult);
      if ( m_lastError )
        return FALSE;
    }
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////
////// wxFTP low-level methods /////////////////////////////////
////////////////////////////////////////////////////////////////
bool wxFTP::ChDir(const wxString& dir)
{
  wxString str = dir;

  str.Prepend(_T("CWD "));
  return SendCommand(str, '2');
}

bool wxFTP::MkDir(const wxString& dir)
{
  wxString str = dir;
  str.Prepend(_T("MKD "));
  return SendCommand(str, '2');
}

bool wxFTP::RmDir(const wxString& dir)
{
  wxString str = dir;
  
  str.Prepend(_T("PWD "));
  return SendCommand(str, '2');
}

wxString wxFTP::Pwd()
{
  int beg, end;

  if (!SendCommand(_T("PWD"), '2'))
    return wxString((char *)NULL);
  
  beg = m_lastResult.Find(_T('\"'),FALSE);
  end = m_lastResult.Find(_T('\"'),TRUE);

  return wxString(beg+1, end);
}

bool wxFTP::Rename(const wxString& src, const wxString& dst)
{
  wxString str;

  str = _T("RNFR ") + src;
  if (!SendCommand(str, '3'))
    return FALSE;

  str = _T("RNTO ") + dst;
  return SendCommand(str, '2');
}

bool wxFTP::RmFile(const wxString& path)
{
  wxString str;

  str = _T("DELE ");
  str += path;
  return SendCommand(str, '2');
}

////////////////////////////////////////////////////////////////
////// wxFTP download*upload ///////////////////////////////////
////////////////////////////////////////////////////////////////

class wxInputFTPStream : public wxSocketInputStream {
public:
  wxFTP *m_ftp;
  size_t m_ftpsize;

  wxInputFTPStream(wxFTP *ftp_clt, wxSocketBase *sock)
    : wxSocketInputStream(*sock), m_ftp(ftp_clt) {}
  size_t GetSize() const { return m_ftpsize; }
  virtual ~wxInputFTPStream(void)
  { 
     if (LastError() == wxStream_NOERROR)
       m_ftp->GetResult('2');
     else
       m_ftp->Abort();
     delete m_i_socket;
  }
};

class wxOutputFTPStream : public wxSocketOutputStream {
public:
  wxFTP *m_ftp;

  wxOutputFTPStream(wxFTP *ftp_clt, wxSocketBase *sock)
    : wxSocketOutputStream(*sock), m_ftp(ftp_clt) {}
  virtual ~wxOutputFTPStream(void)
  {
     if (LastError() != wxStream_NOERROR)
       m_ftp->GetResult('2');
     else
       m_ftp->Abort();
     delete m_o_socket;
  }
};

wxSocketClient *wxFTP::GetPort()
{
  wxIPV4address addr;
  wxSocketClient *client;
  int a[6];
  wxString straddr;
  int addr_pos;
  wxUint16 port;
  wxUint32 hostaddr;

  if (!SendCommand(_T("PASV"), '2'))
    return NULL;

  addr_pos = m_lastResult.Find(_T('('));
  if (addr_pos == -1) {
    m_lastError = wxPROTO_PROTERR;
    return NULL;
  }
  straddr = m_lastResult(addr_pos+1, m_lastResult.Length());
  wxSscanf((const wxChar *)straddr,_T("%d,%d,%d,%d,%d,%d"),&a[2],&a[3],&a[4],&a[5],&a[0],&a[1]);

  hostaddr = (wxUint16)a[5] << 24 | (wxUint16)a[4] << 16 |
             (wxUint16)a[3] << 8 | a[2]; 
  addr.Hostname(hostaddr);

  port = (wxUint16)a[0] << 8 | a[1];
  addr.Service(port);

  client = new wxSocketClient();
  if (!client->Connect(addr)) {
    delete client;
    return NULL;
  }
  client->Notify(FALSE);

  return client;
}

bool wxFTP::Abort(void)
{
  m_streaming = FALSE;
  if (!SendCommand(_T("ABOR"), '4'))
    return FALSE;
  return GetResult('2');
}

wxInputStream *wxFTP::GetInputStream(const wxString& path)
{
  wxString tmp_str;
  int pos_size;
  wxInputFTPStream *in_stream;

  if (!SendCommand(_T("TYPE I"), '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  if (!sock) {
    m_lastError = wxPROTO_NETERR;
    return NULL;
  }

  tmp_str = _T("RETR ") + wxURL::ConvertFromURI(path);
  if (!SendCommand(tmp_str, '1'))
    return NULL;

  in_stream = new wxInputFTPStream(this, sock);

  pos_size = m_lastResult.Index(_T('('));
  if (pos_size != wxNOT_FOUND) {
    wxString str_size = m_lastResult(pos_size+1, m_lastResult.Index(_T(')'))-1);

    in_stream->m_ftpsize = wxAtoi(WXSTRINGCAST str_size);
  }
  sock->SetFlags(WAITALL);

  return in_stream;
}

wxOutputStream *wxFTP::GetOutputStream(const wxString& path)
{
  wxString tmp_str;

  if (!SendCommand(_T("TYPE I"), '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  tmp_str = _T("STOR ") + path;
  if (!SendCommand(tmp_str, '1'))
    return FALSE;

  return new wxOutputFTPStream(this, sock);
}

wxList *wxFTP::GetList(const wxString& wildcard)
{
  wxList *file_list = new wxList;
  wxSocketBase *sock = GetPort();
  wxString tmp_str = _T("NLST");

  if (!wildcard.IsNull())
    tmp_str += wildcard;

  if (!SendCommand(tmp_str, '1')) {
    delete sock;
    delete file_list;
    return NULL;
  }

  while (GetLine(sock, tmp_str) == wxPROTO_NOERR) {
    file_list->Append((wxObject *)(new wxString(tmp_str)));
  }

  if (!GetResult('2')) {
    delete sock;
    file_list->DeleteContents(TRUE);
    delete file_list;
    return NULL;
  }

  return file_list;
}
#endif
  // wxUSE_SOCKETS
