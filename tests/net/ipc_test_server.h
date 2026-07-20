///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/ipc_test_server.h
// Purpose:     IPC test server (in-process on Windows, same-binary subprocess
//              on Unix where wxSocket IPC requires main-thread event dispatch)
// Author:      JP Mattia
// Copyright:   (c) 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_NET_IPC_TEST_SERVER_H_
#define _WX_TESTS_NET_IPC_TEST_SERVER_H_

#include <wx/evtloop.h>
#include <wx/thread.h>

// Match the guard in tests/net/ipc.cpp and ipc_test_server.cpp: the IPC test is
// excluded from wxQt, so its declarations must be too.
#if wxUSE_THREADS && !defined(__WXQT__)

// Starts the IPC test server and blocks until it is listening (or failed).
// Stops the server in WaitForExit().
class IPCServerThread
{
public:
    IPCServerThread();
    ~IPCServerThread();

    bool Start();
    void WaitForExit();

private:
    struct Private;
    Private* m_priv;

    wxDECLARE_NO_COPY_CLASS(IPCServerThread);
};

// Called from tests/test.cpp when WX_IPC_TEST_SERVER is set in the environment.
void RunIPCServerUntilStopped();

// Dispatch client-side socket events (implemented in ipc.cpp).
void IPCClientDispatch(unsigned long timeoutMs = 10);

// Wait for a joinable thread to finish while pumping the client event loop.
// Worker threads marshal their IPC socket I/O to the main thread (see
// wxTCPEventHandler::RunOnMainThread), so we must keep dispatching here for those
// marshaled jobs to run; otherwise the worker blocks forever. This is safe now
// that workers no longer touch the socket themselves (which is what previously
// made re-entrant dispatch corrupt the connection).
inline void WaitForThreadWithDispatch(wxThread& thread)
{
    while ( thread.IsRunning() )
        IPCClientDispatch(10);

    thread.Wait();
}

#endif // wxUSE_THREADS && !__WXQT__

#endif // _WX_TESTS_NET_IPC_TEST_SERVER_H_
