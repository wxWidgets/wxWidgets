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
#include <memory.h>
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
IMPLEMENT_PROTOCOL(wxFTP, "ftp", "ftp", TRUE)
#endif

////////////////////////////////////////////////////////////////
////// wxFTP constructor and destructor ////////////////////////
////////////////////////////////////////////////////////////////

wxFTP::wxFTP()
  : wxProtocol()
{
  char tmp[256];

  m_lastError = wxPROTO_NOERR;
  m_streaming = FALSE;

  m_user = "anonymous";
  wxGetUserName(tmp, 256);
  m_passwd.sprintf("%s@",tmp);
  wxGetHostName(tmp, 256);
  m_passwd += tmp;

  SetNotify(0);
}

wxFTP::~wxFTP()
{
  SendCommand("QUIT", '2');
}

////////////////////////////////////////////////////////////////
////// wxFTP connect and login methods /////////////////////////
////////////////////////////////////////////////////////////////
bool wxFTP::Connect(wxSockAddress& addr)
{
  if (!m_handler) {
    m_lastError = wxPROTO_NOHNDLR;
    return FALSE;
  }

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

  command.sprintf("USER %s", (const char *)m_user);
  if (!SendCommand(command, '3')) {
    Close();
    return FALSE;
  }

  command.sprintf("PASS %s", (const char *)m_passwd);
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
  addr.Service("ftp");

  return Connect(addr);
}

bool wxFTP::Close()
{
  if (m_streaming) {
    m_lastError = wxPROTO_STREAMING;
    return FALSE;
  }
  if (m_connected)
    SendCommand(wxString("QUIT"), '2');
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
  tmp_str = command + "\r\n";
  if (Write((char *)tmp_str.GetData(), tmp_str.Length()).Error()) {
    m_lastError = wxPROTO_NETERR;
    return FALSE;
  }
  return GetResult(exp_ret);
}

bool wxFTP::GetResult(char exp)
{
  if ((m_lastError = GetLine(this, m_lastResult)))
    return FALSE;
  if (m_lastResult[0UL] != exp) {
    m_lastError = wxPROTO_PROTERR;
    return FALSE;
  }

  if (m_lastResult[3UL] == '-') {
    wxString key = m_lastResult.Left((size_t)3);

    key += ' ';

    while (m_lastResult.Index(key) != 0) {
      if ((m_lastError = GetLine(this, m_lastResult)))
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

  str.Prepend("CWD ");
  return SendCommand(str, '2');
}

bool wxFTP::MkDir(const wxString& dir)
{
  wxString str = dir;
  str.Prepend("MKD ");
  return SendCommand(str, '2');
}

bool wxFTP::RmDir(const wxString& dir)
{
  wxString str = dir;
  
  str.Prepend("PWD ");
  return SendCommand(str, '2');
}

wxString wxFTP::Pwd()
{
  int beg, end;

  if (!SendCommand("PWD", '2'))
    return wxString((char *)NULL);
  
  beg = m_lastResult.Find('\"',FALSE);
  end = m_lastResult.Find('\"',TRUE);

  return wxString(beg+1, end);
}

bool wxFTP::Rename(const wxString& src, const wxString& dst)
{
  wxString str;

  str = "RNFR " + src;
  if (!SendCommand(str, '3'))
    return FALSE;

  str = "RNTO " + dst;
  return SendCommand(str, '2');
}

bool wxFTP::RmFile(const wxString& path)
{
  wxString str;

  str = "DELE ";
  str += path;
  return SendCommand(str, '2');
}

////////////////////////////////////////////////////////////////
////// wxFTP download*upload ///////////////////////////////////
////////////////////////////////////////////////////////////////

class wxInputFTPStream : public wxSocketInputStream {
public:
  wxFTP *m_ftp;

  wxInputFTPStream(wxFTP *ftp_clt, wxSocketBase *sock)
    : wxSocketInputStream(*sock), m_ftp(ftp_clt) {}
  virtual ~wxInputFTPStream(void)
  { 
     if (Eof())
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
     if (Bad())
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
  struct sockaddr sin;
  int a[6];
  wxString straddr;
  int addr_pos;

  if (!SendCommand("PASV", '2'))
    return NULL;

  sin.sa_family = AF_INET;
  addr_pos = m_lastResult.Find('(');
  if (addr_pos == -1) {
    m_lastError = wxPROTO_PROTERR;
    return NULL;
  }
  straddr = m_lastResult(addr_pos+1, m_lastResult.Length());
  sscanf((const char *)straddr,"%d,%d,%d,%d,%d,%d",&a[2],&a[3],&a[4],&a[5],&a[0],&a[1]);
  sin.sa_data[2] = (char)a[2];
  sin.sa_data[3] = (char)a[3];
  sin.sa_data[4] = (char)a[4];
  sin.sa_data[5] = (char)a[5];
  sin.sa_data[0] = (char)a[0];
  sin.sa_data[1] = (char)a[1];

  addr.Disassemble(&sin, sizeof(sin));

  client = m_handler->CreateClient();
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
  if (!SendCommand("ABOR", '4'))
    return FALSE;
  return GetResult('2');
}

wxInputStream *wxFTP::GetInputStream(const wxString& path)
{
  wxString tmp_str;

  if (!SendCommand("TYPE I", '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  if (!sock) {
    m_lastError = wxPROTO_NETERR;
    return NULL;
  }

  tmp_str = "RETR " + path;
  if (!SendCommand(tmp_str, '1'))
    return NULL;

  return new wxInputFTPStream(this, sock);
}

wxOutputStream *wxFTP::GetOutputStream(const wxString& path)
{
  wxString tmp_str;

  if (!SendCommand("TYPE I", '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  tmp_str = "STOR " + path;
  if (!SendCommand(tmp_str, '1'))
    return FALSE;

  return new wxOutputFTPStream(this, sock);
}

wxList *wxFTP::GetList(const wxString& wildcard)
{
  wxList *file_list = new wxList;
  wxSocketBase *sock = GetPort();
  wxString tmp_str = "NLST";

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
