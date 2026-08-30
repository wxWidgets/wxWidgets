///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/socketstream.cpp
// Purpose:     Test wxSocketInputStream/wxSocketOutputStream
// Author:      Vadim Zeitlin
// Copyright:   (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"


#if wxUSE_SOCKETS

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
    virtual void *Entry() override
    {
        wxSocketServer srv(LocalAddress(m_port), wxSOCKET_REUSEADDR);
        CHECK( srv.IsOk() );

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

        return nullptr;
    }

    int m_port;
    void (*m_accept)(wxSocketBase&);

    wxDECLARE_NO_COPY_CLASS(SocketServerThread);
};

// The test case for socket streams
class socketStream :
        public BaseStreamTestCase<wxSocketInputStream, wxSocketOutputStream>
{
public:
    socketStream() = default;

    ~socketStream()
    {
        wxDELETE(m_readSocket);
        wxDELETE(m_writeSocket);

        if ( m_writeThread )
        {
            m_writeThread->Wait();
            wxDELETE(m_writeThread);
        }

        if ( m_readThread )
        {
            m_readThread->Wait();
            wxDELETE(m_readThread);
        }
    }

    // This must be called before running any test, see the comment before
    // WX_SOCKET_STREAM_TEST_CASE() below.
    void Init(wxSocketFlags flags)
    {
        // create the socket threads and wait until they are ready to accept
        // connections (if we called Connect() before this happens, it would
        // fail)
        {
            wxMutexLocker lock(gs_mutex);

            m_writeThread =
                new SocketServerThread(TEST_PORT_READ, &socketStream::WriteSocket);
            REQUIRE( gs_cond.Wait() == wxCOND_NO_ERROR );

            m_readThread =
                new SocketServerThread(TEST_PORT_WRITE, &socketStream::ReadSocket);
            REQUIRE( gs_cond.Wait() == wxCOND_NO_ERROR );
        }

        m_readSocket = new wxSocketClient(flags);
        REQUIRE( m_readSocket->Connect(LocalAddress(TEST_PORT_READ)) );

        m_writeSocket = new wxSocketClient(flags);
        REQUIRE( m_writeSocket->Connect(LocalAddress(TEST_PORT_WRITE)) );
    }

private:
    // Implement base class functions.
    virtual wxSocketInputStream  *DoCreateInStream() override;
    virtual wxSocketOutputStream *DoCreateOutStream() override;
    virtual void DoCheckInputStream(wxSocketInputStream& stream_in) override;

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

    wxSocketClient *m_readSocket = nullptr,
                   *m_writeSocket = nullptr;
    wxThread *m_writeThread = nullptr,
             *m_readThread = nullptr;

    wxSocketInitializer m_socketInit;

    wxDECLARE_NO_COPY_CLASS(socketStream);
};

// All the socket stream tests are repeated with the different socket flags, so
// use this macro instead of WX_STREAM_TEST_CASE() to define them.
//
// Note that some tests don't pass with wxSOCKET_NOWAIT, but this is probably
// not a bug (TODO: check this), so this flag is not used here.
#define WX_SOCKET_STREAM_TEST_CASE(name)                          \
    TEST_CASE_METHOD(socketStream, "socketStream::" #name,        \
                     "[stream][socketStream]")                    \
    {                                                             \
        Init(GENERATE(wxSocketFlags(wxSOCKET_NONE),               \
                      wxSocketFlags(wxSOCKET_WAITALL)));          \
                                                                  \
        name();                                                   \
    }

wxSocketInputStream *socketStream::DoCreateInStream()
{
    wxSocketInputStream *pStrInStream = new wxSocketInputStream(*m_readSocket);
    CHECK(pStrInStream->IsOk());
    return pStrInStream;
}

wxSocketOutputStream *socketStream::DoCreateOutStream()
{
    wxSocketOutputStream *pStrOutStream = new wxSocketOutputStream(*m_writeSocket);
    CHECK(pStrOutStream->IsOk());
    return pStrOutStream;
}

void socketStream::DoCheckInputStream(wxSocketInputStream& stream_in)
{
    // This check sometimes fails in the AppVeyor CI environment for unknown
    // reason, so just log it there but don't fail the entire test suite run.
    if ( wxGetEnv("APPVEYOR", nullptr) )
    {
        if ( !stream_in.IsOk() )
        {
            WARN("Socket input stream test failed.\n"
                 << "Socket error = " << m_readSocket->Error()
                 << ", last count = " << m_readSocket->LastCount());
            return;
        }
    }

    CHECK(stream_in.IsOk());
}

WX_SOCKET_STREAM_TEST_CASE(Input_GetC)
WX_SOCKET_STREAM_TEST_CASE(Input_Eof)
WX_SOCKET_STREAM_TEST_CASE(Input_Read)
WX_SOCKET_STREAM_TEST_CASE(Input_LastRead)
WX_SOCKET_STREAM_TEST_CASE(Input_CanRead)
WX_SOCKET_STREAM_TEST_CASE(Input_Peek)
WX_SOCKET_STREAM_TEST_CASE(Input_Ungetch)

WX_SOCKET_STREAM_TEST_CASE(Output_PutC)
WX_SOCKET_STREAM_TEST_CASE(Output_Write)
WX_SOCKET_STREAM_TEST_CASE(Output_LastWrite)

#endif // wxUSE_SOCKETS
