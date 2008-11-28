///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/gsockgtk.cpp
// Purpose:     implementation of wxGTK-specific socket event handling
// Author:      Guilhem Lavaux, Vadim Zeitlin
// Created:     1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999, 2007 wxWidgets dev team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <glib.h>

#include "wx/private/socket.h"
#include "wx/apptrait.h"

extern "C" {
static
void wxSocket_GDK_Input(gpointer data,
                        gint WXUNUSED(source),
                        GdkInputCondition condition)
{
    wxFDIOHandler * const handler = static_cast<wxFDIOHandler *>(data);

    if ( condition & GDK_INPUT_READ )
        handler->OnReadWaiting();
    if ( condition & GDK_INPUT_WRITE )
        handler->OnWriteWaiting();
}
}

class GTKSocketManager : public wxSocketInputBasedManager
{
public:
    virtual int AddInput(wxFDIOHandler *handler, int fd, SocketDir d)
    {
        return gdk_input_add
               (
                    fd,
                    d == FD_OUTPUT ? GDK_INPUT_WRITE : GDK_INPUT_READ,
                    wxSocket_GDK_Input,
                    handler
               );
    }

    virtual void RemoveInput(int fd)
    {
        gdk_input_remove(fd);
    }
};

wxSocketManager *wxGUIAppTraits::GetSocketManager()
{
    static GTKSocketManager s_manager;
    return &s_manager;
}

#endif // wxUSE_SOCKETS
