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

#include "wx/dir.h"
#include "wx/dynlib.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/stockitem.h"
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
#include "wx/private/jsscriptwrapper.h"
#include <webkit2/webkit2.h>
#include <JavaScriptCore/JSValueRef.h>
#include <JavaScriptCore/JSStringRef.h>

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

        webKitCtrl->HandleWindowEvent(event);
    }
    else if (load_event == WEBKIT_LOAD_COMMITTED)
    {
        webKitCtrl->m_busy = true;
        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATED,
                             webKitCtrl->GetId(),
                             url, target);

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
    wxString target = webkit_navigation_policy_decision_get_frame_name(navigation_decision);

    //If m_creating is true then we are the result of a new window
    //and so we need to send the event and veto the load
    if(webKitCtrl->m_creating)
    {
        webKitCtrl->m_creating = false;
        wxWebViewEvent event(wxEVT_WEBVIEW_NEWWINDOW,
                             webKitCtrl->GetId(),
                             wxString(uri, wxConvUTF8),
                             target);

        webKitCtrl->HandleWindowEvent(event);

        webkit_policy_decision_ignore(decision);
        return TRUE;
    }

    webKitCtrl->m_busy = true;

    wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING,
                         webKitCtrl->GetId(),
                         wxString( uri, wxConvUTF8 ),
                         target);

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

    wxString description(error->message, wxConvUTF8);

    if (strcmp(g_quark_to_string(error->domain), "soup_http_error_quark") == 0)
    {
        switch (error->code)
        {
            case SOUP_STATUS_CANCELLED:
                type = wxWEBVIEW_NAV_ERR_USER_CANCELLED;
                break;

            case SOUP_STATUS_CANT_RESOLVE:
            case SOUP_STATUS_NOT_FOUND:
                type = wxWEBVIEW_NAV_ERR_NOT_FOUND;
                break;

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
    event.SetString(description);
    event.SetInt(type);

    webKitWindow->HandleWindowEvent(event);

    return FALSE;
}

static gboolean
wxgtk_webview_webkit_new_window(WebKitPolicyDecision *decision,
                                wxWebViewWebKit *webKitCtrl)
{
    WebKitNavigationPolicyDecision* navigation_decision = WEBKIT_NAVIGATION_POLICY_DECISION(decision);
    WebKitNavigationAction* action = webkit_navigation_policy_decision_get_navigation_action(navigation_decision);
    WebKitURIRequest* request = webkit_navigation_action_get_request(action);
    const gchar* uri = webkit_uri_request_get_uri(request);

    wxString target = webkit_navigation_policy_decision_get_frame_name(navigation_decision);
    wxWebViewEvent event(wxEVT_WEBVIEW_NEWWINDOW,
                                       webKitCtrl->GetId(),
                                       wxString( uri, wxConvUTF8 ),
                                       target);

    webKitCtrl->HandleWindowEvent(event);

    //We always want the user to handle this themselves
    webkit_policy_decision_ignore(decision);
    return TRUE;
}

static gboolean
wxgtk_webview_webkit_enter_fullscreen(WebKitWebView *WXUNUSED(web_view),
                                      wxWebViewWebKit *webKitCtrl)
{
    wxWebViewEvent event(wxEVT_WEBVIEW_FULLSCREEN_CHANGED,
                                       webKitCtrl->GetId(),
                                       wxString(),
                                       wxString());
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
    event.SetInt(0);
    webKitCtrl->HandleWindowEvent(event);

    return FALSE;
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
        case WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION:
            return wxgtk_webview_webkit_new_window(decision, webKitCtrl);
        default:
            return FALSE;
    }
}

static void
wxgtk_webview_webkit_title_changed(GtkWidget* widget,
                                   GParamSpec *,
                                   wxWebViewWebKit *webKitCtrl)
{
    gchar *title;
    g_object_get(G_OBJECT(widget), "title", &title, NULL);

    wxWebViewEvent event(wxEVT_WEBVIEW_TITLE_CHANGED,
                         webKitCtrl->GetId(),
                         webKitCtrl->GetCurrentURL(),
                         "");
    event.SetString(wxString(title, wxConvUTF8));

    webKitCtrl->HandleWindowEvent(event);

    g_free(title);
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
                                    WebKitNavigationAction *,
                                    wxWebViewWebKit *webKitCtrl)
{
    //As we do not know the uri being loaded at this point allow the load to
    //continue and catch it in navigation-policy-decision-requested
    webKitCtrl->m_creating = true;
    return web_view;
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
    GVariant *user_data = g_variant_new("(s)", address);

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
                if ( !TrySetWebExtensionsDirectory(context, directories[n]) )
                    break;
            }
        }
    }

    webkit_web_context_set_web_extensions_initialization_user_data(context,
                                                                   user_data);
}

static gboolean
wxgtk_new_connection_cb(GDBusServer *,
                        GDBusConnection *connection,
                        GDBusProxy **proxy)
{
    GError *error = NULL;
    GDBusProxyFlags flags = GDBusProxyFlags(G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS);
    *proxy = g_dbus_proxy_new_sync(connection,
                                   flags,
                                   NULL,
                                   NULL,
                                   WXGTK_WEB_EXTENSION_OBJECT_PATH,
                                   WXGTK_WEB_EXTENSION_INTERFACE,
                                   NULL,
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
    GError *error = NULL;

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
// wxWebViewFactoryWebKit
//-----------------------------------------------------------------------------

wxVersionInfo wxWebViewFactoryWebKit::GetVersionInfo()
{
    return wxVersionInfo("webkit2", webkit_get_major_version(),
        webkit_get_minor_version(), webkit_get_micro_version());
}

//-----------------------------------------------------------------------------
// wxWebViewWebKit
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewWebKit, wxWebView);

wxWebViewWebKit::wxWebViewWebKit()
{
    m_web_view = NULL;
    m_dbusServer = NULL;
    m_extension = NULL;
}

bool wxWebViewWebKit::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &url,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    m_web_view = NULL;
    m_dbusServer = NULL;
    m_extension = NULL;
    m_busy = false;
    m_guard = false;
    m_creating = false;
    FindClear();

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
    g_signal_connect(webkit_web_context_get_default(),
                     "initialize-web-extensions",
                     G_CALLBACK(wxgtk_initialize_web_extensions),
                     m_dbusServer);

    m_web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    GTKCreateScrolledWindowWith(GTK_WIDGET(m_web_view));
    g_object_ref(m_widget);

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

    WebKitFindController* findctrl = webkit_web_view_get_find_controller(m_web_view);
    g_signal_connect(findctrl, "counted-matches",
                     G_CALLBACK(wxgtk_webview_webkit_counted_matches),
                     &m_findCount);

    m_parent->DoAddChild( this );

    PostCreation(size);

    /* Open a webpage */
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
            webkit_web_context_get_default(), m_dbusServer);
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
    return webkit_web_view_get_zoom_level(m_web_view);
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
    webkit_web_view_load_uri(m_web_view, wxGTK_CONV(url));
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
        wxWebViewHistoryItem* wxitem = new wxWebViewHistoryItem(
                              webkit_back_forward_list_item_get_uri(gtkitem),
                              webkit_back_forward_list_item_get_title(gtkitem));
        wxitem->m_histItem = gtkitem;
        wxSharedPtr<wxWebViewHistoryItem> item(wxitem);
        backhist.push_back(item);
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
        wxWebViewHistoryItem* wxitem = new wxWebViewHistoryItem(
                              webkit_back_forward_list_item_get_uri(gtkitem),
                              webkit_back_forward_list_item_get_title(gtkitem));
        wxitem->m_histItem = gtkitem;
        wxSharedPtr<wxWebViewHistoryItem> item(wxitem);
        forwardhist.push_back(item);
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
    GAsyncResult *result = NULL;
    webkit_web_view_can_execute_editing_command(m_web_view,
                                                command,
                                                NULL,
                                                wxgtk_can_execute_editing_command_cb,
                                                &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    while (!result)
    {
        g_main_context_iteration(main_context, TRUE);
    }

    gboolean can_execute = webkit_web_view_can_execute_editing_command_finish(m_web_view,
                                                                              result,
                                                                              NULL);
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

    GAsyncResult *result = NULL;
    webkit_web_resource_get_data(resource, NULL,
                                 wxgtk_web_resource_get_data_cb,
                                 &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    while (!result)
    {
        g_main_context_iteration(main_context, TRUE);
    }

    size_t length;
    guchar *source = webkit_web_resource_get_data_finish(resource, result,
                                                         &length, NULL);
    if (result)
    {
        g_object_unref(result);
    }

    if (source)
    {
        wxString wxs(source, wxConvUTF8, length);
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
                              html.mb_str(wxConvUTF8),
                              baseUri.mb_str(wxConvUTF8));
}

void wxWebViewWebKit::Print()
{
    WebKitPrintOperation* printop = webkit_print_operation_new(m_web_view);
    webkit_print_operation_run_dialog(printop, NULL);
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
    g_object_get(m_web_view, "editable", &editable, NULL);
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
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "DeleteSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            g_variant_unref(retval);
        }
    }
}

bool wxWebViewWebKit::HasSelection() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "HasSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            gboolean has_selection = FALSE;
            g_variant_get(retval, "(b)", &has_selection);
            g_variant_unref(retval);
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
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "GetSelectedText",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            char *text;
            g_variant_get(retval, "(s)", &text);
            g_variant_unref(retval);
            return wxString(text, wxConvUTF8);
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
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "GetSelectedSource",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            char *source;
            g_variant_get(retval, "(s)", &source);
            g_variant_unref(retval);
            return wxString(source, wxConvUTF8);
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
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "ClearSelection",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            g_variant_unref(retval);
        }
    }
}

wxString wxWebViewWebKit::GetPageText() const
{
    GDBusProxy *extension = GetExtensionProxy();
    if (extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(extension,
                                                  "GetPageText",
                                                  g_variant_new("(t)", page_id),
                                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                                  NULL, NULL);
        if (retval)
        {
            char *text;
            g_variant_get(retval, "(s)", &text);
            g_variant_unref(retval);
            return wxString(text, wxConvUTF8);
        }
    }
    return wxString();
}

extern "C"
{

static void wxgtk_run_javascript_cb(GObject *,
                                    GAsyncResult *res,
                                    void *user_data)
{
    g_object_ref(res);

    GAsyncResult** res_out = static_cast<GAsyncResult**>(user_data);
    *res_out = res;
}

} // extern "C"

// Run the given script synchronously and return its result in output.
bool wxWebViewWebKit::RunScriptSync(const wxString& javascript, wxString* output) const
{
    GAsyncResult *result = NULL;
    webkit_web_view_run_javascript(m_web_view,
                                   javascript.utf8_str(),
                                   NULL,
                                   wxgtk_run_javascript_cb,
                                   &result);

    GMainContext *main_context = g_main_context_get_thread_default();

    while ( !result )
        g_main_context_iteration(main_context, TRUE);

    wxGtkError error;
    wxWebKitJavascriptResult js_result
                             (
                                webkit_web_view_run_javascript_finish
                                (
                                    m_web_view,
                                    result,
                                    error.Out()
                                )
                             );

    // Match g_object_ref() in wxgtk_run_javascript_cb()
    g_object_unref(result);

    if ( !js_result )
    {
        if ( output )
            *output = error.GetMessage();
        return false;
    }

    JSGlobalContextRef context = webkit_javascript_result_get_global_context(js_result);
    JSValueRef value = webkit_javascript_result_get_value(js_result);

    JSValueRef exception = NULL;
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
            wxJSStringRef ex_value(JSValueToStringCopy(context, exception, NULL));
            *output = ex_value.ToWxString();
        }

        return false;
    }

    if ( output != NULL )
        *output = js_value.ToWxString();

    return true;
}

bool wxWebViewWebKit::RunScript(const wxString& javascript, wxString* output) const
{
    wxJSScriptWrapper wrapJS(javascript, &m_runScriptCount);

    // This string is also used as an error indicator: it's cleared if there is
    // no error or used in the warning message below if there is one.
    wxString result;
    if ( RunScriptSync(wrapJS.GetWrappedCode(), &result)
            && result == wxS("true") )
    {
        if ( RunScriptSync(wrapJS.GetOutputCode(), &result) )
        {
            if ( output )
                *output = result;
            result.clear();
        }

        RunScriptSync(wrapJS.GetCleanUpCode());
    }

    if ( !result.empty() )
    {
        wxLogWarning(_("Error running JavaScript: %s"), result);
        return false;
    }

    return true;
}

void wxWebViewWebKit::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    m_handlerList.push_back(handler);
    WebKitWebContext* context = webkit_web_context_get_default();
    webkit_web_context_register_uri_scheme(context, handler->GetName().utf8_str(),
                                           (WebKitURISchemeRequestCallback)wxgtk_webview_webkit_uri_scheme_request_cb,
                                           this, NULL);
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
                                             wxGTK_CONV(text),
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
                                          wxGTK_CONV(text),
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
    char *address = g_strdup_printf("unix:tmpdir=%s", g_get_tmp_dir());
    char *guid = g_dbus_generate_guid();
    GDBusAuthObserver *observer = g_dbus_auth_observer_new();
    GError *error = NULL;

    g_signal_connect(observer, "authorize-authenticated-peer",
                     G_CALLBACK(wxgtk_authorize_authenticated_peer_cb), this);

    m_dbusServer = g_dbus_server_new_sync(address,
                                          G_DBUS_SERVER_FLAGS_NONE,
                                          guid,
                                          observer,
                                          NULL,
                                          &error);

    if (error)
    {
        g_warning("Failed to start web extension server on %s: %s", address, error->message);
        g_error_free(error);
    }
    else
    {
        g_signal_connect(m_dbusServer, "new-connection",
                         G_CALLBACK(wxgtk_new_connection_cb), &m_extension);
        g_dbus_server_start(m_dbusServer);
    }

    g_free(address);
    g_free(guid);
    g_object_unref(observer);
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
