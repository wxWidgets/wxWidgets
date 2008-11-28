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
#include "wx/unix/private.h"
#include "wx/private/socketiohandler.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSocketImplFDIO
// ----------------------------------------------------------------------------

class wxSocketImplFDIO : public wxSocketImplUnix
{
public:
    wxSocketImplFDIO(wxSocketBase& wxsocket)
        : wxSocketImplUnix(wxsocket)
    {
        m_handler = NULL;
    }

    virtual ~wxSocketImplFDIO()
    {
        delete m_handler;
    }

    wxSocketIOHandler *m_handler;
};

// ----------------------------------------------------------------------------
// wxSocketSelectManager
// ----------------------------------------------------------------------------

class wxSocketSelectManager : public wxSocketFDBasedManager
{
public:
    virtual wxSocketImpl *CreateSocket(wxSocketBase& wxsocket)
    {
        return new wxSocketImplFDIO(wxsocket);
    }

    virtual void Install_Callback(wxSocketImpl *socket, wxSocketNotify event);
    virtual void Uninstall_Callback(wxSocketImpl *socket, wxSocketNotify event);
};

void wxSocketSelectManager::Install_Callback(wxSocketImpl *socket_,
                                             wxSocketNotify event)
{
    wxSocketImplFDIO * const socket = static_cast<wxSocketImplFDIO *>(socket_);

    const int fd = socket->m_fd;

    if ( fd == -1 )
        return;

    const SocketDir d = GetDirForEvent(socket, event);

    wxFDIODispatcher * const dispatcher = wxFDIODispatcher::Get();
    if ( !dispatcher )
        return;

    wxSocketIOHandler *& handler = socket->m_handler;

    // we should register the new handlers but modify the existing ones in place
    bool registerHandler;
    if ( handler )
    {
        registerHandler = false;
    }
    else // no existing handler
    {
        registerHandler = true;
        handler = new wxSocketIOHandler(socket);
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

void wxSocketSelectManager::Uninstall_Callback(wxSocketImpl *socket_,
                                               wxSocketNotify event)
{
    wxSocketImplFDIO * const socket = static_cast<wxSocketImplFDIO *>(socket_);

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

    wxSocketIOHandler *& handler = socket->m_handler;
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
