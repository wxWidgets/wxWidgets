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

#include <gdk/gdk.h>
#include <glib.h>

#include "wx/gsocket.h"

GdkInputCondition TranslateEventCondition(GSocket* socket, GSocketEvent event) {
  switch (event)
  {
    case GSOCK_LOST:       return GDK_INPUT_EXCEPTION;
    case GSOCK_INPUT:      return GDK_INPUT_READ; 
    case GSOCK_OUTPUT:   return GDK_INPUT_WRITE;
    case GSOCK_CONNECTION: return ((socket->m_server) ? GDK_INPUT_READ : GDK_INPUT_WRITE); 
    default: wxASSERT(0); return (GdkInputCondition)0; // This is invalid
  }	
}

void Uninstall_Callback(GSocket *socket, GSocketEvent event);
void Install_Callback(GSocket *socket, GSocketEvent event);

extern "C" {
static
void _GSocket_GDK_Input(gpointer data,
                        gint source,
                        GdkInputCondition condition)
{
  GSocket *socket = (GSocket *)data;

  if (condition & GDK_INPUT_READ)
  {
    Uninstall_Callback(socket,GSOCK_INPUT);
    socket->Detected_Read();
  }
  
  if (condition & GDK_INPUT_WRITE)
  {
    Uninstall_Callback(socket,GSOCK_OUTPUT);
    socket->Detected_Write();
  }
  
  if (condition & GDK_INPUT_EXCEPTION)
  {
    Uninstall_Callback(socket,GSOCK_LOST);
    socket->Detected_Lost();
  }
  
}
}

void Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  gint *m_id = (gint *)(socket->m_gui_dependent);

  wxCHECK_RET( m_id != NULL, wxT("Critical: socket has no gui callback") );
  wxCHECK_RET( event < GSOCK_MAX_EVENT, wxT("Critical: trying to install callback for an unknown socket event") );
  
  if ( socket->m_fd == -1 )
    return;

  if (m_id[event] != -1)
  {
    gdk_input_remove(m_id[event]);
    m_id[event] = -1;
  }
}

void Install_Callback(GSocket *socket, GSocketEvent event)
{
  gint *m_id = (gint *)(socket->m_gui_dependent);

  // Just in case there's some callback left for this event
  // Sanity checks are done on the Uninstall function, too.
  Uninstall_Callback(socket,event);
 
  m_id[event] = gdk_input_add(socket->m_fd,
                          TranslateEventCondition(socket, event),
                          _GSocket_GDK_Input,
                          (gpointer)socket);
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

  socket->m_gui_dependent = (char *)malloc(sizeof(gint)*GSOCK_MAX_EVENT);
  m_id = (gint *)(socket->m_gui_dependent);

  for (int i=0; i < GSOCK_MAX_EVENT; ++i)
    m_id[i] = -1;

  return TRUE;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  Install_Callback(socket, GSOCK_INPUT);
  Install_Callback(socket, GSOCK_OUTPUT);
  Install_Callback(socket, GSOCK_LOST);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  Uninstall_Callback(socket, GSOCK_INPUT);
  Uninstall_Callback(socket, GSOCK_OUTPUT);
  Uninstall_Callback(socket, GSOCK_LOST);
}

void GSocketGUIFunctionsTableConcrete::Enable_Event(GSocket* socket, GSocketEvent event)
{
  Install_Callback(socket, event);
}

void GSocketGUIFunctionsTableConcrete::Disable_Event(GSocket* socket, GSocketEvent event)
{
  Uninstall_Callback(socket, event);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
