/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/private/webview_edge.h
// Purpose:     wxMSW Edge Chromium wxWebView backend private classes
// Author:      Tobias Taschner
// Created:     2020-01-15
// Copyright:   (c) 2020 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewEdge_PRIVATE_H
#define wxWebViewEdge_PRIVATE_H

#include "wx/dynlib.h"
#include "wx/msw/private/comptr.h"

#include <Webview2.h>

class wxWebViewEdgeImpl
{
public:
    explicit wxWebViewEdgeImpl(wxWebViewEdge* webview);
    ~wxWebViewEdgeImpl();

    bool Create();

    wxWebViewEdge* m_ctrl;

    wxCOMPtr<IWebView2Environment3> m_webViewEnvironment;
    wxCOMPtr<IWebView2WebView5> m_webView;

    bool m_initialized;
    bool m_isBusy;
    wxString m_pendingURL;

    // WebView Events tokens
    EventRegistrationToken m_navigationStartingToken = { };
    EventRegistrationToken m_navigationCompletedToken = { };
    EventRegistrationToken m_newWindowRequestedToken = { };

    // WebView Event handlers
    HRESULT OnNavigationStarting(IWebView2WebView* sender, IWebView2NavigationStartingEventArgs* args);
    HRESULT OnNavigationCompleted(IWebView2WebView* sender, IWebView2NavigationCompletedEventArgs* args);
    HRESULT OnNewWindowRequested(IWebView2WebView* sender, IWebView2NewWindowRequestedEventArgs* args);

    HRESULT OnEnvironmentCreated(HRESULT result, IWebView2Environment* environment);
    HRESULT OnWebViewCreated(HRESULT result, IWebView2WebView* webview);

    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    void UpdateBounds();

    IWebView2Settings2* GetSettings();

    static int ms_isAvailable;
    static wxDynamicLibrary ms_loaderDll;

    static bool Initialize();

    static void Uninitalize();

    friend class wxWebViewEdgeModule;
};

#endif // wxWebViewEdge_PRIVATE_H
