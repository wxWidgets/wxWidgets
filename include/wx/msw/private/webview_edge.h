/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webview_edge.h
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

    EventRegistrationToken m_navigationStartingToken = { };
    EventRegistrationToken m_navigationCompletedToken = { };
    EventRegistrationToken m_newWindowRequestedToken = { };
    EventRegistrationToken m_documentStateChangedToken = { };

    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    void InitWebViewCtrl();

    void UpdateBounds();

    IWebView2Settings2* GetSettings();

    static int ms_isAvailable;
    static wxDynamicLibrary ms_loaderDll;

    static bool Initialize();

    static void Uninitalize();

    friend class wxWebViewEdgeModule;
};

#endif // wxWebViewEdge_PRIVATE_H
