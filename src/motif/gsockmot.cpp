// -------------------------------------------------------------------------
// Project: GSocket (Generic Socket) for WX
// Name:    gsockmot.cpp
// Purpose: GSocket: Motif part
// CVSID:   $Id$
// Log:     $Log$
// Log:     Revision 1.1  1999/07/23 17:24:24  GL
// Log:     Added GSocket motif (it compiles but I didn't tested it)
// Log:     Changed wxSockFlags type (switched to int)
// Log:
// -------------------------------------------------------------------------
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <wx/gsocket.h>
#include <wx/app.h>
#include "../unix/gsockunx.h"

#define wxAPP_CONTEXT ((XtAppContext)wxTheApp->GetAppContext())

static void _GSocket_Motif_Input(XtPointer data, int *fid,
                                 XtInputId *id)
{
  GSocket *socket = (GSocket *)data;

  _GSocket_Detected_Read(socket);
}

static void _GSocket_Motif_Output(XtPointer data, int *fid,
                                  XtInputId *id)
{
  GSocket *socket = (GSocket *)data;

  _GSocket_Detected_Write(socket);
}

void _GSocket_GUI_Init(GSocket *socket)
{
  int i;
  int *m_id;

  socket->m_gui_dependent = (char *)malloc(sizeof(int)*3);
  m_id = (int *)(socket->m_gui_dependent);

  for (i=0;i<3;i++)
    m_id[i] = -1;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
  int i;
  int *m_id;

  m_id = (int *)(socket->m_gui_dependent);

  for (i=0;i<3;i++)
    if (m_id[i] == -1)
      XtRemoveInput(m_id[i]);

  free(socket->m_gui_dependent);
}

void _GSocket_Install_Fallback(GSocket *socket, GSocketEvent event)
{
  int *m_id;

  m_id = (int *)(socket->m_gui_dependent);

  switch (event) {
  case GSOCK_CONNECTION:
  case GSOCK_LOST:
  case GSOCK_INPUT: 
     if (m_id[0] != -1)
       XtRemoveInput(m_id[0]);
     m_id[0] = XtAppAddInput(wxAPP_CONTEXT, socket->m_fd,
                             (XtPointer *)XtInputReadMask,
                             (XtInputCallbackProc) _GSocket_Motif_Input,
                             (XtPointer) socket);
     break;
  case GSOCK_OUTPUT:
     if (m_id[1] != -1)
       XtRemoveInput(m_id[1]);
     m_id[1] = XtAppAddInput(wxAPP_CONTEXT, socket->m_fd,
                             (XtPointer *)XtInputWriteMask,
                             (XtInputCallbackProc) _GSocket_Motif_Output,
                             (XtPointer) socket);
     break;
  default: return;
  }
}

void _GSocket_Uninstall_Fallback(GSocket *socket, GSocketEvent event)
{
  int c;
  int *m_id;

  m_id = (int *)(socket->m_gui_dependent);

  switch (event) {
  case GSOCK_CONNECTION: 
  case GSOCK_LOST:
  case GSOCK_INPUT: c = 0; break;
  case GSOCK_OUTPUT: c = 1; break;
     break;
  default: return;
  }

  if (m_id[c] != -1)
    XtRemoveInput(m_id[c]);

  m_id[c] = -1;
}

unsigned long GSocket_GetEventID(GSocket *socket)
{
  return 0;
}

void GSocket_DoEvent(unsigned long evt_id)
{
}
