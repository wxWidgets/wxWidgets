/////////////////////////////////////////////////////////////////////////////
// Name:        source/msw/webview_edge.h
// Purpose:     wxMSW Edge wxWebView backend implementation
// Author:      Tobias Taschner
// Created:     2018-05-10
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/msw/webview_edge.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE

#include "wx/module.h"
#include "wx/msw/rt/utils.h"
#include "wx/msw/wrapshl.h"
#include "wx/msw/ole/comimpl.h"

#include <wrl/event.h>

namespace rt = wxWinRT;

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Web::UI;
using namespace ABI::Windows::Web::UI::Interop;

using namespace Microsoft::WRL;


wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEdge, wxWebView);

wxBEGIN_EVENT_TABLE(wxWebViewEdge, wxControl)
    EVT_SIZE(wxWebViewEdge::OnSize)
wxEND_EVENT_TABLE()

#define WX_ERROR_CASE(error, wxerror) \
        case ABI::Windows::Web::error: \
            event.SetString(#error); \
            event.SetInt(wxerror); \
            break;

wxCOMPtr<IWebViewControlProcess> wxWebViewEdge::ms_webViewCtrlProcess;
int wxWebViewEdge::ms_isAvailable = -1;

static ABI::Windows::Foundation::Rect wxRectToFoundationRect(const wxRect& rect)
{
    return {(FLOAT) rect.x, (FLOAT) rect.y, (FLOAT) rect.width, (FLOAT) rect.height};
}

static wxString wxStringFromFoundationURI(IUriRuntimeClass* uri)
{
    HSTRING uriStr;
    if (uri && SUCCEEDED(uri->get_RawUri(&uriStr)))
        return rt::wxStringFromHSTRING(uriStr);
    else
        return wxString();
}

bool wxWebViewEdge::IsAvailable()
{
    if (ms_isAvailable == -1)
        Initialize();

    return (ms_isAvailable == 1);
}

void wxWebViewEdge::Initialize()
{
    // Initialize control process
    wxCOMPtr<IWebViewControlProcessFactory> processFactory;
    if (!rt::GetActivationFactory(RuntimeClass_Windows_Web_UI_Interop_WebViewControlProcess,
        wxIID_PPV_ARGS(IWebViewControlProcessFactory, &processFactory)))
    {
        wxLogDebug("Could not create WebViewControlProcessFactory");
        ms_isAvailable = 0;
        return;
    }

    wxCOMPtr<IWebViewControlProcessOptions> procOptions;
    wxCOMPtr<IInspectable> insp;
    if (!rt::ActivateInstance(RuntimeClass_Windows_Web_UI_Interop_WebViewControlProcessOptions, &insp) ||
        FAILED(insp->QueryInterface(wxIID_PPV_ARGS(IWebViewControlProcessOptions, &procOptions))))
    {
        wxLogDebug("Could not create WebViewControlProcessOptions");
        ms_isAvailable = 0;
        return;
    }

    if (!SUCCEEDED(processFactory->CreateWithOptions(procOptions, &ms_webViewCtrlProcess)))
    {
        wxLogDebug("Could not create WebViewControlProcess");
        ms_isAvailable = 0;
        return;
    }

    ms_isAvailable = 1;
}

void wxWebViewEdge::Uninitalize()
{
    if (ms_isAvailable == 1)
    {
        ms_webViewCtrlProcess.reset();
        ms_isAvailable = -1;
    }
}

wxWebViewEdge::~wxWebViewEdge()
{
    if (m_webViewCtrlSite)
        m_webViewCtrlSite->Close();

    if (m_webViewCtrl)
    {
        m_webViewCtrl->remove_NavigationCompleted(m_navigationCompletedToken);
        m_webViewCtrl->remove_NavigationStarting(m_navigationStartingToken);
        m_webViewCtrl->remove_DOMContentLoaded(m_DOMContentLoadedToken);
        m_webViewCtrl->remove_NewWindowRequested(m_newWindowRequestedToken);
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

    if (!wxControl::Create(parent, id, pos, size, style,
        wxDefaultValidator, name))
    {
        return false;
    }

    if (!IsAvailable())
        return false;

    m_pendingURL = url;

    IAsyncOperation<WebViewControl*>* createCtrlOp = NULL;
    if (FAILED(ms_webViewCtrlProcess->CreateWebViewControlAsync(
        (INT64)GetHWND(), wxRectToFoundationRect(GetClientRect()), &createCtrlOp)))
    {
        wxLogError("Could not create WebViewControl");
        return false;
    }

    createCtrlOp->put_Completed(Callback<IAsyncOperationCompletedHandler<WebViewControl*> >(
        [this](IAsyncOperation<WebViewControl*>* asyncOp, AsyncStatus status) -> HRESULT
    {
        if (status == Completed)
        {
            HRESULT hr = asyncOp->GetResults(&m_webViewCtrl);
            if (SUCCEEDED(hr))
                InitWebViewCtrl();
            return hr;
        }
        else
            return S_OK;
    }).Get());

    createCtrlOp->Release();

    return true;
}

void wxWebViewEdge::InitWebViewCtrl()
{
    m_initialized = true;
    if (FAILED(m_webViewCtrl->QueryInterface(wxIID_PPV_ARGS(IWebViewControlSite, &m_webViewCtrlSite))))
        wxLogError("Could not init control site");

    UpdateBounds();

    // Connect and handle the various WebView events

    m_webViewCtrl->add_NavigationStarting(
        Callback<ITypedEventHandler<IWebViewControl*, WebViewControlNavigationStartingEventArgs*> >(
            [this](IWebViewControl* ctrl, IWebViewControlNavigationStartingEventArgs* args) -> HRESULT
    {
        m_isBusy = true;
        wxString evtURL;
        wxCOMPtr<IUriRuntimeClass> uri;
        if (SUCCEEDED(args->get_Uri(&uri)))
            evtURL = wxStringFromFoundationURI(uri);

        wxWebViewEvent event(wxEVT_WEBVIEW_NAVIGATING, GetId(), evtURL, wxString());
        event.SetEventObject(this);
        HandleWindowEvent(event);

        if (!event.IsAllowed())
            args->put_Cancel(true);

        m_isBusy = false;
        return S_OK;
    }).Get(), &m_navigationStartingToken);

    m_webViewCtrl->add_NavigationCompleted(
        Callback<ITypedEventHandler<IWebViewControl*, WebViewControlNavigationCompletedEventArgs*> >(
        [this](IWebViewControl* ctrl, IWebViewControlNavigationCompletedEventArgs* args) -> HRESULT
    {
        boolean isSuccess;
        if (FAILED(args->get_IsSuccess(&isSuccess)))
            isSuccess = false;

        wxString evtURL;
        wxCOMPtr<IUriRuntimeClass> uri;
        if (SUCCEEDED(args->get_Uri(&uri)))
            evtURL = wxStringFromFoundationURI(uri);

        if (!isSuccess)
        {
            ABI::Windows::Web::WebErrorStatus status;
            wxWebViewEvent event(wxEVT_WEBVIEW_ERROR, GetId(), evtURL, wxString());
            event.SetEventObject(this);

            if (SUCCEEDED(args->get_WebErrorStatus(&status)))
            {
                switch (status)
                {
                    WX_ERROR_CASE(WebErrorStatus_Unknown, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_CertificateCommonNameIsIncorrect, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                    WX_ERROR_CASE(WebErrorStatus_CertificateExpired, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                    WX_ERROR_CASE(WebErrorStatus_CertificateContainsErrors, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                    WX_ERROR_CASE(WebErrorStatus_CertificateRevoked, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                    WX_ERROR_CASE(WebErrorStatus_CertificateIsInvalid, wxWEBVIEW_NAV_ERR_CERTIFICATE)
                    WX_ERROR_CASE(WebErrorStatus_ServerUnreachable, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_Timeout, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_ErrorHttpInvalidServerResponse, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_ConnectionAborted, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_ConnectionReset, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_Disconnected, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_HttpToHttpsOnRedirection, wxWEBVIEW_NAV_ERR_SECURITY)
                    WX_ERROR_CASE(WebErrorStatus_HttpsToHttpOnRedirection, wxWEBVIEW_NAV_ERR_SECURITY)
                    WX_ERROR_CASE(WebErrorStatus_CannotConnect, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_HostNameNotResolved, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_OperationCanceled, wxWEBVIEW_NAV_ERR_USER_CANCELLED)
                    WX_ERROR_CASE(WebErrorStatus_RedirectFailed, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_UnexpectedStatusCode, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_UnexpectedRedirection, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_UnexpectedClientError, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_UnexpectedServerError, wxWEBVIEW_NAV_ERR_OTHER)

                    // 400 - Error codes
                    WX_ERROR_CASE(WebErrorStatus_BadRequest, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_Unauthorized, wxWEBVIEW_NAV_ERR_AUTH)
                    WX_ERROR_CASE(WebErrorStatus_PaymentRequired, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_Forbidden, wxWEBVIEW_NAV_ERR_AUTH)
                    WX_ERROR_CASE(WebErrorStatus_NotFound, wxWEBVIEW_NAV_ERR_NOT_FOUND)
                    WX_ERROR_CASE(WebErrorStatus_MethodNotAllowed, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_NotAcceptable, wxWEBVIEW_NAV_ERR_OTHER)
                    WX_ERROR_CASE(WebErrorStatus_ProxyAuthenticationRequired, wxWEBVIEW_NAV_ERR_AUTH)
                    WX_ERROR_CASE(WebErrorStatus_RequestTimeout, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_Conflict, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_Gone, wxWEBVIEW_NAV_ERR_NOT_FOUND)
                    WX_ERROR_CASE(WebErrorStatus_LengthRequired, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_PreconditionFailed, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_RequestEntityTooLarge, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_RequestUriTooLong, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_UnsupportedMediaType, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_RequestedRangeNotSatisfiable, wxWEBVIEW_NAV_ERR_REQUEST)
                    WX_ERROR_CASE(WebErrorStatus_ExpectationFailed, wxWEBVIEW_NAV_ERR_OTHER)

                    // 500 - Error codes
                    WX_ERROR_CASE(WebErrorStatus_InternalServerError, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_NotImplemented, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_BadGateway, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_ServiceUnavailable, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_GatewayTimeout, wxWEBVIEW_NAV_ERR_CONNECTION)
                    WX_ERROR_CASE(WebErrorStatus_HttpVersionNotSupported, wxWEBVIEW_NAV_ERR_REQUEST)
                }
            }
            HandleWindowEvent(event);
        }
        else
            AddPendingEvent(wxWebViewEvent(wxEVT_WEBVIEW_NAVIGATED, GetId(), evtURL, wxString()));
        return S_OK;
    }).Get(), &m_navigationCompletedToken);

    m_webViewCtrl->add_DOMContentLoaded(
        Callback<ITypedEventHandler<IWebViewControl*, WebViewControlDOMContentLoadedEventArgs*> >(
            [this](IWebViewControl* ctrl, IWebViewControlDOMContentLoadedEventArgs* args) -> HRESULT
    {
        wxString evtURL;
        wxCOMPtr<IUriRuntimeClass> uri;
        if (SUCCEEDED(args->get_Uri(&uri)))
            evtURL = wxStringFromFoundationURI(uri);
        AddPendingEvent(wxWebViewEvent(wxEVT_WEBVIEW_LOADED, GetId(), evtURL, wxString()));
        return S_OK;
    }).Get(), &m_DOMContentLoadedToken);

    m_webViewCtrl->add_NewWindowRequested(
        Callback<ITypedEventHandler<IWebViewControl*, WebViewControlNewWindowRequestedEventArgs*> >(
            [this](IWebViewControl* ctrl, IWebViewControlNewWindowRequestedEventArgs* args) -> HRESULT
    {
        wxString evtURL;
        wxCOMPtr<IUriRuntimeClass> uri;
        if (SUCCEEDED(args->get_Uri(&uri)))
            evtURL = wxStringFromFoundationURI(uri);
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
    if (m_webViewCtrlSite)
        m_webViewCtrlSite->put_Bounds(wxRectToFoundationRect(GetClientRect()));
}

void wxWebViewEdge::LoadURL(const wxString& url)
{
    if (!m_webViewCtrl)
    {
        m_pendingURL = url;
        return;
    }

    wxCOMPtr<IUriRuntimeClassFactory> uriFactory;
    if (rt::GetActivationFactory(RuntimeClass_Windows_Foundation_Uri, wxIID_PPV_ARGS(IUriRuntimeClassFactory, &uriFactory)))
    {
        wxCOMPtr<IUriRuntimeClass> uri;
        if (FAILED(uriFactory->CreateUri(rt::TempStringRef::Make(url), &uri)) ||
            FAILED(m_webViewCtrl->Navigate(uri)))
            wxLogError("Could not navigate to URL");
    }
    else
        wxLogError("Could not create URI factory");
}

void wxWebViewEdge::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{

}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetBackwardHistory()
{
    return NULL;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewEdge::GetForwardHistory()
{
    return NULL;
}

bool wxWebViewEdge::CanGoForward() const
{
    boolean result = false;
    if (m_webViewCtrl && SUCCEEDED(m_webViewCtrl->get_CanGoForward(&result)))
        return result;
    else
        return false;
}

bool wxWebViewEdge::CanGoBack() const
{
    boolean result = false;
    if (m_webViewCtrl && SUCCEEDED(m_webViewCtrl->get_CanGoBack(&result)))
        return result;
    else
        return false;
}

void wxWebViewEdge::GoBack()
{
    if (m_webViewCtrl)
        m_webViewCtrl->GoBack();
}

void wxWebViewEdge::GoForward()
{
    if (m_webViewCtrl)
        m_webViewCtrl->GoForward();
}

void wxWebViewEdge::ClearHistory()
{

}

void wxWebViewEdge::EnableHistory(bool enable)
{

}

void wxWebViewEdge::Stop()
{
    if (m_webViewCtrl)
        m_webViewCtrl->Stop();
}

void wxWebViewEdge::Reload(wxWebViewReloadFlags flags)
{
    if (m_webViewCtrl)
        m_webViewCtrl->Refresh();
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
    wxCOMPtr<IUriRuntimeClass> uri;
    if (m_webViewCtrl && SUCCEEDED(m_webViewCtrl->get_Source(&uri)))
        return wxStringFromFoundationURI(uri);
    else
        return wxString();
}

wxString wxWebViewEdge::GetCurrentTitle() const
{
    HSTRING title;
    if (m_webViewCtrl && SUCCEEDED(m_webViewCtrl->get_DocumentTitle(&title)))
        return rt::wxStringFromHSTRING(title);
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

bool wxWebViewEdge::CanSetZoomType(wxWebViewZoomType) const
{
    return true;
}

void wxWebViewEdge::Print()
{

}

wxWebViewZoom wxWebViewEdge::GetZoom() const
{
    return wxWEBVIEW_ZOOM_MEDIUM;
}

void wxWebViewEdge::SetZoom(wxWebViewZoom zoom)
{

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

long wxWebViewEdge::Find(const wxString& text, int flags)
{
    return -1;
}

//Editing functions
void wxWebViewEdge::SetEditable(bool enable)
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

bool wxWebViewEdge::RunScript(const wxString& javascript, wxString* output)
{
    return false;
}

void wxWebViewEdge::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{

}

void wxWebViewEdge::DoSetPage(const wxString& html, const wxString& baseUrl)
{
    if (m_webViewCtrl)
        m_webViewCtrl->NavigateToString(rt::TempStringRef::Make(html));
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
