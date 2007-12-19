///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gsocketiohandler.cpp
// Purpose:     implementation of wxFDIOHandler for GSocket
// Author:      Angel Vidal, Lukasz Michalski
// Created:     08.24.06
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Angel vidal
//              (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
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

#if wxUSE_SOCKETS && wxUSE_SELECT_DISPATCHER

#include "wx/apptrait.h"
#include "wx/unix/private.h"
#include "wx/private/gsocketiohandler.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GSocketSelectManager
// ----------------------------------------------------------------------------

class GSocketSelectManager : public GSocketFDBasedManager
{
public:
    virtual void Install_Callback(GSocket *socket, GSocketEvent event);
    virtual void Uninstall_Callback(GSocket *socket, GSocketEvent event);
};

void GSocketSelectManager::Install_Callback(GSocket *socket,
                                            GSocketEvent event)
{
    const int fd = socket->m_fd;

    if ( fd == -1 )
        return;

    const SocketDir d = GetDirForEvent(socket, event);

    wxFDIODispatcher * const dispatcher = wxFDIODispatcher::Get();
    if ( !dispatcher )
        return;

    wxGSocketIOHandler *& handler = socket->m_handler;

    // we should register the new handlers but modify the existing ones in place
    bool registerHandler;
    if ( handler )
    {
        registerHandler = false;
    }
    else // no existing handler
    {
        registerHandler = true;
        handler = new wxGSocketIOHandler(socket);
    }

    FD(socket, d) = fd;
    if (d == FD_INPUT)
    {
        handler->AddFlag(wxFDIO_INPUT);
    }
    else
    {
        handler->AddFlag(wxFDIO_OUTPUT);
    }

    if ( registerHandler )
        dispatcher->RegisterFD(fd, handler, handler->GetFlags());
    else
        dispatcher->ModifyFD(fd, handler, handler->GetFlags());
}

void GSocketSelectManager::Uninstall_Callback(GSocket *socket,
                                              GSocketEvent event)
{
    const SocketDir d = GetDirForEvent(socket, event);

    const int fd = FD(socket, d);
    if ( fd == -1 )
        return;

    FD(socket, d) = -1;

    const wxFDIODispatcherEntryFlags
        flag = d == FD_INPUT ? wxFDIO_INPUT : wxFDIO_OUTPUT;

    wxFDIODispatcher * const dispatcher = wxFDIODispatcher::Get();
    if ( !dispatcher )
        return;

    wxGSocketIOHandler *& handler = socket->m_handler;
    if ( handler )
    {
        handler->RemoveFlag(flag);

        if ( !handler->GetFlags() )
        {
            dispatcher->UnregisterFD(fd);
            delete handler;
            socket->m_handler = NULL;
        }
        else
        {
            dispatcher->ModifyFD(fd, handler, handler->GetFlags());
        }
    }
    else
    {
        dispatcher->UnregisterFD(fd);
    }
}

GSocketManager *wxAppTraits::GetSocketManager()
{
    static GSocketSelectManager s_manager;

    return &s_manager;
}

#endif // wxUSE_SOCKETS
