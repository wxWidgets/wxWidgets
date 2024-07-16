///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/wrapgdk.h
// Purpose:     Include GDK header for the appropriate window system
// Author:      Vadim Zeitlin
// Created:     2024-07-17
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_WRAPGDK_H_
#define _WX_GTK_PRIVATE_WRAPGDK_H_

#include "wx/gtk/private/wrapgtk.h"

#ifdef GDK_WINDOWING_WAYLAND
    // Wayland headers included from gdkwayland.h may result in -Wundef due to
    // __STDC_VERSION__ used there being undefined, suppress this.
    wxGCC_WARNING_SUPPRESS(undef)

    #include <gdk/gdkwayland.h>

    wxGCC_WARNING_RESTORE(undef)
#endif // GDK_WINDOWING_WAYLAND

#ifdef GDK_WINDOWING_X11
    #include <gdk/gdkx.h>
#endif

#ifdef GDK_WINDOWING_WIN32
    #include <gdk/gdkwin32.h>
#endif

#endif // _WX_GTK_PRIVATE_WRAPGDK_H_
