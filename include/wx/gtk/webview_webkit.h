/////////////////////////////////////////////////////////////////////////////
// Name:        include/gtk/wx/webview.h
// Purpose:     GTK webkit backend for web view component
// Author:      Robert Roebling, Marianne Gagnon
// Copyright:   (c) 2010 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WEBKITCTRL_H_
#define _WX_GTK_WEBKITCTRL_H_

#include "wx/defs.h"

// NOTE: this header is used for both the WebKit1 and WebKit2 implementations
#if wxUSE_WEBVIEW && (wxUSE_WEBVIEW_WEBKIT || wxUSE_WEBVIEW_WEBKIT2) && defined(__WXGTK__)

#include "wx/sharedptr.h"
#include "wx/webview.h"
#if wxUSE_WEBVIEW_WEBKIT2
#include <glib.h>
#include <gio/gio.h>
#endif

typedef struct _WebKitWebView WebKitWebView;

//-----------------------------------------------------------------------------
// wxWebViewWebKit
//-----------------------------------------------------------------------------

class wxWebKitRunScriptParams;

class WXDLLIMPEXP_WEBVIEW wxWebViewWebKit : public wxWebView
{
public:
    wxWebViewWebKit();

#if wxUSE_WEBVIEW_WEBKIT2
    wxWebViewWebKit(WebKitWebView* parentWebView, wxWebViewWebKit* parentWebViewCtrl);
    wxWebViewWebKit(const wxWebViewConfiguration& config);
#endif

    virtual bool Create(wxWindow *parent,
           wxWindowID id = wxID_ANY,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxASCII_STR(wxWebViewNameStr)) override;

    virtual ~wxWebViewWebKit();

    virtual bool Enable( bool enable = true ) override;

    // implementation
    // --------------

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual void Stop() override;
    virtual void LoadURL(const wxString& url) override;
    virtual void GoBack() override;
    virtual void GoForward() override;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) override;
    virtual bool CanGoBack() const override;
    virtual bool CanGoForward() const override;
    virtual void ClearHistory() override;
    virtual void EnableContextMenu(bool enable = true) override;
    virtual void EnableHistory(bool enable = true) override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() override;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) override;
    virtual wxString GetCurrentURL() const override;
    virtual wxString GetCurrentTitle() const override;
    virtual wxString GetPageSource() const override;
    virtual wxString GetPageText() const override;
    virtual void Print() override;
    virtual bool IsBusy() const override;
#if wxUSE_WEBVIEW_WEBKIT2
    virtual void EnableAccessToDevTools(bool enable = true) override;
    virtual bool IsAccessToDevToolsEnabled() const override;
    virtual bool SetUserAgent(const wxString& userAgent) override;
#endif

    void SetZoomType(wxWebViewZoomType) override;
    wxWebViewZoomType GetZoomType() const override;
    bool CanSetZoomType(wxWebViewZoomType) const override;
    virtual float GetZoomFactor() const override;
    virtual void SetZoomFactor(float) override;

    //Clipboard functions
    virtual bool CanCut() const override;
    virtual bool CanCopy() const override;
    virtual bool CanPaste() const override;
    virtual void Cut() override;
    virtual void Copy() override;
    virtual void Paste() override;

    //Undo / redo functionality
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;
    virtual void Undo() override;
    virtual void Redo() override;

    //Find function
    virtual long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT) override;

    //Editing functions
    virtual void SetEditable(bool enable = true) override;
    virtual bool IsEditable() const override;

    //Selection
    virtual void DeleteSelection() override;
    virtual bool HasSelection() const override;
    virtual void SelectAll() override;
    virtual wxString GetSelectedText() const override;
    virtual wxString GetSelectedSource() const override;
    virtual void ClearSelection() override;

#if wxUSE_WEBVIEW_WEBKIT2
    virtual void RunScriptAsync(const wxString& javascript, void* clientData = nullptr) const override;
    virtual bool AddScriptMessageHandler(const wxString& name) override;
    virtual bool RemoveScriptMessageHandler(const wxString& name) override;
    virtual bool AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime = wxWEBVIEW_INJECT_AT_DOCUMENT_START) override;
    virtual void RemoveAllUserScripts() override;
#else
    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const override;
#endif

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;
    virtual wxVector<wxSharedPtr<wxWebViewHandler> > GetHandlers() { return m_handlerList; }

    virtual void* GetNativeBackend() const override { return m_web_view; }

    /** TODO: check if this can be made private
     * The native control has a getter to check for busy state, but except in
     * very recent versions of webkit this getter doesn't say everything we need
     * (namely it seems to stay indefinitely busy when loading is cancelled by
     * user)
     */
    bool m_busy;

    wxString m_vfsurl;

    //We use this flag to stop recursion when we load a page from the navigation
    //callback, mainly when loading a VFS page
    bool m_guard;
    //This flag is use to indicate when a navigation event is the result of a
    //create-web-view signal and so we need to send a new window event
    bool m_creating;

#if wxUSE_WEBVIEW_WEBKIT2
    // This method needs to be public to make it callable from a callback
    void ProcessJavaScriptResult(GAsyncResult *res, wxWebKitRunScriptParams* params) const;
#endif

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

private:

    void ZoomIn();
    void ZoomOut();
    void SetWebkitZoom(float level);
    float GetWebkitZoom() const;

    //Find helper function
    void FindClear();

    // focus event handler: calls GTKUpdateBitmap()
    void GTKOnFocus(wxFocusEvent& event);

#if wxUSE_WEBVIEW_WEBKIT2
    bool CanExecuteEditingCommand(const gchar* command) const;
    void SetupWebExtensionServer();
    GDBusProxy *GetExtensionProxy() const;
#endif

    WebKitWebView *m_web_view;
    wxString m_customUserAgent;
    int m_historyLimit;

    wxVector<wxSharedPtr<wxWebViewHandler> > m_handlerList;

    //variables used for Find()
    int m_findFlags;
    wxString m_findText;
    int m_findPosition;
    int m_findCount;

#if wxUSE_WEBVIEW_WEBKIT2
    //Used for webkit2 extension
    GDBusServer *m_dbusServer;
    GDBusProxy *m_extension;
    wxWebViewConfiguration m_config;
#endif

    wxDECLARE_DYNAMIC_CLASS(wxWebViewWebKit);
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryWebKit : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return new wxWebViewWebKit; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) override
    {
        std::unique_ptr<wxWebView> webView(new wxWebViewWebKit);
        if (webView->Create(parent, id, url, pos, size, style, name))
            return webView.release();
        else
            return nullptr;
    }

#if wxUSE_WEBVIEW_WEBKIT2
    virtual wxVersionInfo GetVersionInfo() override;
    virtual wxWebViewConfiguration CreateConfiguration() override;
    virtual wxWebView* CreateWithConfig(const wxWebViewConfiguration& config) override;
#endif
};


#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT && defined(__WXGTK__)

#endif
