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
    #include "wx/toplevel.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
#endif

#include "wx/display.h"
#include "wx/scrolwin.h"
#include "wx/gtk/dc.h"
#include "wx/gtk/private.h"
#include "wx/gtk/private/overlay.h"


#ifdef __WXGTK3__

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
wxgtk_overlay_draw(GtkWidget* widget, cairo_t* cr, wxOverlayGTKImpl* overlay)
{
    if ( gtk_cairo_should_draw_window(cr, gtk_widget_get_window(widget)) )
    {
        cairo_surface_t* surface = overlay->GetCairoSurface();

        if ( surface && cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS )
        {
            if ( overlay->GetWindow()->GetLayoutDirection() == wxLayout_RightToLeft )
            {
                // DC is mirrored for RTL
                const int w = gdk_window_get_width(gtk_widget_get_window(widget));
                cairo_translate(cr, w, 0);
                cairo_scale(cr, -1, 1);
            }

            cairo_set_source_surface(cr, surface, 0, 0);
            cairo_paint(cr);
        }
    }

    if ( wxPrivate::IsWayland() )
    {
        return FALSE;
    }

    overlay->SetShownOnScreen();

    // no need to call this handler again.
    return TRUE;
}
}

// ----------------------------------------------------------------------------
// wxOverlayGTKImpl
// ----------------------------------------------------------------------------

wxOverlayGTKImpl::wxOverlayGTKImpl()
{
    m_window = NULL;
    m_surface = NULL;
    m_cairoSurface = NULL;

    m_x11Helper = NULL;
}

wxOverlayGTKImpl::~wxOverlayGTKImpl()
{
    delete m_x11Helper;

    Reset();
}

bool wxOverlayGTKImpl::IsGenericSupported() const
{
    static int s_isWayland = wxPrivate::IsWayland();

    return !s_isWayland;
}

bool wxOverlayGTKImpl::IsOk()
{
    return m_surface != NULL && GetBitmap().IsOk();
}

void wxOverlayGTKImpl::CreateSurface(wxOverlay::Target target)
{
    if ( wxPrivate::IsWayland() )
    {
        // In Wayland, a subsurface-based window is created for GTK_WINDOW_POPUP type
        // which is visually tied to a toplevel, and is moved/stacked with it.
        m_surface = gtk_window_new(GTK_WINDOW_POPUP);
    }
    else
    {
        // We do not use GTK_WINDOW_POPUP under X for this unwanted consequence:
        // If W is the window being overlayed by m_surface is covered by another window
        // W2, m_surface will be shown above W2 too even if it has the "above state" set.
        m_surface = gtk_window_new(GTK_WINDOW_TOPLEVEL);

        gtk_window_set_decorated(GTK_WINDOW(m_surface), FALSE);
        gtk_window_set_resizable(GTK_WINDOW(m_surface), FALSE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(m_surface), TRUE);
        gtk_window_set_skip_pager_hint(GTK_WINDOW(m_surface), TRUE);

        if ( target == wxOverlay::Overlay_Screen )
            gtk_window_set_keep_above(GTK_WINDOW(m_surface), TRUE);
    }

    gtk_window_set_focus_on_map(GTK_WINDOW(m_surface), FALSE);
    gtk_window_set_accept_focus(GTK_WINDOW(m_surface), FALSE);

    gtk_widget_set_app_paintable(m_surface, TRUE);

    GdkScreen* const screen = gdk_screen_get_default();
    GdkVisual* const visual = gdk_screen_get_rgba_visual(screen);

    if ( visual && gdk_screen_is_composited(screen) )
    {
        gtk_widget_set_visual(m_surface, visual);
    }

    GtkWidget* const tlw = gtk_widget_get_toplevel(m_window->GetHandle());
    gtk_window_set_transient_for(GTK_WINDOW(m_surface), GTK_WINDOW(tlw));

    if ( m_window->GetLayoutDirection() == wxLayout_LeftToRight )
        gtk_window_move(GTK_WINDOW(m_surface), m_rect.x, m_rect.y);
    else
        gtk_window_move(GTK_WINDOW(m_surface), m_rect.x - m_rect.GetWidth(), m_rect.y);

    gtk_widget_set_size_request(m_surface, m_rect.GetWidth(), m_rect.GetHeight());

    gtk_window_present(GTK_WINDOW(m_surface));

    if ( wxPrivate::IsWayland() || m_x11Helper )
    {
        g_signal_connect(m_surface, "draw", G_CALLBACK(wxgtk_overlay_draw), this);

        if ( m_x11Helper )
        {
            // this has the effect of showing the wxGrid selection overlay faster.
            gdk_display_sync(gtk_widget_get_display(tlw));
        }
    }
}

void wxOverlayGTKImpl::InitFromWindow(wxWindow* win, wxOverlay::Target target)
{
    wxASSERT_MSG( !IsOk(), "You cannot Init an overlay twice" );
    wxASSERT_MSG( !m_window || m_window == win,
        "wxOverlay re-initialized with a different window");

    m_window = win;

    if ( !m_x11Helper && !wxPrivate::IsWayland() &&
         target != wxOverlay::Overlay_Screen )
    {
        m_x11Helper = new wxOverlayX11Helper(this);
    }

    wxRect rect;

    if ( target == wxOverlay::Overlay_Screen )
    {
        rect = wxDisplay(win).GetGeometry();
    }
    else
    {
        rect.SetSize(win->GetClientSize());
        rect.SetPosition(win->GetScreenPosition());
    }

    if ( m_rect.GetWidth() < rect.GetWidth() ||
         m_rect.GetHeight() < rect.GetHeight() )
    {
        m_rect = rect;
    }

    wxBitmap& bitmap = GetBitmap();
    bitmap.Create(m_rect.GetWidth(), m_rect.GetHeight(), 32);

    CreateSurface(target);
}

void wxOverlayGTKImpl::InitFromDC(wxDC* , int, int, int, int)
{
    wxFAIL_MSG("wxOverlay initialized from wxDC not implemented under wxGTK3");
}

void wxOverlayGTKImpl::SetUpdateRectangle(const wxRect& rect)
{
    m_rect = rect;
}

void wxOverlayGTKImpl::BeginDrawing(wxDC* dc)
{
    wxScrolledWindow* const win = wxDynamicCast(m_window, wxScrolledWindow);
    if ( win )
        win->PrepareDC(*dc);
}

void wxOverlayGTKImpl::EndDrawing(wxDC* dc)
{
    if ( !IsOk() )
        return;

    wxGTKCairoDCImpl* const dcimpl =
        static_cast<wxGTKCairoDCImpl*>(dc->GetImpl());

    wxCHECK_RET( dcimpl, wxS("DC is not a wxGTKCairoDCImpl!") );

    cairo_t* const cairoContext =
        static_cast<cairo_t*>(dcimpl->GetGraphicsContext()->GetNativeContext());

    m_cairoSurface = cairo_get_target(cairoContext);

    SetInputShape();

    const int surfaceWidth = gdk_window_get_width(gtk_widget_get_window(m_surface));

    const wxPoint dcOrig = dc->GetDeviceOrigin(); // for wxScrolledWindows

    if ( wxPrivate::IsWayland() )
    {
        int rect_x, oldRect_x;

        if ( m_window->GetLayoutDirection() == wxLayout_LeftToRight )
        {
            rect_x = m_rect.x + dcOrig.x;
            oldRect_x = m_oldRect.x + dcOrig.x;
        }
        else // RTL
        {
            rect_x = surfaceWidth - m_rect.x - m_rect.width - dcOrig.x;
            oldRect_x = surfaceWidth - m_oldRect.x - m_oldRect.width - dcOrig.x;
        }

        // clear the old rectangle if necessary
        if ( !m_oldRect.IsEmpty() && !m_rect.Contains(m_oldRect) )
        {
            // We just rely on GTK to automatically clears the rectangle for us.
            gtk_widget_queue_draw_area(m_surface,
                oldRect_x, m_oldRect.y + dcOrig.y, m_oldRect.width, m_oldRect.height);
        }

        gtk_widget_queue_draw_area(m_surface,
            rect_x, m_rect.y + dcOrig.y, m_rect.width, m_rect.height);
    }
    else if ( IsShownOnScreen() ) // not Wayland
    {
        // Under X we blit m_cairoSurface directly on m_surface

        cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(m_surface));

        if ( m_window->GetLayoutDirection() == wxLayout_LeftToRight )
        {
            cairo_translate(cr, dcOrig.x, dcOrig.y);
        }
        else
        {
            // DC is mirrored for RTL
            cairo_translate(cr, surfaceWidth - dcOrig.x, dcOrig.y);
            cairo_scale(cr, -1, 1);
        }

        cairo_rectangle_int_t rect = {m_rect.x, m_rect.y, m_rect.width, m_rect.height};
        cairo_region_t* region = cairo_region_create_rectangle(&rect);

        if ( !m_oldRect.IsEmpty() )
        {
            rect = {m_oldRect.x, m_oldRect.y, m_oldRect.width, m_oldRect.height};
            cairo_region_union_rectangle(region, &rect);
        }

        gdk_cairo_region(cr, region);
        cairo_region_destroy(region);

        cairo_set_source_surface(cr, m_cairoSurface, -dcOrig.x, -dcOrig.y);
        cairo_clip(cr);

        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);

        cairo_surface_flush(cairo_get_target(cr));
        cairo_destroy(cr);
    }

    m_oldRect = m_rect; // remember the old rectangle

    dc->DestroyClippingRegion(); // destroy the clipping region set in Clear()
}

void wxOverlayGTKImpl::Clear(wxDC* dc)
{
    wxRect rect = m_rect;
    rect.Union(m_oldRect);

    dc->SetClippingRegion(rect);

    dc->SetBackground(wxBrush(wxTransparentColour));
    dc->Clear();
}

void wxOverlayGTKImpl::Reset()
{
    if ( !IsOk() )
        return;

    g_signal_handlers_disconnect_by_func(m_surface, (gpointer)wxgtk_overlay_draw, this);
    gtk_widget_hide(m_surface);
    gtk_widget_destroy(m_surface);

    m_surface = NULL;
    m_cairoSurface = NULL;

    m_oldRect = wxRect();

    GetBitmap() = wxBitmap();
}

void wxOverlayGTKImpl::SetInputShape()
{
    // The overlay window m_surface is an output-only window and should be transparent
    // to mouse clicks so that they will be passed on to the window below it. and since
    // gdk_window_set_pass_through() doesn't seem to work across TLWs boundaries, we
    // have to set the whole window as transparent to mouse clicks by passing a dummy
    // region (1px wide) to gtk_widget_input_shape_combine_region()

    gtk_widget_input_shape_combine_region(m_surface, NULL);

    cairo_rectangle_int_t rect = {m_rect.x, m_rect.y, 1, 1};
    cairo_region_t* region = cairo_region_create_rectangle(&rect);
    gtk_widget_input_shape_combine_region(m_surface, region);
    cairo_region_destroy(region);
}

void wxOverlayGTKImpl::SetShownOnScreen()
{
    if ( m_x11Helper )
        m_x11Helper->SetShownOnScreen();
}

bool wxOverlayGTKImpl::IsShownOnScreen() const
{
    return !m_x11Helper || m_x11Helper->IsShownOnScreen();
}

//------------------------------------------------------------------------------
// wxOverlayX11Helper implementation
//------------------------------------------------------------------------------

#ifdef GDK_WINDOWING_X11

wxOverlayX11Helper::wxOverlayX11Helper(wxOverlayGTKImpl* owner)
    : m_owner(owner), m_timer(this)
{
    wxTopLevelWindow * const appWin =
        wxDynamicCast(wxGetTopLevelParent(owner->GetWindow()), wxTopLevelWindow);

    appWin->Bind(wxEVT_MOVE, &wxOverlayX11Helper::OnMove, this);
    appWin->Bind(wxEVT_SIZE, &wxOverlayX11Helper::OnSize, this);
}

wxOverlayX11Helper::~wxOverlayX11Helper()
{
    wxTopLevelWindow * const appWin =
        wxDynamicCast(wxGetTopLevelParent(m_owner->GetWindow()), wxTopLevelWindow);

    if ( appWin )
    {
        appWin->Unbind(wxEVT_MOVE, &wxOverlayX11Helper::OnMove, this);
        appWin->Unbind(wxEVT_SIZE, &wxOverlayX11Helper::OnSize, this);
    }
}

void wxOverlayX11Helper::SetShownOnScreen()
{
    m_isShownOnScreen = true;
}

void wxOverlayX11Helper::OnMove(wxMoveEvent& event)
{
    event.Skip();

    m_owner->Reset();
    m_timer.StartOnce(100);
}

void wxOverlayX11Helper::OnSize(wxSizeEvent& event)
{
    event.Skip();

    m_owner->Reset();
    m_timer.StartOnce(100);
}

void wxOverlayX11Helper::OverlayTimer::Notify()
{
    m_helper->GetWindow()->GetMainWindowOfCompositeControl()->Refresh(false);
}

#endif // GDK_WINDOWING_X11

wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayGTKImpl(); }

#endif // __WXGTK3__
