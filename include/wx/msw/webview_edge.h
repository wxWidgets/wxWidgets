/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webview_edge.h
// Purpose:     wxMSW Edge Chromium wxWebView backend
// Author:      Markus Pingel
// Created:     2019-12-15
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewEdge_H
#define wxWebViewEdge_H

#include "wx/setup.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE && defined(__WXMSW__)

#include "wx/control.h"
#include "wx/webview.h"

class wxWebViewEdgeImpl;

class WXDLLIMPEXP_WEBVIEW wxWebViewEdge : public wxWebView
{
public:

    wxWebViewEdge();

    explicit wxWebViewEdge(const wxWebViewConfiguration& config);

    wxWebViewEdge(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr);

    ~wxWebViewEdge();

    bool Create(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr) override;

    virtual void LoadURL(const wxString& url) override;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() override;

    virtual bool CanGoForward() const override;
    virtual bool CanGoBack() const override;
    virtual void GoBack() override;
    virtual void GoForward() override;
    virtual void ClearHistory() override;
    virtual void EnableHistory(bool enable = true) override;
    virtual void Stop() override;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) override;

    virtual bool IsBusy() const override;
    virtual wxString GetCurrentURL() const override;
    virtual wxString GetCurrentTitle() const override;

    virtual void SetZoomType(wxWebViewZoomType) override;
    virtual wxWebViewZoomType GetZoomType() const override;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const override;

    virtual void Print() override;

    virtual float GetZoomFactor() const override;
    virtual void SetZoomFactor(float zoom) override;

    //Undo / redo functionality
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;
    virtual void Undo() override;
    virtual void Redo() override;

    //Editing functions
    virtual void SetEditable(bool enable = true) override;
    virtual bool IsEditable() const override;

    virtual void EnableContextMenu(bool enable = true) override;
    virtual bool IsContextMenuEnabled() const override;

    virtual void EnableAccessToDevTools(bool enable = true) override;
    virtual bool IsAccessToDevToolsEnabled() const override;

    virtual void EnableBrowserAcceleratorKeys(bool enable = true) override;
    virtual bool AreBrowserAcceleratorKeysEnabled() const override;

    virtual bool SetUserAgent(const wxString& userAgent) override;
    virtual wxString GetUserAgent() const override;

    virtual bool SetProxy(const wxString& proxy) override;

    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const override;
    virtual void RunScriptAsync(const wxString& javascript, void* clientData = nullptr) const override;
    virtual bool AddScriptMessageHandler(const wxString& name) override;
    virtual bool RemoveScriptMessageHandler(const wxString& name) override;
    virtual bool AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime = wxWEBVIEW_INJECT_AT_DOCUMENT_START) override;
    virtual void RemoveAllUserScripts() override;

    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;

    virtual void* GetNativeBackend() const override;

    static void MSWSetBrowserExecutableDir(const wxString& path);

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

private:
    wxWebViewEdgeImpl* m_impl;

    void OnSize(wxSizeEvent& event);

    void OnSetFocus(wxFocusEvent& event);

    void OnTopLevelParentIconized(wxIconizeEvent& event);

    wxDECLARE_DYNAMIC_CLASS(wxWebViewEdge);

    friend class wxWebViewEdgeImpl;
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryEdge : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return new wxWebViewEdge; }
    virtual wxWebView* CreateWithConfig(const wxWebViewConfiguration& config) override;
    virtual wxWebView* Create(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr) override
    {
        return new wxWebViewEdge(parent, id, url, pos, size, style, name);
    }
    virtual bool IsAvailable() override;
    virtual wxVersionInfo GetVersionInfo() override;
    virtual wxWebViewConfiguration CreateConfiguration() override;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE && defined(__WXMSW__)

#endif // wxWebViewEdge_H
