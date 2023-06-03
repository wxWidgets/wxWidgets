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
// and "wx/cppunit.h"
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

class SocketTestCase : public CppUnit::TestCase
{
public:
    SocketTestCase() { }

    // get the address to connect to, if nullptr is returned it means that the
    // test is disabled and shouldn't run at all
    static wxSockAddress* GetServer();

    // get the socket to read HTTP reply from, returns nullptr if the test is
    // disabled
    static wxSocketClient* GetHTTPSocket(int flags = wxSOCKET_NONE);

private:
    // we need to repeat the tests twice as the sockets behave differently when
    // there is an active event loop and without it
    #define ALL_SOCKET_TESTS() \
        CPPUNIT_TEST( BlockingConnect ); \
        CPPUNIT_TEST( NonblockingConnect ); \
        CPPUNIT_TEST( ReadNormal ); \
        CPPUNIT_TEST( ReadBlock ); \
        CPPUNIT_TEST( ReadNowait ); \
        CPPUNIT_TEST( ReadWaitall ); \
        CPPUNIT_TEST( ReadAnotherThread ); \
        CPPUNIT_TEST( UrlTest )

    CPPUNIT_TEST_SUITE( SocketTestCase );
        ALL_SOCKET_TESTS();
        CPPUNIT_TEST( PseudoTest_SetUseEventLoop );
        ALL_SOCKET_TESTS();
    CPPUNIT_TEST_SUITE_END();

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

    void PseudoTest_SetUseEventLoop() { ms_useLoop = true; }

    void BlockingConnect();
    void NonblockingConnect();
    void ReadNormal();
    void ReadBlock();
    void ReadNowait();
    void ReadWaitall();
    void ReadAnotherThread();

    void UrlTest();

    static bool ms_useLoop;

    wxDECLARE_NO_COPY_CLASS(SocketTestCase);
};

bool SocketTestCase::ms_useLoop = false;

CPPUNIT_TEST_SUITE_REGISTRATION( SocketTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SocketTestCase, "SocketTestCase" );

wxSockAddress* SocketTestCase::GetServer()
{
    if ( gs_serverHost.empty() )
        return nullptr;

    wxIPV4address *addr = new wxIPV4address;
    addr->Hostname(gs_serverHost);
    addr->Service("www");

    return addr;
}

wxSocketClient* SocketTestCase::GetHTTPSocket(int flags)
{
    wxSockAddress *addr = GetServer();
    if ( !addr )
        return nullptr;

    wxSocketClient *sock = new wxSocketClient(flags);
    sock->SetTimeout(1);
    CPPUNIT_ASSERT( sock->Connect(*addr) );

    const wxString httpGetRoot =
        "GET / HTTP/1.1\r\n"
        "Host: " + gs_serverHost + "\r\n"
        "\r\n";

    sock->Write(httpGetRoot.ToAscii(), httpGetRoot.length());

    return sock;
}

void SocketTestCase::BlockingConnect()
{
    wxSockAddressPtr addr(GetServer());
    if ( !addr.get() )
        return;

    wxSocketClient sock;
    CPPUNIT_ASSERT( sock.Connect(*addr) );
}

void SocketTestCase::NonblockingConnect()
{
    wxSockAddressPtr addr(GetServer());
    if ( !addr.get() )
        return;

    SocketTestEventLoop loop(ms_useLoop);

    wxSocketClient sock;
    sock.Connect(*addr, false);
    sock.WaitOnConnect(10);

    CPPUNIT_ASSERT( sock.IsConnected() );
}

void SocketTestCase::ReadNormal()
{
    SocketTestEventLoop loop(ms_useLoop);

    wxSocketClientPtr sock(GetHTTPSocket());
    if ( !sock.get() )
        return;

    char bufSmall[128];
    sock->Read(bufSmall, WXSIZEOF(bufSmall));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), (size_t)sock->LastCount() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), (size_t)sock->LastReadCount() );


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastCount() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastReadCount() );
}

void SocketTestCase::ReadBlock()
{
    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_BLOCK));
    if ( !sock.get() )
        return;

    char bufSmall[128];
    sock->Read(bufSmall, WXSIZEOF(bufSmall));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), (size_t)sock->LastCount() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), (size_t)sock->LastReadCount() );


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastCount() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastReadCount() );
}

void SocketTestCase::ReadNowait()
{
    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_NOWAIT));
    if ( !sock.get() )
        return;

    char buf[1024];
    sock->Read(buf, WXSIZEOF(buf));
    if ( sock->LastError() != wxSOCKET_WOULDBLOCK )
    {
        CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    }
}

void SocketTestCase::ReadWaitall()
{
    SocketTestEventLoop loop(ms_useLoop);

    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_WAITALL));
    if ( !sock.get() )
        return;

    char buf[128];
    sock->Read(buf, WXSIZEOF(buf));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(buf), (size_t)sock->LastCount() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(buf), (size_t)sock->LastReadCount() );
}

void SocketTestCase::ReadAnotherThread()
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
            wxSocketClientPtr sock(SocketTestCase::GetHTTPSocket(wxSOCKET_BLOCK));
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

    SocketTestEventLoop loop(ms_useLoop);

    thr.Run();

    CHECK( thr.Wait() == nullptr );
}

void SocketTestCase::UrlTest()
{
    if ( gs_serverHost.empty() )
        return;

    SocketTestEventLoop loop(ms_useLoop);

    wxURL url("http://" + gs_serverHost);

    const std::unique_ptr<wxInputStream> in(url.GetInputStream());
    CPPUNIT_ASSERT( in.get() );

    wxStringOutputStream out;
    CPPUNIT_ASSERT_EQUAL( wxSTREAM_EOF, in->Read(out).GetLastError() );
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
