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
    socket->Detected_Read();
  
  if (condition & GDK_INPUT_WRITE)
    socket->Detected_Write();
  
  if (condition & GDK_INPUT_EXCEPTION)
    socket->Detected_Lost();
  
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
  socket->m_platform_specific_id = -1;
  socket->m_eventflags = 0;
  return TRUE;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  if (socket->m_platform_specific_id != -1)
    gdk_input_remove((gint)socket->m_platform_specific_id);
}

// Helper function for {En|Dis}able*
void SetNewCallback(GSocket* socket)
{
  if (socket->m_platform_specific_id != -1)
    gdk_input_remove((gint)socket->m_platform_specific_id);  
  
  if (socket->m_eventflags) // Readd the other events if there's any left
    socket->m_platform_specific_id = (long int)gdk_input_add(socket->m_fd,
                          (GdkInputCondition)socket->m_eventflags,
                          _GSocket_GDK_Input,
                          (gpointer)socket);
  else // Set the callback id to -1
    socket->m_platform_specific_id = -1;
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  // Dont' use the Enable_Event function, as it removes/readds
  socket->m_eventflags |= TranslateEventCondition(socket, GSOCK_INPUT);
  socket->m_eventflags |= TranslateEventCondition(socket, GSOCK_OUTPUT);
  socket->m_eventflags |= TranslateEventCondition(socket, GSOCK_LOST);
  
  SetNewCallback(socket);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  socket->m_eventflags = 0;
  
  SetNewCallback(socket);
}

void GSocketGUIFunctionsTableConcrete::Enable_Event(GSocket* socket, GSocketEvent event)
{
  wxCHECK_RET( event < GSOCK_MAX_EVENT, wxT("Critical: trying to install callback for an unknown socket event") );

  if ( socket->m_fd == -1 )
    return;

  socket->m_eventflags |= TranslateEventCondition(socket, event);

  SetNewCallback(socket);  
}

void GSocketGUIFunctionsTableConcrete::Disable_Event(GSocket* socket, GSocketEvent event)
{
  wxCHECK_RET( event < GSOCK_MAX_EVENT, wxT("Critical: trying to uninstall callback for an unknown socket event") );
 
  if ( socket->m_fd == -1 )
    return;

  socket->m_eventflags &= ~TranslateEventCondition(socket, event);
  
  SetNewCallback(socket);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
