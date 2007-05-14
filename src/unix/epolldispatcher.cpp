///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/epolldispatcher.cpp
// Purpose:     implements dispatcher for epoll_wait() call
// Author:      Lukasz Michalski
// Created:     April 2007
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Lukasz Michalski
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef HAVE_SYS_EPOLL_H

#include "wx/unix/private/epolldispatcher.h"
#include "wx/unix/private.h"
#include "wx/log.h"
#include "wx/module.h"

#include <sys/epoll.h>
#include <errno.h>

#define wxEpollDispatcher_Trace wxT("epolldispatcher")

static wxEpollDispatcher *gs_epollDispatcher = NULL;

// ============================================================================
// implementation
// ============================================================================

// helper: return EPOLLxxx mask corresponding to the given flags (and also log
// debugging messages about it)
static uint32_t GetEpollMask(int flags, int fd)
{
    uint32_t ep = 0;

    if ( flags & wxFDIO_INPUT )
    {
        ep |= EPOLLIN;
        wxLogTrace(wxEpollDispatcher_Trace,
                   _T("Registered fd %d for input events"), fd);
    }

    if ( flags & wxFDIO_OUTPUT )
    {
        ep |= EPOLLOUT;
        wxLogTrace(wxEpollDispatcher_Trace,
                   _T("Registered fd %d for output events"), fd);
    }

    if ( flags & wxFDIO_EXCEPTION )
    {
        ep |= EPOLLERR | EPOLLHUP;
        wxLogTrace(wxEpollDispatcher_Trace,
                   _T("Registered fd %d for exceptional events"), fd);
    }

    return ep;
}

// ----------------------------------------------------------------------------
// wxEpollDispatcher
// ----------------------------------------------------------------------------

wxEpollDispatcher::wxEpollDispatcher()
{
    m_epollDescriptor = epoll_create(1024);
    if ( m_epollDescriptor == -1 )
    {
        wxLogSysError(_("Failed to create epoll descriptor"));
    }
}

bool wxEpollDispatcher::RegisterFD(int fd, wxFDIOHandler* handler, int flags)
{
    if ( !wxFDIODispatcher::RegisterFD(fd, handler, flags) )
        return false;

    epoll_event ev;
    ev.events = GetEpollMask(flags, fd);
    ev.data.ptr = handler;

    const int ret = epoll_ctl(m_epollDescriptor, EPOLL_CTL_ADD, fd, &ev);
    if ( ret != 0 )
    {
        wxLogSysError(_("Failed to add descriptor %d to epoll descriptor %d"),
                      fd, m_epollDescriptor);

        return false;
    }

    return true;
}

bool wxEpollDispatcher::ModifyFD(int fd, wxFDIOHandler* handler, int flags)
{
    if ( !wxFDIODispatcher::ModifyFD(fd, handler, flags) )
        return false;

    epoll_event ev;
    ev.events = GetEpollMask(flags, fd);
    ev.data.ptr = handler;

    const int ret = epoll_ctl(m_epollDescriptor, EPOLL_CTL_MOD, fd, &ev);
    if ( ret != 0 )
    {
        wxLogSysError(_("Failed to modify descriptor %d in epoll descriptor %d"),
                      fd, m_epollDescriptor);

        return false;
    }

    return true;
}

wxFDIOHandler *wxEpollDispatcher::UnregisterFD(int fd, int flags)
{
    wxFDIOHandler * const handler = wxFDIODispatcher::UnregisterFD(fd, flags);
    if ( !handler )
        return NULL;

    epoll_event ev;
    ev.events = 0;
    ev.data.ptr = NULL;

    if ( epoll_ctl(m_epollDescriptor, EPOLL_CTL_DEL, fd, &ev) != 0 )
    {
        wxLogSysError(_("Failed to unregister descriptor %d from epoll descriptor %d"),
                      fd, m_epollDescriptor);
    }

    return handler;
}

void wxEpollDispatcher::RunLoop(int timeout)
{
    epoll_event events[16];

    const int e_num = epoll_wait
                      (
                        m_epollDescriptor,
                        events,
                        WXSIZEOF(events),
                        timeout == TIMEOUT_INFINITE ? -1 : timeout
                      );

    if ( e_num == -1 )
    {
        if ( errno != EINTR )
        {
            wxLogSysError(_("Waiting for IO on epoll descriptor %d failed"),
                          m_epollDescriptor);
            return;
        }
    }

    for ( epoll_event *p = events; p < events + e_num; p++ )
    {
        wxFDIOHandler * const handler = (wxFDIOHandler *)(p->data.ptr);
        if ( !handler )
        {
            wxFAIL_MSG( _T("NULL handler in epoll_event?") );
            continue;
        }

        if ( p->events & EPOLLIN )
            handler->OnReadWaiting();

        if ( p->events & EPOLLOUT )
            handler->OnWriteWaiting();

        if ( p->events & (EPOLLERR | EPOLLHUP) )
            handler->OnExceptionWaiting();
    }
}

/* static */
wxEpollDispatcher *wxEpollDispatcher::Get()
{
    if ( !gs_epollDispatcher )
    {
        gs_epollDispatcher = new wxEpollDispatcher;
        if ( !gs_epollDispatcher->IsOk() )
        {
            delete gs_epollDispatcher;
            gs_epollDispatcher = NULL;
        }
    }

    return gs_epollDispatcher;
}

// ----------------------------------------------------------------------------
// wxEpollDispatcherModule
// ----------------------------------------------------------------------------

class wxEpollDispatcherModule : public wxModule
{
public:
    wxEpollDispatcherModule() { }

    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDELETE(gs_epollDispatcher); }

    DECLARE_DYNAMIC_CLASS(wxEpollDispatcherModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxEpollDispatcherModule, wxModule)

#endif // HAVE_SYS_EPOLL_H
