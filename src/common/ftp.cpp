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

#include "wx/setup.h"

#if wxUSE_SOCKETS && wxUSE_STREAMS

#ifndef __MWERKS__
#include <memory.h>
#endif
#if defined(__WXMAC__)
#include "/wx/mac/macsock.h"
#endif

#include <stdlib.h>
#include "wx/string.h"
#include "wx/utils.h"
#include "wx/sckaddr.h"
#include "wx/socket.h"
#include "wx/url.h"
#include "wx/sckstrm.h"
#include "wx/protocol/protocol.h"
#include "wx/protocol/ftp.h"
#include "wx/log.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// the length of FTP status code (3 digits)
static const size_t LEN_CODE = 3;

#define FTP_BSIZE 1024

IMPLEMENT_DYNAMIC_CLASS(wxFTP, wxProtocol)
IMPLEMENT_PROTOCOL(wxFTP, wxT("ftp"), wxT("ftp"), TRUE)

////////////////////////////////////////////////////////////////
////// wxFTP constructor and destructor ////////////////////////
////////////////////////////////////////////////////////////////

wxFTP::wxFTP()
     : wxProtocol()
{
    m_lastError = wxPROTO_NOERR;
    m_streaming = FALSE;

    m_user = wxT("anonymous");
    m_passwd << wxGetUserId() << wxT('@') << wxGetFullHostName();

    SetNotify(0);
    SetFlags(wxSOCKET_NONE);
}

wxFTP::~wxFTP()
{
    if ( m_streaming )
    {
        (void)Abort();
    }

    Close();
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

  command.sprintf(wxT("USER %s"), (const wxChar *)m_user);
  if (!SendCommand(command, '3')) {
    Close();
    return FALSE;
  }

  command.sprintf(wxT("PASS %s"), (const wxChar *)m_passwd);
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
  addr.Service(wxT("ftp"));

  return Connect(addr);
}

bool wxFTP::Close()
{
    if ( m_streaming )
    {
        m_lastError = wxPROTO_STREAMING;
        return FALSE;
    }

    if ( IsConnected() )
        SendCommand(wxT("QUIT"), '2');

    return wxSocketClient::Close();
}

// ============================================================================
// low level methods
// ============================================================================

// ----------------------------------------------------------------------------
// Send command to FTP server
// ----------------------------------------------------------------------------

bool wxFTP::SendCommand(const wxString& command, char exp_ret)
{
    wxString tmp_str;

    if (m_streaming)
    {
        m_lastError = wxPROTO_STREAMING;
        return FALSE;
    }

    tmp_str = command + wxT("\r\n");
    const wxWX2MBbuf tmp_buf = tmp_str.mb_str();
    if ( Write(wxMBSTRINGCAST tmp_buf, strlen(tmp_buf)).Error())
    {
        m_lastError = wxPROTO_NETERR;
        return FALSE;
    }

    wxLogTrace(_T("ftp"), _T("==> %s"), command.c_str());

    return GetResult(exp_ret);
}

// ----------------------------------------------------------------------------
// Recieve servers reply
// ----------------------------------------------------------------------------

bool wxFTP::GetResult(char exp)
{
    wxString code;

    // we handle multiline replies here according to RFC 959: it says that a
    // reply may either be on 1 line of the form "xyz ..." or on several lines
    // in whuch case it looks like
    //      xyz-...
    //      ...
    //      xyz ...
    // and the intermeidate lines may start with xyz or not
    bool badReply = FALSE;
    bool firstLine = TRUE;
    bool endOfReply = FALSE;
    while ( !endOfReply && !badReply )
    {
        m_lastError = ReadLine(m_lastResult);
        if ( m_lastError )
            return FALSE;

        // unless this is an intermediate line of a multiline reply, it must
        // contain the code in the beginning and '-' or ' ' following it
        if ( m_lastResult.Len() < LEN_CODE + 1 )
        {
            if ( firstLine )
            {
                badReply = TRUE;
            }
            else
            {
                wxLogTrace(_T("ftp"), _T("<== %s %s"),
                           code.c_str(), m_lastResult.c_str());
            }
        }
        else // line has at least 4 chars
        {
            // this is the char which tells us what we're dealing with
            wxChar chMarker = m_lastResult.GetChar(LEN_CODE);

            if ( firstLine )
            {
                code = wxString(m_lastResult, LEN_CODE);
                wxLogTrace(_T("ftp"), _T("<== %s %s"),
                           code.c_str(), m_lastResult.c_str() + LEN_CODE + 1);

                switch ( chMarker )
                {
                    case _T(' '):
                        endOfReply = TRUE;
                        break;

                    case _T('-'):
                        firstLine = FALSE;
                        break;

                    default:
                        // unexpected
                        badReply = TRUE;
                }
            }
            else // subsequent line of multiline reply
            {
                if ( wxStrncmp(m_lastResult, code, LEN_CODE) == 0 )
                {
                    if ( chMarker == _T(' ') )
                    {
                        endOfReply = TRUE;
                    }

                    wxLogTrace(_T("ftp"), _T("<== %s %s"),
                               code.c_str(), m_lastResult.c_str() + LEN_CODE + 1);
                }
                else
                {
                    // just part of reply
                    wxLogTrace(_T("ftp"), _T("<== %s %s"),
                               code.c_str(), m_lastResult.c_str());
                }
            }
        }
    }

    if ( badReply )
    {
        wxLogDebug(_T("Broken FTP server: '%s' is not a valid reply."),
                   m_lastResult.c_str());

        m_lastError = wxPROTO_PROTERR;

        return FALSE;
    }

    if ( code.GetChar(0) != exp )
    {
        m_lastError = wxPROTO_PROTERR;

        return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////
////// wxFTP low-level methods /////////////////////////////////
////////////////////////////////////////////////////////////////
bool wxFTP::ChDir(const wxString& dir)
{
  wxString str = dir;

  str.Prepend(wxT("CWD "));
  return SendCommand(str, '2');
}

bool wxFTP::MkDir(const wxString& dir)
{
  wxString str = dir;
  str.Prepend(wxT("MKD "));
  return SendCommand(str, '2');
}

bool wxFTP::RmDir(const wxString& dir)
{
  wxString str = dir;
  
  str.Prepend(wxT("PWD "));
  return SendCommand(str, '2');
}

wxString wxFTP::Pwd()
{
    wxString path;

    if ( SendCommand(wxT("PWD"), '2') )
    {
        // the result is at least that long if SendCommand() succeeded
        const wxChar *p = m_lastResult.c_str() + LEN_CODE + 1;
        if ( *p != _T('"') )
        {
            wxLogDebug(_T("Missing starting quote in reply for PWD: %s"), p);
        }
        else
        {
            for ( p++; *p; p++ )
            {
                if ( *p == _T('"') )
                {
                    // check if the quote is doubled
                    p++;
                    if ( !*p || *p != _T('"') )
                    {
                        // no, this is the end
                        break;
                    }
                    //else: yes, it is: this is an embedded quote in the
                    //      filename, treat as normal char
                }

                path += *p;
            }

            if ( !*p )
            {
                wxLogDebug(_T("Missing ending quote in reply for PWD: %s"),
                           m_lastResult.c_str() + LEN_CODE + 1);
            }
        }
    }

    return path;
}

bool wxFTP::Rename(const wxString& src, const wxString& dst)
{
  wxString str;

  str = wxT("RNFR ") + src;
  if (!SendCommand(str, '3'))
    return FALSE;

  str = wxT("RNTO ") + dst;
  return SendCommand(str, '2');
}

bool wxFTP::RmFile(const wxString& path)
{
  wxString str;

  str = wxT("DELE ");
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
      if ( IsOk() )
      {
          // close data connection first, this will generate "transfer
          // completed" reply
          delete m_o_socket;

          // read this reply
          m_ftp->GetResult('2');
      }
      else
      {
          // abort data connection first
          m_ftp->Abort();

          // and close it after
          delete m_o_socket;
      }
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

  if (!SendCommand(wxT("PASV"), '2'))
    return NULL;

  addr_pos = m_lastResult.Find(wxT('('));
  if (addr_pos == -1) {
    m_lastError = wxPROTO_PROTERR;
    return NULL;
  }
  straddr = m_lastResult(addr_pos+1, m_lastResult.Length());
  wxSscanf((const wxChar *)straddr,wxT("%d,%d,%d,%d,%d,%d"),&a[2],&a[3],&a[4],&a[5],&a[0],&a[1]);

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

bool wxFTP::Abort()
{
    if ( !m_streaming )
        return TRUE;
        
    m_streaming = FALSE;
    if ( !SendCommand(wxT("ABOR"), '4') )
        return FALSE;

    return GetResult('2');
}

wxInputStream *wxFTP::GetInputStream(const wxString& path)
{
  wxString tmp_str;
  int pos_size;
  wxInputFTPStream *in_stream;

  if (!SendCommand(wxT("TYPE I"), '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  if (!sock) {
    m_lastError = wxPROTO_NETERR;
    return NULL;
  }

  tmp_str = wxT("RETR ") + wxURL::ConvertFromURI(path);
  if (!SendCommand(tmp_str, '1'))
    return NULL;

  in_stream = new wxInputFTPStream(this, sock);

  pos_size = m_lastResult.Index(wxT('('));
  if (pos_size != wxNOT_FOUND) {
    wxString str_size = m_lastResult(pos_size+1, m_lastResult.Index(wxT(')'))-1);

    in_stream->m_ftpsize = wxAtoi(WXSTRINGCAST str_size);
  }
  sock->SetFlags(wxSOCKET_WAITALL);

  return in_stream;
}

wxOutputStream *wxFTP::GetOutputStream(const wxString& path)
{
  wxString tmp_str;

  if (!SendCommand(wxT("TYPE I"), '2'))
    return NULL;

  wxSocketClient *sock = GetPort();

  tmp_str = wxT("STOR ") + path;
  if (!SendCommand(tmp_str, '1'))
    return FALSE;

  return new wxOutputFTPStream(this, sock);
}

bool wxFTP::GetList(wxArrayString& files, const wxString& wildcard)
{
    wxSocketBase *sock = GetPort();
    if ( !sock )
    {
        return FALSE;
    }

    wxString line = _T("NLST");
    if ( !!wildcard )
    {
        // notice that there is no space here
        line += wildcard;
    }

    if ( !SendCommand(line, '1') )
    {
        return FALSE;
    }

    files.Empty();

    while ( ReadLine(sock, line) == wxPROTO_NOERR )
    {
        files.Add(line);
    }

    delete sock;

    // the file list should be terminated by "226 Transfer complete""
    if ( !GetResult('2') )
        return FALSE;

    return TRUE;
}

wxList *wxFTP::GetList(const wxString& wildcard)
{
  wxList *file_list = new wxList;
  wxSocketBase *sock = GetPort();
  wxString tmp_str = wxT("NLST");

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
