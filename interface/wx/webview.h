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
        Construtor.
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
};

/**
    @class wxWebViewHandler

    The base class for handling custom schemes in wxWebView, for example to
    allow virtual file system support.

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
    virtual wxFSFile* GetFile(const wxString &uri) = 0;

    /**
        @return The name of the scheme, as passed to the constructor.
    */
    virtual wxString GetName() const;
};

/**
    @class wxWebView

    This control may be used to render web (HTML / CSS / javascript) documents.
    It is designed to allow the creation of multiple backends for each port,
    although currently just one is available. It differs from wxHtmlWindow in
    that each backend is actually a full rendering engine, Trident on MSW and
    Webkit on OS X and GTK. This allows the correct viewing of complex pages with
    javascript and css.

    @section descriptions Backend Descriptions

    @par wxWEBVIEW_BACKEND_IE (MSW)

    The IE backend uses Microsoft's Trident rendering engine, specifically the
    version used by the locally installed copy of Internet Explorer. As such it
    is only available for the MSW port. By default recent versions of the
    <a href="http://msdn.microsoft.com/en-us/library/aa752085%28v=VS.85%29.aspx">WebBrowser</a>
    control, which this backend uses, emulate Internet Explorer 7. This can be
    changed with a registry setting, see
    <a href="http://msdn.microsoft.com/en-us/library/ee330730%28v=vs.85%29.aspx#browser_emulation">
    this</a> article for more information. This backend has full support for
    custom schemes and virtual file systems.

    @par wxWEBVIEW_WEBKIT (GTK)

    Under GTK the WebKit backend uses
    <a href="http://webkitgtk.org/">WebKitGTK+</a>. The current minimum version
    required is 1.3.1 which ships by default with Ubuntu Natty and Debian
    Wheezy and has the package name libwebkitgtk-dev. Custom schemes and
    virtual files systems are supported under this backend, however embedded
    resources such as images and stylesheets are currently loaded using the
    data:// scheme.

    @par wxWEBVIEW_WEBKIT (OSX)

    The OS X WebKit backend uses Apple's
    <a href="http://developer.apple.com/library/mac/#documentation/Cocoa/Reference/WebKit/Classes/WebView_Class/Reference/Reference.html#//apple_ref/doc/uid/20001903">WebView</a>
    class. This backend has full support for custom schemes and virtual file
    systems.

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
    wxWebViewHandler, where wxWebHandler::GetFile() returns a pointer to a
    wxFSFile which represents the given url. You can then register your handler
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
       happen, for example to load the page in a new window or tab.
    @event{EVT_WEBVIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_TITLE_CHANGED event, generated when
       the page title changes. Use GetString to get the title.
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
        @return The created wxWebView, or @c NULL if the requested backend
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
        a pointer to IWebBrowser2 interface, under OS X it's a WebView pointer
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
                            NULL);
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
    virtual wxString GetPageSource() const = 0;

    /**
        Get the text of the current page.
    */
    virtual wxString GetPageText() const = 0;

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
    */
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) = 0;

    /**
        Reload the currently displayed URL.
        @param flags A bit array that may optionally contain reload options.
    */
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) = 0;

    /**
        Runs the given javascript code.
        @note When using wxWEBVIEW_BACKEND_IE you must wait for the current
              page to finish loading before calling RunScript().
    */
    virtual void RunScript(const wxString& javascript) = 0;

    /**
        Set the editable property of the web control. Enabling allows the user
        to edit the page even if the @c contenteditable attribute is not set.
        The exact capabilities vary with the backend being used.
    */
    virtual void SetEditable(bool enable = true) = 0;

    /**
        Set the displayed page source to the contents of the given string.
        @param html    The string that contains the HTML data to display.
        @param baseUrl URL assigned to the HTML data, to be used to resolve
                    relative paths, for instance.
        @note When using @c wxWEBVIEW_BACKEND_IE you must wait for the current
              page to finish loading before calling SetPage(). The baseURL
              parameter is not used in this backend.
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
        @name Clipboard
    */

    /**
        Returns @true if the current selection can be copied.

        @note This always returns @c true on the OS X WebKit backend.
    */
    virtual bool CanCopy() const = 0;

    /**
        Returns @true if the current selection can be cut.

         @note This always returns @c true on the OS X WebKit backend.
    */
    virtual bool CanCut() const = 0;

    /**
        Returns @true if data can be pasted.

        @note This always returns @c true on the OS X WebKit backend.
    */
    virtual bool CanPaste() const = 0;

    /**
        Copies the current selection.
    */
    virtual void Copy() = 0;

    /**
        Cuts the current selection.
    */
    virtual void Cut() = 0;

    /**
        Pastes the current data.
    */
    virtual void Paste() = 0;

    /**
        @name Context Menu
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
    */
    virtual void ClearHistory() = 0;

    /**
        Enable or disable the history. This will also clear the history.
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
    virtual void ClearSelection() = 0;

    /**
        Deletes the current selection. Note that for @c wxWEBVIEW_BACKEND_WEBKIT
        the selection must be editable, either through SetEditable or the
        correct HTML attribute.
    */
    virtual void DeleteSelection() = 0;

    /**
        Returns the currently selected source, if any.
    */
    virtual wxString GetSelectedSource() const = 0;

    /**
        Returns the currently selected text, if any.
    */
    virtual wxString GetSelectedText() const = 0;

    /**
        Returns @true if there is a current selection.
    */
    virtual bool HasSelection() const = 0;

    /**
        Selects the entire page.
    */
    virtual void SelectAll() = 0;

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
              with an empty search phrase. This always returns @c wxNOT_FOUND
              on the OS X WebKit backend.
        @since 2.9.5
    */
    virtual long Find(const wxString& text, wxWebViewFindFlags flags = wxWEBVIEW_FIND_DEFAULT) = 0;

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
        Get the zoom factor of the page.
        @return The current level of zoom.
    */
    virtual wxWebViewZoom GetZoom() const = 0;

    /**
        Get how the zoom factor is currently interpreted.
        @return How the zoom factor is currently interpreted by the HTML engine.
    */
    virtual wxWebViewZoomType GetZoomType() const = 0;

    /**
        Set the zoom factor of the page.
        @param zoom How much to zoom (scale) the HTML document.
    */
    virtual void SetZoom(wxWebViewZoom zoom) = 0;

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
       happen, for example to load the page in a new window or tab.
    @event{EVT_WEBVIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_WEBVIEW_TITLE_CHANGED event, generated when
       the page title changes. Use GetString to get the title.
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
                   const wxString target);

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
};


wxEventType wxEVT_WEBVIEW_NAVIGATING;
wxEventType wxEVT_WEBVIEW_NAVIGATED;
wxEventType wxEVT_WEBVIEW_LOADED;
wxEventType wxEVT_WEBVIEW_ERROR;
wxEventType wxEVT_WEBVIEW_NEWWINDOW;
wxEventType wxEVT_WEBVIEW_TITLE_CHANGED;
