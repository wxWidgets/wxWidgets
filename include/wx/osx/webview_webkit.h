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

// ----------------------------------------------------------------------------
// Web Kit Control
// ----------------------------------------------------------------------------

WX_DECLARE_STRING_HASH_MAP(wxSharedPtr<wxWebViewHandler>, wxStringToWebHandlerMap);

class WXDLLIMPEXP_WEBVIEW wxWebViewWebKit : public wxWebView
{
public:
    wxDECLARE_DYNAMIC_CLASS(wxWebViewWebKit);

    wxWebViewWebKit() {}
    wxWebViewWebKit(wxWindow *parent,
                    wxWindowID winID = wxID_ANY,
                    const wxString& strURL = wxASCII_STR(wxWebViewDefaultURLStr),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxString& name = wxASCII_STR(wxWebViewNameStr))
    {
        Create(parent, winID, strURL, pos, size, style, name);
    }
    bool Create(wxWindow *parent,
                wxWindowID winID = wxID_ANY,
                const wxString& strURL = wxASCII_STR(wxWebViewDefaultURLStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = wxASCII_STR(wxWebViewNameStr)) wxOVERRIDE;
    virtual ~wxWebViewWebKit();

    virtual bool CanGoBack() const wxOVERRIDE;
    virtual bool CanGoForward() const wxOVERRIDE;
    virtual void GoBack() wxOVERRIDE;
    virtual void GoForward() wxOVERRIDE;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) wxOVERRIDE;
    virtual void Stop() wxOVERRIDE;

    virtual void Print() wxOVERRIDE;

    virtual void LoadURL(const wxString& url) wxOVERRIDE;
    virtual wxString GetCurrentURL() const wxOVERRIDE;
    virtual wxString GetCurrentTitle() const wxOVERRIDE;
    virtual float GetZoomFactor() const wxOVERRIDE;
    virtual void SetZoomFactor(float zoom) wxOVERRIDE;

    virtual void SetZoomType(wxWebViewZoomType zoomType) wxOVERRIDE;
    virtual wxWebViewZoomType GetZoomType() const wxOVERRIDE;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const wxOVERRIDE;

    virtual bool IsBusy() const wxOVERRIDE;

    //History functions
    virtual void ClearHistory() wxOVERRIDE;
    virtual void EnableHistory(bool enable = true) wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() wxOVERRIDE;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) wxOVERRIDE;

    //Undo / redo functionality
    virtual bool CanUndo() const wxOVERRIDE;
    virtual bool CanRedo() const wxOVERRIDE;
    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;

    //Editing functions
    virtual void SetEditable(bool enable = true) wxOVERRIDE;
    virtual bool IsEditable() const wxOVERRIDE;

    bool RunScript(const wxString& javascript, wxString* output = NULL) const wxOVERRIDE;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) wxOVERRIDE;

    virtual void* GetNativeBackend() const wxOVERRIDE { return m_webView; }

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) wxOVERRIDE;

    wxDECLARE_EVENT_TABLE();

private:
    OSXWebViewPtr m_webView;
    wxStringToWebHandlerMap m_handlers;

    WX_NSObject m_navigationDelegate;
    WX_NSObject m_UIDelegate;

    bool RunScriptSync(const wxString& javascript, wxString* output = NULL) const;
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryWebKit : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() wxOVERRIDE { return new wxWebViewWebKit; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) wxOVERRIDE
    { return new wxWebViewWebKit(parent, id, url, pos, size, style, name); }
    virtual wxVersionInfo GetVersionInfo() wxOVERRIDE;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT

#endif // _WX_WEBKIT_H_
