///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/ipc.cpp
// Purpose:     IPC classes unit tests
// Author:      Vadim Zeitlin
// Copyright:   (c) 2008 Vadim Zeitlin
// Modified by: JP Mattia, 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


// The IPC tests use a single test binary: the server is started by re-executing
// the same test program with WX_IPC_TEST_SERVER set (see ipc_test_server.cpp).
// The client runs in the main Catch2 process. Catch2 cannot run checks in the
// server process, so the client queries the server for state and verifies it
// here.
//
// This test requires wxUSE_THREADS==1 since it runs the test server concurrently
// with the client. One build configuration is excluded: wxQt.
//
// wxQt is excluded because of a bug in wxQt found during our testing:
// wxIPC worker threads marshal their socket I/O to the main thread via
// CallAfter(), but a cross-thread CallAfter() is not reliably processed by the
// wxQt event loop. wxQtEventLoopBase::WakeUp() wakes the loop without posting a
// Qt event, so the idle handler that runs pending events is never scheduled, and
// server-pushed Advise() notifications stall. That is a wxQt event-loop bug, not
// a wxIPC bug; it is fixed separately on branch
// jpmattia/wxQT-CallAfter-wxWakeUpIdle, which will be a separate PR.
//
#if wxUSE_THREADS && !defined(__WXQT__)

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "ipc_setup_test.h"
#include "ipc_test_server.h"

#include <wx/ipc.h>
#include <wx/thread.h>
#include <wx/utils.h>
#include <wx/evtloop.h>
#include <wx/stopwatch.h>
#include <atomic>
#include <memory>

// forward decl
class IPCTestClient;

// When g_show_message_timing is set to true, Advise() and RequestReply()
// messages will be printed when they arrive. This shows how the IPC messages
// arrive and whether they interleave.
bool g_show_message_timing = false;

// Output for g_show_message_timing uses std::cout, so we can get a sense of the
// raw arrival times.
#include <iostream>

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

    virtual bool OnExec(const wxString& topic, const wxString& data) override
    {
        if ( topic != IPC_TEST_TOPIC )
            return false;

        return data == "Date";
    }

    virtual bool OnAdvise(const wxString& topic,
                          const wxString& item,
                          const void* data,
                          size_t size,
                          wxIPCFormat format) override
    {
        if ( topic != IPC_TEST_TOPIC )
            return false;

        CHECK( format == wxIPC_TEXT );

        wxString s(static_cast<const char*>(data), size);

        if (item == "SimpleAdvise test")
        {
            if ( s == "OK SimpleAdvise" )
                m_advise_complete = true;
            else
                m_general_error << "SimpleAdvise: unexpected payload: " << s << '\n';
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


    virtual bool OnDisconnect() override;

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
    wxCRIT_SECT_LOCKER(lock, m_cs_assign_buffer);

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

    wxConnectionBase* OnMakeConnection() override
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
static wxEventLoop *gs_clientLoop = nullptr;

static bool PumpConnect(const wxString& host,
                        const wxString& service,
                        const wxString& topic)
{
    return gs_client->Connect(host, service, topic);
}

void IPCClientDispatch(unsigned long timeoutMs)
{
    if ( !gs_clientLoop )
        return;

    // The client loop is already active for the lifetime of IPCFixture, so do
    // NOT re-activate it per call: that writes ms_activeLoop and races the
    // worker threads reading it via CallAfter() -> WakeUpIdle().

    // Run any queued CallAfter() work first: worker threads marshal their IPC
    // socket I/O to the main thread via wxTCPEventHandler::RunOnMainThread(),
    // which posts async method-call events. DispatchTimeout() only services FD
    // (socket) events, so without this the marshaled jobs would never run.
    if ( wxTheApp )
        wxTheApp->ProcessPendingEvents();

    gs_clientLoop->DispatchTimeout(timeoutMs);
}

static void PumpDispatch()
{
    IPCClientDispatch(10);
}

static void DrainPendingIPCEvents()
{
    if ( gs_clientLoop )
    {
        // No per-call activation: the client loop is already active via IPCFixture
        // (and DispatchTimeout()/Pending() act on the loop object directly).
        for ( int i = 0; i < 100; ++i )
        {
            if ( !gs_clientLoop->Pending() )
                break;

            gs_clientLoop->DispatchTimeout(10);
        }
    }

    if ( wxTheApp )
    {
        for ( int i = 0; i < 100; ++i )
        {
            if ( !wxTheApp->Pending() )
                break;

            wxTheApp->ProcessPendingEvents();
        }
    }
}

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

        // Resolve the connection on the main thread. GetConn() uses
        // REQUIRE(), a Catch2 macro that is not thread-safe, so it
        // must not run on the worker thread in Entry(). The
        // connection is stable for our lifetime, so caching the
        // pointer is safe.
        m_conn = &gs_client->GetConn();

        Create();
    }

protected:
    virtual void *Entry() override
    {
        IPCTestConnection& conn = *m_conn;

        for (size_t n=1; n < MESSAGE_ITERATIONS + 1; n++)
        {
            wxString s = m_label + wxString::Format(" %zu", n);
            size_t size=0;
            const char* data = (const char*) conn.Request(s, &size, wxIPC_PRIVATE);

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
    IPCTestConnection* m_conn = nullptr;

    wxDECLARE_NO_COPY_CLASS(MultiRequestThread);
};

// A deadlock cannot be detected from the main thread, because the main thread
// is precisely what gets stuck. This watchdog runs on its own thread and aborts
// the process with a diagnostic if the test does not signal completion in time,
// turning an otherwise indefinite hang into a clear, bounded failure.
class DeadlockWatchdog : public wxThread
{
public:
    explicit DeadlockWatchdog(int timeoutMs, const wxString& what = "the IPC test")
        : wxThread(wxTHREAD_JOINABLE), m_timeoutMs(timeoutMs), m_what(what) {}

    // Called by the test once it has completed normally.
    void Done() { m_done.store(true); }

protected:
    void* Entry() override
    {
        const int step = 50;
        for ( int waited = 0; waited < m_timeoutMs; waited += step )
        {
            if ( m_done.load() )
                return nullptr;
            wxMilliSleep(step);
        }

        std::cerr << "\nIPC WATCHDOG: " << m_what
                  << " did not complete within " << m_timeoutMs
                  << " ms; aborting to avoid a CI hang.\n" << std::flush;
        abort();
    }

    const int m_timeoutMs;
    const wxString m_what;
    std::atomic<bool> m_done{false};

    wxDECLARE_NO_COPY_CLASS(DeadlockWatchdog);
};

// RAII wrapper that runs a DeadlockWatchdog for its whole lifetime. Used as the
// first member of IPCFixture so a watchdog covers the entire fixture (setup,
// test body, and teardown): if any of them blocks, e.g. a socket Connect()
// that never returns under an environment where the test server can't run, as
// on the Wine-based wxMSW cross-builds. The watchdog aborts with a diagnostic
// after the timeout instead of letting CI hang until its multi-hour job cap.
class FixtureWatchdog
{
public:
    FixtureWatchdog(int timeoutMs, const wxString& what)
        : m_watchdog(timeoutMs, what) { m_watchdog.Run(); }
    ~FixtureWatchdog() { m_watchdog.Done(); m_watchdog.Wait(); }

private:
    DeadlockWatchdog m_watchdog;
    wxDECLARE_NO_COPY_CLASS(FixtureWatchdog);
};

// IPCFixture starts a fresh IPC server for each test, connects a client to it,
// and shuts the server down again in the destructor, so no IPC server process
// is ever alive between tests, or during the unrelated GUI control tests that
// run in the same test_gui binary.
//
// (A previous version shared one long-lived server across all IPC tests to dodge
// an intermittent Wine Connect() hang caused by rapidly restarting the localhost
// listener. But that left the server (a second GUI process on the one Xvfb
// display) alive for the rest of the run, where it stole window-from-point and
// focus from the client and failed ~30 non-IPC GUI tests, guifuncs/treectrl/
// listbase/etc. The hang is now prevented at its source instead: the client's
// connect attempt is time-bounded (wxIPCTimeout, see wxTCPClient::MakeConnection)
// so a not-yet-ready server fails the attempt promptly and the readiness poll
// below retries, rather than blocking on the socket's long default timeout.)
class IPCFixture
{
    // Declared first so it is constructed first and destroyed last: the watchdog
    // then covers the whole fixture lifetime (setup, test body, teardown).
    FixtureWatchdog m_watchdog{180000, "an IPC test (setup, body, or teardown)"};
    std::unique_ptr<wxEventLoop> m_clientLoop{new wxEventLoop};
    std::unique_ptr<wxEventLoopActivator> m_loopActivator;
    IPCServerThread m_server;

public:
    IPCFixture()
    {
#if wxUSE_SOCKETS_FOR_IPC
        wxSocketBase::Initialize();
#endif // wxUSE_SOCKETS_FOR_IPC

        DrainPendingIPCEvents();

        gs_clientLoop = m_clientLoop.get();

        // Activate the client loop once for the lifetime of this fixture so its
        // worker threads see a stable wxEventLoopBase::ms_activeLoop. Activating
        // per IPCClientDispatch() call would write ms_activeLoop and race the
        // workers' CallAfter() -> WakeUpIdle() -> GetActive() reads.
        m_loopActivator.reset(new wxEventLoopActivator(m_clientLoop.get()));

        gs_client = new IPCTestClient;

        REQUIRE( m_server.Start() );

        // Wait until the freshly-launched server is ready to accept a
        // connection: the re-exec'd server process can take a while to come up
        // (well over a second under sanitizers, or as a GUI process doing full
        // toolkit init). Poll with a throwaway connection until one succeeds,
        // then drop it so the test starts from a clean state. The bound is
        // wall-clock based, not iteration based: in a GUI event loop
        // IPCClientDispatch() returns at once (idle events), so a fixed
        // iteration count would expire before a GUI server is listening. Each
        // Connect() attempt is itself time-bounded (see the class comment), so a
        // not-yet-ready server fails promptly and we retry rather than blocking.
        bool serverReady = false;
        wxStopWatch sw;
        while ( !serverReady && sw.Time() < 30000 )   // up to 30s
        {
            if ( gs_client->Connect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) )
            {
                gs_client->Disconnect();
                serverReady = true;
            }
            else
            {
                // Pace the retries: a refused Connect() returns at once, and
                // IPCClientDispatch() also returns immediately when idle, so
                // without a sleep this loop spins hundreds of connect attempts
                // (each creating and destroying a socket) while the server comes
                // up, enough churn to exhaust resources over a whole run of
                // per-test servers. Sleep briefly so we poll ~20x/second instead.
                IPCClientDispatch(50);
                wxMilliSleep(50);
            }
        }
        REQUIRE( serverReady );
    }

    ~IPCFixture()
    {
        // Ask the server to shut down cleanly, then wait for the child to exit.
        // This runs while the client loop is still active, so the server's
        // wxProcess::OnTerminate() is dispatched before its handler is destroyed
        // (see PumpForProcessExit() in the server launcher).
        if ( gs_client )
        {
            if ( !gs_client->m_conn )
                PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC);

            if ( gs_client->m_conn )
            {
                const wxString s("shutdown");
                gs_client->GetConn().Execute(s);
                wxMilliSleep(100);
            }
        }

        m_server.WaitForExit();

        DrainPendingIPCEvents();

        if ( gs_client )
            gs_client->Disconnect();

        DrainPendingIPCEvents();

        m_loopActivator.reset(); // restore the previously-active loop, once
        gs_clientLoop = nullptr;
        m_clientLoop.reset();

        delete gs_client;
        gs_client = nullptr;

#if wxUSE_SOCKETS_FOR_IPC
        wxSocketBase::Shutdown();
#endif // wxUSE_SOCKETS_FOR_IPC

        if ( g_show_message_timing )
            std::cout << "teardown complete\n" << std::flush;
    }
};

// Test the basics of Connect()
TEST_CASE_METHOD(IPCFixture,
                 "IPC::Connect", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test Connect\n" << std::flush;

    // connecting to the wrong port should fail
    CHECK( !PumpConnect("localhost", "2424", IPC_TEST_TOPIC) );

    // connecting with the wrong topic should fail
    CHECK( !PumpConnect("localhost", IPC_TEST_PORT, "VCP GRFG") );

    // Connecting to the right port on the right topic should succeed.
    REQUIRE( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );
}

// Test the basics of Request(): A Request() goes out and it should result in
// a reply from the server.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::SingleRequest", "[net][ipc][single_command]")
{
    if ( g_show_message_timing )
        std::cout << "Running test SingleRequest\n" << std::flush;

    // If the connection itself failed there is no point in probing
    // the server, and it distinguishes a connect failure from a
    // Request() failure below.
    REQUIRE( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();

    const wxString s("ping");
    size_t size=0;
    const char* data = (char*) conn.Request( s, &size, wxIPC_PRIVATE);

    // Guard against a null return before constructing a wxString from it:
    // a failed Request() must report cleanly instead of dereferencing null
    // (this was an information-free SIGSEGV on wxMSW). size is logged to help
    // diagnose why the very first post-connect Request would fail.
    INFO( "Request() returned size=" << size );
    REQUIRE( data != nullptr );

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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    MultiRequestThread thread1("MultiRequest thread 1");
    thread1.Run();
    WaitForThreadWithDispatch(thread1);

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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    MultiRequestThread thread1("MultiRequest thread 1");
    MultiRequestThread thread2("MultiRequest thread 2");
    MultiRequestThread thread3("MultiRequest thread 3");

    thread1.Run();
    thread2.Run();
    thread3.Run();

    WaitForThreadWithDispatch(thread1);
    WaitForThreadWithDispatch(thread2);
    WaitForThreadWithDispatch(thread3);

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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "SimpleAdvise test";

    CHECK( conn.StartAdvise(item) );

    // Wait a maximum of 2 seconds for completion. The bound is wall-clock based,
    // not iteration based: under a GUI event loop PumpDispatch() returns at once
    // (idle events), so a fixed iteration count would expire almost immediately,
    // before the server's advise arrives.
    wxStopWatch sw;
    while ( sw.Time() < 2000 && !conn.m_advise_complete )
    {
        PumpDispatch();
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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "MultiAdvise test";

    CHECK( conn.StartAdvise(item) );

    // Wait a maximum of 20 seconds for completion (wall-clock bounded; see the
    // note in IPC::SingleAdvise about GUI event loops and PumpDispatch()).
    wxStopWatch sw;
    while ( sw.Time() < 20000 &&
            conn.m_thread1_advise_lastval != MESSAGE_ITERATIONS )
    {
        PumpDispatch();
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

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );

    IPCTestConnection& conn = gs_client->GetConn();
    wxString item = "MultiAdvise MultiThread test";

    CHECK( conn.StartAdvise(item) );

    // Wait a maximum of 20 seconds for completion (wall-clock bounded; see the
    // note in IPC::SingleAdvise about GUI event loops and PumpDispatch()).
    wxStopWatch sw;
    while ( sw.Time() < 20000 )
    {
        PumpDispatch();

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

// Run three concurrent threads in the client sending Requests() to the
// server, and simultaneously run three concurrent threads in the server
// sending Advise() information to the client. Verify that all messages are
// serial and correspond to the correct thread. Lastly, verify that the server
// encountered no errors during this test.
//
// By setting g_show_message_timing to "true", the ordering of the Requests
// and Advise's can be seen. Different systems may need to change the delay
// wxMilliSleep in the client and server threads to make the interleave happen
// properly, which is a stringent test of race conditions that might be present
// in wxIPC.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::AdviseAndRequestMultiThread", "[net][ipc][multi_thread]")
{
    if ( g_show_message_timing )
        std::cout << "Running test MultiAdvise MultiThreads test with simultaneous MultiRequests MultiThreads\n" << std::flush;

    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );
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

    // Phase 1: Request() threads process interleaved Advise() via FindMessage().
    // Do not PumpDispatch() here: main-thread dispatch races worker Request().
    WaitForThreadWithDispatch(thread1);
    WaitForThreadWithDispatch(thread2);
    WaitForThreadWithDispatch(thread3);

    // Phase 2: dispatch any remaining Advise() notifications on the main thread.
    // Wall-clock bounded; see the note in IPC::SingleAdvise about GUI event loops.
    wxStopWatch sw;
    while ( sw.Time() < 20000 )
    {
        PumpDispatch();

        if ( conn.m_thread1_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread2_advise_lastval == MESSAGE_ITERATIONS &&
             conn.m_thread3_advise_lastval == MESSAGE_ITERATIONS )
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

// Exercises the case where a Request() is issued on the main thread while a
// worker thread is also issuing Request()s on the same connection, which was
// a source of several problems.
//
// When this test fails, the watchdog bounds the time-to-failure; the test should
// complete near-instantly if main-thread and worker-thread Request()s are
// properly serialized.
TEST_CASE_METHOD(IPCFixture,
                 "IPC::ConcurrentMainAndWorkerRequest", "[net][ipc][multi_command]")
{
    CHECK( PumpConnect("localhost", IPC_TEST_PORT, IPC_TEST_TOPIC) );
    IPCTestConnection& conn = gs_client->GetConn();

    // Generous timeout: the test completes in ~1-2s when healthy, so the
    // watchdog only ever fires on a *permanent* deadlock (which never recovers).
    // A large value avoids spurious aborts on slow/loaded CI runners or under
    // sanitizers, at no cost to the passing case.
    DeadlockWatchdog watchdog(30000,
        "concurrent main- and worker-thread Request() on the same connection");
    watchdog.Run();

    MultiRequestThread worker("MultiRequest thread 1");
    worker.Run();

    // Hammer the connection from the main thread while the worker does the same
    // from its thread. Pump between requests so that, absent the deadlock, the
    // worker's marshalled socket I/O can run on the main thread.
    while ( worker.IsRunning() )
    {
        size_t size = 0;
        const char* pong = (const char*) conn.Request("ping", &size, wxIPC_PRIVATE);

        CHECK( pong != nullptr );
        if ( pong )
            CHECK( wxString(pong) == "pong" );

        IPCClientDispatch(5);
    }

    worker.Wait();
    watchdog.Done();
    watchdog.Wait();

    INFO( worker.m_error );
    CHECK( worker.m_error.IsEmpty() );
}

#endif // wxUSE_THREADS && !__WXQT__
