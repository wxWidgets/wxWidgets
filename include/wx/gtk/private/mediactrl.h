///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/mediactrl.h
// Purpose:     Wrap runtime checks to manage GTK windows with Wayland and X11
// Author:      Pierluigi Passaro
// Created:     2021-03-18
// Copyright:   (c) 2021 Pierluigi Passaro <pierluigi.p@variscite.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_MEDIACTRL_H_
#define _WX_GTK_PRIVATE_MEDIACTRL_H_

#ifdef GDK_WINDOWING_X11
    #include <gdk/gdkx.h>
#endif
#ifdef GDK_WINDOWING_WAYLAND
    #include <gdk/gdkwayland.h>
#endif
#include "wx/gtk/private/backend.h"

//-----------------------------------------------------------------------------
// "wxGtkGetIdFromWidget" from widget
//
// Get the windows_id performing run-time checks If the window wasn't realized
// when Load was called, this is the callback for when it is - the purpose of
// which is to tell GStreamer to play the video in our control
//-----------------------------------------------------------------------------
extern "C" {
inline gpointer wxGtkGetIdFromWidget(GtkWidget* widget)
{
    gdk_flush();

    GdkWindow* window = gtk_widget_get_window(widget);
    wxASSERT(window);

#ifdef GDK_WINDOWING_X11
#ifdef __WXGTK3__
    if (wxGTKImpl::IsX11(window))
#endif
    {
        return (gpointer)GDK_WINDOW_XID(window);
    }
#endif
#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(window))
    {
        return (gpointer)gdk_wayland_window_get_wl_surface(window);
    }
#endif

    return (gpointer)NULL;
}
}

#endif // _WX_GTK_PRIVATE_MEDIACTRL_H_
