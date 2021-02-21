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
#include "wx/tokenzr.h"
#include "wx/private/jsscriptwrapper.h"
#include "wx/private/json.h"
#include "wx/msw/private.h"
#include "wx/msw/private/cotaskmemptr.h"
#include "wx/msw/private/webview_edge.h"

#include <objbase.h>
#include <atomic>

template <typename baseT, typename ...argTs>
class CInvokable : public baseT
{
public:
    CInvokable() : m_nRefCount(0) {}
    virtual ~CInvokable() {}
    // IUnknown methods
    HRESULT QueryInterface(REFIID WXUNUSED(riid), void **ppvObj) override
    {
        /**
         * WebView2 Runtime apparently doesn't use this method, so it doesn't
         * matter how we implement this. On the other hand, this method must be
         * implemented to make this invokable type a concrete class instead of a
         * abstract one.
         */
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
    ULONG AddRef(void) override {
        return ++m_nRefCount;
    }
    ULONG Release(void) override {
        size_t ret = --m_nRefCount;
        if (ret == 0)
            delete this;
        return ret;
    }
private:
    std::atomic<size_t> m_nRefCount;
};
template <typename baseT, typename ...argTs>
class CInvokableLambda : public CInvokable<baseT, argTs...>
{
public:
    CInvokableLambda(std::function<HRESULT(argTs...)> lambda)
    : m_lambda(lambda) {
    }
    // baseT method
    HRESULT Invoke(argTs ...args) override {
        return m_lambda(args...);
    }
private:
    std::function<HRESULT(argTs...)> m_lambda;
};
template <typename baseT, typename contextT, typename ...argTs>
class CInvokableMethod : public CInvokable<baseT, argTs...>
{
public:
    CInvokableMethod(contextT *ctx, HRESULT (contextT::*mthd)(argTs...))
    : m_ctx(ctx), m_mthd(mthd) {
    }
    // baseT method
    HRESULT Invoke(argTs ...args) override {
        return (m_ctx->*m_mthd)(args...);
    }
private:
    contextT *m_ctx;
    HRESULT (contextT::*m_mthd)(argTs...);
};
// the function templates to generate concrete classes from above class templates
template <
    typename baseT, typename lambdaT, // base type & lambda type
    typename LT, typename ...argTs // for capturing argument types of lambda
>
baseT *callback_impl(lambdaT&& lambda, HRESULT (LT::*)(argTs...) const)
{
    return new CInvokableLambda<baseT, argTs...>(lambda);
}
template <typename baseT, typename lambdaT>
baseT *callback(lambdaT&& lambda)
{
    return callback_impl<baseT>(std::move(lambda), &lambdaT::operator());
}
template <typename baseT, typename contextT, typename ...argTs>
baseT *callback(contextT *ctx, HRESULT (contextT::*mthd)(argTs...))
{
    return new CInvokableMethod<baseT, contextT, argTs...>(ctx, mthd);
}

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

wxDynamicLibrary wxWebViewEdgeImpl::ms_loaderDll;
wxString wxWebViewEdgeImpl::ms_browserExecutableDir;
wxString wxWebViewEdgeImpl::ms_version;

wxWebViewEdgeImpl::wxWebViewEdgeImpl(wxWebViewEdge* webview):
    m_ctrl(webview)
{

}

wxWebViewEdgeImpl::~wxWebViewEdgeImpl()
{
    if (m_webView)
    {
        m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
        m_webView->remove_SourceChanged(m_sourceChangedToken);
        m_webView->remove_NavigationStarting(m_navigationStartingToken);
        m_webView->remove_NewWindowRequested(m_newWindowRequestedToken);
        m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);
        m_webView->remove_ContentLoading(m_contentLoadingToken);
        m_webView->remove_ContainsFullScreenElementChanged(m_containsFullScreenElementChangedToken);
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
        ms_browserExecutableDir.wc_str(),
        userDataPath.wc_str(),
        nullptr,
        callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this,
            &wxWebViewEdgeImpl::OnEnvironmentCreated));
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
        callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            this, &wxWebViewEdgeImpl::OnWebViewCreated));
    return S_OK;
}

bool wxWebViewEdgeImpl::Initialize()
{
    if (ms_loaderDll.IsLoaded())
        return true;

    wxDynamicLibrary loaderDll;
    if (!loaderDll.Load("WebView2Loader.dll", wxDL_DEFAULT | wxDL_QUIET))
        return false;

    // Try to load functions from loader DLL
    wxDL_INIT_FUNC(wx, CreateCoreWebView2EnvironmentWithOptions, loaderDll);
    wxDL_INIT_FUNC(wx, GetAvailableCoreWebView2BrowserVersionString, loaderDll);
    if (!wxGetAvailableCoreWebView2BrowserVersionString || !wxCreateCoreWebView2EnvironmentWithOptions)
        return false;

    // Check if a Edge browser can be found by the loader DLL
    wxCoTaskMemPtr<wchar_t> versionStr;
    HRESULT hr = wxGetAvailableCoreWebView2BrowserVersionString(
        ms_browserExecutableDir.wc_str(), &versionStr);
    if (FAILED(hr) || !versionStr)
    {
        wxLogApiError("GetCoreWebView2BrowserVersionInfo", hr);
        return false;
    }
    ms_version = versionStr;

    ms_loaderDll.Attach(loaderDll.Detach());

    return true;
}

void wxWebViewEdgeImpl::Uninitialize()
{
    ms_loaderDll.Unload();
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

HRESULT wxWebViewEdgeImpl::OnSourceChanged(ICoreWebView2 * WXUNUSED(sender), ICoreWebView2SourceChangedEventArgs * args)
{
    BOOL isNewDocument;
    if (SUCCEEDED(args->get_IsNewDocument(&isNewDocument)) && !isNewDocument)
    {
        // navigation within the current document, send apropriate events
        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, m_ctrl->GetId(), m_ctrl->GetCurrentURL(), wxString());
        event.SetEventObject(m_ctrl);
        m_ctrl->HandleWindowEvent(event);
        OnNavigationCompleted(NULL, NULL);
        OnContentLoading(NULL, NULL);
    }
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnNavigationCompleted(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NavigationCompletedEventArgs* args)
{
    BOOL isSuccess;
    if (!args)
        isSuccess = true;
    else if (FAILED(args->get_IsSuccess(&isSuccess)))
        isSuccess = false;
    m_isBusy = false;
    wxString uri = m_ctrl->GetCurrentURL();

    if (!isSuccess)
    {
        COREWEBVIEW2_WEB_ERROR_STATUS status;
        bool ignoreStatus = false;

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
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED, wxWEBVIEW_NAV_ERR_OTHER)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR, wxWEBVIEW_NAV_ERR_OTHER)
            case COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED:
                // This status is triggered by vetoing a wxEVT_WEBVIEW_NAVIGATING event
                ignoreStatus = true;
                break;
            }
        }
        if (!ignoreStatus)
            m_ctrl->HandleWindowEvent(event);
    }
    else
    {
        if ((m_historyEnabled && !m_historyLoadingFromList &&
            (uri == m_ctrl->GetCurrentURL())) ||
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

HRESULT wxWebViewEdgeImpl::OnContainsFullScreenElementChanged(ICoreWebView2* WXUNUSED(sender), IUnknown* WXUNUSED(args))
{
    BOOL containsFullscreenEvent;
    HRESULT hr = m_webView->get_ContainsFullScreenElement(&containsFullscreenEvent);
    if (FAILED(hr))
        return hr;

    wxWebViewEvent event(wxEVT_WEBVIEW_FULLSCREEN_CHANGED, m_ctrl->GetId(),
        m_ctrl->GetCurrentURL(), wxString());
    event.SetEventObject(m_ctrl);
    event.SetInt(containsFullscreenEvent);
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
    m_webViewController->put_IsVisible(true);

    // Connect and handle the various WebView events
    m_webView->add_NavigationStarting(
        callback<ICoreWebView2NavigationStartingEventHandler>(
            this, &wxWebViewEdgeImpl::OnNavigationStarting),
        &m_navigationStartingToken);
    m_webView->add_SourceChanged(
        Callback<ICoreWebView2SourceChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnSourceChanged).Get(),
        &m_sourceChangedToken);
    m_webView->add_NavigationCompleted(
        callback<ICoreWebView2NavigationCompletedEventHandler>(
            this, &wxWebViewEdgeImpl::OnNavigationCompleted),
        &m_navigationCompletedToken);
    m_webView->add_NewWindowRequested(
        callback<ICoreWebView2NewWindowRequestedEventHandler>(
            this, &wxWebViewEdgeImpl::OnNewWindowRequested),
        &m_newWindowRequestedToken);
    m_webView->add_DocumentTitleChanged(
        callback<ICoreWebView2DocumentTitleChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnDocumentTitleChanged),
        &m_documentTitleChangedToken);
    m_webView->add_ContentLoading(
        callback<ICoreWebView2ContentLoadingEventHandler>(
            this, &wxWebViewEdgeImpl::OnContentLoading),
        &m_contentLoadingToken);
    m_webView->add_ContainsFullScreenElementChanged(
        Callback<ICoreWebView2ContainsFullScreenElementChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnContainsFullScreenElementChanged).Get(),
        &m_containsFullScreenElementChangedToken);

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
    wxWindow* topLevelParent = wxGetTopLevelParent(this);
    if (topLevelParent)
        topLevelParent->Unbind(wxEVT_ICONIZE, &wxWebViewEdge::OnTopLevelParentIconized, this);
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
    wxWindow* topLevelParent = wxGetTopLevelParent(this);
    if (topLevelParent)
        topLevelParent->Bind(wxEVT_ICONIZE, &wxWebViewEdge::OnTopLevelParentIconized, this);

    LoadURL(url);
    return true;
}

void wxWebViewEdge::OnSize(wxSizeEvent& event)
{
    m_impl->UpdateBounds();
    event.Skip();
}

void wxWebViewEdge::OnTopLevelParentIconized(wxIconizeEvent& event)
{
    if (m_impl && m_impl->m_webViewController)
        m_impl->m_webViewController->put_IsVisible(!event.IsIconized());
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

float wxWebViewEdge::GetZoomFactor() const
{
    double old_zoom_factor = 0.0;
    m_impl->m_webViewController->get_ZoomFactor(&old_zoom_factor);
    return old_zoom_factor;
}

void wxWebViewEdge::SetZoomFactor(float zoom)
{
    m_impl->m_webViewController->put_ZoomFactor(zoom);
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

//Editing functions
void wxWebViewEdge::SetEditable(bool WXUNUSED(enable))
{
    wxLogError("Not available");
}

bool wxWebViewEdge::IsEditable() const
{
    return false;
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

void wxWebViewEdge::MSWSetBrowserExecutableDir(const wxString & path)
{
    wxWebViewEdgeImpl::ms_browserExecutableDir = path;
}

bool wxWebViewEdge::RunScriptSync(const wxString& javascript, wxString* output) const
{
    bool scriptExecuted = false;

    // Start script execution
    HRESULT executionResult = m_impl->m_webView->ExecuteScript(javascript.wc_str(), callback<ICoreWebView2ExecuteScriptCompletedHandler>(
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
    }));

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

bool wxWebViewEdge::RunScript(const wxString& javascript, wxString* output) const
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

void wxWebViewEdge::RegisterHandler(wxSharedPtr<wxWebViewHandler> WXUNUSED(handler))
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

wxVersionInfo wxWebViewFactoryEdge::GetVersionInfo()
{
    IsAvailable(); // Make sure ms_version string is initialized (if available)
    long major = 0,
         minor = 0,
         micro = 0;
    wxStringTokenizer tk(wxWebViewEdgeImpl::ms_version, ". ");
    // Ignore the return value because if the version component is missing
    // or invalid (i.e. non-numeric), the only thing we can do is to ignore
    // it anyhow.
    tk.GetNextToken().ToLong(&major);
    tk.GetNextToken().ToLong(&minor);
    tk.GetNextToken().ToLong(&micro);

    return wxVersionInfo("Microsoft Edge WebView2", major, minor, micro);
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
