/////////////////////////////////////////////////////////////////////////////
// Name:        webview.h
// Purpose:     interface of wxWebView
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Zoom levels available in wxWebView
*/
enum wxWebViewZoom
{
    wxWEBVIEW_ZOOM_TINY,
    wxWEBVIEW_ZOOM_SMALL,
    wxWEBVIEW_ZOOM_MEDIUM, //!< default size
    wxWEBVIEW_ZOOM_LARGE,
    wxWEBVIEW_ZOOM_LARGEST
};

/**
    The type of zooming that the web view control can perform
*/
enum wxWebViewZoomType
{
    /**
        The entire layout scales when zooming, including images
    */
    wxWEBVIEW_ZOOM_TYPE_LAYOUT,
    /**
        Only the text changes in size when zooming, images and other layout
        elements retain their initial size
    */
    wxWEBVIEW_ZOOM_TYPE_TEXT
};

/**
    Types of errors that can cause navigation to fail
*/
enum wxWebViewNavigationError
{
    /** Connection error (timeout, etc.) */
    wxWEBVIEW_NAV_ERR_CONNECTION,
    /** Invalid certificate */
    wxWEBVIEW_NAV_ERR_CERTIFICATE,
    /** Authentication required */
    wxWEBVIEW_NAV_ERR_AUTH,
    /** Other security error */
    wxWEBVIEW_NAV_ERR_SECURITY,
    /** Requested resource not found */
    wxWEBVIEW_NAV_ERR_NOT_FOUND,
    /** Invalid request/parameters (e.g. bad URL, bad protocol,
        unsupported resource type) */
    wxWEBVIEW_NAV_ERR_REQUEST,
    /** The user cancelled (e.g. in a dialog) */
    wxWEBVIEW_NAV_ERR_USER_CANCELLED,
    /** Another (exotic) type of error that didn't fit in other categories*/
    wxWEBVIEW_NAV_ERR_OTHER
};

/**
    Type of refresh
*/
enum wxWebViewReloadFlags
{
    /** Default reload, will access cache */
    wxWEBVIEW_RELOAD_DEFAULT,
    /** Reload the current view without accessing the cache */
    wxWEBVIEW_RELOAD_NO_CACHE
};

/**
    Find flags used when searching for text on page.
*/
enum wxWebViewFindFlags
{
    /** Causes the search to restart when end or beginning reached */
    wxWEBVIEW_FIND_WRAP =             0x0001,

    /** Matches an entire word when searching */
    wxWEBVIEW_FIND_ENTIRE_WORD =      0x0002,

    /** Match case, i.e. case sensitive searching */
    wxWEBVIEW_FIND_MATCH_CASE =       0x0004,

    /** Highlights the search results */
    wxWEBVIEW_FIND_HIGHLIGHT_RESULT = 0x0008,

    /** Searches for phrase in backward direction */
    wxWEBVIEW_FIND_BACKWARDS =        0x0010,

    /** The default flag, which is simple searching */
    wxWEBVIEW_FIND_DEFAULT =          0
};

/**
    Navigation action types.

    @since 3.1.2
*/
enum wxWebViewNavigationActionFlags
{
    /** No navigation action */
    wxWEBVIEW_NAV_ACTION_NONE,
    /** The navigation was started by the user */
    wxWEBVIEW_NAV_ACTION_USER,
    /**The navigation was started but not by the user*/
    wxWEBVIEW_NAV_ACTION_OTHER
};

/**
    Specifies at which place of documents an user script will be inserted.

    @since 3.1.5
*/
enum wxWebViewUserScriptInjectionTime
{
    /** Insert the code of the user script at the beginning of loaded documents. */
    wxWEBVIEW_INJECT_AT_DOCUMENT_START,
    /** Insert the code of the user script at the end of the loaded documents. */
    wxWEBVIEW_INJECT_AT_DOCUMENT_END
};

/**
    Internet Explorer emulation modes for wxWebViewIE.

    Elements of this enum can be used with wxWebView::MSWSetEmulationLevel().

    Note that using the @c _FORCE variants is not recommended.

    @since 3.1.3
*/
enum wxWebViewIE_EmulationLevel
{
    /**
        Clear FEATURE_BROWSER_EMULATION registry setting to default,
        corresponding application specific registry key will be deleted
    */
    wxWEBVIEWIE_EMU_DEFAULT =    0,

    /** Prefer IE7 Standards mode, default value for the control. */
    wxWEBVIEWIE_EMU_IE7 =        7000,

    /** Prefer IE8 mode, default value for Internet Explorer 8. */
    wxWEBVIEWIE_EMU_IE8 =        8000,
    /** Force IE8 Standards mode, ignore !DOCTYPE directives. */
    wxWEBVIEWIE_EMU_IE8_FORCE =  8888,

    /** Prefer IE9 mode, default value for Internet Explorer 9. */
    wxWEBVIEWIE_EMU_IE9 =        9000,
    /** Force IE9 Standards mode, ignore !DOCTYPE directives. */
    wxWEBVIEWIE_EMU_IE9_FORCE =  9999,

    /** Prefer IE10 mode, default value for Internet Explorer 10. */
    wxWEBVIEWIE_EMU_IE10 =       10000,
    /** Force IE10 Standards mode, ignore !DOCTYPE directives. */
    wxWEBVIEWIE_EMU_IE10_FORCE = 10001,

    /** Prefer IE11 edge mode, default value for Internet Explorer 11. */
    wxWEBVIEWIE_EMU_IE11 =       11000,
    /** Force IE11 edge mode, ignore !DOCTYPE directives. */
    wxWEBVIEWIE_EMU_IE11_FORCE = 11001
};

/**
    A class describing the window information for a new child window.

    An object of this class can be obtained using wxWebViewEvent::GetTargetWindowFeatures()
    while handling @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES.

    If a @c wxEVT_WEBVIEW_NEWWINDOW is not vetoed, a @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES
    event will be sent to the application. The application can then create a new
    window and call wxWebViewEvent::GetTargetWindowInfo() to get this class providing
    information about the new window. A new child web view will be available
    via GetChildWebView(). The application can then place the child web view into
    the new window by calling wxWebView::Create() on the child web view.

    Sample JavaScript opening a new window:
    @code
        window.open("http://www.wxwidgets.org", "newWindow", "width=400,height=400");
    @endcode

    Sample C++ code handling a new window request:
    @code
        // Bind new window handler
        m_webView->Bind(wxEVT_WEBVIEW_NEWWINDOW, [](wxWebViewEvent& evt) {
            if (evt.GetURL() == "http://badwebsite.com")
                evt.Veto(); // Disallow new window for badwebsite.com
            else
                evt.Skip(); // Allow new window for all other websites
        });

        // Bind new window features handler
        m_webView->Bind(wxEVT_WEBVIEW_NEWWINDOW_FEATURES, [](wxWebViewEvent& evt) {
            // Get target window features
            wxWebViewWindowFeatures* features = evt.GetTargetWindowFeatures();
            // Create a top level window for the child web view
            wxWindow* win = new wxWindow(this, wxID_ANY, features->GetPosition(), features->GetSize());
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            win->SetSizer(sizer);
            // Get the child web view
            wxWebView* childWebView = features->GetChildWebView();
            // Place the child web view into the window
            childWebView->Create(win, wxID_ANY);
            sizer->Add(childWebView, 1, wxEXPAND);
        }
    @endcode

    @since 3.3.0
*/
class WXDLLIMPEXP_WEBVIEW wxWebViewWindowFeatures
{
public:
    /**
        Get the child web view for the target window.

        This is available in the event handler for @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES
        and wxWebView::Create() @b must be called on the child web view directly.

        The requested URL will be loaded automatically in the child web view.
    */
    wxWebView* GetChildWebView();

    /**
        Returns the position of the new window if specified by
        a @c window.open() call.
    */
    virtual wxPoint GetPosition() const = 0;

    /**
        Returns the size of the new window if specified by
        a @c window.open() call.
    */
    virtual wxSize GetSize() const = 0;

    /**
        Returns @true if the target window is expected to display
        a menu bar as specified by a @c window.open() call.
    */
    virtual bool ShouldDisplayMenuBar() const = 0;

    /**
        Returns @true if the target window is expected to display
        a status bar as specified by a @c window.open() call.
    */
    virtual bool ShouldDisplayStatusBar() const = 0;

    /**
        Returns @true if the target window is expected to display
        a tool bar as specified by a @c window.open() call.
    */
    virtual bool ShouldDisplayToolBar() const = 0;

    /**
        Returns @true if the target window is expected to display
        scroll bars as specified by a @c window.open() call.
    */
    virtual bool ShouldDisplayScrollBars() const = 0;
};

/**
    @class wxWebViewConfiguration

    This class allows access to web view configuration options and settings,
    that have to be specified before placing a webview in a window with
    wxWebView::Create().

    @since 3.3.0
    @library{wxwebview}
    @category{webview}

    @see wxWebView::NewConfiguration()
 */
class WXDLLIMPEXP_WEBVIEW wxWebViewConfiguration
{
public:
    /**
        Return the pointer to the native configuration used during creation of
        a wxWebView.

        When using two-step creation this method can be used to customize
        configuration options not available via GetNativeBackend()
        after using Create().

        Additional instances of wxWebView must be created using the same
        wxWebViewConfiguration instance.

        All settings @b must be set before creating a new web view with
        wxWebView::New().

        The return value needs to be down-casted to the appropriate type
        depending on the platform:
            - macOS:
              <a href="https://developer.apple.com/documentation/webkit/wkwebviewconfiguration">WKWebViewConfiguration</a>
              pointer,
            - Windows with Edge:
              <a href="https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2environmentoptions">ICoreWebView2EnvironmentOptions</a>.
            - WebKitGTK:
              <a href="https://webkitgtk.org/reference/webkit2gtk/stable/class.WebContext.html">WebKitWebContext</a>
              pointer.
            - With other backends/platforms it's not implemented.

        The following pseudo code shows how to use this method with two-step
        creation to set no user action requirement to play video in a
        web view:
        @code
            #if defined(__WXMSW__)
            #include "webview2EnvironmentOptions.h"
            #elif defined(__WXOSX__)
            #import "WebKit/WebKit.h"
            #endif

            wxWebViewConfiguration config = wxWebView::NewConfiguration();

            #if defined(__WXMSW__)
            ICoreWebView2EnvironmentOptions* webViewOptions =
                (ICoreWebView2EnvironmentOptions*) config->GetNativeConfiguration();
            webViewOptions->put_AdditionalBrowserArguments("--autoplay-policy=no-user-gesture-required");
            #elif defined(__WXOSX__)
            WKWebViewConfiguration* webViewConfiguration =
                (WKWebViewConfiguration*) config->GetNativeConfiguration();
            webViewConfiguration.mediaTypesRequiringUserActionForPlayback = WKAudiovisualMediaTypeNone;
            #endif

            wxWebView* webView = wxWebView::New(config);
            webView->Create(this, wxID_ANY, "https://www.wxwidgets.org");
        @endcode
    */
    virtual void* GetNativeConfiguration() const { return nullptr; }

    /**
        Returns the backend identifier for which this configuration was created.
    */
    wxString GetBackend() const;

    /**
        Set the data path for the webview.

        This is the path where the webview stores its data, such as cookies,
        local storage, etc.
        @param path The path to the data directory.

        @note This is only used by the Edge and WebKit2GTK+ backend.
    */
    void SetDataPath(const wxString& path);

    /**
        Returns the data path for the webview.

        This is the path where the webview stores its data, such as cookies,
        local storage, etc.
        @return The path to the data directory.

        @note This is only used by the Edge and WebKit2GTK+ backend.
    */
    wxString GetDataPath() const;
};


/**
    @class wxWebViewHandlerRequest

    A class giving access to various parameters of a webview request.

    @since 3.3.0
    @library{wxwebview}
    @category{webview}

    @see wxWebViewHandler::StartRequest()
 */
class WXDLLIMPEXP_WEBVIEW wxWebViewHandlerRequest
{
public:
    /**
        @return The unmodified url of the request.
    */
    virtual wxString GetRawURI() const = 0;

    /**
        @return The url of the request. Can be modified by the backend for
                compatibility.
    */
    virtual wxString GetURI() const { return GetRawURI(); }

    /**
        @return The body data of the request of @c null if nothing was posted.
    */
    virtual wxInputStream* GetData() const = 0;

    /**
        @return The body data of the request as a string. The returned string
            is empty if the supplied @c conv doesn't match the encoding.
    */
    virtual wxString GetDataString(const wxMBConv& conv = wxConvUTF8) const;

    /**
        @return The requests HTTP method (e.g. POST, GET, OPTIONS).
    */
    virtual wxString GetMethod() const = 0;

    /**
        Returns a header from the request or an empty string if the header
        could not be found.

        @param name Name of the header field
    */
    virtual wxString GetHeader(const wxString& name) const = 0;
};

/**
    @class wxWebViewHandlerResponseData

    A class holding the response data. Stream must be available until
    this object is destroyed.

    @since 3.3.0
    @library{wxwebview}
    @category{webview}

    @see wxWebViewHandlerResponse::Finish(wxSharedPtr<wxWebViewHandlerResponseData>)
 */
class WXDLLIMPEXP_WEBVIEW wxWebViewHandlerResponseData
{
public:
    /**
        @return returns pointer to the stream.

        @see wxWebViewHandlerResponse::Finish()
    */
    virtual wxInputStream* GetStream() = 0;
};

/**
    @class wxWebViewHandlerResponse

    A class giving access to various webview response parameters.

    Usually a wxWebViewHandler() would set various parameters required
    for the response like HTTP status, various headers and must then
    call Finish() to complete the response or call FinishWithError() to
    abort the request.

    @since 3.3.0
    @library{wxwebview}
    @category{webview}

    @see wxWebViewHandler::StartRequest()
 */
class WXDLLIMPEXP_WEBVIEW wxWebViewHandlerResponse
{
public:
    /**
        Sets the status code of the response.

        @param status HTTP status code
    */
    virtual void SetStatus(int status) = 0;

    /**
        Sets the MIME type of the response.

        @param contentType MIME type of the response content
    */
    virtual void SetContentType(const wxString& contentType) = 0;

    /**
        Sets a response header which will be sent to the web view.

        The header will be added if it hasn't been set before or replaced
        otherwise.

        @param name
            Name of the header
        @param value
            String value of the header
    */
    virtual void SetHeader(const wxString& name, const wxString& value) = 0;

    /**
        Finishes the request with binary data.

        @param data
            The data object will be dereferenced when the request is completed

        @see Finish(const wxString&, const wxMBConv&)
    */
    virtual void Finish(wxSharedPtr<wxWebViewHandlerResponseData> data) = 0;

    /**
        Finishes the request with text data.

        @param text
            Text content of the response (can be empty)
        @param conv
            Conversion used when sending the text in the response

        @see Finish(wxSharedPtr<wxWebViewHandlerResponseData>)
    */
    virtual void Finish(const wxString& text, const wxMBConv& conv = wxConvUTF8);

    /**
        Finishes the request as an error.

        This will notify that the request could not produce any data.

        @see Finish()
    */
    virtual void FinishWithError() = 0;
};

/**
    @class wxWebViewHistoryItem

    A simple class that contains the URL and title of an element of the history
    of a wxWebView.

    @since 2.9.3
    @library{wxwebview}
    @category{webview}

    @see wxWebView
 */
class wxWebViewHistoryItem
{
public:
    /**
        Constructor.
    */
    wxWebViewHistoryItem(const wxString& url, const wxString& title);

    /**
        @return The url of the page.
    */
    wxString GetUrl();

    /**
        @return The title of the page.
    */
    wxString GetTitle();
};

/**
    @class wxWebViewFactory

    An abstract factory class for creating wxWebView backends. Each
    implementation of wxWebView should have its own factory.

    @since 2.9.5
    @library{wxwebview}
    @category{webview}

    @see wxWebView
 */
class wxWebViewFactory : public wxObject
{
public:
    /**
        Function to create a new wxWebView with two-step creation,
        wxWebView::Create should be called on the returned object.
        @return the created wxWebView
     */
    virtual wxWebView* Create() = 0;

    /**
        Function to create a new wxWebView with two-step creation
        with a wxWebViewConfiguration, wxWebView::Create should be
        called on the returned object.

        @return the created wxWebView
        @since 3.3.0

        @see CreateConfiguration()
    */
    virtual wxWebView* CreateWithConfig(const wxWebViewConfiguration& config);

    /**
        Function to create a new wxWebView with parameters.
        @param parent Parent window for the control
        @param id ID of this control
        @param url Initial URL to load
        @param pos Position of the control
        @param size Size of the control
        @param style
            Window style. For generic window styles, please see wxWindow.
        @param name Window name.
        @return the created wxWebView
    */
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxWebViewNameStr) = 0;
    /**
        Function to check if the backend is available at runtime. The
        wxWebView implementation can use this function to check all
        runtime requirements without trying to create a wxWebView.

        @return returns @true if the backend can be used or @false if it is
            not available during runtime.

        @since 3.1.5
    */
    virtual bool IsAvailable();

    /**
        Retrieve the version information about this backend implementation.

        @since 3.1.5
    */
    virtual wxVersionInfo GetVersionInfo();

    /**
        Create a wxWebViewConfiguration object for wxWebView instances
        created by this factory.

        @since 3.3.0
    */
    virtual wxWebViewConfiguration CreateConfiguration();
};

/**
    @class wxWebViewHandler

    The base class for handling custom schemes in wxWebView, for example to
    allow virtual file system support.

    A new handler should either implement GetFile() or if a more detailed
    request handling is required (access to headers, post data)
    StartRequest() has to be implemented.

    @since 2.9.3
    @library{wxwebview}
    @category{webview}

    @see wxWebView
 */
class wxWebViewHandler
{
public:
    /**
        Constructor. Takes the name of the scheme that will be handled by this
        class for example @c file or @c zip.
    */
    wxWebViewHandler(const wxString& scheme);

    /**
        @return A pointer to the file represented by @c uri.
    */
    virtual wxFSFile* GetFile(const wxString &uri);

    /**
        @return The name of the scheme, as passed to the constructor.
    */
    virtual wxString GetName() const;

    /**
        Sets a custom security URL. Only used by wxWebViewIE.

        @since 3.1.5
    */
    virtual void SetSecurityURL(const wxString& url);

    /**
        @return The custom security URL. Only used by wxWebViewIE.

        @since 3.1.5
    */
    virtual wxString GetSecurityURL() const;

    /**
        When using the edge backend handler urls are https urls with a
        virtual host. As default @c name.wxsite is used as the virtual hostname.
        If you customize this host, use a non existing site (ideally a reserved
        subdomain of a domain you control). If @c localassests.domain.example is
        used the handlers content will be available under
        %https://localassets.domain.example/

        This has to be set @b before registering the handler via
        wxWebView::RegisterHandler().

        @since 3.3.0
    */
    virtual void SetVirtualHost(const wxString& host);

    /**
        @return The virtual host of this handler

        @see SetVirtualHost()
        @since 3.3.0
    */
    virtual wxString GetVirtualHost() const;

    /**
        Implementing this method allows for more control over requests from
        the backend then GetFile(). More details of the request are available
        from the request object which allows access to URL, method, postdata
        and headers.

        A response can be send via the response object. The response does not
        have to be finished from this method and it's possible to be finished
        asynchronously via wxWebViewHandlerResponse::Finish().

        The following pseudo code demonstrates a typical implementation:
        @code
        void StartRequest(const wxWebViewHandlerRequest& request,
                              wxSharedPtr<wxWebViewHandlerResponse> response) override
        {
            // Set common headers allowing access from XMLHTTPRequests()
            response->SetHeader("Access-Control-Allow-Origin", "*");
            response->SetHeader("Access-Control-Allow-Headers", "*");

            // Handle options request
            if (request.GetMethod().IsSameAs("options", false))
            {
                response->Finish("");
                return;
            }

            // Check the post data type
            if (!request.GetHeader("Content-Type").StartsWith("application/json"))
            {
                response->FinishWithError();
                return;
            }

            // Process input data
            wxString postData = request.GetDataString();

            ...

            // Send result
            response->SetContentType("application/json");
            response->Finish("{ result: true }");
        }
        @endcode

        @note This is only used by macOS and the Edge backend.

        @see GetFile()
        @since 3.3.0
    */
    virtual void StartRequest(const wxWebViewHandlerRequest& request,
                              wxSharedPtr<wxWebViewHandlerResponse> response);
};

/**
    @class wxWebView

    This control may be used to render web (HTML / CSS / JavaScript) documents.
    It is designed to allow the creation of multiple backends for each port,
    although currently just one is available. It differs from wxHtmlWindow in
    that each backend is actually a full rendering engine, Internet Explorer or Edge on MSW and
    WebKit on macOS and GTK. This allows the correct viewing of complex pages with
    JavaScript and CSS.

    @section backend_descriptions Backend Descriptions

    This class supports using multiple backends, corresponding to different
    implementations of the same functionality. Under macOS and Unix platforms
    only a single, WebKit-based, backend is currently provided, but under MSW
    both the legacy IE backend and the new Edge backend exist. Backends are
    identified by their names, documented in the backend descriptions below.

    @subsection wxWEBVIEW_BACKEND_IE wxWEBVIEW_BACKEND_IE (MSW)

    The IE backend uses Microsoft's
    <a href="http://msdn.microsoft.com/en-us/library/aa752085%28v=VS.85%29.aspx">WebBrowser</a>
    control, which depends the locally installed version of Internet Explorer.
    By default this backend emulates Internet Explorer 7. This can be
    changed with a registry setting by wxWebViewIE::MSWSetEmulationLevel() see
    <a href="http://msdn.microsoft.com/en-us/library/ee330730%28v=vs.85%29.aspx#browser_emulation">
    this</a> article for more information.

    This backend has full support for custom schemes and virtual file systems.

    The predefined @c wxWebViewBackendIE constant contains the name of this
    backend and can be used to explicitly select it when using wxWebView::New().

    @note If you plan to display any modern web content you should consider using @ref wxWEBVIEW_BACKEND_EDGE,
          as Internet Explorer is not supported anymore by Microsoft.

    @subsection wxWEBVIEW_BACKEND_EDGE wxWEBVIEW_BACKEND_EDGE (MSW)

    The Edge backend uses Microsoft's
    <a href="https://docs.microsoft.com/en-us/microsoft-edge/hosting/webview2">Edge WebView2</a>.
    It is available for Windows 7 and newer.

    This backend does not support custom schemes. When using handlers see
    wxWebViewHandler::SetVirtualHost() for more details on how to access
    handler provided URLs.

    This backend is not enabled by default, to build it follow these steps:
    - With CMake just enable @c wxUSE_WEBVIEW_EDGE
    - When not using CMake:
        - Download the <a href="https://aka.ms/webviewnuget">WebView2 SDK</a>
        nuget package (Version 1.0.864.35 or newer)
        - Extract the package (it's a zip archive) to @c wxWidgets/3rdparty/webview2
        (you should have @c 3rdparty/webview2/build/native/include/WebView2.h
        file after unpacking it)
        - Enable @c wxUSE_WEBVIEW_EDGE in @c setup.h
    - Build wxWidgets webview library
    - Copy @c WebView2Loader.dll from the subdirectory corresponding to the
      architecture used (x86 or x64) of @c wxWidgets/3rdparty/webview2/build/
      to your applications executable
    - At runtime you can use wxWebView::IsBackendAvailable() to check if the
      backend can be used (it will be available if @c WebView2Loader.dll can be
      loaded and Edge (Chromium) is installed)
    - Make sure to add a note about using the WebView2 SDK to your application
      documentation, as required by its licence
    - With Visual Studio 2019 or newer @c wxUSE_WEBVIEW_EDGE_STATIC can be used
      to static link the loader and remove the dependency on @c WebView2Loader.dll
      at runtime.

    If enabled and available at runtime Edge will be selected as the default
    backend. If you require the IE backend use @c wxWebViewBackendIE when
    using wxWebView::New().

    If your application should use a
    <a href="https://docs.microsoft.com/en-us/microsoft-edge/webview2/concepts/distribution#fixed-version-distribution-mode">
    fixed version</a> of the WebView2 runtime you must use
    wxWebViewEdge::MSWSetBrowserExecutableDir() to specify its usage before
    using the Edge backend.

    The predefined @c wxWebViewBackendEdge constant contains the name of this
    backend.

    @subsection wxWEBVIEW_WEBKIT wxWEBVIEW_WEBKIT (GTK)

    Under GTK the WebKit backend uses
    <a href="http://webkitgtk.org/">WebKitGTK+</a>. The current minimum version
    required is 1.3.1 which ships by default with Ubuntu Natty and Debian
    Wheezy and has the package name libwebkitgtk-dev.

    Custom schemes and
    virtual files systems are supported under this backend, however embedded
    resources such as images and stylesheets are currently loaded using the
    data:// scheme.

    The predefined @c wxWebViewBackendWebKit constant contains the name of this
    backend.

    @subsection wxWEBVIEW_WEBKIT2 wxWEBVIEW_WEBKIT2 (GTK3)

    Under GTK3 the WebKit2 version of <a href="http://webkitgtk.org/">WebKitGTK+</a>
    is used. In Ubuntu the required package name is libwebkit2gtk-4.0-dev
    and under Fedora it is webkitgtk4-devel.

    All features are
    supported except for clearing and enabling / disabling the history.

    The predefined @c wxWebViewBackendWebKit constant contains the name of this
    backend.

    @subsection wxWEBVIEW_WEBKIT_MACOS wxWEBVIEW_WEBKIT (macOS)

    The macOS WebKit backend uses Apple's
    <a href="https://developer.apple.com/documentation/webkit/wkwebview">WKWebView</a>
    class.

    This backend has full support for custom schemes and virtual file
    systems on macOS 10.13+. In order to use handlers two-step creation has to be used
    and RegisterHandler() has to be called before Create().

    Starting with macOS 10.11 and iOS 9 an application cannot create unsecure
    connections (this includes HTTP and unverified HTTPS). You have to include
    additional fields in your Info.plist to enable such connections.
    For further details see the documentation on NSAppTransportSecurity
    <a target=_new href="https://developer.apple.com/documentation/bundleresources/information_property_list/nsapptransportsecurity">here</a>

    The predefined @c wxWebViewBackendWebKit constant contains the name of this
    backend.

    @section async Asynchronous Notifications

    Many of the methods in wxWebView are asynchronous, i.e. they return
    immediately and perform their work in the background. This includes
    functions such as LoadURL() and Reload(). To receive notification of the
    progress and completion of these functions you need to handle the events
    that are provided. Specifically @c wxEVT_WEBVIEW_LOADED notifies
    when the page or a sub-frame has finished loading and
    @c wxEVT_WEBVIEW_ERROR notifies that an error has occurred.

    @section vfs Virtual File Systems and Custom Schemes

    wxWebView supports the registering of custom scheme handlers, for example
    @c file or @c http. To do this create a new class which inherits from
    wxWebViewHandler, where wxWebViewHandler::GetFile() returns a pointer to a
    wxFSFile which represents the given url or wxWebViewHandler::StartRequest() for
    more complex requests. You can then register your handler
    with RegisterHandler() it will be called for all pages and resources.

    wxWebViewFSHandler is provided to access the virtual file system encapsulated by
    wxFileSystem. The wxMemoryFSHandler documentation gives an example of how this
    may be used.

    wxWebViewArchiveHandler is provided to allow the navigation of pages inside a zip
    archive. It supports paths of the form:
    @c scheme:///C:/example/docs.zip;protocol=zip/main.htm

    @beginEventEmissionTable{wxWebViewEvent}
    @event{EVT_WEBVIEW_NAVIGATING(id, func)}
       Process a @c wxEVT_WEBVIEW_NAVIGATING event, generated before trying
       to get a resource. This event may be vetoed to prevent navigating to this
       resource. Note that if the displayed HTML document has several frames, one
       such event will be generated per frame.
    @event{EVT_WEBVIEW_NAVIGATED(id, func)}
       Process a @c wxEVT_WEBVIEW_NAVIGATED event generated after it was
       confirmed that a resource would be requested. This event may not be vetoed.
       Note that if the displayed HTML document has several frames, one such event
       will be generated per frame.
    @event{EVT_WEBVIEW_LOADED(id, func)}
       Process a @c wxEVT_WEBVIEW_LOADED event generated when the document
       is fully loaded and displayed. Note that if the displayed HTML document has
       several frames, one such event will be generated per frame.
    @event{EVT_WEBVIEW_ERROR(id, func)}
       Process a @c wxEVT_WEBVIEW_ERROR event generated when a navigation
       error occurs.
       The integer associated with this event will be a wxWebNavigationError item.
       The string associated with this event may contain a backend-specific more
       precise error message/code.
    @event{EVT_WEBVIEW_NEWWINDOW(id, func)}
       Process a @c wxEVT_WEBVIEW_NEWWINDOW event, generated when a new
       window is created. You must handle this event if you want anything to
       happen, for example to load the page in a new window or tab. For usage
       details see wxWebViewWindowFeatures.
    @event{EVT_WEBVIEW_NEWWINDOW_FEATURES(id, func)}
       Process a @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES event, generated when
       window features are available for the new window. For usage
       details see wxWebViewWindowFeatures.
       only available in wxWidgets 3.3.0 or later.
    @event{EVT_WEBVIEW_WINDOW_CLOSE_REQUESTED(id, func)}
       Process a @c wxEVT_WEBVIEW_WINDOW_CLOSE_REQUESTED event, generated when
       a window is requested to be closed.
       only available in wxWidgets 3.3.0 or later.
    @event{EVT_WEBVIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_TITLE_CHANGED event, generated when
       the page title changes. Use GetString to get the title.
    @event{EVT_WEBVIEW_FULLSCREEN_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_FULLSCREEN_CHANGED event, generated when
       the page wants to enter or leave fullscreen. Use GetInt to get the status.
       Not implemented for the IE backend
       and is only available in wxWidgets 3.1.5 or later.
    @event{EVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED(id, func)}
        Process a @c wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED event
        only available in wxWidgets 3.1.5 or later. For usage details see
        AddScriptMessageHandler().
    @event{EVT_WEBVIEW_SCRIPT_RESULT(id, func)}
        Process a @c wxEVT_WEBVIEW_SCRIPT_RESULT event
        only available in wxWidgets 3.1.6 or later. For usage details see
        RunScriptAsync().
    @endEventTable

    @since 2.9.3
    @library{wxwebview}
    @category{ctrl,webview}
    @see wxWebViewHandler, wxWebViewEvent
 */
class wxWebView : public wxControl
{
public:

    /**
        Creation function for two-step creation.
    */
    virtual bool Create(wxWindow* parent,
                        wxWindowID id,
                        const wxString& url = wxWebViewDefaultURLStr,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxWebViewNameStr) = 0;

    /**
        Factory function to create a new wxWebView with two-step creation,
        wxWebView::Create should be called on the returned object.
        @param backend The backend web rendering engine to use.
                       @c wxWebViewBackendDefault, @c wxWebViewBackendIE and
                       @c wxWebViewBackendWebKit are predefined where appropriate.
        @return The created wxWebView
        @since 2.9.5
     */
    static wxWebView* New(const wxString& backend = wxWebViewBackendDefault);

    /**
        Factory function to create a new wxWebView with two-step creation,
        wxWebView::Create should be called on the returned object.

        @param config a configuration object create with NewConfiguration().
        @return The created wxWebView
        @since 3.3.0
     */
    static wxWebView* New(const wxWebViewConfiguration& config);

    /**
        Factory function to create a new wxWebView using a wxWebViewFactory.
        @param parent Parent window for the control
        @param id ID of this control
        @param url Initial URL to load
        @param pos Position of the control
        @param size Size of the control
        @param backend The backend web rendering engine to use.
                       @c wxWebViewBackendDefault, @c wxWebViewBackendIE and
                       @c wxWebViewBackendWebKit are predefined where appropriate.
        @param style
            Window style. For generic window styles, please see wxWindow.
        @param name Window name.
        @return The created wxWebView, or @NULL if the requested backend
                is not available
        @since 2.9.5
    */
    static wxWebView* New(wxWindow* parent,
                          wxWindowID id,
                          const wxString& url = wxWebViewDefaultURLStr,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          const wxString& backend = wxWebViewBackendDefault,
                          long style = 0,
                          const wxString& name = wxWebViewNameStr);

    /**
        Allows the registering of new backend for wxWebView. @a backend can be
        used as an argument to New().
        @param backend The name for the new backend to be registered under
        @param factory A shared pointer to the factory which creates the
                       appropriate backend.
        @since 2.9.5
    */
    static void RegisterFactory(const wxString& backend,
                                wxSharedPtr<wxWebViewFactory> factory);

    /**
        Allows to check if a specific backend is currently available.

        For example, to check for Edge backend availability:
        @code
        if ( wxWebView::IsBackendAvailable(wxWebViewBackendEdge) )
        {
            ... enable some extra functionality not available with the IE backend ...
        }
        @endcode

        @since 3.1.4
    */
    static bool IsBackendAvailable(const wxString& backend);

    /**
        Retrieve the version information about the backend implementation.

        @since 3.1.5
    */
    static wxVersionInfo GetBackendVersionInfo(const wxString& backend = wxWebViewBackendDefault);

    /**
        Create a new wxWebViewConfiguration object.

        @since 3.3.0
    */
    static wxWebViewConfiguration NewConfiguration(const wxString& backend = wxWebViewBackendDefault);

    /**
        Get the title of the current web page, or its URL/path if title is not
        available.
    */
    virtual wxString GetCurrentTitle() const = 0;

   /**
        Get the URL of the currently displayed document.
    */
    virtual wxString GetCurrentURL() const = 0;

    /**
        Return the pointer to the native backend used by this control.

        This method can be used to retrieve the pointer to the native rendering
        engine used by this control. The return value needs to be down-casted
        to the appropriate type depending on the platform: under Windows, it's
        a pointer to IWebBrowser2 interface, under macOS it's a WebView pointer
        and under GTK it's a WebKitWebView.

        For example, you could set the WebKit options using this method:
        @code
            #include <webkit/webkit.h>

            #ifdef __WXGTK__
               WebKitWebView*
                wv = static_cast<WebKitWebView*>(m_window->GetNativeBackend());

               WebKitWebSettings* settings = webkit_web_view_get_settings(wv);
               g_object_set(G_OBJECT(settings),
                            "enable-frame-flattening", TRUE,
                            nullptr);
            #endif
        @endcode

        @since 2.9.5
     */
    virtual void* GetNativeBackend() const = 0;

    /**
        Get the HTML source code of the currently displayed document.
        @return The HTML source code, or an empty string if no page is currently
                shown.
    */
    virtual wxString GetPageSource() const;

    /**
        Get the text of the current page.
    */
    virtual wxString GetPageText() const;

    /**
        Returns whether the web control is currently busy (e.g.\ loading a page).
    */
    virtual bool IsBusy() const = 0;

    /**
        Returns whether the web control is currently editable
    */
    virtual bool IsEditable() const = 0;

    /**
        Load a web page from a URL
        @param url The URL of the page to be loaded.
        @note Web engines generally report errors asynchronously, so if you wish
            to know whether loading the URL was successful, register to receive
            navigation error events.
    */
    virtual void LoadURL(const wxString& url) = 0;

    /**
        Opens a print dialog so that the user may print the currently
        displayed page.
    */
    virtual void Print() = 0;

    /**
        Registers a custom scheme handler.
        @param handler A shared pointer to a wxWebHandler.
        @note On macOS in order to use handlers two-step creation has to be
              used and RegisterHandler() has to be called before Create().
              With the other backends it has to be called after Create().

        @note The Edge backend does not support custom schemes, but the
              handler is available as a virtual host under
              %https://scheme.wxsite to customize this virtual host call
              wxWebViewHandler::SetVirtualHost() before registering the
              handler.
    */
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) = 0;

    /**
        Reload the currently displayed URL.
        @param flags A bit array that may optionally contain reload options.
        @note The flags are ignored by the Edge backend.
    */
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) = 0;

    /**
        Set the editable property of the web control. Enabling allows the user
        to edit the page even if the @c contenteditable attribute is not set.
        The exact capabilities vary with the backend being used.

        @note This is not implemented for macOS and the Edge backend.
    */
    virtual void SetEditable(bool enable = true) = 0;

    /**
        Set the displayed page source to the contents of the given string.
        @param html    The string that contains the HTML data to display.
        @param baseUrl URL assigned to the HTML data, to be used to resolve
                    relative paths, for instance.
        @note When using @c wxWEBVIEW_BACKEND_IE you must wait for the current
              page to finish loading before calling SetPage(). The baseURL
              parameter is not used in this backend and the Edge backend.
    */
    virtual void SetPage(const wxString& html, const wxString& baseUrl) = 0;

    /**
        Set the displayed page source to the contents of the given stream.
        @param html    The stream to read HTML data from.
        @param baseUrl URL assigned to the HTML data, to be used to resolve
                    relative paths, for instance.
    */
    virtual void SetPage(wxInputStream& html, wxString baseUrl);

    /**
        Stop the current page loading process, if any.
        May trigger an error event of type @c wxWEBVIEW_NAV_ERR_USER_CANCELLED.
        TODO: make @c wxWEBVIEW_NAV_ERR_USER_CANCELLED errors uniform across ports.
    */
    virtual void Stop() = 0;

    /**
        Specify a custom user agent string for the web view.
        Returns @true the user agent could be set.

        If your first request should already use the custom user agent
        please use two step creation and call SetUserAgent() before Create().

        @note This is not implemented for IE.

        @since 3.1.5
    */
    virtual bool SetUserAgent(const wxString& userAgent);

    /**
        Returns the current user agent string for the web view.

        @since 3.1.5
    */
    virtual wxString GetUserAgent() const;

    /**
        Set the proxy to use for all requests.

        The @a proxy string must be a valid proxy specification, e.g. @c
        http://my.local.proxy.corp:8080

        @note Currently this function is only implemented in WebKit2 and Edge
            backends and must be called before Create() for the latter one.

        @return @true if proxy was set successfully or @false if it failed,
            e.g. because this is not supported by the currently used backend.
     */
    virtual bool SetProxy(const wxString& proxy);

    /**
        @name Scripting
    */
    /**
        Runs the given JavaScript code.

        @note Because of various potential issues it's recommended to use
            RunScriptAsync() instead of this method. This is especially true
            if you plan to run code from a webview event and will also prevent
            unintended side effects on the UI outside of the webview.

        JavaScript code is executed inside the browser control and has full
        access to DOM and other browser-provided functionality. For example,
        this code
        @code
            webview->RunScript("document.write('Hello from wxWidgets!')");
        @endcode
        will replace the current page contents with the provided string.

        If @a output is non-null, it is filled with the result of executing
        this code on success, e.g. a JavaScript value such as a string, a
        number (integer or floating point), a boolean or JSON representation
        for non-primitive types such as arrays and objects. For example:
        @code
            wxString result;
            if ( webview->RunScript
                          (
                            "document.getElementById('some_id').innerHTML",
                            &result
                          ) )
            {
                ... result contains the contents of the given element ...
            }
            //else: the element with this ID probably doesn't exist.
        @endcode

        This function has a few platform-specific limitations:

        - When using WebKit v1 in wxGTK2, retrieving the result of JavaScript
          execution is unsupported and this function will always return false
          if @a output is non-null to indicate this. This functionality is
          fully supported when using WebKit v2 or later in wxGTK3.

        - When using WebKit under macOS, code execution is limited to at most
          10MiB of memory and 10 seconds of execution time.

        - When using IE backend under MSW, scripts can only be executed when
          the current page is fully loaded (i.e. @c wxEVT_WEBVIEW_LOADED event
          was received). A script tag inside the page HTML is required in order
          to run JavaScript.

        Also notice that under MSW converting JavaScript objects to JSON is not
        supported in the default emulation mode. wxWebView implements its own
        object-to-JSON conversion as a fallback for this case, however it is
        not as full-featured, well-tested or performing as the implementation
        of this functionality in the browser control itself, so it is
        recommended to use MSWSetEmulationLevel() to change emulation
        level to a more modern one in which JSON conversion is done by the
        control itself.

        @param javascript JavaScript code to execute.
        @param output Pointer to a string to be filled with the result value or
            @NULL if it is not needed. This parameter is new since wxWidgets
            version 3.1.1.
        @return @true if there is a result, @false if there is an error.

        @see RunScriptAsync()
    */
    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const = 0;

    /**
        Runs the given JavaScript code asynchronously and returns the result
        via a @c wxEVT_WEBVIEW_SCRIPT_RESULT.

        The script result value can be retrieved via wxWebViewEvent::GetString().
        If the execution fails wxWebViewEvent::IsError() will return @true. In this
        case additional script execution error information maybe available
        via wxWebViewEvent::GetString().

        @param javascript JavaScript code to execute.
        @param clientData Arbirary pointer to data that can be retrieved from
            the result event.

        @note The IE backend does not support async script execution.

        @since 3.1.6
        @see RunScript()
    */
    virtual void RunScriptAsync(const wxString& javascript, void* clientData = nullptr) const;


    /**
        Add a script message handler with the given name.

        To use the script message handler from javascript use
        @c `window.<name>.postMessage(<messageBody>)` where `<name>` corresponds the value
        of the name parameter. The `<messageBody>` will be available to the application
        via a @c wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED event.

        Sample C++ code receiving a script message:
        @code
            // Install message handler with the name wx_msg
            m_webView->AddScriptMessageHandler('wx_msg');
            // Bind handler
            m_webView->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, [](wxWebViewEvent& evt) {
                wxLogMessage("Script message received; value = %s, handler = %s", evt.GetString(), evt.GetMessageHandler());
            });
        @endcode

        Sample JavaScript sending a script message:
        @code
            // Send sample message body
            window.wx_msg.postMessage('This is a message body');
        @endcode

        @param name Name of the message handler that can be used from JavaScript
        @return @true if the handler could be added, @false if it could not be added.

        @see RemoveScriptMessageHandler()

        @note The Edge backend only supports a single message handler and
            the IE backend does not support script message handlers.

        @since 3.1.5
    */
    virtual bool AddScriptMessageHandler(const wxString& name);

    /**
        Remove a script message handler with the given name that was previously added via
        AddScriptMessageHandler().

        @return @true if the handler could be removed, @false if it could not be removed.

        @see AddScriptMessageHandler()

        @since 3.1.5
    */
    virtual bool RemoveScriptMessageHandler(const wxString& name);

    /**
        Injects the specified script into the webpage's content.

        @param javascript The JavaScript code to add.
        @param injectionTime Specifies when the script will be executed.
        @return Returns true if the script was added successfully.

        @note Please note that this is unsupported by the IE backend and
            the Edge backend does only support wxWEBVIEW_INJECT_AT_DOCUMENT_START.

        @see RemoveAllUserScripts()

        @since 3.1.5
    */
    virtual bool AddUserScript(const wxString& javascript,
        wxWebViewUserScriptInjectionTime injectionTime = wxWEBVIEW_INJECT_AT_DOCUMENT_START);

    /**
        Removes all user scripts from the web view.

        @see AddUserScript()

        @since 3.1.5
    */
    virtual void RemoveAllUserScripts();

    /**
        @name Clipboard
    */

    /**
        Returns @true if the current selection can be copied.
    */
    virtual bool CanCopy() const;

    /**
        Returns @true if the current selection can be cut.
    */
    virtual bool CanCut() const;

    /**
        Returns @true if data can be pasted.
    */
    virtual bool CanPaste() const;

    /**
        Copies the current selection.
    */
    virtual void Copy();

    /**
        Cuts the current selection.
    */
    virtual void Cut();

    /**
        Pastes the current data.
    */
    virtual void Paste();

    /**
        @name Settings
    */

    /**
        Enable or disable the right click context menu.

        By default the standard context menu is enabled, this method can be
        used to disable it or re-enable it later.

        @since 2.9.5
    */
    virtual void EnableContextMenu(bool enable = true);

   /**
        Returns @true if a context menu will be shown on right click.

        @since 2.9.5
    */
    virtual bool IsContextMenuEnabled() const;

    /**
        Enable or disable access to dev tools for the user.

        Dev tools are disabled by default.

        @note This is not implemented for the IE backend.

        @since 3.1.4
    */
    virtual void EnableAccessToDevTools(bool enable = true);

    /**
        Returns @true if dev tools are available to the user.

        @since 3.1.4
    */
    virtual bool IsAccessToDevToolsEnabled() const;

    /**
        Enable or disable if browser accelerator keys are enabled.

        Browser accelerator keys are enabled by default.

        The webview might implement various accelerator keys. This includes
        allowing accelerator key access to features such as printing and
        navigation. In many cases this might not be desired and the default
        keys can be disabled with this setting.

        @note This is only implemented for the Edge backend.

        @since 3.3.0
    */
    virtual void EnableBrowserAcceleratorKeys(bool enable = true);

    /**
        Returns @true if browser accelerator keys are enabled.

        @since 3.3.0
    */
    virtual bool AreBrowserAcceleratorKeysEnabled() const;


    /**
        @name History
    */

    /**
        Returns @true if it is possible to navigate backward in the history of
        visited pages.
    */
    virtual bool CanGoBack() const = 0;

    /**
        Returns @true if it is possible to navigate forward in the history of
        visited pages.
    */
    virtual bool CanGoForward() const = 0;

    /**
        Clear the history, this will also remove the visible page.

        @note This is not implemented on the WebKit2GTK+ backend and macOS.
    */
    virtual void ClearHistory() = 0;

    /**
        Enable or disable the history. This will also clear the history.

        @note This is not implemented on the WebKit2GTK+ backend and macOS.
    */
    virtual void EnableHistory(bool enable = true) = 0;

    /**
        Returns a list of items in the back history. The first item in the
        vector is the first page that was loaded by the control.
    */
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() = 0;

    /**
        Returns a list of items in the forward history. The first item in the
        vector is the next item in the history with respect to the currently
        loaded page.
    */
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() = 0;

    /**
        Navigate back in the history of visited pages.
        Only valid if CanGoBack() returns true.
    */
    virtual void GoBack() = 0;

    /**
        Navigate forward in the history of visited pages.
        Only valid if CanGoForward() returns true.
    */
    virtual void GoForward() = 0;

    /**
        Loads a history item.
    */
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) = 0;

    /**
        @name Selection
    */

    /**
        Clears the current selection.
    */
    virtual void ClearSelection();

    /**
        Deletes the current selection. Note that for @c wxWEBVIEW_BACKEND_WEBKIT
        the selection must be editable, either through SetEditable or the
        correct HTML attribute.
    */
    virtual void DeleteSelection();

    /**
        Returns the currently selected source, if any.
    */
    virtual wxString GetSelectedSource() const;

    /**
        Returns the currently selected text, if any.
    */
    virtual wxString GetSelectedText() const;

    /**
        Returns @true if there is a current selection.
    */
    virtual bool HasSelection() const;

    /**
        Selects the entire page.
    */
    virtual void SelectAll();

    /**
        @name Undo / Redo
    */

    /**
        Returns @true if there is an action to redo.
    */
    virtual bool CanRedo() const = 0;

    /**
        Returns @true if there is an action to undo.
    */
    virtual bool CanUndo() const = 0;

    /**
        Redos the last action.
    */
    virtual void Redo() = 0;

    /**
        Undos the last action.
    */
    virtual void Undo() = 0;

    /**
        @name Finding
    */

    /**
        Finds a phrase on the current page and if found, the control will
        scroll the phrase into view and select it.
        @param text The phrase to search for.
        @param flags The flags for the search.
        @return If search phrase was not found in combination with the flags
                then @c wxNOT_FOUND is returned. If called for the first time
                with search phrase then the total number of results will be
                returned. Then for every time its called with the same search
                phrase it will return the number of the current match.
        @note This function will restart the search if the flags
              @c wxWEBVIEW_FIND_ENTIRE_WORD or @c wxWEBVIEW_FIND_MATCH_CASE
              are changed, since this will require a new search. To reset the
              search, for example resetting the highlights call the function
              with an empty search phrase.
        @since 2.9.5
    */
    virtual long Find(const wxString& text, wxWebViewFindFlags flags = wxWEBVIEW_FIND_DEFAULT);

    /**
        @name Zoom
    */

    /**
        Retrieve whether the current HTML engine supports a zoom type.
        @param type The zoom type to test.
        @return Whether this type of zoom is supported by this HTML engine
                (and thus can be set through SetZoomType()).
    */
    virtual bool CanSetZoomType(wxWebViewZoomType type) const = 0;

    /**
        Get the zoom level of the page.
        See GetZoomFactor() to get more precise zoom scale value other than
        as provided by @c wxWebViewZoom.
        @return The current level of zoom.
    */
    virtual wxWebViewZoom GetZoom() const;

    /**
        Get the zoom factor of the page.
        @return The current factor of zoom.
        @since 3.1.4
    */
    virtual float GetZoomFactor() const = 0;

    /**
        Get how the zoom factor is currently interpreted.
        @return How the zoom factor is currently interpreted by the HTML engine.
    */
    virtual wxWebViewZoomType GetZoomType() const = 0;

    /**
        Set the zoom level of the page.
        See SetZoomFactor() for more precise scaling other than the measured
        steps provided by @c wxWebViewZoom.
        @param zoom How much to zoom (scale) the HTML document.
    */
    virtual void SetZoom(wxWebViewZoom zoom);

    /**
        Set the zoom factor of the page.
        @param zoom How much to zoom (scale) the HTML document in arbitrary
                    number.
        @note zoom  scale in IE will be converted into @c wxWebViewZoom levels
                    for @c wxWebViewZoomType of @c wxWEBVIEW_ZOOM_TYPE_TEXT.
        @since 3.1.4
    */
    virtual void SetZoomFactor(float zoom) = 0;

        /**
        Set how to interpret the zoom factor.
        @param zoomType How the zoom factor should be interpreted by the
                        HTML engine.
        @note invoke    CanSetZoomType() first, some HTML renderers may not
                        support all zoom types.
    */
    virtual void SetZoomType(wxWebViewZoomType zoomType) = 0;
};



/**
    @class wxWebViewIE

    wxWebView using IE backend, see @ref wxWEBVIEW_BACKEND_IE.

    @onlyfor{wxmsw}
    @since 2.9.3
    @library{wxwebview}
    @category{ctrl,webview}
    @see wxWebView
 */
class wxWebViewIE : public wxWebView
{
public:
    /**
        Sets emulation level.

        This function is useful to change the emulation level of
        the system browser control used for wxWebView implementation under
        MSW, rather than using the currently default, IE7-compatible, level.

        Please notice that this function works by modifying the per-user part
        of MSW registry, which has several implications: first, it is
        sufficient to call it only once (per user) as the changes done by it
        are persistent and, second, if you do not want them to be persistent,
        you need to call it with @c wxWEBVIEWIE_EMU_DEFAULT argument explicitly.

        In particular, this function should be called to allow RunScript() to
        work for JavaScript code returning arbitrary objects, which is not
        supported at the default emulation level.

        If set to a level higher than installed version, the highest available
        level will be used instead. @c wxWEBVIEWIE_EMU_IE11 is recommended for
        best performance and experience.

        This function is MSW-specific and doesn't exist under other platforms.

        See https://msdn.microsoft.com/en-us/library/ee330730#browser_emulation
        for more information about browser control emulation levels.

        @param level the target emulation level
        @return @true on success, @false on failure (a warning message is also
        logged in the latter case).

        @since 3.1.3
    */
    static bool MSWSetEmulationLevel(wxWebViewIE_EmulationLevel level = wxWEBVIEWIE_EMU_IE11);

    /**
        @deprecated
        This function is kept mostly for backwards compatibility.

        Please explicitly specify emulation level with MSWSetEmulationLevel().

        @param modernLevel @true to set level to IE8, synonym for @c wxWEBVIEWIE_EMU_IE8.
            @false to reset the emulation level to its default,
            synonym for @c wxWEBVIEWIE_EMU_DEFAULT.
        @return @true on success, @false on failure (a warning message is also
            logged in the latter case).

        @since 3.1.1
    */
    static bool MSWSetModernEmulationLevel(bool modernLevel = true);
};

/**
    @class wxWebViewEdge

    wxWebView using Edge backend, see @ref wxWEBVIEW_BACKEND_EDGE.

    @onlyfor{wxmsw}
    @since 3.1.5
    @library{wxwebview}
    @category{ctrl,webview}
    @see wxWebView
 */
class wxWebViewEdge : public wxWebView
{
public:
    /**
        Set path to a fixed version of the WebView2 Edge runtime.

        @param path Path to an extracted fixed version of the WebView2 Edge runtime.

        @since 3.1.5
    */
    static void MSWSetBrowserExecutableDir(const wxString& path);
};


/**
    @class wxWebViewEvent

    A navigation  event holds information about events associated with
    wxWebView objects.

    @beginEventEmissionTable{wxWebViewEvent}
    @event{EVT_WEBVIEW_NAVIGATING(id, func)}
       Process a @c wxEVT_WEBVIEW_NAVIGATING event, generated before trying
       to get a resource. This event may be vetoed to prevent navigating to this
       resource. Note that if the displayed HTML document has several frames, one
       such event will be generated per frame.
    @event{EVT_WEBVIEW_NAVIGATED(id, func)}
       Process a @c wxEVT_WEBVIEW_NAVIGATED event generated after it was
       confirmed that a resource would be requested. This event may not be vetoed.
       Note that if the displayed HTML document has several frames, one such event
       will be generated per frame.
    @event{EVT_WEBVIEW_LOADED(id, func)}
       Process a @c wxEVT_WEBVIEW_LOADED event generated when the document
       is fully loaded and displayed. Note that if the displayed HTML document has
       several frames, one such event will be generated per frame.
    @event{EVT_WEBVIEW_ERROR(id, func)}
       Process a @c wxEVT_WEBVIEW_ERROR event generated when a navigation
       error occurs.
       The integer associated with this event will be a #wxWebViewNavigationError item.
       The string associated with this event may contain a backend-specific more
       precise error message/code.
    @event{EVT_WEBVIEW_NEWWINDOW(id, func)}
       Process a @c wxEVT_WEBVIEW_NEWWINDOW event, generated when a new
       window is created. You must handle this event if you want anything to
       happen, for example to load the page in a new window or tab. For usage
       details see wxWebViewWindowFeatures.
    @event{EVT_WEBVIEW_NEWWINDOW_FEATURES(id, func)}
       Process a @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES event, generated when
       window features are available for the new window. For usage
       details see wxWebViewWindowFeatures.
       only available in wxWidgets 3.3.0 or later.
    @event{EVT_WEBVIEW_WINDOW_CLOSE_REQUESTED(id, func)}
       Process a @c wxEVT_WEBVIEW_WINDOW_CLOSE_REQUESTED event, generated when
       a window is requested to be closed.
       only available in wxWidgets 3.3.0 or later.
    @event{EVT_WEBVIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_TITLE_CHANGED event, generated when
       the page title changes. Use GetString to get the title.
    @event{EVT_WEBVIEW_FULLSCREEN_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_FULLSCREEN_CHANGED event, generated when
       the page wants to enter or leave fullscreen. Use GetInt to get the status.
       Not implemented for the IE backend
       and is only available in wxWidgets 3.1.5 or later.
    @event{EVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED(id, func)}
        Process a @c wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED event
        only available in wxWidgets 3.1.5 or later. For usage details see
        wxWebView::AddScriptMessageHandler().
    @event{EVT_WEBVIEW_SCRIPT_RESULT(id, func)}
        Process a @c wxEVT_WEBVIEW_SCRIPT_RESULT event
        only available in wxWidgets 3.1.6 or later. For usage details see
        wxWebView::RunScriptAsync().
    @endEventTable

    @since 2.9.3
    @library{wxwebview}
    @category{events,webview}

    @see wxWebView
*/
class wxWebViewEvent : public wxNotifyEvent
{
public:
    wxWebViewEvent();
    wxWebViewEvent(wxEventType type, int id, const wxString href,
                   const wxString target,
                   wxWebViewNavigationActionFlags flags = wxWEBVIEW_NAV_ACTION_NONE,
                   const wxString& messageHandler = wxString());

    /**
        Get the name of the target frame which the url of this event
        has been or will be loaded into. This may return an empty string
        if the frame is not available.
    */
    const wxString& GetTarget() const;

    /**
        Get the URL being visited
    */
    const wxString& GetURL() const;

    /**
        Get the type of navigation action. Only valid for events of type
        @c wxEVT_WEBVIEW_NEWWINDOW

        @since 3.1.2
    */
    wxWebViewNavigationActionFlags GetNavigationAction() const;

    /**
        Get the name of the script handler. Only valid for events of type
        @c wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED

        @since 3.1.5
    */
    const wxString& GetMessageHandler() const;

    /**
        Get information about the target window. Only valid for events of type
        @c wxEVT_WEBVIEW_NEWWINDOW_FEATURES

        @note This function is not implemented and always returns @NULL when using WebKit1 or Internet Explorer backend.

        @see wxWebViewWindowFeatures
        @since 3.3.0
    */
    wxWebViewWindowFeatures* GetTargetWindowFeatures() const;

    /**
        Returns true the script execution failed. Only valid for events of type
        @c wxEVT_WEBVIEW_SCRIPT_RESULT

        @since 3.1.6
    */
    bool IsError() const;

    /**
        Returns true if the navigation target is the main frame. Only valid
        for events of type @c wxEVT_WEBVIEW_NAVIGATING

        @note This is only available with the macOS and the Edge backend.

        @since 3.3.0
    */
    bool IsTargetMainFrame() const;
};


wxEventType wxEVT_WEBVIEW_NAVIGATING;
wxEventType wxEVT_WEBVIEW_NAVIGATED;
wxEventType wxEVT_WEBVIEW_LOADED;
wxEventType wxEVT_WEBVIEW_ERROR;
wxEventType wxEVT_WEBVIEW_NEWWINDOW;
wxEventType wxEVT_WEBVIEW_NEWWINDOW_FEATURES;
wxEventType wxEVT_WEBVIEW_TITLE_CHANGED;
wxEventType wxEVT_WEBVIEW_FULLSCREEN_CHANGED;
wxEventType wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED;
wxEventType wxEVT_WEBVIEW_SCRIPT_RESULT;
wxEventType wxEVT_WEBVIEW_WINDOW_CLOSE_REQUESTED;
