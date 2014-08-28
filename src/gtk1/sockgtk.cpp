///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/sockgtk.cpp
// Purpose:     implementation of wxGTK-specific socket event handling
// Author:      Guilhem Lavaux, Vadim Zeitlin
// Created:     1999
// Copyright:   (c) 1999, 2007 wxWidgets dev team
//              (c) 2009 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".

#if defined( __VMS ) && defined( __ia64 )
// Work around for a bug in the C++ compiler on OpenVMS IA64
# include <time.h>
#endif

#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <gdk/gdk.h>
#include <glib.h>

#include "wx/apptrait.h"
#include "wx/private/fdiomanager.h"

extern "C" {
static
void wxSocket_GDK_Input(gpointer data,
                        gint WXUNUSED(source),
                        GdkInputCondition condition)
{
    wxFDIOHandler * const handler = static_cast<wxFDIOHandler *>(data);

    if ( condition & GDK_INPUT_READ )
    {
        handler->OnReadWaiting();

        // we could have lost connection while reading in which case we
        // shouldn't call OnWriteWaiting() as the socket is now closed and it
        // would assert
        if ( !handler->IsOk() )
            return;
    }

    if ( condition & GDK_INPUT_WRITE )
        handler->OnWriteWaiting();
}
}

class GTKFDIOManager : public wxFDIOManager
{
public:
    virtual int AddInput(wxFDIOHandler *handler, int fd, Direction d)
    {
        return gdk_input_add
               (
                    fd,
                    d == OUTPUT ? GDK_INPUT_WRITE : GDK_INPUT_READ,
                    wxSocket_GDK_Input,
                    handler
               );
    }

    virtual void
    RemoveInput(wxFDIOHandler* WXUNUSED(handler), int fd, Direction WXUNUSED(d))
    {
        gdk_input_remove(fd);
    }
};

wxFDIOManager *wxGUIAppTraits::GetFDIOManager()
{
    static GTKFDIOManager s_manager;
    return &s_manager;
}

#endif // wxUSE_SOCKETS
