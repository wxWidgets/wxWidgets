/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/gsocketiohandler.h
// Purpose:     class for registering GSocket in wxSelectDispatcher
// Authors:     Lukasz Michalski
// Created:     December 2006
// Copyright:   (c) Lukasz Michalski
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_GSOCKETIOHANDLER_H_
#define _WX_PRIVATE_GSOCKETIOHANDLER_H_

#include "wx/defs.h"

#if wxUSE_SOCKETS && wxUSE_SELECT_DISPATCHER

#include "wx/private/selectdispatcher.h"
#include "wx/private/gsocket.h"

class WXDLLIMPEXP_BASE wxGSocketIOHandler : public wxFDIOHandler
{
public:
    wxGSocketIOHandler(GSocket* socket)
    {
        m_socket = socket;
        m_flags = 0;
    }

    int GetFlags() const { return m_flags; }
    void RemoveFlag(wxFDIODispatcherEntryFlags flag) { m_flags &= ~flag; }
    void AddFlag(wxFDIODispatcherEntryFlags flag) { m_flags |= flag; }

    virtual void OnReadWaiting() { m_socket->Detected_Read(); }
    virtual void OnWriteWaiting() { m_socket->Detected_Write(); }
    virtual void OnExceptionWaiting() { m_socket->Detected_Read(); }

private:
    GSocket* m_socket;
    int m_flags;
};

#endif // wxUSE_SOCKETS && wxUSE_SELECT_DISPATCHER

#endif // _WX_PRIVATE_SOCKETEVTDISPATCH_H_
