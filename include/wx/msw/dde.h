/////////////////////////////////////////////////////////////////////////////
// Name:        dde.h
// Purpose:     DDE class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DDE_H_
#define _WX_DDE_H_

#ifdef __GNUG__
#pragma interface "dde.h"
#endif

#include "wx/ipcbase.h"

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

class WXDLLEXPORT wxDDEServer;
class WXDLLEXPORT wxDDEClient;

class WXDLLEXPORT wxDDEConnection: public wxConnectionBase
{
  DECLARE_DYNAMIC_CLASS(wxDDEConnection)
public:
  wxDDEConnection(char *buffer, int size);
  wxDDEConnection(void);
  ~wxDDEConnection(void);

  // Calls that CLIENT can make
  virtual bool Execute(wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);
  virtual bool Execute(const wxString& str) { return Execute(WXSTRINGCAST str, -1, wxIPC_TEXT); }
  virtual char *Request(const wxString& item, int *size = NULL, wxIPCFormat format = wxIPC_TEXT);
  virtual bool Poke(const wxString& item, wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);
  virtual bool StartAdvise(const wxString& item);
  virtual bool StopAdvise(const wxString& item);

  // Calls that SERVER can make
  virtual bool Advise(const wxString& item, wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);

  // Calls that both can make
  virtual bool Disconnect(void);

  // Callbacks to SERVER - override at will
  virtual bool OnExecute(const wxString& topic, char *data, int size, wxIPCFormat format) { return FALSE; };
  virtual char *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format) { return NULL; };
  virtual bool OnPoke(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format) { return FALSE; };
  virtual bool OnStartAdvise(const wxString& topic, const wxString& item) { return FALSE; };
  virtual bool OnStopAdvise(const wxString& topic, const wxString& item) { return FALSE; };

  // Callbacks to CLIENT - override at will
  virtual bool OnAdvise(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format) { return FALSE; };

  // Callbacks to BOTH

  // Default behaviour is to delete connection and return TRUE
  virtual bool OnDisconnect(void);

 public:
  char*         m_bufPtr;
  wxString      m_topicName;
  int           m_bufSize;
  wxDDEServer*  m_server;
  wxDDEClient*  m_client;

  WXHCONV       m_hConv;
  wxChar*       m_sendingData;
  int           m_dataSize;
  wxIPCFormat  m_dataType;
};

class WXDLLEXPORT wxDDEServer: public wxServerBase
{
  DECLARE_DYNAMIC_CLASS(wxDDEServer)
 public:

  wxDDEServer(void);
  ~wxDDEServer(void);
  bool Create(const wxString& server_name); // Returns FALSE if can't create server (e.g. port
                                  // number is already in use)
  virtual wxConnectionBase *OnAcceptConnection(const wxString& topic);

  ////////////////////////////////////////////////////////////
  // Implementation

  // Find/delete wxDDEConnection corresponding to the HCONV
  wxDDEConnection *FindConnection(WXHCONV conv);
  bool DeleteConnection(WXHCONV conv);
  inline wxString& GetServiceName(void) const { return (wxString&) m_serviceName; }
  inline wxList& GetConnections(void) const { return (wxList&) m_connections; }

 protected:
  int       m_lastError;
  wxString  m_serviceName;
  wxList    m_connections;
};

class WXDLLEXPORT wxDDEClient: public wxClientBase
{
  DECLARE_DYNAMIC_CLASS(wxDDEClient)
 public:
  wxDDEClient(void);
  ~wxDDEClient(void);
  bool ValidHost(const wxString& host);
  virtual wxConnectionBase *MakeConnection(const wxString& host, const wxString& server, const wxString& topic);
                                                // Call this to make a connection.
                                                // Returns NULL if cannot.
  virtual wxConnectionBase *OnMakeConnection(void); // Tailor this to return own connection.

  ////////////////////////////////////////////////////////////
  // Implementation

  // Find/delete wxDDEConnection corresponding to the HCONV
  wxDDEConnection *FindConnection(WXHCONV conv);
  bool DeleteConnection(WXHCONV conv);
  inline wxList& GetConnections(void) const { return (wxList&) m_connections; }

 protected:
  int       m_lastError;
  wxList    m_connections;
};

void WXDLLEXPORT wxDDEInitialize();
void WXDLLEXPORT wxDDECleanUp();

#endif
    // _WX_DDE_H_
