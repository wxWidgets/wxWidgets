/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dde.cpp
// Purpose:     DDE classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dde.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_IPC

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/module.h"
#include "wx/dde.h"
#include "wx/intl.h"


#include "wx/msw/private.h"

#include <string.h>
#include <windows.h>
#include <ddeml.h>

#if defined(__TWIN32__) || defined(__GNUWIN32_OLD__)
    #include "wx/msw/gnuwin32/extra.h"
#endif

// some compilers headers don't define this one (mingw32)
#ifndef DMLERR_NO_ERROR
    #define DMLERR_NO_ERROR (0)

    // this one is also missing from some mingw32 headers, but there is no way
    // to test for it (I know of) - the test for DMLERR_NO_ERROR works for me,
    // but is surely not the right thing to do
    extern "C"
    HDDEDATA STDCALL DdeClientTransaction(LPBYTE pData,
                                          DWORD cbData,
                                          HCONV hConv,
                                          HSZ hszItem,
                                          UINT wFmt,
                                          UINT wType,
                                          DWORD dwTimeout,
                                          LPDWORD pdwResult);
#endif // no DMLERR_NO_ERROR

// ----------------------------------------------------------------------------
// macros and constants
// ----------------------------------------------------------------------------

#ifdef __WIN32__
    #define _EXPORT
#else
    #define _EXPORT _export
#endif

#if wxUSE_UNICODE
    #define DDE_CP      CP_WINUNICODE
#else
    #define DDE_CP      CP_WINANSI
#endif

#define GetHConv()       ((HCONV)m_hConv)

// default timeout for DDE operations (5sec)
#define DDE_TIMEOUT     5000

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDDEConnection *DDEFindConnection(HCONV hConv);
static void DDEDeleteConnection(HCONV hConv);
static wxDDEServer *DDEFindServer(const wxString& s);

extern "C" HDDEDATA EXPENTRY _EXPORT _DDECallback(WORD wType,
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

// string handles
static HSZ DDEAtomFromString(const wxString& s);
static wxString DDEStringFromAtom(HSZ hsz);

// error handling
static wxString DDEGetErrorMsg(UINT error);
static void DDELogError(const wxString& s, UINT error = DMLERR_NO_ERROR);

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static DWORD DDEIdInst = 0L;
static wxDDEConnection *DDECurrentlyConnecting = NULL;

static wxList wxAtomTable(wxKEY_STRING);
static wxList wxDDEClientObjects;
static wxList wxDDEServerObjects;

char *DDEDefaultIPCBuffer = NULL;
int DDEDefaultIPCBufferSize = 0;
static bool DDEInitialized = FALSE;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// A module to allow DDE cleanup without calling these functions
// from app.cpp or from the user's application.

class wxDDEModule : public wxModule
{
public:
    wxDDEModule() {}
    bool OnInit() { return TRUE; }
    void OnExit() { wxDDECleanUp(); }

private:
    DECLARE_DYNAMIC_CLASS(wxDDEModule)
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDDEServer, wxServerBase)
IMPLEMENT_DYNAMIC_CLASS(wxDDEClient, wxClientBase)
IMPLEMENT_CLASS(wxDDEConnection, wxConnectionBase)
IMPLEMENT_DYNAMIC_CLASS(wxDDEModule, wxModule)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization and cleanup
// ----------------------------------------------------------------------------

extern void wxDDEInitialize()
{
    if ( !DDEInitialized )
    {
        // Should insert filter flags
        PFNCALLBACK callback = (PFNCALLBACK)
            MakeProcInstance((FARPROC)_DDECallback, wxGetInstance());
        UINT rc = DdeInitialize(&DDEIdInst, callback, APPCLASS_STANDARD, 0L);
        if ( rc != DMLERR_NO_ERROR )
        {
            DDELogError(_T("Failed to initialize DDE"), rc);
        }
        else
        {
            DDEInitialized = TRUE;
        }
    }
}

void wxDDECleanUp()
{
    if ( DDEIdInst != 0 )
    {
        DdeUninitialize(DDEIdInst);
        DDEIdInst = 0;
    }

    delete [] DDEDefaultIPCBuffer;
}

// ----------------------------------------------------------------------------
// functions working with the global connection list(s)
// ----------------------------------------------------------------------------

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

  node = wxDDEClientObjects.First();
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

// ----------------------------------------------------------------------------
// wxDDEServer
// ----------------------------------------------------------------------------

wxDDEServer::wxDDEServer()
{
    wxDDEInitialize();

    wxDDEServerObjects.Append(this);
}

bool wxDDEServer::Create(const wxString& server)
{
    m_serviceName = server;

    if ( !DdeNameService(DDEIdInst, DDEAtomFromString(server), (HSZ)NULL, DNS_REGISTER) )
    {
        DDELogError(wxString::Format(_("Failed to register DDE server '%s'"),
                                     server.c_str()));

        return FALSE;
    }

    return TRUE;
}

wxDDEServer::~wxDDEServer()
{
    if ( !!m_serviceName )
    {
        if ( !DdeNameService(DDEIdInst, DDEAtomFromString(m_serviceName),
                             (HSZ)NULL, DNS_UNREGISTER) )
        {
            DDELogError(wxString::Format(_("Failed to unregister DDE server '%s'"),
                                         m_serviceName.c_str()));
        }
    }

    wxDDEServerObjects.DeleteObject(this);

    wxNode *node = m_connections.First();
    while (node)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        wxNode *next = node->Next();
        connection->OnDisconnect(); // May delete the node implicitly
        node = next;
    }

    // If any left after this, delete them
    node = m_connections.First();
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
    wxNode *node = m_connections.First();
    wxDDEConnection *found = NULL;
    while (node && !found)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        if (connection->m_hConv == conv)
            found = connection;
        else node = node->Next();
    }
    return found;
}

// Only delete the entry in the map, not the actual connection
bool wxDDEServer::DeleteConnection(WXHCONV conv)
{
    wxNode *node = m_connections.First();
    bool found = FALSE;
    while (node && !found)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        if (connection->m_hConv == conv)
        {
            found = TRUE;
            delete node;
        }
        else node = node->Next();
    }
    return found;
}

// ----------------------------------------------------------------------------
// wxDDEClient
// ----------------------------------------------------------------------------

wxDDEClient::wxDDEClient()
{
    wxDDEInitialize();

    wxDDEClientObjects.Append(this);
}

wxDDEClient::~wxDDEClient()
{
    wxDDEClientObjects.DeleteObject(this);
    wxNode *node = m_connections.First();
    while (node)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        delete connection;  // Deletes the node implicitly (see ~wxDDEConnection)
        node = m_connections.First();
    }
}

bool wxDDEClient::ValidHost(const wxString& /* host */)
{
    return TRUE;
}

wxConnectionBase *wxDDEClient::MakeConnection(const wxString& WXUNUSED(host),
                                              const wxString& server,
                                              const wxString& topic)
{
    HCONV hConv = DdeConnect(DDEIdInst, DDEAtomFromString(server), DDEAtomFromString(topic),
                             (PCONVCONTEXT)NULL);
    if ( !hConv )
    {
        DDELogError(wxString::Format(_("Failed to create connection to server '%s' on topic '%s'"),
                                     server.c_str(), topic.c_str()));
    }
    else
    {
        wxDDEConnection *connection = (wxDDEConnection*) OnMakeConnection();
        if (connection)
        {
            connection->m_hConv = (WXHCONV) hConv;
            connection->m_topicName = topic;
            connection->m_client = this;
            m_connections.Append(connection);
            return connection;
        }
    }

    return (wxConnectionBase*) NULL;
}

wxConnectionBase *wxDDEClient::OnMakeConnection()
{
    return new wxDDEConnection;
}

wxDDEConnection *wxDDEClient::FindConnection(WXHCONV conv)
{
    wxNode *node = m_connections.First();
    wxDDEConnection *found = NULL;
    while (node && !found)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        if (connection->m_hConv == conv)
            found = connection;
        else node = node->Next();
    }
    return found;
}

// Only delete the entry in the map, not the actual connection
bool wxDDEClient::DeleteConnection(WXHCONV conv)
{
    wxNode *node = m_connections.First();
    bool found = FALSE;
    while (node && !found)
    {
        wxDDEConnection *connection = (wxDDEConnection *)node->Data();
        if (connection->m_hConv == conv)
        {
            found = TRUE;
            delete node;
        }
        else node = node->Next();
    }
    return found;
}

// ----------------------------------------------------------------------------
// wxDDEConnection
// ----------------------------------------------------------------------------

wxDDEConnection::wxDDEConnection(char *buffer, int size)
{
    if (buffer == NULL)
    {
        if (DDEDefaultIPCBuffer == NULL)
            DDEDefaultIPCBuffer = new char[DDEDefaultIPCBufferSize];
        m_bufPtr = DDEDefaultIPCBuffer;
        m_bufSize = DDEDefaultIPCBufferSize;
    }
    else
    {
        m_bufPtr = buffer;
        m_bufSize = size;
    }

    m_client = NULL;
    m_server = NULL;

    m_hConv = 0;
    m_sendingData = NULL;
}

wxDDEConnection::wxDDEConnection()
{
    m_hConv = 0;
    m_sendingData = NULL;
    m_server = NULL;
    m_client = NULL;
    if (DDEDefaultIPCBuffer == NULL)
        DDEDefaultIPCBuffer = new char[DDEDefaultIPCBufferSize];

    m_bufPtr = DDEDefaultIPCBuffer;
    m_bufSize = DDEDefaultIPCBufferSize;
}

wxDDEConnection::~wxDDEConnection()
{
    if (m_server)
        m_server->GetConnections().DeleteObject(this);
    else
        m_client->GetConnections().DeleteObject(this);
}

// Calls that CLIENT can make
bool wxDDEConnection::Disconnect()
{
    DDEDeleteConnection(GetHConv());

    bool ok = DdeDisconnect(GetHConv()) != 0;
    if ( !ok )
    {
        DDELogError(_T("Failed to disconnect from DDE server gracefully"));
    }

    return ok;
}

bool wxDDEConnection::Execute(const wxChar *data, int size, wxIPCFormat format)
{
    DWORD result;
    if (size < 0)
    {
        size = wxStrlen(data) + 1;
    }

    bool ok = DdeClientTransaction((LPBYTE)data, size,
                                    GetHConv(),
                                    NULL,
                                    format,
                                    XTYP_EXECUTE,
                                    DDE_TIMEOUT,
                                    &result) != 0;
    if ( !ok )
    {
        DDELogError(_T("DDE execute request failed"));
    }

    return ok;
}

char *wxDDEConnection::Request(const wxString& item, int *size, wxIPCFormat format)
{
    DWORD result;
    HSZ atom = DDEGetAtom(item);

    HDDEDATA returned_data = DdeClientTransaction(NULL, 0,
                                                  GetHConv(),
                                                  atom, format,
                                                  XTYP_REQUEST,
                                                  DDE_TIMEOUT,
                                                  &result);
    if ( !returned_data )
    {
        DDELogError(_T("DDE data request failed"));

        return NULL;
    }

    DWORD len = DdeGetData(returned_data, (LPBYTE)m_bufPtr, m_bufSize, 0);

    DdeFreeDataHandle(returned_data);

    if (size)
        *size = (int)len;

    return m_bufPtr;
}

bool wxDDEConnection::Poke(const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    DWORD result;
    if (size < 0)
    {
        size = wxStrlen(data) + 1;
    }

    HSZ item_atom = DDEGetAtom(item);
    bool ok = DdeClientTransaction((LPBYTE)data, size,
                                   GetHConv(),
                                   item_atom, format,
                                   XTYP_POKE,
                                   DDE_TIMEOUT,
                                   &result) != 0;
    if ( !ok )
    {
        DDELogError(_("DDE poke request failed"));
    }

    return ok;
}

bool wxDDEConnection::StartAdvise(const wxString& item)
{
    DWORD result;
    HSZ atom = DDEGetAtom(item);

    bool ok = DdeClientTransaction(NULL, 0,
                                   GetHConv(),
                                   atom, CF_TEXT,
                                   XTYP_ADVSTART,
                                   DDE_TIMEOUT,
                                   &result) != 0;
    if ( !ok )
    {
        DDELogError(_("Failed to establish an advise loop with DDE server"));
    }

    return ok;
}

bool wxDDEConnection::StopAdvise(const wxString& item)
{
    DWORD result;
    HSZ atom = DDEGetAtom(item);

    bool ok = DdeClientTransaction(NULL, 0,
                                   GetHConv(),
                                   atom, CF_TEXT,
                                   XTYP_ADVSTOP,
                                   DDE_TIMEOUT,
                                   &result) != 0;
    if ( !ok )
    {
        DDELogError(_("Failed to terminate the advise loop with DDE server"));
    }

    return ok;
}

// Calls that SERVER can make
bool wxDDEConnection::Advise(const wxString& item,
                             wxChar *data,
                             int size,
                             wxIPCFormat format)
{
    if (size < 0)
    {
        size = wxStrlen(data) + 1;
    }

    HSZ item_atom = DDEGetAtom(item);
    HSZ topic_atom = DDEGetAtom(m_topicName);
    m_sendingData = data;
    m_dataSize = size;
    m_dataType = format;

    bool ok = DdePostAdvise(DDEIdInst, topic_atom, item_atom) != 0;
    if ( !ok )
    {
        DDELogError(_("Failed to send DDE advise notification"));
    }

    return ok;
}

bool wxDDEConnection::OnDisconnect()
{
    delete this;
    return TRUE;
}

// ----------------------------------------------------------------------------
// _DDECallback
// ----------------------------------------------------------------------------

#define DDERETURN HDDEDATA

HDDEDATA EXPENTRY _EXPORT
_DDECallback(WORD wType,
             WORD wFmt,
             HCONV hConv,
             HSZ hsz1,
             HSZ hsz2,
             HDDEDATA hData,
             DWORD WXUNUSED(lData1),
             DWORD WXUNUSED(lData2))
{
    switch (wType)
    {
        case XTYP_CONNECT:
            {
                wxString topic = DDEStringFromAtom(hsz1),
                         srv = DDEStringFromAtom(hsz2);
                wxDDEServer *server = DDEFindServer(srv);
                if (server)
                {
                    wxDDEConnection *connection =
                        (wxDDEConnection*) server->OnAcceptConnection(topic);
                    if (connection)
                    {
                        connection->m_server = server;
                        server->GetConnections().Append(connection);
                        connection->m_hConv = 0;
                        connection->m_topicName = topic;
                        DDECurrentlyConnecting = connection;
                        return (DDERETURN)(DWORD)TRUE;
                    }
                }
                break;
            }

        case XTYP_CONNECT_CONFIRM:
            {
                if (DDECurrentlyConnecting)
                {
                    DDECurrentlyConnecting->m_hConv = (WXHCONV) hConv;
                    DDECurrentlyConnecting = NULL;
                    return (DDERETURN)(DWORD)TRUE;
                }
                break;
            }

        case XTYP_DISCONNECT:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);
                if (connection && connection->OnDisconnect())
                {
                    DDEDeleteConnection(hConv);  // Delete mapping: hConv => connection
                    return (DDERETURN)(DWORD)TRUE;
                }
                break;
            }

        case XTYP_EXECUTE:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    DWORD len = DdeGetData(hData,
                                           (LPBYTE)connection->m_bufPtr,
                                           connection->m_bufSize,
                                           0);
                    DdeFreeDataHandle(hData);
                    if ( connection->OnExecute(connection->m_topicName,
                                               connection->m_bufPtr,
                                               (int)len,
                                               (wxIPCFormat) wFmt) )
                    {
                        return (DDERETURN)(DWORD)DDE_FACK;
                    }
                }

                return (DDERETURN)DDE_FNOTPROCESSED;
            }

        case XTYP_REQUEST:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    wxString item_name = DDEStringFromAtom(hsz2);

                    int user_size = -1;
                    char *data = connection->OnRequest(connection->m_topicName,
                                                       item_name,
                                                       &user_size,
                                                       (wxIPCFormat) wFmt);
                    if (data)
                    {
                        if (user_size < 0)
                            user_size = wxStrlen(data) + 1;

                        HDDEDATA handle = DdeCreateDataHandle(DDEIdInst,
                                                              (LPBYTE)data,
                                                              user_size,
                                                              0,
                                                              hsz2,
                                                              wFmt,
                                                              0);
                        return (DDERETURN)handle;
                    }
                }
                break;
            }

        case XTYP_POKE:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    wxString item_name = DDEStringFromAtom(hsz2);

                    DWORD len = DdeGetData(hData,
                                           (LPBYTE)connection->m_bufPtr,
                                           connection->m_bufSize,
                                           0);
                    DdeFreeDataHandle(hData);

                    connection->OnPoke(connection->m_topicName,
                                       item_name,
                                       connection->m_bufPtr,
                                       (int)len,
                                       (wxIPCFormat) wFmt);

                    return (DDERETURN)DDE_FACK;
                }
                else
                {
                    return (DDERETURN)DDE_FNOTPROCESSED;
                }
            }

        case XTYP_ADVSTART:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    wxString item_name = DDEStringFromAtom(hsz2);

                    return (DDERETURN)connection->
                                OnStartAdvise(connection->m_topicName, item_name);
                }

                break;
            }

        case XTYP_ADVSTOP:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    wxString item_name = DDEStringFromAtom(hsz2);

                    return (DDERETURN)connection->
                        OnStopAdvise(connection->m_topicName, item_name);
                }

                break;
            }

        case XTYP_ADVREQ:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection && connection->m_sendingData)
                {
                    HDDEDATA data = DdeCreateDataHandle
                                    (
                                        DDEIdInst,
                                        (LPBYTE)connection->m_sendingData,
                                        connection->m_dataSize,
                                        0,
                                        hsz2,
                                        connection->m_dataType,
                                        0
                                    );

                    connection->m_sendingData = NULL;

                    return (DDERETURN)data;
                }

                break;
            }

        case XTYP_ADVDATA:
            {
                wxDDEConnection *connection = DDEFindConnection(hConv);

                if (connection)
                {
                    wxString item_name = DDEStringFromAtom(hsz2);

                    DWORD len = DdeGetData(hData,
                                           (LPBYTE)connection->m_bufPtr,
                                           connection->m_bufSize,
                                           0);
                    DdeFreeDataHandle(hData);
                    if ( connection->OnAdvise(connection->m_topicName,
                                              item_name,
                                              connection->m_bufPtr,
                                              (int)len,
                                              (wxIPCFormat) wFmt) )
                    {
                        return (DDERETURN)(DWORD)DDE_FACK;
                    }
                }

                return (DDERETURN)DDE_FNOTPROCESSED;
            }
    }

    return (DDERETURN)0;
}

// ----------------------------------------------------------------------------
// DDE strings and atoms
// ----------------------------------------------------------------------------

// Atom table stuff
static HSZ DDEAddAtom(const wxString& string)
{
    HSZ atom = DDEAtomFromString(string);
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

// atom <-> strings
static HSZ DDEAtomFromString(const wxString& s)
{
    wxASSERT_MSG( DDEIdInst, _T("DDE not initialized") );

    HSZ hsz = DdeCreateStringHandle(DDEIdInst, (char*) s.c_str(), DDE_CP);
    if ( !hsz )
    {
        DDELogError(_("Failed to create DDE string"));
    }

    return hsz;
}

static wxString DDEStringFromAtom(HSZ hsz)
{
    // all DDE strings are normally limited to 255 bytes
    static const size_t len = 256;

    wxString s;
    (void)DdeQueryString(DDEIdInst, hsz, s.GetWriteBuf(len), len, DDE_CP);
    s.UngetWriteBuf();

    return s;
}

// ----------------------------------------------------------------------------
// error handling
// ----------------------------------------------------------------------------

static void DDELogError(const wxString& s, UINT error)
{
    if ( !error )
    {
        error = DdeGetLastError(DDEIdInst);
    }

    wxLogError(s + _T(": ") + DDEGetErrorMsg(error));
}

static wxString DDEGetErrorMsg(UINT error)
{
    wxString err;
    switch ( error )
    {
        case DMLERR_NO_ERROR:
            err = _("no DDE error.");
            break;

        case DMLERR_ADVACKTIMEOUT:
            err = _("a request for a synchronous advise transaction has timed out.");
            break;
        case DMLERR_BUSY:
            err = _("the response to the transaction caused the DDE_FBUSY bit to be set.");
            break;
        case DMLERR_DATAACKTIMEOUT:
            err = _("a request for a synchronous data transaction has timed out.");
            break;
        case DMLERR_DLL_NOT_INITIALIZED:
            err = _("a DDEML function was called without first calling the DdeInitialize function,\nor an invalid instance identifier\nwas passed to a DDEML function.");
            break;
        case DMLERR_DLL_USAGE:
            err = _("an application initialized as APPCLASS_MONITOR has\nattempted to perform a DDE transaction,\nor an application initialized as APPCMD_CLIENTONLY has \nattempted to perform server transactions.");
            break;
        case DMLERR_EXECACKTIMEOUT:
            err = _("a request for a synchronous execute transaction has timed out.");
            break;
        case DMLERR_INVALIDPARAMETER:
            err = _("a parameter failed to be validated by the DDEML.");
            break;
        case DMLERR_LOW_MEMORY:
            err = _("a DDEML application has created a prolonged race condition.");
            break;
        case DMLERR_MEMORY_ERROR:
            err = _("a memory allocation failed.");
            break;
        case DMLERR_NO_CONV_ESTABLISHED:
            err = _("a client's attempt to establish a conversation has failed.");
            break;
        case DMLERR_NOTPROCESSED:
            err = _("a transaction failed.");
            break;
        case DMLERR_POKEACKTIMEOUT:
            err = _("a request for a synchronous poke transaction has timed out.");
            break;
        case DMLERR_POSTMSG_FAILED:
            err = _("an internal call to the PostMessage function has failed. ");
            break;
        case DMLERR_REENTRANCY:
            err = _("reentrancy problem.");
            break;
        case DMLERR_SERVER_DIED:
            err = _("a server-side transaction was attempted on a conversation\nthat was terminated by the client, or the server\nterminated before completing a transaction.");
            break;
        case DMLERR_SYS_ERROR:
            err = _("an internal error has occurred in the DDEML.");
            break;
        case DMLERR_UNADVACKTIMEOUT:
            err = _("a request to end an advise transaction has timed out.");
            break;
        case DMLERR_UNFOUND_QUEUE_ID:
            err = _("an invalid transaction identifier was passed to a DDEML function.\nOnce the application has returned from an XTYP_XACT_COMPLETE callback,\nthe transaction identifier for that callback is no longer valid.");
            break;
        default:
            err.Printf(_("Unknown DDE error %08x"), error);
    }

    return err;
}

#endif
  // wxUSE_IPC
