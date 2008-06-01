///////////////////////////////////////////////////////////////////////////////
// Name:        samples/ipc/baseserver.cpp
// Purpose:     IPC sample: console server
// Author:      Anders Larsen
//              Most of the code was stolen from: samples/ipc/server.cpp
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
class MyServer;

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

protected:
    MyServer       *m_server;
};

DECLARE_APP(MyApp)

class MyConnection : public MyConnectionBase, public wxTimer
{
public:
    virtual bool Disconnect() { return wxConnection::Disconnect(); }
    virtual bool OnExecute(const wxString& topic, const void *data, size_t size, wxIPCFormat format);
    virtual const void *OnRequest(const wxString& topic, const wxString& item, size_t *size, wxIPCFormat format);
    virtual bool OnPoke(const wxString& topic, const wxString& item, const void *data, size_t size, wxIPCFormat format);
    virtual bool OnStartAdvise(const wxString& topic, const wxString& item);
    virtual bool OnStopAdvise(const wxString& topic, const wxString& item);
    virtual bool DoAdvise(const wxString& item, const void *data, size_t size, wxIPCFormat format);
    virtual bool OnDisconnect();
    virtual void Notify();

    wxString        m_sAdvise;

protected:
    wxString        m_sRequestDate;
    char            m_achRequestBytes[3];
};

class MyServer: public wxServer
{
public:
    MyServer();
    virtual ~MyServer();
    void Disconnect();
    bool IsConnected() { return m_connection != NULL; };
    MyConnection *GetConnection() { return m_connection; };
    wxConnectionBase *OnAcceptConnection(const wxString& topic);

protected:
    MyConnection     *m_connection;
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_APP_CONSOLE(MyApp)

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    delete wxLog::SetActiveTarget(new wxLogStderr);

    // Create a new server
    m_server = new MyServer;
    if (m_server->Create("4242"))
    {
        wxLogMessage(_T("Server 4242 started"));
#if wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("Server uses DDE"));
#else // !wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("Server uses TCP"));
#endif // wxUSE_DDE_FOR_IPC/!wxUSE_DDE_FOR_IPC
        return true;
    }
    else
    {
        wxLogMessage(_T("Server 4242 failed to start"));
        delete m_server;
        return false;
    }
}

int MyApp::OnExit()
{
    return 0;
}

// ----------------------------------------------------------------------------
// MyServer
// ----------------------------------------------------------------------------

MyServer::MyServer()
{
    m_connection = NULL;
}

MyServer::~MyServer()
{
    Disconnect();
}

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    wxLogMessage(_T("OnAcceptConnection(\"%s\")"), topic.c_str());

    if ( topic == IPC_TOPIC )
    {
        m_connection = new MyConnection;
        wxLogMessage(_T("Connection accepted"));
        return m_connection;
    }
    // unknown topic
    return NULL;
}

void MyServer::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
        wxLogMessage(_T("Disconnected client"));
    }
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool MyConnection::OnExecute(const wxString& topic,
    const void *data, size_t size, wxIPCFormat format)
{
    Log(_T("OnExecute"), topic, _T(""), data, size, format);
    return true;
}

bool MyConnection::OnPoke(const wxString& topic,
    const wxString& item, const void *data, size_t size, wxIPCFormat format)
{
    Log(_T("OnPoke"), topic, item, data, size, format);
    return wxConnection::OnPoke(topic, item, data, size, format);
}

const void *MyConnection::OnRequest(const wxString& topic,
    const wxString& item, size_t *size, wxIPCFormat format)
{
    const void *data;
    if (item == _T("Date"))
    {
        m_sRequestDate = wxDateTime::Now().Format();
        data = m_sRequestDate.c_str();
        *size = wxNO_LEN;
    }
    else if (item == _T("Date+len"))
    {
        m_sRequestDate = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
        data = m_sRequestDate.c_str();
        *size = m_sRequestDate.Length() + 1;
    }
    else if (item == _T("bytes[3]"))
    {
        data = m_achRequestBytes;
        m_achRequestBytes[0] = '1'; m_achRequestBytes[1] = '2'; m_achRequestBytes[2] = '3';
        *size = 3;
    }
    else
    {
        data = NULL;
        *size = 0;
    }
    Log(_T("OnRequest"), topic, item, data, *size, format);
    return data;
}

bool MyConnection::OnStartAdvise(const wxString& topic,
                                 const wxString& item)
{
    wxLogMessage(_T("OnStartAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(_T("Returning true"));
    m_sAdvise = item;
    Start(3000, false);
    return true;
}

bool MyConnection::OnStopAdvise(const wxString& topic,
                                 const wxString& item)
{
    wxLogMessage(_T("OnStopAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(_T("Returning true"));
    m_sAdvise.Empty();
    Stop();
    return true;
}

void MyConnection::Notify()
{
    if (!m_sAdvise.IsEmpty())
    {
        wxString s = wxDateTime::Now().Format();
        Advise(m_sAdvise, s);
        s = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
        Advise(m_sAdvise, (const char *)s.c_str(), s.Length() + 1);

#if wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("DDE Advise type argument cannot be wxIPC_PRIVATE. The client will receive it as wxIPC_TEXT, and receive the correct no of bytes, but not print a correct log entry."));
#endif
        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        Advise(m_sAdvise, bytes, 3, wxIPC_PRIVATE);
        // this works, but the log treats it as a string now
//        m_connection->Advise(m_connection->m_sAdvise, bytes, 3, wxIPC_TEXT );
    }
}

bool MyConnection::DoAdvise(const wxString& item, const void *data, size_t size, wxIPCFormat format)
{
    Log(_T("Advise"), _T(""), item, data, size, format);
    return wxConnection::DoAdvise(item, data, size, format);
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage(_T("OnDisconnect()"));
    return true;
}
