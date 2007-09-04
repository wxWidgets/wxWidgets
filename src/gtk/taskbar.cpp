/////////////////////////////////////////////////////////////////////////
// File:        src/gtk/taskbar.cpp
// Purpose:     wxTaskBarIcon (src/unix/taskbarx11.cpp) helper for GTK2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/05/29
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gtk/taskbarpriv.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
#endif

#include <gdk/gdkx.h>

#ifdef __WXGTK20__
#include <gtk/gtkversion.h>
#if GTK_CHECK_VERSION(2, 1, 0)

#include "gtk/gtk.h"

#include "eggtrayicon.h"

wxTaskBarIconAreaBase::wxTaskBarIconAreaBase()
{
    if (IsProtocolSupported())
    {
        m_widget = GTK_WIDGET(egg_tray_icon_new("systray icon"));
        gtk_window_set_resizable(GTK_WINDOW(m_widget), false);

        wxLogTrace(_T("systray"), _T("using freedesktop.org systray spec"));
    }

    wxTopLevelWindow::Create(
            NULL, wxID_ANY, _T("systray icon"),
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR | wxSIMPLE_BORDER |
            wxFRAME_SHAPED,
            wxEmptyString /*eggtray doesn't like setting wmclass*/);

    m_invokingWindow = NULL;
}

bool wxTaskBarIconAreaBase::IsProtocolSupported()
{
    static int s_supported = -1;
    if (s_supported == -1)
    {
        Display *display = GDK_DISPLAY();
        Screen *screen = DefaultScreenOfDisplay(display);

        char name[32];
        g_snprintf(name, sizeof(name), "_NET_SYSTEM_TRAY_S%d",
            XScreenNumberOfScreen(screen));
        Atom atom = XInternAtom(display, name, False);

        Window manager = XGetSelectionOwner(display, atom);

        s_supported = (manager != None);
    }

    return (bool)s_supported;
}

//-----------------------------------------------------------------------------
// Pop-up menu stuff
//-----------------------------------------------------------------------------

extern "C" WXDLLIMPEXP_CORE void gtk_pop_hide_callback( GtkWidget *widget, bool* is_waiting  );

extern WXDLLIMPEXP_CORE void SetInvokingWindow( wxMenu *menu, wxWindow* win );

extern "C" WXDLLIMPEXP_CORE
    void wxPopupMenuPositionCallback( GtkMenu *menu,
                                      gint *x, gint *y,
                                      gboolean * WXUNUSED(whatever),
                                      gpointer user_data );

#if wxUSE_MENUS_NATIVE
bool wxTaskBarIconAreaBase::DoPopupMenu( wxMenu *menu, int x, int y )
{
    wxCHECK_MSG( m_widget != NULL, false, wxT("invalid window") );

    wxCHECK_MSG( menu != NULL, false, wxT("invalid popup-menu") );

    // NOTE: if you change this code, you need to update
    //       the same code in window.cpp as well. This
    //       is ugly code duplication, I know,

    SetInvokingWindow( menu, this );

    menu->UpdateUI( m_invokingWindow );

    bool is_waiting = true;

    gulong handler = g_signal_connect (menu->m_menu, "hide",
                                       G_CALLBACK (gtk_pop_hide_callback),
                                       &is_waiting);

    wxPoint pos;
    gpointer userdata;
    GtkMenuPositionFunc posfunc;
    if ( x == -1 && y == -1 )
    {
        // use GTK's default positioning algorithm
        userdata = NULL;
        posfunc = NULL;
    }
    else
    {
        pos = ClientToScreen(wxPoint(x, y));
        userdata = &pos;
        posfunc = wxPopupMenuPositionCallback;
    }

    gtk_menu_popup(
                  GTK_MENU(menu->m_menu),
                  (GtkWidget *) NULL,           // parent menu shell
                  (GtkWidget *) NULL,           // parent menu item
                  posfunc,                      // function to position it
                  userdata,                     // client data
                  0,                            // button used to activate it
                  gtk_get_current_event_time()
                );

    while (is_waiting)
    {
        gtk_main_iteration();
    }

    g_signal_handler_disconnect (menu->m_menu, handler);

    return true;
}
#endif // wxUSE_MENUS_NATIVE

#endif // __WXGTK20__
#endif // GTK_CHECK_VERSION(2, 1, 0)
