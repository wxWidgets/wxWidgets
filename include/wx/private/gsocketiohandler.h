/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/gsocketiohandler.h
// Purpose:     class for registering GSocket in wxSelectDispatcher
// Authors:     Lukasz Michalski
// Modified by:
// Created:     December 2006
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_GSOCKETIOHANDLER_H_
#define _WX_PRIVATE_GSOCKETIOHANDLER_H_

#include "wx/defs.h"
#include "wx/private/selectdispatcher.h"

#if wxUSE_SOCKETS

// forward declarations
class GSocket;

class WXDLLIMPEXP_CORE wxGSocketIOHandler : public wxFDIOHandler
{
public:
    wxGSocketIOHandler(GSocket* socket);
    int GetFlags() const;
    void RemoveFlag(wxSelectDispatcherEntryFlags flag);
    void AddFlag(wxSelectDispatcherEntryFlags flag);

private:
    virtual void OnReadWaiting(int fd);
    virtual void OnWriteWaiting(int fd);
    virtual void OnExceptionWaiting(int fd);

    GSocket* m_socket;
    int m_flags;
};

#endif // wxUSE_SOCKETS

#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
