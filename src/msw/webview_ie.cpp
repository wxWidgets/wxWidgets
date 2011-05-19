/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/webview_ie.cpp
// Purpose:     wxMSW wxWebViewIE class implementation for web view component
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#include "wx/msw/webview_ie.h"

#if wxHAVE_WEB_BACKEND_IE

#include <olectl.h>
#include <oleidl.h>
#include <exdispid.h>
#include <exdisp.h>
#include <mshtml.h>

#ifdef __MINGW32__
// FIXME: Seems like MINGW does not have these, how to handle cleanly?
#define DISPID_COMMANDSTATECHANGE   105
typedef enum CommandStateChangeConstants {
    CSC_UPDATECOMMANDS = (int) 0xFFFFFFFF,
    CSC_NAVIGATEFORWARD = 0x1,
    CSC_NAVIGATEBACK = 0x2
} CommandStateChangeConstants;


// FIXME: Seems like MINGW does not have these, how to handle cleanly?
#define DISPID_NAVIGATECOMPLETE2    252
#define DISPID_NAVIGATEERROR        271
#define OLECMDID_OPTICAL_ZOOM  63
#define INET_E_ERROR_FIRST 0x800C0002L
#define INET_E_INVALID_URL 0x800C0002L
#define INET_E_NO_SESSION 0x800C0003L
#define INET_E_CANNOT_CONNECT 0x800C0004L
#define INET_E_RESOURCE_NOT_FOUND 0x800C0005L
#define INET_E_OBJECT_NOT_FOUND 0x800C0006L
#define INET_E_DATA_NOT_AVAILABLE 0x800C0007L
#define INET_E_DOWNLOAD_FAILURE 0x800C0008L
#define INET_E_AUTHENTICATION_REQUIRED 0x800C0009L
#define INET_E_NO_VALID_MEDIA 0x800C000AL
#define INET_E_CONNECTION_TIMEOUT 0x800C000BL
#define INET_E_INVALID_REQUEST 0x800C000CL
#define INET_E_UNKNOWN_PROTOCOL 0x800C000DL
#define INET_E_SECURITY_PROBLEM 0x800C000EL
#define INET_E_CANNOT_LOAD_DATA 0x800C000FL
#define INET_E_CANNOT_INSTANTIATE_OBJECT 0x800C0010L
#define INET_E_QUERYOPTION_UNKNOWN 0x800C0013L
#define INET_E_REDIRECT_FAILED 0x800C0014L
#define INET_E_REDIRECT_TO_DIR 0x800C0015L
#define INET_E_CANNOT_LOCK_REQUEST 0x800C0016L
#define INET_E_USE_EXTEND_BINDING 0x800C0017L
#define INET_E_TERMINATED_BIND 0x800C0018L
#define INET_E_INVALID_CERTIFICATE 0x800C0019L
#define INET_E_CODE_DOWNLOAD_DECLINED 0x800C0100L
#define INET_E_RESULT_DISPATCHED 0x800C0200L
#define INET_E_CANNOT_REPLACE_SFP_FILE 0x800C0300L
#define INET_E_CODE_INSTALL_BLOCKED_BY_HASH_POLICY 0x800C0500L
#define INET_E_CODE_INSTALL_SUPPRESSED 0x800C0400L

#define REFRESH_COMPLETELY 3
#endif

BEGIN_EVENT_TABLE(wxWebViewIE, wxControl)
    EVT_ACTIVEX(wxID_ANY, wxWebViewIE::onActiveXEvent)
    EVT_ERASE_BACKGROUND(wxWebViewIE::onEraseBg)
END_EVENT_TABLE()

bool wxWebViewIE::Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style,
                           wxDefaultValidator, name))
    {
        return false;
    }

    m_webBrowser = NULL;
    m_canNavigateBack = false;
    m_canNavigateForward = false;
    m_isBusy = false;

    if (::CoCreateInstance(CLSID_WebBrowser, NULL,
                           CLSCTX_INPROC_SERVER, // CLSCTX_INPROC,
                           IID_IWebBrowser2 , (void**)&m_webBrowser) != 0)
    {
        wxLogError("Failed to initialize IE, CoCreateInstance returned an error");
        return false;
    }

    m_ie.SetDispatchPtr(m_webBrowser); // wxAutomationObject will release itself

    m_webBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
    m_webBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);
    //m_webBrowser->put_Silent(VARIANT_FALSE);

    m_container = new wxActiveXContainer(this, IID_IWebBrowser2, m_webBrowser);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetDoubleBuffered(true);
    return true;
}


void wxWebViewIE::LoadUrl(const wxString& url)
{
    wxVariant out = m_ie.CallMethod("Navigate", (BSTR) url.wc_str(),
                                    NULL, NULL, NULL, NULL);

    // FIXME: why is out value null??
    //(HRESULT)(out.GetLong()) == S_OK;
}

void wxWebViewIE::SetPage(const wxString& html, const wxString& baseUrl)
{
    LoadUrl("about:blank");

    // Let the wx events generated for navigation events be processed, so
    // that the underlying IE component completes its Document object.
    // FIXME: calling wxYield is not elegant nor very reliable probably
    wxYield();

    wxVariant documentVariant = m_ie.GetProperty("Document");
    void* documentPtr = documentVariant.GetVoidPtr();

    wxASSERT (documentPtr != NULL);

    // TODO: consider the "baseUrl" parameter if possible
    // TODO: consider encoding
    BSTR bstr = SysAllocString(html.wc_str());

    // Creates a new one-dimensional array
    SAFEARRAY *psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    if (psaStrings != NULL)
    {
        VARIANT *param;
        HRESULT hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
        param->vt = VT_BSTR;
        param->bstrVal = bstr;

        hr = SafeArrayUnaccessData(psaStrings);

        IHTMLDocument2* document = (IHTMLDocument2*)documentPtr;
        document->write(psaStrings);

        // SafeArrayDestroy calls SysFreeString for each BSTR
        SafeArrayDestroy(psaStrings);
    }
    else
    {
        wxLogError("wxWebViewIE::SetPage() : psaStrings is NULL");
    }

}

wxString wxWebViewIE::GetPageSource()
{
    wxVariant documentVariant = m_ie.GetProperty("Document");
    void* documentPtr = documentVariant.GetVoidPtr();

    if (documentPtr == NULL)
    {
        return wxEmptyString;
    }

    IHTMLDocument2* document = (IHTMLDocument2*)documentPtr;

    IHTMLElement *bodyTag = NULL;
    IHTMLElement *htmlTag = NULL;
    document->get_body(&bodyTag);
    wxASSERT(bodyTag != NULL);

    document->Release();
    bodyTag->get_parentElement(&htmlTag);
    wxASSERT(htmlTag != NULL);

    BSTR    bstr;
    htmlTag->get_outerHTML(&bstr);

    bodyTag->Release();
    htmlTag->Release();

    //wxMessageBox(wxString(bstr));

    // TODO: check encoding
    return wxString(bstr);
}

// FIXME? retrieve OLECMDID_GETZOOMRANGE instead of hardcoding range 0-4
wxWebViewZoom wxWebViewIE::GetZoom()
{
    const int zoom = GetIETextZoom();

    switch (zoom)
    {
        case 0:
            return wxWEB_VIEW_ZOOM_TINY;
            break;
        case 1:
            return wxWEB_VIEW_ZOOM_SMALL;
            break;
        case 2:
            return wxWEB_VIEW_ZOOM_MEDIUM;
            break;
        case 3:
            return wxWEB_VIEW_ZOOM_LARGE;
            break;
        case 4:
            return wxWEB_VIEW_ZOOM_LARGEST;
            break;
        default:
            wxASSERT(false);
            return wxWEB_VIEW_ZOOM_MEDIUM;
    }
}
void wxWebViewIE::SetZoom(wxWebViewZoom zoom)
{
    // I know I could cast from enum to int since wxWebViewZoom happens to
    // match with IE's zoom levels, but I don't like doing that, what if enum
    // values change...
    switch (zoom)
    {
        case wxWEB_VIEW_ZOOM_TINY:
            SetIETextZoom(0);
            break;
        case wxWEB_VIEW_ZOOM_SMALL:
            SetIETextZoom(1);
            break;
        case wxWEB_VIEW_ZOOM_MEDIUM:
            SetIETextZoom(2);
            break;
        case wxWEB_VIEW_ZOOM_LARGE:
            SetIETextZoom(3);
            break;
        case wxWEB_VIEW_ZOOM_LARGEST:
            SetIETextZoom(4);
            break;
        default:
            wxASSERT(false);
    }
}

void wxWebViewIE::SetIETextZoom(int level)
{
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;
    V_I4(&zoomVariant) = level;

    HRESULT result = m_webBrowser->ExecWB(OLECMDID_ZOOM,
                                          OLECMDEXECOPT_DONTPROMPTUSER,
                                          &zoomVariant, NULL);
    wxASSERT (result == S_OK);

    VariantClear (&zoomVariant);
}

int wxWebViewIE::GetIETextZoom()
{
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;
    V_I4(&zoomVariant) = 4;

    HRESULT result = m_webBrowser->ExecWB(OLECMDID_ZOOM,
                                          OLECMDEXECOPT_DONTPROMPTUSER,
                                          NULL, &zoomVariant);
    wxASSERT (result == S_OK);

    int zoom = V_I4(&zoomVariant);
   // wxMessageBox(wxString::Format("Zoom : %i", zoom));
    VariantClear (&zoomVariant);

    return zoom;
}

void wxWebViewIE::SetIEOpticalZoom(float zoom)
{
    // TODO: add support for optical zoom (IE7+ only)

    // TODO: get range from OLECMDID_OPTICAL_GETZOOMRANGE instead of hardcoding?
    wxASSERT(zoom >= 10.0f);
    wxASSERT(zoom <= 1000.0f);

    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;
    V_I4(&zoomVariant) = (zoom * 100.0f);

    HRESULT result = m_webBrowser->ExecWB((OLECMDID)OLECMDID_OPTICAL_ZOOM,
                                          OLECMDEXECOPT_DODEFAULT,
                                          &zoomVariant,
                                          NULL);
    wxASSERT (result == S_OK);
}

float wxWebViewIE::GetIEOpticalZoom()
{
    // TODO: add support for optical zoom (IE7+ only)

    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;
    V_I4(&zoomVariant) = -1;

    HRESULT result = m_webBrowser->ExecWB((OLECMDID)OLECMDID_OPTICAL_ZOOM,
                                          OLECMDEXECOPT_DODEFAULT, NULL,
                                          &zoomVariant);
    wxASSERT (result == S_OK);

    const int zoom = V_I4(&zoomVariant);
    VariantClear (&zoomVariant);

    return zoom / 100.0f;
}

void wxWebViewIE::SetZoomType(wxWebViewZoomType)
{
    // TODO: add support for optical zoom (IE7+ only)
    wxASSERT(false);
}

wxWebViewZoomType wxWebViewIE::GetZoomType() const
{
    // TODO: add support for optical zoom (IE7+ only)
    return wxWEB_VIEW_ZOOM_TYPE_TEXT;
}

bool wxWebViewIE::CanSetZoomType(wxWebViewZoomType) const
{
    // both are supported
    // TODO: IE6 only supports text zoom, check if it's IE6 first
    return true;
}

void wxWebViewIE::Print()
{
    m_webBrowser->ExecWB(OLECMDID_PRINTPREVIEW,
                         OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

void wxWebViewIE::GoBack()
{
    wxVariant out = m_ie.CallMethod("GoBack");

    // FIXME: why is out value null??
    //return (HRESULT)(out.GetLong()) == S_OK;
}

void wxWebViewIE::GoForward()
{
    wxVariant out = m_ie.CallMethod("GoForward");

    // FIXME: why is out value null??
    //return (HRESULT)(out.GetLong()) == S_OK;
}

void wxWebViewIE::Stop()
{
    wxVariant out = m_ie.CallMethod("Stop");

    // FIXME: why is out value null??
    //return (HRESULT)(out.GetLong()) == S_OK;
}


void wxWebViewIE::Reload(wxWebViewReloadFlags flags)
{
    VARIANTARG level;
    VariantInit(&level);
    V_VT(&level) = VT_I2;

    switch(flags)
    {
        case wxWEB_VIEW_RELOAD_DEFAULT:
            V_I2(&level) = REFRESH_NORMAL;
            break;
        case wxWEB_VIEW_RELOAD_NO_CACHE:
            V_I2(&level) = REFRESH_COMPLETELY;
            break;
        default:
            wxFAIL_MSG("Unexpected reload type");
    }

    m_webBrowser->Refresh2(&level);
}

bool wxWebViewIE::IsOfflineMode()
{
    wxVariant out = m_ie.GetProperty("Offline");

    wxASSERT(out.GetType() == "bool");

    return out.GetBool();
}

void wxWebViewIE::SetOfflineMode(bool offline)
{
    // FIXME: the wxWidgets docs do not really document what the return
    //        parameter of PutProperty is
    const bool success = m_ie.PutProperty("Offline", (offline ?
                                                      VARIANT_TRUE :
                                                      VARIANT_FALSE));
    wxASSERT(success);
}

bool wxWebViewIE::IsBusy()
{
    if (m_isBusy) return true;

    wxVariant out = m_ie.GetProperty("Busy");

    wxASSERT(out.GetType() == "bool");

    return out.GetBool();
}

wxString wxWebViewIE::GetCurrentURL()
{
    wxVariant out = m_ie.GetProperty("LocationURL");

    wxASSERT(out.GetType() == "string");
    return out.GetString();
}

wxString wxWebViewIE::GetCurrentTitle()
{
    wxVariant out = m_ie.GetProperty("LocationName");

    wxASSERT(out.GetType() == "string");
    return out.GetString();
}

void wxWebViewIE::onActiveXEvent(wxActiveXEvent& evt)
{
    if (m_webBrowser == NULL) return;

    switch (evt.GetDispatchId())
    {
        case DISPID_BEFORENAVIGATE2:
        {
            m_isBusy = true;

            wxString url = evt[1].GetString();
            wxString target = evt[3].GetString();

            wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_NAVIGATING,
                                       GetId(), url, target, true);
            event.SetEventObject(this);
            HandleWindowEvent(event);

            if (event.IsVetoed())
            {
                wxActiveXEventNativeMSW* nativeParams =
                    evt.GetNativeParameters();
                *V_BOOLREF(&nativeParams->pDispParams->rgvarg[0]) = VARIANT_TRUE;
            }

            // at this point, either the navigation event has been cancelled
            // and we're not busy, either it was accepted and IWebBrowser2's
            // Busy property will be true; so we don't need our override
            // flag anymore.
            m_isBusy = false;

            break;
        }

        case DISPID_NAVIGATECOMPLETE2:
        {
            wxString url = evt[1].GetString();
            // TODO: set target parameter if possible
            wxString target = wxEmptyString;
            wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
                                       GetId(), url, target, false);
            event.SetEventObject(this);
            HandleWindowEvent(event);
            break;
        }

        case DISPID_PROGRESSCHANGE:
        {
            // download progress
            break;
        }

        case DISPID_DOCUMENTCOMPLETE:
        {
            wxString url = evt[1].GetString();
            // TODO: set target parameter if possible
            wxString target = wxEmptyString;
            wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_LOADED, GetId(),
                                       url, target, false);
            event.SetEventObject(this);
            HandleWindowEvent(event);
            break;
        }

        case DISPID_STATUSTEXTCHANGE:
        {
            break;
        }

        case DISPID_TITLECHANGE:
        {
            break;
        }

        case DISPID_NAVIGATEERROR:
        {
            wxWebNavigationError errorType = wxWEB_NAV_ERR_OTHER;
            wxString errorCode = "?";
            switch (evt[3].GetLong())
            {
            case INET_E_INVALID_URL: // (0x800C0002L or -2146697214)
                errorCode = "INET_E_INVALID_URL";
                errorType = wxWEB_NAV_ERR_REQUEST;
                break;
            case INET_E_NO_SESSION: // (0x800C0003L or -2146697213)
                errorCode = "INET_E_NO_SESSION";
                errorType = wxWEB_NAV_ERR_CONNECTION;
                break;
            case INET_E_CANNOT_CONNECT: // (0x800C0004L or -2146697212)
                errorCode = "INET_E_CANNOT_CONNECT";
                errorType = wxWEB_NAV_ERR_CONNECTION;
                break;
            case INET_E_RESOURCE_NOT_FOUND: // (0x800C0005L or -2146697211)
                errorCode = "INET_E_RESOURCE_NOT_FOUND";
                errorType = wxWEB_NAV_ERR_NOT_FOUND;
                break;
            case INET_E_OBJECT_NOT_FOUND: // (0x800C0006L or -2146697210)
                errorCode = "INET_E_OBJECT_NOT_FOUND";
                errorType = wxWEB_NAV_ERR_NOT_FOUND;
                break;
            case INET_E_DATA_NOT_AVAILABLE: // (0x800C0007L or -2146697209)
                errorCode = "INET_E_DATA_NOT_AVAILABLE";
                errorType = wxWEB_NAV_ERR_NOT_FOUND;
                break;
            case INET_E_DOWNLOAD_FAILURE: // (0x800C0008L or -2146697208)
                errorCode = "INET_E_DOWNLOAD_FAILURE";
                errorType = wxWEB_NAV_ERR_CONNECTION;
                break;
            case INET_E_AUTHENTICATION_REQUIRED: // (0x800C0009L or -2146697207)
                errorCode = "INET_E_AUTHENTICATION_REQUIRED";
                errorType = wxWEB_NAV_ERR_AUTH;
                break;
            case INET_E_NO_VALID_MEDIA: // (0x800C000AL or -2146697206)
                errorCode = "INET_E_NO_VALID_MEDIA";
                errorType = wxWEB_NAV_ERR_REQUEST;
                break;
            case INET_E_CONNECTION_TIMEOUT: // (0x800C000BL or -2146697205)
                errorCode = "INET_E_CONNECTION_TIMEOUT";
                errorType = wxWEB_NAV_ERR_CONNECTION;
                break;
            case INET_E_INVALID_REQUEST: // (0x800C000CL or -2146697204)
                errorCode = "INET_E_INVALID_REQUEST";
                errorType = wxWEB_NAV_ERR_REQUEST;
                break;
            case INET_E_UNKNOWN_PROTOCOL: // (0x800C000DL or -2146697203)
                errorCode = "INET_E_UNKNOWN_PROTOCOL";
                errorType = wxWEB_NAV_ERR_REQUEST;
                break;
            case INET_E_SECURITY_PROBLEM: // (0x800C000EL or -2146697202)
                errorCode = "INET_E_SECURITY_PROBLEM";
                errorType = wxWEB_NAV_ERR_SECURITY;
                break;
            case INET_E_CANNOT_LOAD_DATA: // (0x800C000FL or -2146697201)
                errorCode = "INET_E_CANNOT_LOAD_DATA";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_CANNOT_INSTANTIATE_OBJECT:
                // CoCreateInstance will return an error code if this happens,
                // we'll handle this above.
                return;
                break;
            case INET_E_REDIRECT_FAILED: // (0x800C0014L or -2146697196)
                errorCode = "INET_E_REDIRECT_FAILED";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_REDIRECT_TO_DIR: // (0x800C0015L or -2146697195)
                errorCode = "INET_E_REDIRECT_TO_DIR";
                errorType = wxWEB_NAV_ERR_REQUEST;
                break;
            case INET_E_CANNOT_LOCK_REQUEST: // (0x800C0016L or -2146697194)
                errorCode = "INET_E_CANNOT_LOCK_REQUEST";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_USE_EXTEND_BINDING: // (0x800C0017L or -2146697193)
                errorCode = "INET_E_USE_EXTEND_BINDING";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_TERMINATED_BIND: // (0x800C0018L or -2146697192)
                errorCode = "INET_E_TERMINATED_BIND";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_INVALID_CERTIFICATE: // (0x800C0019L or -2146697191)
                errorCode = "INET_E_INVALID_CERTIFICATE";
                errorType = wxWEB_NAV_ERR_CERTIFICATE;
                break;
            case INET_E_CODE_DOWNLOAD_DECLINED: // (0x800C0100L or -2146696960)
                errorCode = "INET_E_CODE_DOWNLOAD_DECLINED";
                errorType = wxWEB_NAV_ERR_USER_CANCELLED;
                break;
            case INET_E_RESULT_DISPATCHED: // (0x800C0200L or -2146696704)
                // cancel request cancelled...
                errorCode = "INET_E_RESULT_DISPATCHED";
                errorType = wxWEB_NAV_ERR_OTHER;
                break;
            case INET_E_CANNOT_REPLACE_SFP_FILE: // (0x800C0300L or -2146696448)
                errorCode = "INET_E_CANNOT_REPLACE_SFP_FILE";
                errorType = wxWEB_NAV_ERR_SECURITY;
                break;
            case INET_E_CODE_INSTALL_BLOCKED_BY_HASH_POLICY:
                errorCode = "INET_E_CODE_INSTALL_BLOCKED_BY_HASH_POLICY";
                errorType = wxWEB_NAV_ERR_SECURITY;
                break;
            case INET_E_CODE_INSTALL_SUPPRESSED:
                errorCode = "INET_E_CODE_INSTALL_SUPPRESSED";
                errorType = wxWEB_NAV_ERR_SECURITY;
                break;
            }

            wxString url = evt[1].GetString();
            wxString target = evt[2].GetString();
            wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_ERROR, GetId(),
                                       url, target, false);
            event.SetEventObject(this);
            event.SetInt(errorType);
            event.SetString(errorCode);
            HandleWindowEvent(event);
            break;
        }

        case DISPID_COMMANDSTATECHANGE:
        {
            long commandId = evt[0].GetLong();
            bool enable = evt[1].GetBool();
            if (commandId == CSC_NAVIGATEBACK)
            {
                m_canNavigateBack = enable;
            }
            else if (commandId == CSC_NAVIGATEFORWARD)
            {
                m_canNavigateForward = enable;
            }
            break;
        }
        case DISPID_NEWWINDOW2:
        {
            wxActiveXEventNativeMSW* nativeParams = evt.GetNativeParameters();
            // Cancel the attempt to open a new window
            *V_BOOLREF(&nativeParams->pDispParams->rgvarg[0]) = VARIANT_TRUE;
            break;
        }
    }

    evt.Skip();
}

#endif
