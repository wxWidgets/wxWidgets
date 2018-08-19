///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/compat.h
// Purpose:     Compatibility code for older GTK+ versions
// Author:      Vaclav Slavik
// Created:     2011-03-25
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
//
// Note 2: We support building against newer GTK+ version and using an older
//         one at runtime, so we must provide our implementations of these
//         functions even if GTK_CHECK_VERSION would indicate the function is
//         already available in GTK+.

#ifndef __WXGTK3__

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.8

static inline GtkWidget* wx_gtk_scrolled_window_get_hscrollbar(GtkScrolledWindow* scrolled_window)
{
    return scrolled_window->hscrollbar;
}
#define gtk_scrolled_window_get_hscrollbar wx_gtk_scrolled_window_get_hscrollbar

static inline GtkWidget* wx_gtk_scrolled_window_get_vscrollbar(GtkScrolledWindow* scrolled_window)
{
    return scrolled_window->vscrollbar;
}
#define gtk_scrolled_window_get_vscrollbar wx_gtk_scrolled_window_get_vscrollbar

// ----------------------------------------------------------------------------
// the following were introduced in GLib 2.10

static inline gpointer wx_g_object_ref_sink(gpointer object)
{
    g_object_ref(object);
    gtk_object_sink(GTK_OBJECT(object));
    return object;
}
#define g_object_ref_sink wx_g_object_ref_sink

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.12

static inline void wx_gtk_about_dialog_set_program_name(GtkAboutDialog* about, const gchar* name)
{
    gtk_about_dialog_set_name(about, name);
}
#define gtk_about_dialog_set_program_name wx_gtk_about_dialog_set_program_name

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.14

static inline gdouble wx_gtk_adjustment_get_lower(GtkAdjustment* adjustment)
{
    return adjustment->lower;
}
#define gtk_adjustment_get_lower wx_gtk_adjustment_get_lower

static inline gdouble wx_gtk_adjustment_get_page_increment(GtkAdjustment* adjustment)
{
    return adjustment->page_increment;
}
#define gtk_adjustment_get_page_increment wx_gtk_adjustment_get_page_increment

static inline gdouble wx_gtk_adjustment_get_page_size(GtkAdjustment* adjustment)
{
    return adjustment->page_size;
}
#define gtk_adjustment_get_page_size wx_gtk_adjustment_get_page_size

static inline gdouble wx_gtk_adjustment_get_step_increment(GtkAdjustment* adjustment)
{
    return adjustment->step_increment;
}
#define gtk_adjustment_get_step_increment wx_gtk_adjustment_get_step_increment

static inline gdouble wx_gtk_adjustment_get_upper(GtkAdjustment* adjustment)
{
    return adjustment->upper;
}
#define gtk_adjustment_get_upper wx_gtk_adjustment_get_upper

static inline void wx_gtk_adjustment_set_page_size(GtkAdjustment* adjustment, gdouble page_size)
{
    adjustment->page_size = page_size;
}
#define gtk_adjustment_set_page_size wx_gtk_adjustment_set_page_size

static inline GtkWidget* wx_gtk_color_selection_dialog_get_color_selection(GtkColorSelectionDialog* csd)
{
    return csd->colorsel;
}
#define gtk_color_selection_dialog_get_color_selection wx_gtk_color_selection_dialog_get_color_selection

static inline GtkWidget* wx_gtk_dialog_get_content_area(GtkDialog* dialog)
{
    return dialog->vbox;
}
#define gtk_dialog_get_content_area wx_gtk_dialog_get_content_area

static inline GtkWidget* wx_gtk_dialog_get_action_area(GtkDialog* dialog)
{
    return dialog->action_area;
}
#define gtk_dialog_get_action_area wx_gtk_dialog_get_action_area

static inline guint16 wx_gtk_entry_get_text_length(GtkEntry* entry)
{
    return entry->text_length;
}
#define gtk_entry_get_text_length wx_gtk_entry_get_text_length

static inline const guchar* wx_gtk_selection_data_get_data(GtkSelectionData* selection_data)
{
    return selection_data->data;
}
#define gtk_selection_data_get_data wx_gtk_selection_data_get_data

static inline GdkAtom wx_gtk_selection_data_get_data_type(GtkSelectionData* selection_data)
{
    return selection_data->type;
}
#define gtk_selection_data_get_data_type wx_gtk_selection_data_get_data_type

static inline gint wx_gtk_selection_data_get_format(GtkSelectionData* selection_data)
{
    return selection_data->format;
}
#define gtk_selection_data_get_format wx_gtk_selection_data_get_format

static inline gint wx_gtk_selection_data_get_length(GtkSelectionData* selection_data)
{
    return selection_data->length;
}
#define gtk_selection_data_get_length wx_gtk_selection_data_get_length

static inline GdkAtom wx_gtk_selection_data_get_target(GtkSelectionData* selection_data)
{
    return selection_data->target;
}
#define gtk_selection_data_get_target wx_gtk_selection_data_get_target

static inline GdkWindow* wx_gtk_widget_get_window(GtkWidget* widget)
{
    return widget->window;
}
#define gtk_widget_get_window wx_gtk_widget_get_window

static inline GtkWidget* wx_gtk_window_get_default_widget(GtkWindow* window)
{
    return window->default_widget;
}
#define gtk_window_get_default_widget wx_gtk_window_get_default_widget

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.16

static inline GdkAtom wx_gtk_selection_data_get_selection(GtkSelectionData* selection_data)
{
    return selection_data->selection;
}
#define gtk_selection_data_get_selection wx_gtk_selection_data_get_selection

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.18

static inline void wx_gtk_cell_renderer_get_alignment(GtkCellRenderer* cell, gfloat* xalign, gfloat* yalign)
{
    *xalign = cell->xalign;
    *yalign = cell->yalign;
}
#define gtk_cell_renderer_get_alignment wx_gtk_cell_renderer_get_alignment

static inline void wx_gtk_cell_renderer_get_padding(GtkCellRenderer* cell, gint* xpad, gint* ypad)
{
    *xpad = cell->xpad;
    *ypad = cell->ypad;
}
#define gtk_cell_renderer_get_padding wx_gtk_cell_renderer_get_padding

static inline void wx_gtk_cell_renderer_set_padding(GtkCellRenderer* cell, gint xpad, gint ypad)
{
    cell->xpad = xpad;
    cell->ypad = ypad;
}
#define gtk_cell_renderer_set_padding wx_gtk_cell_renderer_set_padding

static inline void wx_gtk_widget_get_allocation(GtkWidget* widget, GtkAllocation* allocation)
{
    *allocation = widget->allocation;
}
#define gtk_widget_get_allocation wx_gtk_widget_get_allocation

inline gboolean wx_gtk_widget_get_has_window(GtkWidget *widget)
{
    return !GTK_WIDGET_NO_WINDOW(widget);
}
#define gtk_widget_get_has_window wx_gtk_widget_get_has_window


inline gboolean wx_gtk_widget_get_has_grab(GtkWidget *widget)
{
    return GTK_WIDGET_HAS_GRAB(widget);
}
#define gtk_widget_get_has_grab wx_gtk_widget_get_has_grab


inline gboolean wx_gtk_widget_get_visible(GtkWidget *widget)
{
    return GTK_WIDGET_VISIBLE(widget);
}
#define gtk_widget_get_visible wx_gtk_widget_get_visible


inline gboolean wx_gtk_widget_get_sensitive(GtkWidget *widget)
{
    return GTK_WIDGET_SENSITIVE(widget);
}
#define gtk_widget_get_sensitive wx_gtk_widget_get_sensitive


inline gboolean wx_gtk_widget_is_drawable(GtkWidget *widget)
{
    return GTK_WIDGET_DRAWABLE(widget);
}
#define gtk_widget_is_drawable wx_gtk_widget_is_drawable


inline gboolean wx_gtk_widget_get_can_focus(GtkWidget *widget)
{
    return GTK_WIDGET_CAN_FOCUS(widget);
}
#define gtk_widget_get_can_focus wx_gtk_widget_get_can_focus

inline void wx_gtk_widget_set_can_focus(GtkWidget *widget, gboolean can)
{
    if ( can )
        GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_FOCUS);
    else
        GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_FOCUS);
}
#define gtk_widget_set_can_focus wx_gtk_widget_set_can_focus

static inline gboolean wx_gtk_widget_has_focus(GtkWidget* widget)
{
    return GTK_WIDGET_HAS_FOCUS(widget);
}
#define gtk_widget_has_focus wx_gtk_widget_has_focus

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


inline gboolean wx_gtk_widget_has_default(GtkWidget *widget)
{
    return GTK_WIDGET_HAS_DEFAULT(widget);
}
#define gtk_widget_has_default wx_gtk_widget_has_default


inline GtkStateType wx_gtk_widget_get_state(GtkWidget *widget)
{
    return (GtkStateType)GTK_WIDGET_STATE(widget);
}
#define gtk_widget_get_state wx_gtk_widget_get_state


inline gboolean wx_gtk_widget_get_double_buffered(GtkWidget *widget)
{
    return GTK_WIDGET_DOUBLE_BUFFERED(widget);
}
#define gtk_widget_get_double_buffered wx_gtk_widget_get_double_buffered

static inline gboolean wx_gtk_widget_has_grab(GtkWidget* widget)
{
    return GTK_WIDGET_HAS_GRAB(widget);
}
#define gtk_widget_has_grab wx_gtk_widget_has_grab

static inline void wx_gtk_widget_set_allocation(GtkWidget* widget, const GtkAllocation* allocation)
{
    widget->allocation = *allocation;
}
#define gtk_widget_set_allocation wx_gtk_widget_set_allocation

static inline gboolean wx_gtk_widget_is_toplevel(GtkWidget* widget)
{
    return GTK_WIDGET_TOPLEVEL(widget);
}
#define gtk_widget_is_toplevel wx_gtk_widget_is_toplevel

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.20

inline gboolean wx_gtk_widget_get_realized(GtkWidget *widget)
{
    return GTK_WIDGET_REALIZED(widget);
}
#define gtk_widget_get_realized wx_gtk_widget_get_realized


inline gboolean wx_gtk_widget_get_mapped(GtkWidget *widget)
{
    return GTK_WIDGET_MAPPED(widget);
}
#define gtk_widget_get_mapped wx_gtk_widget_get_mapped

static inline void wx_gtk_widget_get_requisition(GtkWidget* widget, GtkRequisition* requisition)
{
    *requisition = widget->requisition;
}
#define gtk_widget_get_requisition wx_gtk_widget_get_requisition

static inline GdkWindow* wx_gtk_entry_get_text_window(GtkEntry* entry)
{
    return entry->text_area;
}
#define gtk_entry_get_text_window wx_gtk_entry_get_text_window

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.22

static inline GdkWindow* wx_gtk_button_get_event_window(GtkButton* button)
{
    return button->event_window;
}
#define gtk_button_get_event_window wx_gtk_button_get_event_window

static inline GdkDragAction wx_gdk_drag_context_get_actions(GdkDragContext* context)
{
    return context->actions;
}
#define gdk_drag_context_get_actions wx_gdk_drag_context_get_actions

static inline GdkDragAction wx_gdk_drag_context_get_selected_action(GdkDragContext* context)
{
    return context->action;
}
#define gdk_drag_context_get_selected_action wx_gdk_drag_context_get_selected_action

static inline GdkDragAction wx_gdk_drag_context_get_suggested_action(GdkDragContext* context)
{
    return context->suggested_action;
}
#define gdk_drag_context_get_suggested_action wx_gdk_drag_context_get_suggested_action

static inline GList* wx_gdk_drag_context_list_targets(GdkDragContext* context)
{
    return context->targets;
}
#define gdk_drag_context_list_targets wx_gdk_drag_context_list_targets

static inline gint wx_gdk_visual_get_depth(GdkVisual* visual)
{
    return visual->depth;
}
#define gdk_visual_get_depth wx_gdk_visual_get_depth

static inline gboolean wx_gtk_window_has_group(GtkWindow* window)
{
    return window->group != NULL;
}
#define gtk_window_has_group wx_gtk_window_has_group

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 2.24

#define gdk_window_get_visual gdk_drawable_get_visual

static inline GdkDisplay* wx_gdk_window_get_display(GdkWindow* window)
{
    return gdk_drawable_get_display(window);
}
#define gdk_window_get_display wx_gdk_window_get_display

static inline GdkScreen* wx_gdk_window_get_screen(GdkWindow* window)
{
    return gdk_drawable_get_screen(window);
}
#define gdk_window_get_screen wx_gdk_window_get_screen

static inline gint wx_gdk_window_get_height(GdkWindow* window)
{
    int h;
    gdk_drawable_get_size(window, NULL, &h);
    return h;
}
#define gdk_window_get_height wx_gdk_window_get_height

static inline gint wx_gdk_window_get_width(GdkWindow* window)
{
    int w;
    gdk_drawable_get_size(window, &w, NULL);
    return w;
}
#define gdk_window_get_width wx_gdk_window_get_width

#if GTK_CHECK_VERSION(2,10,0)
static inline void wx_gdk_cairo_set_source_window(cairo_t* cr, GdkWindow* window, gdouble x, gdouble y)
{
    gdk_cairo_set_source_pixmap(cr, window, x, y);
}
#define gdk_cairo_set_source_window wx_gdk_cairo_set_source_window
#endif

// ----------------------------------------------------------------------------
// the following were introduced in Glib 2.32

#ifndef g_signal_handlers_disconnect_by_data
#define g_signal_handlers_disconnect_by_data(instance, data) \
      g_signal_handlers_disconnect_matched ((instance), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, (data))
#endif

// ----------------------------------------------------------------------------
// the following were introduced in GTK+ 3.0

static inline void wx_gdk_window_get_geometry(GdkWindow* window, gint* x, gint* y, gint* width, gint* height)
{
    gdk_window_get_geometry(window, x, y, width, height, NULL);
}
#define gdk_window_get_geometry wx_gdk_window_get_geometry

static inline GtkWidget* wx_gtk_tree_view_column_get_button(GtkTreeViewColumn* tree_column)
{
    return tree_column->button;
}
#define gtk_tree_view_column_get_button wx_gtk_tree_view_column_get_button

static inline GtkWidget* wx_gtk_box_new(GtkOrientation orientation, gint spacing)
{
    GtkWidget* widget;
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        widget = gtk_hbox_new(false, spacing);
    else
        widget = gtk_vbox_new(false, spacing);
    return widget;
}
#define gtk_box_new wx_gtk_box_new

static inline GtkWidget* wx_gtk_button_box_new(GtkOrientation orientation)
{
    GtkWidget* widget;
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        widget = gtk_hbutton_box_new();
    else
        widget = gtk_vbutton_box_new();
    return widget;
}
#define gtk_button_box_new wx_gtk_button_box_new

static inline GtkWidget* wx_gtk_scale_new(GtkOrientation orientation, GtkAdjustment* adjustment)
{
    GtkWidget* widget;
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        widget = gtk_hscale_new(adjustment);
    else
        widget = gtk_vscale_new(adjustment);
    return widget;
}
#define gtk_scale_new wx_gtk_scale_new

static inline GtkWidget* wx_gtk_scrollbar_new(GtkOrientation orientation, GtkAdjustment* adjustment)
{
    GtkWidget* widget;
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        widget = gtk_hscrollbar_new(adjustment);
    else
        widget = gtk_vscrollbar_new(adjustment);
    return widget;
}
#define gtk_scrollbar_new wx_gtk_scrollbar_new

static inline GtkWidget* wx_gtk_separator_new(GtkOrientation orientation)
{
    GtkWidget* widget;
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        widget = gtk_hseparator_new();
    else
        widget = gtk_vseparator_new();
    return widget;
}
#define gtk_separator_new wx_gtk_separator_new

static inline void wx_gtk_widget_get_preferred_height(GtkWidget* widget, gint* minimum, gint* natural)
{
    GtkRequisition req;
    gtk_widget_size_request(widget, &req);
    if (minimum)
        *minimum = req.height;
    if (natural)
        *natural = req.height;
}
#define gtk_widget_get_preferred_height wx_gtk_widget_get_preferred_height

static inline void wx_gtk_widget_get_preferred_width(GtkWidget* widget, gint* minimum, gint* natural)
{
    GtkRequisition req;
    gtk_widget_size_request(widget, &req);
    if (minimum)
        *minimum = req.width;
    if (natural)
        *natural = req.width;
}
#define gtk_widget_get_preferred_width wx_gtk_widget_get_preferred_width

static inline void wx_gtk_widget_get_preferred_size(GtkWidget* widget, GtkRequisition* minimum, GtkRequisition* natural)
{
    GtkRequisition* req = minimum;
    if (req == NULL)
        req = natural;
    gtk_widget_size_request(widget, req);
}
#define gtk_widget_get_preferred_size wx_gtk_widget_get_preferred_size

#include <gdk/gdkkeysyms.h>

#if defined(GDK_Alt_L) && !defined(GDK_KEY_Alt_L)
#define GDK_KEY_Alt_L GDK_Alt_L
#define GDK_KEY_Alt_R GDK_Alt_R
#define GDK_KEY_AudioLowerVolume GDK_AudioLowerVolume
#define GDK_KEY_AudioMute GDK_AudioMute
#define GDK_KEY_AudioNext GDK_AudioNext
#define GDK_KEY_AudioPlay GDK_AudioPlay
#define GDK_KEY_AudioPrev GDK_AudioPrev
#define GDK_KEY_AudioRaiseVolume GDK_AudioRaiseVolume
#define GDK_KEY_AudioStop GDK_AudioStop
#define GDK_KEY_Back GDK_Back
#define GDK_KEY_BackSpace GDK_BackSpace
#define GDK_KEY_Begin GDK_Begin
#define GDK_KEY_Caps_Lock GDK_Caps_Lock
#define GDK_KEY_Clear GDK_Clear
#define GDK_KEY_Control_L GDK_Control_L
#define GDK_KEY_Control_R GDK_Control_R
#define GDK_KEY_Delete GDK_Delete
#define GDK_KEY_Down GDK_Down
#define GDK_KEY_End GDK_End
#define GDK_KEY_Escape GDK_Escape
#define GDK_KEY_Execute GDK_Execute
#define GDK_KEY_F10 GDK_F10
#define GDK_KEY_F11 GDK_F11
#define GDK_KEY_F12 GDK_F12
#define GDK_KEY_F1 GDK_F1
#define GDK_KEY_F2 GDK_F2
#define GDK_KEY_F3 GDK_F3
#define GDK_KEY_F4 GDK_F4
#define GDK_KEY_F5 GDK_F5
#define GDK_KEY_F6 GDK_F6
#define GDK_KEY_F7 GDK_F7
#define GDK_KEY_F8 GDK_F8
#define GDK_KEY_F9 GDK_F9
#define GDK_KEY_Favorites GDK_Favorites
#define GDK_KEY_Forward GDK_Forward
#define GDK_KEY_Help GDK_Help
#define GDK_KEY_Home GDK_Home
#define GDK_KEY_HomePage GDK_HomePage
#define GDK_KEY_Insert GDK_Insert
#define GDK_KEY_ISO_Enter GDK_ISO_Enter
#define GDK_KEY_ISO_Left_Tab GDK_ISO_Left_Tab
#define GDK_KEY_KP_0 GDK_KP_0
#define GDK_KEY_KP_1 GDK_KP_1
#define GDK_KEY_KP_2 GDK_KP_2
#define GDK_KEY_KP_3 GDK_KP_3
#define GDK_KEY_KP_4 GDK_KP_4
#define GDK_KEY_KP_5 GDK_KP_5
#define GDK_KEY_KP_6 GDK_KP_6
#define GDK_KEY_KP_7 GDK_KP_7
#define GDK_KEY_KP_8 GDK_KP_8
#define GDK_KEY_KP_9 GDK_KP_9
#define GDK_KEY_KP_Add GDK_KP_Add
#define GDK_KEY_KP_Begin GDK_KP_Begin
#define GDK_KEY_KP_Decimal GDK_KP_Decimal
#define GDK_KEY_KP_Delete GDK_KP_Delete
#define GDK_KEY_KP_Divide GDK_KP_Divide
#define GDK_KEY_KP_Down GDK_KP_Down
#define GDK_KEY_KP_End GDK_KP_End
#define GDK_KEY_KP_Enter GDK_KP_Enter
#define GDK_KEY_KP_Equal GDK_KP_Equal
#define GDK_KEY_KP_F1 GDK_KP_F1
#define GDK_KEY_KP_F2 GDK_KP_F2
#define GDK_KEY_KP_F3 GDK_KP_F3
#define GDK_KEY_KP_F4 GDK_KP_F4
#define GDK_KEY_KP_Home GDK_KP_Home
#define GDK_KEY_KP_Insert GDK_KP_Insert
#define GDK_KEY_KP_Left GDK_KP_Left
#define GDK_KEY_KP_Multiply GDK_KP_Multiply
#define GDK_KEY_KP_Next GDK_KP_Next
#define GDK_KEY_KP_Prior GDK_KP_Prior
#define GDK_KEY_KP_Right GDK_KP_Right
#define GDK_KEY_KP_Separator GDK_KP_Separator
#define GDK_KEY_KP_Space GDK_KP_Space
#define GDK_KEY_KP_Subtract GDK_KP_Subtract
#define GDK_KEY_KP_Tab GDK_KP_Tab
#define GDK_KEY_KP_Up GDK_KP_Up
#define GDK_KEY_LaunchA GDK_LaunchA
#define GDK_KEY_LaunchB GDK_LaunchB
#define GDK_KEY_Left GDK_Left
#define GDK_KEY_Linefeed GDK_Linefeed
#define GDK_KEY_Mail GDK_Mail
#define GDK_KEY_Menu GDK_Menu
#define GDK_KEY_Meta_L GDK_Meta_L
#define GDK_KEY_Meta_R GDK_Meta_R
#define GDK_KEY_Next GDK_Next
#define GDK_KEY_Num_Lock GDK_Num_Lock
#define GDK_KEY_Page_Down GDK_Page_Down
#define GDK_KEY_Page_Up GDK_Page_Up
#define GDK_KEY_Pause GDK_Pause
#define GDK_KEY_Print GDK_Print
#define GDK_KEY_Prior GDK_Prior
#define GDK_KEY_Refresh GDK_Refresh
#define GDK_KEY_Return GDK_Return
#define GDK_KEY_Right GDK_Right
#define GDK_KEY_Scroll_Lock GDK_Scroll_Lock
#define GDK_KEY_Search GDK_Search
#define GDK_KEY_Select GDK_Select
#define GDK_KEY_Shift_L GDK_Shift_L
#define GDK_KEY_Shift_R GDK_Shift_R
#define GDK_KEY_Stop GDK_Stop
#define GDK_KEY_Super_L GDK_Super_L
#define GDK_KEY_Super_R GDK_Super_R
#define GDK_KEY_Tab GDK_Tab
#define GDK_KEY_Up GDK_Up
#endif

// There is no equivalent in GTK+ 2, but it's not needed there anyhow as the
// backend is determined at compile time in that version.
#define GDK_IS_X11_DISPLAY(dpy) true

// Do perform runtime checks for GTK+ 2 version: we only take the minor version
// component here, major must be 2 and we never need to test for the micro one
// anyhow.
inline bool wx_is_at_least_gtk2(int minor)
{
    return gtk_check_version(2, minor, 0) == NULL;
}

#else // __WXGTK3__

// With GTK+ 3 we don't need to check for GTK+ 2 version and
// gtk_check_version() would fail due to major version mismatch.
inline bool wx_is_at_least_gtk2(int WXUNUSED(minor))
{
    return true;
}

#endif // !__WXGTK3__/__WXGTK3__

#endif // _WX_GTK_PRIVATE_COMPAT_H_
