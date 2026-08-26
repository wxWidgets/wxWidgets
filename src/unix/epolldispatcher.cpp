///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/epolldispatcher.cpp
// Purpose:     implements dispatcher for epoll_wait() call
// Author:      Lukasz Michalski
// Created:     April 2007
// Copyright:   (c) 2007 Lukasz Michalski
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_EPOLL_DISPATCHER

#include "wx/unix/private/epolldispatcher.h"
#include "wx/unix/private.h"
#include "wx/stopwatch.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#define wxEpollDispatcher_Trace wxT("epolldispatcher")

// ============================================================================
// implementation
// ============================================================================

// helper: return EPOLLxxx mask corresponding to the given flags (and also log
// debugging messages about it)
static uint32_t GetEpollMask(int flags, int fd)
{
    wxUnusedVar(fd); // unused if wxLogTrace() disabled

    uint32_t ep = 0;

    if ( flags & wxFDIO_INPUT )
    {
        ep |= EPOLLIN;
        wxLogTrace(wxEpollDispatcher_Trace,
                   wxT("Registered fd %d for input events"), fd);
    }

    if ( flags & wxFDIO_OUTPUT )
    {
        ep |= EPOLLOUT;
        wxLogTrace(wxEpollDispatcher_Trace,
                   wxT("Registered fd %d for output events"), fd);
    }

    if ( flags & wxFDIO_EXCEPTION )
    {
        ep |= EPOLLERR | EPOLLHUP;
        wxLogTrace(wxEpollDispatcher_Trace,
                   wxT("Registered fd %d for exceptional events"), fd);
    }

    return ep;
}

// ----------------------------------------------------------------------------
// wxEpollDispatcher
// ----------------------------------------------------------------------------

/* static */
wxEpollDispatcher *wxEpollDispatcher::Create()
{
    int epollDescriptor = epoll_create(1024);
    if ( epollDescriptor == -1 )
    {
        wxLogSysError(_("Failed to create epoll descriptor"));
        return NULL;
    }
    wxLogTrace(wxEpollDispatcher_Trace,
                   wxT("Epoll fd %d created"), epollDescriptor);
    return new wxEpollDispatcher(epollDescriptor);
}

wxEpollDispatcher::wxEpollDispatcher(int epollDescriptor)
{
    wxASSERT_MSG( epollDescriptor != -1, wxT("invalid descriptor") );

    m_epollDescriptor = epollDescriptor;
}

wxEpollDispatcher::~wxEpollDispatcher()
{
    for ( size_t n = 0; n < m_entries.size(); ++n )
        delete m_entries[n];

    if ( close(m_epollDescriptor) != 0 )
    {
        wxLogSysError(_("Error closing epoll descriptor"));
    }
}

wxEpollDispatcher::Entry *wxEpollDispatcher::GetEntry(int fd)
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(m_entriesCS);
#endif

    if ( static_cast<size_t>(fd) >= m_entries.size() )
    {
        m_entries.resize(fd + 1, NULL);
    }

    Entry *&entry = m_entries[fd];
    if ( !entry )
        entry = new Entry();

    return entry;
}

void wxEpollDispatcher::ForgetEntry(int fd)
{
#if wxUSE_THREADS
    wxCriticalSectionLocker lock(m_entriesCS);
#endif

    // This shouldn't happen because we always extend m_entries to the maximum
    // FD seen so far.
    wxCHECK_RET
    (
        static_cast<size_t>(fd) < m_entries.size(),
        wxString::Format("Unregistering FD %d but max seen FD is %d",
                         fd, static_cast<int>(m_entries.size()) - 1)
    );

    Entry * const entry = m_entries[fd];
    wxCHECK_RET
    (
        entry,
        wxString::Format("Unregistering not registered FD %d", fd)
    );

    entry->handler = NULL;
}

bool wxEpollDispatcher::RegisterFD(int fd, wxFDIOHandler* handler, int flags)
{
    epoll_event ev;
    ev.events = GetEpollMask(flags, fd);

    // The entry and not the handler: see Entry.
    Entry * const entry = GetEntry(fd);
    ev.data.ptr = entry;

    const int ret = epoll_ctl(m_epollDescriptor, EPOLL_CTL_ADD, fd, &ev);
    if ( ret != 0 )
    {
        wxLogSysError(_("Failed to add descriptor %d to epoll descriptor %d"),
                      fd, m_epollDescriptor);

        return false;
    }

    entry->handler = handler;
    wxLogTrace(wxEpollDispatcher_Trace,
               wxT("Added fd %d (handler %p) to epoll %d"), fd, handler, m_epollDescriptor);

    return true;
}

bool wxEpollDispatcher::ModifyFD(int fd, wxFDIOHandler* handler, int flags)
{
    epoll_event ev;
    ev.events = GetEpollMask(flags, fd);

    Entry * const entry = GetEntry(fd);
    ev.data.ptr = entry;

    const int ret = epoll_ctl(m_epollDescriptor, EPOLL_CTL_MOD, fd, &ev);
    if ( ret != 0 )
    {
        wxLogSysError(_("Failed to modify descriptor %d in epoll descriptor %d"),
                      fd, m_epollDescriptor);

        return false;
    }

    entry->handler = handler;

    wxLogTrace(wxEpollDispatcher_Trace,
                wxT("Modified fd %d (handler: %p) on epoll %d"), fd, handler, m_epollDescriptor);
    return true;
}

bool wxEpollDispatcher::UnregisterFD(int fd)
{
    epoll_event ev;
    ev.events = 0;
    ev.data.ptr = NULL;

    if ( epoll_ctl(m_epollDescriptor, EPOLL_CTL_DEL, fd, &ev) != 0 )
    {
        wxLogSysError(_("Failed to unregister descriptor %d from epoll descriptor %d"),
                      fd, m_epollDescriptor);
    }

    // Drop the handler even if epoll_ctl() above failed: the caller is done
    // with it either way, and a stale handler here is exactly what Dispatch()
    // must not find. The entry itself stays valid, see comment for Entry.
    ForgetEntry(fd);

    wxLogTrace(wxEpollDispatcher_Trace,
                wxT("removed fd %d from %d"), fd, m_epollDescriptor);
    return true;
}

int
wxEpollDispatcher::DoPoll(epoll_event *events, int numEvents, int timeout) const
{
    // the code below relies on TIMEOUT_INFINITE being -1 so that we can pass
    // timeout value directly to epoll_wait() which interprets -1 as meaning to
    // wait forever and would need to be changed if the value of
    // TIMEOUT_INFINITE ever changes
    wxCOMPILE_TIME_ASSERT( TIMEOUT_INFINITE == -1, UpdateThisCode );

    wxMilliClock_t timeEnd;
    if ( timeout > 0 )
        timeEnd = wxGetLocalTimeMillis();

    int rc;
    for ( ;; )
    {
        rc = epoll_wait(m_epollDescriptor, events, numEvents, timeout);
        if ( rc != -1 || errno != EINTR )
            break;

        // we got interrupted, update the timeout and restart
        if ( timeout > 0 )
        {
            timeout = wxMilliClockToLong(timeEnd - wxGetLocalTimeMillis());
            if ( timeout < 0 )
                return 0;
        }
    }

    return rc;
}

bool wxEpollDispatcher::HasPending() const
{
    epoll_event event;

    // NB: it's not really clear if epoll_wait() can return a number greater
    //     than the number of events passed to it but just in case it can, use
    //     >= instead of == here, see #10397
    return DoPoll(&event, 1, 0) >= 1;
}

int wxEpollDispatcher::Dispatch(int timeout)
{
    epoll_event events[16];

    const int rc = DoPoll(events, WXSIZEOF(events), timeout);

    if ( rc == -1 )
    {
        wxLogSysError(_("Waiting for IO on epoll descriptor %d failed"),
                      m_epollDescriptor);
        return -1;
    }

    int numEvents = 0;
    for ( epoll_event *p = events; p < events + rc; p++ )
    {
        // Go through the entry instead of using a handler pointer recorded
        // when the descriptor was registered: dispatching an earlier event of
        // this batch may have unregistered, and in the code that owns it
        // destroyed, the handler for a later one, and epoll_wait() filled this
        // array in before any of that happened. Using the recorded pointer
        // would then call a virtual function on a destroyed object.
        //
        // A cleared handler is therefore expected here rather than an error,
        // and is simply skipped.
        const Entry * const entry = static_cast<const Entry *>(p->data.ptr);
        wxFDIOHandler * const handler = entry ? entry->handler : NULL;
        if ( !handler )
            continue;

        // note that for compatibility with wxSelectDispatcher we call
        // OnReadWaiting() on EPOLLHUP as this is what epoll_wait() returns
        // when the write end of a pipe is closed while with select() the
        // remaining pipe end becomes ready for reading when this happens
        if ( p->events & (EPOLLIN | EPOLLHUP) )
            handler->OnReadWaiting();
        else if ( p->events & EPOLLOUT )
            handler->OnWriteWaiting();
        else if ( p->events & EPOLLERR )
            handler->OnExceptionWaiting();
        else
            continue;

        numEvents++;
    }

    return numEvents;
}

#endif // wxUSE_EPOLL_DISPATCHER
