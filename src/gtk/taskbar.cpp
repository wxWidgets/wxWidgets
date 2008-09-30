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

#if wxUSE_TASKBARICON

#include "wx/gtk/taskbarpriv.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "eggtrayicon.h"

wxTaskBarIconAreaBase::wxTaskBarIconAreaBase()
{
    if (IsProtocolSupported())
    {
        m_widget = GTK_WIDGET(egg_tray_icon_new("systray icon"));
        g_object_ref(m_widget);
        gtk_window_set_resizable(GTK_WINDOW(m_widget), false);

        wxLogTrace(_T("systray"), _T("using freedesktop.org systray spec"));
    }

    wxTopLevelWindow::Create(
            NULL, wxID_ANY, _T("systray icon"),
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR | wxSIMPLE_BORDER |
            wxFRAME_SHAPED,
            wxEmptyString /*eggtray doesn't like setting wmclass*/);

    // WM frame extents are not useful for wxTaskBarIcon
    m_deferShow = false;
    gulong handler_id = g_signal_handler_find(
        m_widget,
        GSignalMatchType(G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA),
        g_signal_lookup("property_notify_event", GTK_TYPE_WIDGET),
        0, NULL, NULL, this);
    if (handler_id != 0)
        g_signal_handler_disconnect(m_widget, handler_id);

    m_invokingWindow = NULL;
}

bool wxTaskBarIconAreaBase::IsProtocolSupported()
{
    Display *display = GDK_DISPLAY();
    Screen *screen = DefaultScreenOfDisplay(display);

    char name[32];
    g_snprintf(name, sizeof(name), "_NET_SYSTEM_TRAY_S%d",
        XScreenNumberOfScreen(screen));
    Atom atom = XInternAtom(display, name, False);

    Window manager = XGetSelectionOwner(display, atom);

    return (manager != None);
}

//-----------------------------------------------------------------------------
// Pop-up menu stuff
//-----------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE
void wxTaskBarIconAreaBase::DoPopupMenuUpdateUI(wxMenu* menu)
{
    menu->UpdateUI(m_invokingWindow);
}
#endif // wxUSE_MENUS_NATIVE
#endif // wxUSE_TASKBARICON
