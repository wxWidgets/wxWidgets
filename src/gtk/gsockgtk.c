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
#include "gdk/gdk.h"
#include "glib.h"
#include "wx/gsocket.h"
#include "../unix/gsockunx.h"

void _GSocket_GDK_Input(gpointer data, gint source, GdkInputCondition condition)
{
  GSocket *socket = (GSocket *)data;

  if (condition & GDK_INPUT_READ)
    _GSocket_Detected_Read(socket);
  if (condition & GDK_INPUT_WRITE)
    _GSocket_Detected_Write(socket);
}

void _GSocket_GUI_Init(GSocket *socket)
{
  int i;
  gint *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(gint)*3);
  m_id = (gint *)(socket->m_gui_dependent);

  for (i=0;i<3;i++)
    m_id[i] = -1;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
  int i;
  gint *m_id;

  m_id = (gint *)(socket->m_gui_dependent);

  for (i=0;i<3;i++)
    if (m_id[i] == -1)
      gdk_input_remove(m_id[i]);

  free(socket->m_gui_dependent);
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
  GdkInputCondition flag;
  int c;
  gint *m_id;

  m_id = (gint *)(socket->m_gui_dependent);

  switch (event) {
  case GSOCK_CONNECTION:
  case GSOCK_LOST:
  case GSOCK_INPUT: c = 0; flag = GDK_INPUT_READ; break;
  case GSOCK_OUTPUT: c = 1;flag = GDK_INPUT_WRITE; break;
  default: return;
  }

  if (m_id[c] != -1)
    gdk_input_remove(m_id[c]);

  m_id[c] = gdk_input_add(socket->m_fd, flag, 
                          _GSocket_GDK_Input, (gpointer)socket);
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  int c;
  gint *m_id;

  m_id = (gint *)(socket->m_gui_dependent);

  switch (event) {
  case GSOCK_CONNECTION: 
  case GSOCK_LOST:
  case GSOCK_INPUT: c = 0; break;
  case GSOCK_OUTPUT: c = 1; break;
  default: return;
  }

  if (m_id[c] != -1)
    gdk_input_remove(m_id[c]);

  m_id[c] = -1;
}

unsigned long GSocket_GetEventID(GSocket *socket)
{
  return 0;
}

void GSocket_DoEvent(unsigned long evt_id)
{
}

#endif /* wxUSE_SOCKETS */
