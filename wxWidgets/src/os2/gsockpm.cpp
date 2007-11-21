/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockpm.c
 * Purpose: GSocket: PM part
 * Licence: The wxWindows licence
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/unix/gsockunx.h"
#include "wx/gsocket.h"
#include "wx/app.h"

#define wxSockReadMask  0x01
#define wxSockWriteMask 0x02

static void _GSocket_PM_Input(void *data)
{
    GSocket *socket = (GSocket *) data;
    socket->Detected_Read();
}

static void _GSocket_PM_Output(void *data)
{
    GSocket *socket = (GSocket *) data;
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
    int *m_id;
    socket->m_gui_dependent = (char *)malloc(sizeof(int)*2);
    m_id = (int *)(socket->m_gui_dependent);

    m_id[0] = -1;
    m_id[1] = -1;
    return true;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
    free(socket->m_gui_dependent);
}

void GSocketGUIFunctionsTableConcrete::Install_Callback(GSocket *socket, GSocketEvent event)
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
        wxTheApp->RemoveSocketHandler(m_id[c]);

    if (c == 0)
    {
        m_id[0] = wxTheApp->AddSocketHandler(socket->m_fd, wxSockReadMask,
                                             _GSocket_PM_Input, (void *)socket);
    }
    else
    {
        m_id[1] = wxTheApp->AddSocketHandler(socket->m_fd, wxSockWriteMask,
                                             _GSocket_PM_Output, (void *)socket);
    }
}

void GSocketGUIFunctionsTableConcrete::Uninstall_Callback(GSocket *socket, GSocketEvent event)
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
        wxTheApp->RemoveSocketHandler(m_id[c]);

    m_id[c] = -1;
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
