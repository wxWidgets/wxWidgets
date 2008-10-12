///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/socket.cpp
// Purpose:     wxSocket unit tests
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
// Licence:     wxWidgets licence
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
    CPPUNIT_TEST_SUITE( SocketTestCase );
        CPPUNIT_TEST( BlockingConnect );
        CPPUNIT_TEST( NonblockingConnect );
        CPPUNIT_TEST( ReadNormal );
        CPPUNIT_TEST( ReadBlock );
        CPPUNIT_TEST( ReadNowait );
        CPPUNIT_TEST( ReadWaitall );
    CPPUNIT_TEST_SUITE_END();

    // get the address to connect to, if NULL is returned it means that the
    // test is disabled and shouldn't run at all
    wxSockAddressPtr GetServer() const;

    // get the socket to read HTTP reply from, returns NULL if the test is
    // disabled
    wxSocketClientPtr GetHTTPSocket(int flags = wxSOCKET_NONE) const;

    void BlockingConnect();
    void NonblockingConnect();
    void ReadNormal();
    void ReadBlock();
    void ReadNowait();
    void ReadWaitall();

    DECLARE_NO_COPY_CLASS(SocketTestCase)
};

CPPUNIT_TEST_SUITE_REGISTRATION( SocketTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SocketTestCase, "SocketTestCase" );

wxSockAddressPtr SocketTestCase::GetServer() const
{
    wxSockAddressPtr ptr;
    if ( !gs_serverHost.empty() )
    {
        wxIPV4address *addr = new wxIPV4address;
        addr->Hostname(gs_serverHost);
        addr->Service("www");

        ptr.reset(addr);
    }

    return ptr;
}

wxSocketClientPtr SocketTestCase::GetHTTPSocket(int flags) const
{
    wxSocketClientPtr ptr;

    wxSockAddressPtr addr = GetServer();
    if ( !addr.get() )
        return ptr;

    wxSocketClient *sock = new wxSocketClient(flags);
    sock->SetTimeout(1);
    CPPUNIT_ASSERT( sock->Connect(*addr) );

    const wxString httpGetRoot =
        "GET / HTTP/1.1\r\n"
        "Host: " + gs_serverHost + "\r\n"
        "\r\n";

    sock->Write(httpGetRoot.ToAscii(), httpGetRoot.length());

    ptr.reset(sock);
    return ptr;
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

    wxEventLoopGuarantor loop;

    wxSocketClient sock;
    sock.Connect(*addr, false);
    sock.WaitOnConnect(10);

    CPPUNIT_ASSERT( sock.IsConnected() );
}

void SocketTestCase::ReadNormal()
{
    wxEventLoopGuarantor loop;

    wxSocketClientPtr sock(GetHTTPSocket());
    if ( !sock.get() )
        return;

    char bufSmall[128];
    sock->Read(bufSmall, WXSIZEOF(bufSmall));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), sock->LastCount() );


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
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(bufSmall), sock->LastCount() );


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
    wxEventLoopGuarantor loop;

    wxSocketClientPtr sock(GetHTTPSocket(wxSOCKET_WAITALL));
    if ( !sock.get() )
        return;

    char buf[128];
    sock->Read(buf, WXSIZEOF(buf));

    CPPUNIT_ASSERT_EQUAL( wxSOCKET_NOERROR, sock->LastError() );
    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(buf), sock->LastCount() );
}

#endif // wxUSE_SOCKETS
