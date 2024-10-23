/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/toplevel.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __VMS
#define XIconifyWindow XICONIFYWINDOW
#endif

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/app.h"     // GetAppDisplayName()
    #include "wx/icon.h"
    #include "wx/log.h"
#endif

#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/recguard.h"
#include "wx/sysopt.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/wrapgdk.h"
#include "wx/gtk/private/gtk3-compat.h"
#include "wx/gtk/private/stylecontext.h"
#include "wx/gtk/private/win_gtk.h"
#include "wx/gtk/private/backend.h"
#include "wx/gtk/private/threads.h"

#ifdef GDK_WINDOWING_X11
    #include <X11/Xatom.h>  // XA_CARDINAL
    #include "wx/unix/utilsx11.h"
#endif

#define TRACE_TLWSIZE "tlwsize"

// ----------------------------------------------------------------------------
// data
// ----------------------------------------------------------------------------

// the frame that is currently active (i.e. its child has focus). It is
// used to generate wxActivateEvents
static wxTopLevelWindowGTK *g_activeFrame = nullptr;

extern wxCursor g_globalCursor;
extern wxCursor g_busyCursor;
extern wxRecursionGuardFlag g_inSizeAllocate;

#ifdef GDK_WINDOWING_X11
// Whether _NET_REQUEST_FRAME_EXTENTS support is working
static enum {
    RFE_STATUS_UNKNOWN, RFE_STATUS_WORKING, RFE_STATUS_BROKEN
} gs_requestFrameExtentsStatus;

static bool gs_decorCacheValid;
#endif

#ifdef __WXGTK3__
static bool HasClientDecor(GtkWidget* widget)
{
    if (!gtk_window_get_decorated(GTK_WINDOW(widget)))
        return false;

    GdkDisplay* display = gtk_widget_get_display(widget);
    if (wxGTKImpl::IsX11(display))
        return false;

    // Contrary to the annotation in the header, this function has become
    // available only in 3.22.25 and not 3.22.0.
#if defined(GDK_WINDOWING_WAYLAND) && GTK_CHECK_VERSION(3,22,25)
    if (wxGTKImpl::IsWayland(display))
    {
#ifndef __WXGTK4__
        if (gtk_check_version(3, 22, 25) == nullptr)
#endif
            return !gdk_wayland_display_prefers_ssd(display);
    }
#endif
    return true;
}
#else
static inline bool HasClientDecor(GtkWidget*)
{
    return false;
}
#endif

//-----------------------------------------------------------------------------
// RequestUserAttention related functions
//-----------------------------------------------------------------------------

#ifndef __WXGTK3__
static void wxgtk_window_set_urgency_hint (GtkWindow *win,
                                           gboolean setting)
{
#if GTK_CHECK_VERSION(2,7,0)
    if (wx_is_at_least_gtk2(7))
        gtk_window_set_urgency_hint(win, setting);
    else
#endif
    {
#ifdef GDK_WINDOWING_X11
        GdkWindow* window = gtk_widget_get_window(GTK_WIDGET(win));
        wxCHECK_RET(window, "wxgtk_window_set_urgency_hint: GdkWindow not realized");

        Display* dpy = GDK_WINDOW_XDISPLAY(window);
        Window xid = GDK_WINDOW_XID(window);
        XWMHints* wm_hints = XGetWMHints(dpy, xid);

        if (!wm_hints)
            wm_hints = XAllocWMHints();

        if (setting)
            wm_hints->flags |= XUrgencyHint;
        else
            wm_hints->flags &= ~XUrgencyHint;

        XSetWMHints(dpy, xid, wm_hints);
        XFree(wm_hints);
#endif // GDK_WINDOWING_X11
    }
}
#define gtk_window_set_urgency_hint wxgtk_window_set_urgency_hint
#endif

extern "C" {
static gboolean gtk_frame_urgency_timer_callback( wxTopLevelWindowGTK *win )
{
    gtk_window_set_urgency_hint(GTK_WINDOW(win->m_widget), false);

    win->m_urgency_hint = -2;
    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "focus_in_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean gtk_frame_focus_in_callback( GtkWidget *widget,
                                         GdkEvent *WXUNUSED(event),
                                         wxTopLevelWindowGTK *win )
{
    g_activeFrame = win;

    // MR: wxRequestUserAttention related block
    switch( win->m_urgency_hint )
    {
        default:
            g_source_remove( win->m_urgency_hint );
            // no break, fallthrough to remove hint too
            wxFALLTHROUGH;
        case -1:
            gtk_window_set_urgency_hint(GTK_WINDOW(widget), false);
            win->m_urgency_hint = -2;
            break;

        case -2: break;
    }

    wxActivateEvent event(wxEVT_ACTIVATE, true, g_activeFrame->GetId());
    event.SetEventObject(g_activeFrame);
    g_activeFrame->HandleWindowEvent(event);

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "focus_out_event"
//-----------------------------------------------------------------------------

extern "C" {
static
gboolean gtk_frame_focus_out_callback(GtkWidget * WXUNUSED(widget),
                                      GdkEventFocus *WXUNUSED(gdk_event),
                                      wxTopLevelWindowGTK * WXUNUSED(win))
{
    if (g_activeFrame)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, false, g_activeFrame->GetId());
        event.SetEventObject(g_activeFrame);
        g_activeFrame->HandleWindowEvent(event);

        g_activeFrame = nullptr;
    }

    return FALSE;
}
}

// ----------------------------------------------------------------------------
// "key_press_event"
// ----------------------------------------------------------------------------

extern "C" {
static gboolean
wxgtk_tlw_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
    GtkWindow* const window = GTK_WINDOW(widget);

    // By default GTK+ checks for the menu accelerators in this (top level)
    // window first and then propagates the event to the currently focused
    // child from where it bubbles up the window parent chain. In wxWidgets,
    // however, we want the child window to have the event first but still
    // handle it as an accelerator if it's not processed there, so we need to
    // customize this by reversing the order of the steps done in the standard
    // GTK+ gtk_window_key_press_event() handler.

    if ( gtk_window_propagate_key_event(window, event) )
        return true;

    if ( gtk_window_activate_key(window, event) )
        return true;

    void* parent_class = g_type_class_peek_parent(G_OBJECT_GET_CLASS(widget));
    GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);

    // Avoid calling the default handler, we have already done everything it does
    return true;
}
}

//-----------------------------------------------------------------------------
// "size_allocate" from m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static void
size_allocate(GtkWidget*, GtkAllocation* alloc, wxTopLevelWindowGTK* win)
{
    win->m_useCachedClientSize = true;
    GtkAllocation a;
    gtk_widget_get_allocation(win->m_widget, &a);
    const bool hasClientDecor = HasClientDecor(win->m_widget);
    if (hasClientDecor)
    {
        GtkAllocation a2;
        gtk_widget_get_allocation(win->m_mainWidget, &a2);
        wxTopLevelWindowGTK::DecorSize decorSize;
        decorSize.left = a2.x;
        decorSize.right = a.width - a2.width - a2.x;
        decorSize.top = a2.y;
        decorSize.bottom = a.height - a2.height - a2.y;
        if (memcmp(&win->m_decorSize, &decorSize, sizeof(decorSize)) != 0)
        {
            win->GTKUpdateDecorSize(decorSize);
            win->m_clientWidth = 0;
        }
    }
    if (win->m_clientWidth  != alloc->width ||
        win->m_clientHeight != alloc->height)
    {
        wxLogTrace(TRACE_TLWSIZE, "Size changed for %s (%d, %d) -> (%d, %d)",
                   wxDumpWindow(win),
                   win->m_clientWidth, win->m_clientHeight,
                   alloc->width, alloc->height);

        wxRecursionGuard setInSizeAllocate(g_inSizeAllocate);

        win->m_clientWidth  = alloc->width;
        win->m_clientHeight = alloc->height;

        wxSize size(a.width, a.height);
        if (!hasClientDecor)
        {
            size.x += win->m_decorSize.left + win->m_decorSize.right;
            size.y += win->m_decorSize.top + win->m_decorSize.bottom;
        }
        win->m_width  = size.x;
        win->m_height = size.y;

        if (!win->IsIconized())
        {
            wxSizeEvent event(size, win->GetId());
            event.SetEventObject(win);
            win->HandleWindowEvent(event);
        }
        // else the window is currently unmapped, don't generate size events
    }
}
}

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_delete_callback( GtkWidget *WXUNUSED(widget),
                           GdkEvent *WXUNUSED(event),
                           wxTopLevelWindowGTK *win )
{
    if (win->IsEnabled() &&
        (wxModalDialogHook::GetOpenCount() == 0 ||
         (win->GetExtraStyle() & wxTOPLEVEL_EX_DIALOG) ||
         win->IsGrabbed()))
        win->Close();

    return TRUE;
}
}

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_configure_callback( GtkWidget*,
                              GdkEventConfigure* gdk_event,
                              wxTopLevelWindowGTK *win )
{
    win->GTKConfigureEvent(gdk_event->x, gdk_event->y);
    return false;
}
}

void wxTopLevelWindowGTK::GTKConfigureEvent(int x, int y)
{
#ifdef __WXGTK3__
    // First of all check if our DPI has changed.
    const auto newScaleFactor = GetContentScaleFactor();
    if ( newScaleFactor != m_scaleFactor )
    {
        const auto oldScaleFactor = m_scaleFactor;

        // It seems safer to change it before generating the events to avoid
        // any chance of reentrancy.
        m_scaleFactor = newScaleFactor;

        WXNotifyDPIChange(oldScaleFactor, newScaleFactor);
    }
#endif // __WXGTK3__

    wxPoint point;
#ifdef GDK_WINDOWING_X11
    if (gs_decorCacheValid)
    {
        const DecorSize& decorSize = GetCachedDecorSize();
        point.x = x - decorSize.left;
        point.y = y - decorSize.top;
    }
    else
#else
    wxUnusedVar(x);
    wxUnusedVar(y);
#endif
    {
        gtk_window_get_position(GTK_WINDOW(m_widget), &point.x, &point.y);
    }

    if (m_x != point.x || m_y != point.y)
    {
        m_lastPos = wxPoint(m_x, m_y);

        m_x = point.x;
        m_y = point.y;
        wxMoveEvent event(point, GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

#if GTK_CHECK_VERSION(3,10,0)
extern "C" {
static void findTitlebar(GtkWidget* widget, void* data)
{
    if (GTK_IS_HEADER_BAR(widget))
        *static_cast<GtkWidget**>(data) = widget;
}
}
#endif

// we cannot the WM hints and icons before the widget has been realized,
// so we do this directly after realization

void wxTopLevelWindowGTK::GTKHandleRealized()
{
    wxNonOwnedWindow::GTKHandleRealized();

    GdkWindow* window = gtk_widget_get_window(m_widget);

#if GTK_CHECK_VERSION(3,10,0)
    if (wx_is_at_least_gtk3(10))
    {
        GtkWidget* titlebar = nullptr;
        gtk_container_forall(GTK_CONTAINER(m_widget), findTitlebar, &titlebar);
        if (titlebar)
        {
#if GTK_CHECK_VERSION(3,12,0)
            if (m_gdkDecor && wx_is_at_least_gtk3(12))
            {
                char layout[sizeof("icon,menu:minimize,maximize,close")];
                snprintf(layout, sizeof(layout), "icon%s:%s%s%s",
                     m_gdkDecor & GDK_DECOR_MENU ? ",menu" : "",
                     m_gdkDecor & GDK_DECOR_MINIMIZE ? "minimize," : "",
                     m_gdkDecor & GDK_DECOR_MAXIMIZE ? "maximize," : "",
                     m_gdkFunc & GDK_FUNC_CLOSE ? "close" : "");
                gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(titlebar), layout);
            }
#endif // 3.12
            // Don't set WM decorations when GTK is using Client Side Decorations
            m_gdkDecor = 0;
        }
    }
#endif // 3.10

    gdk_window_set_decorations(window, (GdkWMDecoration)m_gdkDecor);
    gdk_window_set_functions(window, (GdkWMFunction)m_gdkFunc);

    const wxIconBundle& icons = GetIcons();
    if (icons.GetIconCount())
        SetIcons(icons);

    GdkCursor* cursor = g_globalCursor.GetCursor();
    if (wxIsBusy() && !gtk_window_get_modal(GTK_WINDOW(m_widget)))
        cursor = g_busyCursor.GetCursor();

    if (cursor)
        gdk_window_set_cursor(window, cursor);

#ifdef __WXGTK3__
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    if (gtk_window_get_has_resize_grip(GTK_WINDOW(m_widget)))
    {
        // Grip window can end up obscured, probably due to deferred show.
        // Reset grip to ensure it is visible.
        gtk_window_set_has_resize_grip(GTK_WINDOW(m_widget), false);
        gtk_window_set_has_resize_grip(GTK_WINDOW(m_widget), true);
    }
    wxGCC_WARNING_RESTORE()
#endif
}

//-----------------------------------------------------------------------------
// "map_event" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_map_callback( GtkWidget*,
                        GdkEvent * WXUNUSED(event),
                        wxTopLevelWindow *win )
{
    const bool wasIconized = win->IsIconized();
    if (wasIconized)
    {
        // Because GetClientSize() returns (0,0) when IsIconized() is true,
        // a size event must be generated, just in case GetClientSize() was
        // called while iconized. This specifically happens when restoring a
        // tlw that was "rolled up" with some WMs.
        // Queue a resize rather than sending size event directly to allow
        // children to be made visible first.
        win->m_useCachedClientSize = false;
        win->m_clientWidth = 0;
        gtk_widget_queue_resize(win->m_wxwindow);
    }
    // it is possible for m_isShown to be false here, see bug #9909
    if (win->wxWindowBase::Show(true))
    {
        win->GTKDoAfterShow();
    }

    // restore focus-on-map setting in case ShowWithoutActivating() was called
    gtk_window_set_focus_on_map(GTK_WINDOW(win->m_widget), true);

    return false;
}
}

//-----------------------------------------------------------------------------
// "window-state-event" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean
gtk_frame_window_state_callback( GtkWidget* WXUNUSED(widget),
                          GdkEventWindowState *event,
                          wxTopLevelWindow *win )
{
    if (event->changed_mask & GDK_WINDOW_STATE_ICONIFIED)
        win->SetIconizeState((event->new_window_state & GDK_WINDOW_STATE_ICONIFIED) != 0);

    // if maximized bit changed and it is now set
    if (event->changed_mask & event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
    {
        wxMaximizeEvent evt(win->GetId());
        evt.SetEventObject(win);
        win->HandleWindowEvent(evt);
    }

    if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
        win->m_fsIsShowing = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) != 0;

    return false;
}
}

//-----------------------------------------------------------------------------
// "notify::gtk-theme-name" from GtkSettings
//-----------------------------------------------------------------------------

extern "C" {
static void notify_gtk_theme_name(GObject*, GParamSpec*, wxTopLevelWindowGTK* win)
{
    wxSysColourChangedEvent event;
    event.SetEventObject(win);
    win->HandleWindowEvent(event);
}
}

//-----------------------------------------------------------------------------

bool wxGetFrameExtents(GdkWindow* window, int* left, int* right, int* top, int* bottom)
{
#ifdef GDK_WINDOWING_X11
    GdkDisplay* display = gdk_window_get_display(window);

#ifdef __WXGTK3__
    if (!wxGTKImpl::IsX11(display))
        return false;
#endif

    static GdkAtom property = gdk_atom_intern("_NET_FRAME_EXTENTS", false);
    Atom xproperty = gdk_x11_atom_to_xatom_for_display(display, property);
    Atom type;
    int format;
    gulong nitems, bytes_after;
    guchar* data = nullptr;
    Status status = XGetWindowProperty(
        GDK_DISPLAY_XDISPLAY(display),
        GDK_WINDOW_XID(window),
        xproperty,
        0, 4, false, XA_CARDINAL,
        &type, &format, &nitems, &bytes_after, &data);
    const bool success = status == Success && data && nitems == 4;
    if (success)
    {
        // We need to convert the X11 physical extents to GTK+ "logical" units
        int scale = 1;
#if GTK_CHECK_VERSION(3,10,0)
        if (wx_is_at_least_gtk3(10))
            scale = gdk_window_get_scale_factor(window);
#endif
        long* p = (long*)data;
        if (left)   *left   = int(p[0]) / scale;
        if (right)  *right  = int(p[1]) / scale;
        if (top)    *top    = int(p[2]) / scale;
        if (bottom) *bottom = int(p[3]) / scale;
    }
    if (data)
        XFree(data);
    return success;
#else
    wxUnusedVar(window);
    wxUnusedVar(left);
    wxUnusedVar(right);
    wxUnusedVar(top);
    wxUnusedVar(bottom);
    return false;
#endif
}

#ifdef GDK_WINDOWING_X11
//-----------------------------------------------------------------------------
// "property_notify_event" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static gboolean property_notify_event(
    GtkWidget*, GdkEventProperty* event, wxTopLevelWindowGTK* win)
{
    // Watch for changes to _NET_FRAME_EXTENTS property
    static GdkAtom property = gdk_atom_intern("_NET_FRAME_EXTENTS", false);
    if (event->state == GDK_PROPERTY_NEW_VALUE && event->atom == property)
    {
        if (win->m_netFrameExtentsTimerId)
        {
            // WM support for _NET_REQUEST_FRAME_EXTENTS is working
            gs_requestFrameExtentsStatus = RFE_STATUS_WORKING;
            g_source_remove(win->m_netFrameExtentsTimerId);
            win->m_netFrameExtentsTimerId = 0;
        }

        wxTopLevelWindowGTK::DecorSize decorSize = win->m_decorSize;
        gs_decorCacheValid = wxGetFrameExtents(event->window,
            &decorSize.left, &decorSize.right, &decorSize.top, &decorSize.bottom);

        win->GTKUpdateDecorSize(decorSize);
    }
    return false;
}
}

extern "C" {
static gboolean request_frame_extents_timeout(void* data)
{
    // WM support for _NET_REQUEST_FRAME_EXTENTS is broken
    gs_requestFrameExtentsStatus = RFE_STATUS_BROKEN;

    wxGDKThreadsLock threadsLock;
    wxTopLevelWindowGTK* win = static_cast<wxTopLevelWindowGTK*>(data);
    win->m_netFrameExtentsTimerId = 0;
    wxTopLevelWindowGTK::DecorSize decorSize = win->m_decorSize;
    wxGetFrameExtents(gtk_widget_get_window(win->m_widget),
        &decorSize.left, &decorSize.right, &decorSize.top, &decorSize.bottom);
    win->GTKUpdateDecorSize(decorSize);
    return false;
}
}
#endif // GDK_WINDOWING_X11

// ----------------------------------------------------------------------------
// wxTopLevelWindowGTK creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Init()
{
    m_mainWidget = nullptr;
    m_isIconized = false;
    m_fsIsShowing = false;
    m_themeEnabled = true;
    m_gdkDecor =
    m_gdkFunc = 0;
    m_grabbedEventLoop = nullptr;
    m_deferShow = true;
    m_deferShowAllowed = true;
    m_updateDecorSize = true;
    m_netFrameExtentsTimerId = 0;
    m_incWidth = m_incHeight = 0;

    m_urgency_hint = -2;

#ifdef __WXGTK3__
    m_pendingFittingClientSizeFlags = 0;
#endif // __WXGTK3__
}

bool wxTopLevelWindowGTK::Create( wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& title,
                                  const wxPoint& pos,
                                  const wxSize& sizeOrig,
                                  long style,
                                  const wxString &name )
{
    wxSize size(sizeOrig);
    if (!size.IsFullySpecified())
        size.SetDefaults(GetDefaultSize());

    wxTopLevelWindows.Append( this );

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxTopLevelWindowGTK creation failed") );
        return false;
    }

    m_title = title;

#ifndef __WXGTK4__
    // Gnome uses class as display name
    gdk_set_program_class(wxTheApp->GetAppDisplayName().utf8_str());
#endif

    // NB: m_widget may be !=nullptr if it was created by derived class' Create,
    //     e.g. in wxTaskBarIconAreaGTK
    if (m_widget == nullptr)
    {
        m_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
        {
            // Tell WM that this is a dialog window and make it center
            // on parent by default (this is what GtkDialog ctor does):
            gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                                     GDK_WINDOW_TYPE_HINT_DIALOG);
            gtk_window_set_position(GTK_WINDOW(m_widget),
                                    GTK_WIN_POS_CENTER_ON_PARENT);
        }
        else
        {
            if (style & wxFRAME_TOOL_WINDOW)
            {
                gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                                         GDK_WINDOW_TYPE_HINT_UTILITY);

                // On some WMs, like KDE, a TOOL_WINDOW will still show
                // on the taskbar, but on Gnome a TOOL_WINDOW will not.
                // For consistency between WMs and with Windows, we
                // should set the NO_TASKBAR flag which will apply
                // the set_skip_taskbar_hint if it is available,
                // ensuring no taskbar entry will appear.
                style |= wxFRAME_NO_TASKBAR;
            }
        }

        g_object_ref(m_widget);
    }

#ifdef __WXGTK3__
    // This value may be incorrect as here it's just set to the scale factor of
    // the primary monitor because the widget is not realized yet, but it's
    // better than setting it to 1, as chances are that the window will be
    // created on the primary monitor or, maybe, the other monitors use the
    // same scale factor anyhow. And if this isn't the case, we will set it to
    // the correct value when we get "configure-event" from GTK later.
    m_scaleFactor = GetContentScaleFactor();
#endif // __WXGTK3__

    wxWindow *topParent = wxGetTopLevelParent(m_parent);
    if (topParent && (((GTK_IS_WINDOW(topParent->m_widget)) &&
                       (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)) ||
                       (style & wxFRAME_FLOAT_ON_PARENT) ||
                       (style & wxSTAY_ON_TOP)))
    {
        gtk_window_set_transient_for( GTK_WINDOW(m_widget),
                                      GTK_WINDOW(topParent->m_widget) );
    }

    if (style & wxFRAME_NO_TASKBAR)
    {
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(m_widget), TRUE);
    }

    if (style & wxSTAY_ON_TOP)
    {
        gtk_window_set_keep_above(GTK_WINDOW(m_widget), TRUE);
    }
    if (style & wxMAXIMIZE)
        gtk_window_maximize(GTK_WINDOW(m_widget));

#if 0
    if (!name.empty())
        gtk_window_set_role( GTK_WINDOW(m_widget), name.utf8_str() );
#endif

    gtk_window_set_title( GTK_WINDOW(m_widget), title.utf8_str() );
    gtk_widget_set_can_focus(m_widget, false);

    g_signal_connect (m_widget, "delete_event",
                      G_CALLBACK (gtk_frame_delete_callback), this);

    // m_mainWidget is a GtkVBox, holding the bars and client area (m_wxwindow)
    m_mainWidget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show( m_mainWidget );
    gtk_widget_set_can_focus(m_mainWidget, false);
    gtk_container_add( GTK_CONTAINER(m_widget), m_mainWidget );

    // m_wxwindow is the client area
    m_wxwindow = wxPizza::New();
    gtk_widget_show( m_wxwindow );
    gtk_box_pack_start(GTK_BOX(m_mainWidget), m_wxwindow, true, true, 0);

    // we donm't allow the frame to get the focus as otherwise
    // the frame will grab it at arbitrary focus changes
    gtk_widget_set_can_focus(m_wxwindow, false);

    if (m_parent) m_parent->AddChild( this );

    g_signal_connect(m_wxwindow, "size_allocate",
        G_CALLBACK(size_allocate), this);

    PostCreation();

    if (pos.IsFullySpecified())
    {
#ifdef __WXGTK3__
        gtk_window_move(GTK_WINDOW(m_widget), m_x, m_y);
#else
        gtk_widget_set_uposition( m_widget, m_x, m_y );
#endif
    }

    // for some reported size corrections
    g_signal_connect (m_widget, "map_event",
                      G_CALLBACK (gtk_frame_map_callback), this);

    // for iconized state
    g_signal_connect (m_widget, "window_state_event",
                      G_CALLBACK (gtk_frame_window_state_callback), this);


    // for wxMoveEvent
    g_signal_connect (m_widget, "configure_event",
                      G_CALLBACK (gtk_frame_configure_callback), this);

    // activation
    g_signal_connect_after (m_widget, "focus_in_event",
                      G_CALLBACK (gtk_frame_focus_in_callback), this);
    g_signal_connect_after (m_widget, "focus_out_event",
                      G_CALLBACK (gtk_frame_focus_out_callback), this);

    // We need to customize the default GTK+ logic for key processing to make
    // it conforming to wxWidgets event processing order.
    g_signal_connect (m_widget, "key_press_event",
                      G_CALLBACK (wxgtk_tlw_key_press_event), nullptr);

#ifdef __WXGTK3__
    GdkDisplay* display = gtk_widget_get_display(m_widget);
#endif
#ifdef GDK_WINDOWING_X11
#ifdef __WXGTK3__
    if (wxGTKImpl::IsX11(display))
#endif
    {
        gtk_widget_add_events(m_widget, GDK_PROPERTY_CHANGE_MASK);
        g_signal_connect(m_widget, "property_notify_event",
            G_CALLBACK(property_notify_event), this);
    }
#endif // GDK_WINDOWING_X11

    // translate wx decorations styles into WM hints

    // always enable moving the window as we have no separate flag for enabling
    // it
    m_gdkFunc = GDK_FUNC_MOVE;

    if ( style & wxCLOSE_BOX )
        m_gdkFunc |= GDK_FUNC_CLOSE;

    if ( style & wxMINIMIZE_BOX )
        m_gdkFunc |= GDK_FUNC_MINIMIZE;

    if ( style & wxMAXIMIZE_BOX )
        m_gdkFunc |= GDK_FUNC_MAXIMIZE;

    if ( (style & wxSIMPLE_BORDER) || (style & wxNO_BORDER) )
    {
        m_gdkDecor = 0;
        gtk_window_set_decorated(GTK_WINDOW(m_widget), false);
    }
    else // have border
    {
        m_gdkDecor = GDK_DECOR_BORDER;

        if ( style & wxCAPTION )
            m_gdkDecor |= GDK_DECOR_TITLE;
#if GTK_CHECK_VERSION(3,10,0)
        else if (
            wxGTKImpl::IsWayland(display) &&
            gtk_check_version(3,10,0) == nullptr)
        {
            gtk_window_set_titlebar(GTK_WINDOW(m_widget), gtk_header_bar_new());
        }
#endif

        if ( style & wxSYSTEM_MENU )
            m_gdkDecor |= GDK_DECOR_MENU;

        if ( style & wxMINIMIZE_BOX )
            m_gdkDecor |= GDK_DECOR_MINIMIZE;

        if ( style & wxMAXIMIZE_BOX )
            m_gdkDecor |= GDK_DECOR_MAXIMIZE;

        if ( style & wxRESIZE_BORDER )
        {
           m_gdkFunc |= GDK_FUNC_RESIZE;
           m_gdkDecor |= GDK_DECOR_RESIZEH;
        }
    }

    m_decorSize = GetCachedDecorSize();
    int w = m_width;
    int h = m_height;

    if (style & wxRESIZE_BORDER)
    {
        GTKDoGetSize(&w, &h);
        gtk_window_set_default_size(GTK_WINDOW(m_widget), w, h);
#ifndef __WXGTK3__
        gtk_window_set_policy(GTK_WINDOW(m_widget), 1, 1, 1);
#endif
    }
    else
    {
        gtk_window_set_resizable(GTK_WINDOW(m_widget), false);
        // gtk_window_set_default_size() does not work for un-resizable windows,
        // unless you set the size hints, but that causes Ubuntu's WM to make
        // the window resizable even though GDK_FUNC_RESIZE is not set.
        if (!HasClientDecor(m_widget))
            GTKDoGetSize(&w, &h);
        gtk_widget_set_size_request(m_widget, w, h);
    }

    // Note that we need to connect after this signal in order to let the
    // normal g_signal_connect() in wxSystemSettings code to run before our
    // handler: this ensures that system settings cache is cleared before the
    // user-defined wxSysColourChangedEvent handlers using wxSystemSettings
    // methods are executed, which is important as otherwise they would use the
    // old colours etc.
    g_signal_connect_after(gtk_settings_get_default(), "notify::gtk-theme-name",
        G_CALLBACK(notify_gtk_theme_name), this);

    return true;
}

wxTopLevelWindowGTK::~wxTopLevelWindowGTK()
{
    if ( m_netFrameExtentsTimerId )
    {
        // Don't let the timer callback fire as the window pointer passed to it
        // will become invalid very soon.
        g_source_remove(m_netFrameExtentsTimerId);
    }

    if (m_grabbedEventLoop)
    {
        wxFAIL_MSG(wxT("Window still grabbed"));
        RemoveGrab();
    }

    SendDestroyEvent();

    // it may also be GtkScrolledWindow in the case of an MDI child
    if (GTK_IS_WINDOW(m_widget))
    {
        gtk_window_set_focus( GTK_WINDOW(m_widget), nullptr );
    }

    if (g_activeFrame == this)
        g_activeFrame = nullptr;

    g_signal_handlers_disconnect_by_func(
        gtk_settings_get_default(), (void*)notify_gtk_theme_name, this);
}

bool wxTopLevelWindowGTK::EnableCloseButton( bool enable )
{
    if (enable)
        m_gdkFunc |= GDK_FUNC_CLOSE;
    else
        m_gdkFunc &= ~GDK_FUNC_CLOSE;

    GdkWindow* window = gtk_widget_get_window(m_widget);
    if (window)
        gdk_window_set_functions(window, (GdkWMFunction)m_gdkFunc);

    return true;
}

bool wxTopLevelWindowGTK::ShowFullScreen(bool show, long)
{
    if (show == m_fsIsShowing)
        return false; // return what?

    // documented behaviour is to show the window if it's still hidden when
    // showing it full screen
    if (show)
        Show();

    m_fsIsShowing = show;

#if defined(GDK_WINDOWING_X11) && !defined(__WXGTK4__)
    GdkScreen* screen = gtk_widget_get_screen(m_widget);
    GdkDisplay* display = gdk_screen_get_display(screen);
    Display* xdpy = nullptr;
    Window xroot = None;
    wxX11FullScreenMethod method = wxX11_FS_WMSPEC;

#ifdef __WXGTK3__
    if (wxGTKImpl::IsX11(display))
#endif
    {
        xdpy = GDK_DISPLAY_XDISPLAY(display);
        xroot = GDK_WINDOW_XID(gdk_screen_get_root_window(screen));
        method = wxGetFullScreenMethodX11(xdpy, (WXWindow)xroot);
    }

    // NB: gtk_window_fullscreen() uses freedesktop.org's WMspec extensions
    //     to switch to fullscreen, which is not always available. We must
    //     check if WM supports the spec and use legacy methods if it
    //     doesn't.
    if ( method == wxX11_FS_WMSPEC )
#endif // GDK_WINDOWING_X11
    {
        if (show)
            gtk_window_fullscreen( GTK_WINDOW( m_widget ) );
        else
            gtk_window_unfullscreen( GTK_WINDOW( m_widget ) );
    }
#if defined(GDK_WINDOWING_X11) && !defined(__WXGTK4__)
    else if (xdpy != nullptr)
    {
        GdkWindow* window = gtk_widget_get_window(m_widget);
        Window xid = GDK_WINDOW_XID(window);

        if (show)
        {
            GetPosition( &m_fsSaveFrame.x, &m_fsSaveFrame.y );
            GetSize( &m_fsSaveFrame.width, &m_fsSaveFrame.height );

            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            const int screen_width = gdk_screen_get_width(screen);
            const int screen_height = gdk_screen_get_height(screen);
            wxGCC_WARNING_RESTORE()

            gint client_x, client_y, root_x, root_y;
            gint width, height;

            m_fsSaveGdkFunc = m_gdkFunc;
            m_fsSaveGdkDecor = m_gdkDecor;
            m_gdkFunc = m_gdkDecor = 0;
            gdk_window_set_decorations(window, (GdkWMDecoration)0);
            gdk_window_set_functions(window, (GdkWMFunction)0);

            gdk_window_get_origin(window, &root_x, &root_y);
            gdk_window_get_geometry(window, &client_x, &client_y, &width, &height);

            gdk_window_move_resize(
                window, -client_x, -client_y, screen_width + 1, screen_height + 1);

            wxSetFullScreenStateX11(xdpy,
                                    (WXWindow)xroot,
                                    (WXWindow)xid,
                                    show, &m_fsSaveFrame, method);
        }
        else // hide
        {
            m_gdkFunc = m_fsSaveGdkFunc;
            m_gdkDecor = m_fsSaveGdkDecor;
            gdk_window_set_decorations(window, (GdkWMDecoration)m_gdkDecor);
            gdk_window_set_functions(window, (GdkWMFunction)m_gdkFunc);

            wxSetFullScreenStateX11(xdpy,
                                    (WXWindow)xroot,
                                    (WXWindow)xid,
                                    show, &m_fsSaveFrame, method);

            SetSize(m_fsSaveFrame.x, m_fsSaveFrame.y,
                    m_fsSaveFrame.width, m_fsSaveFrame.height);
        }
    }
#endif // GDK_WINDOWING_X11

    return true;
}

// ----------------------------------------------------------------------------
// overridden wxWindow methods
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Refresh( bool WXUNUSED(eraseBackground), const wxRect *WXUNUSED(rect) )
{
    wxCHECK_RET( m_widget, wxT("invalid frame") );

    gtk_widget_queue_draw( m_widget );

    GdkWindow* window = nullptr;
    if (m_wxwindow)
        window = gtk_widget_get_window(m_wxwindow);
    if (window)
        gdk_window_invalidate_rect(window, nullptr, true);
}

#if defined(__WXGTK3__) && defined(GDK_WINDOWING_X11)
// Check conditions under which GTK will use Client Side Decorations with X11
static bool isUsingCSD(GtkWidget* widget)
{
    const char* csd = getenv("GTK_CSD");
    if (csd == nullptr || strcmp(csd, "1") != 0)
        return false;

    GdkScreen* screen = gtk_widget_get_screen(widget);
    if (!gdk_screen_is_composited(screen))
        return false;

    GdkAtom atom = gdk_atom_intern_static_string("_GTK_FRAME_EXTENTS");
    if (!gdk_x11_screen_supports_net_wm_hint(screen, atom))
        return false;

    if (gdk_screen_get_rgba_visual(screen) == nullptr)
        return false;

    return true;
}
#endif // __WXGTK3__ && GDK_WINDOWING_X11

bool wxTopLevelWindowGTK::Show( bool show )
{
    wxCHECK_MSG(m_widget, false, "invalid frame");

#ifdef GDK_WINDOWING_X11
    bool deferShow = show && !m_isShown && !m_isIconized && m_deferShow;
    if (deferShow)
    {
        GdkScreen* screen = gtk_widget_get_screen(m_widget);
        deferShow = m_deferShowAllowed &&
            // Assume size (from cache or wxPersistentTLW) is correct.
            // Avoids problems when WM initially provides an incorrect value
            // for extents, then corrects it later.
            m_decorSize.top == 0 &&

            gs_requestFrameExtentsStatus != RFE_STATUS_BROKEN &&
            !gtk_widget_get_realized(m_widget) &&
#ifdef __WXGTK3__
            wxGTKImpl::IsX11(screen) &&
#endif
            g_signal_handler_find(m_widget,
                GSignalMatchType(G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA),
                g_signal_lookup("property_notify_event", GTK_TYPE_WIDGET),
                0, nullptr, nullptr, this);
#ifdef __WXGTK3__
        // Don't defer with CSD, it isn't needed and causes pixman errors
        if (deferShow)
            deferShow = !isUsingCSD(m_widget);
#endif
        if (deferShow)
        {
            GdkAtom atom = gdk_atom_intern("_NET_REQUEST_FRAME_EXTENTS", false);
            deferShow = gdk_x11_screen_supports_net_wm_hint(screen, atom) != 0;

            // If _NET_REQUEST_FRAME_EXTENTS not supported, don't allow changes
            // to m_decorSize, it breaks saving/restoring window size with
            // GetSize()/SetSize() because it makes window bigger between each
            // restore and save.
            m_updateDecorSize = deferShow;
        }

        m_deferShow = deferShow;
    }
    if (deferShow)
    {
        // Initial show. If WM supports _NET_REQUEST_FRAME_EXTENTS, defer
        // calling gtk_widget_show() until _NET_FRAME_EXTENTS property
        // notification is received, so correct frame extents are known.
        // This allows resizing m_widget to keep the overall size in sync with
        // what wxWidgets expects it to be without an obvious change in the
        // window size immediately after it becomes visible.

        // Realize m_widget, so m_widget->window can be used. Realizing normally
        // causes the widget tree to be size_allocated, which generates size
        // events in the wrong order. However, the size_allocates will not be
        // done if the allocation is not the default (1,1).
        GtkAllocation alloc;
        gtk_widget_get_allocation(m_widget, &alloc);
        const int alloc_width = alloc.width;
        if (alloc_width == 1)
        {
            alloc.width = 2;
            gtk_widget_set_allocation(m_widget, &alloc);
        }
        gtk_widget_realize(m_widget);
        if (alloc_width == 1)
        {
            alloc.width = 1;
            gtk_widget_set_allocation(m_widget, &alloc);
        }

        // send _NET_REQUEST_FRAME_EXTENTS
        XClientMessageEvent xevent;
        memset(&xevent, 0, sizeof(xevent));
        xevent.type = ClientMessage;
        GdkWindow* window = gtk_widget_get_window(m_widget);
        xevent.window = GDK_WINDOW_XID(window);
        xevent.message_type = gdk_x11_atom_to_xatom_for_display(
            gdk_window_get_display(window),
            gdk_atom_intern("_NET_REQUEST_FRAME_EXTENTS", false));
        xevent.format = 32;
        Display* display = GDK_DISPLAY_XDISPLAY(gdk_window_get_display(window));
        XSendEvent(display, DefaultRootWindow(display), false,
            SubstructureNotifyMask | SubstructureRedirectMask,
            (XEvent*)&xevent);

        if (gs_requestFrameExtentsStatus == RFE_STATUS_UNKNOWN)
        {
            // if WM does not respond to request within 1 second,
            // we assume support for _NET_REQUEST_FRAME_EXTENTS is not working
            m_netFrameExtentsTimerId =
                g_timeout_add(1000, request_frame_extents_timeout, this);
        }

        // defer calling gtk_widget_show()
        m_isShown = true;
        return true;
    }
#endif // GDK_WINDOWING_X11

    if (show && !gtk_widget_get_realized(m_widget))
    {
        // size_allocate signals occur in reverse order (bottom to top).
        // Things work better if the initial wxSizeEvents are sent (from the
        // top down), before the initial size_allocate signals occur.
        SendSizeEvent();

#ifdef __WXGTK3__
        GTKSizeRevalidate();
#endif
    }

    bool change = base_type::Show(show);

#ifdef __WXGTK3__
    if (change && show)
    {
        // We may need to redo it after showing the window.
        GTKUpdateClientSizeIfNecessary();
    }

    GTKSendSizeEventIfNeeded();
#endif

    if (change && !show)
    {
        // Generate wxEVT_KILL_FOCUS for the currently focused control
        // immediately (i.e. without waiting until the window is destroyed and
        // doing it from its dtor), as it could be too late to execute the
        // handler for this event, or other events triggered by receiving it,
        // by then because the wxTLW will have been half-destroyed by then.
        if (GTK_IS_WINDOW(m_widget))
        {
            gtk_window_set_focus( GTK_WINDOW(m_widget), nullptr );
        }

        // make sure window has a non-default position, so when it is shown
        // again, it won't be repositioned by WM as if it were a new window
        // Note that this must be done _after_ the window is hidden.
        gtk_window_move((GtkWindow*)m_widget, m_x, m_y);
    }

    return change;
}

void wxTopLevelWindowGTK::ShowWithoutActivating()
{
    if (!m_isShown)
    {
        gtk_window_set_focus_on_map(GTK_WINDOW(m_widget), false);
        Show(true);
    }
}

void wxTopLevelWindowGTK::Raise()
{
    gtk_window_present( GTK_WINDOW( m_widget ) );
}

void wxTopLevelWindowGTK::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxTopLevelWindowGTK") );
}

// ----------------------------------------------------------------------------
// window geometry
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::GTKDoGetSize(int *width, int *height) const
{
    wxSize size(m_width, m_height);
    size.x -= m_decorSize.left + m_decorSize.right;
    size.y -= m_decorSize.top + m_decorSize.bottom;
    if (size.x < 0) size.x = 0;
    if (size.y < 0) size.y = 0;
    if (width)  *width  = size.x;
    if (height) *height = size.y;
}

void wxTopLevelWindowGTK::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxCHECK_RET( m_widget, wxT("invalid frame") );

    // deal with the position first
    int old_x = m_x;
    int old_y = m_y;

    if ( !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
    {
        // -1 means "use existing" unless the flag above is specified
        if ( x != -1 )
            m_x = x;
        if ( y != -1 )
            m_y = y;
    }
    else // wxSIZE_ALLOW_MINUS_ONE
    {
        m_x = x;
        m_y = y;
    }

    const wxSize oldSize(m_width, m_height);
    if (width >= 0)
        m_width = width;
    if (height >= 0)
        m_height = height;
    ConstrainSize();
    if (m_width < 1) m_width = 1;
    if (m_height < 1) m_height = 1;

    if ( m_x != old_x || m_y != old_y )
    {
        gtk_window_move( GTK_WINDOW(m_widget), m_x, m_y );
        wxMoveEvent event(wxPoint(m_x, m_y), GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }

    if (m_width != oldSize.x || m_height != oldSize.y)
    {
        wxLogTrace(TRACE_TLWSIZE, "Size set for %s (%d, %d) -> (%d, %d)",
                   wxDumpWindow(this), oldSize.x, oldSize.y, m_width, m_height);

        m_deferShowAllowed = true;
        m_useCachedClientSize = false;

#ifdef __WXGTK3__
        // Reset pending client size, it is not relevant any more and shouldn't
        // be set when the window is shown, as we don't want it to replace the
        // size explicitly specified here. Note that we do still want to set
        // the minimum client size, as increasing the total size shouldn't
        // allow shrinking the frame beyond its minimum fitting size.
        //
        // Also note that if we're called from WXSetInitialFittingClientSize()
        // itself, this will be overwritten again with the pending flags when
        // we return.
        m_pendingFittingClientSizeFlags &= ~wxSIZE_SET_CURRENT;
#endif // __WXGTK3__

        int w = m_width;
        int h = m_height;
        if (gtk_window_get_resizable(GTK_WINDOW(m_widget)))
        {
            GTKDoGetSize(&w, &h);
            gtk_window_resize(GTK_WINDOW(m_widget), w, h);
        }
        else
        {
            if (!HasClientDecor(m_widget))
                GTKDoGetSize(&w, &h);
            gtk_widget_set_size_request(GTK_WIDGET(m_widget), w, h);
        }

        DoGetClientSize(&m_clientWidth, &m_clientHeight);
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

extern "C" {
static gboolean reset_size_request(void* data)
{
    gtk_widget_set_size_request(GTK_WIDGET(data), -1, -1);
    g_object_unref(data);
    return false;
}
}

void wxTopLevelWindowGTK::DoSetClientSize(int width, int height)
{
    wxLogTrace(TRACE_TLWSIZE, "Client size set for %s: (%d, %d)",
               wxDumpWindow(this), width, height);

    base_type::DoSetClientSize(width, height);

    // Since client size is being explicitly set, don't change it later
    // Has to be done after calling base because it calls SetSize,
    // which sets this true
    m_deferShowAllowed = false;

    if (m_wxwindow)
    {
        // If window is not resizable or not yet shown, set size request on
        // client widget, to make it more likely window will get correct size
        // even if our decorations size cache is incorrect (as it will be before
        // showing first TLW).
        if (!gtk_window_get_resizable(GTK_WINDOW(m_widget)))
        {
            gtk_widget_set_size_request(m_widget, -1, -1);
            gtk_widget_set_size_request(m_wxwindow, m_clientWidth, m_clientHeight);
        }
        else if (!IsShown())
        {
            gtk_widget_set_size_request(m_wxwindow, m_clientWidth, m_clientHeight);
            // Cancel size request at next idle to allow resizing
            g_idle_add_full(G_PRIORITY_LOW - 1, reset_size_request, m_wxwindow, nullptr);
            g_object_ref(m_wxwindow);
        }
    }
}

void wxTopLevelWindowGTK::DoGetClientSize( int *width, int *height ) const
{
    wxCHECK_RET(m_widget, "invalid frame");

    if ( IsIconized() )
    {
        // for consistency with wxMSW, client area is supposed to be empty for
        // the iconized windows
        if ( width )
            *width = 0;
        if ( height )
            *height = 0;
    }
    else if (m_useCachedClientSize)
        base_type::DoGetClientSize(width, height);
    else
    {
        int w = m_width - (m_decorSize.left + m_decorSize.right);
        int h = m_height - (m_decorSize.top + m_decorSize.bottom);
        if (w < 0) w = 0;
        if (h < 0) h = 0;
        if (width) *width = w;
        if (height) *height = h;
    }
}

void wxTopLevelWindowGTK::DoSetSizeHints( int minW, int minH,
                                          int maxW, int maxH,
                                          int incW, int incH )
{
    wxLogTrace(TRACE_TLWSIZE, "Size hints for %s set to (%d, %d)",
               wxDumpWindow(this), minW, minH);

    base_type::DoSetSizeHints(minW, minH, maxW, maxH, incW, incH);

    if (!HasFlag(wxRESIZE_BORDER))
    {
        // It's not useful to set size hints for the windows which can't be
        // resized anyhow, and it even results in warnings from Gnome window
        // manager, so just don't do it (but notice that it's not an error to
        // call this for non-resizeable windows, e.g. because it's done
        // implicitly by SetSizerAndFit() which is useful even in this case).
        return;
    }

    m_incWidth = incW;
    m_incHeight = incH;

    const wxSize minSize = GetMinSize();
    const wxSize maxSize = GetMaxSize();
    GdkGeometry hints;
    // always set both min and max hints, otherwise GTK will
    // make assumptions we don't want about the unset values
    int hints_mask = GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE;
    hints.min_width = 1;
    hints.min_height = 1;
    // using INT_MAX for size will lead to integer overflow with HiDPI scaling
    hints.max_width = INT_MAX / 16;
    hints.max_height = INT_MAX / 16;
    int decorSize_x;
    int decorSize_y;
    if (HasClientDecor(m_widget))
    {
        decorSize_x = 0;
        decorSize_y = 0;
    }
    else
    {
        decorSize_x = m_decorSize.left + m_decorSize.right;
        decorSize_y = m_decorSize.top + m_decorSize.bottom;
    }
    if (minSize.x > decorSize_x)
        hints.min_width = minSize.x - decorSize_x;
    if (minSize.y > decorSize_y)
        hints.min_height = minSize.y - decorSize_y;
    if (maxSize.x > 0)
    {
        hints.max_width = maxSize.x - decorSize_x;
        if (hints.max_width < hints.min_width)
            hints.max_width = hints.min_width;
    }
    if (maxSize.y > 0)
    {
        hints.max_height = maxSize.y - decorSize_y;
        if (hints.max_height < hints.min_height)
            hints.max_height = hints.min_height;
    }
    if (incW > 0 || incH > 0)
    {
        hints_mask |= GDK_HINT_RESIZE_INC;
        hints.width_inc  = incW > 0 ? incW : 1;
        hints.height_inc = incH > 0 ? incH : 1;
    }
    gtk_window_set_geometry_hints(
        (GtkWindow*)m_widget, nullptr, &hints, (GdkWindowHints)hints_mask);
}

void wxTopLevelWindowGTK::GTKUpdateDecorSize(const DecorSize& decorSize)
{
    if (!IsMaximized() && !IsFullScreen())
        GetCachedDecorSize() = decorSize;

    if (HasClientDecor(m_widget))
    {
        if (m_decorSize.top == 0 && !gtk_widget_get_realized(m_widget) && m_deferShowAllowed)
        {
            // shrink m_widget by decoration size before initial show,
            // so that overall size remains correct
            const int w = wxMax(m_width  - decorSize.left - decorSize.right,  m_minWidth);
            const int h = wxMax(m_height - decorSize.top  - decorSize.bottom, m_minHeight);
            gtk_window_resize(GTK_WINDOW(m_widget), w, h);
            if (!gtk_window_get_resizable(GTK_WINDOW(m_widget)))
                gtk_widget_set_size_request(GTK_WIDGET(m_widget), w, h);
        }
        m_decorSize = decorSize;
        return;
    }
#ifdef GDK_WINDOWING_X11
    if (m_updateDecorSize && memcmp(&m_decorSize, &decorSize, sizeof(DecorSize)))
    {
        m_useCachedClientSize = false;
        const wxSize diff(
            decorSize.left - m_decorSize.left + decorSize.right - m_decorSize.right,
            decorSize.top - m_decorSize.top + decorSize.bottom - m_decorSize.bottom);
        m_decorSize = decorSize;
        bool resized = false;
        if (m_minWidth > 0 || m_minHeight > 0 || m_maxWidth > 0 || m_maxHeight > 0)
        {
            // update size hints, they depend on m_decorSize
            if (!m_deferShow)
            {
                // if size hints match old size, assume hints were set to
                // maintain current client size, and adjust hints accordingly
                if (m_minWidth == m_width) m_minWidth += diff.x;
                if (m_maxWidth == m_width) m_maxWidth += diff.x;
                if (m_minHeight == m_height) m_minHeight += diff.y;
                if (m_maxHeight == m_height) m_maxHeight += diff.y;
            }
            DoSetSizeHints(m_minWidth, m_minHeight, m_maxWidth, m_maxHeight, m_incWidth, m_incHeight);
        }
        if (m_deferShow)
        {
            // keep overall size unchanged by shrinking m_widget
            int w, h;
            GTKDoGetSize(&w, &h);
            // but not if size would be less than minimum, it won't take effect
            if (w >= m_minWidth - (decorSize.left + decorSize.right) &&
                h >= m_minHeight - (decorSize.top + decorSize.bottom))
            {
                gtk_window_resize(GTK_WINDOW(m_widget), w, h);
                if (!gtk_window_get_resizable(GTK_WINDOW(m_widget)))
                    gtk_widget_set_size_request(GTK_WIDGET(m_widget), w, h);
                resized = true;
            }
        }
        if (!resized)
        {
            // adjust overall size to match change in frame extents
            m_width  += diff.x;
            m_height += diff.y;
            if (m_width  < 1) m_width  = 1;
            if (m_height < 1) m_height = 1;
            m_clientWidth = 0;
            gtk_widget_queue_resize(m_wxwindow);
        }
    }
    if (m_deferShow)
    {
        // gtk_widget_show() was deferred, do it now
        m_deferShow = false;
        DoGetClientSize(&m_clientWidth, &m_clientHeight);
        SendSizeEvent();

#ifdef __WXGTK3__
        GTKSizeRevalidate();
#endif
        if (!m_isShown)
            return;

        gtk_widget_show(m_widget);

#ifdef __WXGTK3__
        GTKSendSizeEventIfNeeded();
#endif
        GTKDoAfterShow();
    }
#endif // GDK_WINDOWING_X11
}

wxTopLevelWindowGTK::DecorSize& wxTopLevelWindowGTK::GetCachedDecorSize()
{
    static DecorSize size[8];

    int index = 0;
    // title bar
    if (m_gdkDecor & (GDK_DECOR_MENU | GDK_DECOR_MINIMIZE | GDK_DECOR_MAXIMIZE | GDK_DECOR_TITLE))
        index = 1;
    // border
    if (m_gdkDecor & GDK_DECOR_BORDER)
        index |= 2;
    // utility window decor can be different
    if (m_windowStyle & wxFRAME_TOOL_WINDOW)
        index |= 4;
    return size[index];
}

void wxTopLevelWindowGTK::GTKDoAfterShow()
{
#ifdef __WXGTK3__
    // Set the client size again if necessary, we should be able to do it
    // correctly by now as the style cache should be up to date.
    GTKUpdateClientSizeIfNecessary();
#endif // __WXGTK3__

    wxShowEvent showEvent(GetId(), true);
    showEvent.SetEventObject(this);
    HandleWindowEvent(showEvent);
}

#ifdef __WXGTK3__

void wxTopLevelWindowGTK::GTKUpdateClientSizeIfNecessary()
{
    wxLogTrace(TRACE_TLWSIZE, "GTKUpdateClientSizeIfNecessary() for %s, pending=%d",
               wxDumpWindow(this), m_pendingFittingClientSizeFlags);

    if ( m_pendingFittingClientSizeFlags )
    {
        WXSetInitialFittingClientSize(m_pendingFittingClientSizeFlags);

        m_pendingFittingClientSizeFlags = 0;
    }
}

void wxTopLevelWindowGTK::SetMinSize(const wxSize& minSize)
{
    wxLogTrace(TRACE_TLWSIZE, "SetMinSize() for %s", wxDumpWindow(this));

    wxTopLevelWindowBase::SetMinSize(minSize);

    // Explicitly set minimum size should override the pending size, if any.
    m_pendingFittingClientSizeFlags &= ~wxSIZE_SET_MIN;
}

void
wxTopLevelWindowGTK::WXSetInitialFittingClientSize(int flags, wxSizer* sizer)
{
    wxLogTrace(TRACE_TLWSIZE, "WXSetInitialFittingClientSize(%d) for %s (%s)",
               flags, wxDumpWindow(this), IsShown() ? "shown" : "hidden");

    // In any case, update the size immediately.
    wxTopLevelWindowBase::WXSetInitialFittingClientSize(flags, sizer);

    // But if we're not shown yet, the fitting size may be wrong because GTK
    // style cache hasn't been updated yet and we need to do it again when the
    // window becomes visible as we can be sure that by then we'll be able to
    // compute the best size correctly.
    if ( !IsShown() )
        m_pendingFittingClientSizeFlags = flags;
}

#endif // __WXGTK3__

// ----------------------------------------------------------------------------
// frame title/icon
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::SetTitle( const wxString &title )
{
    wxCHECK_RET(m_widget, "invalid frame");

    if ( title == m_title )
        return;

    m_title = title;

    gtk_window_set_title( GTK_WINDOW(m_widget), title.utf8_str() );
}

void wxTopLevelWindowGTK::SetIcons( const wxIconBundle &icons )
{
    base_type::SetIcons(icons);

    // Setting icons before window is realized can cause a GTK assertion if
    // another TLW is realized before this one, and it has this one as its
    // transient parent. The life demo exibits this problem.
    if (m_widget && gtk_widget_get_realized(m_widget))
    {
        GList* list = nullptr;
        for (size_t i = icons.GetIconCount(); i--;)
            list = g_list_prepend(list, icons.GetIconByIndex(i).GetPixbuf());
        gtk_window_set_icon_list(GTK_WINDOW(m_widget), list);
        g_list_free(list);
    }
}

// ----------------------------------------------------------------------------
// frame state: maximized/iconized/normal
// ----------------------------------------------------------------------------

void wxTopLevelWindowGTK::Maximize(bool maximize)
{
    if (maximize)
        gtk_window_maximize( GTK_WINDOW( m_widget ) );
    else
        gtk_window_unmaximize( GTK_WINDOW( m_widget ) );
}

bool wxTopLevelWindowGTK::IsMaximized() const
{
    GdkWindow* window = nullptr;
    if (m_widget)
        window = gtk_widget_get_window(m_widget);
    return window && (gdk_window_get_state(window) & GDK_WINDOW_STATE_MAXIMIZED);
}

void wxTopLevelWindowGTK::Restore()
{
    // "Present" seems similar enough to "restore"
    gtk_window_present( GTK_WINDOW( m_widget ) );
}

void wxTopLevelWindowGTK::Iconize( bool iconize )
{
    if (iconize)
        gtk_window_iconify( GTK_WINDOW( m_widget ) );
    else
        gtk_window_deiconify( GTK_WINDOW( m_widget ) );
}

bool wxTopLevelWindowGTK::IsIconized() const
{
    return m_isIconized;
}

void wxTopLevelWindowGTK::SetIconizeState(bool iconize)
{
    if ( iconize != m_isIconized )
    {
        /*
            We get a configure-event _before_ the window is iconized with dummy
            (0, 0) coordinates. Unfortunately we can't just ignore this event
            when we get it, because we can't know if we're going to be iconized
            or not: even remembering that we should be soon in Iconize() itself
            doesn't help due to the crazy sequence of events generated by GTK,
            e.g. under X11 for the sequence of Iconize() and Show() calls we
            get the following events with GTK2:

            - window-state changes to GDK_WINDOW_STATE_ICONIFIED | WITHDRAWN
            - window-state changes to just GDK_WINDOW_STATE_ICONIFIED
            - map event
            - window-state changes to normal
            - configure event with normal size
            - window-state changes to GDK_WINDOW_STATE_ICONIFIED
            - configure event with normal size
            - configure event with (0, 0) size
            - window-state changes to normal (yes, again)
            - configure event with (0, 0) size
            - window-state changes to GDK_WINDOW_STATE_ICONIFIED

            So even though we could ignore the first (0, 0) configure event, we
            still wouldn't be able to ignore the second one, happening after
            the inexplicable switch to normal state.

            For completeness, with GTK3 the sequence of events is simpler, but
            still very unhelpful for our purposes:

            - window-state changes to GDK_WINDOW_STATE_ICONIFIED
            - window-state changes to normal
            - map event
            - configure event with normal size
            - configure event with normal size (yes, because one is not enough)
            - configure event with (0, 0) size
            - configure event with (0, 0) size (because why not have 2 of them)
            - window-state changes to GDK_WINDOW_STATE_ICONIFIED

            Here again we have (0, 0) configure events happening _after_ the
            window-state switch to normal, that would reset our flag.

            In conclusion, we have no choice but to assume that the window got
            really moved, but at least we can remember its previous position in
            order to restore it here if it turns out that it was just iconized.
         */
        if ( iconize )
        {
            if ( wxPoint(m_x, m_y) == wxPoint(0, 0) )
            {
                m_x = m_lastPos.x;
                m_y = m_lastPos.y;

                // This is not really necessary, but seems tidier.
                m_lastPos = wxPoint();
            }
        }

        m_isIconized = iconize;
        (void)SendIconizeEvent(iconize);
    }
}

void wxTopLevelWindowGTK::AddGrab()
{
    if (!m_grabbedEventLoop)
    {
        wxGUIEventLoop eventLoop;
        m_grabbedEventLoop = &eventLoop;
        gtk_grab_add( m_widget );
        eventLoop.Run();
        gtk_grab_remove( m_widget );
        m_grabbedEventLoop = nullptr;
    }
}

void wxTopLevelWindowGTK::RemoveGrab()
{
    if (m_grabbedEventLoop)
    {
        m_grabbedEventLoop->Exit();
        m_grabbedEventLoop = nullptr;
    }
}

bool wxTopLevelWindowGTK::IsGrabbed() const
{
    return m_grabbedEventLoop != nullptr;
}

bool wxTopLevelWindowGTK::IsActive()
{
    return (this == (wxTopLevelWindowGTK*)g_activeFrame);
}

void wxTopLevelWindowGTK::RequestUserAttention(int flags)
{
    bool new_hint_value = false;

    // FIXME: This is a workaround to focus handling problem
    // If RequestUserAttention is called for example right after a wxSleep, OnInternalIdle
    // hasn't yet been processed, and the internal focus system is not up to date yet.
    // YieldFor(wxEVT_CATEGORY_UI) ensures the processing of it (hopefully it
    // won't have side effects) - MR
    wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI);

    if(m_urgency_hint >= 0)
        g_source_remove(m_urgency_hint);

    m_urgency_hint = -2;

    if( gtk_widget_get_realized(m_widget) && !IsActive() )
    {
        new_hint_value = true;

        if (flags & wxUSER_ATTENTION_INFO)
        {
            m_urgency_hint = g_timeout_add(5000, (GSourceFunc)gtk_frame_urgency_timer_callback, this);
        } else {
            m_urgency_hint = -1;
        }
    }

    gtk_window_set_urgency_hint(GTK_WINDOW(m_widget), new_hint_value);
}

void wxTopLevelWindowGTK::SetWindowStyleFlag( long style )
{
    // Store which styles were changed
    long styleChanges = style ^ m_windowStyle;

    // Process wxWindow styles. This also updates the internal variable
    // Therefore m_windowStyle bits carry now the _new_ style values
    wxWindow::SetWindowStyleFlag(style);

    // just return for now if widget does not exist yet
    if (!m_widget)
        return;

    if ( styleChanges & wxSTAY_ON_TOP )
    {
        gtk_window_set_keep_above(GTK_WINDOW(m_widget),
                                  m_windowStyle & wxSTAY_ON_TOP);
    }

    if ( styleChanges & wxFRAME_NO_TASKBAR )
    {
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(m_widget),
                                         m_windowStyle & wxFRAME_NO_TASKBAR);
    }
}

bool wxTopLevelWindowGTK::SetTransparent(wxByte alpha)
{
    wxCHECK_MSG(m_widget, false, "invalid window");

#ifdef __WXGTK3__
    // RGBA visual is required
    GdkScreen* screen = gtk_widget_get_screen(m_widget);
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    if (visual == nullptr)
        return false;
    if (gtk_widget_get_visual(m_widget) != visual)
    {
        if (gtk_widget_get_realized(m_widget))
        {
            wxLogDebug("SetTransparent() must be called before Show()");
        }
        gtk_widget_set_visual(m_widget, visual);
    }
#if GTK_CHECK_VERSION(3,8,0)
    if (wx_is_at_least_gtk3(8))
    {
        gtk_widget_set_opacity(m_widget, alpha / 255.0);
    }
    else
#endif // GTK+ 3.8+
    {
#ifndef __WXGTK4__
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        gtk_window_set_opacity(GTK_WINDOW(m_widget), alpha / 255.0);
        wxGCC_WARNING_RESTORE()
#endif
    }

    return true;
#else // !__WXGTK3__

#if GTK_CHECK_VERSION(2,12,0)
    if (wx_is_at_least_gtk2(12))
    {
        gtk_window_set_opacity(GTK_WINDOW(m_widget), alpha / 255.0);
        return true;
    }
#endif // GTK_CHECK_VERSION(2,12,0)
#ifdef GDK_WINDOWING_X11
    GdkWindow* window = gtk_widget_get_window(m_widget);
    if (window == nullptr)
        return false;

    Display* dpy = GDK_WINDOW_XDISPLAY(window);
    Window win = GDK_WINDOW_XID(window);

    if (alpha == 0xff)
        XDeleteProperty(dpy, win, XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False));
    else
    {
        long opacity = alpha * 0x1010101L;
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False),
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) &opacity, 1L);
    }
    XSync(dpy, False);
    return true;
#else // !GDK_WINDOWING_X11
    return false;
#endif // GDK_WINDOWING_X11 / !GDK_WINDOWING_X11
#endif // __WXGTK3__/!__WXGTK3__
}

bool wxTopLevelWindowGTK::CanSetTransparent()
{
    // allow to override automatic detection as it's far from perfect
    const wxString SYSOPT_TRANSPARENT = "gtk.tlw.can-set-transparent";
    if ( wxSystemOptions::HasOption(SYSOPT_TRANSPARENT) )
    {
        return wxSystemOptions::GetOptionInt(SYSOPT_TRANSPARENT) != 0;
    }

#ifdef __WXGTK4__
    return gdk_display_is_composited(gtk_widget_get_display(m_widget)) != 0;
#else
#if GTK_CHECK_VERSION(2,10,0)
    if (wx_is_at_least_gtk2(10))
    {
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        return gtk_widget_is_composited(m_widget) != 0;
        wxGCC_WARNING_RESTORE()
    }
    else
#endif // In case of lower versions than gtk+-2.10.0 we could look for _NET_WM_CM_Sn ourselves
    {
        return false;
    }
#endif

#if 0 // Don't be optimistic here for the sake of wxAUI
    int opcode, event, error;
    // Check for the existence of a RGBA visual instead?
    return XQueryExtension(gdk_x11_get_default_xdisplay (),
                           "Composite", &opcode, &event, &error);
#endif
}

wxVisualAttributes wxTopLevelWindowGTK::GetDefaultAttributes() const
{
    wxVisualAttributes attrs(GetClassDefaultAttributes());
#ifdef __WXGTK3__
    wxGtkStyleContext().AddWindow().Bg(attrs.colBg);
#endif
    return attrs;
}
