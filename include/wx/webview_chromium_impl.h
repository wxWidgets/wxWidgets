///////////////////////////////////////////////////////////////////////////////
// Name:        wx/webview_chromium_impl.h
// Purpose:     Helpers for implementing custom CefClient for wxWebViewChromium
// Author:      Vadim Zeitlin
// Created:     2024-02-17
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEW_CHROMIUM_IMPL_H_
#define _WX_WEBVIEW_CHROMIUM_IMPL_H_

// Note that this header includes CEF headers and so the appropriate include
// path should be set up when using it.

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

wxGCC_WARNING_SUPPRESS(unused-parameter)

#include "include/cef_client.h"

wxGCC_WARNING_RESTORE(unused-parameter)

#ifdef __VISUALC__
#pragma warning(pop)
#endif

// ----------------------------------------------------------------------------
// Convenient base class for custom CefClient implementations.
// ----------------------------------------------------------------------------

class wxDelegatingCefClient : public CefClient
{
public:
    // Forward all CefClient methods to the original client.
    virtual CefRefPtr<CefAudioHandler> GetAudioHandler() override
        { return m_clientOrig->GetAudioHandler(); }
      virtual CefRefPtr<CefCommandHandler> GetCommandHandler() override
        { return m_clientOrig->GetCommandHandler(); }
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override
        { return m_clientOrig->GetContextMenuHandler(); }
    virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override
        { return m_clientOrig->GetDialogHandler(); }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
        { return m_clientOrig->GetDisplayHandler(); }
    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override
        { return m_clientOrig->GetDownloadHandler(); }
    virtual CefRefPtr<CefDragHandler> GetDragHandler() override
        { return m_clientOrig->GetDragHandler(); }
    virtual CefRefPtr<CefFindHandler> GetFindHandler() override
        { return m_clientOrig->GetFindHandler(); }
    virtual CefRefPtr<CefFocusHandler> GetFocusHandler() override
        { return m_clientOrig->GetFocusHandler(); }
    virtual CefRefPtr<CefFrameHandler> GetFrameHandler() override
        { return m_clientOrig->GetFrameHandler(); }
    virtual CefRefPtr<CefPermissionHandler> GetPermissionHandler() override
        { return m_clientOrig->GetPermissionHandler(); }
    virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override
        { return m_clientOrig->GetJSDialogHandler(); }
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override
        { return m_clientOrig->GetKeyboardHandler(); }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
        { return m_clientOrig->GetLifeSpanHandler(); }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
        { return m_clientOrig->GetLoadHandler(); }
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override
        { return m_clientOrig->GetPrintHandler(); }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override
        { return m_clientOrig->GetRenderHandler(); }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override
        { return m_clientOrig->GetRequestHandler(); }
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override
    {
        return m_clientOrig->OnProcessMessageReceived(browser, frame,
                                                      source_process, message);
    }

protected:
    // Objects of this class shouldn't be created, only derived classes should
    // be used, hence the constructor is protected.
    explicit wxDelegatingCefClient(CefClient* clientOrig)
        : m_clientOrig{clientOrig}
    {
    }

    CefRefPtr<CefClient> const m_clientOrig;

    IMPLEMENT_REFCOUNTING(wxDelegatingCefClient);
};

#endif // _WX_WEBVIEW_CHROMIUM_IMPL_H_
