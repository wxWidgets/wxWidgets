/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/webview_webkit2_extension.cpp
// Purpose:     GTK WebKit2 extension for web view component
// Author:      Scott Talbert
// Copyright:   (c) 2017 Scott Talbert
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"
#include "wx/gtk/private/webview_webkit2_extension.h"
#include <webkit2/webkit-web-extension.h>
#define WEBKIT_DOM_USE_UNSTABLE_API
#include <webkitdom/WebKitDOMDOMSelection.h>
#include <webkitdom/WebKitDOMDOMWindowUnstable.h>

// We can't easily avoid deprecation warnings about many WebKit functions, e.g.
// webkit_dom_document_get_default_view() and just about everything related to
// the selection, so for now just disable the warnings as we can't do anything
// about them anyhow.
wxGCC_WARNING_SUPPRESS(deprecated-declarations)

static const char introspection_xml[] =
  "<node>"
  " <interface name='org.wxwidgets.wxGTK.WebExtension'>"
  "  <method name='ClearSelection'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "  </method>"
  "  <method name='DeleteSelection'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "  </method>"
  "  <method name='GetPageSource'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "   <arg type='s' name='source' direction='out'/>"
  "  </method>"
  "  <method name='GetPageText'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "   <arg type='s' name='text' direction='out'/>"
  "  </method>"
  "  <method name='GetSelectedSource'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "   <arg type='s' name='source' direction='out'/>"
  "  </method>"
  "  <method name='GetSelectedText'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "   <arg type='s' name='text' direction='out'/>"
  "  </method>"
  "  <method name='HasSelection'>"
  "   <arg type='t' name='page_id' direction='in'/>"
  "   <arg type='b' name='has_selection' direction='out'/>"
  "  </method>"
  " </interface>"
  "</node>";

class wxWebViewWebKitExtension;

extern "C" {
static gboolean
wxgtk_webview_authorize_authenticated_peer_cb(GDBusAuthObserver *observer,
                                              GIOStream *stream,
                                              GCredentials *credentials,
                                              wxWebViewWebKitExtension *extension);
static void
wxgtk_webview_dbus_connection_created_cb(GObject *source_object,
                                         GAsyncResult *result,
                                         void* user_data);
} // extern "C"

static wxWebViewWebKitExtension *gs_extension = nullptr;

class wxWebViewWebKitExtension
{
public:
    wxWebViewWebKitExtension(WebKitWebExtension *webkit_extension, const char* server_address);
    void ClearSelection(GVariant *parameters, GDBusMethodInvocation *invocation);
    void DeleteSelection(GVariant *parameters, GDBusMethodInvocation *invocation);
    void GetPageSource(GVariant *parameters, GDBusMethodInvocation *invocation);
    void GetPageText(GVariant *parameters, GDBusMethodInvocation *invocation);
    void GetSelectedSource(GVariant *parameters, GDBusMethodInvocation *invocation);
    void GetSelectedText(GVariant *parameters, GDBusMethodInvocation *invocation);
    void HasSelection(GVariant *parameters, GDBusMethodInvocation *invocation);
    void SetDBusConnection(GDBusConnection *dbusConnection);

private:
    WebKitWebPage* GetWebPageOrReturnError(GVariant *parameters, GDBusMethodInvocation *invocation);
    void ReturnDBusStringValue(GDBusMethodInvocation *invocation, gchar *value);

    GDBusConnection *m_dbusConnection;
    WebKitWebExtension *m_webkitExtension;
};

wxWebViewWebKitExtension::wxWebViewWebKitExtension(WebKitWebExtension *extension, const char* server_address)
{
    m_webkitExtension = (WebKitWebExtension*)g_object_ref(extension);

    GDBusAuthObserver *observer = g_dbus_auth_observer_new();
    g_signal_connect(observer, "authorize-authenticated-peer",
                     G_CALLBACK(wxgtk_webview_authorize_authenticated_peer_cb),
                     this);

    g_dbus_connection_new_for_address(server_address,
                                      G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                      observer,
                                      nullptr,
                                      wxgtk_webview_dbus_connection_created_cb,
                                      this);
    g_object_unref(observer);
}

WebKitWebPage* wxWebViewWebKitExtension::GetWebPageOrReturnError(GVariant *parameters, GDBusMethodInvocation *invocation)
{
    guint64 page_id;
    g_variant_get(parameters, "(t)", &page_id);
    WebKitWebPage *web_page = webkit_web_extension_get_page(m_webkitExtension,
                                                            page_id);
    if (!web_page)
    {
        g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR,
                                              G_DBUS_ERROR_INVALID_ARGS,
                                              "Invalid page ID: %" G_GUINT64_FORMAT, page_id);
    }
    return web_page;
}

void wxWebViewWebKitExtension::GetSelectedSource(GVariant *parameters,
                                                 GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMDOMWindow *win = webkit_dom_document_get_default_view(doc);
    WebKitDOMDOMSelection *sel = webkit_dom_dom_window_get_selection(win);
    g_object_unref(win);
    if (!sel)
    {
        ReturnDBusStringValue(invocation, nullptr);
        return;
    }
    WebKitDOMRange *range = webkit_dom_dom_selection_get_range_at(sel, 0, nullptr);
    if (!range)
    {
        ReturnDBusStringValue(invocation, nullptr);
        return;
    }
    WebKitDOMElement *div = webkit_dom_document_create_element(doc, "div",
                                                               nullptr);
    WebKitDOMDocumentFragment *clone = webkit_dom_range_clone_contents(range,
                                                                       nullptr);
    webkit_dom_node_append_child(&div->parent_instance,
                                 &clone->parent_instance, nullptr);
    WebKitDOMElement *html = (WebKitDOMElement*)div;
#if WEBKIT_CHECK_VERSION(2, 8, 0)
    gchar *text = webkit_dom_element_get_inner_html(html);
#else
    gchar *text = webkit_dom_html_element_get_inner_html(WEBKIT_DOM_HTML_ELEMENT(html));
#endif
    g_object_unref(range);

    ReturnDBusStringValue(invocation, text);
}

void wxWebViewWebKitExtension::GetPageSource(GVariant *parameters,
                                             GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMElement *body = webkit_dom_document_get_document_element(doc);
#if WEBKIT_CHECK_VERSION(2, 8, 0)
    gchar *source = webkit_dom_element_get_outer_html(body);
#else
    gchar *source =
        webkit_dom_html_element_get_outer_html(WEBKIT_DOM_HTML_ELEMENT(body));
#endif
    g_dbus_method_invocation_return_value(invocation,
                                          g_variant_new("(s)", source ? source : ""));
}

void wxWebViewWebKitExtension::GetPageText(GVariant *parameters,
                                           GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMHTMLElement *body = webkit_dom_document_get_body(doc);
    gchar *text = webkit_dom_html_element_get_inner_text(body);
    g_dbus_method_invocation_return_value(invocation,
                                          g_variant_new("(s)", text));
}

void wxWebViewWebKitExtension::GetSelectedText(GVariant *parameters,
                                               GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMDOMWindow *win = webkit_dom_document_get_default_view(doc);
    WebKitDOMDOMSelection *sel = webkit_dom_dom_window_get_selection(win);
    g_object_unref(win);
    if (!sel)
    {
        ReturnDBusStringValue(invocation, nullptr);
        return;
    }
    WebKitDOMRange *range = webkit_dom_dom_selection_get_range_at(sel, 0, nullptr);
    if (!range)
    {
        ReturnDBusStringValue(invocation, nullptr);
        return;
    }
    gchar *text = webkit_dom_range_get_text(range);
    g_object_unref(range);

    ReturnDBusStringValue(invocation, text);
}

void wxWebViewWebKitExtension::ClearSelection(GVariant *parameters,
                                              GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMDOMWindow *win = webkit_dom_document_get_default_view(doc);
    WebKitDOMDOMSelection *sel = webkit_dom_dom_window_get_selection(win);
    g_object_unref(win);
    if (sel)
    {
        webkit_dom_dom_selection_remove_all_ranges(sel);
    }

    g_dbus_method_invocation_return_value(invocation, nullptr);
}

void wxWebViewWebKitExtension::HasSelection(GVariant *parameters,
                                            GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMDOMWindow *win = webkit_dom_document_get_default_view(doc);
    WebKitDOMDOMSelection *sel = webkit_dom_dom_window_get_selection(win);
    g_object_unref(win);
    gboolean has_selection = FALSE;
    if (WEBKIT_DOM_IS_DOM_SELECTION(sel))
    {
        if (!webkit_dom_dom_selection_get_is_collapsed(sel))
        {
            has_selection = TRUE;
        }
    }
    g_dbus_method_invocation_return_value(invocation,
                                          g_variant_new("(b)", has_selection));
}

void wxWebViewWebKitExtension::DeleteSelection(GVariant *parameters,
                                               GDBusMethodInvocation *invocation)
{
    WebKitWebPage *web_page = GetWebPageOrReturnError(parameters, invocation);
    if (!web_page)
    {
        return;
    }

    WebKitDOMDocument *doc = webkit_web_page_get_dom_document(web_page);
    WebKitDOMDOMWindow *win = webkit_dom_document_get_default_view(doc);
    WebKitDOMDOMSelection *sel = webkit_dom_dom_window_get_selection(win);
    g_object_unref(win);
    if (sel)
    {
        webkit_dom_dom_selection_delete_from_document(sel);
    }

    g_dbus_method_invocation_return_value(invocation, nullptr);
}

void wxWebViewWebKitExtension::ReturnDBusStringValue(GDBusMethodInvocation *invocation, gchar *value)
{
    g_dbus_method_invocation_return_value(invocation,
                                          g_variant_new("(s)", value ? value : ""));
}

void wxWebViewWebKitExtension::SetDBusConnection(GDBusConnection *dbusConnection)
{
    m_dbusConnection = dbusConnection;
}

extern "C" {
static void
wxgtk_webview_handle_method_call(GDBusConnection*,
                                 const char* /* sender */,
                                 const char* /* object_path */,
                                 const char *interface_name,
                                 const char *method_name,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data)
{
    if (g_strcmp0(interface_name, WXGTK_WEB_EXTENSION_INTERFACE) != 0)
    {
        return;
    }

    wxWebViewWebKitExtension *extension = (wxWebViewWebKitExtension*)user_data;

    if (g_strcmp0(method_name, "ClearSelection") == 0)
    {
        extension->ClearSelection(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "DeleteSelection") == 0)
    {
        extension->DeleteSelection(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "GetPageSource") == 0)
    {
        extension->GetPageSource(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "GetPageText") == 0)
    {
        extension->GetPageText(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "GetSelectedSource") == 0)
    {
        extension->GetSelectedSource(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "GetSelectedText") == 0)
    {
        extension->GetSelectedText(parameters, invocation);
    }
    else if (g_strcmp0(method_name, "HasSelection") == 0)
    {
        extension->HasSelection(parameters, invocation);
    }
}
} // extern "C"

wxGCC_WARNING_SUPPRESS(missing-field-initializers)

static const GDBusInterfaceVTable interface_vtable = {
    wxgtk_webview_handle_method_call,
    nullptr,
    nullptr
};

wxGCC_WARNING_RESTORE(missing-field-initializers)

static
gboolean
wxgtk_webview_dbus_peer_is_authorized(GCredentials *peer_credentials)
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

extern "C" {
static gboolean
wxgtk_webview_authorize_authenticated_peer_cb(GDBusAuthObserver*,
                                              GIOStream*,
                                              GCredentials *credentials,
                                              wxWebViewWebKitExtension*)
{
    return wxgtk_webview_dbus_peer_is_authorized(credentials);
}

static void
wxgtk_webview_dbus_connection_created_cb(GObject*,
                                         GAsyncResult *result,
                                         void* user_data)
{
    static GDBusNodeInfo *introspection_data =
        g_dbus_node_info_new_for_xml(introspection_xml, nullptr);

    GError *error = nullptr;
    GDBusConnection *connection =
        g_dbus_connection_new_for_address_finish(result, &error);
    if (error)
    {
        g_warning("Failed to connect to UI process: %s", error->message);
        g_error_free(error);
        return;
    }

    wxWebViewWebKitExtension* extension = static_cast<wxWebViewWebKitExtension*>(user_data);

    guint registration_id =
        g_dbus_connection_register_object(connection,
                                          WXGTK_WEB_EXTENSION_OBJECT_PATH,
                                          introspection_data->interfaces[0],
                                          &interface_vtable,
                                          extension,
                                          nullptr,
                                          &error);
    if (!registration_id)
    {
        g_warning ("Failed to register web extension object: %s\n", error->message);
        g_error_free (error);
        g_object_unref (connection);
        return;
    }

    extension->SetDBusConnection(connection);
}

WXEXPORT void
webkit_web_extension_initialize_with_user_data (WebKitWebExtension *webkit_extension,
                                                GVariant           *user_data)
{
    const char *server_address;

    g_variant_get (user_data, "(&s)", &server_address);

    gs_extension = new wxWebViewWebKitExtension(webkit_extension,
                                                server_address);
}
} // extern "C"
