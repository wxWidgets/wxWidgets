/////////////////////////////////////////////////////////////////////////
// File:        src/gtk/taskbar.cpp
// Purpose:     wxTaskBarIcon
// Author:      Vaclav Slavik
// Modified by: Paul Cornett
// Created:     2004/05/29
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TASKBARICON

#include "wx/taskbar.h"

#ifndef __WXGTK4__

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
    #include "wx/menu.h"
    #include "wx/icon.h"
    #include "wx/filename.h"
#endif

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/backend.h"
#ifdef GDK_WINDOWING_X11
    #include <gdk/gdkx.h>
#endif
#ifndef __WXGTK3__
    #include "eggtrayicon.h"
#endif

#if wxUSE_APPINDICATOR
    #include <libayatana-appindicator/app-indicator.h>

    #include "wx/app.h"
    #include "wx/filename.h"
    #include "wx/log.h"
    #include "wx/stdpaths.h"

    #include "wx/gtk/private/error.h"
    #include "wx/gtk/private/object.h"
    #include "wx/gtk/private/variant.h"

    static constexpr const char* TRACE_APPINDICATOR = "appindicator";
#endif // wxUSE_APPINDICATOR

wxGCC_WARNING_SUPPRESS(deprecated-declarations)

#if !GTK_CHECK_VERSION(2,10,0)
    typedef struct _GtkStatusIcon GtkStatusIcon;
#endif

GdkWindow* wxGetTopLevelGDK();

#if wxUSE_APPINDICATOR

class TempIconFile
{
public:
    TempIconFile() = default;

    ~TempIconFile()
    {
        if ( Cleanup() )
            wxFileName::Rmdir(m_path.GetPath(), wxPATH_RMDIR_PARENTS);
    }

    // Each time this function is called, it returns path to a new file: this
    // is needed because AppIndicator doesn't update the icon if the file name
    // is the same as before, even if the file contents have changed.
    wxFileName GetNewIconPath()
    {
        Cleanup();

        wxString dir = wxString::FromUTF8(g_get_user_cache_dir());
        dir = wxStandardPaths::Get().AppendAppInfo(dir);
        wxFileName::Mkdir(dir, 0700, wxPATH_MKDIR_FULL);

        m_path.Assign(dir, wxString::Format("taskbaricon-%d.png", ++m_counter));

        return m_path;
    }

private:
    bool Cleanup()
    {
       return m_path.IsOk() && wxRemoveFile(m_path.GetFullPath());
    }

    wxFileName m_path;

    int m_counter = 0;

    wxDECLARE_NO_COPY_CLASS(TempIconFile);
};

#endif // wxUSE_APPINDICATOR

class wxTaskBarIcon::Private
{
public:
    Private(wxTaskBarIcon* taskBarIcon);
    ~Private();
    void SetIcon();
    void size_allocate(int width, int height);

    // owning wxTaskBarIcon
    wxTaskBarIcon* m_taskBarIcon;
    // used when GTK+ >= 2.10
    GtkStatusIcon* m_statusIcon;
    // for PopupMenu
    wxWindow* m_win;
    wxBitmapBundle m_bitmap;
    wxString m_tipText;

#if wxUSE_APPINDICATOR
    wxGtkObject<AppIndicator> m_appIndicator;

    // This pointer is non-null only if we own the app indicator menu,
    // otherwise we don't bother to store it.
    std::unique_ptr<wxMenu> m_appIndicatorMenu;

    // Temporary file created to hold the icon image for AppIndicator. It is
    // deleted when the icon is changed or the taskbar icon is destroyed.
    TempIconFile m_iconFile;
#endif // wxUSE_APPINDICATOR

#ifndef __WXGTK3__
    // used when GTK+ < 2.10
    GtkWidget* m_eggTrayIcon;
    // for tooltip when GTK+ < 2.10
    GtkTooltips* m_tooltips;
    // width and height of available space, only used when GTK+ < 2.10
    int m_size;
#endif
};
//-----------------------------------------------------------------------------

extern "C" {
#ifndef __WXGTK3__
static void
icon_size_allocate(GtkWidget*, GtkAllocation* alloc, wxTaskBarIcon::Private* priv)
{
    priv->size_allocate(alloc->width, alloc->height);
}

static void
icon_destroy(GtkWidget*, wxTaskBarIcon::Private* priv)
{
    // Icon window destroyed, probably because tray program has died.
    // Recreate icon so it will appear if tray program is restarted.
    priv->m_eggTrayIcon = nullptr;
    priv->SetIcon();
}
#endif

static void
icon_activate(void*, wxTaskBarIcon* taskBarIcon)
{
    // activate occurs from single click with GTK+
    wxTaskBarIconEvent event(wxEVT_TASKBAR_LEFT_DOWN, taskBarIcon);
    if (!taskBarIcon->SafelyProcessEvent(event))
    {
        // if single click not handled, send double click for compatibility
        event.SetEventType(wxEVT_TASKBAR_LEFT_DCLICK);
        taskBarIcon->SafelyProcessEvent(event);
    }
}

#if wxUSE_APPINDICATOR
static void
appindicator_activate(AppIndicator*, gint, gint, wxTaskBarIcon* taskBarIcon)
{
    // "activate" signal only fires for a genuine double-click (the status
    // notifier watcher itself decides single-click/right-click go to the menu
    // instead), so map it straight to LEFT_DCLICK rather than trying LEFT_DOWN
    // first the way icon_activate() does for a real single click.
    wxTaskBarIconEvent event(wxEVT_TASKBAR_LEFT_DCLICK, taskBarIcon);
    taskBarIcon->SafelyProcessEvent(event);
}
#endif // wxUSE_APPINDICATOR

static gboolean
icon_popup_menu(GtkWidget*, wxTaskBarIcon* taskBarIcon)
{
    wxTaskBarIconEvent event(wxEVT_TASKBAR_CLICK, taskBarIcon);
    taskBarIcon->SafelyProcessEvent(event);
    return true;
}

#ifndef __WXGTK3__
static gboolean
icon_button_press_event(GtkWidget*, GdkEventButton* event, wxTaskBarIcon* taskBarIcon)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        if (event->button == 1)
            icon_activate(nullptr, taskBarIcon);
        else if (event->button == 3)
            icon_popup_menu(nullptr, taskBarIcon);
    }
    return false;
}
#endif

#if GTK_CHECK_VERSION(2,10,0)
static void
status_icon_popup_menu(GtkStatusIcon*, guint, guint, wxTaskBarIcon* taskBarIcon)
{
    icon_popup_menu(nullptr, taskBarIcon);
}
#endif
} // extern "C"
//-----------------------------------------------------------------------------

bool wxTaskBarIconBase::IsAvailable()
{
#if wxUSE_APPINDICATOR
    wxGtkError error;
    wxGtkObject<GDBusConnection>
        conn{g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, error.Out())};
    if ( !conn )
    {
        wxLogTrace(TRACE_APPINDICATOR, "Failed to connect to session bus: %s",
                   error.GetMessage());
        return false;
    }

    const wxGtkVariant res{g_dbus_connection_call_sync(
        conn,
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "NameHasOwner",
        g_variant_new("(s)", "org.kde.StatusNotifierWatcher"),
        G_VARIANT_TYPE("(b)"),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        nullptr,
        error.Out()
    )};

    if ( !res )
    {
        wxLogTrace(TRACE_APPINDICATOR, "NameHasOwner(org.kde.StatusNotifierWatcher) failed: %s",
                   error.GetMessage());
        return false;
    }

    gboolean hasOwner = FALSE;
    res.Get("(b)", &hasOwner);

    return hasOwner != FALSE;
#else // !wxUSE_APPINDICATOR
#ifdef GDK_WINDOWING_X11
#ifdef __WXGTK3__
    if (!wxGTKImpl::IsX11(nullptr))
        return false;
#endif

    char name[32];
    g_snprintf(name, sizeof(name), "_NET_SYSTEM_TRAY_S%d",
        gdk_x11_get_default_screen());
    Atom atom = gdk_x11_get_xatom_by_name(name);

    Window manager = XGetSelectionOwner(gdk_x11_get_default_xdisplay(), atom);

    return manager != None;
#else
    return true;
#endif
#endif // wxUSE_APPINDICATOR/!wxUSE_APPINDICATOR
}
//-----------------------------------------------------------------------------

wxTaskBarIcon::Private::Private(wxTaskBarIcon* taskBarIcon)
{
    m_taskBarIcon = taskBarIcon;
    m_statusIcon = nullptr;
    m_win = nullptr;
#ifndef __WXGTK3__
    m_eggTrayIcon = nullptr;
    m_tooltips = nullptr;
    m_size = 0;
#endif
}

wxTaskBarIcon::Private::~Private()
{
    if (m_statusIcon)
        g_object_unref(m_statusIcon);
#ifndef __WXGTK3__
    else if (m_eggTrayIcon)
    {
        g_signal_handlers_disconnect_by_func(m_eggTrayIcon, (void*)icon_destroy, this);
        gtk_widget_destroy(m_eggTrayIcon);
    }
#endif
    if (m_win)
    {
        m_win->PopEventHandler();
        m_win->Destroy();
    }
#ifndef __WXGTK3__
    if (m_tooltips)
    {
        gtk_object_destroy(GTK_OBJECT(m_tooltips));
        g_object_unref(m_tooltips);
    }
#endif
}

void wxTaskBarIcon::Private::SetIcon()
{
#if wxUSE_APPINDICATOR
    // Currently we always use the size appropriate for the main display scale
    // factor, but we really should create multiple files for all displays
    // scale factors and let the status notifier watcher pick the right one.
    const wxSize size = m_bitmap.GetPreferredBitmapSizeAtScale(
            gdk_window_get_scale_factor(wxGetTopLevelGDK())
        );
    const wxBitmap bmp = m_bitmap.GetBitmap(size);
    if (!bmp.IsOk())
        return;

    wxFileName fnIcon = m_iconFile.GetNewIconPath();
    if (!bmp.SaveFile(fnIcon.GetFullPath(), wxBITMAP_TYPE_PNG))
        return;

    if (!m_appIndicator)
    {
        // Class name is used as application ID under Wayland, so prefer to use
        // it if available.
        wxString appId = wxTheApp->GetClassName();
        if ( appId.empty() )
        {
            // Make the app ID unique to avoid conflicts with another instance
            // of the same application.
            appId = wxString::Format("%s-%lu", wxTheApp->GetAppName(), wxGetProcessId());
        }

        m_appIndicator = app_indicator_new(
            appId.utf8_str(),
            fnIcon.GetName().utf8_str(),
            APP_INDICATOR_CATEGORY_APPLICATION_STATUS
        );

        // The "activate" signal was only added in libayatana-appindicator
        // 0.6.0 (absent in 0.5.94 and earlier); connecting to a signal that
        // doesn't exist on the actual runtime library triggers a loud GLib
        // critical warning, so check for its existence first.
        if ( g_signal_lookup("activate", G_TYPE_FROM_INSTANCE(m_appIndicator.get())) != 0 )
        {
            g_signal_connect(m_appIndicator, "activate",
                G_CALLBACK(appindicator_activate), m_taskBarIcon);
        }

        app_indicator_set_icon_theme_path(m_appIndicator, fnIcon.GetPath().utf8_str());
    }

    app_indicator_set_icon_full(m_appIndicator, fnIcon.GetName().utf8_str(), "");

    // GetPopupMenu() is documented to return a menu that is kept alive and not
    // destroyed by the library, which is exactly what's needed for the
    // persistent GtkMenu the AppIndicator requires, so prefer to use it.
    wxMenu* menu = m_taskBarIcon->GetPopupMenu();
    if ( menu )
    {
        // We don't need the old menu, if we had it.
        m_appIndicatorMenu.reset(nullptr);
    }
    else
    {
        menu = m_taskBarIcon->CreatePopupMenu();
        m_appIndicatorMenu.reset(menu);
    }

    menu->SetEventHandler(m_taskBarIcon);

    app_indicator_set_menu(m_appIndicator, GTK_MENU(menu->m_menu));
    app_indicator_set_status(m_appIndicator, APP_INDICATOR_STATUS_ACTIVE);
    return;
#endif // wxUSE_APPINDICATOR

#if GTK_CHECK_VERSION(2,10,0)
    if (wx_is_at_least_gtk2(10))
    {
        int scale = 1;
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == nullptr)
        {
            scale = gdk_window_get_scale_factor(wxGetTopLevelGDK());
        }
#endif
        const wxSize size(m_bitmap.GetPreferredBitmapSizeAtScale(scale));
        const wxBitmap bmp(m_bitmap.GetBitmap(size));
        if (m_statusIcon)
            gtk_status_icon_set_from_pixbuf(m_statusIcon, bmp.GetPixbuf());
        else
        {
            m_statusIcon = gtk_status_icon_new_from_pixbuf(bmp.GetPixbuf());
            g_signal_connect(m_statusIcon, "activate",
                G_CALLBACK(icon_activate), m_taskBarIcon);
            g_signal_connect(m_statusIcon, "popup_menu",
                G_CALLBACK(status_icon_popup_menu), m_taskBarIcon);
        }
    }
    else
#endif
    {
#ifndef __WXGTK3__
        m_size = 0;
        const wxBitmap bmp(m_bitmap.GetBitmap(wxDefaultSize));
        if (m_eggTrayIcon)
        {
            GtkWidget* image = gtk_bin_get_child(GTK_BIN(m_eggTrayIcon));
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), bmp.GetPixbuf());
        }
        else
        {
            m_eggTrayIcon = GTK_WIDGET(egg_tray_icon_new("wxTaskBarIcon"));
            gtk_widget_add_events(m_eggTrayIcon, GDK_BUTTON_PRESS_MASK);
            g_signal_connect(m_eggTrayIcon, "size_allocate",
                G_CALLBACK(icon_size_allocate), this);
            g_signal_connect(m_eggTrayIcon, "destroy",
                G_CALLBACK(icon_destroy), this);
            g_signal_connect(m_eggTrayIcon, "button_press_event",
                G_CALLBACK(icon_button_press_event), m_taskBarIcon);
            g_signal_connect(m_eggTrayIcon, "popup_menu",
                G_CALLBACK(icon_popup_menu), m_taskBarIcon);
            GtkWidget* image = gtk_image_new_from_pixbuf(bmp.GetPixbuf());
            gtk_container_add(GTK_CONTAINER(m_eggTrayIcon), image);
            gtk_widget_show_all(m_eggTrayIcon);
        }
#endif
    }
#if wxUSE_TOOLTIPS
    const char *tip_text = nullptr;
    if (!m_tipText.empty())
        tip_text = m_tipText.utf8_str();

#if GTK_CHECK_VERSION(2,10,0)
    if (m_statusIcon)
    {
#if GTK_CHECK_VERSION(2,16,0)
        if (wx_is_at_least_gtk2(16))
            gtk_status_icon_set_tooltip_text(m_statusIcon, tip_text);
        else
#endif
        {
#ifndef __WXGTK3__
            gtk_status_icon_set_tooltip(m_statusIcon, tip_text);
#endif
        }
    }
    else
#endif // GTK_CHECK_VERSION(2,10,0)
    {
#ifndef __WXGTK3__
        if (tip_text && m_tooltips == nullptr)
        {
            m_tooltips = gtk_tooltips_new();
            g_object_ref(m_tooltips);
            gtk_object_sink(GTK_OBJECT(m_tooltips));
        }
        if (m_tooltips)
            gtk_tooltips_set_tip(m_tooltips, m_eggTrayIcon, tip_text, "");
#endif
    }
#endif // wxUSE_TOOLTIPS
}

#ifndef __WXGTK3__
void wxTaskBarIcon::Private::size_allocate(int width, int height)
{
    int size = height;
    EggTrayIcon* icon = EGG_TRAY_ICON(m_eggTrayIcon);
    if (egg_tray_icon_get_orientation(icon) == GTK_ORIENTATION_VERTICAL)
        size = width;
    if (m_size == size)
        return;
    m_size = size;
    const wxBitmap bmp(m_bitmap.GetBitmap(wxDefaultSize));
    int w = bmp.GetLogicalWidth();
    int h = bmp.GetLogicalHeight();
    if (w > size || h > size)
    {
        if (w > size) w = size;
        if (h > size) h = size;
        GdkPixbuf* pixbuf =
            gdk_pixbuf_scale_simple(bmp.GetPixbuf(), w, h, GDK_INTERP_BILINEAR);
        GtkImage* image = GTK_IMAGE(gtk_bin_get_child(GTK_BIN(m_eggTrayIcon)));
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    }
}
#endif
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType WXUNUSED(iconType))
{
    m_priv = new Private(this);
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    delete m_priv;
}

bool wxTaskBarIcon::SetIcon(const wxBitmapBundle& icon, const wxString& tooltip)
{
    m_priv->m_bitmap = icon;
    m_priv->m_tipText = tooltip;
    m_priv->SetIcon();
    return true;
}

bool wxTaskBarIcon::RemoveIcon()
{
#if wxUSE_APPINDICATOR
    if (m_priv->m_appIndicator)
    {
        app_indicator_set_status(m_priv->m_appIndicator, APP_INDICATOR_STATUS_PASSIVE);
        return true;
    }
#endif
    delete m_priv;
    m_priv = new Private(this);
    return true;
}

bool wxTaskBarIcon::IsIconInstalled() const
{
#if wxUSE_APPINDICATOR
    if (m_priv->m_appIndicator)
        return app_indicator_get_status(m_priv->m_appIndicator) == APP_INDICATOR_STATUS_ACTIVE;
#endif
#ifdef __WXGTK3__
    return m_priv->m_statusIcon != nullptr;
#else
    return m_priv->m_statusIcon || m_priv->m_eggTrayIcon;
#endif
}

bool wxTaskBarIcon::PopupMenu(wxMenu* menu)
{
    wxCHECK_MSG( menu, false, "menu must be valid" );

#if wxUSE_APPINDICATOR
    if (m_priv->m_appIndicator)
    {
        menu->SetEventHandler(this);
        app_indicator_set_menu(m_priv->m_appIndicator, GTK_MENU(menu->m_menu));

        return true;
    }
#endif // wxUSE_APPINDICATOR
#if wxUSE_MENUS
    if (m_priv->m_win == nullptr)
    {
        m_priv->m_win = new wxTopLevelWindow(
            nullptr, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, 0);
        m_priv->m_win->PushEventHandler(this);
    }
    wxPoint point(-1, -1);
#ifdef __WXUNIVERSAL__
    point = wxGetMousePosition();
#endif
    m_priv->m_win->PopupMenu(menu, point);
#endif // wxUSE_MENUS
    return true;
}

#else
wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType)
{
    m_priv = nullptr;
}

wxTaskBarIcon::~wxTaskBarIcon()
{
}

bool wxTaskBarIcon::SetIcon(const wxIcon&, const wxString&)
{
    return false;
}

bool wxTaskBarIcon::RemoveIcon()
{
    return false;
}

bool wxTaskBarIcon::IsIconInstalled() const
{
    return false;
}

bool wxTaskBarIcon::PopupMenu(wxMenu*)
{
    return false;
}
#endif // __WXGTK4__
#endif // wxUSE_TASKBARICON
