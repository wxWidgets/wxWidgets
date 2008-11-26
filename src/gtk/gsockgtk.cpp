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

#include "wx/private/gsocket.h"
#include "wx/apptrait.h"

extern "C" {
static
void _GSocket_GDK_Input(gpointer data,
                        gint WXUNUSED(source),
                        GdkInputCondition condition)
{
  GSocket *socket = (GSocket *)data;

  if (condition & GDK_INPUT_READ)
    socket->Detected_Read();
  if (condition & GDK_INPUT_WRITE)
    socket->Detected_Write();
}
}

class GTKSocketManager : public GSocketInputBasedManager
{
public:
    virtual int AddInput(GSocket *socket, SocketDir d)
    {
        return gdk_input_add
               (
                    socket->m_fd,
                    d == FD_OUTPUT ? GDK_INPUT_WRITE : GDK_INPUT_READ,
                    _GSocket_GDK_Input,
                    socket
               );
    }

    virtual void RemoveInput(int fd)
    {
        gdk_input_remove(fd);
    }
};

GSocketManager *wxGUIAppTraits::GetSocketManager()
{
    static GTKSocketManager s_manager;
    return &s_manager;
}

#endif // wxUSE_SOCKETS
