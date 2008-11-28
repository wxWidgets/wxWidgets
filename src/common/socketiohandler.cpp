///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/socketiohandler.cpp
// Purpose:     implementation of wxFDIOHandler for wxSocket
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
#include "wx/private/socket.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSocketSelectManager
// ----------------------------------------------------------------------------

class wxSocketSelectManager : public wxSocketFDBasedManager
{
public:
    virtual void Install_Callback(wxSocketImpl *socket, wxSocketNotify event);
    virtual void Uninstall_Callback(wxSocketImpl *socket, wxSocketNotify event);
};

void wxSocketSelectManager::Install_Callback(wxSocketImpl *socket_,
                                             wxSocketNotify event)
{
    wxSocketImplUnix * const socket = static_cast<wxSocketImplUnix *>(socket_);

    const int fd = socket->m_fd;

    if ( fd == -1 )
        return;

    const SocketDir d = GetDirForEvent(socket, event);

    wxFDIODispatcher * const dispatcher = wxFDIODispatcher::Get();
    if ( !dispatcher )
        return;

    FD(socket, d) = fd;

    // register it when it's used for the first time, update it if it had been
    // previously registered
    const bool alreadyRegistered = socket->HasAnyEnabledCallbacks();

    socket->EnableCallback(d == FD_INPUT ? wxFDIO_INPUT : wxFDIO_OUTPUT);

    if ( alreadyRegistered )
        dispatcher->ModifyFD(fd, socket, socket->GetEnabledCallbacks());
    else
        dispatcher->RegisterFD(fd, socket, socket->GetEnabledCallbacks());
}

void wxSocketSelectManager::Uninstall_Callback(wxSocketImpl *socket_,
                                               wxSocketNotify event)
{
    wxSocketImplUnix * const socket = static_cast<wxSocketImplUnix *>(socket_);

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

    socket->DisableCallback(flag);

    if ( !socket->HasAnyEnabledCallbacks() )
        dispatcher->UnregisterFD(fd);
    else
        dispatcher->ModifyFD(fd, socket, socket->GetEnabledCallbacks());
}

// set the wxBase variable to point to our wxSocketManager implementation
//
// see comments in wx/apptrait.h for the explanation of why do we do it
// like this
static struct ManagerSetter
{
    ManagerSetter()
    {
        static wxSocketSelectManager s_manager;
        wxAppTraits::SetDefaultSocketManager(&s_manager);
    }
} gs_managerSetter;

#endif // wxUSE_SOCKETS
