/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/epolldispatcher.h
// Purpose:     wxEpollDispatcher class
// Authors:     Lukasz Michalski
// Created:     April 2007
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EPOLLDISPATCHER_H_
#define _WX_PRIVATE_EPOLLDISPATCHER_H_

#include "wx/defs.h"

#ifdef HAVE_SYS_EPOLL_H

#include "wx/private/fdiodispatcher.h"

class WXDLLIMPEXP_CORE wxEpollDispatcher : public wxFDIODispatcher
{
public:
    // get pointer to the unique instance of this class, can return NULL if
    // epoll() is not supported on this system
    //
    // do not delete the returned pointer
    static wxEpollDispatcher *Get();

    // implement base class pure virtual methods
    virtual bool RegisterFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL);
    virtual bool ModifyFD(int fd, wxFDIOHandler* handler, int flags = wxFDIO_ALL);
    virtual wxFDIOHandler *UnregisterFD(int fd, int flags = wxFDIO_ALL);
    virtual void RunLoop(int timeout = TIMEOUT_INFINITE);

private:
    // ctor is private, use Get()
    wxEpollDispatcher();

    // return true if the object was successfully initialized
    bool IsOk() const { return m_epollDescriptor != -1; }

    int m_epollDescriptor;
};

#endif // HAVE_SYS_EPOLL_H

#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
