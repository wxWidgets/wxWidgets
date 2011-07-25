/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/webview_ie.cpp
// Purpose:     wxMSW wxWebViewIE class implementation for web view component
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#include "wx/msw/webview_ie.h"

#if wxUSE_WEBVIEW_IE

#include <olectl.h>
#include <oleidl.h>
#include <exdispid.h>
#include <exdisp.h>
#include <mshtml.h>
#include "wx/msw/registry.h"
#include "wx/msw/missing.h"
#include "wx/filesys.h"

//We link to urlmon as it is required for CoInternetGetSession
#pragma comment(lib, "urlmon")

//Taken from wx/filesys.cpp
static wxString EscapeFileNameCharsInURL(const char *in)
{
    wxString s;

    for ( const unsigned char *p = (const unsigned char*)in; *p; ++p )
    {
        const unsigned char c = *p;

        if ( c == '/' || c == '-' || c == '.' || c == '_' || c == '~' ||
             (c >= '0' && c <= '9') ||
             (c >= 'a' && c <= 'z') ||
             (c >= 'A' && c <= 'Z') )
        {
            s << c;
        }
        else
        {
            s << wxString::Format("%%%02x", c);
        }
    }

    return s;
}

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
    m_isBusy = false;
    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;
    m_zoomType = wxWEB_VIEW_ZOOM_TYPE_TEXT;

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

    //We register a custom handler for the file protocol so we can handle
    //Virtual file systems
    ClassFactory* cf = new ClassFactory;
    IInternetSession* session;
    if(CoInternetGetSession(0, &session, 0) != S_OK)
        return false;
    HRESULT hr = session->RegisterNameSpace(cf, CLSID_FileProtocol, L"file", 0, NULL, 0);
    if(FAILED(hr))
        return false; 

    m_container = new wxActiveXContainer(this, IID_IWebBrowser2, m_webBrowser);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetDoubleBuffered(true);
    LoadUrl(url);
    return true;
}


void wxWebViewIE::LoadUrl(const wxString& url)
{
    m_ie.CallMethod("Navigate", (BSTR) url.wc_str(), NULL, NULL, NULL, NULL);
}

void wxWebViewIE::SetPage(const wxString& html, const wxString& baseUrl)
{
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

        IHTMLDocument2* document = GetDocument();
        document->write(psaStrings);
        document->Release();

        // SafeArrayDestroy calls SysFreeString for each BSTR
        SafeArrayDestroy(psaStrings);

        //We send the events when we are done to mimic webkit
        //Navigated event
        wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_NAVIGATED,
                                   GetId(), baseUrl, "", false);
        event.SetEventObject(this);
        HandleWindowEvent(event);

        //Document complete event
        event.SetEventType(wxEVT_COMMAND_WEB_VIEW_LOADED);
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
    else
    {
        wxLogError("wxWebViewIE::SetPage() : psaStrings is NULL");
    }

}

wxString wxWebViewIE::GetPageSource()
{
    IHTMLDocument2* document = GetDocument();
    IHTMLElement *bodyTag = NULL;
    IHTMLElement *htmlTag = NULL;
    wxString source;
    HRESULT hr = document->get_body(&bodyTag);
    if(SUCCEEDED(hr))
    {
        hr = bodyTag->get_parentElement(&htmlTag);
        if(SUCCEEDED(hr))
        {
            BSTR bstr;
            htmlTag->get_outerHTML(&bstr);
            source = wxString(bstr);
            htmlTag->Release();
        }
        bodyTag->Release();
    }

    document->Release();
    return source;
}

wxWebViewZoom wxWebViewIE::GetZoom()
{
    if(m_zoomType == wxWEB_VIEW_ZOOM_TYPE_LAYOUT)
        return GetIEOpticalZoom();
    else if(m_zoomType == wxWEB_VIEW_ZOOM_TYPE_TEXT)
        return GetIETextZoom();
    else
        wxFAIL;

    //Dummy return to stop compiler warnings
    return wxWEB_VIEW_ZOOM_MEDIUM;

}

void wxWebViewIE::SetZoom(wxWebViewZoom zoom)
{
    if(m_zoomType == wxWEB_VIEW_ZOOM_TYPE_LAYOUT)
        SetIEOpticalZoom(zoom);
    else if(m_zoomType == wxWEB_VIEW_ZOOM_TYPE_TEXT)
        SetIETextZoom(zoom);
    else
        wxFAIL;
}

void wxWebViewIE::SetIETextZoom(wxWebViewZoom level)
{
    //We do not use OLECMDID_OPTICAL_GETZOOMRANGE as the docs say the range
    //is 0 to 4 so the check is unnecessary, these match exactly with the
    //enum values
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;
    V_I4(&zoomVariant) = level;

    HRESULT result = m_webBrowser->ExecWB(OLECMDID_ZOOM,
                                          OLECMDEXECOPT_DONTPROMPTUSER,
                                          &zoomVariant, NULL);
    wxASSERT(result == S_OK);
}

wxWebViewZoom wxWebViewIE::GetIETextZoom()
{
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;

    HRESULT result = m_webBrowser->ExecWB(OLECMDID_ZOOM,
                                          OLECMDEXECOPT_DONTPROMPTUSER,
                                          NULL, &zoomVariant);
    wxASSERT(result == S_OK);

    //We can safely cast here as we know that the range matches our enum
    return static_cast<wxWebViewZoom>(V_I4(&zoomVariant));
}

void wxWebViewIE::SetIEOpticalZoom(wxWebViewZoom level)
{
    //We do not use OLECMDID_OPTICAL_GETZOOMRANGE as the docs say the range
    //is 10 to 1000 so the check is unnecessary
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;

    //We make a somewhat arbitray map here, taken from values used by webkit
    switch(level)
    {
        case wxWEB_VIEW_ZOOM_TINY:
            V_I4(&zoomVariant) = 60;
            break;
        case wxWEB_VIEW_ZOOM_SMALL:
            V_I4(&zoomVariant) = 80;
            break;
        case wxWEB_VIEW_ZOOM_MEDIUM:
            V_I4(&zoomVariant) = 100;
            break;
        case wxWEB_VIEW_ZOOM_LARGE:
            V_I4(&zoomVariant) = 130;
            break;
        case wxWEB_VIEW_ZOOM_LARGEST:
            V_I4(&zoomVariant) = 160;
            break;
        default:
            wxFAIL;
    }

    HRESULT result = m_webBrowser->ExecWB((OLECMDID)OLECMDID_OPTICAL_ZOOM,
                                          OLECMDEXECOPT_DODEFAULT,
                                          &zoomVariant,
                                          NULL);
    wxASSERT(result == S_OK);
}

wxWebViewZoom wxWebViewIE::GetIEOpticalZoom()
{
    VARIANT zoomVariant;
    VariantInit (&zoomVariant);
    V_VT(&zoomVariant) = VT_I4;

    HRESULT result = m_webBrowser->ExecWB((OLECMDID)OLECMDID_OPTICAL_ZOOM,
                                          OLECMDEXECOPT_DODEFAULT, NULL,
                                          &zoomVariant);
    wxASSERT(result == S_OK);

    const int zoom = V_I4(&zoomVariant);

    //We make a somewhat arbitray map here, taken from values used by webkit
    if (zoom <= 65)
    {
        return wxWEB_VIEW_ZOOM_TINY;
    }
    else if (zoom > 65 && zoom <= 90)
    {
        return wxWEB_VIEW_ZOOM_SMALL;
    }
    else if (zoom > 90 && zoom <= 115)
    {
        return wxWEB_VIEW_ZOOM_MEDIUM;
    }
    else if (zoom > 115 && zoom <= 145)
    {
        return wxWEB_VIEW_ZOOM_LARGE;
    }
    else /*if (zoom > 145) */ //Using else removes a compiler warning
    {
        return wxWEB_VIEW_ZOOM_LARGEST;
    }
}

void wxWebViewIE::SetZoomType(wxWebViewZoomType type)
{
    m_zoomType = type;
}

wxWebViewZoomType wxWebViewIE::GetZoomType() const
{
    return m_zoomType;
}

bool wxWebViewIE::CanSetZoomType(wxWebViewZoomType type) const
{
    //IE 6 and below only support text zoom, so check the registry to see what
    //version we actually have
    wxRegKey key(wxRegKey::HKLM, "Software\\Microsoft\\Internet Explorer");
    wxString value;
    key.QueryValue("Version", value);

    long version = wxAtoi(value.Left(1));
    if(version <= 6 && type == wxWEB_VIEW_ZOOM_TYPE_LAYOUT)
        return false;
    else
        return true;
}

void wxWebViewIE::Print()
{
    m_webBrowser->ExecWB(OLECMDID_PRINTPREVIEW,
                         OLECMDEXECOPT_DODEFAULT, NULL, NULL);
}

bool wxWebViewIE::CanGoBack()
{
    if(m_historyEnabled)
        return m_historyPosition > 0;
    else
        return false;
}

bool wxWebViewIE::CanGoForward()
{
    if(m_historyEnabled)
        return m_historyPosition != static_cast<int>(m_historyList.size()) - 1;
    else
        return false;
}

void wxWebViewIE::LoadHistoryItem(wxSharedPtr<wxWebHistoryItem> item)
{
    int pos = -1;
    for(unsigned int i = 0; i < m_historyList.size(); i++)
    {
        //We compare the actual pointers to find the correct item
        if(m_historyList[i].get() == item.get())
            pos = i;
    }
    wxASSERT_MSG(pos != static_cast<int>(m_historyList.size()),
                 "invalid history item");
    m_historyLoadingFromList = true;
    LoadUrl(item->GetUrl());
    m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebHistoryItem> > wxWebViewIE::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebHistoryItem> > backhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for(int i = 0; i < m_historyPosition; i++)
    {
        backhist.push_back(m_historyList[i]);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebHistoryItem> > wxWebViewIE::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebHistoryItem> > forwardhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for(int i = m_historyPosition + 1; i < static_cast<int>(m_historyList.size()); i++)
    {
        forwardhist.push_back(m_historyList[i]);
    }
    return forwardhist;
}

void wxWebViewIE::GoBack()
{
    LoadHistoryItem(m_historyList[m_historyPosition - 1]);
}

void wxWebViewIE::GoForward()
{
    LoadHistoryItem(m_historyList[m_historyPosition + 1]);
}

void wxWebViewIE::Stop()
{
    m_ie.CallMethod("Stop");
}

void wxWebViewIE::ClearHistory()
{
    m_historyList.clear();
    m_historyPosition = -1;
}

void wxWebViewIE::EnableHistory(bool enable)
{
    m_historyEnabled = enable;
    m_historyList.clear();
    m_historyPosition = -1;
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
    IHTMLDocument2* document = GetDocument();
    BSTR title;

    document->get_nameProp(&title);
    document->Release();
    return wxString(title);
}

bool wxWebViewIE::CanCut()
{
    return CanExecCommand("Cut");
}

bool wxWebViewIE::CanCopy()
{
    return CanExecCommand("Copy");
}
bool wxWebViewIE::CanPaste()
{
    return CanExecCommand("Paste");
}

void wxWebViewIE::Cut()
{
    ExecCommand("Cut");
}

void wxWebViewIE::Copy()
{
    ExecCommand("Copy");
}

void wxWebViewIE::Paste()
{
    ExecCommand("Paste");
}

bool wxWebViewIE::CanUndo()
{
    return CanExecCommand("Undo");
}
bool wxWebViewIE::CanRedo()
{
    return CanExecCommand("Redo");
}

void wxWebViewIE::Undo()
{
    ExecCommand("Undo");
}

void wxWebViewIE::Redo()
{
    ExecCommand("Redo");
}

void wxWebViewIE::SetEditable(bool enable)
{
    IHTMLDocument2* document = GetDocument();
    if( enable )
        document->put_designMode(SysAllocString(L"On"));
    else
        document->put_designMode(SysAllocString(L"Off"));

    document->Release();
}

bool wxWebViewIE::IsEditable()
{
    IHTMLDocument2* document = GetDocument();
    BSTR mode;
    document->get_designMode(&mode);
    document->Release();
    if(wxString(mode) == "On")
        return true;
    else
        return false;
}

void wxWebViewIE::SelectAll()
{
    ExecCommand("SelectAll");
}

bool wxWebViewIE::HasSelection()
{
    IHTMLDocument2* document = GetDocument();
    IHTMLSelectionObject* selection;
    wxString sel;
    HRESULT hr = document->get_selection(&selection);
    if(SUCCEEDED(hr))
    {
        BSTR type;
        selection->get_type(&type);
        sel = wxString(type);
        selection->Release();
    }
    document->Release();
    return sel != "None";
}

void wxWebViewIE::DeleteSelection()
{
    ExecCommand("Delete");
}

wxString wxWebViewIE::GetSelectedText()
{
    IHTMLDocument2* document = GetDocument();
    IHTMLSelectionObject* selection;
    wxString selected;
    HRESULT hr = document->get_selection(&selection);
    if(SUCCEEDED(hr))
    {
        IDispatch* disrange;
        hr = selection->createRange(&disrange);
        if(SUCCEEDED(hr))
        {
            IHTMLTxtRange* range;
            hr = disrange->QueryInterface(IID_IHTMLTxtRange, (void**)&range);
            if(SUCCEEDED(hr))
            {
                BSTR text;
                range->get_text(&text);
                selected = wxString(text);
                range->Release();
            }
            disrange->Release();
        }
        selection->Release();
    }
    document->Release();
    return selected;
}

wxString wxWebViewIE::GetSelectedSource()
{
    IHTMLDocument2* document = GetDocument();
    IHTMLSelectionObject* selection;
    wxString selected;
    HRESULT hr = document->get_selection(&selection);
    if(SUCCEEDED(hr))
    {
        IDispatch* disrange;
        hr = selection->createRange(&disrange);
        if(SUCCEEDED(hr))
        {
            IHTMLTxtRange* range;
            hr = disrange->QueryInterface(IID_IHTMLTxtRange, (void**)&range);
            if(SUCCEEDED(hr))
            {
                BSTR text;
                range->get_htmlText(&text);
                selected = wxString(text);
                range->Release();
            }
            disrange->Release();
        }
        selection->Release();
    }
    document->Release();
    return selected;
}

void wxWebViewIE::ClearSelection()
{
    IHTMLDocument2* document = GetDocument();
    IHTMLSelectionObject* selection;
    wxString selected;
    HRESULT hr = document->get_selection(&selection);
    if(SUCCEEDED(hr))
    {
        selection->empty();
        selection->Release();
    }
    document->Release();
}

wxString wxWebViewIE::GetPageText()
{
    IHTMLDocument2* document = GetDocument();
    wxString text;
    IHTMLElement* body;
    HRESULT hr = document->get_body(&body);
    if(SUCCEEDED(hr))
    {
        BSTR out;
        body->get_innerText(&out);
        text = wxString(out);
        body->Release();
    }
    document->Release();
    return text;
}

void wxWebViewIE::RunScript(const wxString& javascript)
{
    IHTMLDocument2* document = GetDocument();
    IHTMLWindow2* window;
    wxString language = "javascript";
    HRESULT hr = document->get_parentWindow(&window);
    if(SUCCEEDED(hr))
    {
        VARIANT level;
        VariantInit(&level);
        V_VT(&level) = VT_EMPTY;
        window->execScript(SysAllocString(javascript), SysAllocString(language), &level);
    }
    document->Release();
}

bool wxWebViewIE::CanExecCommand(wxString command)
{
    IHTMLDocument2* document = GetDocument();
    VARIANT_BOOL enabled;

    document->queryCommandEnabled(SysAllocString(command.wc_str()), &enabled);
    document->Release();

    return (enabled == VARIANT_TRUE);
}

void wxWebViewIE::ExecCommand(wxString command)
{
    IHTMLDocument2* document = GetDocument();
    document->execCommand(SysAllocString(command.wc_str()), VARIANT_FALSE, VARIANT(), NULL);
    document->Release();
}

IHTMLDocument2* wxWebViewIE::GetDocument()
{
    wxVariant variant = m_ie.GetProperty("Document");
    IHTMLDocument2* document = (IHTMLDocument2*)variant.GetVoidPtr();

    wxASSERT(document);

    return document;
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
            //Only send a complete even if we are actually finished, this brings
            //the event in to line with webkit
            READYSTATE rs;
            m_webBrowser->get_ReadyState( &rs );
            if(rs != READYSTATE_COMPLETE)
                break;

            wxString url = evt[1].GetString();

            //As we are complete we also add to the history list, but not if the
            //page is not the main page, ie it is a subframe
            if(m_historyEnabled && !m_historyLoadingFromList && url == GetCurrentURL())
            {
                //If we are not at the end of the list, then erase everything
                //between us and the end before adding the new page
                if(m_historyPosition != static_cast<int>(m_historyList.size()) - 1)
                {
                    m_historyList.erase(m_historyList.begin() + m_historyPosition + 1,
                                        m_historyList.end());
                }
                wxSharedPtr<wxWebHistoryItem> item(new wxWebHistoryItem(url, GetCurrentTitle()));
                m_historyList.push_back(item);
                m_historyPosition++;
            }
            //Reset as we are done now
            m_historyLoadingFromList = false;
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
        case DISPID_NEWWINDOW3:
        {
            wxString url = evt[4].GetString();

            wxWebNavigationEvent event(wxEVT_COMMAND_WEB_VIEW_NEWWINDOW,
                                       GetId(), url, wxEmptyString, true);
            event.SetEventObject(this);
            HandleWindowEvent(event);

            //We always cancel this event otherwise an Internet Exporer window
            //is opened for the url
            wxActiveXEventNativeMSW* nativeParams = evt.GetNativeParameters();
            *V_BOOLREF(&nativeParams->pDispParams->rgvarg[3]) = VARIANT_TRUE;
            break;
        }
    }

    evt.Skip();
}

VirtualProtocol::VirtualProtocol()
{
    m_refCount = 0;
    m_file = NULL;
    m_fileSys = new wxFileSystem;
}

VirtualProtocol::~VirtualProtocol()
{
    wxDELETE(m_fileSys);
}

ULONG VirtualProtocol::AddRef()
{
    m_refCount++;
    return m_refCount;
}

HRESULT VirtualProtocol::QueryInterface(REFIID riid, void **ppvObject)
{
    if ((riid == IID_IUnknown) || (riid == IID_IInternetProtocol)
       || (riid == IID_IInternetProtocolRoot))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_POINTER;
    }
}

ULONG VirtualProtocol::Release()
{
    m_refCount--;
    if (m_refCount > 0)
    {
        return m_refCount;
    }
    else
    {
        delete this;
        return 0;
    }
}

HRESULT VirtualProtocol::Start(LPCWSTR szUrl, IInternetProtocolSink *pOIProtSink,
                               IInternetBindInfo *pOIBindInfo, DWORD grfPI, 
                               HANDLE_PTR dwReserved)
{
    wxUnusedVar(szUrl);
    wxUnusedVar(pOIBindInfo);
    wxUnusedVar(grfPI);
    wxUnusedVar(dwReserved);
    m_protocolSink = pOIProtSink;
    //We have to clean up incoming paths from the webview control as they are
    //not properly escaped, see also the comment in filesys.cpp line 668
    wxString path = wxString(szUrl).BeforeFirst(':') +  ":" + 
                    EscapeFileNameCharsInURL(wxString(szUrl).AfterFirst(':'));
    path.Replace("///", "/");
    m_file = m_fileSys->OpenFile(path);

    if(!m_file)
        return INET_E_RESOURCE_NOT_FOUND;

    //We return the stream length for current and total size as we can always
    //read the whole file from the stream
    m_protocolSink->ReportData(BSCF_FIRSTDATANOTIFICATION | 
                               BSCF_DATAFULLYAVAILABLE |
                               BSCF_LASTDATANOTIFICATION,
                               m_file->GetStream()->GetLength(),
                               m_file->GetStream()->GetLength());
    return S_OK; 
}

HRESULT VirtualProtocol::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
    //If the file is null we return false to indicte it is finished
    if(!m_file) 
        return S_FALSE;

    wxStreamError err = m_file->GetStream()->Read(pv, cb).GetLastError();
    *pcbRead = m_file->GetStream()->LastRead();

    if(err == wxSTREAM_NO_ERROR)
    {
        if(*pcbRead < cb)
        {
            wxDELETE(m_file);
            m_protocolSink->ReportResult(S_OK, 0, NULL);
        }
        //As we are not eof there is more data
        return S_OK;
    }
    else if(err == wxSTREAM_EOF)
    {
        wxDELETE(m_file);
        m_protocolSink->ReportResult(S_OK, 0, NULL);
        //We are eof and so finished
        return S_OK;
    }
    else if(err ==  wxSTREAM_READ_ERROR)
    {
        wxDELETE(m_file);
        return INET_E_DOWNLOAD_FAILURE;
    }
    else
    {
        //Dummy return to surpress a compiler warning
        wxFAIL;
        return INET_E_DOWNLOAD_FAILURE;
    }
}

HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid,
                                     void ** ppvObject)
{
    if (pUnkOuter) 
        return CLASS_E_NOAGGREGATION;
    VirtualProtocol* vp = new VirtualProtocol;
    vp->AddRef();
    HRESULT hr = vp->QueryInterface(riid, ppvObject);
    vp->Release();
    return hr;

} 

STDMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    wxUnusedVar(fLock);
    return S_OK;
}

ULONG ClassFactory::AddRef(void)
{
    m_refCount++;
    return m_refCount;
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void **ppvObject)
{
    if ((riid == IID_IUnknown) || (riid == IID_IClassFactory))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_POINTER;
    }

}

ULONG ClassFactory::Release(void)
{
    m_refCount--;
    if (m_refCount > 0)
    {
        return m_refCount;
    }
    else
    {
        delete this;
        return 0;
    }

} 

#endif
