/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    src/gtk/gsockgtk.cpp
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

// newer versions of glib define its own GSocket but we unfortunately use this
// name in our own (semi-)public header and so can't change it -- rename glib
// one instead
#define GSocket GlibGSocket
#include <gdk/gdk.h>
#include <glib.h>
#undef GSocket

#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"
#if wxUSE_THREADS
#include "wx/thread.h"
#endif

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

typedef struct {
#if wxUSE_THREADS
  wxMutex* m_mutex;
#endif
  gint m_id[2];
} GSocketGTKMutexProtected;

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
  socket->m_gui_dependent = (char *)malloc(sizeof(GSocketGTKMutexProtected));

  GSocketGTKMutexProtected* guispecific = (GSocketGTKMutexProtected*)socket->m_gui_dependent;

#if wxUSE_THREADS
  guispecific->m_mutex = new wxMutex(wxMUTEX_RECURSIVE);
#endif

  guispecific->m_id[0] = -1;
  guispecific->m_id[1] = -1;

  return TRUE;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  GSocketGTKMutexProtected* guispecific = (GSocketGTKMutexProtected*)socket->m_gui_dependent;

#if wxUSE_THREADS
  delete guispecific->m_mutex;
#endif

  free(guispecific);
}

void GSocketGUIFunctionsTableConcrete::Install_Callback(GSocket *socket, GSocketEvent event)
{
  GSocketGTKMutexProtected* guispecific = (GSocketGTKMutexProtected*)socket->m_gui_dependent;

  assert(guispecific != NULL);

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

#if wxUSE_THREADS
  guispecific->m_mutex->Lock();
#endif
  gint current_id = guispecific->m_id[c];
  guispecific->m_id[c] = -1;
#if wxUSE_THREADS
  guispecific->m_mutex->Unlock();
#endif

  if (current_id != -1) {
    gdk_input_remove(current_id);
  }

  current_id  = gdk_input_add(socket->m_fd,
                          (c ? GDK_INPUT_WRITE : GDK_INPUT_READ),
                          _GSocket_GDK_Input,
                          (gpointer)socket);

#if wxUSE_THREADS
  guispecific->m_mutex->Lock();
#endif
  guispecific->m_id[c] = current_id;
#if wxUSE_THREADS
  guispecific->m_mutex->Unlock();
#endif

}

void GSocketGUIFunctionsTableConcrete::Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  GSocketGTKMutexProtected* guispecific = (GSocketGTKMutexProtected*)socket->m_gui_dependent;
  int c;

  assert( guispecific != NULL );

  switch (event)
  {
    case GSOCK_LOST:       /* fall-through */
    case GSOCK_INPUT:      c = 0; break;
    case GSOCK_OUTPUT:     c = 1; break;
    case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
    default: return;
  }

#if wxUSE_THREADS
  guispecific->m_mutex->Lock();
#endif
  gint current_id = guispecific->m_id[c];
  guispecific->m_id[c] = -1;
#if wxUSE_THREADS
  guispecific->m_mutex->Unlock();
#endif

  if (current_id != -1)
  {
    gdk_input_remove(current_id);
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
