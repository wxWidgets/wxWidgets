/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/webview_edge.cpp
// Purpose:     wxMSW Edge Chromium wxWebView backend implementation
// Author:      Markus Pingel
// Created:     2019-12-15
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/msw/webview_edge.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE

#include "wx/filename.h"
#include "wx/module.h"
#include "wx/log.h"
#include "wx/stdpaths.h"
#include "wx/thread.h"
#include "wx/private/jsscriptwrapper.h"
#include "wx/private/json.h"
#include "wx/msw/private.h"
#include "wx/msw/private/cotaskmemptr.h"
#include "wx/msw/private/webview_edge.h"

#include <wrl/event.h>
#include <Objbase.h>

using namespace Microsoft::WRL;

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdge, wxWebView);

#define WX_ERROR2_CASE(error, wxerror) \
        case error: \
            event.SetString(#error); \
            event.SetInt(wxerror); \
            break;

// WebView2Loader typedefs
typedef HRESULT (__stdcall *CreateCoreWebView2EnvironmentWithOptions_t)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions* environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environment_created_handler);
typedef HRESULT(__stdcall *GetAvailableCoreWebView2BrowserVersionString_t)(
    PCWSTR browserExecutableFolder,
    LPWSTR* versionInfo);

CreateCoreWebView2EnvironmentWithOptions_t wxCreateCoreWebView2EnvironmentWithOptions = NULL;
GetAvailableCoreWebView2BrowserVersionString_t wxGetAvailableCoreWebView2BrowserVersionString = NULL;

bool wxWebViewEdgeImpl::ms_isInitialized = false;
wxDynamicLibrary wxWebViewEdgeImpl::ms_loaderDll;

wxWebViewEdgeImpl::wxWebViewEdgeImpl(wxWebViewEdge* webview):
    m_ctrl(webview)
{

}

wxWebViewEdgeImpl::~wxWebViewEdgeImpl()
{
    if (m_webView)
    {
        m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
        m_webView->remove_NavigationStarting(m_navigationStartingToken);
        m_webView->remove_NewWindowRequested(m_newWindowRequestedToken);
        m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);
        m_webView->remove_ContentLoading(m_contentLoadingToken);
    }
}

bool wxWebViewEdgeImpl::Create()
{
    m_initialized = false;
    m_isBusy = false;
    m_pendingContextMenuEnabled = -1;
    m_pendingAccessToDevToolsEnabled = -1;

    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;

    wxString userDataPath = wxStandardPaths::Get().GetUserLocalDataDir();

    HRESULT hr = wxCreateCoreWebView2EnvironmentWithOptions(
        nullptr,
        userDataPath.wc_str(),
        nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this,
            &wxWebViewEdgeImpl::OnEnvironmentCreated).Get());
    if (FAILED(hr))
    {
        wxLogApiError("CreateWebView2EnvironmentWithOptions", hr);
        return false;
    }
    else
        return true;
}

HRESULT wxWebViewEdgeImpl::OnEnvironmentCreated(
    HRESULT WXUNUSED(result), ICoreWebView2Environment* environment)
{
    environment->QueryInterface(IID_PPV_ARGS(&m_webViewEnvironment));
    m_webViewEnvironment->CreateCoreWebView2Controller(
        m_ctrl->GetHWND(),
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            this, &wxWebViewEdgeImpl::OnWebViewCreated).Get());
    return S_OK;
}

bool wxWebViewEdgeImpl::Initialize()
{
    if (ms_isInitialized)
        return true;

    if (!ms_loaderDll.Load("WebView2Loader.dll", wxDL_DEFAULT | wxDL_QUIET))
        return false;

    // Try to load functions from loader DLL
    wxDL_INIT_FUNC(wx, CreateCoreWebView2EnvironmentWithOptions, ms_loaderDll);
    wxDL_INIT_FUNC(wx, GetAvailableCoreWebView2BrowserVersionString, ms_loaderDll);
    if (!wxGetAvailableCoreWebView2BrowserVersionString || !wxCreateCoreWebView2EnvironmentWithOptions)
        return false;

    // Check if a Edge browser can be found by the loader DLL
    wxCoTaskMemPtr<wchar_t> versionStr;
    HRESULT hr = wxGetAvailableCoreWebView2BrowserVersionString(NULL, &versionStr);
    if (SUCCEEDED(hr) && versionStr)
    {
        ms_isInitialized = true;
        return true;
    }
    else
        wxLogApiError("GetCoreWebView2BrowserVersionInfo", hr);

    return false;
}

void wxWebViewEdgeImpl::Uninitialize()
{
    if (ms_isInitialized)
    {
        ms_loaderDll.Unload();
        ms_isInitialized = false;
    }
}

void wxWebViewEdgeImpl::UpdateBounds()
{
    RECT r;
    wxCopyRectToRECT(m_ctrl->GetClientRect(), r);
    if (m_webView)
        m_webViewController->put_Bounds(r);
}

HRESULT wxWebViewEdgeImpl::OnNavigationStarting(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NavigationStartingEventArgs* args)
{
    m_isBusy = true;
    wxString evtURL;
    wxCoTaskMemPtr<wchar_t> uri;
    if (SUCCEEDED(args->get_Uri(&uri)))
        evtURL = wxString(uri);

    wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, m_ctrl->GetId(), evtURL, wxString());
    event.SetEventObject(m_ctrl);
    m_ctrl->HandleWindowEvent(event);

    if (!event.IsAllowed())
        args->put_Cancel(true);

    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnNavigationCompleted(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NavigationCompletedEventArgs* args)
{
    BOOL isSuccess;
    if (FAILED(args->get_IsSuccess(&isSuccess)))
        isSuccess = false;
    m_isBusy = false;
    wxString uri = m_ctrl->GetCurrentURL();

    if (!isSuccess)
    {
        COREWEBVIEW2_WEB_ERROR_STATUS status;

        wxWebViewEvent event(wxEVT_WEBVIEW_ERROR, m_ctrl->GetId(), uri, wxString());
        event.SetEventObject(m_ctrl);

        if (SUCCEEDED(args->get_WebErrorStatus(&status)))
        {
            switch (status)
            {
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_UNKNOWN, wxWEBVIEW_NAV_ERR_OTHER)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_TIMEOUT, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED, wxWEBVIEW_NAV_ERR_CONNECTION)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED, wxWEBVIEW_NAV_ERR_USER_CANCELLED)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED, wxWEBVIEW_NAV_ERR_OTHER)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR, wxWEBVIEW_NAV_ERR_OTHER)
            }
        }
        m_ctrl->HandleWindowEvent(event);
    }
    else
    {
        if (m_historyEnabled && !m_historyLoadingFromList &&
            (uri == m_ctrl->GetCurrentURL()) ||
            (m_ctrl->GetCurrentURL().substr(0, 4) == "file" &&
                wxFileName::URLToFileName(m_ctrl->GetCurrentURL()).GetFullPath() == uri))
        {
            // If we are not at the end of the list, then erase everything
            // between us and the end before adding the new page
            if (m_historyPosition != static_cast<int>(m_historyList.size()) - 1)
            {
                m_historyList.erase(m_historyList.begin() + m_historyPosition + 1,
                    m_historyList.end());
            }
            wxSharedPtr<wxWebViewHistoryItem> item(new wxWebViewHistoryItem(uri, m_ctrl->GetCurrentTitle()));
            m_historyList.push_back(item);
            m_historyPosition++;
        }
        //Reset as we are done now
        m_historyLoadingFromList = false;
        wxWebViewEvent evt(wxEVT_WEBVIEW_NAVIGATED, m_ctrl->GetId(), uri, wxString());
        m_ctrl->HandleWindowEvent(evt);
    }
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnNewWindowRequested(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NewWindowRequestedEventArgs* args)
{
    wxCoTaskMemPtr<wchar_t> uri;
    wxString evtURL;
    if (SUCCEEDED(args->get_Uri(&uri)))
        evtURL = wxString(uri);
    wxWebViewNavigationActionFlags navFlags = wxWEBVIEW_NAV_ACTION_OTHER;

    BOOL isUserInitiated;
    if (SUCCEEDED(args->get_IsUserInitiated(&isUserInitiated)) && isUserInitiated)
        navFlags = wxWEBVIEW_NAV_ACTION_USER;

    wxWebViewEvent evt(wxEVT_WEBVIEW_NEWWINDOW, m_ctrl->GetId(), evtURL, wxString(), navFlags);
    m_ctrl->HandleWindowEvent(evt);
    args->put_Handled(true);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnDocumentTitleChanged(ICoreWebView2* WXUNUSED(sender), IUnknown* WXUNUSED(args))
{
    wxWebViewEvent event(wxEVT_WEBVIEW_TITLE_CHANGED,
        m_ctrl->GetId(), m_ctrl->GetCurrentURL(), "");
    event.SetString(m_ctrl->GetCurrentTitle());
    event.SetEventObject(m_ctrl);
    m_ctrl->HandleWindowEvent(event);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnContentLoading(ICoreWebView2* WXUNUSED(sender), ICoreWebView2ContentLoadingEventArgs* WXUNUSED(args))
{
    wxWebViewEvent event(wxEVT_WEBVIEW_LOADED, m_ctrl->GetId(),
        m_ctrl->GetCurrentURL(), "");
    event.SetEventObject(m_ctrl);
    m_ctrl->HandleWindowEvent(event);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnWebViewCreated(HRESULT result, ICoreWebView2Controller* webViewController)
{
    if (FAILED(result))
    {
        wxLogApiError("WebView2::WebViewCreated", result);
        return result;
    }

    HRESULT hr = webViewController->get_CoreWebView2(&m_webView);
    if (FAILED(hr))
    {
        wxLogApiError("WebView2::WebViewCreated (get_CoreWebView2)", hr);
        return result;
    }
    m_webViewController = webViewController;

    m_initialized = true;
    UpdateBounds();

    // Connect and handle the various WebView events
    m_webView->add_NavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            this, &wxWebViewEdgeImpl::OnNavigationStarting).Get(),
        &m_navigationStartingToken);
    m_webView->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            this, &wxWebViewEdgeImpl::OnNavigationCompleted).Get(),
        &m_navigationCompletedToken);
    m_webView->add_NewWindowRequested(
        Callback<ICoreWebView2NewWindowRequestedEventHandler>(
            this, &wxWebViewEdgeImpl::OnNewWindowRequested).Get(),
        &m_newWindowRequestedToken);
    m_webView->add_DocumentTitleChanged(
        Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnDocumentTitleChanged).Get(),
        &m_documentTitleChangedToken);
    m_webView->add_ContentLoading(
        Callback<ICoreWebView2ContentLoadingEventHandler>(
            this, &wxWebViewEdgeImpl::OnContentLoading).Get(),
        &m_contentLoadingToken);

    if (m_pendingContextMenuEnabled != -1)
    {
        m_ctrl->EnableContextMenu(m_pendingContextMenuEnabled == 1);
        m_pendingContextMenuEnabled = -1;
    }

    if (m_pendingAccessToDevToolsEnabled != -1)
    {
        m_ctrl->EnableAccessToDevTools(m_pendingAccessToDevToolsEnabled == 1);
        m_pendingContextMenuEnabled = -1;
    }

    wxCOMPtr<ICoreWebView2Settings> settings(GetSettings());
    if (settings)
        settings->put_IsStatusBarEnabled(false);

    if (!m_pendingURL.empty())
    {
        m_ctrl->LoadURL(m_pendingURL);
        m_pendingURL.clear();
    }

    return S_OK;
}

ICoreWebView2Settings* wxWebViewEdgeImpl::GetSettings()
{
    if (!m_webView)
        return NULL;

    ICoreWebView2Settings* settings;
    HRESULT hr = m_webView->get_Settings(&settings);
    if (FAILED(hr))
    {
        wxLogApiError("WebView2::get_Settings", hr);
        return NULL;
    }

    return settings;
}

wxWebViewEdge::~wxWebViewEdge()
{
    Unbind(wxEVT_SHOW, &wxWebViewEdge::OnShow, this);
    delete m_impl;
}

bool wxWebViewEdge::Create(wxWindow* parent,
    wxWindowID id,
    const wxString& url,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
{
    if (!wxWebViewEdgeImpl::Initialize())
        return false;

    if (!wxControl::Create(parent, id, pos, size, style,
        wxDefaultValidator, name))
    {
        return false;
    }

    m_impl = new wxWebViewEdgeImpl(this);
    if (!m_impl->Create())
        return false;
    Bind(wxEVT_SIZE, &wxWebViewEdge::OnSize, this);
    Bind(wxEVT_SHOW, &wxWebViewEdge::OnShow, this);

    LoadURL(url);
    return true;
}

void wxWebViewEdge::OnSize(wxSizeEvent& event)
{
    m_impl->UpdateBounds();
    event.Skip();
}

void wxWebViewEdge::OnShow(wxShowEvent& event)
{
    if (m_impl->m_webView)
        m_impl->m_webViewController->put_IsVisible(event.IsShown());
    event.Skip();
}

void wxWebViewEdge::LoadURL(const wxString& url)
{
    if (!m_impl->m_webView)
    {
        m_impl->m_pendingURL = url;
        return;
    }
    HRESULT hr = m_impl->m_webView->Navigate(url.wc_str());
    if (FAILED(hr))
        wxLogApiError("WebView2::Navigate", hr);
}

void wxWebViewEdge::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    int pos = -1;
    for (unsigned int i = 0; i < m_impl->m_historyList.size(); i++)
    {
        //We compare the actual pointers to find the correct item
        if (m_impl->m_historyList[i].get() == item.get())
            pos = i;
    }
    wxASSERT_MSG(pos != static_cast<int>(m_impl->m_historyList.size()), "invalid history item");
    m_impl->m_historyLoadingFromList = true;
    LoadURL(item->GetUrl());
    m_impl->m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = 0; i < m_impl->m_historyPosition; i++)
    {
        backhist.push_back(m_impl->m_historyList[i]);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = m_impl->m_historyPosition + 1; i < static_cast<int>(m_impl->m_historyList.size()); i++)
    {
        forwardhist.push_back(m_impl->m_historyList[i]);
    }
    return forwardhist;
}

bool wxWebViewEdge::CanGoForward() const
{
    if (m_impl->m_historyEnabled)
        return m_impl->m_historyPosition != static_cast<int>(m_impl->m_historyList.size()) - 1;
    else
        return false;
}

bool wxWebViewEdge::CanGoBack() const
{
    if (m_impl->m_historyEnabled)
        return m_impl->m_historyPosition > 0;
    else
        return false;
}

void wxWebViewEdge::GoBack()
{
    LoadHistoryItem(m_impl->m_historyList[m_impl->m_historyPosition - 1]);
}

void wxWebViewEdge::GoForward()
{
    LoadHistoryItem(m_impl->m_historyList[m_impl->m_historyPosition + 1]);
}

void wxWebViewEdge::ClearHistory()
{
    m_impl->m_historyList.clear();
    m_impl->m_historyPosition = -1;
}

void wxWebViewEdge::EnableHistory(bool enable)
{
    m_impl->m_historyEnabled = enable;
    m_impl->m_historyList.clear();
    m_impl->m_historyPosition = -1;
}

void wxWebViewEdge::Stop()
{
    if (m_impl->m_webView)
        m_impl->m_webView->Stop();
}

void wxWebViewEdge::Reload(wxWebViewReloadFlags WXUNUSED(flags))
{
    if (m_impl->m_webView)
        m_impl->m_webView->Reload();
}

wxString wxWebViewEdge::GetPageSource() const
{
    wxString text;
    const_cast<wxWebViewEdge*>(this)->RunScript("document.documentElement.outerHTML;", &text);
    return text;
}

wxString wxWebViewEdge::GetPageText() const
{
    wxString text;
    const_cast<wxWebViewEdge*>(this)->RunScript("document.body.innerText;", &text);
    return text;
}

bool wxWebViewEdge::IsBusy() const
{
    return m_impl->m_isBusy;
}

wxString wxWebViewEdge::GetCurrentURL() const
{
    wxCoTaskMemPtr<wchar_t> uri;
    if (m_impl->m_webView && SUCCEEDED(m_impl->m_webView->get_Source(&uri)))
        return wxString(uri);
    else
        return wxString();
}

wxString wxWebViewEdge::GetCurrentTitle() const
{
    wxCoTaskMemPtr<wchar_t> title;
    if (m_impl->m_webView && SUCCEEDED(m_impl->m_webView->get_DocumentTitle(&title)))
        return wxString(title);
    else
        return wxString();
}

void wxWebViewEdge::SetZoomType(wxWebViewZoomType)
{
    // only wxWEBVIEW_ZOOM_TYPE_LAYOUT is supported
}

wxWebViewZoomType wxWebViewEdge::GetZoomType() const
{
    return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewEdge::CanSetZoomType(wxWebViewZoomType type) const
{
    return (type == wxWEBVIEW_ZOOM_TYPE_LAYOUT);
}

void wxWebViewEdge::Print()
{
    RunScript("window.print();");
}

wxWebViewZoom wxWebViewEdge::GetZoom() const
{
    double old_zoom_factor = 0.0;
    m_impl->m_webViewController->get_ZoomFactor(&old_zoom_factor);
    if (old_zoom_factor > 1.7)
        return wxWEBVIEW_ZOOM_LARGEST;
    if (old_zoom_factor > 1.3)
        return wxWEBVIEW_ZOOM_LARGE;
    if (old_zoom_factor > 0.8)
        return wxWEBVIEW_ZOOM_MEDIUM;
    if (old_zoom_factor > 0.6)
        return wxWEBVIEW_ZOOM_SMALL;
    return wxWEBVIEW_ZOOM_TINY;
}

float wxWebViewEdge::GetZoomFactor() const
{
    double old_zoom_factor = 0.0;
    m_impl->m_webViewController->get_ZoomFactor(&old_zoom_factor);
    return old_zoom_factor;
}

void wxWebViewEdge::SetZoom(wxWebViewZoom zoom)
{
    double old_zoom_factor = 0.0;
    m_impl->m_webViewController->get_ZoomFactor(&old_zoom_factor);
    double zoom_factor = 1.0;
    switch (zoom)
    {
    case wxWEBVIEW_ZOOM_LARGEST:
        zoom_factor = 2.0;
        break;
    case wxWEBVIEW_ZOOM_LARGE:
        zoom_factor = 1.5;
        break;
    case wxWEBVIEW_ZOOM_MEDIUM:
        zoom_factor = 1.0;
        break;
    case wxWEBVIEW_ZOOM_SMALL:
        zoom_factor = 0.75;
        break;
    case wxWEBVIEW_ZOOM_TINY:
        zoom_factor = 0.5;
        break;
    default:
        break;
    }
    SetZoomFactor(zoom_factor);
}

void wxWebViewEdge::SetZoomFactor(float zoom)
{
    m_impl->m_webViewController->put_ZoomFactor(zoom);
}

bool wxWebViewEdge::CanCut() const
{
    return QueryCommandEnabled("cut");
}

bool wxWebViewEdge::CanCopy() const
{
    return QueryCommandEnabled("copy");
}

bool wxWebViewEdge::CanPaste() const
{
    return QueryCommandEnabled("paste");
}

void wxWebViewEdge::Cut()
{
    ExecCommand("cut");
}

void wxWebViewEdge::Copy()
{
    ExecCommand("copy");
}

void wxWebViewEdge::Paste()
{
    ExecCommand("paste");
}

bool wxWebViewEdge::CanUndo() const
{
    return QueryCommandEnabled("undo");
}

bool wxWebViewEdge::CanRedo() const
{
    return QueryCommandEnabled("redo");
}

void wxWebViewEdge::Undo()
{
    ExecCommand("undo");
}

void wxWebViewEdge::Redo()
{
    ExecCommand("redo");
}

long wxWebViewEdge::Find(const wxString& WXUNUSED(text), int WXUNUSED(flags))
{
    // TODO: not implemented in SDK (could probably be implemented by script)
    return -1;
}

//Editing functions
void wxWebViewEdge::SetEditable(bool WXUNUSED(enable))
{
    wxLogError("Not available");
}

bool wxWebViewEdge::IsEditable() const
{
    return false;
}

void wxWebViewEdge::SelectAll()
{
    RunScript("window.getSelection().selectAllChildren(document);");
}

bool wxWebViewEdge::HasSelection() const
{
    wxString rangeCountStr;
    const_cast<wxWebViewEdge*>(this)->RunScript("window.getSelection().rangeCount;", &rangeCountStr);
    return rangeCountStr != "0";
}

void wxWebViewEdge::DeleteSelection()
{
    ExecCommand("delete");
}

wxString wxWebViewEdge::GetSelectedText() const
{
    wxString selectedText;
    const_cast<wxWebViewEdge*>(this)->RunScript("window.getSelection().toString();", &selectedText);
    return selectedText;
}

wxString wxWebViewEdge::GetSelectedSource() const
{
    // TODO: not implemented in SDK (could probably be implemented by script)
    return wxString();
}

void wxWebViewEdge::ClearSelection()
{
    RunScript("window.getSelection().empty();");
}

void wxWebViewEdge::EnableContextMenu(bool enable)
{
    wxCOMPtr<ICoreWebView2Settings> settings(m_impl->GetSettings());
    if (settings)
        settings->put_AreDefaultContextMenusEnabled(enable);
    else
        m_impl->m_pendingContextMenuEnabled = enable ? 1 : 0;
}

bool wxWebViewEdge::IsContextMenuEnabled() const
{
    wxCOMPtr<ICoreWebView2Settings> settings(m_impl->GetSettings());
    if (settings)
    {
        BOOL menusEnabled = TRUE;
        settings->get_AreDefaultContextMenusEnabled(&menusEnabled);

        if (!menusEnabled)
            return false;
    }
    return true;
}

void wxWebViewEdge::EnableAccessToDevTools(bool enable)
{
    wxCOMPtr<ICoreWebView2Settings> settings(m_impl->GetSettings());
    if (settings)
        settings->put_AreDevToolsEnabled(enable);
    else
        m_impl->m_pendingAccessToDevToolsEnabled = enable ? 1 : 0;
}

bool wxWebViewEdge::IsAccessToDevToolsEnabled() const
{
    wxCOMPtr<ICoreWebView2Settings> settings(m_impl->GetSettings());
    if (settings)
    {
        BOOL devToolsEnabled = TRUE;
        settings->get_AreDevToolsEnabled(&devToolsEnabled);

        if (!devToolsEnabled)
            return false;
    }

    return true;
}

void* wxWebViewEdge::GetNativeBackend() const
{
    return m_impl->m_webView;
}

bool wxWebViewEdge::QueryCommandEnabled(const wxString& command) const
{
    wxString resultStr;
    const_cast<wxWebViewEdge*>(this)->RunScript(
        wxString::Format("function f(){ return document.queryCommandEnabled('%s'); } f();", command), &resultStr);
    return resultStr.IsSameAs("true", false);
}

void wxWebViewEdge::ExecCommand(const wxString& command)
{
    RunScript(wxString::Format("document.execCommand('%s');", command));
}

bool wxWebViewEdge::RunScriptSync(const wxString& javascript, wxString* output)
{
    bool scriptExecuted = false;

    // Start script execution
    HRESULT executionResult = m_impl->m_webView->ExecuteScript(javascript.wc_str(), Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
        [&scriptExecuted, &executionResult, output](HRESULT error, PCWSTR result) -> HRESULT
    {
        // Handle script execution callback
        if (error == S_OK)
        {
            if (output)
                output->assign(result);
        }
        else
            executionResult = error;

        scriptExecuted = true;

        return S_OK;
    }).Get());

    // Wait for script exection
    while (!scriptExecuted)
        wxYield();

    if (FAILED(executionResult))
    {
        if (output)
            output->Printf("%s (0x%08lx)", wxSysErrorMsgStr(executionResult), executionResult);
        return false;
    }
    else
        return true;
}

bool wxWebViewEdge::RunScript(const wxString& javascript, wxString* output)
{
    wxJSScriptWrapper wrapJS(javascript, &m_runScriptCount);

    // This string is also used as an error indicator: it's cleared if there is
    // no error or used in the warning message below if there is one.
    wxString result;
    if (RunScriptSync(wrapJS.GetWrappedCode(), &result)
        && result == wxS("true"))
    {
        if (RunScriptSync(wrapJS.GetUnwrappedOutputCode() + ";", &result))
        {
            if (output)
                // Try to decode JSON string or return original
                // result if it's not a valid JSON string
                if (!wxJSON::DecodeString(result, output))
                    *output = result;
            result.clear();
        }

        RunScriptSync(wrapJS.GetCleanUpCode());
    }

    if (!result.empty())
    {
        wxLogWarning(_("Error running JavaScript: %s"), result);
        return false;
    }

    return true;
}

void wxWebViewEdge::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    // TODO: could maybe be implemented via IWebView2WebView5::add_WebResourceRequested
    wxLogDebug("Registering handlers is not supported");
}

void wxWebViewEdge::DoSetPage(const wxString& html, const wxString& WXUNUSED(baseUrl))
{
    if (m_impl->m_webView)
        m_impl->m_webView->NavigateToString(html.wc_str());
}

// wxWebViewFactoryEdge

bool wxWebViewFactoryEdge::IsAvailable()
{
    return wxWebViewEdgeImpl::Initialize();
}


// ----------------------------------------------------------------------------
// Module ensuring all global/singleton objects are destroyed on shutdown.
// ----------------------------------------------------------------------------

class wxWebViewEdgeModule : public wxModule
{
public:
    wxWebViewEdgeModule()
    {
    }

    virtual bool OnInit() wxOVERRIDE
    {
        return true;
    }

    virtual void OnExit() wxOVERRIDE
    {
        wxWebViewEdgeImpl::Uninitialize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxWebViewEdgeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdgeModule, wxModule);

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE
