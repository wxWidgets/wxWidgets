///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/ipc_test_server.cpp
// Purpose:     IPC test server for unit tests
// Authors:     Vadim Zeitlin, JP Mattia
// Copyright:   (c) 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

// Match the guard in tests/net/ipc.cpp

#if wxUSE_THREADS && !defined(__WXQT__)

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "ipc_test_server.h"
#include "ipc_setup_test.h"

#include <wx/ipc.h>
#include <wx/thread.h>
#include <wx/evtloop.h>
#include <wx/vector.h>
#include <wx/process.h>
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#ifdef __UNIX__
    #include <unistd.h>
#endif

#define MAX_MSG_BUFFERS 2048

class IPCServerTestServer;

class IPCServerConnection : public wxConnection
{
public:
    IPCServerConnection(IPCServerTestServer* server)
    {
        m_server = server;
        ResetThreadTrackers();

        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            m_bufferList[i] = nullptr;

        m_nextAvailable = 0;
    }

    ~IPCServerConnection()
    {
        m_advise_active = false;
        WaitForAdviseWorkers();

        for (int i = 0; i < MAX_MSG_BUFFERS; i++)
            if (m_bufferList[i])
                delete[] m_bufferList[i];
    }

    virtual bool OnExec(const wxString& topic, const wxString& data) override;
    virtual bool OnDisconnect() override;

    virtual const void* OnRequest(const wxString& topic,
                                  const wxString& item,
                                  size_t* size,
                                  wxIPCFormat format) override;

    virtual bool OnStartAdvise(const wxString& topic,
                               const wxString& item) override;

    virtual bool OnStopAdvise(const wxString& topic,
                              const wxString& item) override;

private:
    wxString HandleThreadRequestCounting(const wxString& item);
    void ResetThreadTrackers();
    void StartAdviseWorker(wxThread* thread);
    void WaitForAdviseWorkers();

    char* GetBufPtr(size_t size)
    {
        wxCRIT_SECT_LOCKER(lock, m_cs_assign_buffer);

        if (m_bufferList[m_nextAvailable] != nullptr)
            delete[] m_bufferList[m_nextAvailable];

        char* ptr = new char[size];

        m_bufferList[m_nextAvailable] = ptr;
        m_nextAvailable = (m_nextAvailable + 1) % MAX_MSG_BUFFERS;

        return ptr;
    }

    wxCRIT_SECT_DECLARE_MEMBER(m_cs_assign_buffer);

    IPCServerTestServer* m_server;

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

    wxVector<wxThread*> m_adviseThreads;

    wxDECLARE_NO_COPY_CLASS(IPCServerConnection);
};

class SingleAdviseThread : public wxThread
{
public:
    SingleAdviseThread(IPCServerTestServer* server, const wxString& item)
        : wxThread(wxTHREAD_JOINABLE),
          m_server(server),
          m_item(item)
    {
        Create();
    }

protected:
    virtual void *Entry() override;

    IPCServerTestServer* m_server;
    wxString m_item;

    wxDECLARE_NO_COPY_CLASS(SingleAdviseThread);
};

class MultiAdviseThread : public wxThread
{
public:
    MultiAdviseThread(IPCServerTestServer* server,
                      const wxString& item,
                      const wxString& label)
        : wxThread(wxTHREAD_JOINABLE),
          m_server(server),
          m_item(item),
          m_label(label)
    {
        Create();
    }

protected:
    virtual void *Entry() override;

    IPCServerTestServer* m_server;
    wxString m_item, m_label;

    wxDECLARE_NO_COPY_CLASS(MultiAdviseThread);
};

class IPCServerTestServer : public wxServer
{
public:
    IPCServerTestServer()
    {
        m_conn = nullptr;
    }

    virtual ~IPCServerTestServer()
    {
        if (m_conn)
            delete m_conn;
    }

    virtual wxConnectionBase *OnAcceptConnection(const wxString& topic) override
    {
        if ( topic != IPC_TEST_TOPIC )
            return nullptr;

        m_conn = new IPCServerConnection(this);
        return m_conn;
    }

    void Shutdown()
    {
        if (m_conn)
            m_conn->Disconnect();
    }

    IPCServerConnection& GetConn()
    {
        if (!m_conn)
        {
            m_conn = new IPCServerConnection(this);
            m_conn->m_general_error = "Invalid connection in GetConn()";
        }

        return *m_conn;
    }

    IPCServerConnection *m_conn;

    wxDECLARE_NO_COPY_CLASS(IPCServerTestServer);
};

bool IPCServerConnection::OnExec(const wxString& topic, const wxString& data)
{
    if ( topic != IPC_TEST_TOPIC )
        return false;

    if (data == "shutdown")
    {
        m_advise_active = false;
        WaitForAdviseWorkers();
        m_server->Shutdown();
        if ( wxEventLoopBase::GetActive() )
            wxEventLoopBase::GetActive()->ScheduleExit(0);
        else
            _exit(0);
        return true;
    }

    m_lastExecute = data;
    return true;
}

bool IPCServerConnection::OnDisconnect()
{
    // Clear the server's current-connection pointer only if *this* connection is
    // still the current one. Connections can briefly overlap, e.g. the fixture
    // probes server readiness with a throwaway connect/disconnect, and that
    // probe's disconnect can be processed after the real connection has already
    // been accepted and stored. Nulling unconditionally there would discard the
    // live connection, after which GetConn() fabricates an unconnected one and
    // server-side Advise() fails. (Surfaced under the GUI event loop, where the
    // overlap is more likely.)
    if ( m_server->m_conn == this )
        m_server->m_conn = nullptr;
    return wxConnection::OnDisconnect();
}

const void* IPCServerConnection::OnRequest(const wxString& topic,
                                         const wxString& item,
                                         size_t* size,
                                         wxIPCFormat format)
{
    *size = 0;

    if ( topic != IPC_TEST_TOPIC )
        return nullptr;

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
        s = wxString::Format("%d", m_thread2_request_lastval);
    }
    else if ( item == "get_thread3_request_counter")
    {
        s = wxString::Format("%d", m_thread3_request_lastval);
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

wxString IPCServerConnection::HandleThreadRequestCounting(const wxString& item)
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

void IPCServerConnection::ResetThreadTrackers()
{
    m_general_error = "";

    m_thread1_request_lastval = m_thread2_request_lastval =
        m_thread3_request_lastval = 0;

    m_advise_active = false;

    m_wait_for_first_request = false;
    m_first_request_received = false;
}

void IPCServerConnection::StartAdviseWorker(wxThread* thread)
{
    thread->Run();
    m_adviseThreads.push_back(thread);
}

void IPCServerConnection::WaitForAdviseWorkers()
{
    for ( wxThread* thread : m_adviseThreads )
    {
        // Wait() even if the thread has already finished: a joinable thread
        // must always be joined to release its resources.
        thread->Wait();

        delete thread;
    }

    m_adviseThreads.clear();
}

bool IPCServerConnection::OnStartAdvise(const wxString& topic,
                                      const wxString& item)
{
    if ( topic != IPC_TEST_TOPIC )
        return false;

    WaitForAdviseWorkers();

    m_advise_active = true;

    if (item == "SimpleAdvise test")
    {
        StartAdviseWorker(new SingleAdviseThread(m_server, item));
    }
    else if (item.StartsWith("MultiAdvise test"))
    {
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 1"));
    }
    else if (item.StartsWith("MultiAdvise MultiThread test"))
    {
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 1"));
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 2"));
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 3"));
    }
    else if (item.StartsWith("MultiAdvise MultiThread test with simultaneous Requests"))
    {
        m_wait_for_first_request = true;

        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 1"));
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 2"));
        StartAdviseWorker(
            new MultiAdviseThread(m_server, item, "MultiAdvise thread 3"));
    }
    else
    {
        m_general_error += "Unknown StartAdvise item\n";
        m_advise_active = false;
    }

    return m_advise_active;
}

bool IPCServerConnection::OnStopAdvise(const wxString& topic,
                                     const wxString& WXUNUSED(item))
{
    if ( topic != IPC_TEST_TOPIC )
        return false;

    m_advise_active = false;

    return true;
}

void* SingleAdviseThread::Entry()
{
    wxMilliSleep(50);

    IPCServerConnection& conn = m_server->GetConn();
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
    IPCServerConnection& conn = m_server->GetConn();

    if ( !conn.m_advise_active )
        conn.m_general_error = "Advise() call without StartAdvise()\n";

    if ( conn.m_wait_for_first_request )
        while ( !conn.m_first_request_received )
            wxMilliSleep(50);

    for (size_t n=1; n < MESSAGE_ITERATIONS + 1; n++)
    {
        if ( !conn.m_advise_active )
            break;

        wxMilliSleep(50);

        wxString s = m_label + wxString::Format(" %zu", n);
        size_t size = strlen(s.mb_str());

        if ( !conn.Advise(m_item, s.mb_str(), size, wxIPC_TEXT) )
        {
            conn.m_general_error
                += wxString::Format(m_label +
                                    "Advise() call returned false: %zu", n);
        }
    }

    return nullptr;
}

// ============================================================================
// In-process server context (server child process entry point)
// ============================================================================

class IPCServerContext
{
public:
    IPCServerContext()
        : m_server(new IPCServerTestServer()),
          m_started(false)
    {
    }

    ~IPCServerContext()
    {
        Stop();
        delete m_server;
    }

    bool Start()
    {
        wxEventLoopActivator activate(&m_loop);

        if ( !m_server->Create(IPC_TEST_PORT) )
            return false;

        m_started = true;
        return true;
    }

    void RunUntilStopped()
    {
        wxEventLoopActivator activate(&m_loop);
        m_loop.Run();
    }

    void Stop()
    {
        if ( !m_started )
            return;

        m_server->Shutdown();
        m_started = false;
    }

private:
    IPCServerTestServer* m_server;
    wxEventLoop m_loop;
    bool m_started;

    wxDECLARE_NO_COPY_CLASS(IPCServerContext);
};

void RunIPCServerUntilStopped()
{
#if wxUSE_SOCKETS
    wxSocketBase::Initialize();
#endif

    IPCServerContext ctx;

    if ( !ctx.Start() )
        _exit(1);

    ctx.RunUntilStopped();
}

// ============================================================================
// IPCServerThread: launch server via same test binary (no separate executable)
// ============================================================================

class IPCServerProcess : public wxProcess
{
public:
    IPCServerProcess() { m_finished = false; }

    virtual void OnTerminate(int pid, int status) override
    {
        wxUnusedVar(status);
        wxUnusedVar(pid);
        m_finished = true;
    }

    bool IsFinished() const { return m_finished; }

    bool m_finished;
};

struct IPCServerLaunchState
{
    IPCServerProcess* process;
    wxString command;
    long pid;

    IPCServerLaunchState() : process(new IPCServerProcess()), pid(0) {}
    ~IPCServerLaunchState() { delete process; }
};

// Pump the real event loop so async wxExecute() can deliver its process-exit
// notification (on MSW, a wxWM_PROC_TERMINATED window message handled by
// wxExecuteWindowCbk, which calls wxProcess::OnTerminate()). The IPC client's
// IPCClientDispatch() can't be used here: during end-of-run teardown the client
// loop is already gone (gs_clientLoop == nullptr) so it no-ops, and the exit
// notification would otherwise be delivered only later, after we've destroyed
// the IPCServerProcess handler, making wxExecuteWindowCbk dereference a freed
// wxProcess. Dispatching here lets OnTerminate() run (and wxExecute free its
// per-process data) while the handler is still alive.
static void PumpForProcessExit(unsigned long timeoutMs)
{
    if ( wxEventLoopBase* const active = wxEventLoopBase::GetActive() )
    {
        active->DispatchTimeout(timeoutMs);
    }
    else
    {
        wxEventLoop loop;
        wxEventLoopActivator activate(&loop);
        loop.DispatchTimeout(timeoutMs);
    }
}

class IPCServerLauncher : public wxTimer
{
public:
    IPCServerLauncher(IPCServerLaunchState* state)
        : m_state(state)
    {
        wxFileName fn(wxStandardPaths::Get().GetExecutablePath());
        m_state->command = fn.GetFullPath();
    }

    bool DoStart()
    {
        if ( m_state->pid )
            DoStop();

        wxEventLoop loop;
        Bind(wxEVT_TIMER, &IPCServerLauncher::OnTimer, this);
        StartOnce(10);
        loop.Run();

        return m_state->pid != 0;
    }

    void OnTimer(wxTimerEvent& WXUNUSED(event))
    {
        wxSetEnv("WX_IPC_TEST_SERVER", "1");

        // Pass the environment to the child explicitly. Relying on wxExecute()
        // to inherit a variable set via wxSetEnv() works for the console "test"
        // but not for the wxGTK GUI "test_gui": there the re-executed child did
        // not see WX_IPC_TEST_SERVER and ran the whole test suite instead of the
        // IPC server. Building the env map and handing it to wxExecute() makes
        // the hand-off reliable in both programs.
        wxExecuteEnv execEnv;
        wxGetEnvMap(&execEnv.env);
        execEnv.env["WX_IPC_TEST_SERVER"] = "1";

        m_state->pid = wxExecute(m_state->command, wxEXEC_ASYNC,
                                 m_state->process, &execEnv);

        if ( wxEventLoop::GetActive() )
            wxEventLoop::GetActive()->Exit();
    }

    void DoStop()
    {
        if ( m_state->pid && !m_state->process->IsFinished() )
        {
            wxKill(m_state->pid, wxSIGTERM);
            // Pump the real loop (not IPCClientDispatch) so the child's exit
            // notification reaches wxProcess::OnTerminate() before the handler
            // is destroyed; see PumpForProcessExit().
            for ( int i = 0; i < 250 && !m_state->process->IsFinished(); ++i )
            {
                PumpForProcessExit(20);
                wxMilliSleep(20);
            }
        }

        m_state->pid = 0;
        wxUnsetEnv("WX_IPC_TEST_SERVER");
    }

    void WaitUntilFinished()
    {
        for ( int i = 0; i < 100 && !m_state->process->IsFinished(); ++i )
        {
            PumpForProcessExit(20);
            wxMilliSleep(20);
        }
    }

    IPCServerLaunchState* m_state;
};

struct IPCServerThread::Private
{
    IPCServerLaunchState state;
};

IPCServerThread::IPCServerThread()
    : m_priv(new Private)
{
}

IPCServerThread::~IPCServerThread()
{
    WaitForExit();
    delete m_priv;
}

bool IPCServerThread::Start()
{
    IPCServerLauncher launcher(&m_priv->state);
    return launcher.DoStart();
}

void IPCServerThread::WaitForExit()
{
    if ( !m_priv->state.pid )
        return;

    IPCServerLauncher launcher(&m_priv->state);
    launcher.WaitUntilFinished();
    launcher.DoStop();
}

#endif // wxUSE_THREADS && !__WXQT__
