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

#include <vector>

struct epoll_event;

class WXDLLIMPEXP_BASE wxEpollDispatcher : public wxFDIODispatcher
{
public:
    // create a new instance of this class, can return NULL if
    // epoll() is not supported on this system
    //
    // the caller should delete the returned pointer
    static wxEpollDispatcher *Create();

    virtual ~wxEpollDispatcher();

    // implement base class pure virtual methods
    virtual bool RegisterFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL) wxOVERRIDE;
    virtual bool ModifyFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL) wxOVERRIDE;
    virtual bool UnregisterFD(int fd) wxOVERRIDE;
    virtual bool HasPending() const wxOVERRIDE;
    virtual int Dispatch(int timeout = TIMEOUT_INFINITE) wxOVERRIDE;

private:
    // ctor is private, use Create()
    wxEpollDispatcher(int epollDescriptor);

    // common part of HasPending() and Dispatch(): calls epoll_wait() with the
    // given timeout
    int DoPoll(epoll_event *events, int numEvents, int timeout) const;

    // What epoll_event::data.ptr points at, instead of the handler itself.
    //
    // epoll_wait() copies data.ptr into the batch it returns before any
    // handler runs, and dispatching one event of that batch can unregister,
    // and in the code owning it destroy, the handler for a later one.
    // Unregistering clears the handler here, which is how Dispatch() knows a
    // registration is gone.
    struct Entry
    {
        Entry() : handler(NULL) { }

        wxFDIOHandler *handler;
    };

    // Return the entry for this descriptor, creating one if necessary.
    Entry *GetEntry(int fd);

    // Forget the handler corresponding to the given given descriptor, if any.
    void ForgetEntry(int fd);


    int m_epollDescriptor;

    // Indexed by descriptor, and only ever grown, never shrunk: an entry
    // cannot be freed when its descriptor is unregistered because a batch
    // being dispatched may still point at it, so they live as long as the
    // dispatcher does.
    //
    // The lock guards this vector alone. Descriptors are registered and
    // unregistered from worker threads -- wxSocketImpl does it from the thread
    // performing the socket operation -- while Dispatch() runs on the thread
    // with the event loop. Dispatch() never looks at the vector, reaching an
    // entry through the pointer epoll_wait() gave back, so it takes no lock,
    // and no lock is held across a call into a handler.
    //
    // Entry::handler itself is an ordinary pointer, written by whichever
    // thread registers the descriptor and read by the dispatching one. That is
    // the same cross-thread case as a handler destroyed by another thread
    // while the loop is between finding it and calling it, which is racy here
    // as it was before.
    std::vector<Entry *> m_entries;
#if wxUSE_THREADS
    wxCriticalSection m_entriesCS;
#endif // wxUSE_THREADS
};

#endif // wxUSE_EPOLL_DISPATCHER

#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
