/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minifram.h"
#endif

#include "wx/minifram.h"

#if wxUSE_MINIFRAME

#include "wx/dcscreen.h"

#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"

#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;
extern bool   g_blockEventsOnScroll;

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

/* draw XOR rectangle when moving mine frame around */

static void DrawFrame( GtkWidget *widget, int x, int y, int w, int h )
{
    int org_x = 0;    
    int org_y = 0;
    gdk_window_get_origin( widget->window, &org_x, &org_y );
    x += org_x;
    y += org_y;

    GdkGC *gc = gdk_gc_new( GDK_ROOT_PARENT() );
    gdk_gc_set_subwindow( gc, GDK_INCLUDE_INFERIORS );
    gdk_gc_set_function( gc, GDK_INVERT );
    
    gdk_draw_rectangle( GDK_ROOT_PARENT(), gc, FALSE, x, y, w, h );
    gdk_gc_unref( gc );
}

//-----------------------------------------------------------------------------
// "expose_event" of m_mainWidget
//-----------------------------------------------------------------------------

static void gtk_window_own_expose_callback( GtkWidget *widget, GdkEventExpose *gdk_event, wxFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    if (gdk_event->count > 0) return;
    
    GtkPizza *pizza = GTK_PIZZA(widget);
    
    gtk_draw_shadow( widget->style, 
                     pizza->bin_window,
                     GTK_STATE_NORMAL,
                     GTK_SHADOW_OUT,
                     0, 0,
                     win->m_width, win->m_height );

    if (!win->m_title.IsEmpty() &&
        ((win->GetWindowStyle() & wxCAPTION) || 
         (win->GetWindowStyle() & wxTINY_CAPTION_HORIZ) || 
         (win->GetWindowStyle() & wxTINY_CAPTION_VERT)))
    {
        GdkGC *gc = gdk_gc_new( pizza->bin_window );
        GdkFont *font = wxSMALL_FONT->GetInternalFont(1.0);
        int x = 2;
        if (win->GetWindowStyle() & wxSYSTEM_MENU) x = 18;
        
        gdk_gc_set_foreground( gc, &widget->style->bg[GTK_STATE_SELECTED] );
        gdk_draw_rectangle( pizza->bin_window, gc, TRUE, 
                            x, 
                            3, 
                            win->m_width - 4 - x,
                            font->ascent + font->descent+1 );
                            
        gdk_gc_set_foreground( gc, &widget->style->white );
        gdk_draw_string( pizza->bin_window, font, gc, 
                         x+2, 
                         3+font->ascent, 
                         win->m_title.mb_str() );
        
        gdk_gc_unref( gc );
    }
}

//-----------------------------------------------------------------------------
// "draw" of m_mainWidget
//-----------------------------------------------------------------------------

static void gtk_window_own_draw_callback( GtkWidget *widget, GdkRectangle *WXUNUSED(rect), wxFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return;
    
    GtkPizza *pizza = GTK_PIZZA(widget);
    
    gtk_draw_shadow( widget->style, 
                     pizza->bin_window,
                     GTK_STATE_NORMAL,
                     GTK_SHADOW_OUT,
                     0, 0,
                     win->m_width, win->m_height );
                     
    if (!win->m_title.IsEmpty() &&
        ((win->GetWindowStyle() & wxCAPTION) || 
         (win->GetWindowStyle() & wxTINY_CAPTION_HORIZ) || 
         (win->GetWindowStyle() & wxTINY_CAPTION_VERT)))
    {
        GdkGC *gc = gdk_gc_new( pizza->bin_window );
        GdkFont *font = wxSMALL_FONT->GetInternalFont(1.0);
        int x = 2;
        if (win->GetWindowStyle() & wxSYSTEM_MENU) x = 17;
        
        gdk_gc_set_foreground( gc, &widget->style->bg[GTK_STATE_SELECTED] );
        gdk_draw_rectangle( pizza->bin_window, gc, TRUE, 
                            x, 
                            3, 
                            win->m_width - 4 - x,
                            font->ascent + font->descent+1 );
                            
        gdk_gc_set_foreground( gc, &widget->style->white );
        gdk_draw_string( pizza->bin_window, font, gc, 
                         x+2, 
                         3+font->ascent, 
                         win->m_title.mb_str() );
        
        gdk_gc_unref( gc );
    }
}

//-----------------------------------------------------------------------------
// "button_press_event" of m_mainWidget
//-----------------------------------------------------------------------------

static gint gtk_window_button_press_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxMiniFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (win->m_isDragging) return TRUE;

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
    DrawFrame( widget, 0, 0, win->m_width, win->m_height );
    win->m_oldX = 0;
    win->m_oldY = 0;
    
    win->m_isDragging = TRUE;

    return TRUE;
}

//-----------------------------------------------------------------------------
// "button_release_event" of m_mainWidget
//-----------------------------------------------------------------------------

static gint gtk_window_button_release_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxMiniFrame *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (!win->m_isDragging) return TRUE;
    
    win->m_isDragging = FALSE;
     
    int x = (int)gdk_event->x;
    int y = (int)gdk_event->y;
    
    DrawFrame( widget, win->m_oldX, win->m_oldY, win->m_width, win->m_height );
    gdk_pointer_ungrab ( GDK_CURRENT_TIME );
    int org_x = 0;    
    int org_y = 0;
    gdk_window_get_origin( widget->window, &org_x, &org_y );
    x += org_x - win->m_diffX;
    y += org_y - win->m_diffY;
    win->m_x = x;
    win->m_y = y;
    gtk_widget_set_uposition( win->m_widget, x, y );

    return TRUE;
}

//-----------------------------------------------------------------------------
// "motion_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

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

    DrawFrame( widget, win->m_oldX, win->m_oldY, win->m_width, win->m_height );
    win->m_oldX = (int)gdk_event->x - win->m_diffX;
    win->m_oldY = (int)gdk_event->y - win->m_diffY;
    DrawFrame( widget, win->m_oldX, win->m_oldY, win->m_width, win->m_height );
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// "clicked" of X system button
//-----------------------------------------------------------------------------

static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxMiniFrame *mf )
{
    if (g_isIdle) wxapp_install_idle_handler();

    mf->Close();
}

//-----------------------------------------------------------------------------
// wxMiniFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame,wxFrame)

bool wxMiniFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    style = style | wxSIMPLE_BORDER;
    style = style | wxCAPTION;

    if ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT))
        m_miniTitle = 13;
        
    m_miniEdge = 3;
    m_isDragging = FALSE;
    m_oldX = -1;
    m_oldY = -1;
    m_diffX = 0;
    m_diffY = 0;
    
    wxFrame::Create( parent, id, title, pos, size, style, name );

    if ((style & wxSYSTEM_MENU) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        GtkWidget *close_button = gtk_button_new_with_label( "x" );
    
        gtk_pizza_put( GTK_PIZZA(m_mainWidget), 
                         close_button, 
                         4, 4, 12, 11 );
    
        gtk_widget_show( close_button );
    
        gtk_signal_connect( GTK_OBJECT(close_button), "clicked",
          GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );
    }
    
    /* these are called when the borders are drawn */
    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "expose_event",
        GTK_SIGNAL_FUNC(gtk_window_own_expose_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "draw",
       GTK_SIGNAL_FUNC(gtk_window_own_draw_callback), (gpointer)this );
       
    /* these are required for dragging the mini frame around */
    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "button_press_event",
      GTK_SIGNAL_FUNC(gtk_window_button_press_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "button_release_event",
      GTK_SIGNAL_FUNC(gtk_window_button_release_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_mainWidget), "motion_notify_event",
      GTK_SIGNAL_FUNC(gtk_window_motion_notify_callback), (gpointer)this );

    return TRUE;
}

#endif
