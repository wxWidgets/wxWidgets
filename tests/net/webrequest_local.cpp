/////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/webrequest_local.cpp
// Purpose:     wxWebRequest tests using a local HTTP server
// Author:      Vadim Zeitlin
// Created:     2026-09-20
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

// Unlike the tests in webrequest.cpp, the tests here don't need any external
// server and so are always run, but they do need sockets and threads to
// implement the trivial local server they use.
#if wxUSE_WEBREQUEST && wxUSE_SOCKETS && wxUSE_THREADS

#include "wx/webrequest.h"
#include "wx/evtloop.h"
#include "wx/socket.h"
#include "wx/thread.h"
#include "wx/timer.h"

#include <memory>

namespace
{

// The delay before the server replies, used by the fallback test below only,
// as the other tests don't need it and so don't waste any time on it.
//
// It must be comfortably greater than libcurl's "happy eyeballs" timeout
// (200ms by default): the timer used for the latter resumes the transfer even
// if the event loop source watching its socket has been lost, hiding the bug
// checked for there. This was verified experimentally: with the bug present,
// the test still passed with 200ms but reliably failed starting from 250ms,
// so the value below leaves a comfortable safety margin.
constexpr int SERVER_DELAY_MS = 500;

// Give up after this time if the request doesn't complete: this is not
// supposed to happen and 10 seconds are plenty for a local connection.
constexpr int REQUEST_TIMEOUT_MS = 10000;

// Trivial HTTP server replying to a single request from a worker thread.
class LocalServerThread : public wxThread
{
public:
    LocalServerThread(wxSocketServer& server, int delayMS)
        : wxThread(wxTHREAD_JOINABLE),
          m_server(server),
          m_delayMS(delayMS)
    {
        Create();
        Run();
    }

    const wxString& GetError() const { return m_error; }

private:
    void* Entry() override
    {
        if ( !m_server.WaitForAccept(10) )
        {
            m_error = "Timed out waiting for the connection";
            return nullptr;
        }

        const std::unique_ptr<wxSocketBase> conn{m_server.Accept(false)};
        if ( !conn )
        {
            m_error = "Failed to accept the connection";
            return nullptr;
        }

        conn->SetTimeout(10);

        // We don't care about the request contents, we just need to wait
        // until it arrives before replying to it.
        char buf[4096];
        conn->Read(buf, WXSIZEOF(buf));
        if ( !conn->LastReadCount() )
        {
            m_error = "Failed to read the request";
            return nullptr;
        }

        if ( m_delayMS )
            wxMilliSleep(m_delayMS);

        static const char response[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 2\r\n"
            "\r\n"
            "OK";
        conn->Write(response, WXSIZEOF(response) - 1);
        if ( conn->Error() )
            m_error = "Failed to write the response";

        return nullptr;
    }

    wxSocketServer& m_server;
    const int m_delayMS;
    wxString m_error;

    wxDECLARE_NO_COPY_CLASS(LocalServerThread);
};

class LocalServerFixture : public wxTimer
{
public:
    LocalServerFixture()
    {
        // Note that we listen on the IPv4 address only, this is important
        // for the fallback test below.
        wxIPV4address addr;
        addr.Hostname("127.0.0.1");
        addr.Service(0); // Use any free port.

        // Note that the socket must be blocking because it, and the socket
        // accepted from it, which inherits its flags, are used from another
        // thread below: enabling events for them would register callbacks
        // with the main thread event loop, which is not allowed for the
        // sockets used from the other threads and results in anything from
        // asserts to crashes, depending on the port.
        m_server.reset(new wxSocketServer(addr,
                                          wxSOCKET_REUSEADDR | wxSOCKET_BLOCK));
        REQUIRE( m_server->IsOk() );

        wxIPV4address local;
        REQUIRE( m_server->GetLocal(local) );
        m_port = local.Service();

        Bind(wxEVT_WEBREQUEST_STATE, &LocalServerFixture::OnRequestState, this);
    }

    // Perform a request to the given host, which must resolve to the local
    // machine, and check that it completes successfully.
    void CheckRequestTo(const wxString& host, int delayMS = 0)
    {
        LocalServerThread serverThread(*m_server, delayMS);

        const wxWebRequest request =
            RunRequest(wxString::Format("http://%s:%d/", host, m_port));

        CHECK( serverThread.Wait() == nullptr );
        CHECK( serverThread.GetError().empty() );

        INFO("Error: " << m_errorDescription);
        REQUIRE( m_state == wxWebRequest::State_Completed );
        CHECK( request.GetResponse().AsString() == "OK" );
    }

    // Check that a request to a port nobody listens on fails.
    void CheckRequestRefused()
    {
        RunRequest(wxString::Format("http://127.0.0.1:%d/", GetUnusedPort()));

        REQUIRE( m_state == wxWebRequest::State_Failed );
    }

private:
    // Return a port on the local machine on which nothing is listening.
    static int GetUnusedPort()
    {
        wxIPV4address addr;
        addr.Hostname("127.0.0.1");
        addr.Service(0);

        wxSocketServer server(addr, wxSOCKET_REUSEADDR);
        REQUIRE( server.IsOk() );

        wxIPV4address local;
        REQUIRE( server.GetLocal(local) );

        // The server is closed on leaving this function, so nothing is going
        // to be listening on this port any more.
        return local.Service();
    }

    // Run the request until it finishes, one way or another, or times out.
    wxWebRequest RunRequest(const wxString& url)
    {
        wxWebRequest request =
            wxWebSession::GetDefault().CreateRequest(this, url);
        REQUIRE( request.IsOk() );

        request.Start();

        StartOnce(REQUEST_TIMEOUT_MS);
        m_loop.Run();
        Stop();

        return request;
    }

    void OnRequestState(wxWebRequestEvent& event)
    {
        switch ( event.GetState() )
        {
            case wxWebRequest::State_Idle:
            case wxWebRequest::State_Active:
                return;

            case wxWebRequest::State_Unauthorized:
            case wxWebRequest::State_Completed:
            case wxWebRequest::State_Failed:
            case wxWebRequest::State_Cancelled:
                break;
        }

        m_state = event.GetState();
        m_errorDescription = event.GetErrorDescription();
        m_loop.Exit();
    }

    // Called if the request doesn't finish in REQUEST_TIMEOUT_MS.
    void Notify() override
    {
        m_errorDescription = "timed out";
        m_loop.Exit();
    }

    std::unique_ptr<wxSocketServer> m_server;
    int m_port = 0;

    wxEventLoop m_loop;
    wxWebRequest::State m_state = wxWebRequest::State_Idle;
    wxString m_errorDescription;
};

} // anonymous namespace

TEST_CASE_METHOD(LocalServerFixture,
                 "WebRequest::Local::Simple", "[net][webrequest]")
{
    CheckRequestTo("127.0.0.1");
}

// This test uses the "localhost" name instead of the address used by the one
// above on purpose: libcurl always maps this name to "::1" followed by
// "127.0.0.1", so, as our server listens on the IPv4 address only, the first
// connection attempt is refused and libcurl has to fall back to the next
// address.
//
// This used to make the request hang forever when using the curl backend with
// a GLib-based event loop, i.e. under wxGTK: poll() reports POLLOUT together
// with POLLERR for the refused connection and handling the latter destroyed
// the event loop source watching the socket of the new, working, connection,
// leaving it unwatched, so that the response was never read (see #27040).
//
// Note that this only happens if connecting to "::1" fails asynchronously,
// i.e. is refused, and not if it fails immediately because IPv6 is disabled
// entirely, in which case this test still passes even with the bug.
TEST_CASE_METHOD(LocalServerFixture,
                 "WebRequest::Local::AddressFallback", "[net][webrequest]")
{
    CheckRequestTo("localhost", SERVER_DELAY_MS);
}

// Refusing the connection is also how poll() comes to report POLLOUT together
// with POLLERR, which used to crash under wxGTK, as the handler used for the
// former was destroyed before the latter was processed (see #24885).
TEST_CASE_METHOD(LocalServerFixture,
                 "WebRequest::Local::Refused", "[net][webrequest]")
{
    CheckRequestRefused();
}

#endif // wxUSE_WEBREQUEST && wxUSE_SOCKETS && wxUSE_THREADS
