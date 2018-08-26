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

    virtual wxString GetPageSource() const wxOVERRIDE;
    virtual wxString GetPageText() const wxOVERRIDE;

    virtual bool IsBusy() const wxOVERRIDE;
    virtual wxString GetCurrentURL() const wxOVERRIDE;
    virtual wxString GetCurrentTitle() const wxOVERRIDE;

    virtual void SetZoomType(wxWebViewZoomType type) wxOVERRIDE;
    virtual wxWebViewZoomType GetZoomType() const wxOVERRIDE;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const wxOVERRIDE;

    virtual void Print() wxOVERRIDE;

    virtual wxWebViewZoom GetZoom() const wxOVERRIDE;
    virtual void SetZoom(wxWebViewZoom zoom) wxOVERRIDE;

    virtual void* GetNativeBackend() const wxOVERRIDE;

    virtual long Find(const wxString& WXUNUSED(text), int WXUNUSED(flags) = wxWEBVIEW_FIND_DEFAULT) wxOVERRIDE { return wxNOT_FOUND; }

    //Clipboard functions
    virtual bool CanCut() const wxOVERRIDE { return true; }
    virtual bool CanCopy() const wxOVERRIDE { return true; }
    virtual bool CanPaste() const wxOVERRIDE { return true; }
    virtual void Cut() wxOVERRIDE;
    virtual void Copy() wxOVERRIDE;
    virtual void Paste() wxOVERRIDE;

    //Undo / redo functionality
    virtual bool CanUndo() const wxOVERRIDE { return true; }
    virtual bool CanRedo() const wxOVERRIDE { return true; }
    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;

    //Editing functions
    virtual void SetEditable(bool enable = true) wxOVERRIDE;
    virtual bool IsEditable() const wxOVERRIDE { return false; }

    //Selection
    virtual void SelectAll() wxOVERRIDE;
    virtual bool HasSelection() const wxOVERRIDE { return false; }
    virtual void DeleteSelection() wxOVERRIDE;
    virtual wxString GetSelectedText() const wxOVERRIDE { return ""; }
    virtual wxString GetSelectedSource() const wxOVERRIDE { return ""; }
    virtual void ClearSelection() wxOVERRIDE;

    virtual bool RunScript(const wxString& javascript, wxString* output = NULL) wxOVERRIDE;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) wxOVERRIDE;

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) wxOVERRIDE;

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
