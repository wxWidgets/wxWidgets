/////////////////////////////////////////////////////////////////////////////
// Name:        sckipc.cpp
// Purpose:     Interprocess communication implementation (wxSocket version)
// Author:      Julian Smart, Guilhem Lavaux
// Modified by: Guilhem Lavaux (big rewrite) May 1997, 1998
// Created:     1993
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart 1993, Guilhem Lavaux 1997, 1998
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "sckipc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

#ifndef WX_PRECOMP
#endif

#include <stdlib.h>
#include <stdio.h>

#include "wx/socket.h"
#include "wx/sckipc.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTCPServer, wxServerBase)
IMPLEMENT_DYNAMIC_CLASS(wxTCPClient, wxClientBase)
IMPLEMENT_DYNAMIC_CLASS(wxTCPConnection, wxConnectionBase)
#endif

// It seems to be already defined somewhere in the Xt includes.
#ifndef __XT__
// Message codes
enum {
  IPC_EXECUTE = 1,
  IPC_REQUEST,
  IPC_POKE,
  IPC_ADVISE_START,
  IPC_ADVISE_REQUEST,
  IPC_ADVISE,
  IPC_ADVISE_STOP,
  IPC_REQUEST_REPLY,
  IPC_FAIL,
  IPC_CONNECT,
  IPC_DISCONNECT
};
#endif

void Server_OnRequest(wxSocketServer& server,
		      wxSocketNotify evt,
		      char *cdata);
void Client_OnRequest(wxSocketBase& sock,
		      wxSocketNotify evt,
		      char *cdata);

// ---------------------------------------------------------------------------
// wxTCPClient
// ---------------------------------------------------------------------------

wxTCPClient::wxTCPClient ()
  : wxClientBase()
{
}

wxTCPClient::~wxTCPClient ()
{
}

bool wxTCPClient::ValidHost(const wxString& host)
{
  wxIPV4address addr;

  return addr.Hostname(host);
}

wxConnectionBase *wxTCPClient::MakeConnection (const wxString& host,
                                               const wxString& server_name,
                                               const wxString& topic)
{
  wxIPV4address addr;
  wxSocketClient *client = new wxSocketClient();
  wxSocketStream *stream = new wxSocketStream(*client);
  wxDataInputStream data_is(*stream);
  wxDataOutputStream data_os(*stream);
  
  client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  addr.Service(server_name);
  addr.Hostname(host);

  if (!client->Connect(addr)) {
    delete client;
    return NULL;
  }
  client->Notify(FALSE);

  // Send topic name, and enquire whether this has succeeded
  unsigned char msg;
  
  data_os.Write8(IPC_CONNECT);
  data_os.WriteString(topic);
  
  msg = data_is.Read8();
  
  // OK! Confirmation.
  if (msg == IPC_CONNECT) {
    wxTCPConnection *connection = (wxTCPConnection *)OnMakeConnection ();
    if (connection) {
      if (!connection->IsKindOf(CLASSINFO(wxTCPConnection))) {
        delete connection;
        return NULL;
      }
      connection->m_topic = topic;
      client->Callback(Client_OnRequest);
      client->CallbackData((char *)connection);
      client->Notify(TRUE);
      return connection;
    } else {
      delete client;
      return NULL;
    }
  } else {
    delete client;
    return NULL;
  }
  return NULL;
}

wxConnectionBase *wxTCPClient::OnMakeConnection()
{
  return new wxTCPConnection;
}

// ---------------------------------------------------------------------------
// wxTCPServer
// ---------------------------------------------------------------------------

wxTCPServer::wxTCPServer ()
  : wxServerBase()
{
}

bool wxTCPServer::Create(const wxString& server_name)
{
  wxIPV4address addr;
  wxSocketServer *server;

  addr.Service(server_name);

  // Create a socket listening on specified port
  server = new wxSocketServer(addr);
  server->Callback((wxSocketBase::wxSockCbk)Server_OnRequest);
  server->SetNotify(wxSOCKET_CONNECTION_FLAG);

  server->CallbackData((char *)this);

  return TRUE;
}

wxTCPServer::~wxTCPServer()
{
}

wxConnectionBase *wxTCPServer::OnAcceptConnection( const wxString& WXUNUSED(topic) )
{
  return new wxTCPConnection();
}

// ---------------------------------------------------------------------------
// wxTCPConnection
// ---------------------------------------------------------------------------

wxTCPConnection::wxTCPConnection ()
  : wxConnectionBase(),
    m_sock(NULL), m_sockstrm(NULL), m_codeci(NULL), m_codeco(NULL)
{
}

wxTCPConnection::wxTCPConnection(char * WXUNUSED(buffer), int WXUNUSED(size))
{
}

wxTCPConnection::~wxTCPConnection ()
{
  wxDELETE(m_sock);
  wxDELETE(m_codeci);
  wxDELETE(m_codeco);
  wxDELETE(m_sockstrm);
}

void wxTCPConnection::Compress(bool WXUNUSED(on))
{
  // Use wxLZWStream
}

// Calls that CLIENT can make.
bool wxTCPConnection::Disconnect ()
{
  // Send the the disconnect message to the peer.
  m_codeco->Write8(IPC_DISCONNECT);
  m_sock->Close();

  return TRUE;
}

bool wxTCPConnection::Execute (wxChar *data, int size, wxIPCFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  // Prepare EXECUTE message
  m_codeco->Write8(IPC_EXECUTE);
  m_codeco->Write8(format);
  if (size < 0)
    m_codeco->WriteString(data);
  else {
    m_codeco->Write32(size);
    m_sockstrm->Write(data, size);
  }

  return TRUE;
}

char *wxTCPConnection::Request (const wxString& item, int *size, wxIPCFormat format)
{
  if (!m_sock->IsConnected())
    return NULL;

  m_codeco->Write8(IPC_REQUEST);
  m_codeco->WriteString(item);
  m_codeco->Write8(format);

  // If Unpack doesn't initialize it.
  int ret;

  ret = m_codeci->Read8();
  if (ret == IPC_FAIL)
    return NULL;
  else {
    size_t s;
    char *data = NULL;

    s = m_codeci->Read32();
    data = new char[s];
    m_sockstrm->Read(data, s);

    if (size)
      *size = s;
    return data;
  }
}

bool wxTCPConnection::Poke (const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  m_codeco->Write8(IPC_POKE);
  m_codeco->WriteString(item);
  m_codeco->Write8(format);
  if (size < 0)
    m_codeco->WriteString(data);
  else {
    m_codeco->Write32(size);
    m_sockstrm->Write(data, size);
  }

  return TRUE;
}

bool wxTCPConnection::StartAdvise (const wxString& item)
{
  int ret;

  if (!m_sock->IsConnected())
    return FALSE;

  m_codeco->Write8(IPC_ADVISE_START);
  m_codeco->WriteString(item);

  ret = m_codeci->Read8();

  if (ret != IPC_FAIL)
    return TRUE;
  else
    return FALSE;
}

bool wxTCPConnection::StopAdvise (const wxString& item)
{
  int msg;

  if (!m_sock->IsConnected())
    return FALSE;

  m_codeco->Write8(IPC_ADVISE_STOP);
  m_codeco->WriteString(item);

  msg = m_codeci->Read8();

  if (msg != IPC_FAIL)
    return TRUE;
  else
    return FALSE;
}

// Calls that SERVER can make
bool wxTCPConnection::Advise (const wxString& item,
                              wxChar *data, int size, wxIPCFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  m_codeco->Write8(IPC_ADVISE);
  m_codeco->WriteString(item);
  m_codeco->Write8(format);
  if (size < 0)
    m_codeco->WriteString(data);
  else {
    m_codeco->Write32(size);
    m_sockstrm->Write(data, size);
  }

  return TRUE;
}

void Client_OnRequest(wxSocketBase& sock, wxSocketNotify evt,
		      char *cdata)
{
  int msg = 0;
  wxTCPConnection *connection = (wxTCPConnection *)cdata;
  wxDataInputStream *codeci;
  wxDataOutputStream *codeco; 
  wxSocketStream *sockstrm;
  wxString topic_name = connection->m_topic;
  wxString item;

  // The socket handler signals us that we lost the connection: destroy all.
  if (evt == wxSOCKET_LOST) {
    sock.Close();
    connection->OnDisconnect();
    return;
  }

  // Receive message number.
  codeci = connection->m_codeci;
  codeco = connection->m_codeco;
  sockstrm = connection->m_sockstrm;
  msg = codeci->Read8();

  switch (msg) {
  case IPC_EXECUTE: {
    char *data;
    size_t size; 
    wxIPCFormat format;
    
    format = (wxIPCFormat)codeci->Read8();
    size = codeci->Read32();
    data = new char[size];
    sockstrm->Read(data, size);

    connection->OnExecute (topic_name, data, size, format);

    delete [] data;
    break;
  }
  case IPC_ADVISE: {
    char *data;
    size_t size;
    wxIPCFormat format;

    item = codeci->ReadString();
    format = (wxIPCFormat)codeci->Read8();
    size = codeci->Read32();
    data = new char[size];
    sockstrm->Read(data, size);
    
    connection->OnAdvise (topic_name, item, data, size, format);

    delete [] data;
    break;
  }
  case IPC_ADVISE_START: {
    item = codeci->ReadString();

    bool ok = connection->OnStartAdvise (topic_name, item);
    if (ok)
      codeco->Write8(IPC_ADVISE_START);
    else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_ADVISE_STOP: {
    item = codeci->ReadString();

    bool ok = connection->OnStopAdvise (topic_name, item);
    if (ok)
      codeco->Write8(IPC_ADVISE_STOP);
    else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_POKE: {
    wxIPCFormat format;
    size_t size;
    wxChar *data;

    item = codeci->ReadString();
    format = (wxIPCFormat)codeci->Read8();
    size = codeci->Read32();
    data = new wxChar[size];
    sockstrm->Read(data, size);
    
    connection->OnPoke (topic_name, item, data, size, format);

    delete [] data;

    break;
  }
  case IPC_REQUEST: {
    wxIPCFormat format;

    item = codeci->ReadString();
    format = (wxIPCFormat)codeci->Read8();

    int user_size = -1;
    char *user_data = connection->OnRequest (topic_name, item, &user_size, format);

    if (user_data) {
      codeco->Write8(IPC_REQUEST_REPLY);
      if (user_size != -1) {
        codeco->Write32(user_size);
        sockstrm->Write(user_data, user_size);
      } else
        codeco->WriteString(user_data);
    } else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_DISCONNECT: {
    sock.Close();
    connection->OnDisconnect();
    break;
  }
  default:
    codeco->Write8(IPC_FAIL);
    break;
  }
}

void Server_OnRequest(wxSocketServer& server,
		      wxSocketNotify evt, char *cdata)
{
  wxTCPServer *ipcserv = (wxTCPServer *)cdata;
  wxSocketStream *stream;
  wxDataInputStream *codeci;
  wxDataOutputStream *codeco;

  if (evt != wxSOCKET_CONNECTION)
    return;

  /* Accept the connection, getting a new socket */
  wxSocketBase *sock = server.Accept();
  sock->Notify(FALSE);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);

  stream = new wxSocketStream(*sock);
  codeci = new wxDataInputStream(*stream);
  codeco = new wxDataOutputStream(*stream);

  if (!sock->Ok())
    return;

  int msg;
  msg = codeci->Read8();

  if (msg == IPC_CONNECT) {
    wxString topic_name;
    topic_name = codeci->ReadString();

    /* Register new socket with the notifier */
    wxTCPConnection *new_connection =
         (wxTCPConnection *)ipcserv->OnAcceptConnection (topic_name);
    if (new_connection) {
      if (!new_connection->IsKindOf(CLASSINFO(wxTCPConnection))) {
        delete new_connection;
        codeco->Write8(IPC_FAIL);
        return;
      }
      // Acknowledge success
      codeco->Write8(IPC_CONNECT);
      
      new_connection->m_topic = topic_name;
      new_connection->m_sockstrm = stream;
      new_connection->m_codeci = codeci;
      new_connection->m_codeco = codeco;
      sock->Callback(Client_OnRequest);
      sock->CallbackData((char *)new_connection);
      sock->Notify(TRUE);
    } else {
      // Send failure message
      codeco->Write8(IPC_FAIL);
    }
  }
}

#endif
  // wxUSE_SOCKETS
