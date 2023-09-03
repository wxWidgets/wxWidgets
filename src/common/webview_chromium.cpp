/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 - 2015 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#include "wx/webview.h"
#include "wx/webview_chromium.h"
#include "wx/filename.h"
#include "wx/filesys.h"
#include "wx/rtti.h"
#include "wx/stdpaths.h"
#include "wx/app.h"
#include "wx/base64.h"
#include "wx/module.h"

#include "wx/private/init.h"
#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#endif

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

wxGCC_WARNING_SUPPRESS(unused-parameter)

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_scheme.h"
#include "include/cef_string_visitor.h"
#include "include/cef_version.h"
#include "include/base/cef_lock.h"

wxGCC_WARNING_RESTORE(unused-parameter)

#ifdef __VISUALC__
#pragma warning(pop)
#endif

#if CHROME_VERSION_BUILD < 5845
#error "Unsupported CEF version"
#endif

namespace
{

constexpr const char* TRACE_CEF = "cef";

#define TRACE_CEF_FUNCTION() wxLogTrace(TRACE_CEF, "%s called", __FUNCTION__)

} // anonymous namespace

extern WXDLLIMPEXP_DATA_WEBVIEW_CHROMIUM(const char) wxWebViewBackendChromium[] = "wxWebViewChromium";

bool wxWebViewChromium::ms_cefInitialized = false;

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewChromium, wxWebView);

// ----------------------------------------------------------------------------
// wxWebViewChromiumImplData
// ----------------------------------------------------------------------------

struct wxWebViewChromiumImplData
{
#ifdef __WXGTK__
    // Due to delayed creation of the browser in wxGTK we need to remember the
    // URL passed to Create() as we can't use it there directly.
    wxString m_initialURL;
#endif // __WXGTK__
};

// ----------------------------------------------------------------------------
// ClientHandler
// ----------------------------------------------------------------------------

// ClientHandler implementation.
class ClientHandler : public CefClient,
    public CefContextMenuHandler,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefLoadHandler
{
public:
    // Ctor must be given a backpointer to wxWebView which must remain valid
    // for the entire lifetime of this object.
    explicit ClientHandler(wxWebViewChromium& webview) : m_webview{webview} {}

    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override  { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override  { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override  { return this; }

    // CefDisplayHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
        bool isLoading, bool canGoBack,
        bool canGoForward) override;
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title) override;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
        cef_log_severity_t level,
        const CefString& message,
        const CefString& source,
        int line) override;

    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        CefRefPtr<CefMenuModel> model) override;
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        int command_id,
        CefContextMenuHandler::EventFlags event_flags) override;
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame) override;

    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& target_url,
        const CefString& target_frame_name,
        WindowOpenDisposition target_disposition,
        bool user_gesture,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings,
        CefRefPtr<CefDictionaryValue>& extra_info,
        bool* no_javascript_access) override;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        TransitionType transition_type) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) override;

    CefRefPtr<CefBrowser> GetBrowser() const { return m_browser; }

    // Return the underlying window handle: HWND under MSW, X11 Window under
    // GTK.
    //
    // The handle can be 0.
    CefWindowHandle GetWindowHandle() const
    {
        auto browser = GetBrowser();
        if ( !browser )
            return 0;

        auto host = browser->GetHost();
        if ( !host )
            return 0;

        return host->GetWindowHandle();
    }

private:
    CefRefPtr<CefBrowser> m_browser;
    wxWebViewChromium& m_webview;
    int m_browserId;
    // Record the load error code: enum wxWebViewNavigationError
    // -1 means no error.
    int m_loadErrorCode = -1;
    IMPLEMENT_REFCOUNTING(ClientHandler);
};

class SchemeHandler : public CefResourceHandler
{
public:
    SchemeHandler(const wxSharedPtr<wxWebViewHandler>& handler) : m_handler(handler), m_offset(0) {}

    // CefResourceHandler methods
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override;
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64_t& response_length,
        CefString& redirectUrl) override;
    virtual bool ReadResponse(void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefCallback> callback) override;
    virtual void Cancel() override {}

private:
    wxSharedPtr<wxWebViewHandler> m_handler;
    std::string m_data;
    std::string m_mime_type;
    size_t m_offset;

    IMPLEMENT_REFCOUNTING(SchemeHandler);
    base::Lock m_lock;
};

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
    SchemeHandlerFactory(wxSharedPtr<wxWebViewHandler> handler) : m_handler(handler) {}

    // Return a new scheme handler instance to handle the request.
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> WXUNUSED(browser),
        CefRefPtr<CefFrame> WXUNUSED(frame),
        const CefString& WXUNUSED(scheme_name),
        CefRefPtr<CefRequest> WXUNUSED(request)) override
    {
        return new SchemeHandler(m_handler);
    }

    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
private:
    wxSharedPtr<wxWebViewHandler> m_handler;
};

class wxStringVisitor : public CefStringVisitor
{
public:
    enum StringType
    {
      PAGE_SOURCE,
      PAGE_TEXT,
    };
    wxStringVisitor(wxWebViewChromium& webview, StringType type) :
        m_type(type), m_webview(webview) {}
    void Visit(const CefString& string) override
    {
        switch(m_type)
        {
            case PAGE_SOURCE:
                 m_webview.SetPageSource(string.ToWString());
                 break;
            case PAGE_TEXT:
                 m_webview.SetPageText(string.ToWString());
                 break;
        }
    }
private:
    StringType m_type;
    wxWebViewChromium& m_webview;
    IMPLEMENT_REFCOUNTING(wxStringVisitor);
};

namespace
{

class wxBrowserProcessHandler : public CefBrowserProcessHandler
{
public:
    wxBrowserProcessHandler() = default;

    void OnScheduleMessagePumpWork(int64_t delay_ms) override
    {
        if ( delay_ms > 0 )
        {
            if ( !m_timer.IsRunning() )
            {
                wxLogTrace(TRACE_CEF, "schedule work in %lldms", delay_ms);
                m_timer.StartOnce(delay_ms);
            }
            else
            {
                wxLogTrace(TRACE_CEF, "work already scheduled");
            }
        }
        else
        {
            if ( wxTheApp )
                wxTheApp->CallAfter([]() { CefDoMessageLoopWork(); });
            else
                wxLogTrace(TRACE_CEF, "can't schedule message pump work");
        }
    }

private:
    class CEFTimer : public wxTimer
    {
    public:
        CEFTimer() = default;

        void Notify() override
        {
            CefDoMessageLoopWork();
        }
    } m_timer;

    IMPLEMENT_REFCOUNTING(wxBrowserProcessHandler);
};

class wxCefApp : public CefApp
{
public:
    wxCefApp()
        : m_browserProcessHandler(new wxBrowserProcessHandler{})
    {
    }

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return m_browserProcessHandler;
    }

private:
    CefRefPtr<CefBrowserProcessHandler> m_browserProcessHandler;

    IMPLEMENT_REFCOUNTING(wxCefApp);
};

} // anonymous namespace

bool wxWebViewChromium::Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
#ifdef __WXGTK__
    // Currently CEF works only with X11.
    if ( wxGetDisplayInfo().type != wxDisplayX11 )
        return false;

    style |= wxHSCROLL | wxVSCROLL;
#endif

    if ( !wxControl::Create(parent, id, pos, size, style,
                           wxDefaultValidator, name) )
    {
        return false;
    }
    if ( !InitCEF() )
        return false;

#ifdef __WXMSW__
    MSWDisableComposited();
#endif // __WXMSW__

    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;
    m_zoomLevel = wxWEBVIEW_ZOOM_MEDIUM;

    m_implData = new wxWebViewChromiumImplData{};

    m_clientHandler = new ClientHandler{*this};
    m_clientHandler->AddRef();

#ifdef __WXGTK__

#ifdef __WXGTK3__
    // CEF window creation fails with Match error unless we use the default X11
    // visual, which is not the case by default since GTK 3.15, see dae447728d
    // (X11: Pick better system and rgba visuals for GL, 2014-10-29).
    //
    // We do this unconditionally instead of checking for GTK version because
    // it shouldn't hurt even with earlier version and nobody uses them anyhow.
    GdkScreen* screen = gdk_screen_get_default();
    GdkX11Screen* x11_screen = GDK_X11_SCREEN(screen);
    Visual* default_xvisual = DefaultVisual(GDK_SCREEN_XDISPLAY(x11_screen),
                                            GDK_SCREEN_XNUMBER(x11_screen));
    GdkVisual* default_visual = nullptr;

    for ( GList* visuals = gdk_screen_list_visuals(screen);
          visuals;
          visuals = visuals->next)
    {
        GdkVisual* visual = GDK_X11_VISUAL (visuals->data);
        Visual* xvisual = gdk_x11_visual_get_xvisual(GDK_X11_VISUAL (visuals->data));

        if ( xvisual->visualid == default_xvisual->visualid )
        {
           default_visual = visual;
           break;
        }
    }

    if ( default_visual )
        gtk_widget_set_visual(m_widget, default_visual);
#endif // wxGTK3

    // Under wxGTK we need to wait until the window becomes realized in order
    // to get the X11 window handle, so postpone calling DoCreateBrowser()
    // until GTKHandleRealized().
    m_implData->m_initialURL = url;
#else
    // Under the other platforms we can call it immediately.
    if ( !DoCreateBrowser(url) )
        return false;
#endif

    this->Bind(wxEVT_SIZE, &wxWebViewChromium::OnSize, this);

    Bind(wxEVT_IDLE, [](wxIdleEvent&) { CefDoMessageLoopWork(); });

    return true;
}

#ifdef __WXGTK__

void wxWebViewChromium::GTKHandleRealized()
{
    // Unfortunately there is nothing we can do here if it fails, so just
    // ignore the return value.
    DoCreateBrowser(m_implData->m_initialURL);
}

#endif // __WXGTK__

bool wxWebViewChromium::DoCreateBrowser(const wxString& url)
{
    CefBrowserSettings browsersettings;

    // Initialize window info to the defaults for a child window
    CefWindowInfo info;

    // In wxGTK the handle returned by GetHandle() is the GtkWidget, but we
    // need the underlying X11 window here.
#ifdef __WXGTK__
    const auto handle = GDK_WINDOW_XID(GTKGetDrawingWindow());
#else
    const auto handle = GetHandle();
#endif

    const wxSize sz = GetClientSize();
    info.SetAsChild(handle, {0, 0, sz.x, sz.y});

    if ( !CefBrowserHost::CreateBrowser(
            info,
            CefRefPtr<CefClient>{m_clientHandler},
            url.ToStdString(),
            browsersettings,
            nullptr, // No extra info
            nullptr  // Use global request context
        ) )
    {
        wxLogTrace(TRACE_CEF, "CefBrowserHost::CreateBrowser() failed");
        return false;
    }

    return true;
}

wxWebViewChromium::~wxWebViewChromium()
{
    if (m_clientHandler)
    {
        wxLogTrace(TRACE_CEF, "closing browser");

        // Preserve the original pointer.
        const auto clientHandler = m_clientHandler;

        constexpr bool forceClose = true;
        m_clientHandler->GetBrowser()->GetHost()->CloseBrowser(forceClose);

        // This should be set to nullptr from ClientHandler::DoClose().
        while ( m_clientHandler )
            CefDoMessageLoopWork();

        clientHandler->Release();
    }
}

bool wxWebViewChromium::InitCEF()
{
    if (ms_cefInitialized)
        return true;

    wxFileName cefDataFolder(wxStandardPaths::Get().GetUserLocalDataDir(), "");
    cefDataFolder.AppendDir("CEF");
    cefDataFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    CefSettings settings;

    wxFileName cachePath(cefDataFolder.GetFullPath(), "Cache");
    CefString(&settings.cache_path).FromWString(cachePath.GetFullPath().ToStdWstring());

    // According to b5386249b (alloy: Remove CefSettings.user_data_path (fixes
    // #3511), 2023-06-06) in CEF sources, root_cache_path should be used for
    // all files now.
    wxFileName userDataPath(cefDataFolder.GetFullPath(), "UserData");
    CefString(&settings.root_cache_path).FromWString(userDataPath.GetFullPath().ToStdWstring());

    // Set up CEF for use inside another application, as is the case for us.
    settings.multi_threaded_message_loop = false;
    settings.external_message_pump = true;

    settings.no_sandbox = true;

#ifdef __WXDEBUG__
    wxFileName logFileName(cefDataFolder.GetFullPath(), "debug.log");
    settings.log_severity = LOGSEVERITY_INFO;
    CefString(&settings.log_file).FromWString(logFileName.GetFullPath().ToStdWstring());
#endif

#ifdef __WXMSW__
    CefMainArgs args(wxGetInstance());
#else
    wxAppConsole* app = wxApp::GetInstance();
    CefMainArgs args(app->argc, app->argv);
#endif

    CefRefPtr<CefApp> cefApp{new wxCefApp{}};
    if (CefInitialize(args, settings, cefApp, nullptr))
    {
        ms_cefInitialized = true;
        return true;
    }
    else
    {
        wxLogError("Could not initialize CEF");
        return false;
    }
}

void wxWebViewChromium::ShutdownCEF()
{
    if (ms_cefInitialized)
    {
        CefShutdown();
    }
}

void wxWebViewChromium::OnSize(wxSizeEvent& event)
{
    event.Skip();

    const auto handle = m_clientHandler ? m_clientHandler->GetWindowHandle() : 0;
    if ( !handle )
        return;

    wxSize size = GetClientSize();

#ifdef __WXMSW__
    HDWP hdwp = BeginDeferWindowPos(1);
    hdwp = DeferWindowPos(hdwp, handle, nullptr, 0, 0,
                          size.GetWidth(), size.GetHeight(), SWP_NOZORDER);
    EndDeferWindowPos(hdwp);
#elif defined(__WXGTK__)
    size *= GetDPIScaleFactor();

    ::XResizeWindow(wxGetX11Display(), handle, size.x, size.y);
#endif
}

void wxWebViewChromium::SetPageSource(const wxString& pageSource)
{
    m_pageSource = pageSource;
}

void wxWebViewChromium::SetPageText(const wxString& pageText)
{
    m_pageText = pageText;
}

void* wxWebViewChromium::GetNativeBackend() const
{
    return m_clientHandler->GetBrowser().get();
}

bool wxWebViewChromium::CanGoForward() const
{
    if ( m_historyEnabled )
        return m_historyPosition != static_cast<int>(m_historyList.size()) - 1;
    else
        return false;
}

bool wxWebViewChromium::CanGoBack() const
{
    if ( m_historyEnabled )
        return m_historyPosition > 0;
    else
        return false;
}

void wxWebViewChromium::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    int pos = -1;
    for ( unsigned int i = 0; i < m_historyList.size(); i++ )
    {
        //We compare the actual pointers to find the correct item
        if(m_historyList[i].get() == item.get())
            pos = i;
    }
    wxASSERT_MSG(pos != static_cast<int>(m_historyList.size()),
                 "invalid history item");
    m_historyLoadingFromList = true;
    LoadURL(item->GetUrl());
    m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewChromium::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for ( int i = 0; i < m_historyPosition; i++ )
    {
        backhist.push_back(m_historyList[i]);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewChromium::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for ( int i = m_historyPosition + 1; i < static_cast<int>(m_historyList.size()); i++ )
    {
        forwardhist.push_back(m_historyList[i]);
    }
    return forwardhist;
}

void wxWebViewChromium::GoBack()
{
    LoadHistoryItem(m_historyList[m_historyPosition - 1]);
}

void wxWebViewChromium::GoForward()
{
    LoadHistoryItem(m_historyList[m_historyPosition + 1]);
}

void wxWebViewChromium::LoadURL(const wxString& url)
{
    m_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(url.ToStdString());
}

void wxWebViewChromium::ClearHistory()
{
    m_historyList.clear();
    m_historyPosition = -1;
}

void wxWebViewChromium::EnableHistory(bool enable)
{
    m_historyEnabled = enable;
}

void wxWebViewChromium::Stop()
{
    m_clientHandler->GetBrowser()->StopLoad();
}

void wxWebViewChromium::Reload(wxWebViewReloadFlags flags)
{
    if ( flags == wxWEBVIEW_RELOAD_NO_CACHE )
    {
        m_clientHandler->GetBrowser()->ReloadIgnoreCache();
    }
    else
    {
        m_clientHandler->GetBrowser()->Reload();
    }
}

wxString wxWebViewChromium::GetPageSource() const
{
    return m_pageSource;
}

wxString wxWebViewChromium::GetPageText() const
{
    return m_pageText;
}

wxString wxWebViewChromium::GetCurrentURL() const
{
    return m_clientHandler->GetBrowser()->GetMainFrame()->GetURL().ToString();
}

wxString wxWebViewChromium::GetCurrentTitle() const
{
    return m_title;
}

void wxWebViewChromium::Print()
{
    m_clientHandler->GetBrowser()->GetHost()->Print();
}

void wxWebViewChromium::Cut()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Cut();
}

void wxWebViewChromium::Copy()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Copy();
}

void wxWebViewChromium::Paste()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Paste();
}

void wxWebViewChromium::Undo()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Undo();
}

void wxWebViewChromium::Redo()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Redo();
}

void wxWebViewChromium::SelectAll()
{
   m_clientHandler->GetBrowser()->GetMainFrame()->SelectAll();
}

void wxWebViewChromium::DeleteSelection()
{
    wxString jsdelete = "if (window.getSelection) { if (window.getSelection().deleteFromDocument) { window.getSelection().deleteFromDocument(); } }";
    RunScript(jsdelete);
}

void wxWebViewChromium::ClearSelection()
{
    wxString jsclear = "if (window.getSelection) { if (window.getSelection().empty) { window.getSelection().empty(); } }";
    RunScript(jsclear);
}

bool wxWebViewChromium::RunScript(const wxString& javascript, wxString* output) const
{
    m_clientHandler->GetBrowser()->GetMainFrame()->ExecuteJavaScript(javascript.ToStdString(),
                                                                     "", 0);
    // Returning a result is currently unsupported
    if (output)
        return false;
    else
        return true;
}

bool wxWebViewChromium::IsBusy() const
{
    if(m_clientHandler->GetBrowser())
        return m_clientHandler->GetBrowser()->IsLoading();
    else
        return false;
}

void wxWebViewChromium::SetEditable(bool enable)
{
    wxString mode = enable ? "\"on\"" : "\"off\"";
    RunScript("document.designMode = " + mode);
}

void wxWebViewChromium::DoSetPage(const wxString& html, const wxString& baseUrl)
{
    wxUnusedVar(baseUrl);

    // This seems to be the only way to load a string in CEF now, see
    // https://github.com/chromiumembedded/cef/issues/2586
    const wxScopedCharBuffer& buf = html.utf8_str();
    const wxString url{
        "data:text/html;base64," + wxBase64Encode(buf.data(), buf.length())
    };

    m_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(url.ToStdWstring());
}

wxWebViewZoom wxWebViewChromium::GetZoom() const
{
     return m_zoomLevel;
}

float wxWebViewChromium::GetZoomFactor() const
{
    return m_clientHandler->GetBrowser()->GetHost()->GetZoomLevel();
}

void wxWebViewChromium::SetZoomFactor(float mapzoom)
{
    m_clientHandler->GetBrowser()->GetHost()->SetZoomLevel(mapzoom);
}

void wxWebViewChromium::SetZoom(wxWebViewZoom zoom)
{
    m_zoomLevel = zoom;

    double mapzoom = 0.0;
    // arbitrary way to map our common zoom enum to float zoom
    switch ( zoom )
    {
        case wxWEBVIEW_ZOOM_TINY:
            mapzoom = -1.0;
            break;

        case wxWEBVIEW_ZOOM_SMALL:
            mapzoom = -0.5;
            break;

        case wxWEBVIEW_ZOOM_MEDIUM:
            mapzoom = 0.0;
            break;

        case wxWEBVIEW_ZOOM_LARGE:
            mapzoom = 0.5;
            break;

        case wxWEBVIEW_ZOOM_LARGEST:
            mapzoom = 1.0;
            break;
    }

    SetZoomFactor(mapzoom);
}

void wxWebViewChromium::SetZoomType(wxWebViewZoomType type)
{
    // there is only one supported zoom type at the moment so this setter
    // does nothing beyond checking sanity
    wxASSERT(type == wxWEBVIEW_ZOOM_TYPE_LAYOUT);
}

wxWebViewZoomType wxWebViewChromium::GetZoomType() const
{
    return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewChromium::CanSetZoomType(wxWebViewZoomType type) const
{
    return type == wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

void wxWebViewChromium::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    CefRegisterSchemeHandlerFactory( handler->GetName().ToStdWstring(), "",
                                     new SchemeHandlerFactory(handler) );
}

// CefDisplayHandler methods
void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                         bool WXUNUSED(isLoading),
                                         bool WXUNUSED(canGoBack),
                                         bool WXUNUSED(canGoForward))
{}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                    CefRefPtr<CefFrame> WXUNUSED(frame),
                                    const CefString& WXUNUSED(url))
{}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    m_webview.m_title = title.ToWString();
    wxString target = browser->GetMainFrame()->GetName().ToString();

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_TITLE_CHANGED, m_webview.GetId(), "", target);
    event.SetString(title.ToWString());
    event.SetEventObject(&m_webview);

    m_webview.HandleWindowEvent(event);
}

bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                     cef_log_severity_t WXUNUSED(level),
                                     const CefString& WXUNUSED(message),
                                     const CefString& WXUNUSED(source), int WXUNUSED(line))
{
    return false;
}

// CefContextMenuHandler methods
void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                        CefRefPtr<CefFrame> WXUNUSED(frame),
                                        CefRefPtr<CefContextMenuParams> WXUNUSED(params),
                                        CefRefPtr<CefMenuModel> model)
{
    if(!m_webview.IsContextMenuEnabled())
        model->Clear();
}

bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                         CefRefPtr<CefFrame> WXUNUSED(frame),
                                         CefRefPtr<CefContextMenuParams> WXUNUSED(params),
                                         int WXUNUSED(command_id),
                                         CefContextMenuHandler::EventFlags WXUNUSED(event_flags))
{
    return false;
}

void ClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                           CefRefPtr<CefFrame> WXUNUSED(frame))
{}

// CefLifeSpanHandler methods
bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> WXUNUSED(browser),
                             CefRefPtr<CefFrame> WXUNUSED(frame),
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             WindowOpenDisposition WXUNUSED(target_disposition),
                             bool WXUNUSED(user_gesture),
                             const CefPopupFeatures& WXUNUSED(popupFeatures),
                             CefWindowInfo& WXUNUSED(windowInfo),
                             CefRefPtr<CefClient>& WXUNUSED(client),
                             CefBrowserSettings& WXUNUSED(settings),
                             CefRefPtr<CefDictionaryValue>& WXUNUSED(extra_info),
                             bool* WXUNUSED(no_javascript_access))
{
    wxWebViewEvent *event = new wxWebViewEvent(wxEVT_WEBVIEW_NEWWINDOW,
                                               m_webview.GetId(),
                                               target_url.ToString(),
                                               target_frame_name.ToString());
    event->SetEventObject(&m_webview);
    // We use queue event as this function is called on the render thread
    m_webview.GetEventHandler()->QueueEvent(event);

    return true;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    TRACE_CEF_FUNCTION();

    if ( !m_browser.get() )
    {
        m_browser = browser;
        m_browserId = browser->GetIdentifier();

        m_webview.PostSizeEvent();
    }
}
bool ClientHandler::DoClose(CefRefPtr<CefBrowser> WXUNUSED(browser))
{
    TRACE_CEF_FUNCTION();

    m_webview.m_clientHandler = nullptr;

    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    TRACE_CEF_FUNCTION();

    if ( browser->GetIdentifier() == m_browserId )
    {
        m_browser = nullptr;
    }
}

// CefLoadHandler methods
void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                CefRefPtr<CefFrame> frame,
                                TransitionType WXUNUSED(transition_type))
{
    wxString url = frame->GetURL().ToString();
    wxString target = frame->GetName().ToString();

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATING, m_webview.GetId(), url, target);
    event.SetEventObject(&m_webview);

    m_webview.HandleWindowEvent(event);

    if ( !event.IsAllowed() )
    {
        // We do not yet have support for vetoing pages
    }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> WXUNUSED(browser),
                              CefRefPtr<CefFrame> frame,
                              int WXUNUSED(httpStatusCode))
{
    wxString url = frame->GetURL().ToString();
    wxString target = frame->GetName().ToString();

    // Send webview_error event in case of loading error.
    if ( m_loadErrorCode != -1 )
    {
        m_loadErrorCode = -1;
        wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_ERROR, m_webview.GetId(), url, target);
        event.SetEventObject(&m_webview);
        m_webview.HandleWindowEvent(event);
    }

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATED, m_webview.GetId(), url, target);
    event.SetEventObject(&m_webview);
    m_webview.HandleWindowEvent(event);

    if ( frame->IsMain() )
    {
        //Get source code when main frame loads ended.
        CefRefPtr<CefStringVisitor> source_visitor = new wxStringVisitor(
            m_webview, wxStringVisitor::PAGE_SOURCE);
        frame->GetSource(source_visitor);

        //Get page text when main frame loads ended.
        CefRefPtr<CefStringVisitor> text_visitor = new wxStringVisitor(
            m_webview, wxStringVisitor::PAGE_TEXT);
        frame->GetText(text_visitor);

        //As we are complete we also add to the history list, but not if the
        //page is not the main page, ie it is a subframe
        if ( m_webview.m_historyEnabled && !m_webview.m_historyLoadingFromList )
        {
            //If we are not at the end of the list, then erase everything
            //between us and the end before adding the new page
            if ( m_webview.m_historyPosition != static_cast<int>(m_webview.m_historyList.size()) - 1 )
            {
                m_webview.m_historyList.erase(m_webview.m_historyList.begin() + m_webview.m_historyPosition + 1,
                                              m_webview.m_historyList.end());
            }
            wxSharedPtr<wxWebViewHistoryItem> item(new wxWebViewHistoryItem(url, m_webview.GetCurrentTitle()));
            m_webview.m_historyList.push_back(item);
            m_webview.m_historyPosition++;
        }
        //Reset as we are done now
        m_webview.m_historyLoadingFromList = false;

        wxWebViewEvent levent(wxEVT_COMMAND_WEBVIEW_LOADED, m_webview.GetId(), url, target);
        levent.SetEventObject(&m_webview);

        m_webview.HandleWindowEvent(levent);
    }
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                CefRefPtr<CefFrame> WXUNUSED(frame),
                                ErrorCode errorCode,
                                const CefString& WXUNUSED(errorText),
                                const CefString& WXUNUSED(failedUrl))
{
    //We define a macro for convenience
    #define ERROR_TYPE_CASE(error, wxtype) case(error): \
        m_loadErrorCode = wxtype;\
    break

    switch ( errorCode )
    {
        case ERR_NONE:
            m_loadErrorCode = -1;
            break;

        ERROR_TYPE_CASE(ERR_ABORTED, wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        ERROR_TYPE_CASE(ERR_FILE_NOT_FOUND, wxWEBVIEW_NAV_ERR_NOT_FOUND);
        ERROR_TYPE_CASE(ERR_TIMED_OUT, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_ACCESS_DENIED, wxWEBVIEW_NAV_ERR_AUTH);
        ERROR_TYPE_CASE(ERR_CONNECTION_CLOSED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_RESET, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_REFUSED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_ABORTED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_NAME_NOT_RESOLVED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_INTERNET_DISCONNECTED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_SSL_PROTOCOL_ERROR, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_ADDRESS_INVALID, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_ADDRESS_UNREACHABLE, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED, wxWEBVIEW_NAV_ERR_AUTH);
        ERROR_TYPE_CASE(ERR_TUNNEL_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_NO_SSL_VERSIONS_ENABLED, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_SSL_RENEGOTIATION_REQUESTED, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_CERT_COMMON_NAME_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_DATE_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_AUTHORITY_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_CONTAINS_ERRORS, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_NO_REVOCATION_MECHANISM, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_REVOKED, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_END, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_INVALID_URL, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_DISALLOWED_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_UNKNOWN_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_UNSAFE_REDIRECT, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_UNSAFE_PORT, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_INSECURE_RESPONSE, wxWEBVIEW_NAV_ERR_SECURITY);

        default:
            m_loadErrorCode = wxWEBVIEW_NAV_ERR_OTHER;
    }
}

bool SchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request,
                                   CefRefPtr<CefCallback> callback)
{
    bool handled = false;

    base::AutoLock lock_scope(m_lock);

    std::string url = request->GetURL();
    wxFSFile* file = m_handler->GetFile( url );

    if ( file )
    {
        m_mime_type = (file->GetMimeType()).ToStdString();

        size_t size = file->GetStream()->GetLength();
        char* buf = new char[size];
        file->GetStream()->Read( buf, size );
        m_data = std::string( buf, buf+size );

        delete[] buf;
        handled = true;
    }

    if ( handled )
    {
        // Indicate the headers are available.
        callback->Continue();
        return true;
    }
    return false;
}

void SchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                       int64_t& response_length,
                                       CefString& WXUNUSED(redirectUrl))
{
    if ( !m_mime_type.empty() )
        response->SetMimeType( m_mime_type );
    response->SetStatus( 200 );

    // Set the resulting response length
    response_length = m_data.length();
}

bool SchemeHandler::ReadResponse(void* data_out,
                                 int bytes_to_read,
                                 int& bytes_read,
                                 CefRefPtr<CefCallback> WXUNUSED(callback))
{
    bool has_data = false;
    bytes_read = 0;

    base::AutoLock lock_scope(m_lock);

    if ( m_offset < m_data.length() )
    {
        // Copy the next block of data into the buffer.
        int transfer_size =
            std::min( bytes_to_read, static_cast<int>( m_data.length() - m_offset ) );
        memcpy( data_out, m_data.c_str() + m_offset, transfer_size );
        m_offset += transfer_size;

        bytes_read = transfer_size;
        has_data = true;
    }

    return has_data;
}

#ifdef __WXOSX__
void wxWebViewChromium_InitOSX();
#endif

namespace
{

class wxWebViewChromiumEntry
{
public:
    wxWebViewChromiumEntry()
    {
        wxAddEntryHook(&wxWebViewChromiumEntry::Hook);
    }

private:
    static int Hook()
    {
        const auto& initData = wxInitData::Get();
        for ( int n = 0; n < initData.argc; ++n )
        {
            constexpr const wchar_t* TYPE_OPTION = L"--type=";
            if ( wcsncmp(initData.argv[n], TYPE_OPTION, wcslen(TYPE_OPTION)) == 0 )
            {
                // It looks like we have been launched by CEF as a helper
                // process, so execute it now.
#ifdef __WXMSW__
                // Under MSW CEF wants to have HINSTANCE, so give it to it.
                CefMainArgs args(wxGetInstance());
#else
                // Elsewhere it needs the actual arguments as narrow strings,
                // and, serendipitously, we always have them under non-MSW.
                CefMainArgs args(initData.argc, initData.argvA);
#endif

                // If there is no subprocess then we need to execute on this process
                int code = CefExecuteProcess(args, nullptr, nullptr);
                if (code < 0)
                {
                    // This wasn't a CEF helper process finally, somehow.
                    break;
                }

                // Exit immediately with the returned code.
                return code;
            }
        }

        // Continue normal execution.
        return -1;
    }
};

wxWebViewChromiumEntry gs_chromiumEntryHook;

} // anonymous namespace

class wxWebViewChromiumModule : public wxModule
{
public:
    wxWebViewChromiumModule() { }
    virtual bool OnInit() override
    {
        // Register with wxWebView
        wxWebView::RegisterFactory(wxWebViewBackendChromium,
            wxSharedPtr<wxWebViewFactory>(new wxWebViewFactoryChromium));

#ifdef __WXOSX__
        wxWebViewChromium_InitOSX();
#endif
        return true;
    }
    virtual void OnExit() override
    {
        wxWebViewChromium::ShutdownCEF();
    }
private:
    wxDECLARE_DYNAMIC_CLASS(wxWebViewChromiumModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewChromiumModule, wxModule);

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM
