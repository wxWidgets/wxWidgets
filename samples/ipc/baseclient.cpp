///////////////////////////////////////////////////////////////////////////////
// Name:        samples/ipc/baseclient.cpp
// Purpose:     IPC sample: console client
// Author:      Anders Larsen
//              Most of the code was stolen from: samples/ipc/client.cpp
//              (c) Julian Smart, Jurgen Doornik
// Created:     2007-11-08
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Anders Larsen
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Settings common to both executables: determines whether
// we're using TCP/IP or real DDE.
#include "ipcsetup.h"

#include "connection.h"

#include "wx/timer.h"
#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------


// Define a new application
class MyClient;

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

protected:
    MyClient         *m_client;
};

class MyConnection : public MyConnectionBase
{
public:
    virtual bool DoExecute(const void *data, size_t size, wxIPCFormat format);
    virtual const void *Request(const wxString& item, size_t *size = NULL, wxIPCFormat format = wxIPC_TEXT);
    virtual bool DoPoke(const wxString& item, const void* data, size_t size, wxIPCFormat format);
    virtual bool OnAdvise(const wxString& topic, const wxString& item, const void *data, size_t size, wxIPCFormat format);
    virtual bool OnDisconnect();
};

class MyClient: public wxClient, public wxTimer
{
public:
    MyClient();
    virtual ~MyClient();
    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();
    wxConnectionBase *OnMakeConnection();
    bool IsConnected() { return m_connection != NULL; };
    virtual void Notify();

protected:
    MyConnection     *m_connection;
    int              m_step;
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    delete wxLog::SetActiveTarget(new wxLogStderr);

    // Create a new client
    m_client = new MyClient;
    bool retval = m_client->Connect("localhost", "4242", "IPC TEST");

    wxLogMessage(_T("Client host=\"localhost\" port=\"4242\" topic=\"IPC TEST\" %s"),
        retval ? _T("connected") : _T("failed to connect"));

    return retval;
}

int MyApp::OnExit()
{
    delete m_client;

    return 0;
}

// ----------------------------------------------------------------------------
// MyClient
// ----------------------------------------------------------------------------

MyClient::MyClient() : wxClient()
{
    m_connection = NULL;
    m_step = 0;
}

bool MyClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    // suppress the log messages from MakeConnection()
    wxLogNull nolog;

    m_connection = (MyConnection *)MakeConnection(sHost, sService, sTopic);
    if (m_connection)
        Start(1000, false);
    return m_connection != NULL;
}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyConnection;
}

void MyClient::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
        wxLogMessage(_T("Client disconnected from server"));
    }
    wxGetApp().ExitMainLoop();
}

MyClient::~MyClient()
{
    Disconnect();
}

void MyClient::Notify()
{
    switch (m_step++)
    {
        case 0:
        {
            size_t size;
            m_connection->Request(_T("Date"));
            m_connection->Request(_T("Date+len"), &size);
            m_connection->Request(_T("bytes[3]"), &size, wxIPC_PRIVATE);
            break;
        }
        case 1:
        {
            wxString s = wxDateTime::Now().Format();
            m_connection->Poke(_T("Date"), s);
            s = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
            m_connection->Poke(_T("Date"), (const char *)s.c_str(), s.length() + 1);
            char bytes[3];
            bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
            m_connection->Poke(_T("bytes[3]"), bytes, 3, wxIPC_PRIVATE);
            break;
        }
        case 2:
        {
            wxString s = _T("Date");
            m_connection->Execute(s);
            m_connection->Execute((const char *)s.c_str(), s.length() + 1);
            char bytes[3];
            bytes[0] = '1';
            bytes[1] = '2';
            bytes[2] = '3';
            m_connection->Execute(bytes, WXSIZEOF(bytes));
            break;
        }
        case 3:
            wxLogMessage(_T("StartAdvise(\"something\")"));
            m_connection->StartAdvise(_T("something"));
            break;
        case 10:
            wxLogMessage(_T("StopAdvise(\"something\")"));
            m_connection->StopAdvise(_T("something"));
            break;
        case 15:
            Disconnect();
            break;
    }
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, const void *data,
    size_t size, wxIPCFormat format)
{
    Log(_T("OnAdvise"), topic, item, data, size, format);
    return true;
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage(_T("OnDisconnect()"));
    wxGetApp().ExitMainLoop();
    return true;
}

bool MyConnection::DoExecute(const void *data, size_t size, wxIPCFormat format)
{
    Log(_T("Execute"), wxEmptyString, wxEmptyString, data, size, format);
    bool retval = wxConnection::DoExecute(data, size, format);
    if (!retval)
        wxLogMessage(_T("Execute failed!"));
    return retval;
}

const void *MyConnection::Request(const wxString& item, size_t *size, wxIPCFormat format)
{
    const void *data =  wxConnection::Request(item, size, format);
    Log(_T("Request"), wxEmptyString, item, data, size ? *size : wxNO_LEN, format);
    return data;
}

bool MyConnection::DoPoke(const wxString& item, const void *data, size_t size, wxIPCFormat format)
{
    Log(_T("Poke"), wxEmptyString, item, data, size, format);
    return wxConnection::DoPoke(item, data, size, format);
}
