/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockmot.c
 * Purpose: GSocket: Motif part
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"

extern XtAppContext wxGetAppContext();

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

  socket->m_gui_dependent = (char *)malloc(sizeof(int)*2);
  m_id = (int *)(socket->m_gui_dependent);

  m_id[0] = -1;
  m_id[1] = -1;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
  free(socket->m_gui_dependent);
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
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
    XtRemoveInput(m_id[c]);

  if (c == 0)
  {
     m_id[0] = XtAppAddInput(wxGetAppContext(), socket->m_fd,
                             (XtPointer *)XtInputReadMask,
                             (XtInputCallbackProc) _GSocket_Motif_Input,
                             (XtPointer) socket);
  }
  else
  {
     m_id[1] = XtAppAddInput(wxGetAppContext(), socket->m_fd,
                             (XtPointer *)XtInputWriteMask,
                             (XtInputCallbackProc) _GSocket_Motif_Output,
                             (XtPointer) socket);
  }
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
  int *m_id = (int *)(socket->m_gui_dependent);
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
    XtRemoveInput(m_id[c]);

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

#endif /* wxUSE_SOCKETS */
