/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/webview_webkit.cpp
// Purpose:     GTK WebKit backend for web view component
// Author:      Marianne Gagnon, Robert Roebling
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEBVIEW_WEBKIT

#include "wx/stockitem.h"
#include "wx/gtk/webview_webkit.h"
#include "wx/gtk/control.h"
#include "wx/gtk/private.h"
#include "webkit/webkit.h"

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C"
{

static void
wxgtk_webkitctrl_load_status_callback(GtkWidget* widget, GParamSpec*,
                                      wxWebViewWebKit *webKitCtrl)
{
    if (!webKitCtrl->m_ready) return;

    wxString url = webKitCtrl->GetCurrentURL();

    WebKitLoadStatus status;
    g_object_get(G_OBJECT(widget), "load-status", &status, NULL);

    wxString target; // TODO: get target (if possible)

    if (status == WEBKIT_LOAD_FINISHED)
    {
        webKitCtrl->m_busy = false;
        wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_LOADED,
                                       webKitCtrl->GetId(),
                                       url, target, false);

        if (webKitCtrl && webKitCtrl->GetEventHandler())
            webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);
    }
    else if (status ==  WEBKIT_LOAD_COMMITTED)
    {
        webKitCtrl->m_busy = true;
        wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
                                       webKitCtrl->GetId(),
                                       url, target, false);

        if (webKitCtrl && webKitCtrl->GetEventHandler())
            webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);
    }
}

static WebKitNavigationResponse
wxgtk_webkitctrl_navigation_requ_callback(WebKitWebView*,
                                          WebKitWebFrame       *frame,
                                          WebKitNetworkRequest *request,
                                          wxWebViewWebKit      *webKitCtrl)
{
    webKitCtrl->m_busy = true;

    const gchar* uri = webkit_network_request_get_uri(request);

    wxString target = webkit_web_frame_get_name (frame);
    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATING,
                                   webKitCtrl->GetId(),
                                   wxString( uri, wxConvUTF8 ),
                                   target,
                                   true);

    if (webKitCtrl && webKitCtrl->GetEventHandler())
        webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);

    if (thisEvent.IsVetoed())
    {
        webKitCtrl->m_busy = false;
        return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
    }
    else
    {
        return  WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
    }
}

static gboolean
wxgtk_webkitctrl_error (WebKitWebView*,
                        WebKitWebFrame*,
                        gchar          *uri,
                        gpointer        web_error,
                        wxWebViewWebKit* webKitWindow)
{
    webKitWindow->m_busy = false;
    wxWebNavigationError type = wxWEB_NAV_ERR_OTHER;

    GError* error = (GError*)web_error;
    wxString description(error->message, wxConvUTF8);

    if (strcmp(g_quark_to_string(error->domain), "soup_http_error_quark") == 0)
    {
        switch (error->code)
        {
            case SOUP_STATUS_CANCELLED:
                type = wxWEB_NAV_ERR_USER_CANCELLED;
                break;

            case SOUP_STATUS_CANT_RESOLVE:
            case SOUP_STATUS_NOT_FOUND:
                type = wxWEB_NAV_ERR_NOT_FOUND;
                break;

            case SOUP_STATUS_CANT_RESOLVE_PROXY:
            case SOUP_STATUS_CANT_CONNECT:
            case SOUP_STATUS_CANT_CONNECT_PROXY:
            case SOUP_STATUS_SSL_FAILED:
            case SOUP_STATUS_IO_ERROR:
                type = wxWEB_NAV_ERR_CONNECTION;
                break;

            case SOUP_STATUS_MALFORMED:
            //case SOUP_STATUS_TOO_MANY_REDIRECTS:
                type = wxWEB_NAV_ERR_REQUEST;
                break;

            //case SOUP_STATUS_NO_CONTENT:
            //case SOUP_STATUS_RESET_CONTENT:

            case SOUP_STATUS_BAD_REQUEST:
                type = wxWEB_NAV_ERR_REQUEST;
                break;

            case SOUP_STATUS_UNAUTHORIZED:
            case SOUP_STATUS_FORBIDDEN:
                type = wxWEB_NAV_ERR_AUTH;
                break;

            case SOUP_STATUS_METHOD_NOT_ALLOWED:
            case SOUP_STATUS_NOT_ACCEPTABLE:
                type = wxWEB_NAV_ERR_SECURITY;
                break;

            case SOUP_STATUS_PROXY_AUTHENTICATION_REQUIRED:
                type = wxWEB_NAV_ERR_AUTH;
                break;

            case SOUP_STATUS_REQUEST_TIMEOUT:
                type = wxWEB_NAV_ERR_CONNECTION;
                break;

            //case SOUP_STATUS_PAYMENT_REQUIRED:
            case SOUP_STATUS_REQUEST_ENTITY_TOO_LARGE:
            case SOUP_STATUS_REQUEST_URI_TOO_LONG:
            case SOUP_STATUS_UNSUPPORTED_MEDIA_TYPE:
                type = wxWEB_NAV_ERR_REQUEST;
                break;

            case SOUP_STATUS_BAD_GATEWAY:
            case SOUP_STATUS_SERVICE_UNAVAILABLE:
            case SOUP_STATUS_GATEWAY_TIMEOUT:
                type = wxWEB_NAV_ERR_CONNECTION;
                break;

            case SOUP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
                type = wxWEB_NAV_ERR_REQUEST;
                break;
            //case SOUP_STATUS_INSUFFICIENT_STORAGE:
            //case SOUP_STATUS_NOT_EXTENDED:
        }
    }
    else if (strcmp(g_quark_to_string(error->domain),
                    "webkit-network-error-quark") == 0)
    {
        switch (error->code)
        {
            //WEBKIT_NETWORK_ERROR_FAILED:
            //WEBKIT_NETWORK_ERROR_TRANSPORT:

            case WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL:
                type = wxWEB_NAV_ERR_REQUEST;
                break;

            case WEBKIT_NETWORK_ERROR_CANCELLED:
                type = wxWEB_NAV_ERR_USER_CANCELLED;
                break;

            case WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST:
                type = wxWEB_NAV_ERR_NOT_FOUND;
                break;
        }
    }
    else if (strcmp(g_quark_to_string(error->domain),
                    "webkit-policy-error-quark") == 0)
    {
        switch (error->code)
        {
            //case WEBKIT_POLICY_ERROR_FAILED:
            //case WEBKIT_POLICY_ERROR_CANNOT_SHOW_MIME_TYPE:
            //case WEBKIT_POLICY_ERROR_CANNOT_SHOW_URL:
            //case WEBKIT_POLICY_ERROR_FRAME_LOAD_INTERRUPTED_BY_POLICY_CHANGE:
            case WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT:
                type = wxWEB_NAV_ERR_SECURITY;
                break;
        }
    }
    /*
    webkit_plugin_error_quark
    else
    {
        printf("Error domain %s\n", g_quark_to_string(error->domain));
    }
    */

    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_ERROR,
                                   webKitWindow->GetId(),
                                   uri,
                                   wxEmptyString,
                                   false);
    thisEvent.SetString(description);
    thisEvent.SetInt(type);

    if (webKitWindow && webKitWindow->GetEventHandler())
    {
        webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
    }

    return FALSE;
}

static gboolean
wxgtk_webkitctrl_new_window(WebKitWebView*,
                            WebKitWebFrame *frame,
                            WebKitNetworkRequest *request,
                            WebKitWebNavigationAction*,
                            WebKitWebPolicyDecision *policy_decision,
                    wxWebViewWebKit *webKitCtrl)
{
    const gchar* uri = webkit_network_request_get_uri(request);

    wxString target = webkit_web_frame_get_name (frame);
    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NEWWINDOW,
                                   webKitCtrl->GetId(),
                                   wxString( uri, wxConvUTF8 ),
                                   target,
                                   true);

    if (webKitCtrl && webKitCtrl->GetEventHandler())
        webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);

    if (thisEvent.IsVetoed())
    {
        webkit_web_policy_decision_ignore(policy_decision);
    }
    else
    {
        webkit_web_policy_decision_use(policy_decision);
    }
    return TRUE;
}

} // extern "C"

//-----------------------------------------------------------------------------
// wxWebViewWebKit
//-----------------------------------------------------------------------------

//IMPLEMENT_DYNAMIC_CLASS(wxWebViewWebKit, wxControl)

bool wxWebViewWebKit::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &url,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    m_ready = false;
    m_busy = false;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxWebViewWebKit creation failed") );
        return false;
    }

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    web_view = webkit_web_view_new ();
    g_object_ref(web_view); // TODO: check memory management

    m_widget = scrolled_window;
    g_object_ref(m_widget); // TODO: check memory management

    /* Place the WebKitWebView in the GtkScrolledWindow */
    gtk_container_add (GTK_CONTAINER (scrolled_window), web_view);
    gtk_widget_show(m_widget);
    gtk_widget_show(web_view);

    g_signal_connect_after(web_view, "notify::load-status",
                           G_CALLBACK(wxgtk_webkitctrl_load_status_callback),
                           this);
    g_signal_connect_after(web_view, "navigation-requested",
                           G_CALLBACK(wxgtk_webkitctrl_navigation_requ_callback),
                           this);
    g_signal_connect_after(web_view, "load-error", 
                           G_CALLBACK(wxgtk_webkitctrl_error),
                           this);

    g_signal_connect_after(web_view, "new-window-policy-decision-requested",
                           G_CALLBACK(wxgtk_webkitctrl_new_window), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    /* Open a webpage */
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW (web_view), url);

    //Get the initial history limit so we can enable and disable it later
    WebKitWebBackForwardList* history;
    history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
    m_historyLimit = webkit_web_back_forward_list_get_limit(history);

    m_ready = true;

    return true;
}

bool wxWebViewWebKit::Enable( bool enable )
{
    if (!wxControl::Enable(enable))
        return false;

    gtk_widget_set_sensitive(GTK_BIN(m_widget)->child, enable);

    //if (enable)
    //    GTKFixSensitivity();

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
    webkit_web_view_zoom_in (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::ZoomOut()
{
    webkit_web_view_zoom_out (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::SetWebkitZoom(float level)
{
    webkit_web_view_set_zoom_level (WEBKIT_WEB_VIEW(web_view), level);
}

float wxWebViewWebKit::GetWebkitZoom()
{
    return webkit_web_view_get_zoom_level (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Stop()
{
     webkit_web_view_stop_loading (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Reload(wxWebViewReloadFlags flags)
{
    if (flags & wxWEB_VIEW_RELOAD_NO_CACHE)
    {
        webkit_web_view_reload_bypass_cache (WEBKIT_WEB_VIEW(web_view));
    }
    else
    {
        webkit_web_view_reload (WEBKIT_WEB_VIEW(web_view));
    }
}

void wxWebViewWebKit::LoadUrl(const wxString& url)
{
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), wxGTK_CONV(url));
}


void wxWebViewWebKit::GoBack()
{
    webkit_web_view_go_back (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::GoForward()
{
    webkit_web_view_go_forward (WEBKIT_WEB_VIEW(web_view));
}


bool wxWebViewWebKit::CanGoBack()
{
    return webkit_web_view_can_go_back (WEBKIT_WEB_VIEW(web_view));
}


bool wxWebViewWebKit::CanGoForward()
{
    return webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::ClearHistory()
{
    WebKitWebBackForwardList* history;
    history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
    webkit_web_back_forward_list_clear(history);
}

void wxWebViewWebKit::EnableHistory(bool enable)
{
    WebKitWebBackForwardList* history;
    history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
    if(enable)
    {
        webkit_web_back_forward_list_set_limit(history, m_historyLimit);
    }
    else
    {
        webkit_web_back_forward_list_set_limit(history, 0);
    }
}

wxVector<wxSharedPtr<wxWebHistoryItem> > wxWebViewWebKit::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebHistoryItem> > backhist; 
    WebKitWebBackForwardList* history;
    history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
    GList* list = webkit_web_back_forward_list_get_back_list_with_limit(history, 
                                                                        m_historyLimit);
    //We need to iterate in reverse to get the order we desire
    for(int i = g_list_length(list) - 1; i >= 0 ; i--)
    {
        WebKitWebHistoryItem* gtkitem = (WebKitWebHistoryItem*)g_list_nth_data(list, i);
        wxSharedPtr<wxWebHistoryItem> item(new wxWebHistoryItem(
                                           webkit_web_history_item_get_uri(gtkitem),
                                           webkit_web_history_item_get_title(gtkitem)));
        backhist.push_back(item);
        m_historyMap[item] = gtkitem;
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebHistoryItem> > wxWebViewWebKit::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebHistoryItem> > forwardhist; 
    WebKitWebBackForwardList* history;
    history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
    GList* list = webkit_web_back_forward_list_get_forward_list_with_limit(history, 
                                                                           m_historyLimit);
    for(guint i = 0; i < g_list_length(list); i++)
    {
        WebKitWebHistoryItem* gtkitem = (WebKitWebHistoryItem*)g_list_nth_data(list, i);
        wxSharedPtr<wxWebHistoryItem> item(new wxWebHistoryItem(
                                           webkit_web_history_item_get_uri(gtkitem),
                                           webkit_web_history_item_get_title(gtkitem)));
        forwardhist.push_back(item);
        m_historyMap[item] = gtkitem;
    }
    return forwardhist;
}

void wxWebViewWebKit::LoadHistoryItem(wxSharedPtr<wxWebHistoryItem> item)
{
    WebKitWebHistoryItem* gtkitem = m_historyMap[item];
    if(gtkitem)
    {
        WebKitWebBackForwardList* history;
        history = webkit_web_view_get_back_forward_list(WEBKIT_WEB_VIEW(web_view));
        webkit_web_back_forward_list_go_to_item(history, gtkitem);
    }
}

bool wxWebViewWebKit::CanCut()
{
    return webkit_web_view_can_cut_clipboard(WEBKIT_WEB_VIEW(web_view));
}

bool wxWebViewWebKit::CanCopy()
{
    return webkit_web_view_can_copy_clipboard(WEBKIT_WEB_VIEW(web_view));
}

bool wxWebViewWebKit::CanPaste()
{
    return webkit_web_view_can_paste_clipboard(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Cut()
{
    webkit_web_view_cut_clipboard(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Copy()
{
    webkit_web_view_copy_clipboard(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Paste()
{
    webkit_web_view_paste_clipboard(WEBKIT_WEB_VIEW(web_view));
}

bool wxWebViewWebKit::CanUndo()
{
    return webkit_web_view_can_undo(WEBKIT_WEB_VIEW(web_view));
}

bool wxWebViewWebKit::CanRedo()
{
    return webkit_web_view_can_redo(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Undo()
{
    webkit_web_view_undo(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::Redo()
{
    webkit_web_view_redo(WEBKIT_WEB_VIEW(web_view));
}

wxString wxWebViewWebKit::GetCurrentURL()
{
    // FIXME: check which encoding the web kit control uses instead of
    // assuming UTF8 (here and elsewhere too)
    return wxString::FromUTF8(webkit_web_view_get_uri(
                                WEBKIT_WEB_VIEW(web_view)));
}


wxString wxWebViewWebKit::GetCurrentTitle()
{
    return wxString::FromUTF8(webkit_web_view_get_title(
                                WEBKIT_WEB_VIEW(web_view)));
}


wxString wxWebViewWebKit::GetPageSource()
{
    WebKitWebFrame* frame = webkit_web_view_get_main_frame(
        WEBKIT_WEB_VIEW(web_view));
    WebKitWebDataSource* src = webkit_web_frame_get_data_source (frame);

    // TODO: check encoding with
    // const gchar*
    // webkit_web_data_source_get_encoding(WebKitWebDataSource *data_source);
    return wxString(webkit_web_data_source_get_data (src)->str, wxConvUTF8);
}


wxWebViewZoom wxWebViewWebKit::GetZoom()
{
    float zoom = GetWebkitZoom();

    // arbitrary way to map float zoom to our common zoom enum
    if (zoom <= 0.65)
    {
        return wxWEB_VIEW_ZOOM_TINY;
    }
    else if (zoom > 0.65 && zoom <= 0.90)
    {
        return wxWEB_VIEW_ZOOM_SMALL;
    }
    else if (zoom > 0.90 && zoom <= 1.15)
    {
        return wxWEB_VIEW_ZOOM_MEDIUM;
    }
    else if (zoom > 1.15 && zoom <= 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGE;
    }
    else if (zoom > 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGEST;
    }

    // to shut up compilers, this can never be reached logically
    wxASSERT(false);
    return wxWEB_VIEW_ZOOM_MEDIUM;
}


void wxWebViewWebKit::SetZoom(wxWebViewZoom zoom)
{
    // arbitrary way to map our common zoom enum to float zoom
    switch (zoom)
    {
        case wxWEB_VIEW_ZOOM_TINY:
            SetWebkitZoom(0.6f);
            break;

        case wxWEB_VIEW_ZOOM_SMALL:
            SetWebkitZoom(0.8f);
            break;

        case wxWEB_VIEW_ZOOM_MEDIUM:
            SetWebkitZoom(1.0f);
            break;

        case wxWEB_VIEW_ZOOM_LARGE:
            SetWebkitZoom(1.3);
            break;

        case wxWEB_VIEW_ZOOM_LARGEST:
            SetWebkitZoom(1.6);
            break;

        default:
            wxASSERT(false);
    }
}

void wxWebViewWebKit::SetZoomType(wxWebViewZoomType type)
{
    webkit_web_view_set_full_content_zoom(WEBKIT_WEB_VIEW(web_view),
                                          (type == wxWEB_VIEW_ZOOM_TYPE_LAYOUT ?
                                          TRUE : FALSE));
}

wxWebViewZoomType wxWebViewWebKit::GetZoomType() const
{
    gboolean fczoom = webkit_web_view_get_full_content_zoom(
            WEBKIT_WEB_VIEW(web_view));

    if (fczoom) return wxWEB_VIEW_ZOOM_TYPE_LAYOUT;
    else        return wxWEB_VIEW_ZOOM_TYPE_TEXT;
}

bool wxWebViewWebKit::CanSetZoomType(wxWebViewZoomType) const
{
    // this port supports all zoom types
    return true;
}

void wxWebViewWebKit::SetPage(const wxString& html, const wxString& baseUri)
{
    webkit_web_view_load_string (WEBKIT_WEB_VIEW(web_view),
                                 html.mb_str(wxConvUTF8),
                                 "text/html",
                                 "UTF-8",
                                 baseUri.mb_str(wxConvUTF8));
}

void wxWebViewWebKit::Print()
{
    WebKitWebFrame* frame = webkit_web_view_get_main_frame(
            WEBKIT_WEB_VIEW(web_view));
    webkit_web_frame_print (frame);

    // GtkPrintOperationResult  webkit_web_frame_print_full
    //      (WebKitWebFrame *frame,
    //       GtkPrintOperation *operation,
    //       GtkPrintOperationAction action,
    //       GError **error);

}


bool wxWebViewWebKit::IsBusy()
{
    return m_busy;

    // This code looks nice but returns true after a page was cancelled
    /*
    WebKitLoadStatus status = webkit_web_view_get_load_status
            (WEBKIT_WEB_VIEW(web_view));


#if WEBKIT_CHECK_VERSION(1,1,16)
    // WEBKIT_LOAD_FAILED is new in webkit 1.1.16
    if (status == WEBKIT_LOAD_FAILED)
    {
        return false;
    }
#endif
    if (status == WEBKIT_LOAD_FINISHED)
    {
        return false;
    }

    return true;
     */
}

void wxWebViewWebKit::SetEditable(bool enable)
{
    webkit_web_view_set_editable(WEBKIT_WEB_VIEW(web_view), enable);
}

bool wxWebViewWebKit::IsEditable()
{
    return webkit_web_view_get_editable(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::DeleteSelection()
{
    webkit_web_view_delete_selection(WEBKIT_WEB_VIEW(web_view));
}

bool wxWebViewWebKit::HasSelection()
{
    return webkit_web_view_has_selection(WEBKIT_WEB_VIEW(web_view));
}

void wxWebViewWebKit::SelectAll()
{
    webkit_web_view_select_all(WEBKIT_WEB_VIEW(web_view));
}

wxString wxWebViewWebKit::GetSelectedText()
{
    WebKitDOMDocument* doc; 
    WebKitDOMDOMWindow* win;
    WebKitDOMDOMSelection* sel;
    WebKitDOMRange* range;

    doc = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW(web_view));
    win = webkit_dom_document_get_default_view(WEBKIT_DOM_DOCUMENT(doc));
    sel = webkit_dom_dom_window_get_selection(WEBKIT_DOM_DOM_WINDOW(win));
    range = webkit_dom_dom_selection_get_range_at(WEBKIT_DOM_DOM_SELECTION(sel), 
                                                  0, NULL);
    return wxString(webkit_dom_range_get_text(WEBKIT_DOM_RANGE(range)), 
                    wxConvUTF8);
}

wxString wxWebViewWebKit::GetSelectedHTML()
{
    WebKitDOMDocument* doc; 
    WebKitDOMDOMWindow* win;
    WebKitDOMDOMSelection* sel;
    WebKitDOMRange* range;
    WebKitDOMElement* div;
    WebKitDOMDocumentFragment* clone;
    WebKitDOMHTMLElement* html;

    doc = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW(web_view));
    win = webkit_dom_document_get_default_view(WEBKIT_DOM_DOCUMENT(doc));
    sel = webkit_dom_dom_window_get_selection(WEBKIT_DOM_DOM_WINDOW(win));
    range = webkit_dom_dom_selection_get_range_at(WEBKIT_DOM_DOM_SELECTION(sel), 
                                                  0, NULL);
    div = webkit_dom_document_create_element(WEBKIT_DOM_DOCUMENT(doc), "div", NULL);

    clone = webkit_dom_range_clone_contents(WEBKIT_DOM_RANGE(range), NULL);
    webkit_dom_node_append_child(&div->parent_instance, &clone->parent_instance, NULL);
    html = (WebKitDOMHTMLElement*)div;

    return wxString(webkit_dom_html_element_get_inner_html(WEBKIT_DOM_HTML_ELEMENT(html)), 
                    wxConvUTF8);
}

// static
wxVisualAttributes
wxWebViewWebKit::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
     return GetDefaultAttributesFromGTKWidget(webkit_web_view_new);
}


#endif // wxHAVE_WEB_BACKEND_GTK_WEBKIT
