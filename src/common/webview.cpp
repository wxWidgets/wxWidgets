/////////////////////////////////////////////////////////////////////////////
// Name:        webview.cpp
// Purpose:     Common interface and events for web view component
// Author:      Marianne Gagnon
// Copyright:   (c) 2010 Marianne Gagnon, 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEBVIEW


#include "wx/webview.h"

#if defined(__WXOSX__)
#include "wx/osx/webview_webkit.h"
#elif defined(__WXGTK__)
#include "wx/gtk/webview_webkit.h"
#elif defined(__WXMSW__)
#include "wx/msw/webview_ie.h"
#include "wx/msw/webview_edge.h"
#endif

// DLL options compatibility check:
#include "wx/app.h"
WX_CHECK_BUILD_OPTIONS("wxWEBVIEW")

extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewNameStr[] = "wxWebView";
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewDefaultURLStr[] = "about:blank";
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendIE[] = "wxWebViewIE";
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendEdge[] = "wxWebViewEdge";
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendWebKit[] = "wxWebViewWebKit";

#ifdef __WXMSW__
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendDefault[] = "";
#else
extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendDefault[] = "wxWebViewWebKit";
#endif

wxIMPLEMENT_ABSTRACT_CLASS(wxWebView, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_WEBVIEW_NAVIGATING, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_NAVIGATED, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_LOADED, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_ERROR, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_NEWWINDOW, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_TITLE_CHANGED, wxWebViewEvent );
wxDEFINE_EVENT( wxEVT_WEBVIEW_FULLSCREEN_CHANGED, wxWebViewEvent);
wxDEFINE_EVENT( wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, wxWebViewEvent);

wxStringWebViewFactoryMap wxWebView::m_factoryMap;

wxWebViewZoom wxWebView::GetZoom() const
{
    float zoom = GetZoomFactor();

    // arbitrary way to map float zoom to our common zoom enum
    if (zoom <= 0.55f)
    {
        return wxWEBVIEW_ZOOM_TINY;
    }
    if (zoom <= 0.85f)
    {
        return wxWEBVIEW_ZOOM_SMALL;
    }
    if (zoom <= 1.15f)
    {
        return wxWEBVIEW_ZOOM_MEDIUM;
    }
    if (zoom <= 1.45f)
    {
        return wxWEBVIEW_ZOOM_LARGE;
    }

    return wxWEBVIEW_ZOOM_LARGEST;
}

void wxWebView::SetZoom(wxWebViewZoom zoom)
{
    // arbitrary way to map our common zoom enum to float zoom
    switch (zoom)
    {
        case wxWEBVIEW_ZOOM_TINY:
            SetZoomFactor(0.4f);
            break;

        case wxWEBVIEW_ZOOM_SMALL:
            SetZoomFactor(0.7f);
            break;

        case wxWEBVIEW_ZOOM_MEDIUM:
            SetZoomFactor(1.0f);
            break;

        case wxWEBVIEW_ZOOM_LARGE:
            SetZoomFactor(1.3f);
            break;

        case wxWEBVIEW_ZOOM_LARGEST:
            SetZoomFactor(1.6f);
            break;
    }
}

bool wxWebView::QueryCommandEnabled(const wxString& command) const
{
    wxString resultStr;
    RunScript(
        wxString::Format("function f(){ return document.queryCommandEnabled('%s'); } f();", command), &resultStr);
    return resultStr.IsSameAs("true", false);
}

void wxWebView::ExecCommand(const wxString& command)
{
    RunScript(wxString::Format("document.execCommand('%s');", command));
}

wxString wxWebView::GetPageSource() const
{
    wxString text;
    RunScript("document.documentElement.outerHTML;", &text);
    return text;
}

wxString wxWebView::GetPageText() const
{
    wxString text;
    RunScript("document.body.innerText;", &text);
    return text;
}

bool wxWebView::CanCut() const
{
    return QueryCommandEnabled("cut");
}

bool wxWebView::CanCopy() const
{
    return QueryCommandEnabled("copy");
}

bool wxWebView::CanPaste() const
{
    return QueryCommandEnabled("paste");
}

void wxWebView::Cut()
{
    ExecCommand("cut");
}

void wxWebView::Copy()
{
    ExecCommand("copy");
}

void wxWebView::Paste()
{
    ExecCommand("paste");
}

wxString wxWebView::GetSelectedText() const
{
    wxString text;
    RunScript("window.getSelection().toString();", &text);
    return text;
}

wxString wxWebView::GetSelectedSource() const
{
    // TODO: could probably be implemented by script similar to GetSelectedText()
    return wxString();
}

void wxWebView::DeleteSelection()
{
    ExecCommand("delete");
}

bool wxWebView::HasSelection() const
{
    wxString rangeCountStr;
    RunScript("window.getSelection().rangeCount;", &rangeCountStr);
    return rangeCountStr != "0";
}

void wxWebView::ClearSelection()
{
    //We use javascript as selection isn't exposed at the moment in webkit
    RunScript("window.getSelection().removeAllRanges();");
}

void wxWebView::SelectAll()
{
    RunScript("window.getSelection().selectAllChildren(document.body);");
}

long wxWebView::Find(const wxString& text, int flags)
{
    if (text != m_findText)
        ClearSelection();
    m_findText = text;
    wxString output;
    RunScript(wxString::Format("window.find('%s', %s, %s, %s, %s)",
        text,
        (flags & wxWEBVIEW_FIND_MATCH_CASE) ? "true" : "false",
        (flags & wxWEBVIEW_FIND_BACKWARDS) ? "true" : "false",
        (flags & wxWEBVIEW_FIND_WRAP) ? "true" : "false",
        (flags & wxWEBVIEW_FIND_ENTIRE_WORD) ? "true" : "false"
        ), &output);
    if (output.IsSameAs("false", false))
        return wxNOT_FOUND;
    else
        return 1;
}

wxString wxWebView::GetUserAgent() const
{
    wxString userAgent;
    RunScript("navigator.userAgent", &userAgent);
    return userAgent;
}

// static
wxWebView* wxWebView::New(const wxString& backend)
{
    wxStringWebViewFactoryMap::iterator iter = FindFactory(backend);

    if(iter == m_factoryMap.end())
        return NULL;
    else
        return (*iter).second->Create();
}

// static
wxWebView* wxWebView::New(wxWindow* parent, wxWindowID id, const wxString& url,
                          const wxPoint& pos, const wxSize& size,
                          const wxString& backend, long style,
                          const wxString& name)
{
    wxStringWebViewFactoryMap::iterator iter = FindFactory(backend);

    if(iter == m_factoryMap.end())
        return NULL;
    else
        return (*iter).second->Create(parent, id, url, pos, size, style, name);

}

// static
void wxWebView::RegisterFactory(const wxString& backend,
                                wxSharedPtr<wxWebViewFactory> factory)
{
    m_factoryMap[backend] = factory;
}

// static
bool wxWebView::IsBackendAvailable(const wxString& backend)
{
    wxStringWebViewFactoryMap::iterator iter = FindFactory(backend);
    if (iter != m_factoryMap.end())
        return iter->second->IsAvailable();
    else
        return false;
}

wxVersionInfo wxWebView::GetBackendVersionInfo(const wxString& backend)
{
    wxStringWebViewFactoryMap::iterator iter = FindFactory(backend);
    if (iter != m_factoryMap.end())
        return iter->second->GetVersionInfo();
    else
        return wxVersionInfo();
}

// static
wxStringWebViewFactoryMap::iterator wxWebView::FindFactory(const wxString &backend)
{
    // Initialise the map, it checks internally for existing factories
    InitFactoryMap();

#ifdef __WXMSW__
    // Use edge as default backend on MSW if available
    if (backend.empty())
    {
        wxStringWebViewFactoryMap::iterator defaultBackend =
            m_factoryMap.find(wxWebViewBackendIE);
#if wxUSE_WEBVIEW_EDGE
        wxStringWebViewFactoryMap::iterator edgeFactory = m_factoryMap.find(wxWebViewBackendEdge);
        if (edgeFactory->second->IsAvailable())
            return edgeFactory;
#endif
        return defaultBackend;
    }
    else
#endif
        return m_factoryMap.find(backend);
}

// static
void wxWebView::InitFactoryMap()
{
#ifdef __WXMSW__
#if wxUSE_WEBVIEW_IE
    if(m_factoryMap.find(wxWebViewBackendIE) == m_factoryMap.end())
        RegisterFactory(wxWebViewBackendIE, wxSharedPtr<wxWebViewFactory>
                                                   (new wxWebViewFactoryIE));
#endif

#if wxUSE_WEBVIEW_EDGE
    if (m_factoryMap.find(wxWebViewBackendEdge) == m_factoryMap.end())
        RegisterFactory(wxWebViewBackendEdge, wxSharedPtr<wxWebViewFactory>
        (new wxWebViewFactoryEdge));
#endif

#else
    if(m_factoryMap.find(wxWebViewBackendWebKit) == m_factoryMap.end())
        RegisterFactory(wxWebViewBackendWebKit, wxSharedPtr<wxWebViewFactory>
                                                       (new wxWebViewFactoryWebKit));
#endif
}

#endif // wxUSE_WEBVIEW
