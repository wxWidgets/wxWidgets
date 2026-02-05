///////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/ipc_setup_test.h
// Purpose:     IPC classes unit tests
// Author:      Vadim Zeitlin
// Copyright:   (c) 2008
// Modified by: JP Mattia, 2024
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_NET_IPC_H_
#define _WX_TESTS_NET_IPC_H_

// This test was written for sockets
#define wxUSE_SOCKETS_FOR_IPC 1
#define wxUSE_DDE_FOR_IPC     0

namespace
{
const char *IPC_TEST_PORT = "4242";
const char *IPC_TEST_TOPIC = "IPC TEST";

} // anonymous namespace

// Many IPC issues show up only after several iterations, and
// MESSAGE_ITERATIONS sets the number of iterations. The value of 20 is chosen
// as a compromise between exposing race conditions vs slowing down the
// overall automated test process.
//
// For stress-testing: if the number of MESSAGE_ITERATIONS is set much beyond
// 200, then the wait times should likely also be increased. Search for "wait
// a maximum" in ipc.cpp and sckipc_server to find those values.
#define MESSAGE_ITERATIONS 20
#define MESSAGE_ITERATIONS_STRING  wxString::Format("%d",MESSAGE_ITERATIONS)

#endif // _WX_TESTS_NET_IPC_H_
