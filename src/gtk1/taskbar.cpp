/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:     wxTaskBarIcon (src/unix/taskbarx11.cpp) helper for GTK2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/05/29
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "taskbarpriv.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/log.h"
#include "wx/frame.h"

#include <gdk/gdkx.h>

#ifdef __WXGTK20__
#include <gtk/gtkversion.h>
#if GTK_CHECK_VERSION(2, 1, 0)

#include "wx/gtk/taskbarpriv.h"
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
}

bool wxTaskBarIconAreaBase::IsProtocolSupported()
{
    static int s_supported = -1;
    if (s_supported == -1)
    {
        Display *display = GDK_DISPLAY();
        Screen *screen = DefaultScreenOfDisplay(display);
    
        wxString name;
        name.Printf(_T("_NET_SYSTEM_TRAY_S%d"), XScreenNumberOfScreen(screen));
        Atom atom = XInternAtom(display, name.ToAscii(), False);
        
        Window manager = XGetSelectionOwner(display, atom);
        
        s_supported = (manager != None);
    }
    
    return (bool)s_supported;
}

#endif // __WXGTK20__
#endif // GTK_CHECK_VERSION(2, 1, 0)
