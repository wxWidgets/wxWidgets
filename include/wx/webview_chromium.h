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

class WXDLLIMPEXP_FWD_BASE wxFileName;

extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendChromium[];

class CefClient;

// Private namespace containing classes used only in the implementation.
namespace wxCEF
{
class ClientHandler;
struct ImplData;
}

class WXDLLIMPEXP_WEBVIEW wxWebViewChromium : public wxWebView
{
public:
    wxWebViewChromium() { Init(); }

    explicit wxWebViewChromium(const wxWebViewConfiguration& config);

    wxWebViewChromium(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxWebViewNameStr)
   {
       Init();

       Create(parent, id, url, pos, size, style, name);
   }

    ~wxWebViewChromium();

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
    virtual bool SetProxy(const wxString& proxy) override;

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
    virtual float GetZoomFactor() const override;
    virtual void SetZoomFactor(float zoom) override;

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

    // Chromium-specific functions.
    void SetRoot(const wxFileName& rootDir);

#ifdef __WXGTK__
    virtual void GTKHandleRealized() override;
#endif

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

private:
    // Common part of all ctors.
    void Init();

    void OnSize(wxSizeEvent& event);

    // Actually create the browser: this can only be done once the window is
    // created in wxGTK.
    bool DoCreateBrowser(const wxString& url);

    //History related variables, we currently use our own implementation
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition = -1;
    bool m_historyLoadingFromList = false;
    bool m_historyEnabled = true;

    //We need to store the title and zoom ourselves
    wxString m_title;
    wxWebViewZoom m_zoomLevel = wxWEBVIEW_ZOOM_MEDIUM;

    // Current main frame page source
    wxString m_pageSource;

    // The text of the current page
    wxString m_pageText;

    // Private data used by wxCEFClientHandler.
    struct wxCEF::ImplData* m_implData = nullptr;

    // We also friend wxCEFClientHandler so it can access m_implData.
    friend class wxCEF::ClientHandler;
    wxCEF::ClientHandler* m_clientHandler = nullptr;

    // Actual client used by CEF: this can be either m_clientHandler itself or
    // a custom client provided by the application.
    CefClient* m_actualClient = nullptr;

    friend class wxWebViewChromiumModule;
    static bool ms_cefInitialized;

    static bool InitCEF(const wxWebViewConfiguration& config);

    static void ShutdownCEF();

    wxDECLARE_DYNAMIC_CLASS(wxWebViewChromium);
};


// Chrome-specific configuration class: a pointer to this object can be
// retrieved using wxWebViewConfiguration::GetNativeConfiguration() and used to
// configure some CEF-specific initialization options.
//
// Note that, contrary to the name of the accessor, this class is not native in
// this backend, although it contains data found in native (i.e. defined in
// Chrome) structs.
class wxWebViewConfigurationChromium
{
public:
    wxWebViewConfigurationChromium() = default;

    // Log file location: by default debug.log under data path is used.
    wxString m_logFile;

    // Logging level must be one of cef_log_severity_t values (0 means default).
    int m_logLevel = 0;

    // If non-zero, specifies the port to use for remote debugging (the usual
    // value for it is 9223).
    int m_remoteDebuggingPort = 0;

    // Function to create the custom CefClient to use if non-null.
    //
    // The CefClient subclass must delegate all not otherwise implemented
    // functions to the provided client (and should always delegate the
    // lifetime-related callbacks).
    //
    // It is recommended, although not required, to derive the custom client
    // from wxDelegatingCefClient defined in wx/webview_chromium_impl.h.
    CefClient* (*m_clientCreate)(CefClient* client, void* data) = nullptr;

    // Data to pass to m_clientCreate if it is used.
    void* m_clientCreateData = nullptr;
};


class CefFrame;
class CefProcessMessage;

// Chrome-specific event sent when a message from renderer process is received.
//
// It provides the message received from another process and a frame that can
// be used to send the reply back.
class WXDLLIMPEXP_WEBVIEW wxWebViewChromiumEvent : public wxCommandEvent
{
public:
    wxWebViewChromiumEvent() = default;

    wxWebViewChromiumEvent(
        wxWebViewChromium& webview,
        CefFrame& frame,
        CefProcessMessage& message
    );

    CefFrame& GetFrame() const { return *m_frame; }
    CefProcessMessage& GetMessage() const { return *m_message; }

    wxString GetMessageName() const;

private:
    CefFrame* const m_frame = nullptr;
    CefProcessMessage* const m_message = nullptr;

    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN_DEF_COPY(wxWebViewChromiumEvent);
};

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_WEBVIEW,
                          wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED,
                          wxWebViewChromiumEvent );

typedef void (wxEvtHandler::*wxWebViewChromiumEventFunction)
             (wxWebViewChromiumEvent&);

#define wxWebViewChromiumEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxWebViewChromiumEventFunction, func)

#define EVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED, id, \
                     wxWebViewChromiumEventHandler(fn))

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#endif // _WX_WEBVIEWCHROMIUM_H_
