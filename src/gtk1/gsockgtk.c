/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockgtk.c
 * Purpose: GSocket: GTK part
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <glib.h>

#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"


void _GSocket_GDK_Input(gpointer data,
                        gint source,
                        GdkInputCondition condition)
{
  GSocket *socket = (GSocket *)data;

  if (condition & GDK_INPUT_READ)
    _GSocket_Detected_Read(socket);
  if (condition & GDK_INPUT_WRITE)
    _GSocket_Detected_Write(socket);
}

void _GSocket_GUI_Init(GSocket *socket)
{
  gint *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(gint)*2);
  m_id = (gint *)(socket->m_gui_dependent);

  m_id[0] = -1;
  m_id[1] = -1;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
  gint *m_id = (gint *)(socket->m_gui_dependent);
  int c;

  if (socket->m_fd == -1)
    return;

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  if (m_id[c] != -1)
    gdk_input_remove(m_id[c]);

  m_id[c] = gdk_input_add(socket->m_fd,
                          (c ? GDK_INPUT_WRITE : GDK_INPUT_READ),
                          _GSocket_GDK_Input,
                          (gpointer)socket);
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  gint *m_id = (gint *)(socket->m_gui_dependent);
  int c;

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  if (m_id[c] != -1)
    gdk_input_remove(m_id[c]);

  m_id[c] = -1;
}

void _GSocket_Enable_Events(GSocket *socket)
{
  _GSocket_Install_Callback(socket, GSOCK_INPUT);
  _GSocket_Install_Callback(socket, GSOCK_OUTPUT);
}

void _GSocket_Disable_Events(GSocket *socket)
{
  _GSocket_Uninstall_Callback(socket, GSOCK_INPUT);
  _GSocket_Uninstall_Callback(socket, GSOCK_OUTPUT);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
