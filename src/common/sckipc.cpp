/////////////////////////////////////////////////////////////////////////////
// Name:        sckipc.cpp
// Purpose:     Interprocess communication implementation (wxSocket version)
// Author:      Julian Smart
// Modified by: Guilhem Lavaux (big rewrite) May 1997, 1998
//              Guillermo Rodriguez (updated for wxSocket v2) Jan 2000
//                                  (callbacks deprecated)    Mar 2000
// Created:     1993
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart 1993
//              (c) Guilhem Lavaux 1997, 1998
//              (c) 2000 Guillermo Rodriguez <guille@iies.es>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "sckipc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if wxUSE_SOCKETS && wxUSE_IPC

#include <stdlib.h>
#include <stdio.h>

#include "wx/socket.h"
#include "wx/sckipc.h"
#include "wx/module.h"
#include "wx/event.h"
#include "wx/log.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// --------------------------------------------------------------------------
// macros and constants
// --------------------------------------------------------------------------

// It seems to be already defined somewhere in the Xt includes.
#ifndef __XT__
// Message codes
enum
{
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


// All sockets will be created with the following flags
#define SCKIPC_FLAGS (wxSOCKET_WAITALL)

// --------------------------------------------------------------------------
// wxTCPEventHandler stuff (private class)
// --------------------------------------------------------------------------

class wxTCPEventHandler : public wxEvtHandler
{
public:
  wxTCPEventHandler() : wxEvtHandler() {};

  void Client_OnRequest(wxSocketEvent& event);
  void Server_OnRequest(wxSocketEvent& event);

  DECLARE_EVENT_TABLE()
};

enum
{
  _CLIENT_ONREQUEST_ID = 1000,
  _SERVER_ONREQUEST_ID
};

static wxTCPEventHandler *gs_handler = NULL;

// ==========================================================================
// implementation
// ==========================================================================

IMPLEMENT_DYNAMIC_CLASS(wxTCPServer, wxServerBase)
IMPLEMENT_DYNAMIC_CLASS(wxTCPClient, wxClientBase)
IMPLEMENT_CLASS(wxTCPConnection, wxConnectionBase)

// --------------------------------------------------------------------------
// wxTCPClient
// --------------------------------------------------------------------------

wxTCPClient::wxTCPClient () : wxClientBase()
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
  wxSocketClient *client = new wxSocketClient(SCKIPC_FLAGS);
  wxSocketStream *stream = new wxSocketStream(*client);
  wxDataInputStream *data_is = new wxDataInputStream(*stream);
  wxDataOutputStream *data_os = new wxDataOutputStream(*stream);

  wxIPV4address addr;
  addr.Service(server_name);
  addr.Hostname(host);

  if (client->Connect(addr))
  {
    unsigned char msg;
  
    // Send topic name, and enquire whether this has succeeded
    data_os->Write8(IPC_CONNECT);
    data_os->WriteString(topic);
  
    msg = data_is->Read8();

    // OK! Confirmation.
    if (msg == IPC_CONNECT)
    {
      wxTCPConnection *connection = (wxTCPConnection *)OnMakeConnection ();

      if (connection)
      {
        if (connection->IsKindOf(CLASSINFO(wxTCPConnection)))
        {
          connection->m_topic = topic;
          connection->m_sock  = client;
          connection->m_sockstrm = stream;
          connection->m_codeci = data_is;
          connection->m_codeco = data_os;
          client->SetEventHandler(*gs_handler, _CLIENT_ONREQUEST_ID);
          client->SetClientData(connection);
          client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
          client->Notify(TRUE);
          return connection;
        }
        else
        {
          delete connection;
          // and fall through to delete everything else
        }
      }
    }
  }

  // Something went wrong, delete everything
  delete data_is;
  delete data_os;
  delete stream;
  client->Destroy();

  return NULL;
}

wxConnectionBase *wxTCPClient::OnMakeConnection()
{
  return new wxTCPConnection();
}

// --------------------------------------------------------------------------
// wxTCPServer
// --------------------------------------------------------------------------

wxTCPServer::wxTCPServer () : wxServerBase()
{
}

bool wxTCPServer::Create(const wxString& server_name)
{
  wxSocketServer *server;

  // wxIPV4address defaults to INADDR_ANY:0
  wxIPV4address addr;
  addr.Service(server_name);

  // Create a socket listening on specified port
  server = new wxSocketServer(addr, SCKIPC_FLAGS);
  server->SetEventHandler(*gs_handler, _SERVER_ONREQUEST_ID);
  server->SetClientData(this);
  server->SetNotify(wxSOCKET_CONNECTION_FLAG);
  server->Notify(TRUE);

  return TRUE;
}

wxTCPServer::~wxTCPServer()
{
}

wxConnectionBase *wxTCPServer::OnAcceptConnection( const wxString& WXUNUSED(topic) )
{
  return new wxTCPConnection();
}

// --------------------------------------------------------------------------
// wxTCPConnection
// --------------------------------------------------------------------------

wxTCPConnection::wxTCPConnection () : wxConnectionBase()
{
  m_sock     = NULL;
  m_sockstrm = NULL;
  m_codeci   = NULL;
  m_codeco   = NULL;
}

wxTCPConnection::wxTCPConnection(char * WXUNUSED(buffer), int WXUNUSED(size))
{
}

wxTCPConnection::~wxTCPConnection ()
{
  wxDELETE(m_codeci);
  wxDELETE(m_codeco);
  wxDELETE(m_sockstrm);

  if (m_sock)
  {
    m_sock->SetClientData(NULL);
    m_sock->Destroy();
  }
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
  m_sock->Notify(FALSE);
  m_sock->Close();

  return TRUE;
}

bool wxTCPConnection::Execute(const wxChar *data, int size, wxIPCFormat format)
{
  if (!m_sock->IsConnected())
    return FALSE;

  // Prepare EXECUTE message
  m_codeco->Write8(IPC_EXECUTE);
  m_codeco->Write8(format);

  if (size < 0)
    size = strlen(data) + 1;    // includes final NUL

  m_codeco->Write32(size);
  m_sockstrm->Write(data, size);

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
  else
  {
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
    size = strlen(data) + 1;    // includes final NUL

  m_codeco->Write32(size);
  m_sockstrm->Write(data, size);

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
    size = strlen(data) + 1;    // includes final NUL

  m_codeco->Write32(size);
  m_sockstrm->Write(data, size);

  return TRUE;
}

// --------------------------------------------------------------------------
// wxTCPEventHandler (private class)
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTCPEventHandler, wxEvtHandler)
  EVT_SOCKET(_CLIENT_ONREQUEST_ID, wxTCPEventHandler::Client_OnRequest)
  EVT_SOCKET(_SERVER_ONREQUEST_ID, wxTCPEventHandler::Server_OnRequest)
END_EVENT_TABLE()

void wxTCPEventHandler::Client_OnRequest(wxSocketEvent &event)
{
  wxSocketBase *sock = event.GetSocket();
  wxSocketNotify evt = event.GetSocketEvent();
  wxTCPConnection *connection = (wxTCPConnection *)(event.GetClientData());

  // This socket is being deleted; skip this event
  if (!connection)
    return;

  int msg = 0;
  wxDataInputStream *codeci;
  wxDataOutputStream *codeco; 
  wxSocketStream *sockstrm;
  wxString topic_name = connection->m_topic;
  wxString item;

  // We lost the connection: destroy everything
  if (evt == wxSOCKET_LOST)
  {
    sock->Notify(FALSE);
    sock->Close();
    connection->OnDisconnect();
    return;
  }

  // Receive message number.
  codeci = connection->m_codeci;
  codeco = connection->m_codeco;
  sockstrm = connection->m_sockstrm;
  msg = codeci->Read8();

  switch (msg)
  {
  case IPC_EXECUTE:
  {
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
  case IPC_ADVISE:
  {
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
  case IPC_ADVISE_START:
  {
    item = codeci->ReadString();

    bool ok = connection->OnStartAdvise (topic_name, item);
    if (ok)
      codeco->Write8(IPC_ADVISE_START);
    else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_ADVISE_STOP:
  {
    item = codeci->ReadString();

    bool ok = connection->OnStopAdvise (topic_name, item);
    if (ok)
      codeco->Write8(IPC_ADVISE_STOP);
    else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_POKE:
  {
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
  case IPC_REQUEST:
  {
    wxIPCFormat format;

    item = codeci->ReadString();
    format = (wxIPCFormat)codeci->Read8();

    int user_size = -1;
    char *user_data = connection->OnRequest (topic_name, item, &user_size, format);

    if (user_data)
    {
      codeco->Write8(IPC_REQUEST_REPLY);

      if (user_size == -1)
        user_size = strlen(user_data) + 1;      // includes final NUL

      codeco->Write32(user_size);
      sockstrm->Write(user_data, user_size);
    }
    else
      codeco->Write8(IPC_FAIL);

    break;
  }
  case IPC_DISCONNECT:
  {
    sock->Notify(FALSE);
    sock->Close();
    connection->OnDisconnect();
    break;
  }
  default:
    codeco->Write8(IPC_FAIL);
    break;
  }
}

void wxTCPEventHandler::Server_OnRequest(wxSocketEvent &event)
{
  wxSocketServer *server = (wxSocketServer *) event.GetSocket();
  wxTCPServer *ipcserv = (wxTCPServer *) event.GetClientData();

  // This socket is being deleted; skip this event
  if (!ipcserv)
    return;

  if (event.GetSocketEvent() != wxSOCKET_CONNECTION)
    return;

  // Accept the connection, getting a new socket
  wxSocketBase *sock = server->Accept();
  if (!sock->Ok())
  {
    sock->Destroy();
    return;
  }

  wxSocketStream *stream     = new wxSocketStream(*sock);
  wxDataInputStream *codeci  = new wxDataInputStream(*stream);
  wxDataOutputStream *codeco = new wxDataOutputStream(*stream);

  int msg;
  msg = codeci->Read8();

  if (msg == IPC_CONNECT)
  {
    wxString topic_name;
    topic_name = codeci->ReadString();

    wxTCPConnection *new_connection =
         (wxTCPConnection *)ipcserv->OnAcceptConnection (topic_name);

    if (new_connection)
    {
      if (new_connection->IsKindOf(CLASSINFO(wxTCPConnection)))
      {
        // Acknowledge success
        codeco->Write8(IPC_CONNECT);
        new_connection->m_topic = topic_name;
        new_connection->m_sock = sock;      
        new_connection->m_sockstrm = stream;
        new_connection->m_codeci = codeci;
        new_connection->m_codeco = codeco;
        sock->SetEventHandler(*gs_handler, _CLIENT_ONREQUEST_ID);
        sock->SetClientData(new_connection);
        sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
        sock->Notify(TRUE);
        return;
      }
      else
      {
        delete new_connection;
        // and fall through to delete everything else
      }
    }
  }

  // Something went wrong, send failure message and delete everything
  codeco->Write8(IPC_FAIL);

  delete codeco;
  delete codeci;
  delete stream;
  sock->Destroy();
}

// --------------------------------------------------------------------------
// wxTCPEventHandlerModule (private class)
// --------------------------------------------------------------------------

class WXDLLEXPORT wxTCPEventHandlerModule: public wxModule
{
  DECLARE_DYNAMIC_CLASS(wxTCPEventHandlerModule)

public:
  bool OnInit() { gs_handler = new wxTCPEventHandler(); return TRUE; }
  void OnExit() { wxDELETE(gs_handler); }
};

IMPLEMENT_DYNAMIC_CLASS(wxTCPEventHandlerModule, wxModule)


#endif
    // wxUSE_SOCKETS && wxUSE_IPC
