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

class wxSocketIOHandler;

class wxSocketImplUnix : public wxSocketImpl
{
public:
    wxSocketImplUnix(wxSocketBase& wxsocket);

    virtual void Shutdown();
    virtual wxSocketImpl *WaitConnection(wxSocketBase& wxsocket);

    int Read(char *buffer, int size);
    int Write(const char *buffer, int size);
    //attach or detach from main loop
    void Notify(bool flag);
    void Detected_Read();
    void Detected_Write();

private:
    virtual wxSocketError DoHandleConnect(int ret);
    virtual void DoClose()
    {
        wxSocketManager * const manager = wxSocketManager::Get();
        if ( manager )
        {
            manager->Uninstall_Callback(this, wxSOCKET_INPUT);
            manager->Uninstall_Callback(this, wxSOCKET_OUTPUT);
        }

        close(m_fd);
    }

    virtual void UnblockAndRegisterWithEventLoop()
    {
        int trueArg = 1;
        ioctl(m_fd, FIONBIO, &trueArg);

        EnableEvents();
    }

    // enable or disable notifications for socket input/output events but only
    // if m_use_events is true; do nothing otherwise
    virtual void EnableEvents()
    {
        if ( m_use_events )
            DoEnableEvents(true);
    }

    void DisableEvents()
    {
        if ( m_use_events )
            DoEnableEvents(false);
    }

    // really enable or disable socket input/output events, regardless of
    // m_use_events value
    void DoEnableEvents(bool enable);


    // enable or disable events for the given event if m_use_events; do nothing
    // otherwise
    //
    // notice that these functions also update m_detected: EnableEvent() clears
    // the corresponding bit in it and DisableEvent() sets it
    void EnableEvent(wxSocketNotify event);
    void DisableEvent(wxSocketNotify event);


    wxSocketError Input_Timeout();
    wxSocketError Output_Timeout();
    int Recv_Stream(char *buffer, int size);
    int Recv_Dgram(char *buffer, int size);
    int Send_Stream(const char *buffer, int size);
    int Send_Dgram(const char *buffer, int size);

protected:
    // true if socket should fire events
    bool m_use_events;

    // descriptors for input and output event notification channels associated
    // with the socket
    int m_fds[2];

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

    // allocate/free the storage we need
    virtual wxSocketImpl *CreateSocket(wxSocketBase& wxsocket)
    {
        return new wxSocketImplUnix(wxsocket);
    }

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
                wxFAIL_MSG( "unexpected socket event" );
                // fall through

            case wxSOCKET_LOST:
                // fall through

            case wxSOCKET_INPUT:
                return FD_INPUT;

            case wxSOCKET_OUTPUT:
                return FD_OUTPUT;

            case wxSOCKET_CONNECTION:
                // FIXME: explain this?
                return socket->m_server ? FD_INPUT : FD_OUTPUT;
        }
    }

    // access the FDs we store
    int& FD(wxSocketImpl *socket, SocketDir d)
    {
        return static_cast<wxSocketImplUnix *>(socket)->m_fds[d];
    }
};

// Common base class for all ports using X11-like (and hence implemented in
// X11, Motif and GTK) AddInput() and RemoveInput() functions
class wxSocketInputBasedManager : public wxSocketFDBasedManager
{
public:
    virtual void Install_Callback(wxSocketImpl *socket, wxSocketNotify event)
    {
        wxCHECK_RET( socket->m_fd != -1,
                        "shouldn't be called on invalid socket" );

        const SocketDir d = GetDirForEvent(socket, event);

        int& fd = FD(socket, d);
        if ( fd != -1 )
            RemoveInput(fd);

        fd = AddInput(socket, d);
    }

    virtual void Uninstall_Callback(wxSocketImpl *socket, wxSocketNotify event)
    {
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
    virtual int AddInput(wxSocketImpl *socket, SocketDir d) = 0;
    virtual void RemoveInput(int fd) = 0;
};

#endif  /* _WX_UNIX_GSOCKUNX_H_ */
