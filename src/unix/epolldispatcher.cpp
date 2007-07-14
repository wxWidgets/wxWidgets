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

#ifdef wxUSE_EPOLL_DISPATCHER

#include "wx/unix/private/epolldispatcher.h"
#include "wx/unix/private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include <sys/epoll.h>
#include <errno.h>

#define wxEpollDispatcher_Trace wxT("epolldispatcher")

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

/* static */
wxEpollDispatcher *wxEpollDispatcher::Create()
{
    int epollDescriptor = epoll_create(1024);
    if ( epollDescriptor == -1 )
    {
        wxLogSysError(_("Failed to create epoll descriptor"));
        return NULL;
    }

    return new wxEpollDispatcher(epollDescriptor);
}

wxEpollDispatcher::wxEpollDispatcher(int epollDescriptor)
{
    wxASSERT_MSG( epollDescriptor != -1, _T("invalid descriptor") );

    m_epollDescriptor = epollDescriptor;
}

wxEpollDispatcher::~wxEpollDispatcher()
{
    if ( close(m_epollDescriptor) != 0 )
    {
        wxLogSysError(_("Error closing epoll descriptor"));
    }
}

bool wxEpollDispatcher::RegisterFD(int fd, wxFDIOHandler* handler, int flags)
{
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

    return true;
}

void wxEpollDispatcher::Dispatch(int timeout)
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
        else if ( p->events & EPOLLOUT )
            handler->OnWriteWaiting();
        else if ( p->events & (EPOLLERR | EPOLLHUP) )
            handler->OnExceptionWaiting();
    }
}

#endif // wxUSE_EPOLL_DISPATCHER
