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

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/msw/webview_edge.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE

#include "wx/filename.h"
#include "wx/module.h"
#include "wx/stdpaths.h"
#include "wx/thread.h"
#include "wx/private/jsscriptwrapper.h"

#include <wrl/event.h>

using namespace Microsoft::WRL;

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdge, wxWebView);

#define WX_ERROR2_CASE(error, wxerror) \
        case error: \
            event.SetString(#error); \
            event.SetInt(wxerror); \
            break;

int wxWebViewEdge::ms_isAvailable = -1;
// WebView2Loader typedefs
typedef HRESULT (__stdcall *PFNWXCreateWebView2EnvironmentWithDetails)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    PCWSTR additionalBrowserArguments,
    IWebView2CreateWebView2EnvironmentCompletedHandler* environment_created_handler);
typedef HRESULT(__stdcall *PFNWXGetWebView2BrowserVersionInfo)(
    PCWSTR browserExecutableFolder,
    LPWSTR* versionInfo);

PFNWXCreateWebView2EnvironmentWithDetails wxCreateWebView2EnvironmentWithDetails = NULL;
PFNWXGetWebView2BrowserVersionInfo wxGetWebView2BrowserVersionInfo = NULL;

wxDynamicLibrary wxWebViewEdge::ms_loaderDll;

bool wxWebViewEdge::IsAvailable()
{
    if (ms_isAvailable == -1)
    {
        if (!Initialize())
            ms_isAvailable = 0;
        else
            ms_isAvailable = 1;
    }

    return ms_isAvailable == 1;
}

bool wxWebViewEdge::Initialize()
{
#define RESOLVE_LOADER_FUNCTION(type, funcname)                        \
    wx##funcname = (type)ms_loaderDll.GetSymbol(wxT(#funcname));       \
    if ( !wx##funcname )                                               \
        return false

    // WebView2 is only available for Windows 7 or newer
    if (!wxCheckOsVersion(6, 1))
        return false;

    if (!ms_loaderDll.Load("WebView2Loader.dll", wxDL_DEFAULT | wxDL_QUIET))
        return false;

    // Try to load functions from loader DLL
    RESOLVE_LOADER_FUNCTION(PFNWXCreateWebView2EnvironmentWithDetails, CreateWebView2EnvironmentWithDetails);
    RESOLVE_LOADER_FUNCTION(PFNWXGetWebView2BrowserVersionInfo, GetWebView2BrowserVersionInfo);

    // Check if a Edge browser can be found by the loader DLL
    LPWSTR versionStr;
    if (SUCCEEDED(wxGetWebView2BrowserVersionInfo(NULL, &versionStr)))
    {
        if (versionStr)
            return true;
    }

    return false;
}

void wxWebViewEdge::Uninitalize()
{
    if (ms_isAvailable == 1)
    {
        ms_loaderDll.Unload();
        ms_isAvailable = -1;
    }
}

wxWebViewEdge::~wxWebViewEdge()
{
    if (m_webView)
    {
        // TOOD: Remove additional events
        m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
        m_webView->remove_NavigationStarting(m_navigationStartingToken);
        m_webView->remove_DocumentStateChanged(m_documentStateChangedToken);
        m_webView->remove_NewWindowRequested(m_newWindowRequestedToken);
    }
}

bool wxWebViewEdge::Create(wxWindow* parent,
    wxWindowID id,
    const wxString& url,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
{
    m_initialized = false;
    m_isBusy = false;

    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;

    if (!wxControl::Create(parent, id, pos, size, style,
        wxDefaultValidator, name))
    {
        return false;
    }

    if (!IsAvailable())
        return false;

    m_pendingURL = url;

    Bind(wxEVT_SIZE, &wxWebViewEdge::OnSize, this);

    wxString userDataPath = wxStandardPaths::Get().GetUserLocalDataDir();

    HRESULT hr = wxCreateWebView2EnvironmentWithDetails(
        nullptr,
        userDataPath.wc_str(),
        nullptr,
        Callback<IWebView2CreateWebView2EnvironmentCompletedHandler>(
            [this](HRESULT WXUNUSED(result), IWebView2Environment* environment) -> HRESULT
            {
                environment->QueryInterface(IID_PPV_ARGS(&m_webViewEnvironment));
                m_webViewEnvironment->CreateWebView(
                    GetHWND(), Callback<IWebView2CreateWebViewCompletedHandler>(
                        [this](HRESULT WXUNUSED(result), IWebView2WebView* webview) -> HRESULT
                        {
                            webview->QueryInterface(IID_PPV_ARGS(&m_webView));
                            UpdateBounds();
                            InitWebViewCtrl();
                            return S_OK;
                        })
                    .Get());
                return S_OK;
            }).Get());
    return SUCCEEDED(hr);
}

void wxWebViewEdge::InitWebViewCtrl()
{
    m_initialized = true;
    UpdateBounds();

    // Connect and handle the various WebView events

    m_webView->add_NavigationStarting(
        Callback<IWebView2NavigationStartingEventHandler>(
            [this](IWebView2WebView* WXUNUSED(sender), IWebView2NavigationStartingEventArgs* args) -> HRESULT
            {
                m_isBusy = true;
                wxString evtURL;
                PWSTR uri;
                if (SUCCEEDED(args->get_Uri(&uri)))
                    evtURL = wxString(uri);
                wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, GetId(), evtURL, wxString());
                event.SetEventObject(this);
                HandleWindowEvent(event);

                if (!event.IsAllowed())
                    args->put_Cancel(true);

                return S_OK;
            })
        .Get(), &m_navigationStartingToken);

    m_webView->add_DocumentStateChanged(
        Callback<IWebView2DocumentStateChangedEventHandler>(
            [this](IWebView2WebView* sender, IWebView2DocumentStateChangedEventArgs* WXUNUSED(args)) -> HRESULT
            {
                PWSTR uri;
                sender->get_Source(&uri);
                wxString evtURL(uri);
                if (evtURL.Cmp(L"about:blank") == 0)              
                {
                    evtURL = L"";                    
                }
                
                // AddPendingEvent(wxWebViewEvent(wxEVT_WEBVIEW_NAVIGATED, GetId(), uri, wxString()));
                // SetWindowText(m_toolbar->addressBarWindow, uri.get());
                return S_OK;
            })
        .Get(),
                &m_documentStateChangedToken);

    m_webView->add_NavigationCompleted(
        Callback<IWebView2NavigationCompletedEventHandler>(
            [this](IWebView2WebView* sender, IWebView2NavigationCompletedEventArgs* args) -> HRESULT
            {
                BOOL isSuccess;
                if (FAILED(args->get_IsSuccess(&isSuccess)))
                    isSuccess = false;
                m_isBusy = false;
                PWSTR _uri;
                sender->get_Source(&_uri);
                wxString uri(_uri);

                if (!isSuccess)
                {
                    WEBVIEW2_WEB_ERROR_STATUS status;

                    wxWebViewEvent event(wxEVT_WEBVIEW_ERROR, GetId(), uri, wxString());
                    event.SetEventObject(this);

                    if (SUCCEEDED(args->get_WebErrorStatus(&status)))
                    {
                        switch (status)
                        {
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_UNKNOWN, wxWEBVIEW_NAV_ERR_OTHER)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_TIMEOUT, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED, wxWEBVIEW_NAV_ERR_CONNECTION)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED, wxWEBVIEW_NAV_ERR_USER_CANCELLED)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED, wxWEBVIEW_NAV_ERR_OTHER)
                            WX_ERROR2_CASE(WEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR, wxWEBVIEW_NAV_ERR_OTHER)
                        }
                    }
                    HandleWindowEvent(event);
                }
                else
                {
                    AddPendingEvent(wxWebViewEvent(wxEVT_WEBVIEW_NAVIGATED, GetId(), uri, wxString()));
                    if (m_historyEnabled && !m_historyLoadingFromList &&
                        (uri == GetCurrentURL()) ||
                        (GetCurrentURL().substr(0, 4) == "file" &&
                            wxFileName::URLToFileName(GetCurrentURL()).GetFullPath() == uri))
                    {
                        // If we are not at the end of the list, then erase everything
                        // between us and the end before adding the new page
                        if (m_historyPosition != static_cast<int>(m_historyList.size()) - 1)
                        {
                            m_historyList.erase(m_historyList.begin() + m_historyPosition + 1,
                                m_historyList.end());
                        }
                        wxSharedPtr<wxWebViewHistoryItem> item(new wxWebViewHistoryItem(uri, GetCurrentTitle()));
                        m_historyList.push_back(item);
                        m_historyPosition++;
                    }
                    //Reset as we are done now
                    m_historyLoadingFromList = false;
                }
                return S_OK;
            })
        .Get(), &m_navigationCompletedToken);
    m_webView->add_NewWindowRequested(
        Callback<IWebView2NewWindowRequestedEventHandler>(
            [this](IWebView2WebView* WXUNUSED(sender), IWebView2NewWindowRequestedEventArgs* args) -> HRESULT
            {
                PWSTR uri;
                args->get_Uri(&uri);
                wxString evtURL(uri);
                AddPendingEvent(wxWebViewEvent(wxEVT_WEBVIEW_NEWWINDOW, GetId(), evtURL, wxString()));
                args->put_Handled(true);
                return S_OK;
            }).Get(), &m_newWindowRequestedToken);
    LoadURL(m_pendingURL);
}

void wxWebViewEdge::OnSize(wxSizeEvent& event)
{
    UpdateBounds();
    event.Skip();
}

void wxWebViewEdge::UpdateBounds()
{
    RECT r;
    wxCopyRectToRECT(GetClientRect(), r);
    if (m_webView)
        m_webView->put_Bounds(r);
}

void wxWebViewEdge::LoadURL(const wxString& url)
{
    if (!m_webView)
    {
        m_pendingURL = url;
        return;
    }
    if (FAILED(m_webView->Navigate(url.wc_str())))
        wxLogError("Could not navigate to URL");
}

void wxWebViewEdge::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    int pos = -1;
    for (unsigned int i = 0; i < m_historyList.size(); i++)
    {
        //We compare the actual pointers to find the correct item
        if (m_historyList[i].get() == item.get())
            pos = i;
    }
    // TODO: wxASSERT_MSG(pos != static_cast<int>(m_impl->m_historyList.size()), "invalid history item");
    m_historyLoadingFromList = true;
    LoadURL(item->GetUrl());
    m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = 0; i < m_historyPosition; i++)
    {
        backhist.push_back(m_historyList[i]);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = m_historyPosition + 1; i < static_cast<int>(m_historyList.size()); i++)
    {
        forwardhist.push_back(m_historyList[i]);
    }
    return forwardhist;
}

bool wxWebViewEdge::CanGoForward() const
{
    if (m_historyEnabled)
        return m_historyPosition != static_cast<int>(m_historyList.size()) - 1;
    else
        return false;

    /*
        BOOL result = false;
        if (m_webView && SUCCEEDED(m_webView->get_CanGoForward(&result)))
            return result;
        else
            return false;
    */
}

bool wxWebViewEdge::CanGoBack() const
{
    if (m_historyEnabled)
        return m_historyPosition > 0;
    else
        return false;
    /*
        BOOL result = false;

        if (m_webView && SUCCEEDED(m_webView->get_CanGoBack(&result)))
            return result;
        else
            return false;
    */
}

void wxWebViewEdge::GoBack()
{
    LoadHistoryItem(m_historyList[m_historyPosition - 1]);
    /* if (m_webView)
        m_webView->GoBack(); */
}

void wxWebViewEdge::GoForward()
{
    LoadHistoryItem(m_historyList[m_historyPosition + 1]);
    /* if (m_webView)
        m_webView->GoForward(); */
}

void wxWebViewEdge::ClearHistory()
{
    m_historyList.clear();
    m_historyPosition = -1;
}

void wxWebViewEdge::EnableHistory(bool enable)
{
    m_historyEnabled = enable;
    m_historyList.clear();
    m_historyPosition = -1;
}

void wxWebViewEdge::Stop()
{
    if (m_webView)
        m_webView->Stop();
}

void wxWebViewEdge::Reload(wxWebViewReloadFlags WXUNUSED(flags))
{
    if (m_webView)
        m_webView->Reload();
}

wxString wxWebViewEdge::GetPageSource() const
{
    return wxString();
}

wxString wxWebViewEdge::GetPageText() const
{
    return wxString();
}

bool wxWebViewEdge::IsBusy() const
{
    return m_isBusy;
}

wxString wxWebViewEdge::GetCurrentURL() const
{
    LPWSTR uri;
    if (m_webView && SUCCEEDED(m_webView->get_Source(&uri)))
        return wxString(uri);
    else
        return wxString();
}

wxString wxWebViewEdge::GetCurrentTitle() const
{
    PWSTR _title;
    if (m_webView && SUCCEEDED(m_webView->get_DocumentTitle(&_title)))
        return wxString(_title);
    else
        return wxString();
}

void wxWebViewEdge::SetZoomType(wxWebViewZoomType)
{

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
    double old_zoom_factor = 0.0f;
    m_webView->get_ZoomFactor(&old_zoom_factor);
    if (old_zoom_factor > 1.7f)
        return wxWEBVIEW_ZOOM_LARGEST;
    if (old_zoom_factor > 1.3f)
        return wxWEBVIEW_ZOOM_LARGE;
    if (old_zoom_factor > 0.8f)
        return wxWEBVIEW_ZOOM_MEDIUM;
    if (old_zoom_factor > 0.6f)
        return wxWEBVIEW_ZOOM_SMALL;
    return wxWEBVIEW_ZOOM_TINY;
}

void wxWebViewEdge::SetZoom(wxWebViewZoom zoom)
{
    double old_zoom_factor = 0.0f;
    m_webView->get_ZoomFactor(&old_zoom_factor);
    double zoom_factor = 1.0f;
    switch (zoom)
    {
    case wxWEBVIEW_ZOOM_LARGEST:
        zoom_factor = 2.0f;
        break;
    case wxWEBVIEW_ZOOM_LARGE:
        zoom_factor = 1.5f;
        break;
    case wxWEBVIEW_ZOOM_MEDIUM:
        zoom_factor = 1.0f;
        break;
    case wxWEBVIEW_ZOOM_SMALL:
        zoom_factor = 0.75f;
        break;
    case wxWEBVIEW_ZOOM_TINY:
        zoom_factor = 0.5f;
        break;
    default:
        break;
    }
    m_webView->put_ZoomFactor(zoom_factor);
}

bool wxWebViewEdge::CanCut() const
{
    return false;
}

bool wxWebViewEdge::CanCopy() const
{
    return false;
}

bool wxWebViewEdge::CanPaste() const
{
    return false;
}

void wxWebViewEdge::Cut()
{

}

void wxWebViewEdge::Copy()
{

}

void wxWebViewEdge::Paste()
{

}

bool wxWebViewEdge::CanUndo() const
{
    return false;
}

bool wxWebViewEdge::CanRedo() const
{
    return false;
}

void wxWebViewEdge::Undo()
{

}

void wxWebViewEdge::Redo()
{

}

long wxWebViewEdge::Find(const wxString& WXUNUSED(text), int WXUNUSED(flags))
{
    return -1;
}

//Editing functions
void wxWebViewEdge::SetEditable(bool WXUNUSED(enable))
{

}

bool wxWebViewEdge::IsEditable() const
{
    return false;
}

void wxWebViewEdge::SelectAll()
{

}

bool wxWebViewEdge::HasSelection() const
{
    return false;
}

void wxWebViewEdge::DeleteSelection()
{

}

wxString wxWebViewEdge::GetSelectedText() const
{
    return wxString();
}

wxString wxWebViewEdge::GetSelectedSource() const
{
    return wxString();
}

void wxWebViewEdge::ClearSelection()
{

}

void wxWebViewEdge::EnableContextMenu(bool enable)
{
    wxCOMPtr<IWebView2Settings> settings;
    if (SUCCEEDED(m_webView->get_Settings(&settings)))
    {
        wxCOMPtr<IWebView2Settings2> settings2;
        if (SUCCEEDED(settings->QueryInterface(IID_PPV_ARGS(&settings2))))
        {
            settings2->put_AreDefaultContextMenusEnabled(enable);
        }
    }
}

bool wxWebViewEdge::IsContextMenuEnabled() const
{
    wxCOMPtr<IWebView2Settings> settings;
    if (SUCCEEDED(m_webView->get_Settings(&settings)))
    {
        wxCOMPtr<IWebView2Settings2> settings2;
        if (SUCCEEDED(settings->QueryInterface(IID_PPV_ARGS(&settings2))))
        {
            BOOL menusEnabled = TRUE;
            settings2->get_AreDefaultContextMenusEnabled(&menusEnabled);

            if (!menusEnabled)
                return false;
        }
    }

    return true;
}

void wxWebViewEdge::EnableDevTools(bool enable)
{
    wxCOMPtr<IWebView2Settings> settings;
    if (SUCCEEDED(m_webView->get_Settings(&settings)))
    {
        settings->put_AreDevToolsEnabled(enable);
    }
}

bool wxWebViewEdge::IsDevToolsEnabled() const
{
    wxCOMPtr<IWebView2Settings> settings;
    if (SUCCEEDED(m_webView->get_Settings(&settings)))
    {
        BOOL devToolsEnabled = TRUE;
        settings->get_AreDevToolsEnabled(&devToolsEnabled);

        if (!devToolsEnabled)
            return false;
    }

    return true;
}

bool wxWebViewEdge::RunScriptSync(const wxString& javascript, wxString* output)
{
    bool scriptExecuted = false;

    HRESULT hr = m_webView->ExecuteScript(javascript.wc_str(), Callback<IWebView2ExecuteScriptCompletedHandler>(
        [&scriptExecuted, output](HRESULT error, PCWSTR result) -> HRESULT
    {
        if (error == S_OK)
        {
            if (output)
                output->assign(result);
        }
        else
            wxLogError(_("RunScript failed: %.8x"), error);

        scriptExecuted = true;

        return S_OK;
    }).Get());


    while (!scriptExecuted)
        wxYield();

    return SUCCEEDED(hr);
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
        if (RunScriptSync(wrapJS.GetOutputCode(), &result))
        {
            if (output)
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

}

void wxWebViewEdge::DoSetPage(const wxString& html, const wxString& WXUNUSED(baseUrl))
{
    if (m_webView)
        m_webView->NavigateToString(html.wc_str());
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
        wxWebViewEdge::Uninitalize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxWebViewEdgeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdgeModule, wxModule);

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE
