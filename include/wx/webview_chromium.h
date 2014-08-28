/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEWCHROMIUM_H_
#define _WX_WEBVIEWCHROMIUM_H_

#include "wx/defs.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#include "wx/control.h"
#include "wx/webview.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_scheme.h"

#ifdef __VISUALC__
#pragma warning(pop)
#endif

extern WXDLLIMPEXP_DATA_WEBVIEW_CHROMIUM(const char) wxWebViewBackendChromium[];

class wxWebViewChromium;

// ClientHandler implementation.
class ClientHandler : public CefClient,
                      public CefContextMenuHandler,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler
{
public:
    ClientHandler() : m_loadErrorCode(-1) {}
    virtual ~ClientHandler() {}

    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() { return this; }

    // CefDisplayHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                      bool isLoading,  bool canGoBack,
                                      bool canGoForward);
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 const CefString& url);
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title);
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                  const CefString& message,
                                  const CefString& source,
                                  int line);

    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model);
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      int command_id,
                                      CefContextMenuHandler::EventFlags event_flags);
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame);

    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& target_url,
                               const CefString& target_frame_name,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings,
                               bool* no_javascript_access);
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);
    virtual bool DoClose(CefRefPtr<CefBrowser> browser);
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser);

    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame);
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           int httpStatusCode);
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl);

    CefRefPtr<CefBrowser> GetBrowser() { return m_browser; }

    void SetWebView(wxWebViewChromium *webview) { m_webview = webview; }

private:
    CefRefPtr<CefBrowser> m_browser;
    wxWebViewChromium *m_webview;
    int m_browserId;
    // Record the load error code: enum wxWebViewNavigationError
    // -1 means no error.
    int m_loadErrorCode;
    IMPLEMENT_REFCOUNTING(ClientHandler);
};

class SchemeHandler : public CefResourceHandler
{
public:
    SchemeHandler(const wxSharedPtr<wxWebViewHandler>& handler) : m_handler(handler), m_offset(0) {}

    // CefResourceHandler methods
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                                CefRefPtr<CefCallback> callback);
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                    int64& response_length,
                                    CefString& redirectUrl);
    virtual bool ReadResponse(void* data_out,
                              int bytes_to_read,
                              int& bytes_read,
                              CefRefPtr<CefCallback> callback);
    virtual void Cancel() {}

private:
    wxSharedPtr<wxWebViewHandler> m_handler;
    std::string m_data;
    std::string m_mime_type;
    size_t m_offset;

    IMPLEMENT_REFCOUNTING(SchemeHandler);
    IMPLEMENT_LOCKING(SchemeHandler);
};

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
    SchemeHandlerFactory(wxSharedPtr<wxWebViewHandler> handler): m_handler(handler) {}

    // Return a new scheme handler instance to handle the request.
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                                 CefRefPtr<CefFrame> WXUNUSED(frame),
                                                 const CefString& WXUNUSED(scheme_name),
                                                 CefRefPtr<CefRequest> WXUNUSED(request))
    {
        return new SchemeHandler( m_handler );
    }

    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
private:
    wxSharedPtr<wxWebViewHandler> m_handler;
};

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

    virtual void RunScript(const wxString& javascript);

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
    CefRefPtr<ClientHandler> m_clientHandler;

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
