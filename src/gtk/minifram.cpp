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
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/image.h"
#endif

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool        g_blockEventsOnDrag;
extern bool        g_blockEventsOnScroll;

//-----------------------------------------------------------------------------
// "expose_event" of m_mainWidget
//-----------------------------------------------------------------------------

// StepColour() it a utility function that simply darkens
// or lightens a color, based on the specified percentage
static wxColor StepColour(const wxColor& c, int percent)
{
    int r = c.Red(), g = c.Green(), b = c.Blue();
    return wxColour((unsigned char)wxMin((r*percent)/100,255),
                    (unsigned char)wxMin((g*percent)/100,255),
                    (unsigned char)wxMin((b*percent)/100,255));
}

static wxColor LightContrastColour(const wxColour& c)
{
    int amount = 120;

    // if the color is especially dark, then
    // make the contrast even lighter
    if (c.Red() < 128 && c.Green() < 128 && c.Blue() < 128)
        amount = 160;

    return StepColour(c, amount);
}

extern "C" {
static gboolean gtk_window_own_expose_callback(GtkWidget* widget, GdkEventExpose* gdk_event, wxMiniFrame* win)
{
    if (!win->m_hasVMT || gdk_event->count > 0)
        return false;

    gtk_paint_shadow (widget->style,
                      widget->window,
                      GTK_STATE_NORMAL,
                      GTK_SHADOW_OUT,
                      NULL, NULL, NULL, // FIXME: No clipping?
                      0, 0,
                      win->m_width, win->m_height);

    int style = win->GetWindowStyle();

    wxClientDC dc(win);

#if wxUSE_NEW_DC
    wxImplDC *impl = dc.GetImpl();
    wxGTKClientImplDC *client_impl = wxDynamicCast( impl, wxGTKClientImplDC );
    // Hack alert
    client_impl->m_window = widget->window;
#else
    // Hack alert
    dc.m_window = widget->window;
#endif

    if (style & wxRESIZE_BORDER)
    {
        dc.SetBrush( *wxGREY_BRUSH );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle( win->m_width - 14, win->m_height-14, 14, 14 );
    }

    if (!win->GetTitle().empty() &&
        ((style & wxCAPTION) ||
         (style & wxTINY_CAPTION_HORIZ) ||
         (style & wxTINY_CAPTION_VERT)))
    {
        dc.SetFont( *wxSMALL_FONT );

        wxBrush brush( LightContrastColour( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT) ) );
        dc.SetBrush( brush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle( win->m_miniEdge-1, 
                          win->m_miniEdge-1, 
                          win->m_width - (2*(win->m_miniEdge-1)),
                          15  );

        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( win->GetTitle(), 6, 4 );

        if (style & wxCLOSE_BOX)
            dc.DrawBitmap( win->m_closeButton, win->m_width-18, 3, true );
    }
    
    return false;
}
}

//-----------------------------------------------------------------------------
// "button_press_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gint gtk_window_button_press_callback( GtkWidget *widget, GdkEventButton *gdk_event, wxMiniFrame *win )
{
    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

    if (win->m_isDragging) return TRUE;

    int style = win->GetWindowStyle();

    int y = (int)gdk_event->y;
    int x = (int)gdk_event->x;

    if ((style & wxRESIZE_BORDER) &&
        (x > win->m_width-14) && (y > win->m_height-14))
    {
        GtkWidget *ancestor = gtk_widget_get_toplevel( widget );

        GdkWindow *source = widget->window;

        int org_x = 0;
        int org_y = 0;
        gdk_window_get_origin( source, &org_x, &org_y );

        gtk_window_begin_resize_drag (GTK_WINDOW (ancestor),
                                  GDK_WINDOW_EDGE_SOUTH_EAST,
                                  1,
                                  org_x + x,
                                  org_y + y,
                                  0);

        return TRUE;
    }

    if ((style & wxCLOSE_BOX) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        if ((y > 3) && (y < 19) && (x > win->m_width-19) && (x < win->m_width-3))
        {
            win->Close();
            return TRUE;
        }
    }

    if (y > win->m_miniEdge-1 + 15) return TRUE;

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

    win->m_diffX = x;
    win->m_diffY = y;
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
// "leave_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_leave_callback(GtkWidget *widget,
                          GdkEventCrossing * WXUNUSED(gdk_event),
                          wxMiniFrame *win)
{
    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

    gdk_window_set_cursor( widget->window, NULL );

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "motion_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_window_motion_notify_callback( GtkWidget *widget, GdkEventMotion *gdk_event, wxMiniFrame *win )
{
    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

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

    int style = win->GetWindowStyle();

    int x = (int)gdk_event->x;
    int y = (int)gdk_event->y;

    if (!win->m_isDragging)
    {
        if (style & wxRESIZE_BORDER)
        {
            if ((x > win->m_width-14) && (y > win->m_height-14))
               gdk_window_set_cursor( widget->window, gdk_cursor_new( GDK_BOTTOM_RIGHT_CORNER ) );
            else
               gdk_window_set_cursor( widget->window, NULL );
        }
        return TRUE;
    }

    win->m_oldX = x - win->m_diffX;
    win->m_oldY = y - win->m_diffY;

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
// "size_allocate" from GtkFixed, parent of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static void size_allocate(GtkWidget*, GtkAllocation* alloc, wxMiniFrame* win)
{
    // place m_mainWidget inside of decorations drawn on the GtkFixed
    GtkAllocation alloc2 = win->m_mainWidget->allocation;
    alloc2.width = alloc->width - 2 * win->m_miniEdge;
    alloc2.height = alloc->height - win->m_miniTitle - 2 * win->m_miniEdge;
    gtk_widget_size_allocate(win->m_mainWidget, &alloc2);
}
}

//-----------------------------------------------------------------------------
// wxMiniFrame
//-----------------------------------------------------------------------------

static unsigned char close_bits[]={
    0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xfb, 0xef, 0xdb, 0xed, 0x8b, 0xe8,
    0x1b, 0xec, 0x3b, 0xee, 0x1b, 0xec, 0x8b, 0xe8, 0xdb, 0xed, 0xfb, 0xef,
    0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame,wxFrame)

bool wxMiniFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    if ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT))
        m_miniTitle = 16;

    if (style & wxRESIZE_BORDER)
        m_miniEdge = 4;
    else
        m_miniEdge = 3;
    m_isDragging = false;
    m_oldX = -1;
    m_oldY = -1;
    m_diffX = 0;
    m_diffY = 0;

    wxFrame::Create( parent, id, title, pos, size, style, name );

    // borders and title are on a GtkFixed between m_widget and m_mainWidget
    GtkWidget* fixed = gtk_fixed_new();
    gtk_fixed_set_has_window((GtkFixed*)fixed, true);
    gtk_widget_add_events(fixed,
        GDK_POINTER_MOTION_MASK |
        GDK_POINTER_MOTION_HINT_MASK |
        GDK_BUTTON_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_LEAVE_NOTIFY_MASK);
    gtk_widget_show(fixed);
    gtk_widget_reparent(m_mainWidget, fixed);
    gtk_container_add((GtkContainer*)m_widget, fixed);
    gtk_fixed_move((GtkFixed*)fixed, m_mainWidget, m_miniEdge, m_miniTitle + m_miniEdge);
    g_signal_connect(fixed, "size_allocate", G_CALLBACK(size_allocate), this);

    m_gdkDecor = 0;
    m_gdkFunc = 0;
    if (style & wxRESIZE_BORDER)
       m_gdkFunc = GDK_FUNC_RESIZE;

    // need to reset default size after changing m_gdkDecor
    gtk_window_set_default_size(GTK_WINDOW(m_widget), m_width, m_height);

    if (m_parent && (GTK_IS_WINDOW(m_parent->m_widget)))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(m_parent->m_widget) );
    }

    if ((style & wxCLOSE_BOX) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        wxImage img = wxBitmap((const char*)close_bits, 16, 16).ConvertToImage();
        img.Replace(0,0,0,123,123,123);
        img.SetMaskColour(123,123,123);
        m_closeButton = wxBitmap( img );
    }

    /* these are called when the borders are drawn */
    g_signal_connect_after(fixed, "expose_event",
                      G_CALLBACK (gtk_window_own_expose_callback), this );

    /* these are required for dragging the mini frame around */
    g_signal_connect (fixed, "button_press_event",
                      G_CALLBACK (gtk_window_button_press_callback), this);
    g_signal_connect (fixed, "button_release_event",
                      G_CALLBACK (gtk_window_button_release_callback), this);
    g_signal_connect (fixed, "motion_notify_event",
                      G_CALLBACK (gtk_window_motion_notify_callback), this);
    g_signal_connect (fixed, "leave_notify_event",
                      G_CALLBACK (gtk_window_leave_callback), this);
    return true;
}

void wxMiniFrame::DoGetClientSize(int* width, int* height) const
{
    wxFrame::DoGetClientSize(width, height);
    if (width)
    {
        *width -= 2 * m_miniEdge;
        if (*width < 0) *width = 0;
    }
    if (height)
    {
        *height -= m_miniTitle + 2 * m_miniEdge;
        if (*height < 0) *height = 0;
    }
}

void wxMiniFrame::SetTitle( const wxString &title )
{
    wxFrame::SetTitle( title );

    GtkWidget* fixed = GTK_BIN(m_widget)->child;
    if (fixed->window)
        gdk_window_invalidate_rect(fixed->window, NULL, false);
}

#endif // wxUSE_MINIFRAME
