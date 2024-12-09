/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 - 2015 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM

#include "wx/webview.h"
#include "wx/webview_chromium.h"
#include "wx/eventfilter.h"
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
#include "wx/private/webview.h"

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#endif

// With MSVC we are always using debug CEF build in debug wx builds (i.e. when
// _DEBUG is defined), as otherwise we'd be using incompatible variants of CRT.
#ifdef _DEBUG
    #ifndef wxHAVE_CEF_DEBUG
        #define wxHAVE_CEF_DEBUG
    #endif
#endif

// When not using debug version of CEF under Unix we need to make sure to
// predefine NDEBUG before including its headers to avoid ODR violations.
#ifndef wxHAVE_CEF_DEBUG
    #ifndef NDEBUG
        #define NDEBUG
        #define wxUNDEF_NDEBUG
    #endif
#endif

#ifdef __WXOSX__
#include "wx/osx/private/webview_chromium.h"
#endif

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

wxGCC_WARNING_SUPPRESS(unused-parameter)

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_request_context_handler.h"
#include "include/cef_scheme.h"
#include "include/cef_string_visitor.h"
#include "include/cef_version.h"
#include "include/base/cef_lock.h"
#include "include/wrapper/cef_resource_manager.h"

wxGCC_WARNING_RESTORE(unused-parameter)

#ifdef __VISUALC__
#pragma warning(pop)
#endif

#ifdef wxUNDEF_NDEBUG
    #undef NDEBUG
#endif

#if CHROME_VERSION_BUILD < 5845
#error "Unsupported CEF version"
#endif

namespace
{

constexpr const char* TRACE_CEF = "cef";

#define TRACE_CEF_FUNCTION() wxLogTrace(TRACE_CEF, "%s called", __func__)

} // anonymous namespace

extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendChromium[] = "wxWebViewChromium";

bool wxWebViewChromium::ms_cefInitialized = false;

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewChromium, wxWebView);

namespace wxCEF
{

// ----------------------------------------------------------------------------
// AppImplData contains data shared by all wxWebViewChromium objects
// ----------------------------------------------------------------------------

struct AppImplData : wxEventFilter
{
    // This function is called to dispatch events to the browser.
    void DoMessageLoopWork();

    // Call the function above before processing every event.
    //
    // This is a bit brutal, but DoMessageLoopWork() should return pretty
    // quickly if it has nothing to do and not doing this results in weird
    // bugs, e.g. clicking embedded text boxes doesn't give focus to them.
    virtual int FilterEvent(wxEvent& event) override
    {
        // But we can at least skip doing it for the idle events, as they don't
        // correspond to anything CEF is interested in.
        if ( event.GetEventType() != wxEVT_IDLE )
            DoMessageLoopWork();

        return Event_Skip;
    }

#ifdef __WXGTK__
    // Called from DoMessageLoopWork() if the thread context is allocated.
    void StartThreadDispatch(GMainContext* threadContext);

    // Called from DoMessageLoopWork() when the thread context is freed.
    void StopThreadDispatch();


    // Thread context we're dispatching the events for, if non-null.
    GMainContext* m_threadContext = nullptr;

    // The special source used to wake us up.
    GSource* m_wakeupSource = nullptr;
#endif // __WXGTK__
};

#ifdef __WXGTK__

extern "C"
{

gboolean WakeupSourcePrepare(GSource*, gint* timeout)
{
    if ( timeout )
    {
        // No need to call us again sooner than necessary, so don't set any
        // timeout for the poll.
        *timeout = -1;
    }

    // Always poll, don't dispatch right now.
    return FALSE;
}

gboolean WakeupSourceCheck(GSource*)
{
    // We can't dispatch events from check function, but we can wake up our
    // idle handler to do it next.
    wxWakeUpIdle();

    // Still don't dispatch.
    return FALSE;
}

} // extern "C"

// We don't want to initialize private fields of GSourceFuncs, but not doing
// this results in this warning.
wxGCC_WARNING_SUPPRESS(missing-field-initializers)

GSourceFuncs wakeupSourceVtbl =
{
    WakeupSourcePrepare,
    WakeupSourceCheck,
    nullptr, // No WakeupSourceDispatch as it should never be called.
    nullptr, // No WakeupSourceFinalize because we don't have anything to free.
};

wxGCC_WARNING_RESTORE(missing-field-initializers)

void AppImplData::StartThreadDispatch(GMainContext* threadContext)
{
    if ( !g_main_context_acquire(threadContext) )
    {
        // This really shouldn't happen, no idea what to do if it does.
        wxLogDebug("Failed to acquire thread-specific context.");
        return;
    }

    wxLogTrace(TRACE_CEF, "Acquired thread-specific context");
    m_threadContext = threadContext;

    // Create the special source which will wake us up whenever there
    // are any events to dispatch: otherwise our main message loop could be
    // woken up due to the events for Chrome being received from X server, but
    // would do nothing with them as it doesn't know how to process them, and
    // not even wake us up to do it.
    m_wakeupSource = g_source_new(&wakeupSourceVtbl, sizeof(GSource));

    // Note that we need to attach it to the main context, not the thread one
    // (otherwise this source itself would be never used either).
    g_source_attach(m_wakeupSource, nullptr /* main context */);
}

void AppImplData::StopThreadDispatch()
{
    g_source_destroy(m_wakeupSource);
    g_source_unref(m_wakeupSource);
    m_wakeupSource = nullptr;

    g_main_context_release(m_threadContext);
    m_threadContext = nullptr;
    wxLogTrace(TRACE_CEF, "Released thread-specific context");
}

#endif // __WXGTK__

AppImplData gs_appData;

void AppImplData::DoMessageLoopWork()
{
#ifdef __WXGTK__
    // Chrome may/will create its own GMainContext when it's used from the
    // non-main thread and normal GTK message loop run by gtk_main() doesn't
    // dispatch the events for it, as it only uses the default application
    // context. So check for this case and do it ourselves if necessary.
    auto const threadContext = g_main_context_get_thread_default();
    if ( threadContext != m_threadContext )
    {
        if ( threadContext )
            StartThreadDispatch(threadContext);
        else
            StopThreadDispatch();
    }

    while ( threadContext )
    {
        if ( !g_main_context_iteration(threadContext, FALSE /* don't block */) )
            break;
    }
#endif // __WXGTK__

    CefDoMessageLoopWork();
}

// ----------------------------------------------------------------------------
// Chromium-specific configuration data
// ----------------------------------------------------------------------------

class wxWebViewConfigurationImplChromium : public wxWebViewConfigurationImpl,
                                           public wxWebViewConfigurationChromium
{
public:
    static wxWebViewConfiguration MakeConfig()
    {
        return wxWebViewConfiguration(wxWebViewBackendChromium,
                                      new wxWebViewConfigurationImplChromium);
    }

    virtual void* GetNativeConfiguration() const override
    {
        // Our "native" configuration is our own Chromium-specific class from
        // which we inherit.
        const wxWebViewConfigurationChromium* self = this;
        return const_cast<wxWebViewConfigurationChromium*>(self);
    }

    virtual void SetDataPath(const wxString& path) override
    {
        m_dataPath = path;
    }

    virtual wxString GetDataPath() const override
    {
        return m_dataPath;
    }

private:
    wxString m_dataPath;
};

// ----------------------------------------------------------------------------
// ImplData contains data for a single wxWebViewChromium instance
// ----------------------------------------------------------------------------

struct ImplData
{
    explicit ImplData(const wxWebViewConfiguration& config =
                        wxWebViewConfigurationImplChromium::MakeConfig())
        : m_config{config}
    {
    }

    wxWebViewConfiguration m_config;

#ifdef __WXGTK__
    // Due to delayed creation of the browser in wxGTK we need to remember the
    // URL passed to Create() as we can't use it there directly.
    //
    // We also use this as a flag: it will be non-empty if the browser needs to
    // be created.
    wxString m_initialURL;
#endif // __WXGTK__

    // We also remember the proxy passed to wxWebView::SetProxy() as we can
    // only set it when creating the browser currently.
    wxString m_proxy;

    // These flags are used when destroying wxWebViewChromium, see its dtor.
    bool m_calledDoClose = false;
    bool m_calledOnBeforeClose = false;

    // This can be changed by the application and affects popup menus.
    bool m_enableDevTools = false;
};

// ----------------------------------------------------------------------------
// ClientHandler
// ----------------------------------------------------------------------------

// ClientHandler implementation.
class ClientHandler : public CefClient,
    public CefContextMenuHandler,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefLoadHandler,
    public CefRequestHandler,
    public CefResourceRequestHandler
{
private:
    // Custom context menu commands
    enum class CustomMenuId
    {
        // The first ID value is completely arbitrary, just don't start
        // directly at MENU_ID_USER_FIRST as application might want to add its
        // own items in this range.
        SHOW_DEVTOOLS = MENU_ID_USER_FIRST + 500,
        CLOSE_DEVTOOLS,
        INSPECT_ELEMENT
    };

public:
    // Ctor must be given a backpointer to wxWebView which must remain valid
    // for the entire lifetime of this object.
    explicit ClientHandler(wxWebViewChromium& webview) : m_webview{webview} {}

    // Overridden CefClient methods
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override  { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override  { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override  { return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

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
        cef_window_open_disposition_t target_disposition,
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

    // CefRequestHandler methods
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> WXUNUSED(browser),
        CefRefPtr<CefFrame> WXUNUSED(frame),
        CefRefPtr<CefRequest> WXUNUSED(request),
        bool WXUNUSED(is_navigation),
        bool WXUNUSED(is_download),
        const CefString& WXUNUSED(request_initiator),
        bool& WXUNUSED(disable_default_handling)) override
    {
        return this;
    }

    // CefResourceRequestHandler methods
    virtual cef_return_value_t OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override;
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) override;


    // Our own functions.

    // Initialize the resource manager, creating it if necessary (so don't call
    // this if you don't want it to be created).
    CefRefPtr<CefResourceManager> GetResourceManager()
    {
        if ( !m_resourceManager )
            m_resourceManager = new CefResourceManager{};

        return m_resourceManager;
    }

    CefRefPtr<CefBrowser> GetBrowser() const { return m_browser; }

    // Return the main frame. May be null.
    CefRefPtr<CefFrame> GetMainFrame() const
    {
        if ( !m_browser )
            return {};

        return m_browser->GetMainFrame();
    }

    // Return the browser host. May be null.
    CefRefPtr<CefBrowserHost> GetHost() const
    {
        if ( !m_browser )
            return {};

        return m_browser->GetHost();
    }

    // Return the underlying window handle: HWND under MSW, X11 Window under
    // GTK.
    //
    // The handle can be 0.
    CefWindowHandle GetWindowHandle() const
    {
        auto host = GetHost();
        if ( !host )
            return 0;

        return host->GetWindowHandle();
    }

    // Open the dev tools window.
    bool ShowDevTools()
    {
        if ( !m_browser )
            return false;

        DoShowDevTools(m_browser, CefPoint{});

        return true;
    }

private:
    void DoShowDevTools(CefRefPtr<CefBrowser> browser,
        const CefPoint& inspect_element_at);
    void CloseDevTools(CefRefPtr<CefBrowser> browser);

    CefRefPtr<CefBrowser> m_browser;
    wxWebViewChromium& m_webview;
    int m_browserId;
    // Record the load error code: enum wxWebViewNavigationError
    // -1 means no error.
    int m_loadErrorCode = -1;

    CefRefPtr<CefResourceManager> m_resourceManager;

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

class wxBrowserProcessHandler : public CefBrowserProcessHandler
{
public:
    wxBrowserProcessHandler() = default;

    void OnScheduleMessagePumpWork(int64_t delay_ms) override
    {
        if ( delay_ms > 0 )
        {
            // Time when we should do work.
            const auto scheduledTime = wxGetUTCTimeMillis() + delay_ms;

            if ( m_timer.IsRunning() )
            {
                if ( m_nextTimer > scheduledTime )
                {
                    // Existing timer will expire too late, restart it.
                    m_timer.Stop();
                }
                else
                {
                    wxLogTrace(TRACE_CEF,
                               "work already scheduled before %lldms",
                               delay_ms);
                    return;
                }
            }

            wxLogTrace(TRACE_CEF, "schedule work in %lldms", delay_ms);
            m_timer.StartOnce(delay_ms);

            m_nextTimer = scheduledTime;
        }
        else
        {
            if ( wxTheApp )
                wxTheApp->CallAfter([]() { gs_appData.DoMessageLoopWork(); });
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
            gs_appData.DoMessageLoopWork();
        }
    } m_timer;

    // Time when the currently running timer will expire.
    wxLongLong m_nextTimer = 0;

    IMPLEMENT_REFCOUNTING(wxBrowserProcessHandler);
};

// Application class used in the browser process.
class wxCefBrowserApp : public CefApp
{
public:
    wxCefBrowserApp()
        : m_browserProcessHandler(new wxBrowserProcessHandler{})
    {
    }

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return m_browserProcessHandler;
    }

private:
    CefRefPtr<CefBrowserProcessHandler> m_browserProcessHandler;

    IMPLEMENT_REFCOUNTING(wxCefBrowserApp);
};

} // namespace wxCEF

using namespace wxCEF;

void wxWebViewChromium::Init()
{
    m_implData = new ImplData{};
}

wxWebViewChromium::wxWebViewChromium(const wxWebViewConfiguration& config)
{
    m_implData = new ImplData{config};
}

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

    // Hide the window while creating it, so that its GTKHandleRealized() is
    // not called immediately, but only later, once we show it when exiting
    // this function.
    class DelayedShow
    {
    public:
        explicit DelayedShow(wxWindow& self) : self_{self}
        {
            self_.wxWindowBase::Hide();
        }

        ~DelayedShow()
        {
            self_.Show();
        }

    private:
        wxWindowBase& self_;
    } delayedShow{*this};
#endif

    if ( !wxControl::Create(parent, id, pos, size, style,
                           wxDefaultValidator, name) )
    {
        return false;
    }
    if ( !InitCEF(m_implData->m_config) )
        return false;

#ifdef __WXMSW__
    MSWDisableComposited();
#endif // __WXMSW__

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
    m_implData->m_initialURL = url.empty() ? wxString(" ") : url;
#else
    // Under the other platforms we can call it immediately.
    if ( !DoCreateBrowser(url) )
        return false;
#endif

    Bind(wxEVT_SIZE, &wxWebViewChromium::OnSize, this);

    return true;
}

#ifdef __WXGTK__

void wxWebViewChromium::GTKHandleRealized()
{
    auto& initialURL = m_implData->m_initialURL;

    if ( initialURL.empty() )
        return;

    // Unfortunately there is nothing we can do here if it fails, so just
    // ignore the return value.
    DoCreateBrowser(initialURL);

    // Ensure we don't call it any more.
    initialURL.clear();
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
    const auto handleValue = static_cast<wxUIntPtr>(handle);
#else
    const auto handle = GetHandle();
    const auto handleValue = wxPtrToUInt(handle);
#endif

    const wxSize sz = GetClientSize();
    info.SetAsChild(handle, {0, 0, sz.x, sz.y});

    // Create a request context (which will possibly remain empty) to allow
    // setting the proxy if it was specified.
    CefRefPtr<CefRequestContext> reqContext;

    const auto& proxy = m_implData->m_proxy;
    if ( !proxy.empty() )
    {
        CefRequestContextSettings reqSettings;
        reqContext = CefRequestContext::CreateContext(reqSettings, nullptr);

        // The structure of "proxy" dictionary seems to be documented at
        //
        // https://developer.chrome.com/docs/extensions/reference/proxy/
        //
        // but it looks like we can also use a much simpler dictionary instead
        // of defining "ProxyRules" sub-dictionary as documented there, so just
        // do this instead.
        auto proxyDict = CefDictionaryValue::Create();
        auto proxyVal = CefValue::Create();

        if ( !proxyDict->SetString("mode", "fixed_servers") ||
                !proxyDict->SetString("server", proxy.ToStdWstring()) ||
                    !proxyVal->SetDictionary(proxyDict) )
        {
            // This is really not supposed to happen.
            wxFAIL_MSG("constructing proxy value failed?");
        }

        CefString error;
        if ( !reqContext->SetPreference("proxy", proxyVal, error) )
        {
            wxLogError(_("Failed to set proxy \"%s\": %s"), proxy, error.ToWString());
        }
    }

    // Check if we need to create a custom client handler.
    const auto configChrome =
        static_cast<wxWebViewConfigurationChromium*>(m_implData->m_config.GetNativeConfiguration());
    if ( auto create = configChrome->m_clientCreate )
    {
        m_actualClient = create(m_clientHandler, configChrome->m_clientCreateData);
        if ( m_actualClient )
            m_actualClient->AddRef();
    }

    if ( !m_actualClient )
        m_actualClient = m_clientHandler;

    if ( !CefBrowserHost::CreateBrowser(
            info,
            CefRefPtr<CefClient>{m_actualClient},
            url.ToStdString(),
            browsersettings,
            nullptr, // No extra info
            reqContext
        ) )
    {
        wxLogTrace(TRACE_CEF, "CefBrowserHost::CreateBrowser() failed");
        return false;
    }

    wxLogTrace(TRACE_CEF, "Browser created as child of %zx", handleValue);
    return true;
}

wxWebViewChromium::~wxWebViewChromium()
{
    if (m_clientHandler)
    {
        wxLogTrace(TRACE_CEF, "closing browser");

        const auto handle = m_clientHandler->GetWindowHandle();

        constexpr bool forceClose = true;
        m_clientHandler->GetHost()->CloseBrowser(forceClose);

        // We need to destroy the client handler used by the browser to allow
        // it to close.
        m_actualClient->Release();

        // We need to wait until the browser is really closed, which happens
        // asynchronously, as otherwise we could exit the program and call
        // CefShutdown() before ClientHandler is destroyed, which would kill
        // the program with "Object reference incorrectly held at CefShutdown"
        // error message.

        // First wait until our ClientHandler::DoClose() is called: it will
        // reset our m_clientHandler when this happens.
        while ( !m_implData->m_calledDoClose )
            CefDoMessageLoopWork();

#ifdef __WXMSW__
        // Under MSW we need to destroy the window: if we don't do this,
        // OnBeforeClose() won't get called at all, no matter how many messages
        // we dispatch or how many times we call CefDoMessageLoopWork().
        ::DestroyWindow(handle);

        while ( !m_implData->m_calledOnBeforeClose )
            CefDoMessageLoopWork();
#elif defined(__WXGTK__)
        wxLogTrace(TRACE_CEF, "Destroying %lx", handle);

        // This doesn't seem to be necessary, the window gets destroyed on its
        // own when dispatching the events, but still do it as it might speed
        // up the shutdown and we can also do this if there is no active event
        // loop (which should never happen, of course).
        ::XDestroyWindow(wxGetX11Display(), handle);

        const auto threadContext = gs_appData.m_threadContext;
        while ( !m_implData->m_calledOnBeforeClose )
        {
            // Under GTK just calling CefDoMessageLoopWork() isn't enough,
            // we need to dispatch the lower level (e.g. X11) events
            // notifying CEF about the window destruction.
            //
            // Note that we do it whether or not thread context is null: if it
            // is, the main application context is used, which is what we need
            // when running in the main thread.
            g_main_context_iteration(threadContext, TRUE /* block */);
        }
#elif defined(__WXOSX__)
        // There doesn't seem to be any way to force OnBeforeClose() to be
        // called from here under Mac as it's referenced by an autorelease pool
        // in the outer frame, so just return and count on that pool dtor
        // really destroying the object before CefShutdown() is called.
        wxUnusedVar(handle);
#endif

        // If we hadn't release our own client handler above, we need to do it
        // now (notice that it's safe to do it, as it can't be used any more).
        if ( m_clientHandler != m_actualClient )
            m_clientHandler->Release();
    }

    delete m_implData;
}

// This Linux-specific section exists in order to check that we're not going to
// hang after calling CefInitialize(), as happens if libcef.so doesn't come
// first (or at least before libc.so) in the load order. As debugging this is
// if it happens is not fun at all, it justifies having all this extra code
// just to check for this.
#ifdef __LINUX__

#include "wx/dynlib.h"

bool CheckCEFLoadOrder()
{
    bool foundLibc = false;

    for ( const auto& det : wxDynamicLibrary::ListLoaded() )
    {
        const auto& name = det.GetName();
        if ( name.StartsWith("libc.so") )
        {
            foundLibc = true;
        }
        else if ( name.StartsWith("libcef.so") )
        {
            if ( foundLibc )
            {
                wxLogError(
                    _("Chromium can't be used because libcef.so was't "
                      "loaded early enough; please relink the application "
                      "or use LD_PRELOAD to load it earlier.")
                );
                return false;
            }

            // We've found libcef.so before libc.so, no need to continue.
            break;
        }
        //else: some other library, ignore
    }

    return true;
}

#endif // __LINUX__

bool wxWebViewChromium::InitCEF(const wxWebViewConfiguration& config)
{
    if (ms_cefInitialized)
        return true;

#ifdef __LINUX__
    if ( !CheckCEFLoadOrder() )
        return false;
#endif

    // Construct the path to the confusingly named "root cache" CEF directory
    // which is actually called "user data" directory in Chrome. It is the
    // directory containing "cache" CEF directories (more clearly called
    // "profile" directories in Chrome) and this is the only directory which we
    // can set.
    //
    // It contains both "config" and "cache" files, but as it doesn't contain
    // only the latter, don't put it under GetUserDir(Dir_Cache) by default,
    // although applications that know that they are not going to have any
    // important data there should probably change this by specifying a
    // different directory to use via wxWebViewConfiguration.
    wxFileName cefDataFolder;
    const wxString& dataDir = config.GetDataPath();
    if ( !dataDir.empty() )
    {
        cefDataFolder = wxFileName::DirName(dataDir);
    }
    else
    {
        cefDataFolder = wxFileName::DirName(
                            wxStandardPaths::Get().GetUserLocalDataDir()
                        );
        cefDataFolder.AppendDir("CEF");
    }

    if ( !cefDataFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
    {
        wxLogTrace(TRACE_CEF, "Failed to create CEF directory \"%s\"",
                   cefDataFolder.GetFullPath());
    }

    CefSettings settings;

    const auto app = wxApp::GetInstance();
    wxCHECK_MSG(app, false, "Can't use wxWebViewChromium without wxApp");

    // Check for the presence of a separate helper application under non-macOS
    // platforms (under the latter the helper process must always be present
    // inside the application bundle).
#ifndef __WXOSX__
    wxFileName helperApp(wxStandardPaths::Get().GetExecutablePath());
    helperApp.SetName(app->GetAppName() + "_cef_helper");
    if ( helperApp.FileExists() )
    {
        const wxString& helperPath = helperApp.GetAbsolutePath();

        wxLogTrace(TRACE_CEF, "Using \"%s\" as CEF helper", helperPath);
        CefString(&settings.browser_subprocess_path).FromWString(helperPath.ToStdWstring());
    }
#endif // !__WXOSX__

    const wxString cefPath = cefDataFolder.GetFullPath();

    wxFileName userDataPath(cefPath, "UserData");
    CefString(&settings.root_cache_path).FromWString(userDataPath.GetFullPath().ToStdWstring());

    // Set up CEF for use inside another application, as is the case for us.
    settings.multi_threaded_message_loop = false;
    settings.external_message_pump = true;

    settings.no_sandbox = true;

    // Configure logging.
    const auto configChrome =
        static_cast<wxWebViewConfigurationChromium*>(config.GetNativeConfiguration());
    wxCHECK_MSG( configChrome, false, "Should have Chromium-specific config" );

    settings.log_severity = static_cast<cef_log_severity_t>(configChrome->m_logLevel);
    wxString logFile = configChrome->m_logFile;
    if ( logFile.empty() )
    {
        logFile = wxFileName(cefPath, "debug.log").GetFullPath();
    }

    CefString(&settings.log_file).FromWString(logFile.ToStdWstring());

    // And remote debugging, if specified.
    if ( configChrome->m_remoteDebuggingPort )
    {
        settings.remote_debugging_port = configChrome->m_remoteDebuggingPort;
    }

#ifdef __WXMSW__
    CefMainArgs args(wxGetInstance());
#else
    CefMainArgs args(app->argc, app->argv);
#endif

    CefRefPtr<CefApp> cefApp{new wxCefBrowserApp{}};
    if ( !CefInitialize(args, settings, cefApp, nullptr) )
    {
        wxLogError(_("Could not initialize Chromium"));
        return false;
    }

    wxEvtHandler::AddFilter(&gs_appData);

    ms_cefInitialized = true;
    return true;
}

void wxWebViewChromium::ShutdownCEF()
{
    if (ms_cefInitialized)
    {
        wxEvtHandler::RemoveFilter(&gs_appData);

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
    ::SetWindowPos(handle, nullptr, 0, 0, size.x, size.y,
                   SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
#elif defined(__WXGTK__)
    size *= GetDPIScaleFactor();

    ::XResizeWindow(wxGetX11Display(), handle, size.x, size.y);
#elif defined(__WXOSX__)
    wxWebViewChromium_Resize(handle, size);
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
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->LoadURL(url.ToStdString());
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
    auto browser = m_clientHandler->GetBrowser();
    wxCHECK_RET( browser, "No valid browser object" );

    browser->StopLoad();
}

void wxWebViewChromium::Reload(wxWebViewReloadFlags flags)
{
    auto browser = m_clientHandler->GetBrowser();
    wxCHECK_RET( browser, "No valid browser object" );

    if ( flags == wxWEBVIEW_RELOAD_NO_CACHE )
    {
        browser->ReloadIgnoreCache();
    }
    else
    {
        browser->Reload();
    }
}

bool wxWebViewChromium::SetProxy(const wxString& proxy)
{
    wxCHECK_MSG( !m_clientHandler, false, "should be called before Create()" );

    m_implData->m_proxy = proxy;

    return true;
}

void wxWebViewChromium::EnableAccessToDevTools(bool enable)
{
    m_implData->m_enableDevTools = enable;
}

bool wxWebViewChromium::ShowDevTools()
{
    return m_clientHandler->ShowDevTools();
}

bool wxWebViewChromium::IsAccessToDevToolsEnabled() const
{
    return m_implData->m_enableDevTools;
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
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_MSG( frame, {}, "No valid frame" );

    return frame->GetURL().ToString();
}

wxString wxWebViewChromium::GetCurrentTitle() const
{
    return m_title;
}

void wxWebViewChromium::Print()
{
    auto host = m_clientHandler->GetHost();
    wxCHECK_RET( host, "No valid host" );

    host->Print();
}

void wxWebViewChromium::Cut()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->Cut();
}

void wxWebViewChromium::Copy()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->Copy();
}

void wxWebViewChromium::Paste()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->Paste();
}

void wxWebViewChromium::Undo()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->Undo();
}

void wxWebViewChromium::Redo()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    frame->Redo();
}

void wxWebViewChromium::SelectAll()
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

   frame->SelectAll();
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
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_MSG( frame, false, "No valid frame" );

    frame->ExecuteJavaScript(javascript.ToStdString(), {}, 0);

    // Returning a result is currently unsupported
    if (output)
        return false;
    else
        return true;
}

bool wxWebViewChromium::IsBusy() const
{
    auto browser = m_clientHandler->GetBrowser();
    if ( !browser )
        return false;

    return browser->IsLoading();
}

void wxWebViewChromium::SetEditable(bool enable)
{
    wxString mode = enable ? "\"on\"" : "\"off\"";
    RunScript("document.designMode = " + mode);
}

void wxWebViewChromium::DoSetPage(const wxString& html, const wxString& baseUrl)
{
    auto frame = m_clientHandler->GetMainFrame();
    wxCHECK_RET( frame, "No valid frame" );

    wxUnusedVar(baseUrl);

    // This seems to be the only way to load a string in CEF now, see
    // https://github.com/chromiumembedded/cef/issues/2586
    const wxScopedCharBuffer& buf = html.utf8_str();
    const wxString url{
        "data:text/html;base64," + wxBase64Encode(buf.data(), buf.length())
    };

    frame->LoadURL(url.ToStdWstring());
}

wxWebViewZoom wxWebViewChromium::GetZoom() const
{
     return m_zoomLevel;
}

float wxWebViewChromium::GetZoomFactor() const
{
    auto host = m_clientHandler->GetHost();
    wxCHECK_MSG( host, 0.0, "No valid host" );

    return host->GetZoomLevel();
}

void wxWebViewChromium::SetZoomFactor(float mapzoom)
{
    auto host = m_clientHandler->GetHost();
    wxCHECK_RET( host, "No valid host" );

    host->SetZoomLevel(mapzoom);
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

namespace
{

// Called only by wxWebViewChromium::SetRoot() which ensures that it's called
// on the appropriate thread.
void
SetResourceRoot(
    const CefRefPtr<CefResourceManager>& resourceManager,
    const wxFileName& rootDir
)
{
    resourceManager->AddDirectoryProvider(
        "file:///",
        rootDir.GetFullPath().utf8_string(),
        0,
        "wxRootProvider"
    );
}

} // anonymous namespace

void wxWebViewChromium::SetRoot(const wxFileName& rootDir)
{
    CefPostTask(
        TID_IO,
        base::BindOnce(
            &SetResourceRoot,
            m_clientHandler->GetResourceManager(),
            rootDir
        )
    );
}

bool
ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> WXUNUSED(browser),
    CefRefPtr<CefFrame> frame,
    CefProcessId WXUNUSED(source_process),
    CefRefPtr<CefProcessMessage> message)
{
    // This is not supposed to happen, but test for it nevertheless.
    if ( !frame || !message )
    {
        wxLogTrace(TRACE_CEF, "Ignoring null OnProcessMessageReceived() args");
        return false;
    }

    wxWebViewChromiumEvent event(m_webview, *frame, *message);

    return m_webview.HandleWindowEvent(event);
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
    {
        model->Clear();
        return;
    }

    // Add DevTools items to all context menus if enabled.
    if ( m_webview.IsAccessToDevToolsEnabled() )
    {
        // Add a separator if the menu already has items.
        if ( model->GetCount() )
            model->AddSeparator();

        const auto addMenuItem = [model](CustomMenuId id, const wxString& label)
        {
            model->AddItem(static_cast<int>(id), label.utf8_string());
        };

        addMenuItem(CustomMenuId::SHOW_DEVTOOLS, _("Show DevTools"));
        addMenuItem(CustomMenuId::CLOSE_DEVTOOLS, _("Close DevTools"));

        // Note that if we add support for "Chrome" style, as opposed to using
        // the "Alloy" style we use now, we should stop doing this as "Chrome"
        // style already has "Inspect" menu item in its default context menu.
        model->AddSeparator();
        addMenuItem(CustomMenuId::INSPECT_ELEMENT, _("Inspect"));
    }
}

bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> WXUNUSED(frame),
                                         CefRefPtr<CefContextMenuParams> params,
                                         int command_id,
                                         CefContextMenuHandler::EventFlags WXUNUSED(event_flags))
{
    switch ( static_cast<CustomMenuId>(command_id) )
    {
        case CustomMenuId::SHOW_DEVTOOLS:
            DoShowDevTools(browser, CefPoint{});
            break;
        case CustomMenuId::CLOSE_DEVTOOLS:
            CloseDevTools(browser);
            break;
        case CustomMenuId::INSPECT_ELEMENT:
            DoShowDevTools(browser, CefPoint{params->GetXCoord(), params->GetYCoord()});
            break;
        default:
            // Allow default handling, if any.
            return false;
    }

    return true;
}

void ClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> WXUNUSED(browser),
                                           CefRefPtr<CefFrame> WXUNUSED(frame))
{}

// CefLifeSpanHandler methods
bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> WXUNUSED(browser),
                             CefRefPtr<CefFrame> WXUNUSED(frame),
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             cef_window_open_disposition_t WXUNUSED(target_disposition),
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

        m_webview.NotifyWebViewCreated();
    }
}
bool ClientHandler::DoClose(CefRefPtr<CefBrowser> WXUNUSED(browser))
{
    TRACE_CEF_FUNCTION();

    m_webview.m_implData->m_calledDoClose = true;

    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    TRACE_CEF_FUNCTION();

    // Under Mac the web view and its data might be already destroyed, so don't
    // touch them there, see Mac-specific comment in wxWebViewChromium dtor.
#ifndef __WXOSX__
    m_webview.m_implData->m_calledOnBeforeClose = true;
#endif

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

    wxLogTrace(TRACE_CEF, "Starting to load \"%s\"", url);

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

    wxLogTrace(TRACE_CEF, "Loaded \"%s\"", url);

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

// CefResourceRequestHandler methods
cef_return_value_t ClientHandler::OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback)
{
    if ( !m_resourceManager )
        return RV_CONTINUE;

    return m_resourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> ClientHandler::GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request)
{
    if ( !m_resourceManager )
        return {};

    return m_resourceManager->GetResourceHandler(browser, frame, request);
}

void
ClientHandler::DoShowDevTools(CefRefPtr<CefBrowser> browser,
                              const CefPoint& inspect_element_at)
{
    browser->GetHost()->ShowDevTools({}, {}, {}, inspect_element_at);
}

void ClientHandler::CloseDevTools(CefRefPtr<CefBrowser> browser)
{
    browser->GetHost()->CloseDevTools();
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

#ifdef __WXGTK__
        // We must ensure that we use thread-safe versions of X functions
        // because CEF calls XInitThreads() during initialization and any X
        // objects created before thread support is initialized can't be used
        // after initializing it and doing this results in mysterious crashes
        // (which is why X documentation states that XInitThreads() must be the
        // first function called by the application).
        //
        // It's a bit wasteful to initialize threads if we're not going to use
        // CEF in this run of the application, but it's a much better
        // alternative than crashing, so do it now, before it is too late.
        XInitThreads();
#endif // !__WXGTK__

        // Continue normal execution.
        return -1;
    }
};

wxWebViewChromiumEntry gs_chromiumEntryHook;

} // anonymous namespace

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryChromium : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return new wxWebViewChromium; }

    virtual wxWebView* CreateWithConfig(const wxWebViewConfiguration& config) override
    {
        return new wxWebViewChromium(config);
    }

    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxWebViewNameStr) override
    { return new wxWebViewChromium(parent, id, url, pos, size, style, name); }

    virtual bool IsAvailable() override
    {
#ifdef __WXGTK__
        // Currently CEF works only with X11.
        if ( wxGetDisplayInfo().type != wxDisplayX11 )
            return false;
#endif // __WXGTK__

        return true;
    }

    virtual wxVersionInfo GetVersionInfo(wxVersionContext context) override
    {
        wxString version;
        int major wxDUMMY_INITIALIZE(0),
            minor wxDUMMY_INITIALIZE(0),
            micro wxDUMMY_INITIALIZE(0),
            build wxDUMMY_INITIALIZE(0);
        switch ( context )
        {
            case wxVersionContext::RunTime:
                major = cef_version_info(0);
                minor = cef_version_info(1);
                micro = cef_version_info(2);
                build = cef_version_info(3);
                break;

            case wxVersionContext::BuildTime:
                major = CEF_VERSION_MAJOR;
                minor = CEF_VERSION_MINOR;
                micro = CEF_VERSION_PATCH;
                build = CEF_COMMIT_NUMBER;

                version = CEF_VERSION;
                break;
        }

        return {"CEF", major, minor, micro, build, version};
    }

    virtual wxWebViewConfiguration CreateConfiguration() override
    {
        return wxWebViewConfigurationImplChromium::MakeConfig();
    }
};

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

// ----------------------------------------------------------------------------
// wxWebViewChromiumEvent
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewChromiumEvent, wxCommandEvent);

wxDEFINE_EVENT( wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED, wxWebViewChromiumEvent );

wxWebViewChromiumEvent::wxWebViewChromiumEvent(
    wxWebViewChromium& webview,
    CefFrame& frame,
    CefProcessMessage& message
) : wxCommandEvent(wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED, webview.GetId()),
    m_frame(&frame),
    m_message(&message)
{
    SetEventObject(&webview);
}

wxString wxWebViewChromiumEvent::GetMessageName() const
{
    wxCHECK_MSG( m_message, {}, "No message" );

    return m_message->GetName().ToWString();
}

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM
