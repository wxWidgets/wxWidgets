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

#include "wx/private/selectdispatcher.h"
#include "wx/module.h"
#include "wx/timer.h"
#include "wx/unix/private.h"
#include "wx/log.h"

#ifndef WX_PRECOMP
    #include "wx/hash.h"
#endif

#ifdef HAVE_SYS_SELECT_H
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
        if ( wxFD_ISSET(fd, &m_fds[n]) )
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
            wxLogTrace(wxSelectDispatcher_Trace,
                       _T("Registered fd %d for %s events"), fd, ms_names[n]);
        }
        else if ( wxFD_ISSET(fd, &m_fds[n]) )
        {
            wxFD_CLR(fd, &m_fds[n]);
            wxLogTrace(wxSelectDispatcher_Trace,
                       _T("Unregistered fd %d from %s events"), fd, ms_names[n]);
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
        if ( wxFD_ISSET(fd, &m_fds[n]) )
        {
            wxLogTrace(wxSelectDispatcher_Trace,
                       _T("Got %s event on fd %d"), ms_names[n], fd);
            (handler.*ms_handlers[n])();
        }
    }
}

// ----------------------------------------------------------------------------
// wxSelectDispatcher
// ----------------------------------------------------------------------------

static wxSelectDispatcher *gs_selectDispatcher = NULL;

/* static */
wxSelectDispatcher *wxSelectDispatcher::Get()
{
    if ( !gs_selectDispatcher )
    {
        // the dispatcher should be only created from one thread so it should
        // be ok to use a global without any protection here
        gs_selectDispatcher = new wxSelectDispatcher;
    }

    return gs_selectDispatcher;
}

/* static */
void wxSelectDispatcher::DispatchPending()
{
    if ( gs_selectDispatcher )
        gs_selectDispatcher->RunLoop(0);
}

wxSelectDispatcher::wxSelectDispatcher()
{
    m_maxFD = -1;
}

bool wxSelectDispatcher::RegisterFD(int fd, wxFDIOHandler *handler, int flags)
{
    if ( !wxFDIODispatcher::RegisterFD(fd, handler, flags) )
        return false;

    if ( !m_sets.SetFD(fd, flags) )
       return false;

    if ( fd > m_maxFD )
      m_maxFD = fd;

    return true;
}

bool wxSelectDispatcher::ModifyFD(int fd, wxFDIOHandler *handler, int flags)
{
    if ( !wxFDIODispatcher::ModifyFD(fd, handler, flags) )
        return false;

    wxASSERT_MSG( fd <= m_maxFD, _T("logic error: registered fd > m_maxFD?") );

    return m_sets.SetFD(fd, flags);
}

wxFDIOHandler *wxSelectDispatcher::UnregisterFD(int fd, int flags)
{
    wxFDIOHandler * const handler = wxFDIODispatcher::UnregisterFD(fd, flags);

    m_sets.ClearFD(fd, flags);

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
                    m_maxFD = it->first;
            }
        }
    }

    return handler;
}

void wxSelectDispatcher::ProcessSets(const wxSelectSets& sets)
{
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

        sets.Handle(fd, *handler);
    }
}

void wxSelectDispatcher::RunLoop(int timeout)
{
    struct timeval tv,
                  *ptv = NULL;
    if ( timeout != TIMEOUT_INFINITE )
    {
        ptv = &tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout*1000;
    }

    for ( ;; )
    {
        wxSelectSets sets = m_sets;

        wxStopWatch sw;
        if ( ptv && timeout )
          sw.Start(ptv->tv_usec/10);

        const int ret = sets.Select(m_maxFD + 1, ptv);
        switch ( ret )
        {
            case -1:
                // continue if we were interrupted by a signal, else bail out
                if ( errno != EINTR )
                {
                    wxLogSysError(_("Failed to monitor IO channels"));
                    return;
                }
                break;

            case 0:
                // timeout expired without anything happening
                return;

            default:
                ProcessSets(sets);
        }

        if ( ptv )
        {
            timeout -= sw.Time();
            if ( timeout <= 0 )
                break;

            ptv->tv_usec = timeout*1000;
        }
    }
}

// ----------------------------------------------------------------------------
// wxSelectDispatcherModule
// ----------------------------------------------------------------------------

class wxSelectDispatcherModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDELETE(gs_selectDispatcher); }

private:
    DECLARE_DYNAMIC_CLASS(wxSelectDispatcherModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxSelectDispatcherModule, wxModule)

