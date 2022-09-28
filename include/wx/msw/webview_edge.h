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
        const wxString& name = wxWebViewNameStr) wxOVERRIDE;

    virtual void LoadURL(const wxString& url) wxOVERRIDE;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() wxOVERRIDE;

    virtual bool CanGoForward() const wxOVERRIDE;
    virtual bool CanGoBack() const wxOVERRIDE;
    virtual void GoBack() wxOVERRIDE;
    virtual void GoForward() wxOVERRIDE;
    virtual void ClearHistory() wxOVERRIDE;
    virtual void EnableHistory(bool enable = true) wxOVERRIDE;
    virtual void Stop() wxOVERRIDE;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) wxOVERRIDE;

    virtual bool IsBusy() const wxOVERRIDE;
    virtual wxString GetCurrentURL() const wxOVERRIDE;
    virtual wxString GetCurrentTitle() const wxOVERRIDE;

    virtual void SetZoomType(wxWebViewZoomType) wxOVERRIDE;
    virtual wxWebViewZoomType GetZoomType() const wxOVERRIDE;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const wxOVERRIDE;

    virtual void Print() wxOVERRIDE;

    virtual float GetZoomFactor() const wxOVERRIDE;
    virtual void SetZoomFactor(float zoom) wxOVERRIDE;

    //Undo / redo functionality
    virtual bool CanUndo() const wxOVERRIDE;
    virtual bool CanRedo() const wxOVERRIDE;
    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;

    //Editing functions
    virtual void SetEditable(bool enable = true) wxOVERRIDE;
    virtual bool IsEditable() const wxOVERRIDE;

    virtual void EnableContextMenu(bool enable = true) wxOVERRIDE;
    virtual bool IsContextMenuEnabled() const wxOVERRIDE;

    virtual void EnableAccessToDevTools(bool enable = true) wxOVERRIDE;
    virtual bool IsAccessToDevToolsEnabled() const wxOVERRIDE;

    virtual bool SetUserAgent(const wxString& userAgent) wxOVERRIDE;

    virtual bool RunScript(const wxString& javascript, wxString* output = NULL) const wxOVERRIDE;
    virtual void RunScriptAsync(const wxString& javascript, void* clientData = NULL) const wxOVERRIDE;
    virtual bool AddScriptMessageHandler(const wxString& name) wxOVERRIDE;
    virtual bool RemoveScriptMessageHandler(const wxString& name) wxOVERRIDE;
    virtual bool AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime = wxWEBVIEW_INJECT_AT_DOCUMENT_START) wxOVERRIDE;
    virtual void RemoveAllUserScripts() wxOVERRIDE;

    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) wxOVERRIDE;

    virtual void* GetNativeBackend() const wxOVERRIDE;
    virtual void* GetNativeConfiguration() const wxOVERRIDE;

    static void MSWSetBrowserExecutableDir(const wxString& path);

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) wxOVERRIDE;

private:
    wxWebViewEdgeImpl* m_impl;

    void OnSize(wxSizeEvent& event);

    void OnSetFocus(wxFocusEvent& event);

    void OnTopLevelParentIconized(wxIconizeEvent& event);

    wxDECLARE_DYNAMIC_CLASS(wxWebViewEdge);
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryEdge : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() wxOVERRIDE { return new wxWebViewEdge; }
    virtual wxWebView* Create(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr) wxOVERRIDE
    {
        return new wxWebViewEdge(parent, id, url, pos, size, style, name);
    }
    virtual bool IsAvailable() wxOVERRIDE;
    virtual wxVersionInfo GetVersionInfo() wxOVERRIDE;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE && defined(__WXMSW__)

#endif // wxWebViewEdge_H
