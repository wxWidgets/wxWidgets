///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/power.h
// Purpose:     Private GTK-specific power management-related declarations.
// Author:      Vadim Zeitlin
// Created:     2025-02-06
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_POWER_H_
#define _WX_GTK_PRIVATE_POWER_H_

#include <glib.h>

// Power events are only supported under Unix with glib >= 2.26 as this is when
// GDBus support was added and GUnixFDList is only available in 2.30, so just
// test for it to keep things simple.
#if defined(__UNIX__) && GLIB_CHECK_VERSION(2, 30, 0)
    #define wxHAS_GLIB_POWER_SUPPORT
#endif

#endif // _WX_GTK_PRIVATE_POWER_H_
