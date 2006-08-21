/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    src/x11/gsockx11.cpp
 * Purpose: GSocket: X11 part
 * Licence: The wxWindows licence
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/gsocket.h"

/*
 * FIXME: have these in a common header instead of being repeated
 * in evtloop.cpp and gsockx11.c
 */

typedef void (*wxSocketCallback) (int fd, void* data);

typedef enum
{ wxSocketTableInput, wxSocketTableOutput } wxSocketTableType ;

extern "C" void wxRegisterSocketCallback(int fd, wxSocketTableType socketType, wxSocketCallback cback, void* data);
extern "C" void wxUnregisterSocketCallback(int fd, wxSocketTableType socketType);

typedef struct {
  int input;
  int output;
} CallbackData;

#define GSOCKX11_INPUT 1
#define GSOCKX11_OUTPUT 2

#define DATATYPE CallbackData
#define PLATFORM_DATA(x) (*(DATATYPE*)x)

static void _GSocket_X11_Input(int *fid, void* data)
{
  GSocket *socket = (GSocket *)data;

  socket->Detected_Read();
}

static void _GSocket_X11_Output(int *fid, void* data)
{
  GSocket *socket = (GSocket *)data;

  socket->Detected_Write();
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

  socket->m_platform_specific_data = malloc(sizeof(DATATYPE));
  PLATFORM_DATA(socket->m_platform_specific_data).input = -1;
  PLATFORM_DATA(socket->m_platform_specific_data).output = -1;
  
  return true;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  // Just to be sure
  Disable_Events(socket);
  
  if (socket->m_platform_specific_data)
    free(socket->m_platform_specific_data);
  
  socket->m_platform_specific_data = NULL; 
}

int TranslateEventCondition(GSocket* socket, GSocketEvent event) {
  switch (event)
  {
    case GSOCK_LOST:       
    case GSOCK_INPUT:      return GSOCKX11_INPUT; 
    case GSOCK_OUTPUT:   return GSOCKX11_OUTPUT;
    case GSOCK_CONNECTION: return ((socket->m_server) ? GSOCKX11_INPUT : GSOCKX11_OUTPUT); 
    default: wxASSERT(0); return 0; // This is invalid
  }	
}

// Helper functions for {En|Dis}able*

void CheckCurrentState(GSocket* socket, char event)
{
  int* current_state;
  wxSocketTableType table;
  wxSocketCallback callback;
  switch (event)
  {
    case GSOCKX11_INPUT:
      current_state = &(PLATFORM_DATA(socket->m_platform_specific_data).input);
      table = wxSocketTableInput;
      callback = (wxSocketCallback)_GSocket_X11_Input;
      break;
    case GSOCKX11_OUTPUT:
      current_state = &(PLATFORM_DATA(socket->m_platform_specific_data).output);
      table = wxSocketTableOutput;
      callback = (wxSocketCallback)_GSocket_X11_Output;
      break;
    default:
      wxASSERT_MSG(0,wxT("Error: Checking socket state for unknown event type\n"));
      return;
  }
  
  if (*current_state != -1)
  {
    if (!(socket->m_eventflags & event))
    {
      // We have to use *current_state here because the socket->fd 
      // might have already been set to -1.
      wxUnregisterSocketCallback(*current_state, table);
      *current_state = -1;
    }
  }
  else if (socket->m_eventflags | event)
  {
    wxRegisterSocketCallback(socket->m_fd, table, callback, (void*) socket);
    *current_state = socket->m_fd;
  } 
}

void SetNewCallback(GSocket* socket)
{
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Setting callback for non-init or destroyed socket") );

  // This functions updates each callback table
  
  CheckCurrentState(socket, GSOCKX11_INPUT);
  CheckCurrentState(socket, GSOCKX11_OUTPUT);
}

void GSocketGUIFunctionsTableConcrete::Enable_Event(GSocket *socket, GSocketEvent event)
{
  wxCHECK_RET( event < GSOCK_MAX_EVENT, wxT("Critical: trying to install callback for an unknown socket event") );

  if ( socket->m_fd == -1 )
    return;

  int internal_event = TranslateEventCondition(socket, event);
  
  if (socket->m_eventflags & internal_event)
    return;
  
  socket->m_eventflags |= internal_event;
  
  // Don't use SetCallback here - It would check the other internal event too
  CheckCurrentState(socket, internal_event);
}

void GSocketGUIFunctionsTableConcrete::Disable_Event(GSocket *socket, GSocketEvent event)
{
  wxCHECK_RET( event < GSOCK_MAX_EVENT, wxT("Critical: trying to uninstall callback for an unknown socket event") );

  if ( socket->m_fd == -1 )
    return;

  int internal_event = TranslateEventCondition(socket, event);
  
  if (!(socket->m_eventflags & internal_event))
    return;
  
  socket->m_eventflags &= ~internal_event;
  
  // Don't use SetCallback here - It would check the other internal event too
  CheckCurrentState(socket, internal_event);
}

void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  socket->m_eventflags = TranslateEventCondition(socket, GSOCK_INPUT)
                                        | TranslateEventCondition(socket, GSOCK_OUTPUT);
  SetNewCallback(socket);
}

void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  socket->m_eventflags = 0;
  SetNewCallback(socket);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
