/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 -2015 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEWCHROMIUM_H_
#define _WX_WEBVIEWCHROMIUM_H_

#include "wx/defs.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#include "wx/webview.h"
#include "wx/timer.h"

extern WXDLLIMPEXP_DATA_WEBVIEW_CHROMIUM(const char) wxWebViewBackendChromium[];

class wxWebViewChromium;
class ClientHandler;

class WXDLLIMPEXP_WEBVIEW_CHROMIUM wxWebViewChromium : public wxWebView
{
public:
    wxWebViewChromium() {}

    wxWebViewChromium(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxWebViewNameStr)
   {
       Create(parent, id, url, pos, size, style, name);
   }

    ~wxWebViewChromium();

    void OnSize(wxSizeEvent &event);

    void SetPageSource(const wxString& pageSource);

    void SetPageText(const wxString& pageText);

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

    virtual wxString GetPageSource() const override;
    virtual wxString GetPageText() const override;

    virtual bool IsBusy() const override;
    virtual wxString GetCurrentURL() const override;
    virtual wxString GetCurrentTitle() const override;

    virtual void SetZoomType(wxWebViewZoomType type) override;
    virtual wxWebViewZoomType GetZoomType() const override;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const override;

    virtual void Print() override;

    virtual wxWebViewZoom GetZoom() const override;
    virtual void SetZoom(wxWebViewZoom zoom) override;

    virtual void* GetNativeBackend() const override;

    virtual long Find(const wxString& WXUNUSED(text), int WXUNUSED(flags) = wxWEBVIEW_FIND_DEFAULT) override { return wxNOT_FOUND; }

    //Clipboard functions
    virtual bool CanCut() const override { return true; }
    virtual bool CanCopy() const override { return true; }
    virtual bool CanPaste() const override { return true; }
    virtual void Cut() override;
    virtual void Copy() override;
    virtual void Paste() override;

    //Undo / redo functionality
    virtual bool CanUndo() const override { return true; }
    virtual bool CanRedo() const override { return true; }
    virtual void Undo() override;
    virtual void Redo() override;

    //Editing functions
    virtual void SetEditable(bool enable = true) override;
    virtual bool IsEditable() const override { return false; }

    //Selection
    virtual void SelectAll() override;
    virtual bool HasSelection() const override { return false; }
    virtual void DeleteSelection() override;
    virtual wxString GetSelectedText() const override { return ""; }
    virtual wxString GetSelectedSource() const override { return ""; }
    virtual void ClearSelection() override;

    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const override;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

private:
    //History related variables, we currently use our own implementation
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    //We need to store the title and zoom ourselves
    wxString m_title;
    wxWebViewZoom m_zoomLevel;

    // Current main frame page source
    wxString m_pageSource;

    // The text of the current page
    wxString m_pageText;

    //We also friend ClientHandler so it can access the history
    friend class ClientHandler;
    ClientHandler* m_clientHandler;

    friend class wxWebViewChromiumModule;
    static int ms_activeWebViewCount;
    static bool ms_cefInitialized;

    static void OnIdle(wxIdleEvent& evt);

    static bool InitCEF();

    static void ShutdownCEF();

    wxDECLARE_DYNAMIC_CLASS(wxWebViewChromium);
};

class WXDLLIMPEXP_WEBVIEW_CHROMIUM wxWebViewFactoryChromium : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() { return new wxWebViewChromium; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxWebViewNameStr)
    { return new wxWebViewChromium(parent, id, url, pos, size, style, name); }
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#endif // _WX_WEBVIEWCHROMIUM_H_
