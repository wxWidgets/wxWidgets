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
		      wxSocketBase::wxRequestEvent evt,
		      char *cdata);
void Client_OnRequest(wxSocketBase& sock,
		      wxSocketBase::wxRequestEvent evt,
		      char *cdata);

// ---------------------------------------------------------------------------
// wxTCPClient
// ---------------------------------------------------------------------------

wxTCPClient::wxTCPClient (void)
  : wxClientBase()
{
}

wxTCPClient::~wxTCPClient (void)
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
  wxSocketHandler *hsock = &wxSocketHandler::Master();
  wxSocketClient *client = hsock->CreateClient();
  wxSocketStream *stream = new wxSocketStream(*client);
  wxDataStream data_s(*stream);
  
  client->SetNotify(wxSocketBase::REQ_READ | wxSocketBase::REQ_LOST);
  addr.Service(server_name);
  addr.Hostname(host);

  if (!client->Connect(addr)) {
    delete client;
    return NULL;
  }
  client->Notify(FALSE);

  // Send topic name, and enquire whether this has succeeded
  unsigned char msg;
  
  data_s.Write8(IPC_CONNECT);
  data_s.WriteString(topic);
  
  msg = data_s.Read8();
  
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

wxTCPServer::wxTCPServer (void)
  : wxServerBase()
{
}

bool wxTCPServer::Create(const wxString& server_name)
{
  wxIPV4address addr;
  wxSocketHandler *hsock = &wxSocketHandler::Master();
  wxSocketServer *server;

  addr.Service(server_name);

  // Create a socket listening on specified port
  server = hsock->CreateServer(addr);
  server->Callback((wxSocketBase::wxSockCbk)Server_OnRequest);
  server->SetNotify(wxSocketBase::REQ_ACCEPT);

  server->CallbackData((char *)this);

  return TRUE;
}

wxTCPServer::~wxTCPServer(void)
{
}

wxConnectionBase *wxTCPServer::OnAcceptConnection( const wxString& WXUNUSED(topic) )
{
  return new wxTCPConnection();
}

// ---------------------------------------------------------------------------
// wxTCPConnection
// ---------------------------------------------------------------------------

wxTCPConnection::wxTCPConnection (void)
  : wxConnectionBase(),
    m_sock(NULL), m_sockstrm(NULL), m_codec(NULL)
{
}

wxTCPConnection::~wxTCPConnection (void)
{
  wxDELETE(m_sock);
  wxDELETE(m_codec);
  wxDELETE(m_sockstrm);
}

void wxTCPConnection::Compress(bool on)
{
  // Use wxLZWStream
}

// Calls that CLIENT can make.
bool wxTCPConnection::Disconnect (void)
{
  // Send the the disconnect message to the peer.
  m_codec->Write8(IPC_DISCONNECT);
  m_sock->Close();

  return TRUE;
}

bool wxTCPConnection::Execute (char *data, int size, wxDataFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  // Prepare EXECUTE message
  m_codec->Write8(IPC_EXECUTE);
  m_codec->Write8(format);
  if (size < 0)
    m_codec->WriteString(data);
  else {
    m_codec->Write32(size);
    m_codec->Write(data, size);
  }

  return TRUE;
}

char *wxTCPConnection::Request (const wxString& item, int *size, wxDataFormat format)
{
  if (!m_sock->IsConnected())
    return NULL;

  m_codec->Write8(IPC_REQUEST);
  m_codec->WriteString(item);
  m_codec->Write8(format);

  // If Unpack doesn't initialize it.
  int ret;

  ret = m_codec->Read8();
  if (ret == IPC_FAIL)
    return NULL;
  else {
    size_t s;
    char *data = NULL;

    s = m_codec->Read32();
    data = new char[s];
    m_codec->Read(data, s);

    if (size)
      *size = s;
    return data;
  }
}

bool wxTCPConnection::Poke (const wxString& item, char *data, int size, wxDataFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  m_codec->Write8(IPC_POKE);
  m_codec->WriteString(item);
  m_codec->Write8(format);
  if (size < 0)
    m_codec->WriteString(data);
  else {
    m_codec->Write32(size);
    m_codec->Write(data, size);
  }

  return TRUE;
}

bool wxTCPConnection::StartAdvise (const wxString& item)
{
  int ret;

  if (!m_sock->IsConnected())
    return FALSE;

  m_codec->Write8(IPC_ADVISE_START);
  m_codec->WriteString(item);

  ret = m_codec->Read8();

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

  m_codec->Write8(IPC_ADVISE_STOP);
  m_codec->WriteString(item);

  msg = m_codec->Read8();

  if (msg != IPC_FAIL)
    return TRUE;
  else
    return FALSE;
}

// Calls that SERVER can make
bool wxTCPConnection::Advise (const wxString& item,
                              char *data, int size, wxDataFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  m_codec->Write8(IPC_ADVISE);
  m_codec->WriteString(item);
  m_codec->Write8(format);
  if (size < 0)
    m_codec->WriteString(data);
  else {
    m_codec->Write32(size);
    m_codec->Write(data, size);
  }

  return TRUE;
}

void Client_OnRequest(wxSocketBase& sock, wxSocketBase::wxRequestEvent evt,
		      char *cdata)
{
  int msg = 0;
  wxTCPConnection *connection = (wxTCPConnection *)cdata;
  wxDataStream *codec;
  wxString topic_name = connection->m_topic;
  wxString item;

  // The socket handler signals us that we lost the connection: destroy all.
  if (evt == wxSocketBase::EVT_LOST) {
    sock.Close();
    connection->OnDisconnect();
    return;
  }

  // Receive message number.
  codec = connection->m_codec;
  msg = codec->Read8();

  switch (msg) {
  case IPC_EXECUTE: {
    char *data;
    size_t size; 
    wxDataFormat format;
    
    format = (wxDataFormat)codec->Read8();
    size = codec->Read32();
    data = new char[size];
    codec->Read(data, size);

    connection->OnExecute (topic_name, data, size, format);

    delete [] data;
    break;
  }
  case IPC_ADVISE: {
    char *data;
    size_t size;
    wxDataFormat format;

    item = codec->ReadString();
    format = (wxDataFormat)codec->Read8();
    size = codec->Read32();
    data = new char[size];
    codec->Read(data, size);
    
    connection->OnAdvise (topic_name, item, data, size, format);

    delete [] data;
    break;
  }
  case IPC_ADVISE_START: {
    item = codec->ReadString();

    bool ok = connection->OnStartAdvise (topic_name, item);
    if (ok)
      codec->Write8(IPC_ADVISE_START);
    else
      codec->Write8(IPC_FAIL);

    break;
  }
  case IPC_ADVISE_STOP: {
    item = codec->ReadString();

    bool ok = connection->OnStopAdvise (topic_name, item);
    if (ok)
      codec->Write8(IPC_ADVISE_STOP);
    else
      codec->Write8(IPC_FAIL);

    break;
  }
  case IPC_POKE: {
    wxDataFormat format;
    size_t size;
    char *data;

    item = codec->ReadString();
    format = (wxDataFormat)codec->Read8();
    size = codec->Read32();
    data = new char[size];
    codec->Read(data, size);
    
    connection->OnPoke (topic_name, item, data, size, format);

    delete [] data;

    break;
  }
  case IPC_REQUEST: {
    wxDataFormat format;

    item = codec->ReadString();
    format = (wxDataFormat)codec->Read8();

    int user_size = -1;
    char *user_data = connection->OnRequest (topic_name, item, &user_size, format);

    if (user_data) {
      codec->Write8(IPC_REQUEST_REPLY);
      if (user_size != -1) {
        codec->Write32(user_size);
        codec->Write(user_data, user_size);
      } else
        codec->WriteString(user_data);
    } else
      codec->Write8(IPC_FAIL);

    break;
  }
  case IPC_DISCONNECT: {
    sock.Close();
    connection->OnDisconnect();
    break;
  }
  default:
    codec->Write8(IPC_FAIL);
    break;
  }
}

void Server_OnRequest(wxSocketServer& server,
		      wxSocketBase::wxRequestEvent evt, char *cdata)
{
  wxTCPServer *ipcserv = (wxTCPServer *)cdata;
  wxSocketStream *stream;
  wxDataStream *codec;

  if (evt != wxSocketBase::EVT_ACCEPT)
    return;

  /* Accept the connection, getting a new socket */
  wxSocketBase *sock = server.Accept();
  sock->Notify(FALSE);
  sock->SetNotify(wxSocketBase::REQ_READ | wxSocketBase::REQ_LOST);

  stream = new wxSocketStream(*sock);
  codec = new wxDataStream(*stream);

  if (!sock->Ok())
    return;

  int msg;
  msg = codec->Read8();

  if (msg == IPC_CONNECT) {
    wxString topic_name;
    topic_name = codec->ReadString();

    /* Register new socket with the notifier */
    wxTCPConnection *new_connection =
         (wxTCPConnection *)ipcserv->OnAcceptConnection (topic_name);
    if (new_connection) {
      if (!new_connection->IsKindOf(CLASSINFO(wxTCPConnection))) {
        delete new_connection;
        codec->Write8(IPC_FAIL);
        return;
      }
      // Acknowledge success
      codec->Write8(IPC_CONNECT);
      
      new_connection->m_topic = topic_name;
      new_connection->m_sockstrm = stream;
      new_connection->m_codec = codec;
      sock->Callback(Client_OnRequest);
      sock->CallbackData((char *)new_connection);
      sock->Notify(TRUE);
    } else {
      // Send failure message
      codec->Write8(IPC_FAIL);
    }
  }
}
