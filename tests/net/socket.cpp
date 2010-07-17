///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/socket.cpp
// Purpose:     wxSocket unit tests
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include "wx/socket.h"
#include "wx/url.h"
#include "wx/sstream.h"
#include "wx/evtloop.h"
#include <memory>

typedef std::auto_ptr<wxSockAddress> wxSockAddressPtr;
typedef std::auto_ptr<wxSocketClient> wxSocketClientPtr;

static wxString gs_serverHost(wxGetenv("WX_TEST_SERVER"));

class SocketTestCase : public CppUnit::TestCase
{
public:
    SocketTestCase() { }

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

    // get the address to connect to, if NULL is returned it means that the
    // test is disabled and shouldn't run at all
    wxSockAddressPtr GetServer() const;

    // get the socket to read HTTP reply from, returns NULL if the test is
    // disabled
    wxSocketClientPtr GetHTTPSocket(int flags = wxSOCKET_NONE) const;

    void PseudoTest_SetUseEventLoop() { ms_useLoop = true; }

    void BlockingConnect();
    void NonblockingConnect();
    void ReadNormal();
    void ReadBlock();
    void ReadNowait();
    void ReadWaitall();

    void UrlTest();

    static bool ms_useLoop;

    DECLARE_NO_COPY_CLASS(SocketTestCase)
};

bool SocketTestCase::ms_useLoop = false;

CPPUNIT_TEST_SUITE_REGISTRATION( SocketTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SocketTestCase, "SocketTestCase" );

wxSockAddressPtr SocketTestCase::GetServer() const
{
    if ( gs_serverHost.empty() )
        return wxSockAddressPtr();

    wxIPV4address *addr = new wxIPV4address;
    addr->Hostname(gs_serverHost);
    addr->Service("www");

    return wxSockAddressPtr(addr);
}

wxSocketClientPtr SocketTestCase::GetHTTPSocket(int flags) const
{
    wxSockAddressPtr addr = GetServer();
    if ( !addr.get() )
        return wxSocketClientPtr();

    wxSocketClient *sock = new wxSocketClient(flags);
    sock->SetTimeout(1);
    CPPUNIT_ASSERT( sock->Connect(*addr) );

    const wxString httpGetRoot =
        "GET / HTTP/1.1\r\n"
        "Host: " + gs_serverHost + "\r\n"
        "\r\n";

    sock->Write(httpGetRoot.ToAscii(), httpGetRoot.length());

    return wxSocketClientPtr(sock);
}

void SocketTestCase::BlockingConnect()
{
    wxSockAddressPtr addr = GetServer();
    if ( !addr.get() )
        return;

    wxSocketClient sock;
    CPPUNIT_ASSERT( sock.Connect(*addr) );
}

void SocketTestCase::NonblockingConnect()
{
    wxSockAddressPtr addr = GetServer();
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


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastCount() );
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


    char bufBig[102400];
    sock->Read(bufBig, WXSIZEOF(bufBig));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT( WXSIZEOF(bufBig) >= sock->LastCount() );
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
}

void SocketTestCase::UrlTest()
{
    if ( gs_serverHost.empty() )
        return;

    SocketTestEventLoop loop(ms_useLoop);

    wxURL url("http://" + gs_serverHost);

    const std::auto_ptr<wxInputStream> in(url.GetInputStream());
    CPPUNIT_ASSERT( in.get() );

    wxStringOutputStream out;
    CPPUNIT_ASSERT_EQUAL( wxSTREAM_EOF, in->Read(out).GetLastError() );
}

#endif // wxUSE_SOCKETS
