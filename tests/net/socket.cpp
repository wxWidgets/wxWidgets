///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/socket.cpp
// Purpose:     wxSocket unit tests
// Author:      Vadim Zeitlin
// Copyright:   (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    IMPORTANT NOTE: the environment variable WX_TEST_SERVER must be set to the
    hostname of the server to use for the tests below, if it is not set all
    tests are silently skipped (rationale: this makes it possible to run the
    test in the restricted environments (e.g. sandboxes) without any network
    connectivity).
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"


#if wxUSE_SOCKETS

#include "wx/socket.h"
#include "wx/url.h"
#include "wx/sstream.h"
#include "wx/evtloop.h"

#include <memory>

typedef std::unique_ptr<wxSockAddress> wxSockAddressPtr;
typedef std::unique_ptr<wxSocketClient> wxSocketClientPtr;

static wxString gs_serverHost(wxGetenv("WX_TEST_SERVER"));

namespace
{

// helper event loop class which sets itself as active only if we pass it
// true in ctor
class SocketTestEventLoop : public wxEventLoop
{
public:
    SocketTestEventLoop(bool useLoop)
    {
        m_useLoop = useLoop;
        if ( useLoop )
        {
            m_evtLoopOld = wxEventLoopBase::GetActive();
            SetActive(this);
        }
    }

    virtual ~SocketTestEventLoop()
    {
        if ( m_useLoop )
        {
            wxEventLoopBase::SetActive(m_evtLoopOld);
        }
    }

private:
    bool m_useLoop;
    wxEventLoopBase *m_evtLoopOld;
};

// get the address to connect to, if nullptr is returned it means that the
// test is disabled and shouldn't run at all
wxSockAddress* GetServer()
{
    if ( gs_serverHost.empty() )
        return nullptr;

    wxIPV4address *addr = new wxIPV4address;
    addr->Hostname(gs_serverHost);
    addr->Service("www");

    return addr;
}

// get the socket to read HTTP reply from, returns nullptr if the test is
// disabled
wxSocketClient* GetHTTPSocket(int flags = wxSOCKET_NONE)
{
    wxSockAddress *addr = GetServer();
    if ( !addr )
        return nullptr;

    wxSocketClient *sock = new wxSocketClient(flags);
    sock->SetTimeout(1);
    REQUIRE( sock->Connect(*addr) );

    const wxString httpGetRoot =
        "GET / HTTP/1.1\r\n"
        "Host: " + gs_serverHost + "\r\n"
        "\r\n";

    sock->Write(httpGetRoot.ToAscii(), httpGetRoot.length());

    return sock;
}

} // anonymous namespace

// Note that the tests using SocketTestEventLoop below are run twice, with and
// without an active event loop, as sockets behave differently in these two
// cases.

TEST_CASE("Socket::BlockingConnect", "[socket]")
{
    wxSockAddressPtr addr(GetServer());
    if ( !addr.get() )
        return;

    wxSocketClient sock;
    CHECK( sock.Connect(*addr) );
}

TEST_CASE("Socket::NonblockingConnect", "[socket]")
{
    wxSockAddressPtr addr(GetServer());
    if ( !addr.get() )
        return;

    const bool useLoop = GENERATE(false, true);
    CAPTURE(useLoop);

    SocketTestEventLoop loop(useLoop);

    wxSocketClient sock;
    sock.Connect(*addr, false);
    sock.WaitOnConnect(10);

    CHECK( sock.IsConnected() );
}

TEST_CASE("Socket::ReadNormal", "[socket]")
{
    const bool useLoop = GENERATE(false, true);
    CAPTURE(useLoop);

    SocketTestEventLoop loop(useLoop);

    wxSocketClientPtr sock(GetHTTPSocket());
    if ( !sock.get() )
        return;

    char bufSmall[128];
    sock->Read(bufSmall, WXSIZEOF(bufSmall));

    CHECK( sock->LastError() == wxSOCKET_NOERROR );
    CHECK( (size_t)sock->LastCount() == WXSIZEOF(bufSmall) );
    CHECK( (size_t)sock->LastReadCount() == WXSIZEOF(bufSmall) );


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CHECK( sock->LastError() == wxSOCKET_NOERROR );
    CHECK( WXSIZEOF(bufBig) >= sock->LastCount() );
    CHECK( WXSIZEOF(bufBig) >= sock->LastReadCount() );
}

TEST_CASE("Socket::ReadBlock", "[socket]")
{
    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_BLOCK));
    if ( !sock.get() )
        return;

    char bufSmall[128];
    sock->Read(bufSmall, WXSIZEOF(bufSmall));

    CHECK( sock->LastError() == wxSOCKET_NOERROR );
    CHECK( (size_t)sock->LastCount() == WXSIZEOF(bufSmall) );
    CHECK( (size_t)sock->LastReadCount() == WXSIZEOF(bufSmall) );


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CHECK( sock->LastError() == wxSOCKET_NOERROR );
    CHECK( WXSIZEOF(bufBig) >= sock->LastCount() );
    CHECK( WXSIZEOF(bufBig) >= sock->LastReadCount() );
}

TEST_CASE("Socket::ReadNowait", "[socket]")
{
    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_NOWAIT));
    if ( !sock.get() )
        return;

    char buf[1024];
    sock->Read(buf, WXSIZEOF(buf));
    if ( sock->LastError() != wxSOCKET_WOULDBLOCK )
    {
        CHECK( sock->LastError() == wxSOCKET_NOERROR );
    }
}

TEST_CASE("Socket::ReadWaitall", "[socket]")
{
    const bool useLoop = GENERATE(false, true);
    CAPTURE(useLoop);

    SocketTestEventLoop loop(useLoop);

    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_WAITALL));
    if ( !sock.get() )
        return;

    char buf[128];
    sock->Read(buf, WXSIZEOF(buf));

    CHECK( sock->LastError() == wxSOCKET_NOERROR );
    CHECK( (size_t)sock->LastCount() == WXSIZEOF(buf) );
    CHECK( (size_t)sock->LastReadCount() == WXSIZEOF(buf) );
}

TEST_CASE("Socket::ReadAnotherThread", "[socket]")
{
    class SocketThread : public wxThread
    {
    public:
        SocketThread()
            : wxThread(wxTHREAD_JOINABLE)
        {
        }

        virtual void* Entry() override
        {
            wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_BLOCK));
            if ( !sock )
                return nullptr;

            char bufSmall[128];
            sock->Read(bufSmall, WXSIZEOF(bufSmall));

            REQUIRE( sock->LastError() == wxSOCKET_NOERROR );
            CHECK( sock->LastCount() == WXSIZEOF(bufSmall) );
            CHECK( sock->LastReadCount() == WXSIZEOF(bufSmall) );

            REQUIRE_NOTHROW( sock.reset() );

            return nullptr;
        }
    };

    SocketThread thr;

    const bool useLoop = GENERATE(false, true);
    CAPTURE(useLoop);

    SocketTestEventLoop loop(useLoop);

    thr.Run();

    CHECK( thr.Wait() == nullptr );
}

TEST_CASE("Socket::Url", "[socket]")
{
#if wxUSE_URL
    if ( gs_serverHost.empty() )
        return;

    const bool useLoop = GENERATE(false, true);
    CAPTURE(useLoop);

    SocketTestEventLoop loop(useLoop);

    wxURL url("http://" + gs_serverHost);

    const std::unique_ptr<wxInputStream> in(url.GetInputStream());
    CHECK( in.get() );

    wxStringOutputStream out;
    CHECK( in->Read(out).GetLastError() == wxSTREAM_EOF );
#endif
}

TEST_CASE("wxDatagramSocket::ShortRead", "[socket][dgram]")
{
    // Check that reading fewer bytes than are present in a
    // datagram does not leave the socket in an error state

    wxIPV4address addr;
    addr.LocalHost();
    addr.Service(19898);// Arbitrary port number
    wxDatagramSocket sock(addr);

    // Send ourselves a datagram
    unsigned int sendbuf[4] = {1, 2, 3, 4};
    sock.SendTo(addr, sendbuf, sizeof(sendbuf));

    // Read less than we know we sent
    unsigned int recvbuf[1] = {0};
    sock.Read(recvbuf, sizeof(recvbuf));
    CHECK(!sock.Error());
    CHECK(sock.LastReadCount() == sizeof(recvbuf));
    CHECK(recvbuf[0] == sendbuf[0]);
}

TEST_CASE("wxDatagramSocket::ShortPeek", "[socket][dgram]")
{
    // Check that peeking fewer bytes than are present in a datagram
    // does not lose the rest of the data in that datagram (#23594)

    wxIPV4address addr;
    addr.LocalHost();
    addr.Service(27384);// Arbitrary port number
    wxDatagramSocket sock(addr);

    // Send ourselves 2 datagrams
    unsigned int sendbuf1[2] = {1, 2};
    sock.SendTo(addr, sendbuf1, sizeof(sendbuf1));
    unsigned int sendbuf2[2] = {3, 4};
    sock.SendTo(addr, sendbuf2, sizeof(sendbuf2));

    long timeout_s = 1;
    if ( !sock.WaitForRead(timeout_s) )
        return;

    // Peek the first word
    unsigned int peekbuf[1] = {0};
    sock.Peek(peekbuf, sizeof(peekbuf));
    CHECK(sock.LastCount() == sizeof(peekbuf));
    CHECK(peekbuf[0] == sendbuf1[0]);

    // Read the whole of the first datagram
    unsigned int recvbuf[2] = {0};
    sock.Read(recvbuf, sizeof(recvbuf));
    CHECK(sock.LastReadCount() == sizeof(recvbuf));
    CHECK(recvbuf[0] == sendbuf1[0]);
    CHECK(recvbuf[1] == sendbuf1[1]);
}

#endif // wxUSE_SOCKETS
