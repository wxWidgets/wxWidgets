/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/minifram.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MINIFRAME

#include "wx/minifram.h"

#ifndef WX_PRECOMP
    #include "wx/dcscreen.h"
#endif

#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"
#include "wx/gtk/private.h"

#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool        g_blockEventsOnDrag;
extern bool        g_blockEventsOnScroll;
extern GtkWidget  *wxGetRootWindow();

//-----------------------------------------------------------------------------
// "expose_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_window_own_expose_callback( GtkWidget *widget, GdkEventExpose *gdk_event, wxFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (gdk_event->count > 0) return;

    GtkPizza *pizza = GTK_PIZZA(widget);

    gtk_paint_shadow (widget->style,
                      pizza->bin_window,
                      GTK_STATE_NORMAL,
                      GTK_SHADOW_OUT,
                      NULL, NULL, NULL, // FIXME: No clipping?
                      0, 0,
                      win->m_width, win->m_height);

    if (!win->GetTitle().empty() &&
        ((win->GetWindowStyle() & wxCAPTION) ||
         (win->GetWindowStyle() & wxTINY_CAPTION_HORIZ) ||
         (win->GetWindowStyle() & wxTINY_CAPTION_VERT)))
    {
        wxClientDC dc(win);
        dc.SetFont( *wxSMALL_FONT );
        int height = dc.GetCharHeight();

        GdkGC *gc = gdk_gc_new( pizza->bin_window );
        gdk_gc_set_foreground( gc, &widget->style->bg[GTK_STATE_SELECTED] );
        gdk_draw_rectangle( pizza->bin_window, gc, TRUE,
                            3,
                            3,
                            win->m_width - 7,
                            height+1 );
        g_object_unref (gc);

        // Hack alert
        dc.m_window = pizza->bin_window;
        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( win->GetTitle(), 6, 3 );
    }
}
}

//-----------------------------------------------------------------------------
// "button_press_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_window_button_press_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxMiniFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (win->m_isDragging) return TRUE;

    GtkPizza *pizza = GTK_PIZZA(widget);
    if (gdk_event->window != pizza->bin_window) return TRUE;

    wxClientDC dc(win);
    dc.SetFont( *wxSMALL_FONT );
    int height = dc.GetCharHeight() + 1;

    if (gdk_event->y > height) return TRUE;

    gdk_window_raise( win->m_widget->window );

    gdk_pointer_grab( widget->window, FALSE,
                      (GdkEventMask)
                         (GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_POINTER_MOTION_MASK        |
                          GDK_POINTER_MOTION_HINT_MASK  |
                          GDK_BUTTON_MOTION_MASK        |
                          GDK_BUTTON1_MOTION_MASK),
                      (GdkWindow *) NULL,
                      (GdkCursor *) NULL,
                      (unsigned int) GDK_CURRENT_TIME );

    win->m_diffX = (int)gdk_event->x;
    win->m_diffY = (int)gdk_event->y;
    win->m_oldX = 0;
    win->m_oldY = 0;

    win->m_isDragging = true;

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "button_release_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_window_button_release_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxMiniFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (!win->m_isDragging) return TRUE;

    win->m_isDragging = false;

    int x = (int)gdk_event->x;
    int y = (int)gdk_event->y;

    gdk_pointer_ungrab ( (guint32)GDK_CURRENT_TIME );
    int org_x = 0;
    int org_y = 0;
    gdk_window_get_origin( widget->window, &org_x, &org_y );
    x += org_x - win->m_diffX;
    y += org_y - win->m_diffY;
    win->m_x = x;
    win->m_y = y;
    gtk_window_move( GTK_WINDOW(win->m_widget), x, y );

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "motion_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_window_motion_notify_callback( GtkWidget *widget, GdkEventMotion *gdk_event, wxMiniFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (!win->m_isDragging) return TRUE;

    if (gdk_event->is_hint)
    {
       int x = 0;
       int y = 0;
       GdkModifierType state;
       gdk_window_get_pointer(gdk_event->window, &x, &y, &state);
       gdk_event->x = x;
       gdk_event->y = y;
       gdk_event->state = state;
    }

    win->m_oldX = (int)gdk_event->x - win->m_diffX;
    win->m_oldY = (int)gdk_event->y - win->m_diffY;

    int x = (int)gdk_event->x;
    int y = (int)gdk_event->y;
    
    int org_x = 0;
    int org_y = 0;
    gdk_window_get_origin( widget->window, &org_x, &org_y );
    x += org_x - win->m_diffX;
    y += org_y - win->m_diffY;
    win->m_x = x;
    win->m_y = y;
    gtk_window_move( GTK_WINDOW(win->m_widget), x, y );


    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "clicked" of X system button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxMiniFrame *mf )
{
    if (g_isIdle) wxapp_install_idle_handler();

    mf->Close();
}
}

//-----------------------------------------------------------------------------
// wxMiniFrame
//-----------------------------------------------------------------------------

static const char *cross_xpm[] = {
/* columns rows colors chars-per-pixel */
"5 5 2 1",
"# c Gray0",
"  c None",
/* pixels */
"#   #",
" # # ",
"  #  ",
" # # ",
"#   #",
};

IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame,wxFrame)

bool wxMiniFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    style = style | wxCAPTION;

    if ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT))
        m_miniTitle = 13;

    m_miniEdge = 3;
    m_isDragging = false;
    m_oldX = -1;
    m_oldY = -1;
    m_diffX = 0;
    m_diffY = 0;

    wxFrame::Create( parent, id, title, pos, size, style, name );

    if (m_parent && (GTK_IS_WINDOW(m_parent->m_widget)))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(m_parent->m_widget) );
    }

    if ((style & wxSYSTEM_MENU) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        GdkBitmap *mask = (GdkBitmap*) NULL;
        GdkPixmap *pixmap = gdk_pixmap_create_from_xpm_d
                            (
                                wxGetRootWindow()->window,
                                &mask,
                                NULL,
                                (char **)cross_xpm
                            );

        GtkWidget *pw = gtk_pixmap_new( pixmap, mask );
        g_object_unref (mask);
        g_object_unref (pixmap);
        gtk_widget_show( pw );

        GtkWidget *close_button = gtk_button_new();
#ifdef __WXGTK24__
        if (!gtk_check_version(2,4,0))
            gtk_button_set_focus_on_click( GTK_BUTTON(close_button), FALSE );
#endif
        gtk_container_add( GTK_CONTAINER(close_button), pw );

        gtk_pizza_put( GTK_PIZZA(m_mainWidget),
                         close_button,
                         size.x-16, 4, 11, 11 );

        gtk_widget_show( close_button );

        g_signal_connect (close_button, "clicked",
                          G_CALLBACK (gtk_button_clicked_callback),
                          this);
    }

    /* these are called when the borders are drawn */
    g_signal_connect (m_mainWidget, "expose_event",
                      G_CALLBACK (gtk_window_own_expose_callback), this );

    /* these are required for dragging the mini frame around */
    g_signal_connect (m_mainWidget, "button_press_event",
                      G_CALLBACK (gtk_window_button_press_callback), this);
    g_signal_connect (m_mainWidget, "button_release_event",
                      G_CALLBACK (gtk_window_button_release_callback), this);
    g_signal_connect (m_mainWidget, "motion_notify_event",
                      G_CALLBACK (gtk_window_motion_notify_callback), this);

    return true;
}

void wxMiniFrame::SetTitle( const wxString &title )
{
    wxFrame::SetTitle( title );

    gdk_window_invalidate_rect( GTK_PIZZA(m_mainWidget)->bin_window, NULL, true );
}

#endif // wxUSE_MINIFRAME
