/////////////////////////////////////////////////////////////////////////////
// Name:        dde.cpp
// Purpose:     DDE classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dde.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if USE_IPC

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/app.h"
#endif

#include "wx/msw/private.h"
#include "wx/dde.h"

#ifdef __GNUWIN32__
#include "wx/msw/gnuwin32/extra.h"
#endif

#include <windows.h>
#include <ddeml.h>
#include <string.h>

#ifdef __WIN32__
#define _EXPORT /**/
#else
#define _EXPORT _export
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxDDEServer, wxServerBase)
IMPLEMENT_DYNAMIC_CLASS(wxDDEClient, wxClientBase)
IMPLEMENT_CLASS(wxDDEConnection, wxConnectionBase)
#endif

static wxDDEConnection *DDEFindConnection(HCONV hConv);
static void DDEDeleteConnection(HCONV hConv);
static wxDDEServer *DDEFindServer(const wxString& s);

extern "C" HDDEDATA EXPENTRY _EXPORT _DDECallback(
WORD wType,
WORD wFmt,
HCONV hConv,
HSZ hsz1,
HSZ hsz2,
HDDEDATA hData,
DWORD lData1,
DWORD lData2);

// Add topic name to atom table before using in conversations
static HSZ DDEAddAtom(const wxString& string);
static HSZ DDEGetAtom(const wxString& string);
static void DDEPrintError(void);

static DWORD DDEIdInst = 0L;
static wxDDEConnection *DDECurrentlyConnecting = NULL;

static wxList wxAtomTable(wxKEY_STRING);
static wxList wxDDEClientObjects;
static wxList wxDDEServerObjects;

char *DDEDefaultIPCBuffer = NULL;
int DDEDefaultIPCBufferSize = 0;

/*
 * Initialization
 *
 */

static bool DDEInitialized = FALSE;

void wxDDEInitialize()
{
  if (DDEInitialized)
    return;
  DDEInitialized = TRUE;

  // Should insert filter flags
  DdeInitialize(&DDEIdInst, (PFNCALLBACK)MakeProcInstance(
               (FARPROC)_DDECallback, wxGetInstance()),
               APPCLASS_STANDARD,
               0L);
}

/*
 * CleanUp
 */

void wxDDECleanUp()
{
  if (DDEIdInst != 0)
  {
    DdeUninitialize(DDEIdInst);
    DDEIdInst = 0;
  }
  if (DDEDefaultIPCBuffer)
    delete [] DDEDefaultIPCBuffer ;
}

// Global find connection
static wxDDEConnection *DDEFindConnection(HCONV hConv)
{
  wxNode *node = wxDDEServerObjects.First();
  wxDDEConnection *found = NULL;
  while (node && !found)
  {
    wxDDEServer *object = (wxDDEServer *)node->Data();
    found = object->FindConnection((WXHCONV) hConv);
    node = node->Next();
  }
  if (found)
  	return found;

  node = wxDDEClientObjects.First();
  while (node && !found)
  {
    wxDDEClient *object = (wxDDEClient *)node->Data();
    found = object->FindConnection((WXHCONV) hConv);
    node = node->Next();
  }
  return found;
}

// Global delete connection
static void DDEDeleteConnection(HCONV hConv)
{
  wxNode *node = wxDDEServerObjects.First();
  bool found = FALSE;
  while (node && !found)
  {
    wxDDEServer *object = (wxDDEServer *)node->Data();
    found = object->DeleteConnection((WXHCONV) hConv);
    node = node->Next();
  }
  if (found)
	return;

  node = wxDDEServerObjects.First();
  while (node && !found)
  {
    wxDDEClient *object = (wxDDEClient *)node->Data();
    found = object->DeleteConnection((WXHCONV) hConv);
    node = node->Next();
  }
}

// Find a server from a service name
static wxDDEServer *DDEFindServer(const wxString& s)
{
  wxNode *node = wxDDEServerObjects.First();
  wxDDEServer *found = NULL;
  while (node && !found)
  {
    wxDDEServer *object = (wxDDEServer *)node->Data();
    
    if (object->GetServiceName() == s)
      found = object;
    else node = node->Next();
  }
  return found;
}

/*
 * Server
 *
 */

wxDDEServer::wxDDEServer(void)
{
  service_name = "";
  wxDDEServerObjects.Append(this);
}

bool wxDDEServer::Create(const wxString& server_name)
{
  service_name = server_name;
  HSZ serviceName = DdeCreateStringHandle(DDEIdInst, (char*) (const char *)server_name, CP_WINANSI);

  if (DdeNameService(DDEIdInst, serviceName, NULL, DNS_REGISTER) == 0)
  {
    DDEPrintError();
    return FALSE;
  }
  return TRUE;
}

wxDDEServer::~wxDDEServer(void)
{
  if (service_name != "")
  {
    HSZ serviceName = DdeCreateStringHandle(DDEIdInst, (char*) (const char *)service_name, CP_WINANSI);
    if (DdeNameService(DDEIdInst, serviceName, NULL, DNS_UNREGISTER) == 0)
    {
      DDEPrintError();
    }
  }
  wxDDEServerObjects.DeleteObject(this);

  wxNode *node = connections.First();
  while (node)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    wxNode *next = node->Next();
    connection->OnDisconnect(); // May delete the node implicitly
    node = next;
  }

  // If any left after this, delete them
  node = connections.First();
  while (node)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    wxNode *next = node->Next();
    delete connection;
    node = next;
  }
}

wxConnectionBase *wxDDEServer::OnAcceptConnection(const wxString& /* topic */)
{
	return new wxDDEConnection;
}

wxDDEConnection *wxDDEServer::FindConnection(WXHCONV conv)
{
  wxNode *node = connections.First();
  wxDDEConnection *found = NULL;
  while (node && !found)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    if (connection->hConv == conv)
      found = connection;
    else node = node->Next();
  }
  return found;
}

// Only delete the entry in the map, not the actual connection
bool wxDDEServer::DeleteConnection(WXHCONV conv)
{
  wxNode *node = connections.First();
  bool found = FALSE;
  while (node && !found)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    if (connection->hConv == conv)
    {
      found = TRUE;
      delete node;
    }
    else node = node->Next();
  }
  return found;
}


/*
 * Client
 *
 */


wxDDEClient::wxDDEClient(void)
{
  wxDDEClientObjects.Append(this);
}

wxDDEClient::~wxDDEClient(void)
{
  wxDDEClientObjects.DeleteObject(this);
  wxNode *node = connections.First();
  while (node)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    delete connection;  // Deletes the node implicitly (see ~wxDDEConnection)
    node = connections.First();
  }
}

bool wxDDEClient::ValidHost(const wxString& /* host */)
{
  return TRUE;
}

wxConnectionBase *wxDDEClient::MakeConnection(const wxString& /* host */, const wxString& server_name, const wxString& topic)
{
  HSZ serviceName = DdeCreateStringHandle(DDEIdInst, (char*) (const char *)server_name, CP_WINANSI);
  HSZ topic_atom = DdeCreateStringHandle(DDEIdInst, (char*) (const char *)topic, CP_WINANSI);

  HCONV hConv = DdeConnect(DDEIdInst, serviceName, topic_atom, (PCONVCONTEXT)NULL);
  if (hConv == NULL)
    return NULL;
  else
  {
    wxDDEConnection *connection = (wxDDEConnection*) OnMakeConnection();
    if (connection)
    {
      connection->hConv = (WXHCONV) hConv;
      connection->topic_name = topic;
	  connection->client = this;
      connections.Append(connection);
      return connection;
    }
    else return NULL;
  }
}

wxConnectionBase *wxDDEClient::OnMakeConnection(void)
{
	return new wxDDEConnection;
}

wxDDEConnection *wxDDEClient::FindConnection(WXHCONV conv)
{
  wxNode *node = connections.First();
  wxDDEConnection *found = NULL;
  while (node && !found)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    if (connection->hConv == conv)
      found = connection;
    else node = node->Next();
  }
  return found;
}

// Only delete the entry in the map, not the actual connection
bool wxDDEClient::DeleteConnection(WXHCONV conv)
{
  wxNode *node = connections.First();
  bool found = FALSE;
  while (node && !found)
  {
    wxDDEConnection *connection = (wxDDEConnection *)node->Data();
    if (connection->hConv == conv)
    {
      found = TRUE;
      delete node;
    }
    else node = node->Next();
  }
  return found;
}

/*
 * Connection
 */

wxDDEConnection::wxDDEConnection(char *buffer, int size)
{
  if (buffer == NULL)
  {
    if (DDEDefaultIPCBuffer == NULL)
      DDEDefaultIPCBuffer = new char[DDEDefaultIPCBufferSize];
    buf_ptr = DDEDefaultIPCBuffer;
    buf_size = DDEDefaultIPCBufferSize;
  }
  else
  {
    buf_ptr = buffer;
    buf_size = size;
  }

  topic_name = "";

  client = NULL;
  server = NULL;

  hConv = 0;
  sending_data = NULL;
}

wxDDEConnection::wxDDEConnection(void)
{
  hConv = 0;
  sending_data = NULL;
  server = NULL;
  client = NULL;
  if (DDEDefaultIPCBuffer == NULL)
    DDEDefaultIPCBuffer = new char[DDEDefaultIPCBufferSize];

  buf_ptr = DDEDefaultIPCBuffer;
  buf_size = DDEDefaultIPCBufferSize;
  topic_name = "";
}

wxDDEConnection::~wxDDEConnection(void)
{
	if (server)
		server->GetConnections().DeleteObject(this);
	else
	    client->GetConnections().DeleteObject(this);
}

// Calls that CLIENT can make
bool wxDDEConnection::Disconnect(void)
{
  DDEDeleteConnection((HCONV) hConv);
  return (DdeDisconnect((HCONV) hConv) != 0);
}

bool wxDDEConnection::Execute(char *data, int size, int format)
{
  DWORD result;
  if (size < 0)
    size = strlen(data);

  size ++;

  return (DdeClientTransaction((LPBYTE)data, size, (HCONV) hConv,
    NULL, format, XTYP_EXECUTE, 5000, &result) ? TRUE : FALSE);
}

char *wxDDEConnection::Request(const wxString& item, int *size, int format)
{
  DWORD result;
  HSZ atom = DDEGetAtom(item);

  HDDEDATA returned_data = DdeClientTransaction(NULL, 0, (HCONV) hConv,
    atom, format, XTYP_REQUEST, 5000, &result);

  DWORD len = DdeGetData(returned_data, (LPBYTE)(buf_ptr), buf_size, 0);

  DdeFreeDataHandle(returned_data);

  if (size) *size = (int)len;
  if (len > 0)
  {
    return buf_ptr;
  }
  else return NULL;
}

bool wxDDEConnection::Poke(const wxString& item, char *data, int size, int format)
{
  DWORD result;
  if (size < 0)
    size = strlen(data);

  size ++;

  HSZ item_atom = DDEGetAtom(item);
  return (DdeClientTransaction((LPBYTE)data, size, (HCONV) hConv,
    item_atom, format, XTYP_POKE, 5000, &result) ? TRUE : FALSE);
}

bool wxDDEConnection::StartAdvise(const wxString& item)
{
  DWORD result;
  HSZ atom = DDEGetAtom(item);

  return (DdeClientTransaction(NULL, 0, (HCONV) hConv,
    atom, CF_TEXT, XTYP_ADVSTART, 5000, &result) ? TRUE : FALSE);
}

bool wxDDEConnection::StopAdvise(const wxString& item)
{
  DWORD result;
  HSZ atom = DDEGetAtom(item);

  return (DdeClientTransaction(NULL, 0, (HCONV) hConv,
    atom, CF_TEXT, XTYP_ADVSTOP, 5000, &result) ? TRUE : FALSE);
}

// Calls that SERVER can make
bool wxDDEConnection::Advise(const wxString& item, char *data, int size, int format)
{
  if (size < 0)
    size = strlen(data);

  size ++;

  HSZ item_atom = DDEGetAtom(item);
  HSZ topic_atom = DDEGetAtom(topic_name);
  sending_data = data;
  data_size = size;
  data_type = format;
  return (DdePostAdvise(DDEIdInst, topic_atom, item_atom) != 0);
}

bool wxDDEConnection::OnDisconnect(void)
{
	delete this;
	return TRUE;
}


#define DDERETURN HDDEDATA

HDDEDATA EXPENTRY _EXPORT _DDECallback(
WORD wType,
WORD wFmt,
HCONV hConv,
HSZ hsz1,
HSZ hsz2,
HDDEDATA hData,
DWORD /* lData1 */,
DWORD /* lData2 */)
{
  switch (wType)
  {
    case XTYP_CONNECT:
    {
      char topic_buf[100];
      char server_buf[100];
      DdeQueryString(DDEIdInst, hsz1, (LPSTR)topic_buf, sizeof(topic_buf),
                     CP_WINANSI);
      DdeQueryString(DDEIdInst, hsz2, (LPSTR)server_buf, sizeof(topic_buf),
                     CP_WINANSI);
      wxDDEServer *server = DDEFindServer(server_buf);
      if (server)
      {
        wxDDEConnection *connection =
          (wxDDEConnection*) server->OnAcceptConnection(wxString(topic_buf));
        if (connection)
        {
          connection->server = server;
          server->GetConnections().Append(connection);
          connection->hConv = 0;
          connection->topic_name = topic_buf;
          DDECurrentlyConnecting = connection;
          return (DDERETURN)TRUE;
        }
      }
      else return (DDERETURN)0;
      break;
    }

    case XTYP_CONNECT_CONFIRM:
    {
      if (DDECurrentlyConnecting)
      {
        DDECurrentlyConnecting->hConv = (WXHCONV) hConv;
        DDECurrentlyConnecting = NULL;
        return (DDERETURN)TRUE;
      }
      else return 0;
      break;
    }

    case XTYP_DISCONNECT:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);
      if (connection && connection->OnDisconnect())
      {
        DDEDeleteConnection(hConv);  // Delete mapping: hConv => connection
        return (DDERETURN)TRUE;
      }
      else return (DDERETURN)0;
      break;
    }

    case XTYP_EXECUTE:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        DWORD len = DdeGetData(hData, (LPBYTE)(connection->buf_ptr), connection->buf_size, 0);
        DdeFreeDataHandle(hData);
        if (connection->OnExecute(connection->topic_name, connection->buf_ptr, (int)len, wFmt))
          return (DDERETURN)DDE_FACK;
        else
          return (DDERETURN)DDE_FNOTPROCESSED;
      } else return (DDERETURN)DDE_FNOTPROCESSED;
      break;
    }

    case XTYP_REQUEST:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        char item_name[200];
        DdeQueryString(DDEIdInst, hsz2, (LPSTR)item_name, sizeof(item_name),
                     CP_WINANSI);

        int user_size = -1;
        char *data = connection->OnRequest(connection->topic_name, wxString(item_name), &user_size, wFmt);
        if (data)
        {
          if (user_size < 0) user_size = strlen(data);

          HDDEDATA handle = DdeCreateDataHandle(DDEIdInst,
                 (LPBYTE)data, user_size + 1, 0, hsz2, wFmt, 0);
          return (DDERETURN)handle;
        } else return (DDERETURN)0;
      } else return (DDERETURN)0;
      break;
    }

    case XTYP_POKE:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        char item_name[200];
        DdeQueryString(DDEIdInst, hsz2, (LPSTR)item_name, sizeof(item_name),
                     CP_WINANSI);
        DWORD len = DdeGetData(hData, (LPBYTE)(connection->buf_ptr), connection->buf_size, 0);
        DdeFreeDataHandle(hData);
        connection->OnPoke(connection->topic_name, wxString(item_name), connection->buf_ptr, (int)len, wFmt);
        return (DDERETURN)DDE_FACK;
      } else return (DDERETURN)DDE_FNOTPROCESSED;
      break;
    }

    case XTYP_ADVSTART:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        char item_name[200];
        DdeQueryString(DDEIdInst, hsz2, (LPSTR)item_name, sizeof(item_name),
                     CP_WINANSI);

        return (DDERETURN)connection->OnStartAdvise(connection->topic_name, wxString(item_name));
      } else return (DDERETURN)0;
      break;
    }

    case XTYP_ADVSTOP:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        char item_name[200];
        DdeQueryString(DDEIdInst, hsz2, (LPSTR)item_name, sizeof(item_name),
                     CP_WINANSI);
        return (DDERETURN)connection->OnStopAdvise(connection->topic_name, wxString(item_name));
      } else return (DDERETURN)0;
      break;
    }

    case XTYP_ADVREQ:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection && connection->sending_data)
      {
        HDDEDATA data = DdeCreateDataHandle(DDEIdInst,
                          (LPBYTE)connection->sending_data,
                          connection->data_size, 0, hsz2, connection->data_type, 0);
        connection->sending_data = NULL;
        return (DDERETURN)data;
      } else return (DDERETURN)NULL;
      break;
    }

    case XTYP_ADVDATA:
    {
      wxDDEConnection *connection = DDEFindConnection(hConv);

      if (connection)
      {
        char item_name[200];
        DdeQueryString(DDEIdInst, hsz2, (LPSTR)item_name, sizeof(item_name),
                     CP_WINANSI);

        DWORD len = DdeGetData(hData, (LPBYTE)(connection->buf_ptr), connection->buf_size, 0);
        DdeFreeDataHandle(hData);
        if (connection->OnAdvise(connection->topic_name, wxString(item_name), connection->buf_ptr, (int)len, wFmt))
          return (DDERETURN)DDE_FACK;
        else
          return (DDERETURN)DDE_FNOTPROCESSED;
      } else return (DDERETURN)DDE_FNOTPROCESSED;
      break;
    }
  }
  return 0;
}

// Atom table stuff
static HSZ DDEAddAtom(const wxString& string)
{
  HSZ atom = DdeCreateStringHandle(DDEIdInst, (char*) (const char *)string, CP_WINANSI);
  wxAtomTable.Append(string, (wxObject *)atom);
  return atom;
}

static HSZ DDEGetAtom(const wxString& string)
{
  wxNode *node = wxAtomTable.Find(string);
  if (node)
    return (HSZ)node->Data();
  else
  {
    DDEAddAtom(string);
    return (HSZ)(wxAtomTable.Find(string)->Data());
  }
}

void DDEPrintError(void)
{
  char *err = NULL;
  switch (DdeGetLastError(DDEIdInst))
  {
    case DMLERR_ADVACKTIMEOUT:
      err = "A request for a synchronous advise transaction has timed out.";
      break;
    case DMLERR_BUSY:
      err = "The response to the transaction caused the DDE_FBUSY bit to be set.";
      break;
    case DMLERR_DATAACKTIMEOUT:
      err = "A request for a synchronous data transaction has timed out.";
      break;
    case DMLERR_DLL_NOT_INITIALIZED:
      err = "A DDEML function was called without first calling the DdeInitialize function,\n\ror an invalid instance identifier\n\rwas passed to a DDEML function.";
      break;
    case DMLERR_DLL_USAGE:
      err = "An application initialized as APPCLASS_MONITOR has\n\rattempted to perform a DDE transaction,\n\ror an application initialized as APPCMD_CLIENTONLY has \n\rattempted to perform server transactions.";
      break;
    case DMLERR_EXECACKTIMEOUT:
      err = "A request for a synchronous execute transaction has timed out.";
      break;
    case DMLERR_INVALIDPARAMETER:
      err = "A parameter failed to be validated by the DDEML.";
      break;
    case DMLERR_LOW_MEMORY:
      err = "A DDEML application has created a prolonged race condition.";
      break;
    case DMLERR_MEMORY_ERROR:
      err = "A memory allocation failed.";
      break;
    case DMLERR_NO_CONV_ESTABLISHED:
      err = "A client's attempt to establish a conversation has failed.";
      break;
    case DMLERR_NOTPROCESSED:
      err = "A transaction failed.";
      break;
    case DMLERR_POKEACKTIMEOUT:
      err = "A request for a synchronous poke transaction has timed out.";
      break;
    case DMLERR_POSTMSG_FAILED:
      err = "An internal call to the PostMessage function has failed. ";
      break;
    case DMLERR_REENTRANCY:
      err = "Reentrancy problem.";
      break;
    case DMLERR_SERVER_DIED:
      err = "A server-side transaction was attempted on a conversation\n\rthat was terminated by the client, or the server\n\rterminated before completing a transaction.";
      break;
    case DMLERR_SYS_ERROR:
      err = "An internal error has occurred in the DDEML.";
      break;
    case DMLERR_UNADVACKTIMEOUT:
      err = "A request to end an advise transaction has timed out.";
      break;
    case DMLERR_UNFOUND_QUEUE_ID:
      err = "An invalid transaction identifier was passed to a DDEML function.\n\rOnce the application has returned from an XTYP_XACT_COMPLETE callback,\n\rthe transaction identifier for that callback is no longer valid.";
      break;
    default:
      err = "Unrecognised error type.";
      break;
  }
  MessageBox(NULL, (LPCSTR)err, "DDE Error", MB_OK | MB_ICONINFORMATION);
}

#endif
  // USE_IPC
