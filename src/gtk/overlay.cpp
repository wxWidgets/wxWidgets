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

#include "wx/caret.h"
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

    if ( m_x11Helper )
    {
        if ( m_x11Helper->IsFrozen() )
            return;

        m_x11Helper->ResetIsShownOnScreen();
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

extern "C"
{
static GdkFilterReturn
wxOverlayWindowFilter(GdkXEvent* gdkxevent, GdkEvent* event, gpointer data)
{
    wxOverlayX11Helper * const overlay = static_cast<wxOverlayX11Helper *>(data);

    XEvent * const xevent = static_cast<XEvent *>(gdkxevent);

    Window appwin = overlay->XGetAppWindow();

    if ( xevent->type == ConfigureNotify )
    {
        if ( ((XConfigureEvent*)xevent)->window == appwin )
        {
            overlay->Freeze();
        }
    }
    else if ( xevent->type == DestroyNotify )
    {
        if ( ((XDestroyWindowEvent*)xevent)->window == appwin )
        {
            // we won't need it any more
            gdk_window_remove_filter(event->any.window,
                                     wxOverlayWindowFilter, data);
        }
    }

    return GDK_FILTER_CONTINUE;
}
}

wxOverlayX11Helper::wxOverlayX11Helper(wxOverlayGTKImpl* owner)
    : m_owner(owner), m_timer(this),
      m_isFrozen(false), m_isPointerInsideWindow(false)
{
    wxTopLevelWindow * const appWin =
        wxDynamicCast(wxGetTopLevelParent(owner->GetWindow()), wxTopLevelWindow);

    appWin->Bind(wxEVT_MOVE, &wxOverlayX11Helper::OnMove, this);
    appWin->Bind(wxEVT_SIZE, &wxOverlayX11Helper::OnSize, this);

    appWin->Bind(wxEVT_ENTER_WINDOW, &wxOverlayX11Helper::OnEnter, this);
    appWin->Bind(wxEVT_LEAVE_WINDOW, &wxOverlayX11Helper::OnLeave, this);
}

wxOverlayX11Helper::~wxOverlayX11Helper()
{
    wxTopLevelWindow * const appWin =
        wxDynamicCast(wxGetTopLevelParent(m_owner->GetWindow()), wxTopLevelWindow);

    if ( appWin )
    {
        appWin->Unbind(wxEVT_MOVE, &wxOverlayX11Helper::OnMove, this);
        appWin->Unbind(wxEVT_SIZE, &wxOverlayX11Helper::OnSize, this);

        appWin->Unbind(wxEVT_ENTER_WINDOW, &wxOverlayX11Helper::OnEnter, this);
        appWin->Unbind(wxEVT_LEAVE_WINDOW, &wxOverlayX11Helper::OnLeave, this);

        GdkWindow* root = gtk_widget_get_root_window(appWin->GetHandle());
        gdk_window_remove_filter(root, wxOverlayWindowFilter, this);
    }
}

Window wxOverlayX11Helper::XGetAppWindow() const
{
    if ( !m_owner->GetSurface() || !IsShownOnScreen() )
        return None;

    // It's safe to cache the _appwin_ value here as it is unique for each process.
    static Window appwin = None;

    if ( appwin != None )
    {
        return appwin;
    }

    GdkDisplay* display = gtk_widget_get_display(m_owner->GetSurface());

    gdk_x11_display_error_trap_push(display);
    gdk_x11_display_grab(display);

    GtkWidget* const tlw = gtk_widget_get_toplevel(m_owner->GetWindow()->GetHandle());

    Window parent = GDK_WINDOW_XID(gtk_widget_get_window(tlw));

    Window root, *wins;

    do
    {
        unsigned int nwins = 0;

        appwin = parent;

        if ( XQueryTree(GDK_DISPLAY_XDISPLAY(display), appwin, &root, &parent, &wins, &nwins) )
        {
            if ( nwins )
                XFree(wins);
        }

    } while ( parent != root );

    gdk_x11_display_ungrab(display);
    gdk_x11_display_error_trap_pop_ignored(display);

    return appwin;
}

void wxOverlayX11Helper::SetShownOnScreen()
{
    m_isShownOnScreen = true;

    wxWindow* window = m_owner->GetWindow();
    GtkWidget* surface = m_owner->GetSurface();

#if wxUSE_CARET
    if ( window->GetCaret() )
    {
        // We need to reposition the caret as it is not positioned
        // correctly when it is first shown under Gnome. and in fact,
        // there is no harm in doing it unconditionally for any WM
        // knowing that this adjustment is performed only once.
        // i.e. when the overlay window becomes visible on screen.
        const wxPoint pos = window->GetScreenPosition();

        gtk_window_move(GTK_WINDOW(surface), pos.x, pos.y);
    }
#endif // wxUSE_CARET

    GdkWindow* root = gtk_widget_get_root_window(window->GetHandle());
    GdkEventMask mask = gdk_window_get_events(root);
    gdk_window_set_events(root, GdkEventMask(mask | GDK_SUBSTRUCTURE_MASK));
    gdk_window_add_filter(root, wxOverlayWindowFilter, this);

    if ( !m_isPointerInsideWindow )
    {
        gtk_window_set_transient_for(GTK_WINDOW(surface), NULL);
    }
}

void wxOverlayX11Helper::Freeze()
{
    if ( m_isFrozen )
        return;

    m_isFrozen = true;

#if wxUSE_CARET
    wxCaret* caret = m_owner->GetWindow()->GetCaret();
    if ( caret )
        caret->Hide();
#endif // wxUSE_CARET

    m_owner->Reset();
}

void wxOverlayX11Helper::Thaw()
{
    if ( !m_isFrozen )
        return;

    GdkWindow* const gdkwin =
        gtk_widget_get_window(
            gtk_widget_get_toplevel(m_owner->GetWindow()->GetHandle()));

    const GdkEventMask mask = GdkEventMask(
            GDK_BUTTON_PRESS_MASK |
            GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_HINT_MASK |
            GDK_POINTER_MOTION_MASK);

    GdkDisplay* display = gdk_window_get_display(gdkwin);
    GdkDeviceManager* manager = gdk_display_get_device_manager(display);
    GdkDevice* device = gdk_device_manager_get_client_pointer(manager);
    GdkGrabStatus status = gdk_device_grab(device, gdkwin, GDK_OWNERSHIP_NONE, true,
                                           mask, NULL, unsigned(GDK_CURRENT_TIME));

    if ( status == GDK_GRAB_SUCCESS )
    {
        m_isFrozen = false;

#if wxUSE_CARET
        wxCaret* caret = m_owner->GetWindow()->GetCaret();
        if ( caret )
        {
            caret->Show();
        }
        else
#endif // wxUSE_CARET
        {
            // TODO: call UpdateOverlay() directly instead
            m_owner->GetWindow()->GetMainWindowOfCompositeControl()->Refresh(false);
        }

        gdk_device_ungrab(device, unsigned(GDK_CURRENT_TIME));
    }
    // else do nothing if there is an active grab on the pointer
    // meaning that we are still moving or resizing the window.
}

void wxOverlayX11Helper::OnMove(wxMoveEvent& event)
{
    event.Skip();
    Freeze(); // some systems need this (e.g. Unity)
    Thaw();
}

void wxOverlayX11Helper::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Freeze(); // some systems need this (e.g. Unity)

    // We need to postpone the call to "Thaw()" slightly because KDE generates
    // a ConfigureNotify after running this handler and we will end up with a
    // frozen overlay.
    m_timer.StartOnce(100);
}

void wxOverlayX11Helper::OnEnter(wxMouseEvent& event)
{
    m_isPointerInsideWindow = true;

    if ( m_isFrozen )
    {
        Thaw();
    }
    else if ( m_owner->IsOk() )
    {
        GtkWidget* const tlw = gtk_widget_get_toplevel(m_owner->GetWindow()->GetHandle());
        gtk_window_set_transient_for(GTK_WINDOW(m_owner->GetSurface()), GTK_WINDOW(tlw));
    }

    event.Skip();
}

void wxOverlayX11Helper::OnLeave(wxMouseEvent& event)
{
    m_isPointerInsideWindow = false;

    if ( m_owner->IsOk() )
    {
        gtk_window_set_transient_for(GTK_WINDOW(m_owner->GetSurface()), NULL);
    }

    event.Skip();
}

#endif // GDK_WINDOWING_X11

wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayGTKImpl(); }

#endif // __WXGTK3__
