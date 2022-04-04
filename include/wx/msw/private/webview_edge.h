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

#if !wxUSE_WEBVIEW_EDGE_STATIC
#include "wx/dynlib.h"
#endif
#include "wx/msw/private/comptr.h"

#include <WebView2.h>

#ifndef __ICoreWebView2_2_INTERFACE_DEFINED__
    #error "WebView2 SDK version 1.0.705.50 or newer is required"
#endif

#ifndef __VISUALC__
__CRT_UUID_DECL(ICoreWebView2_2, 0x9E8F0CF8, 0xE670, 0x4B5E, 0xB2, 0xBC, 0x73, 0xE0, 0x61, 0xE3, 0x18, 0x4C);
__CRT_UUID_DECL(ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler, 0xb99369f3, 0x9b11, 0x47b5, 0xbc,0x6f, 0x8e,0x78,0x95,0xfc,0xea,0x17);
__CRT_UUID_DECL(ICoreWebView2ContainsFullScreenElementChangedEventHandler, 0xe45d98b1, 0xafef, 0x45be, 0x8b,0xaf, 0x6c,0x77,0x28,0x86,0x7f,0x73);
__CRT_UUID_DECL(ICoreWebView2ContentLoadingEventHandler, 0x364471e7, 0xf2be, 0x4910, 0xbd,0xba, 0xd7,0x20,0x77,0xd5,0x1c,0x4b);
__CRT_UUID_DECL(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler, 0x6c4819f3, 0xc9b7, 0x4260, 0x81,0x27, 0xc9,0xf5,0xbd,0xe7,0xf6,0x8c);
__CRT_UUID_DECL(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler, 0x4e8a3389, 0xc9d8, 0x4bd2, 0xb6,0xb5, 0x12,0x4f,0xee,0x6c,0xc1,0x4d);
__CRT_UUID_DECL(ICoreWebView2DocumentTitleChangedEventHandler, 0xf5f2b923, 0x953e, 0x4042, 0x9f,0x95, 0xf3,0xa1,0x18,0xe1,0xaf,0xd4);
__CRT_UUID_DECL(ICoreWebView2DOMContentLoadedEventHandler, 0x4BAC7E9C, 0x199E, 0x49ED, 0x87, 0xED, 0x24, 0x93, 0x03, 0xAC, 0xF0, 0x19);
__CRT_UUID_DECL(ICoreWebView2Environment, 0xb96d755e, 0x0319, 0x4e92, 0xa2,0x96, 0x23,0x43,0x6f,0x46,0xa1,0xfc);
__CRT_UUID_DECL(ICoreWebView2EnvironmentOptions, 0x2fde08a8, 0x1e9a, 0x4766, 0x8c,0x05, 0x95,0xa9,0xce,0xb9,0xd1,0xc5);
__CRT_UUID_DECL(ICoreWebView2ExecuteScriptCompletedHandler, 0x49511172, 0xcc67, 0x4bca, 0x99,0x23, 0x13,0x71,0x12,0xf4,0xc4,0xcc);
__CRT_UUID_DECL(ICoreWebView2NavigationCompletedEventHandler, 0xd33a35bf, 0x1c49, 0x4f98, 0x93,0xab, 0x00,0x6e,0x05,0x33,0xfe,0x1c);
__CRT_UUID_DECL(ICoreWebView2NavigationStartingEventHandler, 0x9adbe429, 0xf36d, 0x432b, 0x9d,0xdc, 0xf8,0x88,0x1f,0xbd,0x76,0xe3);
__CRT_UUID_DECL(ICoreWebView2NewWindowRequestedEventHandler, 0xd4c185fe, 0xc81c, 0x4989, 0x97,0xaf, 0x2d,0x3f,0xa7,0xab,0x56,0x51);
__CRT_UUID_DECL(ICoreWebView2SourceChangedEventHandler, 0x3c067f9f, 0x5388, 0x4772, 0x8b,0x48, 0x79,0xf7,0xef,0x1a,0xb3,0x7c);
__CRT_UUID_DECL(ICoreWebView2WebMessageReceivedEventHandler, 0x57213f19, 0x00e6, 0x49fa, 0x8e,0x07, 0x89,0x8e,0xa0,0x1e,0xcb,0xd2);
#endif

class wxWebViewEdgeImpl
{
public:
    explicit wxWebViewEdgeImpl(wxWebViewEdge* webview);
    ~wxWebViewEdgeImpl();

    bool Create();

    wxWebViewEdge* m_ctrl;

    wxCOMPtr<ICoreWebView2Environment> m_webViewEnvironment;
    wxCOMPtr<ICoreWebView2_2> m_webView;
    wxCOMPtr<ICoreWebView2Controller> m_webViewController;

    bool m_initialized;
    bool m_isBusy;
    wxString m_pendingURL;
    wxString m_pendingPage;
    int m_pendingContextMenuEnabled;
    int m_pendingAccessToDevToolsEnabled;
    wxVector<wxString> m_pendingUserScripts;
    wxVector<wxString> m_userScriptIds;
    wxString m_scriptMsgHandlerName;
    wxString m_customUserAgent;

    // WebView Events tokens
    EventRegistrationToken m_navigationStartingToken = { };
    EventRegistrationToken m_sourceChangedToken = { };
    EventRegistrationToken m_navigationCompletedToken = { };
    EventRegistrationToken m_newWindowRequestedToken = { };
    EventRegistrationToken m_documentTitleChangedToken = { };
    EventRegistrationToken m_DOMContentLoadedToken = { };
    EventRegistrationToken m_containsFullScreenElementChangedToken = { };
    EventRegistrationToken m_webMessageReceivedToken = { };

    // WebView Event handlers
    HRESULT OnNavigationStarting(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args);
    HRESULT OnSourceChanged(ICoreWebView2* sender, ICoreWebView2SourceChangedEventArgs* args);
    HRESULT OnNavigationCompleted(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args);
    HRESULT OnNewWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
    HRESULT OnDocumentTitleChanged(ICoreWebView2* sender, IUnknown* args);
    HRESULT OnDOMContentLoaded(ICoreWebView2* sender, ICoreWebView2DOMContentLoadedEventArgs* args);
    HRESULT OnContainsFullScreenElementChanged(ICoreWebView2* sender, IUnknown* args);
    HRESULT OnWebMessageReceived(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);
    HRESULT OnAddScriptToExecuteOnDocumentedCreatedCompleted(HRESULT errorCode, LPCWSTR id);

    HRESULT OnEnvironmentCreated(HRESULT result, ICoreWebView2Environment* environment);
    HRESULT OnWebViewCreated(HRESULT result, ICoreWebView2Controller* webViewController);

    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    void UpdateBounds();

    ICoreWebView2Settings* GetSettings();

    void UpdateWebMessageHandler();

#if !wxUSE_WEBVIEW_EDGE_STATIC
    static wxDynamicLibrary ms_loaderDll;
#endif
    static wxString ms_browserExecutableDir;

    static bool Initialize();

    static void Uninitialize();

    friend class wxWebViewEdgeModule;
};

#endif // wxWebViewEdge_PRIVATE_H
