/////////////////////////////////////////////////////////////////////////////
// Name:    src/motif/gsockmot.cpp
// Project: GSocket (Generic Socket) for WX
// Purpose: GSocket: Motif part
// CVSID:   $Id$
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include "wx/gsocket.h"
#include "wx/unix/gsockunx.h"

extern "C" XtAppContext wxGetAppContext();

static void _GSocket_Motif_Input(XtPointer data, int *WXUNUSED(fid),
                                 XtInputId *WXUNUSED(id))
{
    GSocket *socket = (GSocket *)data;

    socket->Detected_Read();
}

static void _GSocket_Motif_Output(XtPointer data, int *WXUNUSED(fid),
                                  XtInputId *WXUNUSED(id))
{
    GSocket *socket = (GSocket *)data;

    socket->Detected_Write();
}

bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{
    return true;
}

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
        XtRemoveInput(m_id[c]);

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
