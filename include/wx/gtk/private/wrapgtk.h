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

wxGCC_WARNING_SUPPRESS(deprecated-declarations)
wxGCC_WARNING_SUPPRESS(parentheses)
#include <gtk/gtk.h>
wxGCC_WARNING_RESTORE(parentheses)
wxGCC_WARNING_RESTORE(deprecated-declarations)

#include "wx/gtk/private/gtk2-compat.h"

#ifdef __ELF__
#define wxHAS_GTK_SELECTION_SET_TARGETS

// (Re)declare this function with weak attribute to allow easily checking
// for its presence at runtime (i.e. easier than with dlsym()).
extern "C"
void gtk_selection_set_targets(GtkWidget *widget,
                               GdkAtom selection,
                               const GtkTargetEntry *targets,
                               guint ntargets) __attribute__((weak));
#endif

#endif // _WX_GTK_PRIVATE_WRAPGTK_H_
