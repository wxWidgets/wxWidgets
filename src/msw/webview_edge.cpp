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
#include "wx/mstream.h"
#include "wx/log.h"
#include "wx/scopeguard.h"
#include "wx/stdpaths.h"
#include "wx/thread.h"
#include "wx/tokenzr.h"
#include "wx/uilocale.h"
#include "wx/uri.h"
#include "wx/private/jsscriptwrapper.h"
#include "wx/private/json.h"
#include "wx/private/webview.h"
#include "wx/msw/private.h"
#include "wx/msw/private/cotaskmemptr.h"
#include "wx/msw/private/webview_edge.h"
#include "wx/msw/private/comstream.h"

#ifdef __VISUALC__
#include <wrl/event.h>
using namespace Microsoft::WRL;
#include "WebView2EnvironmentOptions.h"
#else
#include <wx/msw/wrl/event.h>
#endif // !__VISUALC__

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdge, wxWebView);

#define WX_ERROR2_CASE(error, wxerror) \
        case error: \
            event.SetString(#error); \
            event.SetInt(wxerror); \
            break;

#if wxUSE_WEBVIEW_EDGE_STATIC
    #define wxCreateCoreWebView2EnvironmentWithOptions ::CreateCoreWebView2EnvironmentWithOptions
    #define wxGetAvailableCoreWebView2BrowserVersionString ::GetAvailableCoreWebView2BrowserVersionString

    // Automatically link the static loader lib with MSVC
    #pragma comment(lib, "WebView2LoaderStatic")
#else
// WebView2Loader typedefs
typedef HRESULT (__stdcall *CreateCoreWebView2EnvironmentWithOptions_t)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions* environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environment_created_handler);
typedef HRESULT(__stdcall *GetAvailableCoreWebView2BrowserVersionString_t)(
    PCWSTR browserExecutableFolder,
    LPWSTR* versionInfo);

CreateCoreWebView2EnvironmentWithOptions_t wxCreateCoreWebView2EnvironmentWithOptions = nullptr;
GetAvailableCoreWebView2BrowserVersionString_t wxGetAvailableCoreWebView2BrowserVersionString = nullptr;
wxDynamicLibrary wxWebViewEdgeImpl::ms_loaderDll;
#endif // wxUSE_WEBVIEW_EDGE_STATIC

class wxWebViewEdgeHandlerRequest : public wxWebViewHandlerRequest
{
public:
    wxWebViewEdgeHandlerRequest(ICoreWebView2WebResourceRequest* request):
        m_request(request),
        m_handler(nullptr),
        m_dataStream(nullptr)
    { }

    ~wxWebViewEdgeHandlerRequest()
    {
        wxDELETE(m_dataStream);
    }

    void SetHandler(wxWebViewHandler* handler) { m_handler = handler; }

    virtual wxString GetRawURI() const override
    {
        wxCoTaskMemPtr<wchar_t> uri;
        if (SUCCEEDED(m_request->get_Uri(&uri)))
            return wxString(uri);
        else
            return wxString();
    }

    virtual wxString GetURI() const override
    {
        wxURI rawURI(GetRawURI());
        wxString path = rawURI.GetPath();
        if (!path.empty()) // Remove / in front
            path.erase(0, 1);
        wxString uri = wxString::Format("%s:%s", m_handler->GetName(), path);
        return uri;
    }

    virtual wxInputStream* GetData() const override
    {
        if (!m_dataStream)
        {
            wxCOMPtr<IStream> dataStream;
            if (SUCCEEDED(m_request->get_Content(&dataStream)) && dataStream)
            {
                ULARGE_INTEGER size;
                LARGE_INTEGER pos;
                pos.QuadPart = 0;
                HRESULT hr = dataStream->Seek(pos, STREAM_SEEK_END, &size);
                if (FAILED(hr))
                    return nullptr;
                hr = dataStream->Seek(pos, STREAM_SEEK_SET, nullptr);
                if (FAILED(hr))
                    return nullptr;
                hr = dataStream->Read(m_data.GetWriteBuf(size.QuadPart), size.QuadPart, nullptr);
                if (FAILED(hr))
                    return nullptr;
                m_dataStream = new wxMemoryInputStream(m_data.GetData(), size.QuadPart);
            }
        }

        return m_dataStream;
    }

    virtual wxString GetMethod() const override
    {
        wxCoTaskMemPtr<wchar_t> method;
        if (SUCCEEDED(m_request->get_Method(&method)))
            return wxString(method);
        else
            return wxString();
    }

    virtual wxString GetHeader(const wxString& name) const override
    {
        wxCOMPtr<ICoreWebView2HttpRequestHeaders> headers;
        if (SUCCEEDED(m_request->get_Headers(&headers)))
        {
            wxCoTaskMemPtr<wchar_t> value;
            if (SUCCEEDED(headers->GetHeader(name.wc_str(), &value)))
                return wxString(value);
        }

        return wxString();
    }

    wxCOMPtr<ICoreWebView2WebResourceRequest> m_request;
    wxWebViewHandler* m_handler;
    mutable wxInputStream* m_dataStream;
    mutable wxMemoryBuffer m_data;
};

class wxWebViewEdgeHandlerResponseStream : public wxCOMInputStreamAdapter
{
public:
    wxWebViewEdgeHandlerResponseStream(wxSharedPtr<wxWebViewHandlerResponseData> data):
        wxCOMInputStreamAdapter(data->GetStream()),
        m_data(data)
    { }

    wxSharedPtr<wxWebViewHandlerResponseData> m_data;
};

class wxWebViewEdgeHandlerResponse : public wxWebViewHandlerResponse
{
public:
    wxWebViewEdgeHandlerResponse(ICoreWebView2WebResourceRequestedEventArgs* args, ICoreWebView2WebResourceResponse* response):
        m_response(response),
        m_args(args)
    {
        HRESULT hr = m_args->GetDeferral(&m_deferral);
        if (FAILED(hr))
            wxLogApiError("GetDeferral", hr);
    }

    void SetReason(const wxString& reason)
    {
        HRESULT hr = m_response->put_ReasonPhrase(reason.wc_str());
        if (FAILED(hr))
            wxLogApiError("put_ReasonPhrase", hr);
    }

    virtual void SetStatus(int status) override
    {
        HRESULT hr = m_response->put_StatusCode(status);
        if (FAILED(hr))
            wxLogApiError("put_StatusCode", hr);
    }

    virtual void SetContentType(const wxString& contentType) override
    { SetHeader("Content-Type", contentType); }

    virtual void SetHeader(const wxString& name, const wxString& value) override
    {
        wxCOMPtr<ICoreWebView2HttpResponseHeaders> headers;
        if (SUCCEEDED(m_response->get_Headers(&headers)))
            headers->AppendHeader(name.wc_str(), value.wc_str());
    }

    bool SendResponse()
    {
        // put response
        HRESULT hr = m_args->put_Response(m_response);
        if (FAILED(hr))
        {
            wxLogApiError("put_Response", hr);
            return false;
        }
        // Mark event as completed
        hr = m_deferral->Complete();
        if (FAILED(hr))
        {
            wxLogApiError("deferral->Complete()", hr);
            return false;
        }

        return true;
    }

    using wxWebViewHandlerResponse::Finish;

    virtual void Finish(wxSharedPtr<wxWebViewHandlerResponseData> data) override
    {
        SetReason("OK");
        // put content
        if (data)
        {
            IStream* stream = new wxWebViewEdgeHandlerResponseStream(data);
            HRESULT hr = m_response->put_Content(stream);
            if (FAILED(hr))
                wxLogApiError("put_Content", hr);
        }
        SendResponse();
    }

    virtual void FinishWithError() override
    {
        SetStatus(500);
        SetReason("Error");
        SendResponse();
    }

    wxCOMPtr<ICoreWebView2WebResourceResponse> m_response;
    wxCOMPtr<ICoreWebView2Deferral> m_deferral;
    wxCOMPtr<ICoreWebView2WebResourceRequestedEventArgs> m_args;
};

// wxWebViewConfigurationImplEdge
class wxWebViewConfigurationImplEdge : public wxWebViewConfigurationImpl
{
public:
    wxWebViewConfigurationImplEdge(ICoreWebView2Environment* environment = nullptr):
        m_webViewEnvironment(environment)
    {
        m_dataPath = wxStandardPaths::Get().GetUserLocalDataDir();
#ifdef __VISUALC__
        m_webViewEnvironmentOptions = Make<CoreWebView2EnvironmentOptions>().Get();
        m_webViewEnvironmentOptions->put_Language(wxUILocale::GetCurrent().GetLocaleId().GetName().wc_str());

        wxCOMPtr<ICoreWebView2EnvironmentOptions3> options3;
        if (SUCCEEDED(m_webViewEnvironmentOptions->QueryInterface(IID_PPV_ARGS(&options3))))
            options3->put_IsCustomCrashReportingEnabled(false);
#endif
    }

    bool SetProxy(const wxString& proxy)
    {
#ifdef __VISUALC__
        m_webViewEnvironmentOptions->put_AdditionalBrowserArguments(
            wxString::Format("--proxy-server=\"%s\"", proxy).wc_str()
        );
        return true;
#else
        wxUnusedVar(proxy);

        wxLogError(_("This program was compiled without support for setting Edge proxy."));

        return false;
#endif
    }

    virtual void* GetNativeConfiguration() const override
    {
        return m_webViewEnvironmentOptions;
    }

    virtual void SetDataPath(const wxString& path) override { m_dataPath = path;}
    virtual wxString GetDataPath() const override { return m_dataPath; }

    virtual bool EnablePersistentStorage(bool enable) override
    {
        m_persistentStorage = enable;
        return true;
    }

    bool CreateOrGetEnvironment(wxWebViewEdgeImpl* impl)
    {
        if (!m_webViewEnvironment)
        {
            m_webViewsWaitingForEnvironment.push_back(impl);
            HRESULT hr = wxCreateCoreWebView2EnvironmentWithOptions(
                ms_browserExecutableDir.wc_str(),
                GetDataPath().wc_str(),
                m_webViewEnvironmentOptions,
                Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this,
                    &wxWebViewConfigurationImplEdge::OnEnvironmentCreated).Get());
            if (FAILED(hr))
            {
                wxLogApiError("CreateWebView2EnvironmentWithOptions", hr);
                return false;
            }
            else
                return true;
        }
        else
        {
            impl->EnvironmentAvailable(m_webViewEnvironment);
            return true;
        }
    }

    HRESULT OnEnvironmentCreated(HRESULT WXUNUSED(result), ICoreWebView2Environment* environment)
    {
        m_webViewEnvironment = environment;
        for (auto impl : m_webViewsWaitingForEnvironment)
            impl->EnvironmentAvailable(m_webViewEnvironment);
        m_webViewsWaitingForEnvironment.clear();
        return S_OK;
    }

    static wxString ms_browserExecutableDir;
    std::vector<wxWebViewEdgeImpl*> m_webViewsWaitingForEnvironment;
    wxCOMPtr<ICoreWebView2EnvironmentOptions> m_webViewEnvironmentOptions;
    wxCOMPtr<ICoreWebView2Environment> m_webViewEnvironment;
    wxString m_dataPath;
    bool m_persistentStorage = true;
};

wxString wxWebViewConfigurationImplEdge::ms_browserExecutableDir;

// wxWebViewWindowFeaturesEdge

class wxWebViewWindowFeaturesEdge : public wxWebViewWindowFeatures
{
public:
    wxWebViewWindowFeaturesEdge(wxWebView* childWebView, ICoreWebView2NewWindowRequestedEventArgs* args):
        wxWebViewWindowFeatures(childWebView),
        m_args(args)
    {
        m_args->get_WindowFeatures(&m_windowFeatures);
    }

    virtual wxPoint GetPosition() const override
    {
        wxPoint result(-1, -1);
        BOOL hasPosition;
        if (SUCCEEDED(m_windowFeatures->get_HasPosition(&hasPosition)) && hasPosition)
        {
            UINT32 x, y;
            if (SUCCEEDED(m_windowFeatures->get_Left(&x)) &&
                SUCCEEDED(m_windowFeatures->get_Top(&y)))
                result = wxPoint(x, y);
        }
        return result;
    }

    virtual wxSize GetSize() const override
    {
        wxSize result(-1, -1);
        BOOL hasSize;
        if (SUCCEEDED(m_windowFeatures->get_HasSize(&hasSize)) && hasSize)
        {
            UINT32 width, height;
            if (SUCCEEDED(m_windowFeatures->get_Width(&width)) &&
                SUCCEEDED(m_windowFeatures->get_Height(&height)))
                result = wxSize(width, height);
        }
        return result;
    }

    virtual bool ShouldDisplayMenuBar() const override
    {
        BOOL result;
        if (SUCCEEDED(m_windowFeatures->get_ShouldDisplayMenuBar(&result)))
            return result;
        else
            return true;
    }

    virtual bool ShouldDisplayStatusBar() const override
    {
        BOOL result;
        if (SUCCEEDED(m_windowFeatures->get_ShouldDisplayStatus(&result)))
            return result;
        else
            return true;
    }
    virtual bool ShouldDisplayToolBar() const override
    {
        BOOL result;
        if (SUCCEEDED(m_windowFeatures->get_ShouldDisplayToolbar(&result)))
            return result;
        else
            return true;
    }

    virtual bool ShouldDisplayScrollBars() const override
    {
        BOOL result;
        if (SUCCEEDED(m_windowFeatures->get_ShouldDisplayScrollBars(&result)))
            return result;
        else
            return true;
    }

private:
    wxCOMPtr<ICoreWebView2NewWindowRequestedEventArgs> m_args;
    wxCOMPtr<ICoreWebView2WindowFeatures> m_windowFeatures;
};

#define wxWEBVIEW_EDGE_EVENT_HANDLER_METHOD \
    m_inEventCallback = true; \
    wxON_BLOCK_EXIT_SET(m_inEventCallback, false);

wxWebViewEdgeImpl::wxWebViewEdgeImpl(wxWebViewEdge* webview, const wxWebViewConfiguration& config):
    m_ctrl(webview),
    m_config(config)
{
}

wxWebViewEdgeImpl::wxWebViewEdgeImpl(wxWebViewEdge* webview) :
    m_ctrl(webview),
    m_config(wxWebViewConfiguration(wxWebViewBackendEdge, new wxWebViewConfigurationImplEdge))
{
}

wxWebViewEdgeImpl::~wxWebViewEdgeImpl()
{
    if (m_webView)
    {
        m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
        m_webView->remove_SourceChanged(m_sourceChangedToken);
        m_webView->remove_NavigationStarting(m_navigationStartingToken);
        m_webView->remove_FrameNavigationStarting(m_frameNavigationStartingToken);
        m_webView->remove_NewWindowRequested(m_newWindowRequestedToken);
        m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);
        m_webView->remove_DOMContentLoaded(m_DOMContentLoadedToken);
        m_webView->remove_ContainsFullScreenElementChanged(m_containsFullScreenElementChangedToken);
        m_webView->remove_WebMessageReceived(m_webMessageReceivedToken);
        m_webView->remove_WebResourceRequested(m_webResourceRequestedToken);
        m_webView->remove_WindowCloseRequested(m_windowCloseRequestedToken);
    }
}

bool wxWebViewEdgeImpl::Create()
{
    m_initialized = false;
    m_isBusy = false;
    m_inEventCallback = false;
    m_pendingContextMenuEnabled = -1;
    m_pendingAccessToDevToolsEnabled = 0;
    m_pendingEnableBrowserAcceleratorKeys = -1;

    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;

    return static_cast<wxWebViewConfigurationImplEdge*>(m_config.GetImpl())->
        CreateOrGetEnvironment(this);
}

void wxWebViewEdgeImpl::EnvironmentAvailable(ICoreWebView2Environment* environment)
{
    environment->QueryInterface(IID_PPV_ARGS(&m_webViewEnvironment));
    wxCOMPtr<ICoreWebView2Environment10> environment10;
    if (SUCCEEDED(m_webViewEnvironment->QueryInterface(IID_PPV_ARGS(&environment10))))
    {
        wxCOMPtr<ICoreWebView2ControllerOptions> controllerOptions;
        if (SUCCEEDED(environment10->CreateCoreWebView2ControllerOptions(&controllerOptions)))
        {
            auto config = static_cast<wxWebViewConfigurationImplEdge*>(m_config.GetImpl());
            controllerOptions->put_IsInPrivateModeEnabled(config->m_persistentStorage ? FALSE : TRUE);

            environment10->CreateCoreWebView2ControllerWithOptions(
                m_ctrl->GetHWND(),
                controllerOptions.get(),
                Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    this, &wxWebViewEdgeImpl::OnWebViewCreated).Get());
        }
    }
    else
        m_webViewEnvironment->CreateCoreWebView2Controller(
            m_ctrl->GetHWND(),
            Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                this, &wxWebViewEdgeImpl::OnWebViewCreated).Get());
}

bool wxWebViewEdgeImpl::Initialize()
{
#if !wxUSE_WEBVIEW_EDGE_STATIC
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
#endif
    // Check if a Edge browser can be found by the loader DLL
    wxCoTaskMemPtr<wchar_t> versionStr;
    HRESULT hr = wxGetAvailableCoreWebView2BrowserVersionString(
        wxWebViewConfigurationImplEdge::ms_browserExecutableDir.wc_str(), &versionStr);
    if (FAILED(hr) || !versionStr)
    {
        wxLogApiError("GetCoreWebView2BrowserVersionInfo", hr);
        return false;
    }

#if !wxUSE_WEBVIEW_EDGE_STATIC
    ms_loaderDll.Attach(loaderDll.Detach());
#endif

    return true;
}

void wxWebViewEdgeImpl::Uninitialize()
{
#if !wxUSE_WEBVIEW_EDGE_STATIC
    ms_loaderDll.Unload();
#endif
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
    return HandleNavigationStarting(args, true);
}

HRESULT wxWebViewEdgeImpl::OnFrameNavigationStarting(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NavigationStartingEventArgs* args)
{
    return HandleNavigationStarting(args, false);
}

HRESULT wxWebViewEdgeImpl::HandleNavigationStarting(ICoreWebView2NavigationStartingEventArgs* args, bool mainFrame)
{
    wxWEBVIEW_EDGE_EVENT_HANDLER_METHOD
    m_isBusy = true;
    wxString evtURL;
    wxCoTaskMemPtr<wchar_t> uri;
    if (SUCCEEDED(args->get_Uri(&uri)))
        evtURL = wxString(uri);

    wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, m_ctrl->GetId(), evtURL, wxString());
    event.SetEventObject(m_ctrl);
    if (mainFrame)
        event.SetInt(1);
    m_ctrl->HandleWindowEvent(event);

    if (!event.IsAllowed())
        args->put_Cancel(true);

    return S_OK;
}

void wxWebViewEdgeImpl::SendErrorEventForAPI(const wxString& api, HRESULT errorCode)
{
    wxLogApiError(api, errorCode);

    wxWebViewEvent event(wxEVT_WEBVIEW_ERROR, m_ctrl->GetId(), wxString(), wxString());
    event.SetEventObject(m_ctrl);
    event.SetInt(wxWEBVIEW_NAV_ERR_OTHER);
    event.SetString(wxSysErrorMsgStr(errorCode));

    m_ctrl->GetEventHandler()->AddPendingEvent(event);
}

HRESULT wxWebViewEdgeImpl::OnSourceChanged(ICoreWebView2 * WXUNUSED(sender), ICoreWebView2SourceChangedEventArgs * args)
{
    BOOL isNewDocument;
    if (SUCCEEDED(args->get_IsNewDocument(&isNewDocument)) && !isNewDocument)
    {
        // navigation within the current document, send apropriate events
        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, m_ctrl->GetId(), m_ctrl->GetCurrentURL(), wxString());
        event.SetEventObject(m_ctrl);
        m_ctrl->GetEventHandler()->AddPendingEvent(event);
        OnNavigationCompleted(nullptr, nullptr);
        OnDOMContentLoaded(nullptr, nullptr);
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
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_VALID_AUTHENTICATION_CREDENTIALS_REQUIRED, wxWEBVIEW_NAV_ERR_AUTH)
                WX_ERROR2_CASE(COREWEBVIEW2_WEB_ERROR_STATUS_VALID_PROXY_AUTHENTICATION_REQUIRED, wxWEBVIEW_NAV_ERR_AUTH)
            case COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED:
                // This status is triggered by vetoing a wxEVT_WEBVIEW_NAVIGATING event
                ignoreStatus = true;
                break;
            }
        }
        if (!ignoreStatus)
            m_ctrl->GetEventHandler()->AddPendingEvent(event);
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
        m_ctrl->GetEventHandler()->AddPendingEvent(evt);
    }
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnNewWindowRequested(ICoreWebView2* WXUNUSED(sender), ICoreWebView2NewWindowRequestedEventArgs* args)
{
    wxWEBVIEW_EDGE_EVENT_HANDLER_METHOD
    wxCoTaskMemPtr<wchar_t> uri;
    wxString evtURL;
    if (SUCCEEDED(args->get_Uri(&uri)))
        evtURL = wxString(uri);
    wxWebViewNavigationActionFlags navFlags = wxWEBVIEW_NAV_ACTION_OTHER;

    BOOL isUserInitiated;
    if (SUCCEEDED(args->get_IsUserInitiated(&isUserInitiated)) && isUserInitiated)
        navFlags = wxWEBVIEW_NAV_ACTION_USER;

    wxWebViewEvent newWindowEvent(wxEVT_WEBVIEW_NEWWINDOW, m_ctrl->GetId(), evtURL, wxString(), navFlags, "");
    m_ctrl->HandleWindowEvent(newWindowEvent);
    args->put_Handled(true);

    if (newWindowEvent.IsAllowed())
    {
        wxWebViewEdge* childWebView = new wxWebViewEdge(m_config);
        childWebView->m_impl->m_newWindowArgs = args;
        HRESULT hr = args->GetDeferral(&childWebView->m_impl->m_newWindowDeferral);
        if (FAILED(hr))
            wxLogApiError("GetDeferral", hr);

        wxWebViewWindowFeaturesEdge windowFeatures(childWebView, args);
        wxWebViewEvent featuresEvent(wxEVT_WEBVIEW_NEWWINDOW_FEATURES, m_ctrl->GetId(), evtURL, wxString(), navFlags, "");
        featuresEvent.SetClientData(&windowFeatures);
        m_ctrl->HandleWindowEvent(featuresEvent);
    }

    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnWindowCloseRequested(ICoreWebView2* WXUNUSED(sender), IUnknown* WXUNUSED(args))
{
    wxWebViewEvent evt(wxEVT_WEBVIEW_WINDOW_CLOSE_REQUESTED, m_ctrl->GetId(), m_ctrl->GetCurrentURL(), "");
    m_ctrl->GetEventHandler()->AddPendingEvent(evt);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnDocumentTitleChanged(ICoreWebView2* WXUNUSED(sender), IUnknown* WXUNUSED(args))
{
    wxWebViewEvent event(wxEVT_WEBVIEW_TITLE_CHANGED,
        m_ctrl->GetId(), m_ctrl->GetCurrentURL(), "");
    event.SetString(m_ctrl->GetCurrentTitle());
    event.SetEventObject(m_ctrl);
    m_ctrl->GetEventHandler()->AddPendingEvent(event);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnDOMContentLoaded(ICoreWebView2* WXUNUSED(sender), ICoreWebView2DOMContentLoadedEventArgs* WXUNUSED(args))
{
    wxWebViewEvent event(wxEVT_WEBVIEW_LOADED, m_ctrl->GetId(),
        m_ctrl->GetCurrentURL(), "");
    event.SetEventObject(m_ctrl);
    m_ctrl->GetEventHandler()->AddPendingEvent(event);
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
    m_ctrl->GetEventHandler()->AddPendingEvent(event);

    return S_OK;
}

HRESULT
wxWebViewEdgeImpl::OnWebMessageReceived(ICoreWebView2* WXUNUSED(sender),
                                        ICoreWebView2WebMessageReceivedEventArgs* args)
{
    wxCoTaskMemPtr<wchar_t> msgContent;

    HRESULT hr = args->get_WebMessageAsJson(&msgContent);
    if (FAILED(hr))
    {
        wxLogApiError("get_WebMessageAsJson", hr);
        return hr;
    }

    wxWebViewEvent event(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, m_ctrl->GetId(),
        m_ctrl->GetCurrentURL(), wxString(),
        wxWEBVIEW_NAV_ACTION_NONE, m_scriptMsgHandlerName);
    event.SetEventObject(m_ctrl);

    // Try to decode JSON string or return original
    // result if it's not a valid JSON string
    wxString msgStr;
    wxString msgJson(msgContent);
    if (!wxJSON::DecodeString(msgJson, &msgStr))
        msgStr = msgJson;
    event.SetString(msgStr);

    m_ctrl->GetEventHandler()->AddPendingEvent(event);

    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnWebResourceRequested(ICoreWebView2* WXUNUSED(sender), ICoreWebView2WebResourceRequestedEventArgs* args)
{
    wxCOMPtr<ICoreWebView2WebResourceRequest> apiRequest;
    HRESULT hr = args->get_Request(&apiRequest);
    if (FAILED(hr))
        return hr;
    wxWebViewEdgeHandlerRequest request(apiRequest);

    // Find handler
    wxURI uri(request.GetRawURI());
    if (!uri.HasServer())
        return E_INVALIDARG;
    wxSharedPtr<wxWebViewHandler> handler = m_handlers[uri.GetServer()];
    if (!handler)
        return E_INVALIDARG;
    request.SetHandler(handler.get());

    // Create response
    wxCOMPtr<ICoreWebView2WebResourceResponse> runtimeResponse;
    hr = m_webViewEnvironment->CreateWebResourceResponse(nullptr, 200, L"OK", nullptr, &runtimeResponse);
    if (FAILED(hr))
    {
        wxLogApiError("CreateWebResourceResponse", hr);
        return hr;
    }

    wxSharedPtr<wxWebViewHandlerResponse> resp(new wxWebViewEdgeHandlerResponse(args, runtimeResponse));
    handler->StartRequest(request, resp);
    return S_OK;
}

HRESULT wxWebViewEdgeImpl::OnWebViewCreated(HRESULT result, ICoreWebView2Controller* webViewController)
{
    if (FAILED(result))
    {
        SendErrorEventForAPI("WebView2::WebViewCreated", result);
        return result;
    }

    wxCOMPtr<ICoreWebView2> baseWebView;
    HRESULT hr = webViewController->get_CoreWebView2(&baseWebView);
    if (FAILED(hr))
    {
        SendErrorEventForAPI("WebView2::WebViewCreated (get_CoreWebView2)", hr);
        return result;
    }
    hr = baseWebView->QueryInterface(IID_PPV_ARGS(&m_webView));
    if (FAILED(hr))
    {
        SendErrorEventForAPI("WebView2::WebViewCreated (QueryInterface)", hr);
        return result;
    }

    m_webViewController = webViewController;

    m_initialized = true;
    UpdateBounds();
    m_webViewController->put_IsVisible(true);

    // Connect and handle the various WebView events
    m_webView->add_NavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            this, &wxWebViewEdgeImpl::OnNavigationStarting).Get(),
        &m_navigationStartingToken);
    m_webView->add_FrameNavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            this, &wxWebViewEdgeImpl::OnFrameNavigationStarting).Get(),
        &m_frameNavigationStartingToken);
    m_webView->add_SourceChanged(
        Callback<ICoreWebView2SourceChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnSourceChanged).Get(),
        &m_sourceChangedToken);
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
    m_webView->add_DOMContentLoaded(
        Callback<ICoreWebView2DOMContentLoadedEventHandler>(
            this, &wxWebViewEdgeImpl::OnDOMContentLoaded).Get(),
        &m_DOMContentLoadedToken);
    m_webView->add_ContainsFullScreenElementChanged(
        Callback<ICoreWebView2ContainsFullScreenElementChangedEventHandler>(
            this, &wxWebViewEdgeImpl::OnContainsFullScreenElementChanged).Get(),
        &m_containsFullScreenElementChangedToken);
    m_webView->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            this, &wxWebViewEdgeImpl::OnWebMessageReceived).Get(),
        &m_webMessageReceivedToken);
    m_webView->add_WebResourceRequested(
        Callback<ICoreWebView2WebResourceRequestedEventHandler>(
            this, &wxWebViewEdgeImpl::OnWebResourceRequested).Get(),
        &m_webResourceRequestedToken);
    m_webView->add_WindowCloseRequested(
        Callback<ICoreWebView2WindowCloseRequestedEventHandler>(
            this, &wxWebViewEdgeImpl::OnWindowCloseRequested).Get(),
        &m_windowCloseRequestedToken);

    // Register handlers
    for (const auto& kv : m_handlers)
    {
        wxString filterURI = wxString::Format("*://%s/*", kv.first);
        m_webView->AddWebResourceRequestedFilter(filterURI.wc_str(), COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
    }

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

    if (m_pendingEnableBrowserAcceleratorKeys != -1)
    {
        m_ctrl->EnableBrowserAcceleratorKeys(m_pendingEnableBrowserAcceleratorKeys == 1);
        m_pendingEnableBrowserAcceleratorKeys = -1;
    }

    if (!m_pendingUserAgent.empty())
    {
        m_ctrl->SetUserAgent(m_pendingUserAgent);
        m_pendingUserAgent.clear();
    }

    wxCOMPtr<ICoreWebView2Settings> settings(GetSettings());
    if (settings)
    {
        settings->put_IsStatusBarEnabled(false);

        wxCOMPtr<ICoreWebView2Settings8> settings8;
        if (SUCCEEDED(settings->QueryInterface(IID_PPV_ARGS(&settings8))))
            settings8->put_IsReputationCheckingRequired(false);
    }
    UpdateWebMessageHandler();

    if (!m_pendingUserScripts.empty())
    {
        for (wxVector<wxString>::iterator it = m_pendingUserScripts.begin();
            it != m_pendingUserScripts.end(); ++it)
            m_ctrl->AddUserScript(*it);
        m_pendingUserScripts.clear();
    }

    if (m_newWindowArgs)
    {
        if (FAILED(m_newWindowArgs->put_NewWindow(baseWebView)))
            SendErrorEventForAPI("WebView2::WebViewCreated (put_NewWindow)", hr);
        if (FAILED(m_newWindowDeferral->Complete()))
            SendErrorEventForAPI("WebView2::WebViewCreated (Complete)", hr);
        m_newWindowArgs.reset();
        m_newWindowDeferral.reset();

        return S_OK;
    }

    if (!m_pendingURL.empty())
    {
        m_ctrl->LoadURL(m_pendingURL);
        m_pendingURL.clear();
    }

    if (!m_pendingPage.empty())
    {
        m_ctrl->SetPage(m_pendingPage, "");
        m_pendingPage.clear();
    }

    return S_OK;
}

void wxWebViewEdgeImpl::UpdateWebMessageHandler()
{
    wxCOMPtr<ICoreWebView2Settings> settings(GetSettings());
    if (!settings)
        return;

    settings->put_IsWebMessageEnabled(!m_scriptMsgHandlerName.empty());

    if (!m_scriptMsgHandlerName.empty())
    {
        // Make edge message handler available under common name
        wxString js = wxString::Format("window.%s = window.chrome.webview;",
            m_scriptMsgHandlerName);
        m_ctrl->AddUserScript(js);
        m_webView->ExecuteScript(js.wc_str(), nullptr);
    }
}

ICoreWebView2Settings* wxWebViewEdgeImpl::GetSettings()
{
    if (!m_webView)
        return nullptr;

    ICoreWebView2Settings* settings;
    HRESULT hr = m_webView->get_Settings(&settings);
    if (FAILED(hr))
    {
        wxLogApiError("WebView2::get_Settings", hr);
        return nullptr;
    }

    return settings;
}

ICoreWebView2Settings3* wxWebViewEdgeImpl::GetSettings3()
{
    ICoreWebView2Settings3* settings3 = nullptr;
    wxCOMPtr<ICoreWebView2Settings> settings(GetSettings());
    if (settings)
    {
        HRESULT hr = settings->QueryInterface(IID_PPV_ARGS(&settings3));
        if (FAILED(hr))
        {
            wxLogApiError("WebView2::get_Settings3", hr);
            return nullptr;
        }
    }

    return settings3;
}

wxWebViewEdge::wxWebViewEdge():
    m_impl(new wxWebViewEdgeImpl(this))
{

}

wxWebViewEdge::wxWebViewEdge(const wxWebViewConfiguration& config):
    m_impl(new wxWebViewEdgeImpl(this, config))
{
}

wxWebViewEdge::wxWebViewEdge(wxWindow* parent,
    wxWindowID id,
    const wxString& url,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name):
    m_impl(new wxWebViewEdgeImpl(this))
{
    Create(parent, id, url, pos, size, style, name);
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

    MSWDisableComposited();

    if (!m_impl->Create())
        return false;
    Bind(wxEVT_SIZE, &wxWebViewEdge::OnSize, this);
    Bind(wxEVT_SET_FOCUS, &wxWebViewEdge::OnSetFocus, this);
    wxWindow* topLevelParent = wxGetTopLevelParent(this);
    if (topLevelParent)
        topLevelParent->Bind(wxEVT_ICONIZE, &wxWebViewEdge::OnTopLevelParentIconized, this);

    NotifyWebViewCreated();

    LoadURL(url);
    return true;
}

void wxWebViewEdge::OnSize(wxSizeEvent& event)
{
    m_impl->UpdateBounds();
    event.Skip();
}

void wxWebViewEdge::OnSetFocus(wxFocusEvent& event)
{
    if (m_impl && m_impl->m_webViewController)
        m_impl->m_webViewController->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
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
        m_impl->m_pendingPage.clear();
        m_impl->m_pendingURL = url;
        return;
    }
    wxString navURL = url;
    if (!m_impl->m_handlers.empty())
    {
        // Emulate custom protocol support for LoadURL()
        for (const auto& kv : m_impl->m_handlers)
        {
            wxString scheme = kv.second->GetName() + ":";
            if (navURL.StartsWith(scheme))
            {
                navURL.Remove(0, scheme.Length());
                navURL.insert(0, "https://" + kv.second->GetVirtualHost() + "/");
                break;
            }
        }
    }
    HRESULT hr = m_impl->m_webView->Navigate(navURL.wc_str());
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

bool wxWebViewEdge::ShowDevTools()
{
    const HRESULT hr = m_impl->m_webView->OpenDevToolsWindow();
    if ( FAILED(hr) )
    {
        wxLogApiError("ICoreWebView2::OpenDevToolsWindow", hr);
        return false;
    }

    return true;
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

void wxWebViewEdge::EnableBrowserAcceleratorKeys(bool enable)
{
    wxCOMPtr<ICoreWebView2Settings3> settings(m_impl->GetSettings3());
    if (settings)
        settings->put_AreBrowserAcceleratorKeysEnabled(enable);
    else
        m_impl->m_pendingEnableBrowserAcceleratorKeys = enable ? 1 : 0;
}

bool wxWebViewEdge::AreBrowserAcceleratorKeysEnabled() const
{
    wxCOMPtr<ICoreWebView2Settings3> settings(m_impl->GetSettings3());
    if (settings)
    {
        BOOL browserAcceleratorKeysEnabled = TRUE;
        settings->get_AreBrowserAcceleratorKeysEnabled(&browserAcceleratorKeysEnabled);

        if (!browserAcceleratorKeysEnabled)
            return false;
    }

    return true;
}


bool wxWebViewEdge::SetUserAgent(const wxString& userAgent)
{
    wxCOMPtr<ICoreWebView2Settings3> settings(m_impl->GetSettings3());
    if (settings)
        return SUCCEEDED(settings->put_UserAgent(userAgent.wc_str()));
    else
        m_impl->m_pendingUserAgent = userAgent;

    return true;
}

wxString wxWebViewEdge::GetUserAgent() const
{
    wxCOMPtr<ICoreWebView2Settings3> settings(m_impl->GetSettings3());
    if (settings)
    {
        wxCoTaskMemPtr<wchar_t> userAgent;
        if (SUCCEEDED(settings->get_UserAgent(&userAgent)))
            return wxString(userAgent);
    }

    return wxString{};
}


bool wxWebViewEdge::SetProxy(const wxString& proxy)
{
    wxCHECK_MSG(!m_impl->m_webViewController, false,
                "Proxy must be set before calling Create()");

    auto configImpl = static_cast<wxWebViewConfigurationImplEdge*>(m_impl->m_config.GetImpl());

    return configImpl->SetProxy(proxy);
}

void* wxWebViewEdge::GetNativeBackend() const
{
    return m_impl->m_webView;
}

void wxWebViewEdge::MSWSetBrowserExecutableDir(const wxString & path)
{
    wxWebViewConfigurationImplEdge::ms_browserExecutableDir = path;
}

bool wxWebViewEdge::RunScript(const wxString& javascript, wxString* output) const
{
    wxCHECK_MSG(!m_impl->m_inEventCallback, false,
        "RunScript() cannot be used during event callbacks. Consider using RunScriptAsync()");
    return wxWebView::RunScript(javascript, output);
}

void wxWebViewEdge::RunScriptAsync(const wxString& javascript, void* clientData) const
{
    if (!m_impl->m_webView)
    {
        SendScriptResult(clientData, false, "");
        return; // TODO: postpone execution
    }

    wxJSScriptWrapper wrapJS(javascript, wxJSScriptWrapper::JS_OUTPUT_STRING);

    // Start script execution
    HRESULT executionResult = m_impl->m_webView->ExecuteScript(wrapJS.GetWrappedCode().wc_str(), Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
        [this, clientData](HRESULT error, PCWSTR result) -> HRESULT
    {
        // Handle script execution callback
        if (error == S_OK)
        {
            wxString scriptDecodedResult;
            // Try to decode JSON string or return original
            // result if it's not a valid JSON string
            if (!wxJSON::DecodeString(result, &scriptDecodedResult))
                scriptDecodedResult = result;

            wxString scriptExtractedOutput;
            bool success = wxJSScriptWrapper::ExtractOutput(scriptDecodedResult, &scriptExtractedOutput);
            SendScriptResult(clientData, success, scriptExtractedOutput);
        }
        else
        {
            SendScriptResult(clientData, false, wxString::Format("%s (0x%08lx)",
                wxSysErrorMsgStr(error), error));
        }

        return S_OK;
    }).Get());
    if (FAILED(executionResult))
    {
        SendScriptResult(clientData, false, wxString::Format("%s (0x%08lx)",
            wxSysErrorMsgStr(executionResult), executionResult));
    }
}

bool wxWebViewEdge::AddScriptMessageHandler(const wxString& name)
{
    // Edge only supports a single message handler
    if (!m_impl->m_scriptMsgHandlerName.empty())
        return false;

    m_impl->m_scriptMsgHandlerName = name;
    m_impl->UpdateWebMessageHandler();

    return true;
}

bool wxWebViewEdge::RemoveScriptMessageHandler(const wxString& WXUNUSED(name))
{
    m_impl->m_scriptMsgHandlerName.clear();
    m_impl->UpdateWebMessageHandler();
    return true;
}

HRESULT wxWebViewEdgeImpl::OnAddScriptToExecuteOnDocumentedCreatedCompleted(HRESULT errorCode, LPCWSTR id)
{
    if (SUCCEEDED(errorCode))
        m_userScriptIds.push_back(id);
    return S_OK;
}

bool wxWebViewEdge::AddUserScript(const wxString& javascript,
    wxWebViewUserScriptInjectionTime injectionTime)
{
    // Currently only AT_DOCUMENT_START is supported
    if (injectionTime != wxWEBVIEW_INJECT_AT_DOCUMENT_START)
        return false;

    if (m_impl->m_webView)
    {
        HRESULT hr = m_impl->m_webView->AddScriptToExecuteOnDocumentCreated(javascript.wc_str(),
            Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(m_impl,
            &wxWebViewEdgeImpl::OnAddScriptToExecuteOnDocumentedCreatedCompleted).Get());
        if (FAILED(hr))
            return false;
    }
    else
        m_impl->m_pendingUserScripts.push_back(javascript);

    return true;
}

void wxWebViewEdge::RemoveAllUserScripts()
{
    m_impl->m_pendingUserScripts.clear();
    for (auto& scriptId : m_impl->m_userScriptIds)
    {
        HRESULT hr = m_impl->m_webView->RemoveScriptToExecuteOnDocumentCreated(scriptId.wc_str());
        if (FAILED(hr))
            wxLogApiError("RemoveScriptToExecuteOnDocumentCreated", hr);
    }
    m_impl->m_userScriptIds.clear();
}

void wxWebViewEdge::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    wxString handlerHost = handler->GetVirtualHost();
    m_impl->m_handlers[handlerHost] = handler;

    if (m_impl->m_webView)
    {
        wxString filterURI = wxString::Format("*://%s/*", handlerHost);
        m_impl->m_webView->AddWebResourceRequestedFilter(filterURI.wc_str(), COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
    }
}

void wxWebViewEdge::DoSetPage(const wxString& html, const wxString& WXUNUSED(baseUrl))
{
    if (!m_impl->m_webView)
    {
        m_impl->m_pendingPage = html;
        m_impl->m_pendingURL.clear();
        return;
    }
    HRESULT hr = m_impl->m_webView->NavigateToString(html.wc_str());
    if (FAILED(hr))
        wxLogApiError("WebView2::NavigateToString", hr);
}

// wxWebViewFactoryEdge

wxWebView* wxWebViewFactoryEdge::CreateWithConfig(const wxWebViewConfiguration& config)
{
    return new wxWebViewEdge(config);
}

bool wxWebViewFactoryEdge::IsAvailable()
{
    return wxWebViewEdgeImpl::Initialize();
}

wxVersionInfo wxWebViewFactoryEdge::GetVersionInfo(wxVersionContext context)
{
    switch ( context )
    {
        case wxVersionContext::BuildTime:
            // There is no build-time version for this backend.
            break;

        case wxVersionContext::RunTime:
            long major = 0,
                 minor = 0,
                 micro = 0,
                 revision = 0;

            if (wxWebViewEdgeImpl::Initialize())
            {
                wxCoTaskMemPtr<wchar_t> nativeVersionStr;
                HRESULT hr = wxGetAvailableCoreWebView2BrowserVersionString(
                    wxWebViewConfigurationImplEdge::ms_browserExecutableDir.wc_str(), &nativeVersionStr);
                if (SUCCEEDED(hr) && nativeVersionStr)
                {
                    wxStringTokenizer tk(wxString(nativeVersionStr), ". ");
                    // Ignore the return value because if the version component is missing
                    // or invalid (i.e. non-numeric), the only thing we can do is to ignore
                    // it anyhow.
                    tk.GetNextToken().ToLong(&major);
                    tk.GetNextToken().ToLong(&minor);
                    tk.GetNextToken().ToLong(&micro);
                    tk.GetNextToken().ToLong(&revision);
                }
            }

            return wxVersionInfo("Microsoft Edge WebView2", major, minor, micro, revision);
    }

    return {};
}

wxWebViewConfiguration wxWebViewFactoryEdge::CreateConfiguration()
{
    return wxWebViewConfiguration(wxWebViewBackendEdge, new wxWebViewConfigurationImplEdge);
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

    virtual bool OnInit() override
    {
        return true;
    }

    virtual void OnExit() override
    {
        wxWebViewEdgeImpl::Uninitialize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxWebViewEdgeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdgeModule, wxModule);

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE
