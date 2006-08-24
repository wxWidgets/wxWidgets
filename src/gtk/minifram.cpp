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
#include "wx/settings.h"

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
static void gtk_window_own_expose_callback( GtkWidget *widget, GdkEventExpose *gdk_event, wxMiniFrame *win )
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

    int style = win->GetWindowStyle();
    
    if (!win->GetTitle().empty() &&
        ((style & wxCAPTION) ||
         (style & wxTINY_CAPTION_HORIZ) ||
         (style & wxTINY_CAPTION_VERT)))
    {
        wxClientDC dc(win);
        // Hack alert
        dc.m_window = pizza->bin_window;
        
        dc.SetFont( *wxSMALL_FONT );
        int height = dc.GetCharHeight();

        wxBrush brush( LightContrastColour( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT) ) );
        dc.SetBrush( brush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle( 3, 3, win->m_width - 7, height-2 );

        dc.SetTextForeground( *wxWHITE );
        dc.DrawText( win->GetTitle(), 6, 3 );

        if (style & wxCLOSE_BOX)
            dc.DrawBitmap( win->m_closeButton, win->m_width-19, 3, true );
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

    int y = (int)gdk_event->y;
    int x = (int)gdk_event->x;

    int style = win->GetWindowStyle();
    if ((style & wxCLOSE_BOX) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        if ((y > 3) && (y < 19) && (x > win->m_width-19) && (x < win->m_width-3))
        {
            win->Close();
            return TRUE;
        }
    }
        
    wxClientDC dc(win);
    dc.SetFont( *wxSMALL_FONT );
    int height = dc.GetCharHeight() + 1;


    if (y > height) return TRUE;

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
    style = style | wxCAPTION;

    if ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT))
        m_miniTitle = 16;

    if (style & wxRESIZE_BORDER)
        m_miniEdge = 5;
    else
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

    if ((style & wxCLOSE_BOX) &&
        ((style & wxCAPTION) || (style & wxTINY_CAPTION_HORIZ) || (style & wxTINY_CAPTION_VERT)))
    {
        wxImage img = wxBitmap((const char*)close_bits, 16, 16).ConvertToImage();
        img.Replace(0,0,0,123,123,123);
        img.SetMaskColour(123,123,123);
        m_closeButton = wxBitmap( img );
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
