///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/dgramsocket.cpp
// Purpose:     wxDatagramSocket unit tests
// Author:      Brian Nixon
// Copyright:   (c) 2023 Brian Nixon
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"


#if wxUSE_SOCKETS

#include "wx/socket.h"

TEST_CASE("wxDatagramSocket::SendPeek", "[socket][dgram]")
{
    wxIPV4address addr;
    addr.LocalHost();
    addr.Service(19898);// Arbitrary port number
    wxDatagramSocket sock(addr);
    unsigned int sendbuf[4] = {1, 2, 3, 4};
    unsigned int recvbuf[1] = {0};
    sock.SendTo(addr, sendbuf, sizeof(sendbuf));
    sock.Peek(recvbuf, sizeof(recvbuf));
    CHECK(!sock.Error());
    CHECK(recvbuf[0] == sendbuf[0]);
    sock.Read(recvbuf, sizeof(recvbuf));
    CHECK(!sock.Error());
    CHECK(recvbuf[0] == sendbuf[0]);
}

#endif // wxUSE_SOCKETS
