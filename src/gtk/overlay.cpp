/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/overlay.cpp
// Purpose:     wxOverlay implementation for wxGTK
// Author:      Kettab Ali
// Created:     2021-06-01
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
#endif

#include "wx/display.h"
#include "wx/scrolwin.h"
#include "wx/gtk/dc.h"
#include "wx/gtk/private.h"
#include "wx/gtk/private/overlay.h"
#include "wx/overlay.h"


#ifdef wxHAS_NATIVE_OVERLAY

// ----------------------------------------------------------------------------
// Helper
// ----------------------------------------------------------------------------
namespace wxPrivate
{
static bool IsWayland()
{
#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay* display = gdk_display_get_default();
    const char* displayTypeName = g_type_name(G_TYPE_FROM_INSTANCE(display));

    return strcmp("GdkWaylandDisplay", displayTypeName) == 0;
#else
    return false;
#endif // GDK_WINDOWING_WAYLAND
}
} // namespace wxPrivate

// ============================================================================
// implementation
// ============================================================================
extern "C" {
static gboolean
wxgtk_draw_overlay(GtkWidget* widget, cairo_t* cr, wxOverlayGtkImpl* overlay)
{
    if ( gtk_cairo_should_draw_window(cr, gtk_widget_get_window(widget)) )
    {
        cairo_surface_t* surface = overlay->GetCairoSurface();

        if ( surface && cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS )
        {
            cairo_set_source_surface(cr, surface, 0, 0);
            cairo_paint(cr);
        }
    }

    return false;
}
}

// ----------------------------------------------------------------------------
// wxOverlayGtkImpl
// ----------------------------------------------------------------------------

wxOverlayGtkImpl::wxOverlayGtkImpl()
{
    m_window = NULL;
    m_surface = NULL;
    m_cairoSurface = NULL;
}

wxOverlayGtkImpl::~wxOverlayGtkImpl()
{
    Reset();
}

bool wxOverlayGtkImpl::IsGenericSupported() const
{
    static int s_isWayland = wxPrivate::IsWayland();

    return !s_isWayland;
}

bool wxOverlayGtkImpl::IsOk()
{
    return m_surface != NULL && GetBitmap().IsOk();
}

void wxOverlayGtkImpl::CreateSurface()
{
    m_surface = gtk_window_new(GTK_WINDOW_POPUP);

    gtk_window_set_focus_on_map(GTK_WINDOW(m_surface), FALSE);
    gtk_window_set_accept_focus(GTK_WINDOW(m_surface), FALSE);

    gtk_widget_set_app_paintable(m_surface, TRUE);

    GdkScreen* const screen = gdk_screen_get_default();
    GdkVisual* const visual = gdk_screen_get_rgba_visual(screen);

    if ( visual && gdk_screen_is_composited(screen) )
    {
        gtk_widget_set_visual(m_surface, visual);
    }

    GtkWidget* const tlw = wxGetTopLevelParent(m_window)->GetHandle();
    gtk_window_set_transient_for(GTK_WINDOW(m_surface), GTK_WINDOW(tlw));

    gtk_window_move(GTK_WINDOW(m_surface), m_rect.x, m_rect.y);
    gtk_widget_set_size_request(m_surface, m_rect.GetWidth(), m_rect.GetHeight());

    if ( wxPrivate::IsWayland() )
        g_signal_connect(m_surface, "draw", G_CALLBACK(wxgtk_draw_overlay), this);

    gtk_widget_show_all(m_surface);
}

void wxOverlayGtkImpl::InitFromWindow(wxWindow* win, bool fullscreen)
{
    wxASSERT_MSG( !IsOk(), "You cannot Init an overlay twice" );

    m_window = win;
    m_fullscreen = fullscreen;

    if ( fullscreen )
    {
        m_rect = wxDisplay(win).GetGeometry();
    }
    else
    {
        m_rect.SetSize(win->GetClientSize());
        m_rect.SetPosition(win->GetScreenPosition());
    }

    wxBitmap& bitmap = GetBitmap();
    bitmap.Create(m_rect.GetWidth(), m_rect.GetHeight(), 32);

    CreateSurface();
}

void wxOverlayGtkImpl::InitFromDC(wxDC* , int, int, int, int)
{
    wxFAIL_MSG("wxOverlay initialized from wxDC not implemented under wxGTK3");
}

void wxOverlayGtkImpl::SetUpdateRectangle(const wxRect& rect)
{
    m_rect = rect;
}

void wxOverlayGtkImpl::BeginDrawing(wxDC* dc)
{
    wxScrolledWindow* const win = wxDynamicCast(m_window, wxScrolledWindow);
    if ( win )
        win->PrepareDC(*dc);
}

void wxOverlayGtkImpl::EndDrawing(wxDC* dc)
{
    if ( !IsOk() )
        return;

    wxGTKCairoDCImpl* const dcimpl =
        static_cast<wxGTKCairoDCImpl*>(dc->GetImpl());

    wxCHECK_RET( dcimpl, wxS("DC is not a wxGTKCairoDCImpl!") );

    cairo_t* const cairoContext =
        static_cast<cairo_t*>(dcimpl->GetGraphicsContext()->GetNativeContext());

    m_cairoSurface = cairo_get_target(cairoContext);

    cairo_region_t* region = SetInputShape();

    const wxRect oldRect = m_oldRect;
    const wxPoint dcOrig = dc->GetDeviceOrigin(); // for wxScrolledWindows

    m_oldRect = m_rect; // remember the old rectangle

    if ( wxPrivate::IsWayland() )
    {
        // clear the old rectangle if necessary
        if ( !oldRect.IsEmpty() && !m_rect.Contains(oldRect) )
        {
            // We just rely on GTK to automatically clears the rectangle for us.
            gtk_widget_queue_draw_area(m_surface,
                dcOrig.x+oldRect.x, dcOrig.y+oldRect.y, oldRect.width, oldRect.height);
        }

        gtk_widget_queue_draw_area(m_surface,
            dcOrig.x+m_rect.x, dcOrig.y+m_rect.y, m_rect.width, m_rect.height);
    }
    else // not Wayland
    {
        // Under X we blit m_cairoSurface directly on m_surface

        cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(m_surface));
        cairo_translate(cr, dcOrig.x, dcOrig.y);

        cairo_rectangle_int_t rect = {oldRect.x, oldRect.y, oldRect.width, oldRect.height};
        cairo_region_union_rectangle(region, &rect);

        gdk_cairo_region(cr, region);

        cairo_set_source_surface(cr, m_cairoSurface, -dcOrig.x, -dcOrig.y);
        cairo_clip(cr);

        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);

        cairo_destroy(cr);
    }

    cairo_region_destroy(region);
    dc->DestroyClippingRegion();
}

void wxOverlayGtkImpl::Clear(wxDC* dc)
{
//  No idea why wxRegion doesn't work but wxRect ?
//  wxRegion region(m_rect);
//  region.Union(m_oldRect);
    wxRect rect = m_rect;
    rect.Union(m_oldRect);

    dc->SetClippingRegion(rect);

    // this is not working, too ?
    // dc->SetBackground(*wxTRANSPARENT_BRUSH);

    dc->SetBackground(wxBrush(wxTransparentColour));
    dc->Clear();
}

void wxOverlayGtkImpl::Reset()
{
    if ( !IsOk() )
        return;

    g_signal_handlers_disconnect_by_func(m_surface, (gpointer)wxgtk_draw_overlay, this);
    gtk_widget_hide(m_surface);
    gtk_widget_destroy(m_surface);

    m_window = NULL;
    m_surface = NULL;
    m_cairoSurface = NULL;

    GetBitmap() = wxBitmap();
}

cairo_region_t* wxOverlayGtkImpl::SetInputShape()
{
    // The overlay window m_surface is an output-only window and should be transparent
    // to mouse clicks so that they will be passed on to the window below it. and since
    // gdk_window_set_pass_through() doesn't seem to work across TLWs boundaries, we
    // have to set the whole window as transparent to mouse clicks except the region
    // denoted by m_rect.
    // Note that only windows with a permanently displayed overlay (wxCaret for example)
    // need this hack.

    gtk_widget_input_shape_combine_region(m_surface, NULL);

    cairo_rectangle_int_t rect = {m_rect.x, m_rect.y, m_rect.width, m_rect.height};
    cairo_region_t* region = cairo_region_create_rectangle(&rect);
    gtk_widget_input_shape_combine_region(m_surface, region);

    return region;
}

wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayGtkImpl(); }

#endif // wxHAS_NATIVE_OVERLAY
