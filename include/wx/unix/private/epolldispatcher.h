/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/epolldispatcher.h
// Purpose:     wxEpollDispatcher class
// Authors:     Lukasz Michalski
// Created:     April 2007
// Copyright:   (c) Lukasz Michalski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EPOLLDISPATCHER_H_
#define _WX_PRIVATE_EPOLLDISPATCHER_H_

#include "wx/defs.h"

#ifdef wxUSE_EPOLL_DISPATCHER

#include "wx/private/fdiodispatcher.h"

#if wxUSE_THREADS
    #include "wx/thread.h"
#endif

struct epoll_event;

class WXDLLIMPEXP_BASE wxEpollDispatcher : public wxFDIODispatcher
{
public:
    // create a new instance of this class, can return nullptr if
    // epoll() is not supported on this system
    //
    // the caller should delete the returned pointer
    static wxEpollDispatcher *Create();

    virtual ~wxEpollDispatcher();

    // implement base class pure virtual methods
    virtual bool RegisterFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL) override;
    virtual bool ModifyFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL) override;
    virtual bool UnregisterFD(int fd) override;
    virtual bool HasPending() const override;
    virtual int Dispatch(int timeout = TIMEOUT_INFINITE) override;

private:
    // ctor is private, use Create()
    wxEpollDispatcher(int epollDescriptor);

    // common part of HasPending() and Dispatch(): calls epoll_wait() with the
    // given timeout
    int DoPoll(epoll_event *events, int numEvents, int timeout) const;

    // Look up the handler currently registered for the given descriptor, or
    // nullptr if there is none (any more).
    wxFDIOHandler *FindHandler(int fd) const;

    // Record, replace or forget the handler for a descriptor. Kept separate
    // from the epoll_ctl() calls so that the map is only updated once the
    // kernel has accepted the change.
    void StoreHandler(int fd, wxFDIOHandler *handler);
    void ForgetHandler(int fd);


    int m_epollDescriptor;

    // Maps the descriptors we have registered to their handlers. Dispatch()
    // needs this because epoll_event::data is a union: it holds the descriptor
    // so that a handler unregistered while the batch is being processed can be
    // detected, which means the handler pointer has to be found elsewhere.
    //
    // Guarded because descriptors are registered and unregistered from worker
    // threads -- wxSocketImpl does it from the thread performing the socket
    // operation -- while Dispatch() reads the map from the thread running the
    // event loop. epoll_ctl() itself is thread-safe, so nothing but this map
    // needs the protection, and the lock is never held across a handler call.
    wxFDIOHandlerMap m_handlers;
#if wxUSE_THREADS
    mutable wxCriticalSection m_handlersCS;
#endif // wxUSE_THREADS
};

#endif // wxUSE_EPOLL_DISPATCHER

#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
