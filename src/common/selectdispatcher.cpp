///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/selectdispatcher.cpp
// Purpose:     implements dispatcher for select() call
// Author:      Lukasz Michalski and Vadim Zeitlin
// Created:     December 2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Lukasz Michalski
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

#if wxUSE_SELECT_DISPATCHER

#include "wx/private/selectdispatcher.h"
#include "wx/unix/private.h"

#ifndef WX_PRECOMP
    #include "wx/hash.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#if defined(HAVE_SYS_SELECT_H) || defined(__WATCOMC__)
    #include <sys/time.h>
    #include <sys/select.h>
#endif

#include <errno.h>

#define wxSelectDispatcher_Trace wxT("selectdispatcher")

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSelectSets
// ----------------------------------------------------------------------------

int wxSelectSets::ms_flags[wxSelectSets::Max] =
{
    wxFDIO_INPUT,
    wxFDIO_OUTPUT,
    wxFDIO_EXCEPTION,
};

const char *wxSelectSets::ms_names[wxSelectSets::Max] =
{
    "input",
    "output",
    "exceptional",
};

wxSelectSets::Callback wxSelectSets::ms_handlers[wxSelectSets::Max] =
{
    &wxFDIOHandler::OnReadWaiting,
    &wxFDIOHandler::OnWriteWaiting,
    &wxFDIOHandler::OnExceptionWaiting,
};

wxSelectSets::wxSelectSets()
{
    for ( int n = 0; n < Max; n++ )
    {
        wxFD_ZERO(&m_fds[n]);
    }
}

bool wxSelectSets::HasFD(int fd) const
{
    for ( int n = 0; n < Max; n++ )
    {
        if ( wxFD_ISSET(fd, (fd_set*) &m_fds[n]) )
            return true;
    }

    return false;
}

bool wxSelectSets::SetFD(int fd, int flags)
{
    wxCHECK_MSG( fd >= 0, false, _T("invalid descriptor") );

    for ( int n = 0; n < Max; n++ )
    {
        if ( flags & ms_flags[n] )
        {
            wxFD_SET(fd, &m_fds[n]);
        }
        else if ( wxFD_ISSET(fd,  (fd_set*) &m_fds[n]) )
        {
            wxFD_CLR(fd, &m_fds[n]);
        }
    }

    return true;
}

int wxSelectSets::Select(int nfds, struct timeval *tv)
{
    return select(nfds, &m_fds[Read], &m_fds[Write], &m_fds[Except], tv);
}

void wxSelectSets::Handle(int fd, wxFDIOHandler& handler) const
{
    for ( int n = 0; n < Max; n++ )
    {
        if ( wxFD_ISSET(fd, (fd_set*) &m_fds[n]) )
        {
            wxLogTrace(wxSelectDispatcher_Trace,
                       _T("Got %s event on fd %d"), ms_names[n], fd);
            (handler.*ms_handlers[n])();
            // callback can modify sets and destroy handler
            // this forces that one event can be processed at one time
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// wxSelectDispatcher
// ----------------------------------------------------------------------------

bool wxSelectDispatcher::RegisterFD(int fd, wxFDIOHandler *handler, int flags)
{
    if ( !wxMappedFDIODispatcher::RegisterFD(fd, handler, flags) )
        return false;

    if ( !m_sets.SetFD(fd, flags) )
       return false;

    if ( fd > m_maxFD )
      m_maxFD = fd;

    wxLogTrace(wxSelectDispatcher_Trace,
                _T("Registered fd %d: input:%d, output:%d, exceptional:%d"), fd, (flags & wxFDIO_INPUT) == wxFDIO_INPUT, (flags & wxFDIO_OUTPUT), (flags & wxFDIO_EXCEPTION) == wxFDIO_EXCEPTION);
    return true;
}

bool wxSelectDispatcher::ModifyFD(int fd, wxFDIOHandler *handler, int flags)
{
    if ( !wxMappedFDIODispatcher::ModifyFD(fd, handler, flags) )
        return false;

    wxASSERT_MSG( fd <= m_maxFD, _T("logic error: registered fd > m_maxFD?") );

    wxLogTrace(wxSelectDispatcher_Trace,
                _T("Modified fd %d: input:%d, output:%d, exceptional:%d"), fd, (flags & wxFDIO_INPUT) == wxFDIO_INPUT, (flags & wxFDIO_OUTPUT) == wxFDIO_OUTPUT, (flags & wxFDIO_EXCEPTION) == wxFDIO_EXCEPTION);
    return m_sets.SetFD(fd, flags);
}

bool wxSelectDispatcher::UnregisterFD(int fd)
{
    m_sets.ClearFD(fd);

    if ( !wxMappedFDIODispatcher::UnregisterFD(fd) )
        return false;

    // remove the handler if we don't need it any more
    if ( !m_sets.HasFD(fd) )
    {
        if ( fd == m_maxFD )
        {
            // need to find new max fd
            m_maxFD = -1;
            for ( wxFDIOHandlerMap::const_iterator it = m_handlers.begin();
                  it != m_handlers.end();
                  ++it )
            {
                if ( it->first > m_maxFD )
                {
                    m_maxFD = it->first;
                }
            }
        }
    }

    wxLogTrace(wxSelectDispatcher_Trace,
                _T("Removed fd %d, current max: %d"), fd, m_maxFD);
    return true;
}

bool wxSelectDispatcher::ProcessSets(const wxSelectSets& sets)
{
    bool gotEvent = false;
    for ( int fd = 0; fd <= m_maxFD; fd++ )
    {
        if ( !sets.HasFD(fd) )
            continue;

        wxFDIOHandler * const handler = FindHandler(fd);
        if ( !handler )
        {
            wxFAIL_MSG( _T("NULL handler in wxSelectDispatcher?") );
            continue;
        }

        gotEvent = true;

        sets.Handle(fd, *handler);
    }

    return gotEvent;
}

bool wxSelectDispatcher::Dispatch(int timeout)
{
    struct timeval tv,
                  *ptv;
    if ( timeout != TIMEOUT_INFINITE )
    {
        ptv = &tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout*1000;
    }
    else // no timeout
    {
        ptv = NULL;
    }

    wxSelectSets sets = m_sets;

    const int ret = sets.Select(m_maxFD + 1, ptv);
    switch ( ret )
    {
        case -1:
            if ( errno != EINTR )
            {
                wxLogSysError(_("Failed to monitor I/O channels"));
            }
            break;

        case 0:
            // timeout expired without anything happening
            break;

        default:
            if ( ProcessSets(sets) )
                return true;
    }

    // nothing happened
    return false;
}

#endif // wxUSE_SELECT_DISPATCHER
