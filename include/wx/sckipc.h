/////////////////////////////////////////////////////////////////////////////
// Name:        sckipc.h
// Purpose:     Interprocess communication
// Author:      Julian Smart/Guilhem Lavaux (big rewrite)
// Modified by: Guilhem Lavaux 1997
// Created:     1993
// RCS-ID:      $Id$
// Copyright:   (c) 1993 Julian Smart
//              (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_SCKIPC_H
#define _WX_SCKIPC_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/setup.h"
#include "wx/ipcbase.h"
#include "wx/socket.h"
#include "wx/sckstrm.h"
#include "wx/datstrm.h"

/*
 * Mini-DDE implementation

   Most transactions involve a topic name and an item name (choose these
   as befits your application).

   A client can:

   - ask the server to execute commands (data) associated with a topic
   - request data from server by topic and item
   - poke data into the server
   - ask the server to start an advice loop on topic/item
   - ask the server to stop an advice loop

   A server can:

   - respond to execute, request, poke and advice start/stop
   - send advise data to client

   Note that this limits the server in the ways it can send data to the
   client, i.e. it can't send unsolicited information.
 *
 */

class wxTCPServer;
class wxTCPClient;
class wxTCPConnection: public wxConnectionBase
{
  DECLARE_DYNAMIC_CLASS(wxTCPConnection)
  
protected:
  wxSocketBase *m_sock;
  wxSocketStream *m_sockstrm;
  wxDataInputStream *m_codeci;
  wxDataOutputStream *m_codeco;
  wxString m_topic;

  friend class wxTCPServer;
  friend class wxTCPClient;
  friend void Client_OnRequest(wxSocketBase&,
			       wxSocketBase::wxRequestEvent, char *);
  friend void Server_OnRequest(wxSocketServer&,
			       wxSocketBase::wxRequestEvent, char *);
public:

  wxTCPConnection(char *buffer, int size);
  wxTCPConnection();
  virtual ~wxTCPConnection();

  // Calls that CLIENT can make
  bool Execute(char *data, int size = -1,
               wxIPCFormat format = wxIPC_TEXT);
  char *Request(const wxString& item, int *size = NULL,
                wxIPCFormat format = wxIPC_TEXT);
  bool Poke(const wxString& item, char *data, int size = -1,
            wxIPCFormat format = wxIPC_TEXT);
  bool StartAdvise(const wxString& item);
  bool StopAdvise(const wxString& item);

  // Calls that SERVER can make
  bool Advise(const wxString& item, char *data, int size = -1,
              wxIPCFormat format = wxIPC_TEXT);

  // Calls that both can make
  bool Disconnect();

  // Called when we lost the peer.
  bool OnDisconnect() { return TRUE; }

  // To enable the compressor
  void Compress(bool on);
};

class wxTCPServer: public wxServerBase
{
  DECLARE_DYNAMIC_CLASS(wxTCPServer)

public:
  wxTCPConnection *topLevelConnection;

  wxTCPServer();
  virtual ~wxTCPServer();
  
  // Returns FALSE if can't create server (e.g. port number is already in use)
  virtual bool Create(const wxString& server_name); 
  virtual wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

class wxTCPClient: public wxClientBase
{
  DECLARE_DYNAMIC_CLASS(wxTCPClient)

public: 
  wxTCPClient();
  virtual ~wxTCPClient();

  virtual bool ValidHost(const wxString& host);
    // Call this to make a connection.
    // Returns NULL if cannot.
  virtual wxConnectionBase *MakeConnection(const wxString& host,
                                           const wxString& server,
                                           const wxString& topic);
  
  // Tailor this to return own connection.
  virtual wxConnectionBase *OnMakeConnection();
};

#endif // ipcsock.h
