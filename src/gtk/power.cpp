///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/power.cpp
// Purpose:     Power management functions for GTK/Linux.
// Author:      Vadim Zeitlin
// Created:     2006-05-27
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// This will define wxHAS_GLIB_POWER_SUPPORT if glib is new enough.
#include "wx/gtk/private/power.h"

#ifdef wxHAS_GLIB_POWER_SUPPORT

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/module.h"
#endif

#include "wx/power.h"

#include "wx/gtk/private/error.h"
#include "wx/gtk/private/object.h"
#include "wx/gtk/private/variant.h"

#include <gio/gio.h>

// Since 2.74, this header is included by the main GIO header, but we have to
// include it separately when using older versions.
#if !GLIB_CHECK_VERSION(2, 74, 0)
    #include <gio/gunixfdlist.h>
#endif

#include <atomic>
#include <memory>

// ----------------------------------------------------------------------------
// private declarations
// ----------------------------------------------------------------------------

namespace
{

constexpr int INVALID_FD = -1;

// Represents a D-Bus connection to the login manager.
class wxGDBusLoginManagerProxy
{
public:
    wxGDBusLoginManagerProxy()
        : m_proxyLoginManager(CreateProxyLoginManager())
    {
    }

    ~wxGDBusLoginManagerProxy()
    {
        if ( m_fdInhibit != INVALID_FD )
            close(m_fdInhibit);
    }

    bool Inhibit(const wxString& reason);

private:
    static GDBusProxy* CreateProxyLoginManager();

    wxGtkObject<GDBusProxy> m_proxyLoginManager;

    int m_fdInhibit = INVALID_FD;

    wxDECLARE_NO_COPY_CLASS(wxGDBusLoginManagerProxy);
};

// The global login manager proxy, non-null if g_powerResourceAcquired != 0.
std::unique_ptr<wxGDBusLoginManagerProxy> g_proxyLoginManager;

// Net number of times system power resource was acquired.
std::atomic<int> g_powerResourceAcquired{0};

// Ensure cleanup on exit.
class wxPowerInhibitorModule : public wxModule
{
public:
    bool OnInit() override { return true; }
    void OnExit() override
    {
        // There should be no other threads by now, so no locking is needed.
        if ( g_powerResourceAcquired )
        {
            g_proxyLoginManager.reset();

            g_powerResourceAcquired = 0;
        }
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxPowerInhibitorModule);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// real implementation
// ----------------------------------------------------------------------------

// static
GDBusProxy* wxGDBusLoginManagerProxy::CreateProxyLoginManager()
{
    wxGtkError error;
    GDBusProxy* const proxyLoginManager = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_PROXY_FLAGS_NONE,
        nullptr,
        "org.freedesktop.login1",
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        nullptr,
        error.Out()
    );

    if ( error )
    {
        wxLogError(
            _("Failed to open D-Bus connection to the login manager: %s"),
            error.GetMessage()
        );
    }

    return proxyLoginManager;
}

// Start inhibiting system suspend.
bool wxGDBusLoginManagerProxy::Inhibit(const wxString& reason)
{
    if ( !m_proxyLoginManager )
        return false;

    wxCHECK_MSG( m_fdInhibit == INVALID_FD, true, "Already inhibited" );

    // Provide user-readable description of the application inhibiting sleep or
    // shutdown.
    wxString who;
    if ( wxTheApp )
        who = wxTheApp->GetAppDisplayName();
    if ( who.empty() )
        who = _("wxWidgets application");

    // And also some user-readable reason for doing this.
    wxString why = reason;
    if ( why.empty() )
        why = _("Application needs to keep running");

    wxGtkObject<GUnixFDList> fd_list;
    wxGtkError error;
    const wxGtkVariant res{g_dbus_proxy_call_with_unix_fd_list_sync(
        m_proxyLoginManager,
        "Inhibit",
        g_variant_new(
            "(ssss)",
            "sleep:shutdown:idle",
            static_cast<const char*>(who.utf8_str()),
            static_cast<const char*>(why.utf8_str()),
            "block"
        ),
        G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT,               // No timeout.
        nullptr,                // No input FD list.
        fd_list.Out(),
        nullptr,                // No GCancellable.
        error.Out()
    )};

    if ( error )
    {
        wxLogError(_("Failed to inhibit sleep: %s"), error.GetMessage());
        return false;
    }

    if ( !fd_list || g_unix_fd_list_get_length(fd_list) != 1 )
    {
        wxLogError(_(R"(Unexpected response to D-Bus "Inhibit" request.)"));
        return false;
    }

    gint* const fds = g_unix_fd_list_steal_fds(fd_list, nullptr);
    m_fdInhibit = fds[0];
    g_free(fds);

    return true;
}

// ----------------------------------------------------------------------------
// public API
// ----------------------------------------------------------------------------

bool
wxPowerResource::Acquire(wxPowerResourceKind kind, const wxString& reason)
{
    switch ( kind )
    {
        case wxPOWER_RESOURCE_SCREEN:
            // This is not supported by GTK/systemd.
            break;

        case wxPOWER_RESOURCE_SYSTEM:
            if ( !g_powerResourceAcquired++ )
            {
                g_proxyLoginManager.reset(new wxGDBusLoginManagerProxy);
                if ( g_proxyLoginManager->Inhibit(reason) )
                    return true;

                g_proxyLoginManager.reset();
                g_powerResourceAcquired--;
            }
            break;
    }

    return false;
}

void wxPowerResource::Release(wxPowerResourceKind kind)
{
    switch ( kind )
    {
        case wxPOWER_RESOURCE_SCREEN:
            break;

        case wxPOWER_RESOURCE_SYSTEM:
            if ( !--g_powerResourceAcquired )
                g_proxyLoginManager.reset();
            break;
    }
}

#endif // wxHAS_GLIB_POWER_SUPPORT
