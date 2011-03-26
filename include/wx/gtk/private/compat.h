///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/compat.h
// Purpose:     Compatibility code for older GTK+ versions
// Author:      Vaclav Slavik
// Created:     2011-03-25
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_COMPAT_H_
#define _WX_GTK_PRIVATE_COMPAT_H_

// ----------------------------------------------------------------------------
// Implementations of new accessors for older GTK+ versions
// ----------------------------------------------------------------------------

// GTK+ deprecated direct access to struct members and some other stuff,
// replacing them with simple accessor functions. These aren't available in
// older versions, though, so we have to provide them for compatibility.
//
// Note: wx_ prefix is used to avoid symbol conflicts at runtime


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_has_window(GtkWidget *widget)
    {
        return !GTK_WIDGET_NO_WINDOW(widget);
    }
    #define gtk_widget_get_has_window wx_gtk_widget_get_has_window
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_has_grab(GtkWidget *widget)
    {
        return GTK_WIDGET_HAS_GRAB(widget);
    }
    #define gtk_widget_get_has_grab wx_gtk_widget_get_has_grab
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_visible(GtkWidget *widget)
    {
        return GTK_WIDGET_VISIBLE(widget);
    }
    #define gtk_widget_get_visible wx_gtk_widget_get_visible
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_sensitive(GtkWidget *widget)
    {
        return GTK_WIDGET_VISIBLE(widget);
    }
    #define gtk_widget_get_sensitive wx_gtk_widget_get_sensitive
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_is_drawable(GtkWidget *widget)
    {
        return GTK_WIDGET_DRAWABLE(widget);
    }
    #define gtk_widget_is_drawable wx_gtk_widget_is_drawable
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_can_focus(GtkWidget *widget)
    {
        return GTK_WIDGET_CAN_FOCUS(widget);
    }
    #define gtk_widget_get_can_focus wx_gtk_widget_get_can_focus

    inline void wx_gtk_widset_set_can_focus(GtkWidget *widget, gboolean can)
    {
        if ( can )
            GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_FOCUS);
        else
            GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_FOCUS);
    }
    #define gtk_widset_set_can_focus wx_gtk_widset_set_can_focus
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_can_default(GtkWidget *widget)
    {
        return GTK_WIDGET_CAN_DEFAULT(widget);
    }
    #define gtk_widget_get_can_default wx_gtk_widget_get_can_default

    inline void wx_gtk_widget_set_can_default(GtkWidget *widget, gboolean can)
    {
        if ( can )
            GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_DEFAULT);
        else
            GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_DEFAULT);
    }
    #define gtk_widget_set_can_default wx_gtk_widget_set_can_default
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_has_default(GtkWidget *widget)
    {
        return GTK_WIDGET_HAS_DEFAULT(widget);
    }
    #define gtk_widget_has_default wx_gtk_widget_has_default
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline GtkStateType wx_gtk_widget_get_state(GtkWidget *widget)
    {
        return (GtkStateType)GTK_WIDGET_STATE(widget);
    }
    #define gtk_widget_get_state wx_gtk_widget_get_state
#endif


#if !GTK_CHECK_VERSION(2,18,0)
    inline gboolean wx_gtk_widget_get_double_buffered(GtkWidget *widget)
    {
        return GTK_WIDGET_DOUBLE_BUFFERED(widget);
    }
    #define gtk_widget_get_double_buffered wx_gtk_widget_get_double_buffered
#endif


#if !GTK_CHECK_VERSION(2,20,0)
    inline gboolean wx_gtk_widget_get_realized(GtkWidget *widget)
    {
        return GTK_WIDGET_REALIZED(widget);
    }
    #define gtk_widget_get_realized wx_gtk_widget_get_realized
#endif


#if !GTK_CHECK_VERSION(2,20,0)
    inline gboolean wx_gtk_widget_get_mapped(GtkWidget *widget)
    {
        return GTK_WIDGET_MAPPED(widget);
    }
    #define gtk_widget_get_mapped wx_gtk_widget_get_mapped
#endif


#endif // _WX_GTK_PRIVATE_COMPAT_H_

