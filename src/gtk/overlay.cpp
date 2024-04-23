///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/overlay.cpp
// Author:      Paul Cornett
// Copyright:   (c) 2022 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __WXGTK3__

#include "wx/private/overlay.h"
#include "wx/dc.h"
#include "wx/graphics.h"
#include "wx/window.h"
#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/backend.h"

class wxOverlayImpl: public wxOverlay::Impl
{
public:
    wxOverlayImpl();
    ~wxOverlayImpl();
    virtual bool IsOk() override;
    virtual void Init(wxDC* dc, int x, int y, int width, int height) override;
    virtual void BeginDrawing(wxDC* dc) override;
    virtual void EndDrawing(wxDC* dc) override;
    virtual void Clear(wxDC* dc) override;
    virtual void Reset() override;
    void PositionOverlay(GtkWidget* tlw);

    GtkWidget* m_overlay;
    GtkWidget* m_target;
    cairo_surface_t* m_surface;
    cairo_t* m_cr;
    wxRect m_rect;
};

wxOverlay::Impl* wxOverlay::Create()
{
    if (wxGTKImpl::IsWayland(nullptr))
        return new wxOverlayImpl;

    // Use generic
    return nullptr;
}

extern "C" {
static gboolean draw(GtkWidget*, cairo_t* cr, wxOverlayImpl* overlay)
{
    if (overlay->m_surface)
    {
        cairo_set_source_surface(cr, overlay->m_surface, 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
    }
    return true;
}

static gboolean map_event(GtkWidget* widget, GdkEvent*, wxOverlayImpl* overlay)
{
    overlay->PositionOverlay(widget);
    g_signal_handlers_disconnect_by_data(widget, overlay);
    return false;
}
} // extern "C"

wxOverlayImpl::wxOverlayImpl()
{
    m_overlay = nullptr;
    m_target = nullptr;
    m_surface = nullptr;
    m_cr = nullptr;
}

wxOverlayImpl::~wxOverlayImpl()
{
    if (m_surface)
        cairo_surface_destroy(m_surface);
    if (m_overlay)
    {
        gtk_widget_destroy(m_overlay);
        g_object_unref(m_overlay);
    }
}

bool wxOverlayImpl::IsOk()
{
    return false;
}

void wxOverlayImpl::Init(wxDC* dc, int x, int y, int width, int height)
{
    wxWindow* const win = dc->GetWindow();
    if (wxGraphicsContext* gc = dc->GetGraphicsContext())
        m_cr = static_cast<cairo_t*>(gc->GetNativeContext());

    wxCHECK_RET(win && m_cr, "invalid dc for wxOverlay");

    m_target = win->GetConnectWidget();
    GtkWidget* const tlw = gtk_widget_get_toplevel(m_target);
    if (m_overlay == nullptr)
    {
        // The overlay is a TLW to get "save-under" behavior, which avoids
        // repainting of the underlying window.
        // GTK_WINDOW_POPUP is implemented as a subsurface on Wayland, so it
        // will be moved/stacked with the underlying window.
        m_overlay = gtk_window_new(GTK_WINDOW_POPUP);
        g_object_ref(m_overlay);
        gtk_window_set_transient_for(GTK_WINDOW(m_overlay), GTK_WINDOW(tlw));
        gtk_window_set_accept_focus(GTK_WINDOW(m_overlay), false);
        gtk_widget_set_app_paintable(m_overlay, true);
        GdkScreen* screen = gtk_widget_get_screen(tlw);
        GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
        if (visual)
            gtk_widget_set_visual(m_overlay, visual);
        g_signal_connect(m_overlay, "draw", G_CALLBACK(draw), this);
    }
    // Overlay must be transparent for mouse events. gdk_window_set_pass_through()
    // doesn't work here, so use an input shape that is as small as possible.
    cairo_rectangle_int_t rect = { 0, 0, 1, 1 };
    cairo_region_t* region = cairo_region_create_rectangle(&rect);
    gtk_widget_input_shape_combine_region(m_overlay, nullptr);
    gtk_widget_input_shape_combine_region(m_overlay, region);
    cairo_region_destroy(region);

    // Convert to device space
    double d1 = x;
    double d2 = y;
    cairo_user_to_device(m_cr, &d1, &d2);
    m_rect.x = int(d1);
    m_rect.y = int(d2);
    d1 = width;
    d2 = height;
    cairo_user_to_device_distance(m_cr, &d1, &d2);
    m_rect.width = int(d1);
    m_rect.height = int(d2);
    // If axis is inverted
    if (m_rect.width < 0)
    {
        m_rect.width = -m_rect.width;
        m_rect.x -= m_rect.width;
    }
    if (m_rect.height < 0)
    {
        m_rect.height = -m_rect.height;
        m_rect.y -= m_rect.height;
    }

    gtk_widget_set_size_request(m_overlay, m_rect.width, m_rect.height);
    // Underlying window must be mapped before overlay can be positioned on it
    if (gtk_widget_get_mapped(tlw))
        PositionOverlay(tlw);
    else
        g_signal_connect(tlw, "map-event", G_CALLBACK(map_event), this);
}

void wxOverlayImpl::BeginDrawing(wxDC*)
{
    if (m_cr)
        cairo_push_group(m_cr);
}

void wxOverlayImpl::EndDrawing(wxDC* dc)
{
    if (m_cr == nullptr)
        return;

    cairo_pattern_t* pattern = cairo_pop_group(m_cr);
    if (m_surface)
        cairo_surface_destroy(m_surface);
    cairo_pattern_get_surface(pattern, &m_surface);
    cairo_surface_reference(m_surface);
    cairo_pattern_destroy(pattern);
    m_cr = nullptr;

    const wxSize size(dc->GetSize());
    if (m_rect.width < size.x || m_rect.height < size.y)
    {
        cairo_surface_t* surface = cairo_surface_create_similar(
            m_surface, CAIRO_CONTENT_COLOR_ALPHA, m_rect.width, m_rect.height);
        cairo_t* cr = cairo_create(surface);
        cairo_set_source_surface(cr, m_surface, -m_rect.x, -m_rect.y);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
        cairo_destroy(cr);
        cairo_surface_destroy(m_surface);
        m_surface = surface;
    }
    gtk_widget_queue_draw(m_overlay);
}

void wxOverlayImpl::Clear(wxDC*)
{
    // Surface is already cleared by Cairo
}

void wxOverlayImpl::Reset()
{
    if (m_surface)
    {
        cairo_surface_destroy(m_surface);
        m_surface = nullptr;
    }
    if (m_overlay)
        gtk_widget_hide(m_overlay);
}

void wxOverlayImpl::PositionOverlay(GtkWidget* tlw)
{
    int x, y;
    gtk_widget_translate_coordinates(m_target, tlw, m_rect.x, m_rect.y, &x, &y);
    gtk_window_move(GTK_WINDOW(m_overlay), x, y);
    gtk_widget_show(m_overlay);
}

#endif // __WXGTK3__
