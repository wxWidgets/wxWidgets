///////////////////////////////////////////////////////////////////////////////
// Name:        samples/ipc/baseserver.cpp
// Purpose:     IPC sample: console server
// Author:      Anders Larsen
//              Most of the code was stolen from: samples/ipc/server.cpp
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

// ----------------------------------------------------------------------------
// local classes
// ----------------------------------------------------------------------------

// a simple connection class testing and logging various operations
class MyConnection : public MyConnectionBase, public wxTimer
{
public:
    virtual bool Disconnect() override { return wxConnection::Disconnect(); }
    virtual bool OnExecute(const wxString& topic,
                           const void *data,
                           size_t size,
                           wxIPCFormat format) override;
    virtual const void *OnRequest(const wxString& topic,
                                  const wxString& item,
                                  size_t *size,
                                  wxIPCFormat format) override;
    virtual bool OnPoke(const wxString& topic,
                        const wxString& item,
                        const void *data,
                        size_t size,
                        wxIPCFormat format) override;
    virtual bool OnStartAdvise(const wxString& topic, const wxString& item) override;
    virtual bool OnStopAdvise(const wxString& topic, const wxString& item) override;
    virtual bool DoAdvise(const wxString& item,
                          const void *data,
                          size_t size,
                          wxIPCFormat format) override;
    virtual bool OnDisconnect() override;
    virtual void Notify() override;

private:
    wxString        m_sAdvise;

    wxString        m_sRequestDate;
    char            m_achRequestBytes[3];
};

// a connection used for benchmarking some IPC operations by
// tests/benchmarks/ipcclient.cpp
class BenchConnection : public wxConnection
{
public:
    BenchConnection() { m_advise = false; }

    virtual bool OnPoke(const wxString& topic,
                        const wxString& item,
                        const void *data,
                        size_t size,
                        wxIPCFormat format) override;
    virtual bool OnStartAdvise(const wxString& topic, const wxString& item) override;
    virtual bool OnStopAdvise(const wxString& topic, const wxString& item) override;

private:
    // return true if this is the supported topic+item combination, log an
    // error message otherwise
    bool IsSupportedTopicAndItem(const wxString& operation,
                                 const wxString& topic,
                                 const wxString& item) const;

    // the item which can be manipulated by the client via Poke() calls
    wxString m_item;

    // should we notify the client about changes to m_item?
    bool m_advise;

    wxDECLARE_NO_COPY_CLASS(BenchConnection);
};

// a simple server accepting connections to IPC_TOPIC and IPC_BENCHMARK_TOPIC
class MyServer : public wxServer
{
public:
    MyServer();
    virtual ~MyServer();
    void Disconnect();
    bool IsConnected() { return m_connection != nullptr; }

    virtual wxConnectionBase *OnAcceptConnection(const wxString& topic) override;

private:
    wxConnection *m_connection;
};

// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;

protected:
    MyServer m_server;
};

wxDECLARE_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_APP_CONSOLE(MyApp);

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    delete wxLog::SetActiveTarget(new wxLogStderr);

    const char * const kind =
#if wxUSE_DDE_FOR_IPC
                 "DDE"
#else
                 "TCP"
#endif
                 ;

    // Create a new server
    if ( !m_server.Create(IPC_SERVICE) )
    {
        wxLogMessage("%s server failed to start on %s", kind, IPC_SERVICE);
        return false;
    }

    wxLogMessage("%s server started on %s", kind, IPC_SERVICE);
    return true;
}

// ----------------------------------------------------------------------------
// MyServer
// ----------------------------------------------------------------------------

MyServer::MyServer()
{
    m_connection = nullptr;
}

MyServer::~MyServer()
{
    Disconnect();
}

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    wxLogMessage("OnAcceptConnection(\"%s\")", topic);

    if ( topic == IPC_TOPIC )
    {
        m_connection = new MyConnection;
    }
    else if ( topic == IPC_BENCHMARK_TOPIC )
    {
        m_connection = new BenchConnection;
    }
    else // unknown topic
    {
        wxLogMessage("Unknown topic");
        return nullptr;
    }

    wxLogMessage("Connection accepted");
    return m_connection;
}

void MyServer::Disconnect()
{
    if ( m_connection )
    {
        m_connection->Disconnect();
        wxDELETE(m_connection);
        wxLogMessage("Disconnected client");
    }
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool
MyConnection::OnExecute(const wxString& topic,
                        const void *data,
                        size_t size,
                        wxIPCFormat format)
{
    Log("OnExecute", topic, "", data, size, format);
    return true;
}

bool
MyConnection::OnPoke(const wxString& topic,
                     const wxString& item,
                     const void *data,
                     size_t size,
                     wxIPCFormat format)
{
    Log("OnPoke", topic, item, data, size, format);
    return wxConnection::OnPoke(topic, item, data, size, format);
}

const void *
MyConnection::OnRequest(const wxString& topic,
                        const wxString& item,
                        size_t *size,
                        wxIPCFormat format)
{
    const void *data;
    if (item == "Date")
    {
        m_sRequestDate = wxDateTime::Now().Format();
        data = m_sRequestDate.c_str();
        *size = wxNO_LEN;
    }
    else if (item == "Date+len")
    {
        m_sRequestDate = wxDateTime::Now().FormatTime() +
                            " " + wxDateTime::Now().FormatDate();
        data = m_sRequestDate.c_str();
        *size = m_sRequestDate.length() + 1;
    }
    else if (item == "bytes[3]")
    {
        data = m_achRequestBytes;
        m_achRequestBytes[0] = '1';
        m_achRequestBytes[1] = '2';
        m_achRequestBytes[2] = '3';
        *size = 3;
    }
    else
    {
        data = nullptr;
        *size = 0;
    }
    Log("OnRequest", topic, item, data, *size, format);
    return data;
}

bool MyConnection::OnStartAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage("OnStartAdvise(\"%s\",\"%s\")", topic, item);
    wxLogMessage("Returning true");
    m_sAdvise = item;
    Start(3000); // schedule our Notify() to be called in 3 seconds
    return true;
}

bool MyConnection::OnStopAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage("OnStopAdvise(\"%s\",\"%s\")", topic, item);
    wxLogMessage("Returning true");
    m_sAdvise.clear();
    Stop();
    return true;
}

void MyConnection::Notify()
{
    if (!m_sAdvise.empty())
    {
        wxString s = wxDateTime::Now().Format();
        Advise(m_sAdvise, s);
        s = wxDateTime::Now().FormatTime() + " "
                + wxDateTime::Now().FormatDate();
        Advise(m_sAdvise, s.mb_str(), s.length() + 1);

#if wxUSE_DDE_FOR_IPC
        wxLogMessage("DDE Advise type argument cannot be wxIPC_PRIVATE. "
                     "The client will receive it as wxIPC_TEXT, "
                     "and receive the correct no of bytes, "
                     "but not print a correct log entry.");
#endif // DDE

        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        Advise(m_sAdvise, bytes, 3, wxIPC_PRIVATE);
        // this works, but the log treats it as a string now
//        m_connection->Advise(m_connection->m_sAdvise, bytes, 3, wxIPC_TEXT );
    }
}

bool MyConnection::DoAdvise(const wxString& item,
                            const void *data,
                            size_t size,
                            wxIPCFormat format)
{
    Log("Advise", "", item, data, size, format);
    return wxConnection::DoAdvise(item, data, size, format);
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage("OnDisconnect()");
    return true;
}

// ----------------------------------------------------------------------------
// BenchConnection
// ----------------------------------------------------------------------------

bool BenchConnection::IsSupportedTopicAndItem(const wxString& operation,
                                              const wxString& topic,
                                              const wxString& item) const
{
    if ( topic != IPC_BENCHMARK_TOPIC ||
            item != IPC_BENCHMARK_ITEM )
    {
        wxLogMessage("Unexpected %s(\"%s\", \"%s\") call.",
                     operation, topic, item);
        return false;
    }

    return true;
}

bool BenchConnection::OnPoke(const wxString& topic,
                             const wxString& item,
                             const void *data,
                             size_t size,
                             wxIPCFormat format)
{
    if ( !IsSupportedTopicAndItem("OnPoke", topic, item) )
        return false;

    if ( !IsTextFormat(format) )
    {
        wxLogMessage("Unexpected format %d in OnPoke().", format);
        return false;
    }

    m_item = GetTextFromData(data, size, format);

    if ( m_advise )
    {
        if ( !Advise(item, m_item) )
        {
            wxLogMessage("Failed to advise client about the change.");
        }
    }

    return true;
}

bool BenchConnection::OnStartAdvise(const wxString& topic, const wxString& item)
{
    if ( !IsSupportedTopicAndItem("OnStartAdvise", topic, item) )
        return false;

    m_advise = true;

    return true;
}

bool BenchConnection::OnStopAdvise(const wxString& topic, const wxString& item)
{
    if ( !IsSupportedTopicAndItem("OnStopAdvise", topic, item) )
        return false;

    m_advise = false;

    return true;
}

