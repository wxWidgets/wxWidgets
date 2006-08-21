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

#define DATATYPE gint
#define PLATFORM_DATA(x) (*(DATATYPE*)x)

int GSocketGUIFunctionsTableConcrete::TranslateEventCondition(GSocket* socket, GSocketEvent event) {
  switch (event)
  {
    case GSOCK_LOST:       return (int)GDK_INPUT_EXCEPTION;
    case GSOCK_INPUT:      return (int)GDK_INPUT_READ; 
    case GSOCK_OUTPUT:   return (int)GDK_INPUT_WRITE;
    case GSOCK_CONNECTION: return (int)((socket->m_server) ? GDK_INPUT_READ : GDK_INPUT_WRITE); 
    default: wxASSERT(0); return 0; // This is invalid
  }	
}

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
  wxCHECK_MSG( !socket->m_platform_specific_data, false, wxT("Critical: Double inited socket.") );
  
  socket->m_platform_specific_data = malloc(sizeof(DATATYPE));
  PLATFORM_DATA(socket->m_platform_specific_data) = -1;
  
  return TRUE;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Destroying non-inited socket or double destroy.") );
  
  if (PLATFORM_DATA(socket->m_platform_specific_data) != -1)
    gdk_input_remove(PLATFORM_DATA(socket->m_platform_specific_data));
  
  if (socket->m_platform_specific_data)
    free(socket->m_platform_specific_data);
  
  socket->m_platform_specific_data = NULL;
}

void GSocketGUIFunctionsTableConcrete::SetNewCallback(GSocket* socket)
{
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Setting callback for non-init or destroyed socket") );
  
  if (PLATFORM_DATA(socket->m_platform_specific_data) != -1)
    gdk_input_remove(PLATFORM_DATA(socket->m_platform_specific_data));
  
  if (socket->m_eventflags) // Readd the other events if there's any left
    PLATFORM_DATA(socket->m_platform_specific_data) = gdk_input_add(socket->m_fd,
                          (GdkInputCondition)socket->m_eventflags,
                          _GSocket_GDK_Input,
                          (gpointer)socket);
  else // Set the callback id to -1
    PLATFORM_DATA(socket->m_platform_specific_data) = -1;
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
