/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/osx/webkit.h
// Purpose:     wxWebViewWebKit - embeddable web kit control,
//                             OS X implementation of web view component
// Author:      Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Modified by:
// Created:     2004-4-16
// Copyright:   (c) Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBKIT_H
#define _WX_WEBKIT_H

#include "wx/defs.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT && defined(__WXOSX__)

#include "wx/control.h"
#include "wx/webview.h"

#include "wx/osx/core/objcid.h"

#include <unordered_map>

// ----------------------------------------------------------------------------
// Web Kit Control
// ----------------------------------------------------------------------------

using wxStringToWebHandlerMap = std::unordered_map<wxString, wxSharedPtr<wxWebViewHandler>>;

class wxWebViewConfigurationImplWebKit;

class WXDLLIMPEXP_WEBVIEW wxWebViewWebKit : public wxWebView
{
public:
    explicit wxWebViewWebKit(const wxWebViewConfiguration& config, WX_NSObject request = nullptr);

    bool Create(wxWindow *parent,
                wxWindowID winID = wxID_ANY,
                const wxString& strURL = wxASCII_STR(wxWebViewDefaultURLStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = wxASCII_STR(wxWebViewNameStr)) override;
    virtual ~wxWebViewWebKit();

    virtual bool CanGoBack() const override;
    virtual bool CanGoForward() const override;
    virtual void GoBack() override;
    virtual void GoForward() override;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) override;
    virtual void Stop() override;

    virtual void Print() override;

    virtual void LoadURL(const wxString& url) override;
    virtual wxString GetCurrentURL() const override;
    virtual wxString GetCurrentTitle() const override;
    virtual float GetZoomFactor() const override;
    virtual void SetZoomFactor(float zoom) override;

    virtual void SetZoomType(wxWebViewZoomType zoomType) override;
    virtual wxWebViewZoomType GetZoomType() const override;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const override;

    virtual bool IsBusy() const override;

    virtual bool IsAccessToDevToolsEnabled() const override;
    virtual void EnableAccessToDevTools(bool enable = true) override;
    virtual bool SetUserAgent(const wxString& userAgent) override;

    //History functions
    virtual void ClearHistory() override;
    virtual void EnableHistory(bool enable = true) override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() override;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) override;

    virtual void Paste() override;

    //Undo / redo functionality
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;
    virtual void Undo() override;
    virtual void Redo() override;

    //Editing functions
    virtual void SetEditable(bool enable = true) override;
    virtual bool IsEditable() const override;

    virtual void RunScriptAsync(const wxString& javascript, void* clientData = nullptr) const override;
    virtual bool AddScriptMessageHandler(const wxString& name) override;
    virtual bool RemoveScriptMessageHandler(const wxString& name) override;
    virtual bool AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime = wxWEBVIEW_INJECT_AT_DOCUMENT_START) override;
    virtual void RemoveAllUserScripts() override;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;

    virtual void* GetNativeBackend() const override { return m_webView; }

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

    wxDECLARE_EVENT_TABLE();

private:
    wxWebViewConfiguration m_configuration;
    OSXWebViewPtr m_webView;
    wxStringToWebHandlerMap m_handlers;
    wxString m_customUserAgent;
    WX_NSObject m_request;

    WX_NSObject m_navigationDelegate;
    WX_NSObject m_UIDelegate;

    void Init();
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryWebKit : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return CreateWithConfig(CreateConfiguration()); }
    virtual wxWebView* CreateWithConfig(const wxWebViewConfiguration& config) override { return new wxWebViewWebKit(config); }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) override
    {
        auto webView = CreateWithConfig(CreateConfiguration());
        if (webView->Create(parent, id, url, pos, size, style, name))
            return webView;
        else
            return nullptr;
    }
    virtual wxVersionInfo GetVersionInfo() override;
    virtual wxWebViewConfiguration CreateConfiguration() override;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT

#endif // _WX_WEBKIT_H_
