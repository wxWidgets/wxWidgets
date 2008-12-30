/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/sockunix.h
// Purpose:     wxSocketImpl implementation for Unix systems
// Authors:     Guilhem Lavaux, Vadim Zeitlin
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Guilhem Lavaux
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_GSOCKUNX_H_
#define _WX_UNIX_GSOCKUNX_H_

#include <unistd.h>
#include <sys/ioctl.h>
#include "wx/private/fdiodispatcher.h"

class wxSocketImplUnix : public wxSocketImpl,
                         public wxFDIOHandler
{
public:
    wxSocketImplUnix(wxSocketBase& wxsocket)
        : wxSocketImpl(wxsocket)
    {
        m_fds[0] =
        m_fds[1] = -1;

        m_enabledCallbacks = 0;
    }

    virtual wxSocketError GetLastError() const;

    // wxFDIOHandler methods
    virtual void OnReadWaiting();
    virtual void OnWriteWaiting();
    virtual void OnExceptionWaiting();

    // Unix-specific functions
    bool HasAnyEnabledCallbacks() const { return m_enabledCallbacks != 0; }
    void EnableCallback(wxFDIODispatcherEntryFlags flag)
        { m_enabledCallbacks |= flag; }
    void DisableCallback(wxFDIODispatcherEntryFlags flag)
        { m_enabledCallbacks &= ~flag; }
    int GetEnabledCallbacks() const { return m_enabledCallbacks; }

private:
    virtual void DoClose()
    {
        DisableEvents();

        close(m_fd);
    }

    virtual void UnblockAndRegisterWithEventLoop()
    {
        int trueArg = 1;
        ioctl(m_fd, FIONBIO, &trueArg);

        EnableEvents();
    }

    // enable or disable notifications for socket input/output events
    void EnableEvents() { DoEnableEvents(true); }
    void DisableEvents() { DoEnableEvents(false); }

    // really enable or disable socket input/output events
    void DoEnableEvents(bool enable);

protected:
    // descriptors for input and output event notification channels associated
    // with the socket
    int m_fds[2];

    // the events which are currently enabled for this socket, combination of
    // wxFDIO_INPUT and wxFDIO_OUTPUT values
    int m_enabledCallbacks;

private:
    // notify the associated wxSocket about a change in socket state and shut
    // down the socket if the event is wxSOCKET_LOST
    void OnStateChange(wxSocketNotify event);

    // give it access to our m_fds
    friend class wxSocketFDBasedManager;
};

// A version of wxSocketManager which uses FDs for socket IO
class wxSocketFDBasedManager : public wxSocketManager
{
public:
    // no special initialization/cleanup needed when using FDs
    virtual bool OnInit() { return true; }
    virtual void OnExit() { }

protected:
    // identifies either input or output direction
    //
    // NB: the values of this enum shouldn't change
    enum SocketDir
    {
        FD_INPUT,
        FD_OUTPUT
    };

    // get the FD index corresponding to the given wxSocketNotify
    SocketDir GetDirForEvent(wxSocketImpl *socket, wxSocketNotify event)
    {
        switch ( event )
        {
            default:
                wxFAIL_MSG( "unknown socket event" );
                return FD_INPUT; // we must return something

            case wxSOCKET_LOST:
                wxFAIL_MSG( "unexpected socket event" );
                return FD_INPUT; // as above

            case wxSOCKET_INPUT:
                return FD_INPUT;

            case wxSOCKET_OUTPUT:
                return FD_OUTPUT;

            case wxSOCKET_CONNECTION:
                // for server sockets we're interested in events indicating
                // that a new connection is pending, i.e. that accept() will
                // succeed and this is indicated by socket becoming ready for
                // reading, while for the other ones we're interested in the
                // completion of non-blocking connect() which is indicated by
                // the socket becoming ready for writing
                return socket->IsServer() ? FD_INPUT : FD_OUTPUT;
        }
    }

    // access the FDs we store
    int& FD(wxSocketImplUnix *socket, SocketDir d)
    {
        return socket->m_fds[d];
    }
};

// Common base class for all ports using X11-like (and hence implemented in
// X11, Motif and GTK) AddInput() and RemoveInput() functions
class wxSocketInputBasedManager : public wxSocketFDBasedManager
{
public:
    virtual void Install_Callback(wxSocketImpl *socket_, wxSocketNotify event)
    {
        wxSocketImplUnix * const
            socket = static_cast<wxSocketImplUnix *>(socket_);

        wxCHECK_RET( socket->m_fd != -1,
                        "shouldn't be called on invalid socket" );

        const SocketDir d = GetDirForEvent(socket, event);

        int& fd = FD(socket, d);
        if ( fd != -1 )
            RemoveInput(fd);

        fd = AddInput(socket, socket->m_fd, d);
    }

    virtual void Uninstall_Callback(wxSocketImpl *socket_, wxSocketNotify event)
    {
        wxSocketImplUnix * const
            socket = static_cast<wxSocketImplUnix *>(socket_);

        const SocketDir d = GetDirForEvent(socket, event);

        int& fd = FD(socket, d);
        if ( fd != -1 )
        {
            RemoveInput(fd);
            fd = -1;
        }
    }

private:
    // these functions map directly to XtAdd/RemoveInput() or
    // gdk_input_add/remove()
    virtual int AddInput(wxFDIOHandler *handler, int fd, SocketDir d) = 0;
    virtual void RemoveInput(int fd) = 0;
};

#endif  /* _WX_UNIX_GSOCKUNX_H_ */
