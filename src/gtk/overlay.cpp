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
    #include "wx/window.h"
#endif

#include "wx/private/overlay.h"
#include "wx/gtk/dc.h"
#include "wx/gtk/private.h"

#ifdef wxHAS_NATIVE_OVERLAY

// ============================================================================
// implementation
// ============================================================================
extern "C" {
static gboolean
wxgtk_draw_overlay(GtkWidget* widget, cairo_t* cr, wxOverlayImpl* overlay)
{
    wxWindow* const win = overlay->GetWindow();

    if ( !win || !win->IsShownOnScreen() )
    {
        // We cannot draw to the overlay if there is no associated window
        // or the overlay widget is hidden.
        return false;
    }

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
// wxOverlay
// ----------------------------------------------------------------------------

wxOverlayImpl::wxOverlayImpl()
{
    m_surface = NULL;
    m_window = NULL;
    m_cairoContext = NULL;
    m_cairoSurface = NULL;
}

wxOverlayImpl::~wxOverlayImpl()
{
    if ( m_window )
        m_window->Unbind(wxEVT_SIZE, &wxOverlayImpl::OnSize, this);

    if ( m_cairoSurface )
        cairo_surface_destroy(m_cairoSurface);
}

bool wxOverlayImpl::IsOk() const
{
    return m_cairoSurface != NULL;
}

void wxOverlayImpl::Init(wxDC *dc, int x, int y, int width, int height)
{
    wxCHECK_RET( dc, "NULL dc pointer" );

    wxWindow* const win = dc->GetWindow();

    if ( m_window != win ) // wxOverlay reinitialized with another window
    {
        wxCHECK_RET( win->GTKEnableOverlay(),
            "Overlay couldn't be enabled for m_window" );

        Reset(true /*wxOverlay::DISPOSE*/);

        if ( m_window )
            m_window->Unbind(wxEVT_SIZE, &wxOverlayImpl::OnSize, this);

        m_window = win;
        m_window->Bind(wxEVT_SIZE, &wxOverlayImpl::OnSize, this);
    }

    if ( !m_surface )
    {
        // the surface might already be created and stashed in the associated window
        m_surface = static_cast<GtkWidget*>(
            g_object_get_data( G_OBJECT(m_window->m_widget), "overlay" ));
    }

    m_rect = wxRect(x, y, width, height);

    int clientWidth, clientHeight;
    m_window->GetClientSize(&clientWidth, &clientHeight);

    if ( m_surface )
    {
        if ( gtk_widget_get_allocated_width(m_surface) != clientWidth ||
             gtk_widget_get_allocated_height(m_surface) != clientHeight )
        {
            // We need to resize the surface because GTK won't automatically
            // resized it for us if the window was resized.
            gtk_widget_set_size_request(m_surface, clientWidth, clientHeight);
        }

        if ( !gtk_widget_get_visible(m_surface) )
            gtk_widget_show(m_surface);

        return;
    }

    // Create the surface and stash it in the associated window:

    GtkWidget* const overlay = m_window->m_widget;

    m_surface = gtk_drawing_area_new();
    gtk_widget_set_has_window(m_surface, FALSE);
    gtk_widget_set_size_request(m_surface, clientWidth, clientHeight);
    gtk_widget_set_halign(m_surface, GTK_ALIGN_FILL);
    gtk_widget_set_valign(m_surface, GTK_ALIGN_FILL);
    gtk_widget_set_vexpand(m_surface, TRUE);
    gtk_widget_set_hexpand(m_surface, TRUE);
    gtk_widget_set_can_focus(m_surface, FALSE);
    gtk_widget_set_sensitive(m_surface, FALSE);

    gtk_overlay_add_overlay( GTK_OVERLAY(overlay), m_surface );
    gtk_overlay_set_overlay_pass_through( GTK_OVERLAY(overlay), m_surface, TRUE );

    // We stash the created surface in the associated window in case the window
    // changes in the next call to this function (i.e. after a Reset())
    g_object_set_data( G_OBJECT(overlay), "overlay", m_surface );

    g_signal_connect(m_surface, "draw", G_CALLBACK(wxgtk_draw_overlay), this);

    gtk_widget_show(m_surface);
}

void wxOverlayImpl::SetUpdateRectangle(const wxRect& rect)
{
    m_rect = rect;
}

void wxOverlayImpl::BeginDrawing(wxDC* dc)
{
    wxCHECK_RET( dc, "NULL dc pointer" );

    wxClientDCImpl* const
        dcimpl = static_cast<wxClientDCImpl*>(dc->GetImpl());

    wxCHECK_RET( dcimpl, wxS("DC is not a wxClientDC!") );

    if ( m_cairoSurface )
        cairo_surface_destroy(m_cairoSurface);

    m_cairoContext = static_cast<cairo_t*>(dcimpl->GetCairoContext());
    cairo_push_group(m_cairoContext);

    m_cairoSurface = cairo_get_group_target(m_cairoContext);
    cairo_surface_reference(m_cairoSurface);
}

void wxOverlayImpl::EndDrawing(wxDC* WXUNUSED(dc))
{
    cairo_pop_group_to_source(m_cairoContext);
    cairo_surface_set_device_offset(m_cairoSurface, 0, 0);


    // clear the old rectangle if necessary
    if ( !m_oldRect.IsEmpty() && !m_rect.Contains(m_oldRect) )
    {
        // We just rely on GTK to automatically clears the rectangle for us.
        gtk_widget_queue_draw_area(m_surface,
            m_oldRect.x, m_oldRect.y, m_oldRect.width, m_oldRect.height);
    }

    gtk_widget_queue_draw_area(m_surface,
        m_rect.x, m_rect.y, m_rect.width, m_rect.height);

    m_oldRect = m_rect; // remember the old rectangle
}

void wxOverlayImpl::Clear(wxDC* WXUNUSED(dc))
{
    // everything will be cleared at the right moment in EndDrawing()
}

void wxOverlayImpl::Reset(bool dispose)
{
    if ( m_cairoSurface )
    {
        cairo_surface_destroy(m_cairoSurface);
        m_cairoSurface = NULL;
    }

    // In this port, we only hide and (re)show the overlay as needed,
    // never dispose it.

    if ( dispose && m_surface )
    {
        gtk_widget_hide(m_surface);
        m_surface = NULL;
    }
}

void wxOverlayImpl::OnSize(wxSizeEvent& event)
{
    Reset(false /*wxOverlay::NO_DISPOSE*/);
    event.Skip();
}

#endif // wxHAS_NATIVE_OVERLAY
