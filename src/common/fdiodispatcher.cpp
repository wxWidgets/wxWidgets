///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fdiodispatcher.cpp
// Purpose:     Implementation of common wxFDIODispatcher methods
// Author:      Vadim Zeitlin
// Created:     2007-05-13
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/private/fdiodispatcher.h"

// ============================================================================
// implementation
// ============================================================================

wxFDIOHandler *wxFDIODispatcher::FindHandler(int fd) const
{
    const wxFDIOHandlerMap::const_iterator it = m_handlers.find(fd);

    return it == m_handlers.end() ? NULL : it->second.handler;
}


bool wxFDIODispatcher::RegisterFD(int fd, wxFDIOHandler *handler, int flags)
{
    wxUnusedVar(flags);

    wxCHECK_MSG( handler, false, _T("handler can't be NULL") );

    // notice that it's not an error to register a handler for the same fd
    // twice as it can be done with different flags -- but it is an error to
    // register different handlers
    wxFDIOHandlerMap::iterator i = m_handlers.find(fd);
    if ( i != m_handlers.end() )
    {
        wxASSERT_MSG( i->second.handler == handler,
                        _T("registering different handler for the same fd?") );
        wxASSERT_MSG( i->second.flags != flags,
                        _T("reregistering with the same flags?") );
    }

    m_handlers[fd] = wxFDIOHandlerEntry(handler, flags);

    return true;
}

bool wxFDIODispatcher::ModifyFD(int fd, wxFDIOHandler *handler, int flags)
{
    wxUnusedVar(flags);

    wxCHECK_MSG( handler, false, _T("handler can't be NULL") );

    wxFDIOHandlerMap::iterator i = m_handlers.find(fd);
    wxCHECK_MSG( i != m_handlers.end(), false,
                    _T("modifying unregistered handler?") );

    i->second = wxFDIOHandlerEntry(handler, flags);

    return true;
}

wxFDIOHandler *wxFDIODispatcher::UnregisterFD(int fd, int flags)
{
    wxFDIOHandlerMap::iterator i = m_handlers.find(fd);
    wxCHECK_MSG( i != m_handlers.end(), NULL,
                    _T("unregistering unregistered handler?") );

    wxFDIOHandler * const handler = i->second.handler;
    i->second.flags &= ~flags;
    if ( !i->second.flags )
    {
        // this handler is not registered for anything any more, get rid of it
        m_handlers.erase(i);
    }

    return handler;
}

