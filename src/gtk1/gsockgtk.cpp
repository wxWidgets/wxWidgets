/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    src/gtk1/gsockgtk.cpp
 * Purpose: GSocket: GTK part
 * Licence: The wxWindows licence
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <glib.h>

#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"


extern "C" {
static
void _GSocket_GDK_Input(gpointer data,
                        gint source,
                        GdkInputCondition condition)
{
  GSocket *socket = (GSocket *)data;

  if (condition & GDK_INPUT_READ)
    socket->Detected_Read();
  if (condition & GDK_INPUT_WRITE)
    socket->Detected_Write();
}
}

bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{   return true; }

bool GSocketGUIFunctionsTableConcrete::OnInit(void)
{
    return 1;
}

void GSocketGUIFunctionsTableConcrete::OnExit(void)
{
}

bool GSocketGUIFunctionsTableConcrete::Init_Socket(GSocket *socket)
{
  gint *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(gint)*2);
  m_id = (gint *)(socket->m_gui_dependent);

  m_id[0] = -1;
  m_id[1] = -1;

  return TRUE;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void GSocketGUIFunctionsTableConcrete::Install_Callback(GSocket *socket, GSocketEvent event)
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

void GSocketGUIFunctionsTableConcrete::Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  gint *m_id = (gint *)(socket->m_gui_dependent);
  int c;

  assert( m_id != NULL );

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

  if (m_id[c] != -1)
  {
    gdk_input_remove(m_id[c]);
    m_id[c] = -1;
  }
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  Install_Callback(socket, GSOCK_INPUT);
  Install_Callback(socket, GSOCK_OUTPUT);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  Uninstall_Callback(socket, GSOCK_INPUT);
  Uninstall_Callback(socket, GSOCK_OUTPUT);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
