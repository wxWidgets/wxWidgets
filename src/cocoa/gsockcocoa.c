/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockmot.c
 * Purpose: GSocket: X11 part
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/gsocket.h"

int _GSocket_GUI_Init(GSocket *socket)
{
    return 0;
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

void _GSocket_Enable_Events(GSocket *socket)
{
}

void _GSocket_Disable_Events(GSocket *socket)
{
}

#endif // wxUSE_SOCKETS
