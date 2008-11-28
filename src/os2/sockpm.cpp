/* -------------------------------------------------------------------------
 * Project: wxSocketImpl (Generic Socket) for WX
 * Name:    gsockpm.c
 * Purpose: wxSocketImpl: PM part
 * Licence: The wxWindows licence
 * CVSID:   $Id$
 * ------------------------------------------------------------------------- */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/private/socket.h"
#include "wx/app.h"
#include "wx/apptrait.h"

#define wxSockReadMask  0x01
#define wxSockWriteMask 0x02

static void wxSocket_PM_Input(void *data)
{
    wxSocketImpl *socket = static_cast<wxSocketImpl *>(data);

    socket->Detected_Read();
}

static void wxSocket_PM_Output(void *data)
{
    wxSocketImpl *socket = static_cast<wxSocketImpl *>(data);

    socket->Detected_Write();
}

class PMSocketManager : public wxSocketInputBasedManager
{
public:
    virtual int AddInput(wxSocketImpl *socket, SocketDir d)
    {

      if (d == FD_OUTPUT)
          return wxTheApp->AddSocketHandler(socket->m_fd, wxSockWriteMask,
                                            wxSocket_PM_Output, (void *)socket);
      else
          return wxTheApp->AddSocketHandler(socket->m_fd, wxSockReadMask,
                                            wxSocket_PM_Input, (void *)socket);
    }

    virtual void RemoveInput(int fd)
    {
        wxTheApp->RemoveSocketHandler(fd);
    }
};

wxSocketManager *wxGUIAppTraits::GetSocketManager()
{
    static PMSocketManager s_manager;
    return &s_manager;
}


#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
