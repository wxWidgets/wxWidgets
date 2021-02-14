///////////////////////////////////////////////////////////////////////////////
// Name:        samples/ipc/baseclient.cpp
// Purpose:     IPC sample: console client
// Author:      Anders Larsen
//              Most of the code was stolen from: samples/ipc/client.cpp
//              (c) Julian Smart, Jurgen Doornik
// Created:     2007-11-08
// Copyright:   (c) 2007 Anders Larsen
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Settings common to both executables: determines whether
// we're using TCP/IP or real DDE.
#include "ipcsetup.h"

#include "connection.h"

#include "wx/timer.h"
#include "wx/datetime.h"
#include "wx/vector.h"

class MyClient;

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    MyApp() { Bind(wxEVT_IDLE, &MyApp::OnIdle, this); }

    virtual bool OnInit() wxOVERRIDE;
    virtual int OnExit() wxOVERRIDE;

private:
    void OnIdle(wxIdleEvent& event);

    MyClient *m_client;
};

class MyConnection : public MyConnectionBase
{
public:
    virtual bool DoExecute(const void *data, size_t size, wxIPCFormat format) wxOVERRIDE;
    virtual const void *Request(const wxString& item, size_t *size = NULL, wxIPCFormat format = wxIPC_TEXT) wxOVERRIDE;
    virtual bool DoPoke(const wxString& item, const void* data, size_t size, wxIPCFormat format) wxOVERRIDE;
    virtual bool OnAdvise(const wxString& topic, const wxString& item, const void *data, size_t size, wxIPCFormat format) wxOVERRIDE;
    virtual bool OnDisconnect() wxOVERRIDE;
};

class MyClient : public wxClient,
                 private wxTimer
{
public:
    MyClient();
    virtual ~MyClient();

    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();
    wxConnectionBase *OnMakeConnection() wxOVERRIDE;
    bool IsConnected() { return m_connection != NULL; }

    virtual void Notify() wxOVERRIDE;

    void StartNextTestIfNecessary();

private:
    void TestRequest();
    void TestPoke();
    void TestExecute();
    void TestStartAdvise();
    void TestStopAdvise();
    void TestDisconnect();


    MyConnection *m_connection;

    // the test functions to be executed by StartNextTestIfNecessary()
    typedef void (MyClient::*MyClientTestFunc)();
    wxVector<MyClientTestFunc> m_tests;

    // number of seconds since the start of the test
    int m_step;
};

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_APP_CONSOLE(MyApp);

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create a new client
    m_client = new MyClient;
    bool retval = m_client->Connect("localhost", "4242", "IPC TEST");

    wxLogMessage("Client host=\"localhost\" port=\"4242\" topic=\"IPC TEST\" %s",
                 retval ? "connected" : "failed to connect");

    return retval;
}

int MyApp::OnExit()
{
    delete m_client;

    return 0;
}

void MyApp::OnIdle(wxIdleEvent& event)
{
    if ( m_client )
        m_client->StartNextTestIfNecessary();

    event.Skip();
}

// ----------------------------------------------------------------------------
// MyClient
// ----------------------------------------------------------------------------

MyClient::MyClient()
    : wxClient()
{
    m_connection = NULL;
    m_step = 0;
}

bool
MyClient::Connect(const wxString& sHost,
                  const wxString& sService,
                  const wxString& sTopic)
{
    // suppress the log messages from MakeConnection()
    wxLogNull nolog;

    m_connection = (MyConnection *)MakeConnection(sHost, sService, sTopic);
    if ( !m_connection )
        return false;

    Start(1000);

    return true;
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
        wxDELETE(m_connection);
        wxLogMessage("Client disconnected from server");
    }
    wxGetApp().ExitMainLoop();
}

MyClient::~MyClient()
{
    Disconnect();
}

void MyClient::Notify()
{
    // we shouldn't call wxIPC methods from here directly as we may be called
    // from inside an IPC call when using TCP/IP as the sockets are used in
    // non-blocking code and so can dispatch events, including the timer ones,
    // while waiting for IO and so starting another IPC call would result in
    // fatal reentrancies -- instead, just set a flag and perform the test
    // indicated by it later from our idle event handler
    MyClientTestFunc testfunc = NULL;
    switch ( m_step++ )
    {
        case 0:
            testfunc = &MyClient::TestRequest;
            break;

        case 1:
            testfunc = &MyClient::TestPoke;
            break;

        case 2:
            testfunc = &MyClient::TestExecute;
            break;

        case 3:
            testfunc = &MyClient::TestStartAdvise;
            break;

        case 10:
            testfunc = &MyClient::TestStopAdvise;
            break;

        case 15:
            testfunc = &MyClient::TestDisconnect;
            // We don't need the timer any more, we're going to exit soon.
            Stop();
            break;

        default:
            // No need to wake up idle handling.
            return;
    }

    m_tests.push_back(testfunc);

    wxWakeUpIdle();
}

void MyClient::StartNextTestIfNecessary()
{
    while ( !m_tests.empty() )
    {
        MyClientTestFunc testfunc = m_tests.front();
        m_tests.erase(m_tests.begin());
        (this->*testfunc)();
    }
}

void MyClient::TestRequest()
{
    size_t size;
    m_connection->Request("Date");
    m_connection->Request("Date+len", &size);
    m_connection->Request("bytes[3]", &size, wxIPC_PRIVATE);
}

void MyClient::TestPoke()
{
    wxString s = wxDateTime::Now().Format();
    m_connection->Poke("Date", s);
    s = wxDateTime::Now().FormatTime() + " " + wxDateTime::Now().FormatDate();
    m_connection->Poke("Date", (const char *)s.c_str(), s.length() + 1);
    char bytes[3];
    bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
    m_connection->Poke("bytes[3]", bytes, 3, wxIPC_PRIVATE);
}

void MyClient::TestExecute()
{
    wxString s = "Date";
    m_connection->Execute(s);
    m_connection->Execute((const char *)s.c_str(), s.length() + 1);
    char bytes[3];
    bytes[0] = '1';
    bytes[1] = '2';
    bytes[2] = '3';
    m_connection->Execute(bytes, WXSIZEOF(bytes));
}

void MyClient::TestStartAdvise()
{
    wxLogMessage("StartAdvise(\"something\")");
    m_connection->StartAdvise("something");
}

void MyClient::TestStopAdvise()
{
    wxLogMessage("StopAdvise(\"something\")");
    m_connection->StopAdvise("something");
}

void MyClient::TestDisconnect()
{
    Disconnect();
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, const void *data,
    size_t size, wxIPCFormat format)
{
    Log("OnAdvise", topic, item, data, size, format);
    return true;
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage("OnDisconnect()");
    wxGetApp().ExitMainLoop();
    return true;
}

bool MyConnection::DoExecute(const void *data, size_t size, wxIPCFormat format)
{
    Log("Execute", wxEmptyString, wxEmptyString, data, size, format);
    bool retval = wxConnection::DoExecute(data, size, format);
    if (!retval)
    {
        wxLogMessage("Execute failed!");
    }
    return retval;
}

const void *MyConnection::Request(const wxString& item, size_t *size, wxIPCFormat format)
{
    const void *data =  wxConnection::Request(item, size, format);
    Log("Request", wxEmptyString, item, data, size ? *size : wxNO_LEN, format);
    return data;
}

bool MyConnection::DoPoke(const wxString& item, const void *data, size_t size, wxIPCFormat format)
{
    Log("Poke", wxEmptyString, item, data, size, format);
    return wxConnection::DoPoke(item, data, size, format);
}
