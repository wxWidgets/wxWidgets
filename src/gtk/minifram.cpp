/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/minifram.cpp
// Purpose:
// Author:      Robert Roebling
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
#endif

#ifdef __WXGTK3__
#include "wx/gtk/dc.h"
#else
#include "wx/gtk/dcclient.h"
#endif

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/gtk3-compat.h"
#include "wx/gtk/private/backend.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool        g_blockEventsOnDrag;
extern bool        g_blockEventsOnScroll;

//-----------------------------------------------------------------------------
// "expose_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
#ifdef __WXGTK3__
static gboolean draw(GtkWidget* widget, cairo_t* cr, wxMiniFrame* win)
#else
static gboolean expose_event(GtkWidget* widget, GdkEventExpose* gdk_event, wxMiniFrame* win)
#endif
{
#ifdef __WXGTK3__
    if (!gtk_cairo_should_draw_window(cr, gtk_widget_get_window(widget)))
        return false;

    GtkStyleContext* sc = gtk_widget_get_style_context(widget);
    gtk_style_context_save(sc);
    gtk_style_context_add_class(sc, GTK_STYLE_CLASS_BUTTON);
    gtk_render_frame(sc, cr, 0, 0, win->m_width, win->m_height);
    gtk_style_context_restore(sc);

    wxGTKCairoDC dc(cr, win);
#else
    if (gdk_event->count > 0 ||
        gdk_event->window != gtk_widget_get_window(widget))
    {
        return false;
    }

    gtk_paint_shadow (gtk_widget_get_style(widget),
                      gtk_widget_get_window(widget),
                      GTK_STATE_NORMAL,
                      GTK_SHADOW_OUT,
                      nullptr, nullptr, nullptr, // FIXME: No clipping?
                      0, 0,
                      win->m_width, win->m_height);

    wxClientDC dc(win);

    wxDCImpl *impl = dc.GetImpl();
    wxClientDCImpl *gtk_impl = wxDynamicCast( impl, wxClientDCImpl );
    gtk_impl->m_gdkwindow = gtk_widget_get_window(widget); // Hack alert
#endif

    int style = win->GetWindowStyle();

    if (style & wxRESIZE_BORDER)
    {
        dc.SetBrush( *wxGREY_BRUSH );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(win->m_width - 14, win->m_height - win->m_miniEdge, 14, win->m_miniEdge);
        dc.DrawRectangle(win->m_width - win->m_miniEdge, win->m_height - 14, win->m_miniEdge, 14);
    }

    if (win->m_miniTitle && !win->GetTitle().empty())
    {
        dc.SetFont( *wxSMALL_FONT );
        int height = wxMax(dc.GetTextExtent("X").y, 16);

        wxBrush brush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        dc.SetBrush( brush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle( win->m_miniEdge-1,
                          win->m_miniEdge-1,
                          win->m_width - (2*(win->m_miniEdge-1)),
                          height );

        const wxColour textColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        dc.SetTextForeground(textColor);
        dc.DrawText( win->GetTitle(), 6, 2 );

        if (style & wxCLOSE_BOX)
        {
            dc.SetTextBackground(textColor);
            dc.DrawBitmap(
                    win->m_closeButton,
                    win->m_width-18,
                    win->m_miniEdge - 1 + (height - 16) / 2,
                    true );
        }
    }

    return false;
}
}

//-----------------------------------------------------------------------------
// "button_press_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_button_press_callback(GtkWidget* widget, GdkEventButton* gdk_event, wxMiniFrame* win)
{
    if (gdk_event->window != gtk_widget_get_window(widget))
        return false;
    if (win->m_isDragMove || g_blockEventsOnDrag || g_blockEventsOnScroll)
        return true;

    int style = win->GetWindowStyle();

    int y = (int)gdk_event->y;
    int x = (int)gdk_event->x;

    if ((style & wxRESIZE_BORDER) &&
        (x > win->m_width-14) && (y > win->m_height-14))
    {
        gtk_window_begin_resize_drag(GTK_WINDOW(win->m_widget),
            GDK_WINDOW_EDGE_SOUTH_EAST,
            gdk_event->button,
            int(gdk_event->x_root), int(gdk_event->y_root),
            gdk_event->time);

        return TRUE;
    }

    if (win->m_miniTitle && (style & wxCLOSE_BOX))
    {
        if ((y > 3) && (y < 19) && (x > win->m_width-19) && (x < win->m_width-3))
        {
            win->Close();
            return TRUE;
        }
    }

    if (y >= win->m_miniEdge + win->m_miniTitle)
        return true;

    gdk_window_raise(gtk_widget_get_window(win->m_widget));

#ifdef __WXGTK3__
#ifndef __WXGTK4__
    if (wxGTKImpl::IsWayland(gdk_event->window))
#endif
    {
        gtk_window_begin_move_drag(GTK_WINDOW(win->m_widget),
            gdk_event->button,
            int(gdk_event->x_root), int(gdk_event->y_root),
            gdk_event->time);

        return true;
    }
#endif
#ifndef __WXGTK4__
    const GdkEventMask mask = GdkEventMask(
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_POINTER_MOTION_MASK |
        GDK_BUTTON_MOTION_MASK);
#ifdef __WXGTK3__
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gdk_device_grab(
        gdk_event->device, gdk_event->window, GDK_OWNERSHIP_NONE,
        false, mask, nullptr, gdk_event->time);
    wxGCC_WARNING_RESTORE()
#else
    gdk_pointer_grab(gdk_event->window, false, mask, nullptr, nullptr, gdk_event->time);
#endif

    win->m_dragOffset.Set(int(gdk_event->x), int(gdk_event->y));
    win->m_isDragMove = true;

    return TRUE;
#endif // !__WXGTK4__
}
}

#ifndef __WXGTK4__
//-----------------------------------------------------------------------------
// "button-release-event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
button_release_event(GtkWidget* widget, GdkEventButton* gdk_event, wxMiniFrame* win)
{
    if (gdk_event->window != gtk_widget_get_window(widget))
        return false;
    if (!win->m_isDragMove || g_blockEventsOnDrag || g_blockEventsOnScroll)
        return true;

    win->m_isDragMove = false;

#ifdef __WXGTK3__
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gdk_device_ungrab(gdk_event->device, gdk_event->time);
    wxGCC_WARNING_RESTORE()
#else
    gdk_pointer_ungrab(gdk_event->time);
#endif

    return true;
}
}
#endif // !__WXGTK4__

//-----------------------------------------------------------------------------
// "leave_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_leave_callback(GtkWidget *widget,
                          GdkEventCrossing* gdk_event,
                          wxMiniFrame*)
{
    if (g_blockEventsOnDrag) return FALSE;
    if (gdk_event->window != gtk_widget_get_window(widget))
        return false;

    gdk_window_set_cursor(gtk_widget_get_window(widget), nullptr);

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "motion_notify_event" of m_mainWidget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_window_motion_notify_callback( GtkWidget *widget, GdkEventMotion *gdk_event, wxMiniFrame *win )
{
    if (gdk_event->window != gtk_widget_get_window(widget))
        return false;
    if (g_blockEventsOnDrag) return TRUE;
    if (g_blockEventsOnScroll) return TRUE;

#ifndef __WXGTK4__
    if (win->m_isDragMove)
    {
        gtk_window_move(GTK_WINDOW(win->m_widget),
            int(gdk_event->x_root) - win->m_dragOffset.x,
            int(gdk_event->y_root) - win->m_dragOffset.y);

        return true;
    }
#endif
    {
        if (win->GetWindowStyle() & wxRESIZE_BORDER)
        {
            const int x = int(gdk_event->x);
            const int y = int(gdk_event->y);

            GdkCursor* cursor = nullptr;
            GdkWindow* window = gtk_widget_get_window(widget);
            if ((x > win->m_width-14) && (y > win->m_height-14))
            {
                GdkDisplay* display = gdk_window_get_display(window);
                cursor = gdk_cursor_new_for_display(display, GDK_BOTTOM_RIGHT_CORNER);
            }
            gdk_window_set_cursor(window, cursor);
            if (cursor)
            {
#ifdef __WXGTK3__
                g_object_unref(cursor);
#else
                gdk_cursor_unref(cursor);
#endif
            }
        }
    }

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


wxIMPLEMENT_DYNAMIC_CLASS(wxMiniFrame, wxFrame);

wxMiniFrame::~wxMiniFrame()
{
    if (m_widget)
    {
        GtkWidget* eventbox = gtk_bin_get_child(GTK_BIN(m_widget));
        GTKDisconnect(eventbox);
    }
}

bool wxMiniFrame::Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    wxFrame::Create( parent, id, title, pos, size, style, name );

    m_isDragMove = false;
    m_miniTitle = 0;
    if (style & wxCAPTION)
    {
        wxClientDC dc(this);
        dc.SetFont(*wxSMALL_FONT);
        m_miniTitle = wxMax(dc.GetTextExtent("X").y, 16);
    }

    if (style & wxRESIZE_BORDER)
        m_miniEdge = 4;
    else
        m_miniEdge = 3;

    // don't allow sizing smaller than decorations
    int minWidth = 2 * m_miniEdge;
    int minHeight = 2 * m_miniEdge + m_miniTitle;
    if (m_minWidth < minWidth)
        m_minWidth = minWidth;
    if (m_minHeight < minHeight)
        m_minHeight = minHeight;

    // Use a GtkEventBox for the title and borders. Using m_widget for this
    // almost works, except that setting the resize cursor has no effect.
    GtkWidget* eventbox = gtk_event_box_new();
    gtk_widget_add_events(eventbox, GDK_POINTER_MOTION_MASK);
    gtk_widget_show(eventbox);
#ifdef __WXGTK3__
    g_object_ref(m_mainWidget);
    gtk_container_remove(GTK_CONTAINER(m_widget), m_mainWidget);
    gtk_container_add(GTK_CONTAINER(eventbox), m_mainWidget);
    g_object_unref(m_mainWidget);

    gtk_widget_set_margin_start(m_mainWidget, m_miniEdge);
    gtk_widget_set_margin_end(m_mainWidget, m_miniEdge);
    gtk_widget_set_margin_top(m_mainWidget, m_miniTitle + m_miniEdge);
    gtk_widget_set_margin_bottom(m_mainWidget, m_miniEdge);
#else
    // Use a GtkAlignment to position m_mainWidget inside the decorations
    GtkWidget* alignment = gtk_alignment_new(0, 0, 1, 1);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment),
        m_miniTitle + m_miniEdge, m_miniEdge, m_miniEdge, m_miniEdge);
    gtk_widget_show(alignment);
    // The GtkEventBox and GtkAlignment go between m_widget and m_mainWidget
    gtk_widget_reparent(m_mainWidget, alignment);
    gtk_container_add(GTK_CONTAINER(eventbox), alignment);
#endif
    gtk_container_add(GTK_CONTAINER(m_widget), eventbox);

    m_gdkDecor = 0;
    gtk_window_set_decorated(GTK_WINDOW(m_widget), false);
    m_gdkFunc = GDK_FUNC_MOVE;
    if (style & wxRESIZE_BORDER)
       m_gdkFunc |= GDK_FUNC_RESIZE;
    gtk_window_set_default_size(GTK_WINDOW(m_widget), m_width, m_height);

    // Reset m_decorSize, wxMiniFrame manages its own decorations
    m_decorSize = DecorSize();
    m_deferShow = false;

    if (m_parent && (GTK_IS_WINDOW(m_parent->m_widget)))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(m_parent->m_widget) );
    }

    if (m_miniTitle && (style & wxCLOSE_BOX))
    {
        m_closeButton = wxBitmap((const char*)close_bits, 16, 16);
        m_closeButton.SetMask(new wxMask(m_closeButton));
    }

    /* these are called when the borders are drawn */
#ifdef __WXGTK3__
    g_signal_connect_after(eventbox, "draw", G_CALLBACK(draw), this);
#else
    g_signal_connect_after(eventbox, "expose_event", G_CALLBACK(expose_event), this);
#endif

    /* these are required for dragging the mini frame around */
    g_signal_connect (eventbox, "button_press_event",
                      G_CALLBACK (gtk_window_button_press_callback), this);
#ifndef __WXGTK4__
    g_signal_connect(eventbox, "button-release-event",
        G_CALLBACK(button_release_event), this);
#endif
    g_signal_connect (eventbox, "motion_notify_event",
                      G_CALLBACK (gtk_window_motion_notify_callback), this);
    g_signal_connect (eventbox, "leave_notify_event",
                      G_CALLBACK (gtk_window_leave_callback), this);
    return true;
}

void wxMiniFrame::DoGetClientSize(int* width, int* height) const
{
    wxFrame::DoGetClientSize(width, height);

    if (m_useCachedClientSize)
        return;

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

// Keep min size at least as large as decorations
void wxMiniFrame::DoSetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
    const int w = 2 * m_miniEdge;
    const int h = 2 * m_miniEdge + m_miniTitle;
    if (minW < w) minW = w;
    if (minH < h) minH = h;
    wxFrame::DoSetSizeHints(minW, minH, maxW, maxH, incW, incH);
}

void wxMiniFrame::SetTitle( const wxString &title )
{
    wxFrame::SetTitle( title );

    GdkWindow* window = gtk_widget_get_window(gtk_bin_get_child(GTK_BIN(m_widget)));
    if (window)
        gdk_window_invalidate_rect(window, nullptr, false);
}

#endif // wxUSE_MINIFRAME
