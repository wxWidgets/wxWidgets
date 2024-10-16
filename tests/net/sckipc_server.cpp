///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/sckipc_server.cpp
// Purpose:     External server for IPC test with sockets
// Authors:     Vadim Zeitlin, JP Mattia
// Copyright:   (c) 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include <wx/wx.h>

#include <iostream>

#if wxUSE_THREADS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "ipc_setup_test.h"

#include <wx/ipc.h>
#include <wx/thread.h>

#define MAX_MSG_BUFFERS 2048

// forward decl
class IPCTestServer;

// test connection class used by IPCTestServer
class IPCTestConnection : public wxConnection
{
public:
    IPCTestConnection(IPCTestServer* server)
    {
        m_server = server;
        ResetThreadTrackers();

        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            m_bufferList[i] = nullptr;

        m_nextAvailable = 0;
    }

    ~IPCTestConnection()
    {
        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            if (m_bufferList[i])
                delete[] m_bufferList[i];
    }

    virtual bool OnExec(const wxString& topic, const wxString& data)
    {
        if ( topic != IPC_TEST_TOPIC )
            return false;

        if (data == "shutdown")
            wxTheApp->ExitMainLoop();

        m_lastExecute = data;
        return true;
    }

    virtual bool OnDisconnect();

    virtual const void* OnRequest(const wxString& topic,
                                  const wxString& item,
                                  size_t* size,
                                  wxIPCFormat format);

    virtual bool OnStartAdvise(const wxString& topic,
                               const wxString& item);

    virtual bool OnStopAdvise(const wxString& topic,
                              const wxString& item);

private:

    wxString HandleThreadRequestCounting(const wxString& item);
    void ResetThreadTrackers();

    // memory that must persist past the return of some wxConnection methods
    // is stored in an array of buffers and alloted with GetBufPtr().
    char* GetBufPtr(size_t size)
    {
        wxCRIT_SECT_LOCKER(lock, m_cs_assign_buffer);

        if (m_bufferList[m_nextAvailable] != nullptr)
        {
            // Free the memory from the last use
            delete[] m_bufferList[m_nextAvailable];
        }

        char* ptr = new char[size];

        m_bufferList[m_nextAvailable] = ptr;
        m_nextAvailable = (m_nextAvailable + 1) % MAX_MSG_BUFFERS;

        return ptr;
    }

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_assign_buffer);

    IPCTestServer* m_server;

    char* m_bufferList[MAX_MSG_BUFFERS];
    int m_nextAvailable;

    wxString m_lastExecute;

    int m_thread1_request_lastval, m_thread2_request_lastval,
        m_thread3_request_lastval;

public:
    bool m_advise_active;
    wxString m_general_error;

    bool m_wait_for_first_request;
    bool m_first_request_received;

   wxDECLARE_NO_COPY_CLASS(IPCTestConnection);
};


// SingleAdviseThread sends a single Advise() message
class SingleAdviseThread : public wxThread
{
public:

    SingleAdviseThread(const wxString& item)
    {
        m_item = item;

        Create();
    }

protected:
    virtual void *Entry();

    wxString m_item;

    wxDECLARE_NO_COPY_CLASS(SingleAdviseThread);
};


// MultiAdviseThread sends repeated Advise() messages
class MultiAdviseThread : public wxThread
{
public:
    MultiAdviseThread(const wxString& item, const wxString& label)
    {
        m_item = item;
        m_label = label;

        Create();
    }

protected:
    virtual void *Entry();

    wxString m_item, m_label;

    wxDECLARE_NO_COPY_CLASS(MultiAdviseThread);
};



// IPCTestConnection implementation
const void* IPCTestConnection::OnRequest(const wxString& topic,
                                         const wxString& item,
                                         size_t* size,
                                         wxIPCFormat format)
{
    *size = 0;

    if ( topic != IPC_TEST_TOPIC )
        return nullptr;

    // When running simultaneous MultiAdvise (server) and MultiRequest
    // (client) threads, notify MultiAdvise when the first request has been
    // received helps to synchronize the two processes, which tests
    // possible race conditions between Advise() and Request() calls.
    m_first_request_received = true;

    wxString s;

    if ( item == "ping" )
    {
        if (format != wxIPC_PRIVATE)
            return nullptr;

        s = "pong";
    }

    else if ( item == "last_execute" )
    {
        s = m_lastExecute;
    }

    else if ( item.StartsWith("MultiRequest thread") )
    {
        s = HandleThreadRequestCounting(item);
    }

    else if ( item == "get_thread1_request_counter")
    {
        s = wxString::Format("%d", m_thread1_request_lastval);
    }

    else if ( item == "get_thread2_request_counter")
    {
        s = wxString::Format("%d", m_thread1_request_lastval);
    }

    else if ( item == "get_thread3_request_counter")
    {
        s = wxString::Format("%d", m_thread1_request_lastval);
    }

    else if ( item == "get_error_string")
    {
        s = m_general_error;
    }

    else
    {
        s = "Error: Unknown request - " + item;
        m_general_error += s;
    }

    *size = strlen(s.mb_str()) + 1;
    char* ret = GetBufPtr(*size);
    strncpy(ret, s.mb_str(), *size);
    return ret;
}


// Helper for the multirequest thread test. This test will send repeated
// requests of the form "MultiRequest thread <thread_number>
// <serial_number>". Track the serial number in the appropriate
// m_threadN_request_lastval vars.
//
// The return string is prefaced with "Error:" when a problem arises, along
// with a human readable message describing the error, and "OK:" when the
// request went as expected.
wxString IPCTestConnection::HandleThreadRequestCounting(const wxString& item)
{
    wxString info;
    item.StartsWith("MultiRequest thread", &info);

    int thread_number = wxAtoi(info.Left(2));
    int counter_value = wxAtoi(info.Mid(3));
    int lastval = -2;

    bool err = false;
    wxString err_string;

    switch (thread_number)
    {
    case 0:
        err_string =
            "Error: MultiRequest thread number could not be converted.\n";
        err = true;
        break;

    case 1:
        lastval = m_thread1_request_lastval;
        m_thread1_request_lastval = counter_value;
        break;

    case 2:
        lastval = m_thread2_request_lastval;
        m_thread2_request_lastval = counter_value;
        break;

    case 3:
        lastval = m_thread3_request_lastval;
        m_thread3_request_lastval = counter_value;
        break;

    default:
        err_string =
            "Error: MultiRequest thread number must be 1, 2, or3.\n";
        err = true;
    }

    if (lastval !=  counter_value -1)
    {
        // Concatenate to any other error:
        err_string +=
            "Error: Misordered count in thread " +
            wxString::Format("%d - expected %d, received %d\n",
                             thread_number, lastval + 1, counter_value);
        err = true;
    }

    if (err)
    {
        m_general_error += err_string;
        return err_string;
    }

    return "OK: " + item;
}

void IPCTestConnection::ResetThreadTrackers()
{
    m_general_error = "";

    m_thread1_request_lastval = m_thread2_request_lastval =
        m_thread3_request_lastval = 0;

    m_advise_active = false;

    m_wait_for_first_request = false;
    m_first_request_received = false;
}

bool IPCTestConnection::OnStartAdvise(const wxString& topic,
                                      const wxString& item)
{
    if ( topic != IPC_TEST_TOPIC )
        return false;

    m_advise_active = true;

    if (item == "SimpleAdvise test")
    {
        SingleAdviseThread* thread = new SingleAdviseThread(item);
        thread->Run();
    }
    else if (item.StartsWith("MultiAdvise test"))
    {
        MultiAdviseThread* thread1 =
            new MultiAdviseThread(item, "MultiAdvise thread 1");
        thread1->Run();
    }

    else if (item.StartsWith("MultiAdvise MultiThread test"))
    {
        MultiAdviseThread* thread1 =
            new MultiAdviseThread(item, "MultiAdvise thread 1");
        MultiAdviseThread* thread2 =
            new MultiAdviseThread(item, "MultiAdvise thread 2");
        MultiAdviseThread* thread3 =
            new MultiAdviseThread(item, "MultiAdvise thread 3");

        thread1->Run();
        thread2->Run();
        thread3->Run();
    }

    else if (item.StartsWith("MultiAdvise MultiThread test with simultaneous Requests"))
    {
        // Tell the advise threads to wait for the requests to start.
        m_wait_for_first_request = true;

        MultiAdviseThread* thread1 =
            new MultiAdviseThread(item, "MultiAdvise thread 1");
        MultiAdviseThread* thread2 =
            new MultiAdviseThread(item, "MultiAdvise thread 2");
        MultiAdviseThread* thread3 =
            new MultiAdviseThread(item, "MultiAdvise thread 3");

        thread1->Run();
        thread2->Run();
        thread3->Run();
    }

    else
    {
        m_general_error += "Unknown StartAdvise item\n";
        m_advise_active = false;
    }

    return m_advise_active;
}

bool IPCTestConnection::OnStopAdvise(const wxString& topic,
                                     const wxString& WXUNUSED(item))
{
    if ( topic != IPC_TEST_TOPIC )
        return false;

    m_advise_active = false;

    return true;
}


// test server class
class IPCTestServer : public wxServer
{
public:
    IPCTestServer()
    {
        m_conn = nullptr;
    }

    virtual ~IPCTestServer()
    {
        if (m_conn)
            delete m_conn;
    }

    virtual IPCTestConnection *OnAcceptConnection(const wxString& topic)
    {
        if ( topic != IPC_TEST_TOPIC )
            return nullptr;

        m_conn = new IPCTestConnection(this);
        return m_conn;
    }

    void Shutdown()
    {
        if (m_conn)
            m_conn->Disconnect();
    }

    IPCTestConnection& GetConn()
    {
        if (!m_conn)
        {
            // create a bogus connection rather than crash.
            m_conn = new IPCTestConnection(this);
            m_conn->m_general_error = "Invalid connection in GetConn()";
        }

        return *m_conn;
    }

    IPCTestConnection *m_conn;

    wxDECLARE_NO_COPY_CLASS(IPCTestServer);
};


// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;

    IPCTestServer m_server;
};

wxDECLARE_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_APP_CONSOLE(MyApp);

bool IPCTestConnection::OnDisconnect()
{
    m_server->m_conn = nullptr;
    return wxConnection::OnDisconnect();
}

void* SingleAdviseThread::Entry()
{
    wxMilliSleep(50); // wait for StartAdvise acknowledgement

    IPCTestConnection& conn = wxGetApp().m_server.GetConn();
    if ( !conn.m_advise_active )
        conn.m_general_error = "Advise() call without StartAdvise()\n";

    wxString s = "OK SimpleAdvise";
    size_t size = strlen(s.mb_str());

    if ( !conn.Advise(m_item, s.mb_str(), size, wxIPC_TEXT) )
        conn.m_general_error = "Advise() call returned false";

    return nullptr;
}

void* MultiAdviseThread::Entry()
{
    IPCTestConnection& conn = wxGetApp().m_server.GetConn();

    if ( !conn.m_advise_active )
        conn.m_general_error = "Advise() call without StartAdvise()\n";

    // The MultiAdviseThread threads in the server start dispatching their
    // calls quickly compared to the MultiRequestThread in the client, so
    // wait until we start seeing Request()'s arrive.

    if ( conn.m_wait_for_first_request )
        while ( !conn.m_first_request_received )
            wxMilliSleep(50);

    for (size_t n=1; n < MESSAGE_ITERATIONS + 1; n++)
    {
        if ( !conn.m_advise_active )
            break;

        wxMilliSleep(50); // interleave with request

        wxString s = m_label + wxString::Format(" %zu", n);
        size_t size = strlen(s.mb_str());

        if ( !conn.Advise(m_item, s.mb_str(), size, wxIPC_TEXT) )
            conn.m_general_error
                += wxString::Format(m_label +
                                    "Advise() call returned false: %zu",n);
    }

    return nullptr;
}

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_SOCKETS_FOR_IPC
    // we must call this from the main thread
    wxSocketBase::Initialize();
#endif // wxUSE_SOCKETS_FOR_IPC

    if ( !m_server.Create(IPC_TEST_PORT) )
    {
        std::cout << "Failed to create server. Make sure nothing is running on port " << IPC_TEST_PORT << std::flush;

#if wxUSE_SOCKETS_FOR_IPC
        wxSocketBase::Shutdown();
#endif // wxUSE_SOCKETS_FOR_IPC

        return false;
    }

    return true;
}

int MyApp::OnExit()
{
    m_server.Shutdown();

#if wxUSE_SOCKETS_FOR_IPC
    wxSocketBase::Shutdown();
#endif // wxUSE_SOCKETS_FOR_IPC

    return wxApp::OnExit();
}


#endif // wxUSE_THREADS
