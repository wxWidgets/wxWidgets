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
           const wxString& name = wxWebViewNameStr);

    virtual void LoadURL(const wxString& url);
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item);
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory();
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory();

    virtual bool CanGoForward() const;
    virtual bool CanGoBack() const;
    virtual void GoBack();
    virtual void GoForward();
    virtual void ClearHistory();
    virtual void EnableHistory(bool enable = true);
    virtual void Stop();
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT);

    virtual wxString GetPageSource() const;
    virtual wxString GetPageText() const;

    virtual bool IsBusy() const;
    virtual wxString GetCurrentURL() const;
    virtual wxString GetCurrentTitle() const;

    virtual void SetZoomType(wxWebViewZoomType type);
    virtual wxWebViewZoomType GetZoomType() const;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const;

    virtual void Print();

    virtual wxWebViewZoom GetZoom() const;
    virtual void SetZoom(wxWebViewZoom zoom);

    virtual void* GetNativeBackend() const;

    virtual long Find(const wxString& WXUNUSED(text), int WXUNUSED(flags) = wxWEBVIEW_FIND_DEFAULT) { return wxNOT_FOUND; }

    //Clipboard functions
    virtual bool CanCut() const { return true; }
    virtual bool CanCopy() const { return true; }
    virtual bool CanPaste() const { return true; }
    virtual void Cut();
    virtual void Copy();
    virtual void Paste();

    //Undo / redo functionality
    virtual bool CanUndo() const { return true; }
    virtual bool CanRedo() const { return true; }
    virtual void Undo();
    virtual void Redo();

    //Editing functions
    virtual void SetEditable(bool enable = true);
    virtual bool IsEditable() const { return false; }

    //Selection
    virtual void SelectAll();
    virtual bool HasSelection() const { return false; }
    virtual void DeleteSelection();
    virtual wxString GetSelectedText() const { return ""; }
    virtual wxString GetSelectedSource() const { return ""; }
    virtual void ClearSelection();

    virtual bool RunScript(const wxString& javascript, wxString* output = NULL) wxOVERRIDE;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler);

#ifdef __WXMSW__
    static bool StartUp(int &code, const wxString &path = "");
#else
    static bool StartUp(int &code, const wxString &path,
                        int argc, char* argv[]);
#endif

    static void Shutdown();
    static void DoCEFWork();

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl);

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
