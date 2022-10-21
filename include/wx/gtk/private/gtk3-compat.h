///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/gtk3-compat.h
// Purpose:     Compatibility code for older GTK+ 3 versions
// Author:      Paul Cornett
// Created:     2015-10-10
// Copyright:   (c) 2015 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_COMPAT3_H_
#define _WX_GTK_PRIVATE_COMPAT3_H_

#ifdef __WXGTK4__

inline GdkDevice* wx_get_gdk_device_from_display(GdkDisplay* display)
{
    GdkSeat* seat = gdk_display_get_default_seat(display);
    return gdk_seat_get_pointer(seat);
}

#else // !__WXGTK4__

wxGCC_WARNING_SUPPRESS(deprecated-declarations)

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 3.20

static inline gboolean wx_gtk_text_iter_starts_tag(const GtkTextIter* iter, GtkTextTag* tag)
{
    return gtk_text_iter_begins_tag(iter, tag);
}
#define gtk_text_iter_starts_tag wx_gtk_text_iter_starts_tag

#ifdef __WXGTK3__

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 3.12

static inline void wx_gtk_widget_set_margin_start(GtkWidget* widget, gint margin)
{
    gtk_widget_set_margin_left(widget, margin);
}
#define gtk_widget_set_margin_start wx_gtk_widget_set_margin_start

static inline void wx_gtk_widget_set_margin_end(GtkWidget* widget, gint margin)
{
    gtk_widget_set_margin_right(widget, margin);
}
#define gtk_widget_set_margin_end wx_gtk_widget_set_margin_end

inline GdkDevice* wx_get_gdk_device_from_display(GdkDisplay* display)
{
    GdkDeviceManager* manager = gdk_display_get_device_manager(display);
    return gdk_device_manager_get_client_pointer(manager);
}

#endif // __WXGTK3__

wxGCC_WARNING_RESTORE()

#endif // __WXGTK4__/!__WXGTK4__

#if defined(__WXGTK4__) || !defined(__WXGTK3__)
static inline bool wx_is_at_least_gtk3(int /* minor */)
{
#ifdef __WXGTK4__
    return true;
#else
    return false;
#endif
}
#else
static inline bool wx_is_at_least_gtk3(int minor)
{
    return gtk_check_version(3, minor, 0) == nullptr;
}
#endif

#endif // _WX_GTK_PRIVATE_COMPAT3_H_
