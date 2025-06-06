/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/webview_webkit2.cpp
// Purpose:     GTK WebKit2 backend for web view component
// Author:      Scott Talbert
// Copyright:   (c) 2017 Scott Talbert
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT2

// In Ubuntu 18.04 compiling glib.h with gcc 4.8 results in the warnings inside
// it, and disabling them temporarily using wxGCC_WARNING_SUPPRESS/RESTORE
// doesn't work (i.e. they're still given), so disable them globally here.
#if wxCHECK_GCC_VERSION(4, 8) && !wxCHECK_GCC_VERSION(4, 9)
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "wx/dir.h"
#include "wx/dynlib.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/stockitem.h"
#include "wx/private/webview.h"
#include "wx/gtk/webview_webkit.h"
#include "wx/gtk/control.h"
#include "wx/gtk/private.h"
#include "wx/filesys.h"
#include "wx/base64.h"
#include "wx/log.h"
#include "wx/gtk/private/webview_webkit2_extension.h"
#include "wx/gtk/private/string.h"
#include "wx/gtk/private/webkit.h"
#include "wx/gtk/private/error.h"
#include "wx/gtk/private/object.h"
#include "wx/gtk/private/variant.h"
#include "wx/private/jsscriptwrapper.h"
#include <webkit2/webkit2.h>
#include <JavaScriptCore/JSValueRef.h>
#include <JavaScriptCore/JSStringRef.h>

#if WEBKIT_CHECK_VERSION(2, 10, 0)
#define wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
#endif

#if WEBKIT_CHECK_VERSION(2, 16, 0)
#define wxHAVE_WEBKIT_EPHEMERAL_CONTEXT
#define wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER_CLEAR
#endif

// Function to check webkit version at runtime
bool wx_check_webkit_version(int major, int minor, int micro)
{
    const unsigned int version = webkit_get_major_version() * 10000 +
                                 webkit_get_minor_version() * 100 +
                                 webkit_get_micro_version();
    const unsigned int required = major * 10000 + minor * 100 + micro;
    return version >= required;
}

// Helper function to get string from Webkit JS result
bool wxGetStringFromJSResult(WebKitJavascriptResult* js_result, wxString* output)
{
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    JSGlobalContextRef context = webkit_javascript_result_get_global_context(js_result);
    JSValueRef value = webkit_javascript_result_get_value(js_result);

    wxGCC_WARNING_RESTORE(deprecated-declarations)

    JSValueRef exception = nullptr;
    wxJSStringRef js_value
                  (
                   JSValueIsObject(context, value)
                       ? JSValueCreateJSONString(context, value, 0, &exception)
                       : JSValueToStringCopy(context, value, &exception)
                  );

    if ( exception )
    {
        if ( output )
        {
            wxJSStringRef ex_value(JSValueToStringCopy(context, exception, nullptr));
            *output = ex_value.ToWxString();
        }

        return false;
    }

    if ( output != nullptr )
        *output = js_value.ToWxString();

    return true;
}

//-----------------------------------------------------------------------------
// wxWebViewWindowFeaturesWebKit
//-----------------------------------------------------------------------------
class wxWebViewWindowFeaturesWebKit : public wxWebViewWindowFeatures
{
public:
    wxWebViewWindowFeaturesWebKit(wxWebView* webViewCtrl, WebKitWebView *web_view):
        wxWebViewWindowFeatures(webViewCtrl)
    {
        m_properties = webkit_web_view_get_window_properties(web_view);
        webkit_window_properties_get_geometry(m_properties, &m_geometry);
        // Treat 0 as -1 to indicate that the value is not set
        if (m_geometry.width == 0)
            m_geometry.width = -1;
        if (m_geometry.height == 0)
            m_geometry.height = -1;
        if (m_geometry.x == 0)
            m_geometry.x = -1;
        if (m_geometry.y == 0)
            m_geometry.y = -1;
    }

    virtual wxPoint GetPosition() const override
    {
        return wxPoint(m_geometry.x, m_geometry.y);
    }

    virtual wxSize GetSize() const override
    {
        return wxSize(m_geometry.width, m_geometry.height);
    }

    virtual bool ShouldDisplayMenuBar() const override
    {
        return webkit_window_properties_get_toolbar_visible(m_properties);
    }

    virtual bool ShouldDisplayStatusBar() const override
    {
        return webkit_window_properties_get_statusbar_visible(m_properties);
    }

    virtual bool ShouldDisplayToolBar() const override
    {
        return webkit_window_properties_get_toolbar_visible(m_properties);
    }

    virtual bool ShouldDisplayScrollBars() const override
    {
        return webkit_window_properties_get_scrollbars_visible(m_properties);
    }

    GdkRectangle m_geometry;
    WebKitWindowProperties *m_properties;
};

wxWebViewNavigationActionFlags wxGetNavigationActionFlags(WebKitNavigationAction* navigation)
{
    wxWebViewNavigationActionFlags flags;
    switch (webkit_navigation_action_get_navigation_type(navigation))
    {
        case WEBKIT_NAVIGATION_TYPE_LINK_CLICKED:
        case WEBKIT_NAVIGATION_TYPE_FORM_SUBMITTED:
        case WEBKIT_NAVIGATION_TYPE_FORM_RESUBMITTED:
            flags = wxWEBVIEW_NAV_ACTION_USER;
            break;
        default:
            flags = wxWEBVIEW_NAV_ACTION_OTHER;
            break;
    }
    return flags;
}

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C"
{

static void
wxgtk_webview_webkit_load_changed(GtkWidget *,
                                  WebKitLoadEvent load_event,
                                  wxWebViewWebKit *webKitCtrl)
{
    wxString url = webKitCtrl->GetCurrentURL();

    wxString target; // TODO: get target (if possible)

    if (load_event == WEBKIT_LOAD_FINISHED)
    {
        webKitCtrl->m_busy = false;
        wxWebViewEvent event(wxEVT_WEBVIEW_LOADED,
                             webKitCtrl->GetId(),
                             url, target);
        event.SetEventObject(webKitCtrl);

        webKitCtrl->HandleWindowEvent(event);
    }
    else if (load_event == WEBKIT_LOAD_COMMITTED)
    {
        webKitCtrl->m_busy = true;
        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATED,
                             webKitCtrl->GetId(),
                             url, target);
        event.SetEventObject(webKitCtrl);

        webKitCtrl->HandleWindowEvent(event);
    }
}

static gboolean
wxgtk_webview_webkit_navigation(WebKitWebView *,
                                WebKitPolicyDecision *decision,
                                wxWebViewWebKit *webKitCtrl)
{
    WebKitNavigationPolicyDecision* navigation_decision = WEBKIT_NAVIGATION_POLICY_DECISION(decision);
    WebKitNavigationAction* action = webkit_navigation_policy_decision_get_navigation_action(navigation_decision);
    WebKitURIRequest* request = webkit_navigation_action_get_request(action);
    const gchar* uri = webkit_uri_request_get_uri(request);

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    wxString target = webkit_navigation_policy_decision_get_frame_name(navigation_decision);
    wxGCC_WARNING_RESTORE(deprecated-declarations)

    webKitCtrl->m_busy = true;

    wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING,
                         webKitCtrl->GetId(),
                         wxString::FromUTF8( uri ),
                         target,
                         wxGetNavigationActionFlags(action));
    event.SetEventObject(webKitCtrl);

    webKitCtrl->HandleWindowEvent(event);

    if (!event.IsAllowed())
    {
        webKitCtrl->m_busy = false;
        webkit_policy_decision_ignore(decision);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static gboolean
wxgtk_webview_webkit_load_failed(WebKitWebView *,
                                 WebKitLoadEvent,
                                 gchar *uri,
                                 GError *error,
                                 wxWebViewWebKit* webKitWindow)
{
    webKitWindow->m_busy = false;
    wxWebViewNavigationError type = wxWEBVIEW_NAV_ERR_OTHER;

    if (strcmp(g_quark_to_string(error->domain), "soup_http_error_quark") == 0)
    {
        switch (error->code)
        {
#if SOUP_MAJOR_VERSION < 3
            case SOUP_STATUS_CANCELLED:
                type = wxWEBVIEW_NAV_ERR_USER_CANCELLED;
                break;

            case SOUP_STATUS_CANT_RESOLVE:
#endif
            case SOUP_STATUS_NOT_FOUND:
                type = wxWEBVIEW_NAV_ERR_NOT_FOUND;
                break;

#if SOUP_MAJOR_VERSION < 3
            case SOUP_STATUS_CANT_RESOLVE_PROXY:
            case SOUP_STATUS_CANT_CONNECT:
            case SOUP_STATUS_CANT_CONNECT_PROXY:
            case SOUP_STATUS_SSL_FAILED:
            case SOUP_STATUS_IO_ERROR:
                type = wxWEBVIEW_NAV_ERR_CONNECTION;
                break;

            case SOUP_STATUS_MALFORMED:
                type = wxWEBVIEW_NAV_ERR_REQUEST;
                break;
#endif

            case SOUP_STATUS_BAD_REQUEST:
                type = wxWEBVIEW_NAV_ERR_REQUEST;
                break;

            case SOUP_STATUS_UNAUTHORIZED:
            case SOUP_STATUS_FORBIDDEN:
                type = wxWEBVIEW_NAV_ERR_AUTH;
                break;

            case SOUP_STATUS_METHOD_NOT_ALLOWED:
            case SOUP_STATUS_NOT_ACCEPTABLE:
                type = wxWEBVIEW_NAV_ERR_SECURITY;
                break;

            case SOUP_STATUS_PROXY_AUTHENTICATION_REQUIRED:
                type = wxWEBVIEW_NAV_ERR_AUTH;
                break;

            case SOUP_STATUS_REQUEST_TIMEOUT:
                type = wxWEBVIEW_NAV_ERR_CONNECTION;
                break;

            case SOUP_STATUS_REQUEST_ENTITY_TOO_LARGE:
            case SOUP_STATUS_REQUEST_URI_TOO_LONG:
            case SOUP_STATUS_UNSUPPORTED_MEDIA_TYPE:
                type = wxWEBVIEW_NAV_ERR_REQUEST;
                break;

            case SOUP_STATUS_BAD_GATEWAY:
            case SOUP_STATUS_SERVICE_UNAVAILABLE:
            case SOUP_STATUS_GATEWAY_TIMEOUT:
                type = wxWEBVIEW_NAV_ERR_CONNECTION;
                break;

            case SOUP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
                type = wxWEBVIEW_NAV_ERR_REQUEST;
                break;
        }
    }
    else if (strcmp(g_quark_to_string(error->domain),
                    "webkit-network-error-quark") == 0)
    {
        switch (error->code)
        {
            case WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL:
                type = wxWEBVIEW_NAV_ERR_REQUEST;
                break;

            case WEBKIT_NETWORK_ERROR_CANCELLED:
                type = wxWEBVIEW_NAV_ERR_USER_CANCELLED;
                break;

            case WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST:
                type = wxWEBVIEW_NAV_ERR_NOT_FOUND;
                break;
        }
    }
    else if (strcmp(g_quark_to_string(error->domain),
                    "webkit-policy-error-quark") == 0)
    {
        switch (error->code)
        {
            case WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT:
                type = wxWEBVIEW_NAV_ERR_SECURITY;
                break;
        }
    }

    wxWebViewEvent event(wxEVT_WEBVIEW_ERROR,
                         webKitWindow->GetId(),
                         uri, "");
    event.SetEventObject(webKitWindow);
    event.SetString(wxString::FromUTF8(error->message));
    event.SetInt(type);


    webKitWindow->HandleWindowEvent(event);

    return FALSE;
}

static gboolean
wxgtk_webview_webkit_enter_fullscreen(WebKitWebView *WXUNUSED(web_view),
                                      wxWebViewWebKit *webKitCtrl)
{
    wxWebViewEvent event(wxEVT_WEBVIEW_FULLSCREEN_CHANGED,
                                       webKitCtrl->GetId(),
                                       wxString(),
                                       wxString());
    event.SetEventObject(webKitCtrl);
    event.SetInt(1);
    webKitCtrl->HandleWindowEvent(event);

    return FALSE;
}

static gboolean
wxgtk_webview_webkit_leave_fullscreen(WebKitWebView *WXUNUSED(web_view),
                                      wxWebViewWebKit *webKitCtrl)
{
    wxWebViewEvent event(wxEVT_WEBVIEW_FULLSCREEN_CHANGED,
                                       webKitCtrl->GetId(),
                                       wxString(),
                                       wxString());
    event.SetEventObject(webKitCtrl);
    event.SetInt(0);
    webKitCtrl->HandleWindowEvent(event);

    return FALSE;
}

static void
wxgtk_webview_webkit_script_message_received(WebKitUserContentManager *WXUNUSED(content_manager),
                                             WebKitJavascriptResult *js_result,
                                             wxWebViewWebKit *webKitCtrl)
{
    wxWebViewEvent event(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED,
                         webKitCtrl->GetId(),
                         webKitCtrl->GetCurrentURL(),
                         "");
    wxString msgStr;
    if (wxGetStringFromJSResult(js_result, &msgStr))
        event.SetString(msgStr);
    webKitCtrl->HandleWindowEvent(event);
}

static void wxgtk_webview_webkit_close (WebKitWebView *WXUNUSED(web_view),
                                        wxWebViewWebKit *webKitCtrl)
{
    wxWebViewEvent event(wxEVT_WEBVIEW_WINDOW_CLOSE_REQUESTED,
                         webKitCtrl->GetId(),
                         webKitCtrl->GetCurrentURL(),
                         "");
    webKitCtrl->HandleWindowEvent(event);
}

class wxReadyToShowParams
{
public:
    wxWebViewWebKit* childWebView;
    wxWebViewWebKit* parentWebView;
};

static void wxgtk_webview_webkit_ready_to_show (WebKitWebView *web_view,
                                                wxReadyToShowParams *params)
{
    wxWebViewWindowFeaturesWebKit features(params->childWebView, web_view);
    wxWebViewEvent event(wxEVT_WEBVIEW_NEWWINDOW_FEATURES,
                         params->parentWebView->GetId(),
                         params->childWebView->GetCurrentURL(),
                         "");
    event.SetEventObject(params->parentWebView);
    event.SetClientData(&features);
    params->parentWebView->HandleWindowEvent(event);
    delete params;
}

static gboolean
wxgtk_webview_webkit_decide_policy(WebKitWebView *web_view,
                                   WebKitPolicyDecision *decision,
                                   WebKitPolicyDecisionType type,
                                   wxWebViewWebKit *webKitCtrl)
{
    switch (type)
    {
        case WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION:
            return wxgtk_webview_webkit_navigation(web_view, decision, webKitCtrl);
        default:
            return FALSE;
    }
}

static void
wxgtk_webview_webkit_title_changed(GtkWidget* widget,
                                   GParamSpec *,
                                   wxWebViewWebKit *webKitCtrl)
{
    wxGlibPtr<gchar> title;
    g_object_get(G_OBJECT(widget), "title", title.Out(), nullptr);

    wxWebViewEvent event(wxEVT_WEBVIEW_TITLE_CHANGED,
                         webKitCtrl->GetId(),
                         webKitCtrl->GetCurrentURL(),
                         "");
    event.SetEventObject(webKitCtrl);
    event.SetString(wxString::FromUTF8(title));

    webKitCtrl->HandleWindowEvent(event);
}

static void
wxgtk_webview_webkit_uri_scheme_request_cb(WebKitURISchemeRequest *request,
                                           wxWebViewWebKit *webKitCtrl)
{
    const wxString scheme = wxString::FromUTF8(webkit_uri_scheme_request_get_scheme(request));

    wxSharedPtr<wxWebViewHandler> handler;
    wxVector<wxSharedPtr<wxWebViewHandler> > handlers = webKitCtrl->GetHandlers();

    for(wxVector<wxSharedPtr<wxWebViewHandler> >::iterator it = handlers.begin();
        it != handlers.end(); ++it)
    {
        if(scheme == (*it)->GetName())
        {
            handler = (*it);
        }
    }

    if(handler)
    {
        const wxString uri = wxString::FromUTF8(webkit_uri_scheme_request_get_uri(request));

        wxFSFile* file = handler->GetFile(uri);
        if(file)
        {
            gint64 length = file->GetStream()->GetLength();
            guint8 *data = g_new(guint8, length);
            file->GetStream()->Read(data, length);
            GInputStream *stream = g_memory_input_stream_new_from_data(data,
                                                                       length,
                                                                       g_free);
            wxString mime = file->GetMimeType();
            webkit_uri_scheme_request_finish(request, stream, length, mime.utf8_str());
        }
        else
        {
            wxGtkError error(g_error_new(WEBKIT_NETWORK_ERROR,
                                         WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST,
                                         "File not found: %s", uri.utf8_str().data()));
            webkit_uri_scheme_request_finish_error(request, error);
        }
    }
    else
    {
        wxGtkError error(g_error_new(WEBKIT_NETWORK_ERROR,
                                     WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL,
                                     "Unknown scheme: %s", scheme.utf8_str().data()));
        webkit_uri_scheme_request_finish_error(request, error);
    }
}

static gboolean
wxgtk_webview_webkit_context_menu(WebKitWebView *,
                                  WebKitContextMenu *,
                                  GdkEvent *,
                                  WebKitHitTestResult *,
                                  wxWebViewWebKit *webKitCtrl)
{
    return !webKitCtrl->IsContextMenuEnabled();
}

static WebKitWebView*
wxgtk_webview_webkit_create_webview(WebKitWebView *web_view,
                                    WebKitNavigationAction *navigation_action,
                                    wxWebViewWebKit *webKitCtrl)
{
    auto request = webkit_navigation_action_get_request(navigation_action);
    wxString url = wxString::FromUTF8(webkit_uri_request_get_uri(request));
    wxWebViewEvent event(wxEVT_WEBVIEW_NEWWINDOW,
                         webKitCtrl->GetId(),
                         url,
                         "",
                         wxGetNavigationActionFlags(navigation_action));
    event.SetEventObject(webKitCtrl);
    webKitCtrl->HandleWindowEvent(event);

    if ( event.IsAllowed() )
    {
        wxWebView* childWebView = new wxWebViewWebKit(web_view, webKitCtrl);
        return (WebKitWebView*)childWebView->GetNativeBackend();
    }
    else
        return nullptr;
}

static void
wxgtk_webview_webkit_counted_matches(WebKitFindController *,
                                     guint match_count,
                                     int *findCount)
{
    *findCount = match_count;
}

// This function checks if the specified directory contains our web extension.
static bool CheckDirectoryForWebExt(const wxString& dirname)
{
    wxDir dir;
    if ( !wxDir::Exists(dirname) || !dir.Open(dirname) )
        return false;

    wxString file;
    bool cont = dir.GetFirst
                    (
                        &file,
                        "webkit2_ext*" + wxDynamicLibrary::GetDllExt(wxDL_MODULE),
                        wxDIR_FILES
                    );
    while ( cont )
    {
        wxDynamicLibrary dl;
        if ( dl.Load(wxFileName(dirname, file).GetFullPath(),
                     wxDL_VERBATIM | wxDL_LAZY) &&
                dl.HasSymbol("webkit_web_extension_initialize_with_user_data") )
        {
            // Looks like our extension.
            return true;
        }

        cont = dir.GetNext(&file);
    }

    return false;
}

static bool TrySetWebExtensionsDirectory(WebKitWebContext *context, const wxString& dir)
{
    if (dir.empty() || !CheckDirectoryForWebExt(dir))
        return false;

    webkit_web_context_set_web_extensions_directory(context, dir.utf8_str());
    return true;
}

static wxString GetStandardWebExtensionsDir()
{
    wxString dir = wxDynamicLibrary::GetPluginsDirectory();
    if ( !dir.empty() )
        dir += "/web-extensions";
    return dir;
}

static void
wxgtk_initialize_web_extensions(WebKitWebContext *context,
                                GDBusServer *dbusServer)
{
    const char *address = g_dbus_server_get_client_address(dbusServer);
    wxGtkVariant user_data(g_variant_new("(s)", address));

    // Try to setup extension loading from the location it is supposed to be
    // normally installed in.
    if ( !TrySetWebExtensionsDirectory(context, GetStandardWebExtensionsDir()) )
    {
        // These relative locations are used as fallbacks to allow running
        // the tests and sample using wxWebView before installing it.
        wxString exepath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
        if ( !exepath.empty() )
        {
            wxString const directories[] =
            {
                exepath + "/..",
                exepath + "/../..",
                exepath + "/lib",
            };

            for ( size_t n = 0; n < WXSIZEOF(directories); ++n )
            {
                if ( TrySetWebExtensionsDirectory(context, directories[n]) )
                    break;
            }
        }
    }

    webkit_web_context_set_web_extensions_initialization_user_data(context,
                                                                   user_data.Release());
}

static gboolean
wxgtk_new_connection_cb(GDBusServer *,
                        GDBusConnection *connection,
                        GDBusProxy **proxy)
{
    GError *error = nullptr;
    GDBusProxyFlags flags = GDBusProxyFlags(G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS);
    *proxy = g_dbus_proxy_new_sync(connection,
                                   flags,
                                   nullptr,
                                   nullptr,
                                   WXGTK_WEB_EXTENSION_OBJECT_PATH,
                                   WXGTK_WEB_EXTENSION_INTERFACE,
                                   nullptr,
                                   &error);
    if (error)
    {
        g_warning("Failed to create dbus proxy: %s", error->message);
        g_error_free(error);
    }

    return TRUE;
}

static
gboolean
wxgtk_dbus_peer_is_authorized(GCredentials *peer_credentials)
{
    static GCredentials *own_credentials = g_credentials_new();
    GError *error = nullptr;

    if (peer_credentials && g_credentials_is_same_user(peer_credentials, own_credentials, &error))
    {
        return TRUE;
    }

    if (error)
    {
        g_warning("Failed to authorize web extension connection: %s", error->message);
        g_error_free(error);
    }
    return FALSE;
}

static gboolean
wxgtk_authorize_authenticated_peer_cb(GDBusAuthObserver *,
                                      GIOStream *,
                                      GCredentials *credentials,
                                      wxWebViewWebKit *)
{
    return wxgtk_dbus_peer_is_authorized(credentials);
}

} // extern "C"

//-----------------------------------------------------------------------------
// wxWebViewConfigurationImplWebKit
//-----------------------------------------------------------------------------
class wxWebViewConfigurationImplWebKit : public wxWebViewConfigurationImpl
{
public:

#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
    wxString GetDataPath() const override
    {
        GetOrCreateContext();
        if (m_websiteDataManager)
            return webkit_website_data_manager_get_base_data_directory(m_websiteDataManager);
        else
            return wxString{};
    }

    void SetDataPath(const wxString& path) override
    {
        wxASSERT_MSG(!m_webContext, "Cannot set data path after web context has been created");
        m_dataPath = path;
    }
#endif

#ifdef wxHAVE_WEBKIT_EPHEMERAL_CONTEXT
    virtual bool EnablePersistentStorage(bool enable) override
    {
        if (wx_check_webkit_version(2, 16, 0))
        {
            m_persistentStorage = enable;
            return true;
        }
        else
            return false;
    }
#endif

    virtual void* GetNativeConfiguration() const override
    {
        return GetOrCreateContext();
    }

#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
    WebKitWebsiteDataManager* GetWebsiteDataManager()
    {
        return m_websiteDataManager;
    }
#endif

private:
    wxString m_dataPath;
    mutable WebKitWebContext* m_webContext = nullptr;
#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
    mutable WebKitWebsiteDataManager* m_websiteDataManager = nullptr;
#endif
#ifdef wxHAVE_WEBKIT_EPHEMERAL_CONTEXT
    bool m_persistentStorage = true;
#endif

    WebKitWebContext* GetOrCreateContext() const
    {
        if (m_webContext)
            return m_webContext;

#ifdef wxHAVE_WEBKIT_EPHEMERAL_CONTEXT
        if (!m_persistentStorage)
        {
            m_webContext = webkit_web_context_new_ephemeral();
            return m_webContext;
        }
#endif

#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
        if (wx_check_webkit_version(2, 10, 0))
        {
            gchar* cachePath = nullptr;
            gchar* dataPath = nullptr;
            if (!m_dataPath.empty())
            {
                wxFileName configCachePath = wxFileName::DirName(m_dataPath);
                configCachePath.AppendDir("cache");
                cachePath = g_strdup(configCachePath.GetPath().utf8_str());
                wxFileName configDataPath = wxFileName::DirName(m_dataPath);
                configDataPath.AppendDir("data");
                dataPath = g_strdup(configDataPath.GetPath().utf8_str());
            }

            m_websiteDataManager = webkit_website_data_manager_new(
                "base-cache-directory", cachePath,
                "base-data-directory", dataPath,
                nullptr);
            m_webContext = webkit_web_context_new_with_website_data_manager(m_websiteDataManager);
        }
        else
#endif
            m_webContext = webkit_web_context_get_default();
        return m_webContext;
    }

};

//-----------------------------------------------------------------------------
// wxWebViewFactoryWebKit
//-----------------------------------------------------------------------------

wxVersionInfo wxWebViewFactoryWebKit::GetVersionInfo(wxVersionContext context)
{
    int major = 0,
        minor = 0,
        micro = 0;

    switch ( context )
    {
        case wxVersionContext::RunTime:
            major = webkit_get_major_version();
            minor = webkit_get_minor_version();
            micro = webkit_get_micro_version();
            break;

        case wxVersionContext::BuildTime:
            major = WEBKIT_MAJOR_VERSION;
            minor = WEBKIT_MINOR_VERSION;
            micro = WEBKIT_MICRO_VERSION;
            break;
    }

    return wxVersionInfo("webkit2", major, minor, micro);
}

wxWebViewConfiguration wxWebViewFactoryWebKit::CreateConfiguration()
{
    return wxWebViewConfiguration(wxWebViewBackendWebKit,
        new wxWebViewConfigurationImplWebKit());
}

wxWebView* wxWebViewFactoryWebKit::CreateWithConfig(const wxWebViewConfiguration& config)
{
    return new wxWebViewWebKit(config);
}

//-----------------------------------------------------------------------------
// wxWebViewWebKit
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewWebKit, wxWebView);

wxWebViewWebKit::wxWebViewWebKit():
    m_config(wxWebViewBackendWebKit, new wxWebViewConfigurationImplWebKit)
{
    m_web_view = nullptr;
    m_dbusServer = nullptr;
    m_extension = nullptr;
}

wxWebViewWebKit::wxWebViewWebKit(WebKitWebView* parentWebView, wxWebViewWebKit* parentWebViewCtrl):
    m_config(parentWebViewCtrl->m_config)
{
    m_web_view = (WebKitWebView*) webkit_web_view_new_with_related_view(parentWebView);
    m_dbusServer = nullptr;
    m_extension = nullptr;

    wxReadyToShowParams* params = new wxReadyToShowParams();
    params->childWebView = this;
    params->parentWebView = parentWebViewCtrl;

    g_signal_connect(m_web_view, "ready-to-show",
                     G_CALLBACK(wxgtk_webview_webkit_ready_to_show), params);
}

wxWebViewWebKit::wxWebViewWebKit(const wxWebViewConfiguration &config):
    m_config(config)
{
    m_web_view = nullptr;
    m_dbusServer = nullptr;
    m_extension = nullptr;
}

bool wxWebViewWebKit::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &url,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    m_dbusServer = nullptr;
    m_extension = nullptr;
    m_busy = false;
    m_guard = false;
    FindClear();

    bool isChildWebView = m_web_view != nullptr;

    // We currently unconditionally impose scrolling in both directions as it's
    // necessary to show arbitrary pages.
    style |= wxHSCROLL | wxVSCROLL;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxWebViewWebKit creation failed") );
        return false;
    }

    SetupWebExtensionServer();
    g_signal_connect(m_config.GetNativeConfiguration(),
                     "initialize-web-extensions",
                     G_CALLBACK(wxgtk_initialize_web_extensions),
                     m_dbusServer);

    if (!isChildWebView)
#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER
        m_web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(WEBKIT_WEB_CONTEXT(m_config.GetNativeConfiguration())));
#else
        m_web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
#endif
    GTKCreateScrolledWindowWith(GTK_WIDGET(m_web_view));
    g_object_ref(m_widget);

    if (!m_customUserAgent.empty())
        SetUserAgent(m_customUserAgent);

    g_signal_connect(m_web_view, "decide-policy",
                     G_CALLBACK(wxgtk_webview_webkit_decide_policy),
                     this);

    g_signal_connect(m_web_view, "load-failed",
                     G_CALLBACK(wxgtk_webview_webkit_load_failed), this);

    g_signal_connect(m_web_view, "notify::title",
                     G_CALLBACK(wxgtk_webview_webkit_title_changed), this);

    g_signal_connect(m_web_view, "context-menu",
                     G_CALLBACK(wxgtk_webview_webkit_context_menu), this);

    g_signal_connect(m_web_view, "create",
                     G_CALLBACK(wxgtk_webview_webkit_create_webview), this);

    g_signal_connect(m_web_view, "enter-fullscreen",
                     G_CALLBACK(wxgtk_webview_webkit_enter_fullscreen), this);

    g_signal_connect(m_web_view, "leave-fullscreen",
                     G_CALLBACK(wxgtk_webview_webkit_leave_fullscreen), this);

    g_signal_connect(m_web_view, "close",
                     G_CALLBACK(wxgtk_webview_webkit_close), this);

    WebKitFindController* findctrl = webkit_web_view_get_find_controller(m_web_view);
    g_signal_connect(findctrl, "counted-matches",
                     G_CALLBACK(wxgtk_webview_webkit_counted_matches),
                     &m_findCount);

    m_parent->DoAddChild( this );

    PostCreation(size);

    NotifyWebViewCreated();

    /* Open a webpage */
    if (!isChildWebView)
        webkit_web_view_load_uri(m_web_view, url.utf8_str());

    // last to avoid getting signal too early
    g_signal_connect(m_web_view, "load-changed",
                     G_CALLBACK(wxgtk_webview_webkit_load_changed),
                     this);

    return true;
}

wxWebViewWebKit::~wxWebViewWebKit()
{
    if (m_web_view)
        GTKDisconnect(m_web_view);
    if (m_dbusServer)
    {
        g_dbus_server_stop(m_dbusServer);
        g_signal_handlers_disconnect_by_data(
            m_config.GetNativeConfiguration(), m_dbusServer);
    }
    g_clear_object(&m_dbusServer);
    g_clear_object(&m_extension);
}

bool wxWebViewWebKit::Enable( bool enable )
{
    if (!wxControl::Enable(enable))
        return false;

    gtk_widget_set_sensitive(gtk_bin_get_child(GTK_BIN(m_widget)), enable);

    return true;
}

GdkWindow*
wxWebViewWebKit::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    GdkWindow* window = gtk_widget_get_parent_window(m_widget);
    return window;
}

void wxWebViewWebKit::ZoomIn()
{
    SetWebkitZoom(GetWebkitZoom() + 0.1f);
}

void wxWebViewWebKit::ZoomOut()
{
    SetWebkitZoom(GetWebkitZoom() - 0.1f);
}

void wxWebViewWebKit::SetWebkitZoom(float level)
{
    webkit_web_view_set_zoom_level(m_web_view, double(level));
}

float wxWebViewWebKit::GetWebkitZoom() const
{
    return float(webkit_web_view_get_zoom_level(m_web_view));
}

void wxWebViewWebKit::EnableAccessToDevTools(bool enable)
{
    WebKitSettings* settings = webkit_web_view_get_settings(m_web_view);
    webkit_settings_set_enable_developer_extras(settings, enable);
}

bool wxWebViewWebKit::ShowDevTools()
{
    // If we don't enable access to dev tools, the inspector is simply not
    // shown.
    EnableAccessToDevTools();

    WebKitWebInspector* const
        inspector = webkit_web_view_get_inspector(m_web_view);
    if ( !inspector )
    {
        wxLogDebug("Unexpectedly failed to obtain WebKit inspector.");
        return false;
    }

    webkit_web_inspector_show(inspector);

    return true;
}

bool wxWebViewWebKit::IsAccessToDevToolsEnabled() const
{
    WebKitSettings* settings = webkit_web_view_get_settings(m_web_view);
    return webkit_settings_get_enable_developer_extras(settings);
}

bool wxWebViewWebKit::SetUserAgent(const wxString& userAgent)
{
    if (m_web_view)
    {
        WebKitSettings* settings = webkit_web_view_get_settings(m_web_view);
        webkit_settings_set_user_agent(settings, userAgent.utf8_str());
    }
    else
        m_customUserAgent = userAgent;
    return true;
}

bool wxWebViewWebKit::SetProxy(const wxString& proxy)
{
#if WEBKIT_CHECK_VERSION(2, 16, 0)
    if (wx_check_webkit_version(2, 16, 0))
    {
        const auto context = static_cast<WebKitWebContext*>(m_config.GetNativeConfiguration());
        wxCHECK_MSG( context, false, "no context?" );

        const auto data_manager = webkit_web_context_get_website_data_manager(context);
        wxCHECK_MSG( data_manager, false, "no data manager?" );

        const auto proxy_settings = webkit_network_proxy_settings_new(
            proxy.utf8_str(),
            nullptr // no hosts to ignore
        );
        wxCHECK_MSG( proxy_settings, false, "failed to create proxy settings" );

        webkit_website_data_manager_set_network_proxy_settings(
            data_manager,
            WEBKIT_NETWORK_PROXY_MODE_CUSTOM,
            proxy_settings
        );

        webkit_network_proxy_settings_free(proxy_settings);

        return true;
    }

    wxLogError(_("Setting proxy is not supported by WebKit, at least version 2.16 is required."));

    return false;
#else // WebKit < 2.16 doesn't support setting proxy
    wxUnusedVar(proxy);

    wxLogError(_("This program was compiled without support for setting WebKit proxy."));

    return false;
#endif // WebKit 2.16+
}

void wxWebViewWebKit::Stop()
{
     webkit_web_view_stop_loading(m_web_view);
}

void wxWebViewWebKit::Reload(wxWebViewReloadFlags flags)
{
    if (flags & wxWEBVIEW_RELOAD_NO_CACHE)
    {
        webkit_web_view_reload_bypass_cache(m_web_view);
    }
    else
    {
        webkit_web_view_reload(m_web_view);
    }
}

void wxWebViewWebKit::LoadURL(const wxString& url)
{
    webkit_web_view_load_uri(m_web_view, url.utf8_str());
}


void wxWebViewWebKit::GoBack()
{
    webkit_web_view_go_back(m_web_view);
}

void wxWebViewWebKit::GoForward()
{
    webkit_web_view_go_forward(m_web_view);
}


bool wxWebViewWebKit::CanGoBack() const
{
    return webkit_web_view_can_go_back(m_web_view) != 0;
}


bool wxWebViewWebKit::CanGoForward() const
{
    return webkit_web_view_can_go_forward(m_web_view) != 0;
}

void wxWebViewWebKit::ClearHistory()
{
    // In WebKit2GTK+, the BackForwardList can't be cleared so do nothing.
}

void wxWebViewWebKit::EnableHistory(bool)
{
    // In WebKit2GTK+, history can't be disabled so do nothing here.
}

/* static */
wxSharedPtr<wxWebViewHistoryItem>
wxWebViewWebKit::CreateHistoryItemFromWKItem(WebKitBackForwardListItem* gtkitem)
{
    wxWebViewHistoryItem* wxitem = new wxWebViewHistoryItem(
                          wxString::FromUTF8(webkit_back_forward_list_item_get_uri(gtkitem)),
                          // Since WebKit 2.43.4 titles are not stored any more
                          // and the function is deprecated, so don't use it.
#if !WEBKIT_CHECK_VERSION(2, 43, 4)
                          wxString::FromUTF8(webkit_back_forward_list_item_get_title(gtkitem))
#else
                          wxString()
#endif
                          );
    wxitem->m_histItem = gtkitem;
    return wxSharedPtr<wxWebViewHistoryItem>(wxitem);
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWebKit::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    WebKitBackForwardList* history =
        webkit_web_view_get_back_forward_list(m_web_view);
    GList* list = webkit_back_forward_list_get_back_list(history);
    //We need to iterate in reverse to get the order we desire
    for(int i = g_list_length(list) - 1; i >= 0 ; i--)
    {
        WebKitBackForwardListItem* gtkitem = (WebKitBackForwardListItem*)g_list_nth_data(list, i);
        backhist.push_back(CreateHistoryItemFromWKItem(gtkitem));
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWebKit::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    WebKitBackForwardList* history =
        webkit_web_view_get_back_forward_list(m_web_view);
    GList* list = webkit_back_forward_list_get_forward_list(history);
    for(guint i = 0; i < g_list_length(list); i++)
    {
        WebKitBackForwardListItem* gtkitem = (WebKitBackForwardListItem*)g_list_nth_data(list, i);
        forwardhist.push_back(CreateHistoryItemFromWKItem(gtkitem));
    }
    return forwardhist;
}

void wxWebViewWebKit::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    WebKitBackForwardListItem* gtkitem = (WebKitBackForwardListItem*)item->m_histItem;
    if(gtkitem)
    {
        webkit_web_view_go_to_back_forward_list_item(m_web_view,
                                                     WEBKIT_BACK_FORWARD_LIST_ITEM(gtkitem));
    }
}

extern "C" {
static void wxgtk_can_execute_editing_command_cb(GObject*,
                                                 GAsyncResult *res,
                                                 void* user_data)
{
    GAsyncResult** res_out = static_cast<GAsyncResult**>(user_data);
    g_object_ref(res);
    *res_out = res;
}
}

bool wxWebViewWebKit::CanExecuteEditingCommand(const gchar* command) const
{
    GAsyncResult *result = nullptr;
    webkit_web_view_can_execute_editing_command(m_web_view,
                                                command,
                                                nullptr,
                                                wxgtk_can_execute_editing_command_cb,
                                                &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    while (!result)
    {
        g_main_context_iteration(main_context, TRUE);
    }

    gboolean can_execute = webkit_web_view_can_execute_editing_command_finish(m_web_view,
                                                                              result,
                                                                              nullptr);
    g_object_unref(result);

    return can_execute != 0;
}

bool wxWebViewWebKit::CanCut() const
{
    return CanExecuteEditingCommand(WEBKIT_EDITING_COMMAND_CUT);
}

bool wxWebViewWebKit::CanCopy() const
{
    return CanExecuteEditingCommand(WEBKIT_EDITING_COMMAND_COPY);
}

bool wxWebViewWebKit::CanPaste() const
{
    return CanExecuteEditingCommand(WEBKIT_EDITING_COMMAND_PASTE);
}

void wxWebViewWebKit::Cut()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_CUT);
}

void wxWebViewWebKit::Copy()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_COPY);
}

void wxWebViewWebKit::Paste()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_PASTE);
}

bool wxWebViewWebKit::CanUndo() const
{
    return CanExecuteEditingCommand(WEBKIT_EDITING_COMMAND_UNDO);
}

bool wxWebViewWebKit::CanRedo() const
{
    return CanExecuteEditingCommand(WEBKIT_EDITING_COMMAND_REDO);
}

void wxWebViewWebKit::Undo()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_UNDO);
}

void wxWebViewWebKit::Redo()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_REDO);
}

wxString wxWebViewWebKit::GetCurrentURL() const
{
    // FIXME: check which encoding the web kit control uses instead of
    // assuming UTF8 (here and elsewhere too)
    return wxString::FromUTF8(webkit_web_view_get_uri(m_web_view));
}


wxString wxWebViewWebKit::GetCurrentTitle() const
{
    return wxString::FromUTF8(webkit_web_view_get_title(m_web_view));
}


extern "C" {
static void wxgtk_web_resource_get_data_cb(GObject*,
                                           GAsyncResult *res,
                                           void* user_data)
{
    GAsyncResult** res_out = static_cast<GAsyncResult**>(user_data);
    g_object_ref(res);
    *res_out = res;
}
}

wxString wxWebViewWebKit::GetPageSource() const
{
    WebKitWebResource *resource = webkit_web_view_get_main_resource(m_web_view);
    if (!resource)
    {
        return wxString();
    }

    GAsyncResult *result = nullptr;
    webkit_web_resource_get_data(resource, nullptr,
                                 wxgtk_web_resource_get_data_cb,
                                 &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    while (!result)
    {
        g_main_context_iteration(main_context, TRUE);
    }

    size_t length;
    guchar *source = webkit_web_resource_get_data_finish(resource, result,
                                                         &length, nullptr);
    if (result)
    {
        g_object_unref(result);
    }

    if (source)
    {
        const wxString& wxs = wxString::FromUTF8((const char*)source, length);
        free(source);
        return wxs;
    }
    return wxString();
}


float wxWebViewWebKit::GetZoomFactor() const
{
    return GetWebkitZoom();
}

void wxWebViewWebKit::SetZoomFactor(float zoom)
{
    SetWebkitZoom(zoom);
}

void wxWebViewWebKit::SetZoomType(wxWebViewZoomType type)
{
    WebKitSettings* settings = webkit_web_view_get_settings(m_web_view);
    webkit_settings_set_zoom_text_only(settings,
                                       type == wxWEBVIEW_ZOOM_TYPE_TEXT);
}

wxWebViewZoomType wxWebViewWebKit::GetZoomType() const
{
    WebKitSettings* settings = webkit_web_view_get_settings(m_web_view);
    gboolean tozoom = webkit_settings_get_zoom_text_only(settings);

    if (tozoom)
        return wxWEBVIEW_ZOOM_TYPE_TEXT;
    else
        return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewWebKit::CanSetZoomType(wxWebViewZoomType) const
{
    // this port supports all zoom types
    return true;
}

void wxWebViewWebKit::DoSetPage(const wxString& html, const wxString& baseUri)
{
    webkit_web_view_load_html(m_web_view,
                              html.utf8_str(),
                              baseUri.utf8_str());
}

void wxWebViewWebKit::Print()
{
    WebKitPrintOperation* printop = webkit_print_operation_new(m_web_view);
    webkit_print_operation_run_dialog(printop, nullptr);
    g_object_unref(printop);
}


bool wxWebViewWebKit::IsBusy() const
{
    return m_busy;
}

void wxWebViewWebKit::SetEditable(bool enable)
{
#if WEBKIT_CHECK_VERSION(2, 8, 0)
    webkit_web_view_set_editable(m_web_view, enable);
#else
    // Not supported in older versions.
    wxUnusedVar(enable);
#endif
}

bool wxWebViewWebKit::IsEditable() const
{
#if WEBKIT_CHECK_VERSION(2, 8, 0)
    gboolean editable;
    g_object_get(m_web_view, "editable", &editable, nullptr);
    return editable != 0;
#else
    return false;
#endif
}

void wxWebViewWebKit::DeleteSelection()
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "DeleteSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
    }
}

bool wxWebViewWebKit::HasSelection() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "HasSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
        if (retval)
        {
            gboolean has_selection = FALSE;
            retval.Get("(b)", &has_selection);

            return has_selection != 0;
        }
    }
    return false;
}

void wxWebViewWebKit::SelectAll()
{
    webkit_web_view_execute_editing_command(m_web_view,
                                            WEBKIT_EDITING_COMMAND_SELECT_ALL);
}

wxString wxWebViewWebKit::GetSelectedText() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "GetSelectedText",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
        if (retval)
        {
            char *text;
            retval.Get("(&s)", &text);
            return wxString::FromUTF8(text);
        }
    }
    return wxString();
}

wxString wxWebViewWebKit::GetSelectedSource() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "GetSelectedSource",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
        if (retval)
        {
            char *source;
            retval.Get("(&s)", &source);
            return wxString::FromUTF8(source);
        }
    }
    return wxString();
}

void wxWebViewWebKit::ClearSelection()
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "ClearSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
    }
}

wxString wxWebViewWebKit::GetPageText() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        wxGtkVariant retval(g_dbus_proxy_call_sync(extension,
                                                  "GetPageText",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  nullptr, nullptr));
        if (retval)
        {
            char *text;
            retval.Get("(&s)", &text);
            return wxString::FromUTF8(text);
        }
    }
    return wxString();
}

class wxWebKitRunScriptParams
{
public:
    const wxWebViewWebKit* webKitCtrl;
    void* clientData;
};

extern "C"
{

static void wxgtk_run_javascript_cb(GObject *,
                                    GAsyncResult *res,
                                    void *user_data)
{
    wxWebKitRunScriptParams* params = static_cast<wxWebKitRunScriptParams*>(user_data);
    params->webKitCtrl->ProcessJavaScriptResult(res, params);
}

} // extern "C"

void wxWebViewWebKit::ProcessJavaScriptResult(GAsyncResult *res, wxWebKitRunScriptParams* params) const
{
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    wxGtkError error;
    wxWebKitJavascriptResult js_result
                             (
                                webkit_web_view_run_javascript_finish
                                (
                                    m_web_view,
                                    res,
                                    error.Out()
                                )
                             );

    wxGCC_WARNING_RESTORE(deprecated-declarations)

    if ( js_result )
    {
        wxString scriptResult;
        if ( wxGetStringFromJSResult(js_result, &scriptResult) )
        {
            wxString scriptOutput;
            bool success = wxJSScriptWrapper::ExtractOutput(scriptResult, &scriptOutput);
            SendScriptResult(params->clientData, success, scriptOutput);
        }
    }
    else
        SendScriptResult(params->clientData, false, error.GetMessage());

    delete params;
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, void* clientData) const
{
    wxJSScriptWrapper wrapJS(javascript, wxJSScriptWrapper::JS_OUTPUT_STRING);

    // Collect parameters for access from the callback
    wxWebKitRunScriptParams* params = new wxWebKitRunScriptParams;
    params->webKitCtrl = this;
    params->clientData = clientData;

    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    webkit_web_view_run_javascript(m_web_view,
                                   wrapJS.GetWrappedCode().utf8_str(),
                                   nullptr,
                                   wxgtk_run_javascript_cb,
                                   params);
    wxGCC_WARNING_RESTORE(deprecated-declarations)
}

bool wxWebViewWebKit::AddScriptMessageHandler(const wxString& name)
{
    if (!m_web_view)
        return false;

    WebKitUserContentManager *ucm = webkit_web_view_get_user_content_manager(m_web_view);
    g_signal_connect(ucm, wxString::Format("script-message-received::%s", name).utf8_str(),
                     G_CALLBACK(wxgtk_webview_webkit_script_message_received), this);
    bool res = webkit_user_content_manager_register_script_message_handler(ucm, name.utf8_str());
    if (res)
    {
        // Make webkit message handler available under common name
        wxString js = wxString::Format("window.%s = window.webkit.messageHandlers.%s;",
                name, name);
        AddUserScript(js);
        RunScript(js);
    }

    return res;
}

bool wxWebViewWebKit::RemoveScriptMessageHandler(const wxString& name)
{
    WebKitUserContentManager *ucm = webkit_web_view_get_user_content_manager(m_web_view);
    webkit_user_content_manager_unregister_script_message_handler(ucm, name.utf8_str());
    return true;
}

bool wxWebViewWebKit::AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime)
{
    WebKitUserScript* userScript = webkit_user_script_new(
        javascript.utf8_str(),
        WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
        (injectionTime == wxWEBVIEW_INJECT_AT_DOCUMENT_START) ?
            WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START : WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_END,
        nullptr, nullptr
    );
    WebKitUserContentManager *ucm = webkit_web_view_get_user_content_manager(m_web_view);
    webkit_user_content_manager_add_script(ucm, userScript);
    webkit_user_script_unref(userScript);

    return true;
}

void wxWebViewWebKit::RemoveAllUserScripts()
{
    WebKitUserContentManager *ucm = webkit_web_view_get_user_content_manager(m_web_view);
    webkit_user_content_manager_remove_all_scripts(ucm);
}


#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER_CLEAR
static void
wxgtk_webview_webkit_clear_data_ready(GObject *,
                                      GAsyncResult *result,
                                      void *user_data)
{
    wxWebViewWebKit* webKitCtrl = static_cast<wxWebViewWebKit*>(user_data);
    WebKitWebsiteDataManager* manager = static_cast<wxWebViewConfigurationImplWebKit*>(webKitCtrl->m_config.GetImpl())->GetWebsiteDataManager();

    gboolean success = webkit_website_data_manager_clear_finish(manager, result, nullptr);
    wxWebViewEvent event(wxEVT_WEBVIEW_BROWSING_DATA_CLEARED,
                         webKitCtrl->GetId(),
                         webKitCtrl->GetCurrentURL(),
                         "");
    event.SetEventObject(webKitCtrl);
    event.SetInt((success) ? 1 : 0);
    webKitCtrl->HandleWindowEvent(event);
}
#endif // wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER_CLEAR

bool wxWebViewWebKit::ClearBrowsingData(int types, wxDateTime since)
{
#ifdef wxHAVE_WEBKIT_WEBSITE_DATA_MANAGER_CLEAR
    if (wx_check_webkit_version(2, 16, 0))
    {
        WebKitWebsiteDataManager* manager = static_cast<wxWebViewConfigurationImplWebKit*>(m_config.GetImpl())->GetWebsiteDataManager();

        int wkTypes = 0;

        if (types & wxWEBVIEW_BROWSING_DATA_ALL)
        {
            wkTypes |= WEBKIT_WEBSITE_DATA_ALL;
        }
        else
        {
            if (types & wxWEBVIEW_BROWSING_DATA_COOKIES)
                wkTypes |= WEBKIT_WEBSITE_DATA_COOKIES;

            if (types & wxWEBVIEW_BROWSING_DATA_CACHE)
            {
                wkTypes |= WEBKIT_WEBSITE_DATA_DISK_CACHE |
                           WEBKIT_WEBSITE_DATA_MEMORY_CACHE |
                           WEBKIT_WEBSITE_DATA_OFFLINE_APPLICATION_CACHE |
                           WEBKIT_WEBSITE_DATA_DOM_CACHE;
            }

            if (types & wxWEBVIEW_BROWSING_DATA_DOM_STORAGE)
            {
                wkTypes |= WEBKIT_WEBSITE_DATA_LOCAL_STORAGE |
                           WEBKIT_WEBSITE_DATA_SESSION_STORAGE |
                           WEBKIT_WEBSITE_DATA_INDEXEDDB_DATABASES |
                           WEBKIT_WEBSITE_DATA_WEBSQL_DATABASES;
            }

            if (types & wxWEBVIEW_BROWSING_DATA_OTHER)
            {
                // All the elements of WebKitWebsiteDataTypes not already
                // appearing above.
                wkTypes |= WEBKIT_WEBSITE_DATA_PLUGIN_DATA |
                           WEBKIT_WEBSITE_DATA_HSTS_CACHE |
                           WEBKIT_WEBSITE_DATA_DEVICE_ID_HASH_SALT |
                           WEBKIT_WEBSITE_DATA_ITP |
                           WEBKIT_WEBSITE_DATA_SERVICE_WORKER_REGISTRATIONS;
            }
        }

        GTimeSpan timeSpan = 0;
        if ( since.IsValid() )
        {
            const auto now = wxDateTime::Now();

            wxCHECK_MSG( since < now, false, "Date must be in the past" );

            // GTimeSpan is in microseconds.
            timeSpan = (since - now).GetMilliseconds().GetValue() * 1000;
        }

        webkit_website_data_manager_clear(
            manager,
            (WebKitWebsiteDataTypes) wkTypes,
            timeSpan,
            nullptr,
            wxgtk_webview_webkit_clear_data_ready, this);

        return true;
    }
    else
#endif
    return false;
}

void wxWebViewWebKit::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    m_handlerList.push_back(handler);
    WebKitWebContext* context = static_cast<WebKitWebContext*>(m_config.GetNativeConfiguration());
    webkit_web_context_register_uri_scheme(context, handler->GetName().utf8_str(),
                                           (WebKitURISchemeRequestCallback)wxgtk_webview_webkit_uri_scheme_request_cb,
                                           this, nullptr);
}

void wxWebViewWebKit::EnableContextMenu(bool enable)
{
    wxWebView::EnableContextMenu(enable);
}

long wxWebViewWebKit::Find(const wxString& text, int flags)
{
    WebKitFindController* findctrl = webkit_web_view_get_find_controller(m_web_view);
    bool newSearch = false;
    if(text != m_findText ||
       (flags & wxWEBVIEW_FIND_MATCH_CASE) != (m_findFlags & wxWEBVIEW_FIND_MATCH_CASE))
    {
        newSearch = true;
        //If it is a new search we need to clear existing highlights
        webkit_find_controller_search_finish(findctrl);
    }

    m_findFlags = flags;
    m_findText = text;

    //If the search string is empty then we clear any selection and highlight
    if(text.empty())
    {
        webkit_find_controller_search_finish(findctrl);
        ClearSelection();
        return wxNOT_FOUND;
    }

    bool wrap = false, forward = true;
    guint32 options = WEBKIT_FIND_OPTIONS_NONE;
    if(flags & wxWEBVIEW_FIND_WRAP)
    {
        wrap = true;
        options |= WEBKIT_FIND_OPTIONS_WRAP_AROUND;
    }
    if(!(flags & wxWEBVIEW_FIND_MATCH_CASE))
    {
        options |= WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE;
    }
    if(flags & wxWEBVIEW_FIND_BACKWARDS)
    {
        forward = false;
        options |= WEBKIT_FIND_OPTIONS_BACKWARDS;
    }

    if(newSearch)
    {
        //Initially we count the matches to know how many we have
        m_findCount = -1;
        webkit_find_controller_count_matches(findctrl,
                                             text.utf8_str(),
                                             options,
                                             G_MAXUINT);
        GMainContext *main_context = g_main_context_get_thread_default();
        while (m_findCount == -1)
        {
            g_main_context_iteration(main_context, TRUE);
        }
        //Highlight them if needed
        if(flags & wxWEBVIEW_FIND_HIGHLIGHT_RESULT)
        {
            webkit_find_controller_search(findctrl,
                                          text.utf8_str(),
                                          options,
                                          G_MAXUINT);
        }
        //In this case we return early to match IE behaviour
        m_findPosition = -1;
        return m_findCount;
    }
    else
    {
        if(forward)
            m_findPosition++;
        else
            m_findPosition--;
        if(m_findPosition < 0)
            m_findPosition += m_findCount;
        if(m_findPosition > m_findCount)
            m_findPosition -= m_findCount;
    }

    if(forward)
    {
        webkit_find_controller_search_next(findctrl);
        if(m_findPosition == m_findCount && !wrap)
        {
            return wxNOT_FOUND;
        }
    }
    else
    {
        webkit_find_controller_search_previous(findctrl);
        if(m_findPosition == -1 && !wrap)
        {
            return wxNOT_FOUND;
        }
    }

    return newSearch ? m_findCount : m_findPosition;
}

void wxWebViewWebKit::FindClear()
{
    m_findCount = 0;
    m_findFlags = 0;
    m_findText.clear();
    m_findPosition = -1;
}

// static
wxVisualAttributes
wxWebViewWebKit::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
     return GetDefaultAttributesFromGTKWidget(webkit_web_view_new());
}

void wxWebViewWebKit::SetupWebExtensionServer()
{
    wxGtkString address(g_strdup_printf("unix:tmpdir=%s", g_get_tmp_dir()));
    wxGtkString guid(g_dbus_generate_guid());
    wxGtkObject<GDBusAuthObserver> observer(g_dbus_auth_observer_new());
    wxGtkError error;

    g_signal_connect(observer, "authorize-authenticated-peer",
                     G_CALLBACK(wxgtk_authorize_authenticated_peer_cb), this);

    m_dbusServer = g_dbus_server_new_sync(address,
                                          G_DBUS_SERVER_FLAGS_NONE,
                                          guid,
                                          observer,
                                          nullptr,
                                          error.Out());

    if (error)
    {
        g_warning("Failed to start web extension server on %s: %s",
                  address.c_str(), error.GetMessageStr());
    }
    else
    {
        g_signal_connect(m_dbusServer, "new-connection",
                         G_CALLBACK(wxgtk_new_connection_cb), &m_extension);
        g_dbus_server_start(m_dbusServer);
    }
}

GDBusProxy *wxWebViewWebKit::GetExtensionProxy() const
{
    if (!m_extension)
    {
        g_warning("Web extension not found in \"%s\", "
                  "some wxWebView functionality will be not available",
                  (const char*)GetStandardWebExtensionsDir().utf8_str());
    }
    return m_extension;
}

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT2
