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

#ifndef __ICoreWebView2Environment_INTERFACE_DEFINED__
    #error "WebView2 SDK version 0.9.430 or newer is required"
#endif

class wxWebViewEdgeImpl
{
public:
    explicit wxWebViewEdgeImpl(wxWebViewEdge* webview);
    ~wxWebViewEdgeImpl();

    bool Create();

    wxWebViewEdge* m_ctrl;

    wxCOMPtr<ICoreWebView2Environment> m_webViewEnvironment;
    wxCOMPtr<ICoreWebView2> m_webView;
    wxCOMPtr<ICoreWebView2Controller> m_webViewController;

    bool m_initialized;
    bool m_isBusy;
    wxString m_pendingURL;
    int m_pendingContextMenuEnabled;
    int m_pendingAccessToDevToolsEnabled;

    // WebView Events tokens
    EventRegistrationToken m_navigationStartingToken = { };
    EventRegistrationToken m_navigationCompletedToken = { };
    EventRegistrationToken m_newWindowRequestedToken = { };
    EventRegistrationToken m_documentTitleChangedToken = { };
    EventRegistrationToken m_contentLoadingToken = { };

    // WebView Event handlers
    HRESULT OnNavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args);
    HRESULT OnNavigationCompleted(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args);
    HRESULT OnNewWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
    HRESULT OnDocumentTitleChanged(ICoreWebView2* sender, IUnknown* args);
    HRESULT OnContentLoading(ICoreWebView2* sender, ICoreWebView2ContentLoadingEventArgs* args);

    HRESULT OnEnvironmentCreated(HRESULT result, ICoreWebView2Environment* environment);
    HRESULT OnWebViewCreated(HRESULT result, ICoreWebView2Controller* webViewController);

    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    void UpdateBounds();

    ICoreWebView2Settings* GetSettings();

    static bool ms_isInitialized;
    static wxDynamicLibrary ms_loaderDll;

    static bool Initialize();

    static void Uninitialize();

    friend class wxWebViewEdgeModule;
};

#endif // wxWebViewEdge_PRIVATE_H
