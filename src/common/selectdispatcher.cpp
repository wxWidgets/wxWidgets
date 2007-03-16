///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/socketevtdispatch.cpp
// Purpose:     implements dispatcher for select() call
// Author:      Lukasz Michalski
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

#include <sys/time.h>
#include <unistd.h>

#ifdef HAVE_SYS_SELECT_H
#   include <sys/select.h>
#endif

#define wxSelectDispatcher_Trace wxT("selectdispatcher")

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSelectDispatcher
// ----------------------------------------------------------------------------

wxSelectDispatcher* wxSelectDispatcher::ms_instance = NULL;

/* static */
wxSelectDispatcher& wxSelectDispatcher::Get()
{
    if ( !ms_instance )
        ms_instance = new wxSelectDispatcher;
    return *ms_instance;
}

void
wxSelectDispatcher::RegisterFD(int fd, wxFDIOHandler* handler, int flags)
{
    if ((flags & wxSelectInput) == wxSelectInput) 
    {
        wxFD_SET(fd, &m_readset);
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Registered fd %d for input events"),fd);
    };

    if ((flags & wxSelectOutput) == wxSelectOutput)
    {
        wxFD_SET(fd, &m_writeset);
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Registered fd %d for output events"),fd);
    }

    if ((flags & wxSelectException) == wxSelectException)
    {
        wxFD_SET(fd, &m_exeptset);
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Registered fd %d for exception events"),fd);
    };

    m_handlers[fd] = handler;
    if (fd > m_maxFD)
      m_maxFD = fd;
}

wxFDIOHandler*
wxSelectDispatcher::UnregisterFD(int fd, int flags)
{
    // GSocket likes to unregister -1 descriptor
    if (fd == -1)
      return NULL;

    if ((flags & wxSelectInput) == wxSelectInput)
    {
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Unregistered fd %d from input events"),fd);
        wxFD_CLR(fd, &m_readset);
    }

    if ((flags & wxSelectOutput) == wxSelectOutput)
    {
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Unregistered fd %d from output events"),fd);
        wxFD_CLR(fd, &m_writeset);
    }

    if ((flags & wxSelectException) == wxSelectException)
    {
        wxLogTrace(wxSelectDispatcher_Trace,wxT("Unregistered fd %d from exeption events"),fd);
        wxFD_CLR(fd, &m_exeptset);
    };

    wxFDIOHandler* ret = NULL;
    wxFDIOHandlerMap::const_iterator it = m_handlers.find(fd);
    if (it != m_handlers.end())
    {
        ret = it->second;
        if (!wxFD_ISSET(fd,&m_readset) && !wxFD_ISSET(fd,&m_writeset) && !wxFD_ISSET(fd,&m_exeptset)) 
        {
            m_handlers.erase(it);
            if ( m_handlers.empty() )
                m_maxFD = 0;
        };
    };
    return ret;
}

void wxSelectDispatcher::ProcessSets(fd_set* readset, fd_set* writeset, fd_set* exeptset, int max_fd)
{
    // it is safe to remove handler from onXXX methods,
    // if you unregister descriptor first.
    wxFDIOHandlerMap::const_iterator it = m_handlers.begin();
    for ( int i = 0; i < max_fd; i++ )
    {
        wxFDIOHandler* handler = NULL;
        if (wxFD_ISSET(i, readset)) 
        {
            wxLogTrace(wxSelectDispatcher_Trace,wxT("Got read event on fd %d"),i);
            handler = FindHandler(i);
            if (handler != NULL && wxFD_ISSET(i,&m_readset))
                handler->OnReadWaiting(i);
            else
            {
              wxLogError(wxT("Lost fd in read fdset: %d, removing"),i);
              wxFD_CLR(i,&m_readset);
            };
        };

        if (wxFD_ISSET(i, writeset)) 
        {
            wxLogTrace(wxSelectDispatcher_Trace,wxT("Got write event on fd %d"),i);
            if (handler == NULL)
                handler = FindHandler(i);
            if (handler != NULL && wxFD_ISSET(i,&m_writeset))
                handler->OnWriteWaiting(i);
            else
            {
              wxLogError(wxT("Lost fd in write fdset: %d, removing"),i);
              wxFD_CLR(i,&m_writeset);
            };
        };

        if (wxFD_ISSET(i, exeptset))
        {
            wxLogTrace(wxSelectDispatcher_Trace,wxT("Got exception event on fd %d"),i);
            if (handler == NULL)
                handler = FindHandler(i);
            if (handler != NULL && wxFD_ISSET(i,&m_writeset))
                handler->OnExceptionWaiting(i);
            else
            {
              wxLogError(wxT("Lost fd in exept fdset: %d, removing"),i);
              wxFD_CLR(i,&m_exeptset);
            };
        };
    };
}

wxFDIOHandler* wxSelectDispatcher::FindHandler(int fd)
{
    wxFDIOHandlerMap::const_iterator it = m_handlers.find(fd);
    if (it != m_handlers.end())
        return it->second;
    return NULL;
};

void wxSelectDispatcher::RunLoop(int timeout)
{
    struct timeval tv, *ptv = NULL;
    if ( timeout != wxSELECT_TIMEOUT_INFINITE )
    {
        ptv = &tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout*10;
    };

    int ret;
    do
    {
        fd_set readset = m_readset;
        fd_set writeset = m_writeset;
        fd_set exeptset = m_exeptset;
        wxStopWatch sw;
        if ( ptv )
          sw.Start(ptv->tv_usec/10);
        ret = select(m_maxFD+1, &readset, &writeset, &exeptset, ptv);
        switch ( ret )
        {
            // TODO: handle unix signals here
            case -1:
                if ( ptv )
                {
                    ptv->tv_sec = 0;
                    ptv->tv_usec = timeout - sw.Time()*10;
                }
                break;

            // timeout
            case 0:
                break;

            default:
                ProcessSets(&readset, &writeset, &exeptset, m_maxFD+1);
        };
    } while (ret != 0);
}

// ----------------------------------------------------------------------------
// wxSelectDispatcherModule
// ----------------------------------------------------------------------------

class wxSelectDispatcherModule: public wxModule
{
public:
    bool OnInit() { wxLog::AddTraceMask(wxSelectDispatcher_Trace); return true; }
    void OnExit() { wxDELETE(wxSelectDispatcher::ms_instance); }

private:
    DECLARE_DYNAMIC_CLASS(wxSelectDispatcherModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxSelectDispatcherModule, wxModule)

