/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockno.c
 * Purpose: GSocket stub GUI file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */
#include <stdlib.h>
#include "gsocket.h"

void _GSocket_GUI_Init(GSocket *socket)
{
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
}

unsigned long GSocket_GetEventID(GSocket *socket)
{
  return 0;
}

void GSocket_DoEvent(unsigned long evt_id)
{
}
