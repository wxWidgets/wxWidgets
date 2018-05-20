///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/wrapgtk.h
// Purpose:     Include gtk/gtk.h without warnings and with compatibility
// Author:      Vadim Zeitlin
// Created:     2018-05-20
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_WRAPGTK_H_
#define _WX_GTK_PRIVATE_WRAPGTK_H_

wxGCC_WARNING_SUPPRESS(parentheses)
#include <gtk/gtk.h>
wxGCC_WARNING_RESTORE(parentheses)

#include "wx/gtk/private/gtk2-compat.h"

#endif // _WX_GTK_PRIVATE_WRAPGTK_H_
