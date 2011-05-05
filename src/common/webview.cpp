/////////////////////////////////////////////////////////////////////////////
// Name:        webview.cpp
// Purpose:     Common interface and events for web view component
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEB

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#include "wx/webview.h"

#include "wx/osx/webview.h"
#include "wx/gtk/webview.h"
#include "wx/msw/webviewie.h"

// DLL options compatibility check:
#include "wx/app.h"
WX_CHECK_BUILD_OPTIONS("wxWEB")

extern WXDLLIMPEXP_DATA_WEB(const char) wxWebViewNameStr[] = "wxWebView";
extern WXDLLIMPEXP_DATA_WEB(const char) wxWebViewDefaultURLStr[] = "about:blank";

IMPLEMENT_DYNAMIC_CLASS(wxWebNavigationEvent, wxCommandEvent)

wxDEFINE_EVENT( wxEVT_COMMAND_WEB_VIEW_NAVIGATING, wxWebNavigationEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_WEB_VIEW_NAVIGATED, wxWebNavigationEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_WEB_VIEW_LOADED, wxWebNavigationEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_WEB_VIEW_ERROR, wxWebNavigationEvent );

// static
wxWebView* wxWebView::New(wxWebViewBackend backend)
{
    switch (backend)
    {
        #if wxHAVE_WEB_BACKEND_OSX_WEBKIT
            case wxWEB_VIEW_BACKEND_OSX_WEBKIT:
                return new wxOSXWebKitCtrl();
        #endif

        #if wxHAVE_WEB_BACKEND_GTK_WEBKIT
            case wxWEB_VIEW_BACKEND_GTK_WEBKIT:
                return new wxGtkWebKitCtrl();
        #endif

        #if wxHAVE_WEB_BACKEND_IE
            case wxWEB_VIEW_BACKEND_IE:
                return new wxIEPanel();
        #endif

        case wxWEB_VIEW_BACKEND_DEFAULT:

            #if wxHAVE_WEB_BACKEND_OSX_WEBKIT
            return new wxOSXWebKitCtrl();
            #endif

            #if wxHAVE_WEB_BACKEND_GTK_WEBKIT
            return new wxGtkWebKitCtrl();
            #endif

            #if wxHAVE_WEB_BACKEND_IE
            return new wxIEPanel();
            #endif

        // fall-through intended
        default:
            return NULL;
    }
}

// static
wxWebView* wxWebView::New(wxWindow* parent,
       wxWindowID id,
       const wxString& url,
       const wxPoint& pos,
       const wxSize& size,
       wxWebViewBackend backend,
       long style,
       const wxString& name)
{
    switch (backend)
    {
        #if wxHAVE_WEB_BACKEND_OSX_WEBKIT
            case wxWEB_VIEW_BACKEND_OSX_WEBKIT:
                return new wxOSXWebKitCtrl(parent, id, url, pos, size, style,
                                           name);
        #endif

        #if wxHAVE_WEB_BACKEND_GTK_WEBKIT
            case wxWEB_VIEW_BACKEND_GTK_WEBKIT:
                return new wxGtkWebKitCtrl(parent, id, url, pos, size, style,
                                           name);
        #endif

        #if wxHAVE_WEB_BACKEND_IE
            case wxWEB_VIEW_BACKEND_IE:
                return new wxIEPanel(parent, id, url, pos, size, style, name);
        #endif

        case wxWEB_VIEW_BACKEND_DEFAULT:

            #if wxHAVE_WEB_BACKEND_OSX_WEBKIT
            return new wxOSXWebKitCtrl(parent, id, url, pos, size, style, name);
            #endif

            #if wxHAVE_WEB_BACKEND_GTK_WEBKIT
            return new wxGtkWebKitCtrl(parent, id, url, pos, size, style, name);
            #endif

            #if wxHAVE_WEB_BACKEND_IE
            return new wxIEPanel(parent, id, url, pos, size, style, name);
            #endif

        // fall-through intended
        default:
            return NULL;
    }
}

#endif // wxUSE_WEB
