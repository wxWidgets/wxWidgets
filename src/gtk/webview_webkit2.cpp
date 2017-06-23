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

#include "wx/stockitem.h"
#include "wx/gtk/webview_webkit.h"
#include "wx/gtk/control.h"
#include "wx/gtk/private.h"
#include "wx/filesys.h"
#include "wx/base64.h"
#include "wx/log.h"
#include "wx/gtk/private/webview_webkit2_extension.h"
#include "wx/gtk/private/string.h"
#include "wx/gtk/private/error.h"
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
wxgtk_webview_webkit_navigation(WebKitWebView *web_view,
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

    if(webKitCtrl->m_guard)
    {
        webKitCtrl->m_guard = false;
        //We set this to make sure that we don't try to load the page again from
        //the resource request callback
        webKitCtrl->m_vfsurl = webkit_web_view_get_uri(web_view);
        webkit_policy_decision_use(decision);
        return FALSE;
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
        wxString wxuri = uri;
        wxSharedPtr<wxWebViewHandler> handler;
        wxVector<wxSharedPtr<wxWebViewHandler> > handlers = webKitCtrl->GetHandlers();
        //We are not vetoed so see if we match one of the additional handlers
        for(wxVector<wxSharedPtr<wxWebViewHandler> >::iterator it = handlers.begin();
            it != handlers.end(); ++it)
        {
            if(wxuri.substr(0, (*it)->GetName().length()) == (*it)->GetName())
            {
                handler = (*it);
            }
        }
        //If we found a handler we can then use it to load the file directly
        //ourselves
        if(handler)
        {
            webKitCtrl->m_guard = true;
            wxFSFile* file = handler->GetFile(wxuri);
            if(file)
            {
                webKitCtrl->SetPage(*file->GetStream(), wxuri);
            }
            //We need to throw some sort of error here if file is NULL
            webkit_policy_decision_ignore(decision);
            return TRUE;
        }
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
wxgtk_webview_webkit_resource_req(WebKitWebView *,
                                  WebKitWebResource *,
                                  WebKitURIRequest *request,
                                  wxWebViewWebKit *webKitCtrl)
{
    wxString uri = webkit_uri_request_get_uri(request);

    wxSharedPtr<wxWebViewHandler> handler;
    wxVector<wxSharedPtr<wxWebViewHandler> > handlers = webKitCtrl->GetHandlers();

    //We are not vetoed so see if we match one of the additional handlers
    for(wxVector<wxSharedPtr<wxWebViewHandler> >::iterator it = handlers.begin();
        it != handlers.end(); ++it)
    {
        if(uri.substr(0, (*it)->GetName().length()) == (*it)->GetName())
        {
            handler = (*it);
        }
    }
    //If we found a handler we can then use it to load the file directly
    //ourselves
    if(handler)
    {
        //If it is requsting the page itself then return as we have already
        //loaded it from the archive
        if(webKitCtrl->m_vfsurl == uri)
            return;

        wxFSFile* file = handler->GetFile(uri);
        if(file)
        {
            //We load the data into a data url to save it being written out again
            size_t size = file->GetStream()->GetLength();
            char *buffer = new char[size];
            file->GetStream()->Read(buffer, size);
            wxString data = wxBase64Encode(buffer, size);
            delete[] buffer;
            wxString mime = file->GetMimeType();
            wxString path = "data:" + mime + ";base64," + data;
            //Then we can redirect the call
            webkit_uri_request_set_uri(request, path.utf8_str());
        }

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

static void
wxgtk_initialize_web_extensions(WebKitWebContext *context,
                                GDBusServer *dbusServer)
{
    const char *address = g_dbus_server_get_client_address(dbusServer);
    GVariant *user_data = g_variant_new("(s)", address);
    webkit_web_context_set_web_extensions_directory(context,
                                                    WX_WEB_EXTENSIONS_DIRECTORY);
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
    g_signal_connect_after(webkit_web_context_get_default(),
                           "initialize-web-extensions",
                           G_CALLBACK(wxgtk_initialize_web_extensions),
                           m_dbusServer);

    m_web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    GTKCreateScrolledWindowWith(GTK_WIDGET(m_web_view));
    g_object_ref(m_widget);

    g_signal_connect_after(m_web_view, "decide-policy",
                           G_CALLBACK(wxgtk_webview_webkit_decide_policy),
                           this);

    g_signal_connect_after(m_web_view, "load-failed",
                           G_CALLBACK(wxgtk_webview_webkit_load_failed), this);

    g_signal_connect_after(m_web_view, "notify::title",
                           G_CALLBACK(wxgtk_webview_webkit_title_changed), this);

    g_signal_connect_after(m_web_view, "resource-load-started",
                           G_CALLBACK(wxgtk_webview_webkit_resource_req), this);

    g_signal_connect_after(m_web_view, "context-menu",
                           G_CALLBACK(wxgtk_webview_webkit_context_menu), this);

    g_signal_connect_after(m_web_view, "create",
                           G_CALLBACK(wxgtk_webview_webkit_create_webview), this);

    WebKitFindController* findctrl = webkit_web_view_get_find_controller(m_web_view);
    g_signal_connect_after(findctrl, "counted-matches",
                           G_CALLBACK(wxgtk_webview_webkit_counted_matches),
                           &m_findCount);

    m_parent->DoAddChild( this );

    PostCreation(size);

    /* Open a webpage */
    webkit_web_view_load_uri(m_web_view, url.utf8_str());

    // last to avoid getting signal too early
    g_signal_connect_after(m_web_view, "load-changed",
                           G_CALLBACK(wxgtk_webview_webkit_load_changed),
                           this);

    return true;
}

wxWebViewWebKit::~wxWebViewWebKit()
{
    if (m_web_view)
        GTKDisconnect(m_web_view);
    if (m_dbusServer)
        g_dbus_server_stop(m_dbusServer);
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
    SetWebkitZoom(GetWebkitZoom() + 0.1);
}

void wxWebViewWebKit::ZoomOut()
{
    SetWebkitZoom(GetWebkitZoom() - 0.1);
}

void wxWebViewWebKit::SetWebkitZoom(float level)
{
    webkit_web_view_set_zoom_level(m_web_view, level);
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


wxWebViewZoom wxWebViewWebKit::GetZoom() const
{
    float zoom = GetWebkitZoom();

    // arbitrary way to map float zoom to our common zoom enum
    if (zoom <= 0.65)
    {
        return wxWEBVIEW_ZOOM_TINY;
    }
    if (zoom <= 0.90)
    {
        return wxWEBVIEW_ZOOM_SMALL;
    }
    if (zoom <= 1.15)
    {
        return wxWEBVIEW_ZOOM_MEDIUM;
    }
    if (zoom <= 1.45)
    {
        return wxWEBVIEW_ZOOM_LARGE;
    }
    return wxWEBVIEW_ZOOM_LARGEST;
}


void wxWebViewWebKit::SetZoom(wxWebViewZoom zoom)
{
    // arbitrary way to map our common zoom enum to float zoom
    switch (zoom)
    {
        case wxWEBVIEW_ZOOM_TINY:
            SetWebkitZoom(0.6f);
            break;

        case wxWEBVIEW_ZOOM_SMALL:
            SetWebkitZoom(0.8f);
            break;

        case wxWEBVIEW_ZOOM_MEDIUM:
            SetWebkitZoom(1.0f);
            break;

        case wxWEBVIEW_ZOOM_LARGE:
            SetWebkitZoom(1.3);
            break;

        case wxWEBVIEW_ZOOM_LARGEST:
            SetWebkitZoom(1.6);
            break;

        default:
            wxFAIL;
    }
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
    webkit_web_view_set_editable(m_web_view, enable);
}

bool wxWebViewWebKit::IsEditable() const
{
    gboolean editable;
    g_object_get(m_web_view, "editable", &editable, NULL);
    return editable != 0;
}

void wxWebViewWebKit::DeleteSelection()
{
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
    if (m_extension)
    {
        guint64 page_id = webkit_web_view_get_page_id(m_web_view);
        GVariant *retval = g_dbus_proxy_call_sync(m_extension,
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

<<<<<<< HEAD
static void wxgtk_run_javascript_cb(WebKitWebView *,
                                    GAsyncResult *res,
                                    GAsyncResult **res_out)
{
<<<<<<< HEAD
    *res_out = (GAsyncResult*)g_object_ref(res);
}

wxString JSResultToString(GObject *object, GAsyncResult *result)
=======
static void
web_view_javascript_finished(GObject      *object,
                              GAsyncResult *result,
<<<<<<< HEAD
                              gpointer      options)
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
{
    wxString                return_value;
    WebKitJavascriptResult  *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    wxGtkError              error;
=======

  printf("Starting web_view_javascript_finished\n");
=======
                              gpointer     user_data)
{
>>>>>>> Trying to allocate memory for result string
=======
/*
static void
web_view_javascript_finished
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error;

    js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object), result, &error);
>>>>>>> Sleep runscript when callback is called

<<<<<<< HEAD
    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW (object), (GAsyncResult *)result, error.Out());

=======
>>>>>>> realloc fails
    if (!js_result)
    {
<<<<<<< HEAD
        wxLogError("Error running javascript: %s", error.GetMessage());
	return_value = wxString();
	return return_value;
=======
        //Can't get wxLogMessage working in this context
        g_warning ("Error running javascript: %s", error->message);
        return;
>>>>>>> Trying to allocate memory for result string
=======

static void wxgtk_run_javascript_cb(WebKitWebView *,
                                    GAsyncResult *res,
                                    GAsyncResult **res_out)
{
    *res_out = (GAsyncResult*)g_object_ref(res);
}

wxString JSResultToString(GObject *object, GAsyncResult *result)
{
    wxString                return_value;
    WebKitJavascriptResult  *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    wxGtkError              error;

    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW (object), (GAsyncResult *)result, error.Out());

    if (!js_result)
    {
<<<<<<< HEAD
      wxLogError("Error running javascript: %s", error.GetMessage());
      return;
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
        wxLogError("Error running javascript: %s", error.GetMessage());
<<<<<<< HEAD
        return;
>>>>>>> Fixing some style
=======
	return_value = wxString();
	return return_value;
>>>>>>> Refactor duplicated code
=======

static void wxgtk_run_javascript_cb(WebKitWebView *,
                                    GAsyncResult *res,
                                    GAsyncResult **res_out)
{
    *res_out = (GAsyncResult*)g_object_ref(res);
}

<<<<<<< HEAD
wxString JSResultToString(GObject *object, GAsyncResult *result)
=======
=======
/*
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
static void
web_view_javascript_finished
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error;

    js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object), result, &error);

    if (!js_result)
    {
        //Can't get wxLogMessage working in this context
        g_warning ("Error running javascript: %s", error->message);
        return;
    }

    context = webkit_javascript_result_get_global_context (js_result);
    value = webkit_javascript_result_get_value (js_result);
    if (JSValueIsString (context, value))
    {
      	printf("excute3\n");
	fflush(stdout);

        JSStringRef js_str_value;
        gsize       str_length;
	gchar      *str_value = (gchar*)user_data;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
	printf("excute1\n");
	fflush(stdout);
        //str_value = (gchar *)g_realloc (str_value,str_length);
	user_data = str_value;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);
	printf("excute2\n");
	fflush(stdout);
	
	//str_value = (gchar*)g_alloc(str_value,str_length+sizeof(char));
        JSStringGetUTF8CString (js_str_value, str_value+sizeof(char), str_length);
        JSStringRelease (js_str_value);
	user_data = str_value;
	str_value[0] = 1;
	printf("str_length = %d\n",str_length);
	printf("str_value = %s\n",str_value+sizeof(char));
	if (user_data != NULL)
	  printf("user_data != NULL");
    }
    else 
        g_warning("Error running javascript: unexpected return value");
    
    webkit_javascript_result_unref (js_result);
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
void wxWebViewWebKit::RunScript(const wxString& javascript, wxObject* user_data)
>>>>>>> Integrate Proof of Concept inside webview_webkit2
{
    wxString                return_value;
    WebKitJavascriptResult  *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    wxGtkError              error;

    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW (object), (GAsyncResult *)result, error.Out());

    if (!js_result)
    {
<<<<<<< HEAD
      wxLogError("Error running javascript: %s", error.GetMessage());
      return;
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
        wxLogError("Error running javascript: %s", error.GetMessage());
<<<<<<< HEAD
        return;
>>>>>>> Fixing some style
=======
	return_value = wxString();
	return return_value;
>>>>>>> Refactor duplicated code
    }

    context = webkit_javascript_result_get_global_context (js_result);
    value = webkit_javascript_result_get_value (js_result);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    
    if (JSValueIsString (context, value))
    {
<<<<<<< HEAD
<<<<<<< HEAD
=======
    if (JSValueIsString (context, value)) {

        printf("(WebKit callback) Result is a String\n");
	
<<<<<<< HEAD
	if (wxwebviewwebkit && wxwebviewwebkit->GetEventHandler()) {
	  printf("Trigged event wxEVT_RUNSCRIPT_RESULT");
	  wxwebviewwebkit->GetEventHandler()->ProcessEvent(event);
	}

>>>>>>> Sleep runscript when callback is called
=======
>>>>>>> Modified sample
=======
=======
    
>>>>>>> Refactor duplicated code
=======
    
>>>>>>> Refactor duplicated code
    if (JSValueIsString (context, value))
    {
<<<<<<< HEAD
<<<<<<< HEAD
      	printf("excute3\n");
	fflush(stdout);

>>>>>>> Trying to allocate memory for result string
        JSStringRef js_str_value;
        gsize       str_length;
	gchar      *str_value = (gchar*)user_data;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
<<<<<<< HEAD
<<<<<<< HEAD
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_str_value, (char*) str_value.c_str(), str_length);
=======
        str_value = (gchar *)g_malloc (str_length);
        JSStringGetUTF8CString (js_str_value, str_value, str_length);
>>>>>>> Sleep runscript when callback is called
        JSStringRelease (js_str_value);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

        return_value = wxString::FromUTF8(str_value);
=======
	snprintf(data,8192,str_value);
=======
=======

	char str[8192];
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
	
	if (user_data == NULL) {
	  snprintf(data,8192,str_value);
	}
	else {
	  snprintf(str,8192,str_value);
	}
	
<<<<<<< HEAD
>>>>>>> Modified sample
        g_print ("Script result: %s\n", str_value);
=======
	if (user_data != NULL) {
	  fflush(stdout);


	  wxString target;
	  wxString url = wxwebviewwebkit->GetCurrentURL();
    
	  wxWebViewEvent event(wxEVT_WEBVIEW_RUNSCRIPT_RESULT,
				   wxwebviewwebkit -> GetId(),
				   url, target);

	  
	  fflush(stdout);
	  if (wxwebviewwebkit && wxwebviewwebkit->GetEventHandler()) {
	    printf("(WebKit callback) Trigged event wxEVT_RUNSCRIPT_RESULT\n");
	    event.SetString(wxString::FromUTF8(str));
	    event.SetEventObject(user_data);
	    wxwebviewwebkit->GetEventHandler()->ProcessEvent(event);
	  }
	}

<<<<<<< HEAD
	g_print ("Script result: %s\n", str_value);
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
	g_print ("(WebKit callback) Script result: %s\n", str_value);
>>>>>>> Now printf are more clear. Minor changes.
        g_free (str_value);

    }
    else if (JSValueIsBoolean(context,value)) {
<<<<<<< HEAD
      printf("Result is a String\n");
>>>>>>> Fixed pointer not getting the result
=======
      printf("Result is a Boolean\n");
>>>>>>> Modified sample
    }
    else if (JSValueIsNumber (context,value))
    {
        double js_number_value;
      
        js_number_value = JSValueToNumber(context,value,NULL);
        return_value = wxString::Format(wxT("%lf"),js_number_value);
    }
    else if (JSValueIsBoolean (context,value))
    {
        bool js_bool_value;
      
        js_bool_value = JSValueToBoolean(context, value);
        return_value = _((js_bool_value) ? "true" : "false");
    }
    else if (JSValueIsObject (context,value))
    {
        JSStringRef js_object_value;
        gsize str_length;
      
        js_object_value = JSValueCreateJSONString(context, value, 0, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_object_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_object_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_object_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsUndefined (context,value))
    {
        return_value = wxString::FromUTF8("undefined");
    }
    else if (JSValueIsNull (context,value))
    {
        return_value =  wxString();
    }
    else 
        wxLogError("Error running javascript: unexpected return value");
=======
	printf("excute1\n");
	fflush(stdout);
        //str_value = (gchar *)g_realloc (str_value,str_length);
	user_data = str_value;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);
	printf("excute2\n");
	fflush(stdout);
	
	//str_value = (gchar*)g_alloc(str_value,str_length+sizeof(char));
        JSStringGetUTF8CString (js_str_value, str_value+sizeof(char), str_length);
        JSStringRelease (js_str_value);
	user_data = str_value;
	str_value[0] = 1;
	printf("str_length = %d\n",str_length);
	printf("str_value = %s\n",str_value+sizeof(char));
	if (user_data != NULL)
	  printf("user_data != NULL");
    }
    else 
        g_warning("Error running javascript: unexpected return value");
>>>>>>> Trying to allocate memory for result string
    
=======
      JSStringRef js_str_value;
      gsize       str_length;
=======
        JSStringRef js_str_value;
        gsize       str_length;
>>>>>>> Fixing some style

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_str_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_str_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsNumber (context,value))
    {
        double js_number_value;
      
        js_number_value = JSValueToNumber(context,value,NULL);
        return_value = wxString::Format(wxT("%lf"),js_number_value);
    }
    else if (JSValueIsBoolean (context,value))
    {
        bool js_bool_value;
      
        js_bool_value = JSValueToBoolean(context, value);
        return_value = _((js_bool_value) ? "true" : "false");
    }
    else if (JSValueIsObject (context,value))
    {
        JSStringRef js_object_value;
        gsize str_length;
      
        js_object_value = JSValueCreateJSONString(context, value, 0, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_object_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_object_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_object_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsUndefined (context,value))
    {
        return_value = wxString::FromUTF8("undefined");
    }
    else if (JSValueIsNull (context,value))
    {
        return_value =  wxString();
    }
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
    webkit_javascript_result_unref (js_result);
<<<<<<< HEAD
<<<<<<< HEAD

    return return_value;
=======
=======
    else 
        wxLogError("Error running javascript: unexpected return value");
>>>>>>> Refactor duplicated code
    
=======
      JSStringRef js_str_value;
      gsize       str_length;
=======
        JSStringRef js_str_value;
        gsize       str_length;
>>>>>>> Fixing some style

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_str_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_str_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsNumber (context,value))
    {
        double js_number_value;
      
        js_number_value = JSValueToNumber(context,value,NULL);
        return_value = wxString::Format(wxT("%lf"),js_number_value);
    }
    else if (JSValueIsBoolean (context,value))
    {
        bool js_bool_value;
      
        js_bool_value = JSValueToBoolean(context, value);
        return_value = _((js_bool_value) ? "true" : "false");
    }
    else if (JSValueIsObject (context,value))
    {
        JSStringRef js_object_value;
        gsize str_length;
      
        js_object_value = JSValueCreateJSONString(context, value, 0, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_object_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_object_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_object_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsUndefined (context,value))
    {
        return_value = wxString::FromUTF8("undefined");
    }
    else if (JSValueIsNull (context,value))
    {
        return_value =  wxString();
    }
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
    webkit_javascript_result_unref (js_result);

    return return_value;
=======
=======
    else 
        wxLogError("Error running javascript: unexpected return value");
>>>>>>> Refactor duplicated code
    
    webkit_javascript_result_unref (js_result);

    return return_value;
}

static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
=======
wxString wxWebViewWebKit::RunScript(const wxString& javascript, wxObject* user_data)
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
{
<<<<<<< HEAD
    wxString return_value;
    wxWebViewEvent *event = (wxWebViewEvent*)user_data;
    wxWebViewWebKit *wxwebviewwebkit = (wxWebViewWebKit*)(event -> GetEventObject());

    return_value = JSResultToString(object, result);
    
    if (wxwebviewwebkit && wxwebviewwebkit->GetEventHandler())
    {
        event -> SetString(return_value);
        wxwebviewwebkit->GetEventHandler()->ProcessEvent(*event);
    }
>>>>>>> Adding other returning values
}

<<<<<<< HEAD
static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
    wxString return_value;
    wxWebViewEvent *event = (wxWebViewEvent*)user_data;
    wxWebViewWebKit *wxwebviewwebkit = (wxWebViewWebKit*)(event -> GetEventObject());

    return_value = JSResultToString(object, result);
    
    if (wxwebviewwebkit && wxwebviewwebkit->GetEventHandler())
    {
        event -> SetString(return_value);
        wxwebviewwebkit->GetEventHandler()->ProcessEvent(*event);
    }
>>>>>>> Adding other returning values
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
    wxString return_value;
    wxWebViewEvent *event = (wxWebViewEvent*)user_data;
    wxWebViewWebKit *wxwebviewwebkit = (wxWebViewWebKit*)(event -> GetEventObject());

    return_value = JSResultToString(object, result);
    
    if (wxwebviewwebkit && wxwebviewwebkit->GetEventHandler())
    {
        event -> SetString(return_value);
        wxwebviewwebkit->GetEventHandler()->ProcessEvent(*event);
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
wxString wxWebViewWebKit::RunScript(const wxString& javascript)
=======
wxString wxWebViewWebKit::RunScript(const wxString& javascript, wxObject* user_data)
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
=======
/*
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
static void
web_view_javascript_finished
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error;

    js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object), result, &error);

    if (!js_result)
    {
        //Can't get wxLogMessage working in this context
        g_warning ("Error running javascript: %s", error->message);
        return;
    }

    context = webkit_javascript_result_get_global_context (js_result);
    value = webkit_javascript_result_get_value (js_result);
    if (JSValueIsString (context, value))
    {
      	printf("excute3\n");
	fflush(stdout);

        JSStringRef js_str_value;
        gsize       str_length;
	gchar      *str_value = (gchar*)user_data;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
	printf("excute1\n");
	fflush(stdout);
        //str_value = (gchar *)g_realloc (str_value,str_length);
	user_data = str_value;
	printf("result pointer %p\n",str_value);
	printf("result pointer %p\n",user_data);
	printf("excute2\n");
	fflush(stdout);
	
	//str_value = (gchar*)g_alloc(str_value,str_length+sizeof(char));
        JSStringGetUTF8CString (js_str_value, str_value+sizeof(char), str_length);
        JSStringRelease (js_str_value);
	user_data = str_value;
	str_value[0] = 1;
	printf("str_length = %d\n",str_length);
	printf("str_value = %s\n",str_value+sizeof(char));
	if (user_data != NULL)
	  printf("user_data != NULL");
    }
    else 
        g_warning("Error running javascript: unexpected return value");
    
    webkit_javascript_result_unref (js_result);
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
void wxWebViewWebKit::RunScript(const wxString& javascript, wxObject* user_data)
>>>>>>> Integrate Proof of Concept inside webview_webkit2
=======
wxString wxWebViewWebKit::RunScript(const wxString& javascript, wxObject* user_data)
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
{
<<<<<<< HEAD
    wxString return_value;
    GAsyncResult *result = NULL;
    webkit_web_view_run_javascript(m_web_view,
                                   javascript,
                                   NULL,
                                   (GAsyncReadyCallback)wxgtk_run_javascript_cb,
                                   &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    
    while (!result)
        g_main_context_iteration(main_context, TRUE);

    return_value = JSResultToString((GObject*)m_web_view, result);    
    
    return return_value;
=======
    //mutex -> Post();
>>>>>>> Sleep runscript when callback is called
=======
>>>>>>> Modified sample
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, int id)
{
<<<<<<< HEAD
    wxWebViewEvent* event = new wxWebViewEvent(wxEVT_WEBVIEW_RUNSCRIPT_RESULT,
                                               GetId(),
                                               GetCurrentURL(),
                                               "");
    event -> SetEventObject(this);
    event -> SetId(id);
  
=======
    printf("Using Runscript winth Webkit2\n");
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> Now printf are more clear. Minor changes.
    char result[8192] = "\0";
=======
=======
=======
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
=======
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
*/
static void wxgtk_run_javascript_cb(WebKitWebView *webview,
                                                 WebKitJavascriptResult *res,
                                                 gpointer *user_data)
<<<<<<< HEAD
<<<<<<< HEAD
=======

static void wxgtk_run_javascript_cb(WebKitWebView *,
<<<<<<< HEAD
<<<<<<< HEAD
				    WebKitJavascriptResult *res,
				    WebKitJavascriptResult *res_out)
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
				    GAsyncResult *res,
				    GAsyncResult **res_out)
>>>>>>> Fixed sync. Improving a little bit the example
=======
                                    GAsyncResult *res,
                                    GAsyncResult **res_out)
>>>>>>> Fixing some style
{
<<<<<<< HEAD
    *res_out = (GAsyncResult*)g_object_ref(res);
}

>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
=======
>>>>>>> Refactor duplicated code
wxString wxWebViewWebKit::RunScript(const wxString& javascript)
{
<<<<<<< HEAD
<<<<<<< HEAD
    wxString return_value;
    GAsyncResult *result = NULL;
    webkit_web_view_run_javascript(m_web_view,
                                   javascript,
                                   NULL,
                                   (GAsyncReadyCallback)wxgtk_run_javascript_cb,
                                   &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    
    while (!result)
        g_main_context_iteration(main_context, TRUE);

    return_value = JSResultToString((GObject*)m_web_view, result);    
    
<<<<<<< HEAD
    if (JSValueIsString (context, value))
    {
        JSStringRef js_str_value;
        gsize       str_length;

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_str_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_str_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsNumber (context,value))
    {
        double js_number_value;
      
        js_number_value = JSValueToNumber(context,value,NULL);
        return_value = wxString::Format(wxT("%lf"),js_number_value);
    }
    else if (JSValueIsBoolean (context,value))
    {
        bool js_bool_value;
      
        js_bool_value = JSValueToBoolean(context, value);
        return_value = _((js_bool_value) ? "true" : "false");
    }
    else if (JSValueIsObject (context,value))
    {
        JSStringRef js_object_value;
        gsize str_length;
      
        js_object_value = JSValueCreateJSONString(context, value, 0, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_object_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_object_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_object_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsUndefined (context,value))
    {
        return_value = wxString::FromUTF8("undefined");
    }
    else if (JSValueIsNull (context,value))
    {
        return_value =  _("");
    }
    else 
        wxLogError("Error running javascript: unexpected return value");
    
    webkit_javascript_result_unref (js_result);

<<<<<<< HEAD

<<<<<<< HEAD
    return _("");
<<<<<<< HEAD

    /*
    gchar *result = (gchar*)g_malloc(8192);
    result[0]=0;
    printf("result pointer %p",result);
  
    webkit_web_view_run_javascript(m_web_view,
				   javascript.mb_str(wxConvUTF8),
				   NULL,
				   web_view_javascript_finished,
				   result);
>>>>>>> Trying to allocate memory for result string

    printf("excute\n");
    fflush(stdout);

<<<<<<< HEAD
    void** options = (void**)malloc(sizeof(void*)*3);
    options[0] = (void*)this;
<<<<<<< HEAD
<<<<<<< HEAD
    options[1] = (void*)mutex;
    options[2] = (void*)result;
    options[3] = user_data;
>>>>>>> Sleep runscript when callback is called
=======
    options[1] = (void*)event;
    options[2] = user_data;
=======
    options[1] = user_data;
    options[2] = (void*)result;
<<<<<<< HEAD
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
    //options[3] = (void*)event;
>>>>>>> Async working, need to use user_data to know which runscript trigged the event
    
>>>>>>> Modified sample
=======
    printf("Using Runscript winth Webkit2\n");
<<<<<<< HEAD
>>>>>>> Putting printf on webkits in order to know when is using one or the other, easily
=======
    printf("Using Runscript winth Webkit2\n");
>>>>>>> Putting printf on webkits in order to know when is using one or the other, easily
    webkit_web_view_run_javascript(m_web_view,
                                   javascript.mb_str(wxConvUTF8),
                                   NULL,
                                   web_view_javascript_finished,
<<<<<<< HEAD
<<<<<<< HEAD
                                   event);
=======
                                   options);

    if (user_data == NULL) {
      // wait for the javascript result:
      printf("(RunScript Sync) entering loop...\n");
      //this limit is probably too short for complex scripts...
      int i;
      for (i=0;i<100;i++) {
	// process events, do not block (just in case)
	gtk_main_iteration_do(false);
	// check if result was modified:
	if (result[0] != 0) {
	  break;
	}
      }
      printf("(RunScript Sync) Loop done!\n");
      // safety check: result should not be empty...
      if (result[0] == 0) {
	// a js exception could have been raised, or the result was undefined...
	printf("(RunScript Sync) Result is empty!\n");
      } else {
	printf("(RunScript Sync) String is: %s (loops=%d)\n", result, i);
      }

      return wxString::FromUTF8(result);
    }
<<<<<<< HEAD
    
    printf("String is: %s\n", result);
>>>>>>> Sleep runscript when callback is called
=======
    else 
      return _("");
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
    while (result[0] == 0)
	gtk_main_iteration_do(false);

    wxString wsx = wxString::FromUTF8(result+sizeof(char));
    printf("RunScript result inside = %s\n",result+sizeof(char));
    printf("RunScript result inside = %s\n",(const char*)wsx.mb_str(wxConvUTF8));
    //g_free (result);

    return wsx;
<<<<<<< HEAD
>>>>>>> Trying to allocate memory for result string
=======
    */

>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
=======
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
=======
>>>>>>> Fixing some style
=======
>>>>>>> Refactor duplicated code
    return return_value;
>>>>>>> Fixed sync. Improving a little bit the example
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, int id)
{
    wxWebViewEvent* event = new wxWebViewEvent(wxEVT_WEBVIEW_RUNSCRIPT_RESULT,
                                               GetId(),
                                               GetCurrentURL(),
                                               "");
    event -> SetEventObject(this);
    event -> SetId(id);
  
=======
=======
>>>>>>> Sleep runscript when callback is called
    char result[8192];
    wxSemaphore* mutex = new wxSemaphore(0,1);
    void* options[4];
    options[0] = (void*)this;
    options[1] = (void*)mutex;
    options[2] = (void*)result;
    options[3] = user_data;
<<<<<<< HEAD
>>>>>>> Sleep runscript when callback is called
=======

=======
>>>>>>> Now printf are more clear. Minor changes.
    char result[8192] = "\0";


    void** options = (void**)malloc(sizeof(void*)*3);
    options[0] = (void*)this;
    options[1] = user_data;
    options[2] = (void*)result;
    //options[3] = (void*)event;
    
>>>>>>> Modified sample
    webkit_web_view_run_javascript(m_web_view,
                                   javascript.mb_str(wxConvUTF8),
                                   NULL,
                                   web_view_javascript_finished,
<<<<<<< HEAD
<<<<<<< HEAD
                                   event);
=======
                                   user_data);
>>>>>>> Integrate Proof of Concept inside webview_webkit2
=======
                                   options);

    if (user_data == NULL) {
      // wait for the javascript result:
      printf("(RunScript Sync) entering loop...\n");
      //this limit is probably too short for complex scripts...
      int i;
      for (i=0;i<100;i++) {
	// process events, do not block (just in case)
	gtk_main_iteration_do(false);
	// check if result was modified:
	if (result[0] != 0) {
	  break;
	}
      }
      printf("(RunScript Sync) Loop done!\n");
      // safety check: result should not be empty...
      if (result[0] == 0) {
	// a js exception could have been raised, or the result was undefined...
	printf("(RunScript Sync) Result is empty!\n");
      } else {
	printf("(RunScript Sync) String is: %s (loops=%d)\n", result, i);
      }

      return wxString::FromUTF8(result);
    }
<<<<<<< HEAD
    
    printf("String is: %s\n", result);
>>>>>>> Sleep runscript when callback is called
=======
    else 
      return _("");
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
=======
=======

static void wxgtk_run_javascript_cb(WebKitWebView *,
<<<<<<< HEAD
<<<<<<< HEAD
				    WebKitJavascriptResult *res,
				    WebKitJavascriptResult *res_out)
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
				    GAsyncResult *res,
				    GAsyncResult **res_out)
>>>>>>> Fixed sync. Improving a little bit the example
=======
                                    GAsyncResult *res,
                                    GAsyncResult **res_out)
>>>>>>> Fixing some style
{
    *res_out = (GAsyncResult*)g_object_ref(res);
}

>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
=======
>>>>>>> Refactor duplicated code
wxString wxWebViewWebKit::RunScript(const wxString& javascript)
{
    wxString return_value;
    GAsyncResult *result = NULL;
    webkit_web_view_run_javascript(m_web_view,
                                   javascript,
                                   NULL,
                                   (GAsyncReadyCallback)wxgtk_run_javascript_cb,
                                   &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    
    while (!result)
        g_main_context_iteration(main_context, TRUE);

    return_value = JSResultToString((GObject*)m_web_view, result);    
    
<<<<<<< HEAD
    if (JSValueIsString (context, value))
    {
        JSStringRef js_str_value;
        gsize       str_length;

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_str_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_str_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsNumber (context,value))
    {
        double js_number_value;
      
        js_number_value = JSValueToNumber(context,value,NULL);
        return_value = wxString::Format(wxT("%lf"),js_number_value);
    }
    else if (JSValueIsBoolean (context,value))
    {
        bool js_bool_value;
      
        js_bool_value = JSValueToBoolean(context, value);
        return_value = _((js_bool_value) ? "true" : "false");
    }
    else if (JSValueIsObject (context,value))
    {
        JSStringRef js_object_value;
        gsize str_length;
      
        js_object_value = JSValueCreateJSONString(context, value, 0, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_object_value);
        wxGtkString str_value((gchar *)g_malloc (str_length));
        JSStringGetUTF8CString (js_object_value, (char*) str_value.c_str(), str_length);
        JSStringRelease (js_object_value);

        return_value = wxString::FromUTF8(str_value);
    }
    else if (JSValueIsUndefined (context,value))
    {
        return_value = wxString::FromUTF8("undefined");
    }
    else if (JSValueIsNull (context,value))
    {
        return_value =  _("");
    }
    else 
        wxLogError("Error running javascript: unexpected return value");
    
    webkit_javascript_result_unref (js_result);

<<<<<<< HEAD

<<<<<<< HEAD
    return _("");
<<<<<<< HEAD

    /*
    gchar *result = (gchar*)g_malloc(8192);
=======
wxString wxWebViewWebKit::RunScript(const wxString& javascript)
{
<<<<<<< HEAD
    gchar *result = (gchar*)g_malloc(10);
>>>>>>> Trying to allocate memory for result string
=======
=======
{
    res = webkit_javascript_result_ref(res);
}

wxString wxWebViewWebKit::RunScript(const wxString& javascript)
{
    GAsyncResult *result = NULL;
    webkit_web_view_run_javascript(m_web_view,
                                   javascript,
                                   NULL,
                                   (GAsyncReadyCallback)wxgtk_run_javascript_cb,
                                   &result);

    GMainContext *main_context = g_main_context_get_thread_default();
    while (!result)
    {
      gtk_main_iteration_do(false);

      //  g_main_context_iteration(main_context, TRUE);
    }


    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error;

    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW (m_web_view), result, &error);


    if (!js_result)
    {
        //Can't get wxLogMessage working in this context
        g_warning ("Error running javascript: %s", error->message);
        return _("");
    }

    context = webkit_javascript_result_get_global_context (js_result);
    value = webkit_javascript_result_get_value (js_result);
    if (JSValueIsString (context, value))
    {
      JSStringRef js_str_value;
      gchar      *str_value;
      gsize       str_length;

      js_str_value = JSValueToStringCopy (context, value, NULL);
      str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
      str_value = (gchar *)g_malloc (str_length);
      JSStringGetUTF8CString (js_str_value, str_value, str_length);
      JSStringRelease (js_str_value);
      g_print ("Script result: %s\n", str_value);
      g_free (str_value);
    }
    else 
        g_warning("Error running javascript: unexpected return value");
>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
    
    webkit_javascript_result_unref (js_result);


    return _("");

    /*
    gchar *result = (gchar*)g_malloc(8192);
>>>>>>> realloc fails
    result[0]=0;
    printf("result pointer %p",result);
  
    webkit_web_view_run_javascript(m_web_view,
				   javascript.mb_str(wxConvUTF8),
				   NULL,
				   web_view_javascript_finished,
				   result);
<<<<<<< HEAD

    printf("excute\n");
    fflush(stdout);

    while (result[0] == 0)
	gtk_main_iteration_do(false);

    wxString wsx = wxString::FromUTF8(result+sizeof(char));
    printf("RunScript result inside = %s\n",result+sizeof(char));
    printf("RunScript result inside = %s\n",(const char*)wsx.mb_str(wxConvUTF8));
    //g_free (result);
<<<<<<< HEAD

    return wsx;
<<<<<<< HEAD
>>>>>>> Trying to allocate memory for result string
=======
    */

>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
=======
>>>>>>> Implementing async and sync. Sync does a segfault and async don't go to event handler
=======
=======
>>>>>>> Fixing some style
=======
>>>>>>> Refactor duplicated code
    return return_value;
>>>>>>> Fixed sync. Improving a little bit the example
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, int id)
{
    wxWebViewEvent* event = new wxWebViewEvent(wxEVT_WEBVIEW_RUNSCRIPT_RESULT,
                                               GetId(),
                                               GetCurrentURL(),
                                               "");
    event -> SetEventObject(this);
    event -> SetId(id);
  
=======
>>>>>>> Sleep runscript when callback is called
=======

=======
>>>>>>> Now printf are more clear. Minor changes.
    char result[8192] = "\0";
=======
>>>>>>> Trying to allocate memory for result string

    printf("excute\n");
    fflush(stdout);

<<<<<<< HEAD
    void** options = (void**)malloc(sizeof(void*)*3);
    options[0] = (void*)this;
    options[1] = user_data;
    options[2] = (void*)result;
    //options[3] = (void*)event;
    
>>>>>>> Modified sample
    webkit_web_view_run_javascript(m_web_view,
                                   javascript.mb_str(wxConvUTF8),
                                   NULL,
                                   web_view_javascript_finished,
<<<<<<< HEAD
                                   event);
=======
                                   user_data);
>>>>>>> Integrate Proof of Concept inside webview_webkit2
=======
                                   options);

    if (user_data == NULL) {
      // wait for the javascript result:
      printf("(RunScript Sync) entering loop...\n");
      //this limit is probably too short for complex scripts...
      int i;
      for (i=0;i<100;i++) {
	// process events, do not block (just in case)
	gtk_main_iteration_do(false);
	// check if result was modified:
	if (result[0] != 0) {
	  break;
	}
      }
      printf("(RunScript Sync) Loop done!\n");
      // safety check: result should not be empty...
      if (result[0] == 0) {
	// a js exception could have been raised, or the result was undefined...
	printf("(RunScript Sync) Result is empty!\n");
      } else {
	printf("(RunScript Sync) String is: %s (loops=%d)\n", result, i);
      }

      return wxString::FromUTF8(result);
    }
<<<<<<< HEAD
    
    printf("String is: %s\n", result);
>>>>>>> Sleep runscript when callback is called
=======
    else 
      return _("");
>>>>>>> New RunScript menuitems on webview sample. Sync is working, async not
=======
    while (result[0] == 0)
	gtk_main_iteration_do(false);

    wxString wsx = wxString::FromUTF8(result+sizeof(char));
    g_free (result);
=======
>>>>>>> realloc fails

    return wsx;
<<<<<<< HEAD
>>>>>>> Trying to allocate memory for result string
=======
    */

>>>>>>> Trying to include the callback inside the RunScript function to not use pointer and simplify the code
}

void wxWebViewWebKit::RunScriptAsync(const wxString& javascript, wxObject* user_data)
{

}

void wxWebViewWebKit::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    m_handlerList.push_back(handler);
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

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT2
