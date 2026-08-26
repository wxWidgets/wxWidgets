///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/evtsource.cpp
// Purpose:     Test wxFDIODispatcher under Unix
// Created:     2026-08-26
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#ifdef __UNIX__

#include "wx/private/fdiodispatcher.h"

#include <unistd.h>

// ----------------------------------------------------------------------------
// a handler that unregisters another one from inside its own callback
// ----------------------------------------------------------------------------

namespace
{

// Both handlers below are registered for descriptors that are ready at the
// same time, so a single Dispatch() call sees both of them in one batch. The
// first one to run unregisters the other, which must then not be called: the
// dispatcher has to notice that the registration is gone rather than reuse
// whatever it recorded when the batch was collected.
class UnregisteringHandler : public wxFDIOHandler
{
public:
    explicit UnregisteringHandler(int fd)
        : m_fd(fd), m_peer(NULL), m_registered(false), m_called(false) { }

    void OnReadWaiting() wxOVERRIDE
    {
        m_called = true;

        if ( m_peer && m_peer->m_registered )
        {
            wxFDIODispatcher::Get()->UnregisterFD(m_peer->m_fd);
            m_peer->m_registered = false;
        }
    }

    void OnWriteWaiting() wxOVERRIDE { }
    void OnExceptionWaiting() wxOVERRIDE { }

    int m_fd;
    UnregisteringHandler *m_peer;
    bool m_registered;
    bool m_called;
};

// Pipe whose read end is already readable, so that registering it guarantees
// the dispatcher reports it immediately.
struct ReadablePipe
{
    ReadablePipe()
    {
        if ( pipe(m_fds) != 0 )
        {
            m_fds[0] =
            m_fds[1] = -1;
            return;
        }

        const char b = 'x';
        if ( write(m_fds[1], &b, 1) != 1 )
        {
            // Leave the descriptors valid; the test below checks readiness.
        }
    }

    ~ReadablePipe()
    {
        if ( IsOk() )
        {
            close(m_fds[0]);
            close(m_fds[1]);
        }
    }

    int ReadEnd() const { return m_fds[0]; }
    bool IsOk() const { return m_fds[0] != -1; }

    int m_fds[2];
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// the test itself
// ----------------------------------------------------------------------------

// A descriptor unregistered while the dispatcher is still working through the
// events it collected must not have its handler called afterwards.
//
// wxSelectDispatcher looks the handler up per ready descriptor and so has
// always behaved this way. wxEpollDispatcher used to store the handler pointer
// in the epoll_event itself and call through the copy taken before any handler
// ran, which meant a handler unregistered - and, in real code, destroyed -
// while servicing an earlier event of the same batch was still called, on
// memory its owner had already released.
TEST_CASE("EventSource::UnregisterDuringDispatch", "[fdiodispatcher]")
{
    wxFDIODispatcher * const dispatcher = wxFDIODispatcher::Get();
    REQUIRE( dispatcher );

    ReadablePipe pipe1, pipe2;
    REQUIRE( pipe1.IsOk() );
    REQUIRE( pipe2.IsOk() );

    UnregisteringHandler handler1(pipe1.ReadEnd());
    UnregisteringHandler handler2(pipe2.ReadEnd());
    handler1.m_peer = &handler2;
    handler2.m_peer = &handler1;

    REQUIRE( dispatcher->RegisterFD(handler1.m_fd, &handler1, wxFDIO_INPUT) );
    handler1.m_registered = true;
    REQUIRE( dispatcher->RegisterFD(handler2.m_fd, &handler2, wxFDIO_INPUT) );
    handler2.m_registered = true;

    // Both descriptors are readable, so this collects both of them and then
    // dispatches the first, which unregisters the second.
    const int numEvents = dispatcher->Dispatch(0);

    // Exactly one handler ran: the one that was unregistered before its turn
    // came must have been skipped.
    CHECK( handler1.m_called != handler2.m_called );
    CHECK( numEvents == 1 );

    if ( handler1.m_registered )
        dispatcher->UnregisterFD(handler1.m_fd);
    if ( handler2.m_registered )
        dispatcher->UnregisterFD(handler2.m_fd);
}

#endif // __UNIX__
