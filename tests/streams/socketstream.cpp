///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/socketstream.cpp
// Purpose:     Test wxSocketInputStream/wxSocketOutputStream
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/socket.h"
#include "wx/sckstrm.h"
#include "wx/thread.h"

#include "bstream.h"

namespace
{

const int TEST_PORT_READ = 0x7778;  // arbitrary, chosen because == "wx"
const int TEST_PORT_WRITE = 0x7779; // well, "wy"

// these cond and mutex are used to minimize the risk of the main thread
// Connect()-ing before this thread starts Accept()-ing connections but
// unfortunately we can't make this truly safe, see comment in
// SocketServerThread::Entry()
wxMutex gs_mutex;
wxCondition gs_cond(gs_mutex);
} // anonymous namespace

// return address for the given port on local host
static inline wxIPV4address LocalAddress(int port)
{
    wxIPV4address addr;
    addr.LocalHost();
    addr.Service(port);

    return addr;
}

// A thread which creates a listening socket on the specified port and executes
// the given function with each socket which connects to it
class SocketServerThread : public wxThread
{
public:
    // port is the port to listen on and function will be called on each
    // accepted socket
    SocketServerThread(int port, void (*accept)(wxSocketBase&))
        : wxThread(wxTHREAD_JOINABLE),
          m_port(port),
          m_accept(accept)
    {
        Create();
        Run();
    }

protected:
    virtual void *Entry()
    {
        wxSocketServer srv(LocalAddress(m_port), wxSOCKET_REUSEADDR);

        // FIXME: this is still not atomic, of course and the main thread could
        //        call Connect() before we have time to Accept() but there is
        //        no way to fix it with current API
        {
            wxMutexLocker lock(gs_mutex);
            gs_cond.Signal();
        }

        wxSocketBase *socket = srv.Accept();
        if ( socket )
        {
            (*m_accept)(*socket);
            delete socket;
        }

        return NULL;
    }

    int m_port;
    void (*m_accept)(wxSocketBase&);

    DECLARE_NO_COPY_CLASS(SocketServerThread)
};

// The test case for socket streams
class socketStream :
        public BaseStreamTestCase<wxSocketInputStream, wxSocketOutputStream>
{
public:
    socketStream();
    virtual ~socketStream();

    virtual void setUp();
    virtual void tearDown();

    // repeat all socket tests several times with different socket flags, so we
    // define this macro which is used several times in the test suite
    //
    // there must be some more elegant way to do this but I didn't find it...
#define ALL_SOCKET_TESTS()                                                    \
        CPPUNIT_TEST(Input_GetC);                                             \
        CPPUNIT_TEST(Input_Eof);                                              \
        CPPUNIT_TEST(Input_Read);                                             \
        CPPUNIT_TEST(Input_LastRead);                                         \
        CPPUNIT_TEST(Input_CanRead);                                          \
        CPPUNIT_TEST(Input_Peek);                                             \
        CPPUNIT_TEST(Input_Ungetch);                                          \
                                                                              \
        CPPUNIT_TEST(Output_PutC);                                            \
        CPPUNIT_TEST(Output_Write);                                           \
        CPPUNIT_TEST(Output_LastWrite)

    CPPUNIT_TEST_SUITE(socketStream);
        ALL_SOCKET_TESTS();
        // some tests don't pass with NOWAIT flag but this is probably not a
        // bug (TODO: check this)
#if 0
        CPPUNIT_TEST( PseudoTest_SetNoWait );
        ALL_SOCKET_TESTS();
#endif
        CPPUNIT_TEST( PseudoTest_SetWaitAll );
        ALL_SOCKET_TESTS();
    CPPUNIT_TEST_SUITE_END();

private:
    // Implement base class functions.
    virtual wxSocketInputStream  *DoCreateInStream();
    virtual wxSocketOutputStream *DoCreateOutStream();

    // socket thread functions
    static void WriteSocket(wxSocketBase& socket)
    {
        socket.Write("hello, world!", 13);
    }

    static void ReadSocket(wxSocketBase& socket)
    {
        char ch;
        while ( socket.Read(&ch, 1).LastCount() == 1 )
            ;
    }

    void PseudoTest_SetNoWait() { ms_flags = wxSOCKET_NOWAIT; }
    void PseudoTest_SetWaitAll() { ms_flags = wxSOCKET_WAITALL; }

    wxSocketClient *m_readSocket,
                   *m_writeSocket;
    wxThread *m_writeThread,
             *m_readThread;

    static wxSocketFlags ms_flags;
};

wxSocketFlags socketStream::ms_flags = wxSOCKET_NONE;

socketStream::socketStream()
{
    m_readSocket =
    m_writeSocket = NULL;

    m_writeThread =
    m_readThread = NULL;

    wxSocketBase::Initialize();
}

socketStream::~socketStream()
{
    wxSocketBase::Shutdown();
}

void socketStream::setUp()
{
    // create the socket threads and wait until they are ready to accept
    // connections (if we called Connect() before this happens, it would fail)
    {
        wxMutexLocker lock(gs_mutex);

        m_writeThread =
            new SocketServerThread(TEST_PORT_READ, &socketStream::WriteSocket);
        CPPUNIT_ASSERT_EQUAL( wxCOND_NO_ERROR, gs_cond.Wait() );

        m_readThread =
            new SocketServerThread(TEST_PORT_WRITE, &socketStream::ReadSocket);
        CPPUNIT_ASSERT_EQUAL( wxCOND_NO_ERROR, gs_cond.Wait() );
    }

    m_readSocket = new wxSocketClient(ms_flags);
    CPPUNIT_ASSERT( m_readSocket->Connect(LocalAddress(TEST_PORT_READ)) );

    m_writeSocket = new wxSocketClient(ms_flags);
    CPPUNIT_ASSERT( m_writeSocket->Connect(LocalAddress(TEST_PORT_WRITE)) );
}

void socketStream::tearDown()
{
    wxDELETE(m_readSocket);
    wxDELETE(m_writeSocket);

    m_writeThread->Wait();
    wxDELETE(m_writeThread);

    m_readThread->Wait();
    wxDELETE(m_readThread);
}

wxSocketInputStream *socketStream::DoCreateInStream()
{
    wxSocketInputStream *pStrInStream = new wxSocketInputStream(*m_readSocket);
    CPPUNIT_ASSERT(pStrInStream->IsOk());
    return pStrInStream;
}

wxSocketOutputStream *socketStream::DoCreateOutStream()
{
    wxSocketOutputStream *pStrOutStream = new wxSocketOutputStream(*m_writeSocket);
    CPPUNIT_ASSERT(pStrOutStream->IsOk());
    return pStrOutStream;
}

// Register the stream sub suite, by using some stream helper macro.
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(socketStream)
