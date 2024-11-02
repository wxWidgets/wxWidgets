///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/ipc.cpp
// Purpose:     IPC classes unit tests
// Author:      Vadim Zeitlin
// Copyright:   (c) 2008 Vadim Zeitlin
// Modified by: JP Mattia, 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

// this test needs threads as it runs the test server in a secondary thread
#if wxUSE_THREADS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "ipc_setup_test.h"

#include <wx/ipc.h>
#include <wx/thread.h>
#include <wx/process.h>
#include <wx/timer.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/utils.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>

// forward decl
class IPCTestClient;
class ExecAsyncWrapper;

// The test for IPC needs to run the client and socket in separate
// processes. Since catch2 does not have a facility for another process, we
// create one using wxExecute. The client is run below, and the wxExecute runs
// the server.
//
// Note that catch2 cannot run checks in the external server process, so
// instead the client queries the server for the desired information and then
// runs the checks in this file.


// Automated test needs a process with an external server.  When running the
// tests manually, set g_start_external_server to false and then start the
// test_sckipc_server via the command line.
bool g_start_external_server = true;

// When g_show_message_timing is set to true, Advise() and RequestReply()
// messages will be printed when they arrive. This shows how the IPC messages
// arrive and whether they interleave,
bool g_show_message_timing = false;

// Output for g_show_message_timing uses std::cout, so we can get a sense of the
// raw arrival times.
#include <iostream>

// The command to run the external server.
#ifdef __UNIX__
    #define SERVER_COMMAND "test_sckipc_server"
#elif defined(__WINDOWS__)
    #define SERVER_COMMAND "test_sckipc_server.exe"
#else
    #error "no command to exec"
#endif // OS

// Test connection class used by the client.
class IPCTestConnection : public wxConnection
{
public:
    IPCTestConnection(IPCTestClient* client)
    {
        m_client = client;
        ResetThreadTrackers();
    }

    virtual ~IPCTestConnection() {}

    virtual bool OnExec(const wxString& topic, const wxString& data)
    {
        if ( topic != IPC_TEST_TOPIC )
            return false;

        return data == "Date";
    }

    virtual bool OnAdvise(const wxString& topic,
                          const wxString& item,
                          const void* data,
                          size_t size,
                          wxIPCFormat format)
    {
        if ( topic != IPC_TEST_TOPIC )
            return false;

        CHECK( format == wxIPC_TEXT );

        wxString s(static_cast<const char*>(data), size);

        if (item == "SimpleAdvise test")
        {

            CHECK( s == "OK SimpleAdvise" );
            m_advise_complete = true;
        }

        else if (item == "MultiAdvise test" ||
                 item == "MultiAdvise MultiThread test" ||
                 item == "MultiAdvise MultiThread test with simultaneous Requests")
        {
            HandleThreadAdviseCounting(s);

            if (m_thread1_advise_lastval == MESSAGE_ITERATIONS &&
                m_thread2_advise_lastval == MESSAGE_ITERATIONS &&
                m_thread3_advise_lastval == MESSAGE_ITERATIONS)
            {
                m_advise_complete = true;
            }
        }

        else
        {
            m_general_error << "Unknown Advise item: " << item << wxString('\n');
        }

        return true;
    }


    virtual bool OnDisconnect();

private:

    void ResetThreadTrackers()
    {
        m_general_error = "";

        m_advise_complete = false;

        m_thread1_advise_lastval = m_thread2_advise_lastval =
            m_thread3_advise_lastval = 0;
    }

    void HandleThreadAdviseCounting(const wxString& advise_string);

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_assign_buffer);

    IPCTestClient* m_client;

public:
    bool  m_advise_complete;

    int m_thread1_advise_lastval;
    int m_thread2_advise_lastval;
    int m_thread3_advise_lastval;

    wxString m_general_error;

    wxDECLARE_NO_COPY_CLASS(IPCTestConnection);
};

// Helper for the MultiAdvise thread tests. Repeated Advise's of the form
// "MultiAdvise thread <thread_number N> <serial_number>" are received during
// the test. Track the serial number in the appropriate
// m_threadN_advise_lastval member variables for CHECKing at the end of the
// test.
void IPCTestConnection::HandleThreadAdviseCounting(const wxString& advise_string)
{
    wxString info;
    advise_string.StartsWith("MultiAdvise thread", &info);

    int thread_number = wxAtoi(info.Left(2));
    int counter_value = wxAtoi(info.Mid(3));
    int lastval = INT_MIN; // default to causing an error below

    bool err = false;
    wxString err_string;

    if ( g_show_message_timing )
        std::cout << advise_string << '\n' << std::flush;


    switch (thread_number)
    {
    case 0:
        err_string =
            "Error: MultiAdvise thread number could not be converted.\n";
        err = true;
        break;

    case 1:
        lastval = m_thread1_advise_lastval;
        m_thread1_advise_lastval = counter_value;
        break;

    case 2:
        lastval = m_thread2_advise_lastval;
        m_thread2_advise_lastval = counter_value;
        break;

    case 3:
        lastval = m_thread3_advise_lastval;
        m_thread3_advise_lastval = counter_value;
        break;

    default:
        err_string =
            "Error: MultiAdvise thread number must be 1, 2, or3.\n";
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
    }
}

// After the server finishes running in its process, IPCServerProcess gets
// notified of the termination.
class IPCServerProcess : public wxProcess
{
public:
    IPCServerProcess(ExecAsyncWrapper* parent)
    {
        m_parent = parent;
    }

    virtual void OnTerminate(int pid, int status);

    ExecAsyncWrapper* m_parent;
};

// The server is started in an external process using wxExecute. Since
// wxExecute needs an event loop to run, we set up a wrapper to start a loop
// and use wxTimer as a callback to run the wxExecute command.
//
// There are a number of utilities included here, so that we can make sure the
// server process terminates when we want it to. If the server were not to
// terminate, then the IPC port would remain bound and all further tests would
// potentially run into trouble if the server had hung.
class ExecAsyncWrapper : public wxTimer
{
public:
    ExecAsyncWrapper()
        : m_process(nullptr)
    {
        if (!g_start_external_server)
            return;

        m_process = new IPCServerProcess(this);
        m_process_finished = false;

        // Get the path that test is running in, and compose the full path to
        // the executable for the server command.
        wxFileName fn_testpath(wxStandardPaths::Get().GetExecutablePath());
        wxString testPath(fn_testpath.GetPath());

        wxFileName fn_executable;
        fn_executable.Assign(testPath, SERVER_COMMAND);

        REQUIRE( fn_executable.Exists() );

        m_command = fn_executable.GetFullPath();
    }

    ~ExecAsyncWrapper()
    {
        if (m_process) delete m_process;
    }

    long DoExecute()
    {
        // Trigger the timer to go off inside the event loop
        // so that we can run wxExecute there.
        StartOnce(10);

        // Run the event loop.
        wxEventLoop loop;
        loop.Run();

        return m_pid;
    }

    void Notify() override
    {
        // Run wxExecute inside the event loop.
        m_pid = wxExecute(m_command, wxEXEC_ASYNC, m_process);

        REQUIRE( m_pid != 0);

        wxEventLoop::GetActive()->Exit();
    }

    bool SendSIGTERM()
    {
        if (IsFinished())
            return true;

        // For some reason on wxMSW, the process sometimes needs more than one
        //  iteration, even with wxKILL_OK as the return value from wxKill.
        for ( int i=0; i < 3 && StillRunning(); i++ )
        {
            wxKill(m_pid, wxSIGTERM);
            wxMilliSleep(50);
        }

        return IsFinished();
    }

    bool SendSIGKILL()
    {
        if (IsFinished())
            return true;

        wxKillError result;

        wxKill(m_pid, wxSIGKILL, &result);

        bool process_killed = result == wxKILL_OK || result == wxKILL_NO_PROCESS;

        // SIGKILL will bypass wxProcess::OnTerminate, so set the following
        // manually.
        if ( process_killed )
            m_process_finished = true;

        if ( g_show_message_timing )
        {
            if ( process_killed )
                std::cout << "server process killed\n" << std::flush;
            else
                std::cout << "wxSIGKILL unsucessful: "
                          << wxKILL_ResultToString(result)
                          << "\n" << std::flush;
        }

        return process_killed;
    }

    // Utility for wxKILL
    wxString wxKILL_ResultToString(wxKillError result)
    {
        switch (result)
        {
        case wxKILL_OK:            return "wxKILL_OK";
        case wxKILL_BAD_SIGNAL:    return "wxKILL_BAD_SIGNAL";
        case wxKILL_ACCESS_DENIED: return "wxKILL_ACCESS_DENIED";
        case wxKILL_NO_PROCESS:    return "wxKILL_NO_PROCESS";
        case wxKILL_ERROR:         return "wxKILL_ERROR";
        default:
            return "Unknown result from wxKILL";
        };
    };

    bool IsFinished() const { return m_process_finished; }
    bool StillRunning() const { return !m_process_finished; }

    wxString m_command;
    long m_pid;
    IPCServerProcess* m_process;
    bool m_process_finished;

    wxDECLARE_NO_COPY_CLASS(ExecAsyncWrapper);
};

void IPCServerProcess::OnTerminate(int pid, int status)
{
    m_parent->m_process_finished = true;

    if ( g_show_message_timing )
    {
        std::cout
            << wxString::Format("Process %u terminated, exit code %d.\n",
                                pid, status)
            << std::flush;
    }
}

// SleepProcess starts a loop, for methods that need a main loop to be running,
// eg wxProcess::OnTerminate
class SleepProcess : public wxTimer
{
public:
    SleepProcess() {};

    void DoWait()
    {
        // Trigger the timer to go off inside the event loop.
        // While the loop is running, wxProcess::OnTerminate gets called
        // if the IPC server terminated.
        StartOnce(50);

        // Run the event loop.
        wxEventLoop loop;
        loop.Run();
    }

    void Notify() override
    {
        wxEventLoop::GetActive()->Exit();
    }
};

// The actual client is pretty thin, most of the work is done in the
// connection class.
class IPCTestClient : public wxClient
{
public:
    IPCTestClient()
    {
        m_conn = nullptr;
    }

    virtual ~IPCTestClient()
    {
        Disconnect();
    }

    bool
    Connect(const wxString& host, const wxString& service, const wxString& topic)
    {
        m_conn = (IPCTestConnection*) MakeConnection(host, service, topic);

        return m_conn != nullptr;
    }

    void Disconnect()
    {
        if ( m_conn )
        {
            m_conn->Disconnect();
            delete m_conn;
            m_conn = nullptr;
        }
    }

    wxConnectionBase* OnMakeConnection()
    {
        return new IPCTestConnection(this);
    }

    IPCTestConnection& GetConn() const
    {
        REQUIRE( m_conn );

        return *m_conn;
    }

    IPCTestConnection *m_conn;

    wxDECLARE_NO_COPY_CLASS(IPCTestClient);
};

static IPCTestClient *gs_client = nullptr;

bool IPCTestConnection::OnDisconnect()
{
    m_client->m_conn = nullptr;
    return wxConnection::OnDisconnect();
}

// MultiRequestThread sends repeated Request()'s, each with a serial number,
// so that we can verify the repeated messages are sent and received correctly
// and in order.
class MultiRequestThread : public wxThread
{
public:

    // label: A header to be put on the string sent to the server.
    // It should be of the form "MultiRequest thread N", where N
    // is "1", "2", or "3".
    MultiRequestThread(const wxString& label )
        : wxThread(wxTHREAD_JOINABLE)
    {
        m_label = label;

        Create();
    }

protected:
    virtual void *Entry()
    {
        IPCTestConnection& conn = gs_client->GetConn();

        for (size_t n=1; n < MESSAGE_ITERATIONS + 1; n++)
        {
            wxString s = m_label + wxString::Format(" %zu", n);
            size_t size=0;
            const char* data = (char*) conn.Request(s, &size, wxIPC_PRIVATE);

            // Catch2 macros are not thread safe, so we check explicitly and
            // store any deviation from the expected result.
            if ( wxString(data) != "OK: " + s )
            {
                m_error += "MultiRequestThread error: expected \"OK: " + s;
                m_error += ", received " + wxString(data);
                m_error += '\n';
            }

            if ( g_show_message_timing )
                std::cout << wxString(data) << '\n' << std::flush;

            // Space out the requests, to test any race conditions with
            // incoming messages, like Advise()
            wxMilliSleep(50);
        }

        return nullptr;
    }

public:
    wxString m_label;
    wxString m_error;

    wxDECLARE_NO_COPY_CLASS(MultiRequestThread);
};

// IPCFixture is responsible for setting up and tearing down the external
// server process for the test cases.
class IPCFixture
{
public:
    IPCFixture()
    {
#if wxUSE_SOCKETS_FOR_IPC
        // We will be using sockets from worker threads, so we need to
        // initialize.
        wxSocketBase::Initialize();
#endif // wxUSE_SOCKETS_FOR_IPC

        gs_client = new IPCTestClient;

        if ( g_start_external_server )
        {
            long pid = m_exec.DoExecute();

            // Allow a moment for the server to bind the port
            wxMilliSleep(50);

            REQUIRE( pid != 0);
        };

    };

    ~IPCFixture()
    {
        if ( g_start_external_server )
        {
            // Make sure there is a connection
            IPCTestConnection& conn = gs_client->GetConn();

            // Executing a shutdown on the server should cause the server to
            // self-terminate.
            const wxString s("shutdown");
            CHECK( conn.Execute(s) );

            // Give the server a moment to self-exit
            for ( int i=0; i < 3 && m_exec.StillRunning(); i++ )
            {
                SleepProcess proc;
                proc.DoWait();
            }

            // Self-exit failed, send a SIGTERM.
            if ( !m_exec.SendSIGTERM() )
            {
                // SIGTERM did not work, try SIGKILL
                m_exec.SendSIGKILL();
            }

            CHECK( m_exec.IsFinished() );
        };

        delete gs_client;

#if wxUSE_SOCKETS_FOR_IPC
        wxSocketBase::Shutdown();
#endif // wxUSE_SOCKETS_FOR_IPC

        if ( g_show_message_timing )
            std::cout << "teardown complete\n" << std::flush;
    }

    ExecAsyncWrapper m_exec;
};

// Test the basics of Connect()
TEST_CASE_METHOD(IPCFixture,
                 "IPC::Connect", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Connect\n" << std::flush;

    // connecting to the wrong port should fail
    CHECK( !gs_client->Connect("localhost", "2424", IPC_TEST_TOPIC) );

    // connecting with the wrong topic should fail
    CHECK( !gs_client->Connect("localhost", IPC_TEST_PORT, "VCP GRFG") );

    // Connecting to the right port on the right topic should succeed.
    REQUIRE( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );
}

// Test the basics of Request(): A Request() goes out and it should result in
// a reply from the server.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::SingleRequest", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test SingleRequest\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();

    const wxString s("ping");
    size_t size=0;
    const char* data = (char*) conn.Request(s, &size, wxIPC_PRIVATE);

    // Make sure that Request() works, because we use it to probe the
    // state of the server for the remaining tests.
    REQUIRE( wxString(data) == "pong"  );
}

// Test the basics of Execute(). The Execute() goes out: Note that a return
// value of "true" means simply that the message was transmitted. We follow
// the Execute with a Request() to verify that the server received the Execute
// correctly.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::SingleExecute", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Execute\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();

    wxString s("Date");
    CHECK( conn.Execute(s) );

    // Get the last execute from the server side.
    size_t size=0;
    const wxString last_execute_query("last_execute");

    char* data = (char*) conn.Request(last_execute_query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == s );


    s = "another execution command!";
    CHECK( conn.Execute(s.mb_str(), s.length() + 1) );

    data = (char*) conn.Request(last_execute_query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == s );
}

// Send multiple requests to the server. Each request has a serial number, and
// this test verifies that the replies have the correct serial in the reply
// message. After the serial requests are done, the client queries the server
// and verifies the server received the requests error-free.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::RequestThread", "[net][ipc][multi_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Single Thread Of Requests\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    MultiRequestThread thread1("MultiRequest thread 1");
    thread1.Run();
    thread1.Wait();

    INFO( thread1.m_error );
    CHECK( thread1.m_error.IsEmpty() );

    // Make sure the server got all the requests in the correct order.
    IPCTestConnection& conn = gs_client->GetConn();

    size_t size=0;
    wxString query("get_thread1_request_counter");

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_error_string";
    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

// Send multiple requests to the server via three concurrent threads. Each
// reply is verified to make sure that the request corresponds to the correct
// thread and has the correctly ordered serial number. After the request
// threads are finished, the client queries the server and verifies the server
// received the requests error-free.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::RequestMultiThread", "[net][ipc][multi_thread]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Requests with Multiple Threads\n"
                  << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    MultiRequestThread thread1("MultiRequest thread 1");
    MultiRequestThread thread2("MultiRequest thread 2");
    MultiRequestThread thread3("MultiRequest thread 3");

    thread1.Run();
    thread2.Run();
    thread3.Run();

    thread1.Wait();
    thread2.Wait();
    thread3.Wait();

    INFO( thread1.m_error );
    CHECK( thread1.m_error.IsEmpty() );

    INFO( thread2.m_error );
    CHECK( thread2.m_error.IsEmpty() );

    INFO( thread2.m_error );
    CHECK( thread2.m_error.IsEmpty() );

    // Make sure the server got all the requests in the correct order.
    IPCTestConnection& conn = gs_client->GetConn();

    size_t size=0;
    wxString query = "get_thread1_request_counter";

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_thread2_request_counter";

    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_thread3_request_counter";

    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_error_string";
    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

// Test the basics of Advise(). First, send a StartAdvise(), then wait for the
// server to send a single Advise(). When that is received, StopAdvise() is
// sent.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::SingleAdvise", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Advise as single command\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "SimpleAdvise test";

    CHECK( conn.StartAdvise(item) );

    // wait a maximum of 2 seconds for completion.
    int cnt = 0;
    while ( cnt++ < 200 && !conn.m_advise_complete )
    {
        wxMilliSleep(10);
    }

    CHECK( conn.StopAdvise(item) );
    CHECK( conn.m_advise_complete );

    // Make sure the server didn't record an error
    wxString query = "get_error_string";
    size_t size=0;

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

// Instruct the server to send a series of Advise() items to the client. Each
// Advise() is verified to make sure that the Advise() arrives in order by
// checking its serial number. Also verifies that the server encountered no
// errors during the Advise() calls.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::AdviseThread", "[net][ipc][multi_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Single Thread Of Advise()'s\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "MultiAdvise test";

    CHECK( conn.StartAdvise(item) );

    // wait a maximum of 20 seconds for completion.
    int cnt = 0;
    while ( cnt++ < 2000 &&
            conn.m_thread1_advise_lastval != MESSAGE_ITERATIONS )
    {
        wxMilliSleep(10);
    }

    CHECK( conn.StopAdvise(item) );

    // Verify the results of the test.
    CHECK( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS );

    INFO( conn.m_general_error );
    CHECK( conn.m_general_error.IsEmpty() );

    // Make sure the server didn't record an error
    wxString query = "get_error_string";
    size_t size=0;

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

// Instruct the server to send a series of Advise() items to the client via
// three concurrent threads. Each Advise() is verified to make sure that the
// Advise() arrives in order within its thread number. Also verifies that
// the server encountered no errors during the Advise() calls.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::AdviseMultiThread", "[net][ipc][multi_thread]")
{
    if ( g_show_message_timing )
        std::cout << "Running test MultipleThreadsOfMultiAdvise\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "MultiAdvise MultiThread test";

    CHECK( conn.StartAdvise(item) );

    // wait a maximum of 20 seconds for completion.
    int cnt = 0;
    while ( cnt++ < 2000 )
    {
        wxMilliSleep(10);

        if ( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread2_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread3_advise_lastval == MESSAGE_ITERATIONS)
        {
            break;
        }
    }

    CHECK( conn.StopAdvise(item) );

    CHECK( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS );
    CHECK( conn.m_thread2_advise_lastval == MESSAGE_ITERATIONS );
    CHECK( conn.m_thread3_advise_lastval == MESSAGE_ITERATIONS );

    INFO( conn.m_general_error );
    CHECK( conn.m_general_error.IsEmpty() );

    // Make sure the server didn't record an error
    wxString query = "get_error_string";
    size_t size=0;

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

#ifdef wxMSW

// Run three concurrent threads in the client sending Requests() to the
// server, and simultaneously run three concurrent threads in the server
// sending Advise() information to the client. Verify that all messages are
// serial and correspond to the correct thread. Lastly, verify that the server
// encountered no errors during this test.
//
// By setting g_show_message_timing to "true", the ordering of the Requests
// and Advise's can be seen. Different systems may need to change the delay
// wxMilliSleep in the client and server threads to make the interleave happen
// properly, which is a strigent test of race conditions that might be present
// in wxIPC.
//
// Note that this currently works only on MSW. When run on Linux (Rocky 9) there
// is some hang with an unidentified cause. The hang gets better by turning off
// ReenableEvents in ~wxSocketWriteGuard, but that is not a viable solution.
//
TEST_CASE_METHOD(IPCFixture,
                 "IPC::AdviseAndRequestMultiThread", "[net][ipc][multi_thread]")
{
    if ( g_show_message_timing )
        std::cout << "Running test MultiAdvise MultiThreads test with simultaneous MultiRequests MultiThreads\n" << std::flush;

    CHECK( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );
    IPCTestConnection& conn = gs_client->GetConn();

    MultiRequestThread thread1("MultiRequest thread 1");
    MultiRequestThread thread2("MultiRequest thread 2");
    MultiRequestThread thread3("MultiRequest thread 3");

    // start local and remote threads as close to simultaneously as possible
    wxString item = "MultiAdvise MultiThread test with simultaneous Requests";

    CHECK( conn.StartAdvise(item) ); // starts 3 advise threads on the server

    thread1.Run();
    thread2.Run();
    thread3.Run();


    // Wait for local threads to finish ...
    thread1.Wait();
    thread2.Wait();
    thread3.Wait();

    // ... and the remote threads too.
    int cnt = 0;
    while ( cnt++ < 200 ) // max of 2 seconds
    {
        SleepProcess proc;
        proc.DoWait();

        if ( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread2_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread3_advise_lastval == MESSAGE_ITERATIONS)
        {
            break;
        }
    }

    CHECK( conn.StopAdvise(item) );

    // Everything is done, check that all the advise messages were
    // correctly received.

    CHECK( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS );
    CHECK( conn.m_thread2_advise_lastval == MESSAGE_ITERATIONS );
    CHECK( conn.m_thread3_advise_lastval == MESSAGE_ITERATIONS );

    INFO( conn.m_general_error );
    CHECK( conn.m_general_error.IsEmpty() );


    // Also make sure all the request messages were correctly received on
    // the server side. The client side was already validated in the
    // MultiRequestThread.
    size_t size=0;
    wxString query = "get_thread1_request_counter";

    char* data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_thread2_request_counter";

    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_thread3_request_counter";

    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);
    CHECK( wxString(data) == MESSAGE_ITERATIONS_STRING );

    size=0;
    query = "get_error_string";
    data = (char*) conn.Request(query, &size, wxIPC_PRIVATE);

    INFO( wxString(data) );
    CHECK( wxString(data).IsEmpty() );
}

#endif // wxMSW

#endif // wxUSE_THREADS
