/////////////////////////////////////////////////////////////////////////////
// Name:        webview.h
// Purpose:     interface of wxWebView
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Zoom levels availiable in wxWebView
*/
enum wxWebViewZoom
{
    wxWEB_VIEW_ZOOM_TINY,
    wxWEB_VIEW_ZOOM_SMALL,
    wxWEB_VIEW_ZOOM_MEDIUM, //!< default size
    wxWEB_VIEW_ZOOM_LARGE,
    wxWEB_VIEW_ZOOM_LARGEST
};

/**
    The type of zooming that the web view control can perform
*/
enum wxWebViewZoomType
{
    /** 
        The entire layout scales when zooming, including images 
    */
    wxWEB_VIEW_ZOOM_TYPE_LAYOUT,
    /** 
        Only the text changes in size when zooming, images and other layout
        elements retain their initial size 
    */
    wxWEB_VIEW_ZOOM_TYPE_TEXT
};

/** 
    Types of errors that can cause navigation to fail
*/
enum wxWebNavigationError
{
    /** Connection error (timeout, etc.) */
    wxWEB_NAV_ERR_CONNECTION,
    /** Invalid certificate */
    wxWEB_NAV_ERR_CERTIFICATE,
    /** Authentication required */
    wxWEB_NAV_ERR_AUTH,
    /** Other security error */
    wxWEB_NAV_ERR_SECURITY,
    /** Requested resource not found */
    wxWEB_NAV_ERR_NOT_FOUND,
    /** Invalid request/parameters (e.g. bad URL, bad protocol,
        unsupported resource type) */
    wxWEB_NAV_ERR_REQUEST,
    /** The user cancelled (e.g. in a dialog) */
    wxWEB_NAV_ERR_USER_CANCELLED,
    /** Another (exotic) type of error that didn't fit in other categories*/
    wxWEB_NAV_ERR_OTHER
};

/** 
    Type of refresh 
*/
enum wxWebViewReloadFlags
{
    /** Default reload, will access cache */
    wxWEB_VIEW_RELOAD_DEFAULT,
    /** Reload the current view without accessing the cache */
    wxWEB_VIEW_RELOAD_NO_CACHE 
};


/**
 * List of available backends for wxWebView
 */
enum wxWebViewBackend
{
    /** Value that may be passed to wxWebView to let it pick an appropriate
     * engine for the current platform*/
    wxWEB_VIEW_BACKEND_DEFAULT,

    /** The WebKit web engine */
    wxWEB_VIEW_BACKEND_WEBKIT,

    /** Use Microsoft Internet Explorer as web engine */
    wxWEB_VIEW_BACKEND_IE
};

/**
    @class wxWebHistoryItem
  
    A simple class that contains the URL and title of an element of the history
    of a wxWebView. 
   
    @library{wxweb}
    @category{web}
    
    @see wxWebView
 */
class wxWebHistoryItem
{
public:
    /**
        Construtor.
    */
    wxWebHistoryItem(const wxString& url, const wxString& title);
    
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
    @class wxWebHandler
  
    The base class for handling custom schemes in wxWebView, for example to 
    allow virtual file system support.
   
    @library{wxweb}
    @category{web}
    
    @see wxWebView
 */
class wxWebHandler
{
public:
    /**
        @return A pointer to the file represented by @c uri.
    */  
    virtual wxFSFile* GetFile(const wxString &uri) = 0;

    /**
        @return The name of the scheme, for example @c file or @c http.
    */
    virtual wxString GetName() const = 0;
};

/**
    @class wxWebView
  
    This control may be used to render web (HTML / CSS / javascript) documents.
    Capabilities of the HTML renderer will depend upon the backed.

    @c wxWEB_VIEW_BACKEND_IE uses the the Trident rendering engine, which 
    is also used by Internet Explorer. It is important to note that by default 
    it emulates Internet Explorer 7, this can be chaged with a registry 
    setting, see 
    <a href="http://msdn.microsoft.com/en-us/library/ee330730%28v=vs.85%29.aspx#browser_emulation">
    this</a> article for more information.
  
    Note that errors are generally reported asynchronously though the
    @c wxEVT_COMMAND_WEB_VIEW_ERROR event described below.
    
    @section vfs Virtual File Systems and Custom Schemes
    
    wxWebView supports the registering of custom scheme handlers, for example
    @c file or @c http. To do this create a new class which inherits from 
    wxWebHandler, where the wxWebHandler::GetName() method returns the scheme
    you wish to handle and wxWebHandler::GetFile() returns a pointer to a 
    wxFSFile which represents the given url. You can then register your handler
    with RegisterHandler() it will be called for all pages and resources.
    
    wxWebFileHandler is provided to allow the navigation of pages inside a zip
    archive. It overrides the @c file scheme and provides support for the 
    standard @c file syntax as well as paths to archives of the form 
    @c file:///C:/exmaple/docs.zip;protocol=zip/main.htm 
  
    @beginEventEmissionTable{wxWebNavigationEvent}
    @event{EVT_WEB_VIEW_NAVIGATING(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NAVIGATING event, generated before trying
       to get a resource. This event may be vetoed to prevent navigating to this
       resource. Note that if the displayed HTML document has several frames, one
       such event will be generated per frame.
    @event{EVT_WEB_VIEW_NAVIGATED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NAVIGATED event generated after it was
       confirmed that a resource would be requested. This event may not be vetoed.
       Note that if the displayed HTML document has several frames, one such event
       will be generated per frame.
    @event{EVT_WEB_VIEW_LOADED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_LOADED event generated when the document
       is fully loaded and displayed. Note that if the displayed HTML document has 
       several frames, one such event will be generated per frame.
    @event{EVT_WEB_VIEW_ERRROR(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_ERROR event generated when a navigation
       error occurs.
       The integer associated with this event will be a wxWebNavigationError item.
       The string associated with this event may contain a backend-specific more
       precise error message/code.
    @event{EVT_WEB_VIEW_NEWWINDOW(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NEWWINDOW event, generated when a new
       window is created. You must handle this event if you want anything to 
       happen, for example to load the page in a new window or tab.
    @event{EVT_WEB_VIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_TITLE_CHANGED event, generated when 
       the page title changes. Use GetString to get the title.
    @endEventTable
   
    @library{wxweb}
    @category{ctrl,web}
    @see wxWebHandler, wxWebNavigationEvent
 */
class wxWebView : public wxControl
{
public:

    /**
        Creation function for two-step creation.
    */
    virtual bool Create(wxWindow* parent,
                        wxWindowID id,
                        const wxString& url,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) = 0;

    /**
        Factory function to create a new wxWebView for two-step creation
        (you need to call wxWebView::Create on the returned object)
        @param backend which web engine to use as backend for wxWebView
        @return the created wxWebView, or NULL if the requested backend is
                not available
     */
    static wxWebView* New(wxWebViewBackend backend = wxWEB_VIEW_BACKEND_DEFAULT);

    /**
        Factory function to create a new wxWebView
        @param parent parent window to create this view in
        @param id ID of this control
        @param url URL to load by default in the web view
        @param pos position to create this control at
               (you may use wxDefaultPosition if you use sizers)
        @param size size to create this control with
               (you may use wxDefaultSize if you use sizers)
        @param backend which web engine to use as backend for wxWebView
        @return the created wxWebView, or NULL if the requested backend
                is not available
    */
    static wxWebView* New(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           wxWebViewBackend backend = wxWEB_VIEW_BACKEND_DEFAULT,
           long style = 0,
           const wxString& name = wxWebViewNameStr);

    /**
        Get the title of the current web page, or its URL/path if title is not
        available.
    */
    virtual wxString GetCurrentTitle() = 0;

   /**
        Get the URL of the currently displayed document.
    */
    virtual wxString GetCurrentURL() = 0;

    /**
        Get the HTML source code of the currently displayed document.
        @return The HTML source code, or an empty string if no page is currently
                shown.
    */
    virtual wxString GetPageSource() = 0;
    
    /**
        Get the text of the current page.
    */
    virtual wxString GetPageText() = 0;
    
    /**
        Returns whether the web control is currently busy (e.g. loading a page).
    */
    virtual bool IsBusy() = 0;

    /**
        Returns whether the web control is currently editable
    */
    virtual bool IsEditable() = 0;

    /**
        Load a web page from a URL
        @param url The URL of the page to be loaded.
        @note Web engines generally report errors asynchronously, so if you wish
            to know whether loading the URL was successful, register to receive
            navigation error events.
    */
    virtual void LoadUrl(const wxString& url) = 0;

    /**
        Opens a print dialog so that the user may print the currently
        displayed page.
    */
    virtual void Print() = 0;
    
    /**
        Registers a custom scheme handler.
        @param handler A pointer to a heap allocated wxWebHandler.
    */
    virtual void RegisterHandler(wxWebHandler* handler) = 0;

    /**
        Reload the currently displayed URL.
        @param flags A bit array that may optionally contain reload options.
    */
    virtual void Reload(wxWebViewReloadFlags flags = wxWEB_VIEW_RELOAD_DEFAULT) = 0;
    
    /**
        Runs the given javascript code. 
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
        May trigger an error event of type @c wxWEB_NAV_ERR_USER_CANCELLED.
        TODO: make @c wxWEB_NAV_ERR_USER_CANCELLED errors uniform across ports.
    */
    virtual void Stop() = 0;

    /**
        @name Clipboard
    */

    /**
        Returns @true if the current selection can be copied.
    */
    virtual bool CanCopy() = 0;

    /**
        Returns @true if the current selection can be cut.
    */
    virtual bool CanCut() = 0;

    /**
        Returns @true if data can be pasted.
    */
    virtual bool CanPaste() = 0;

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
        @name History
    */

    /** 
        Returns @true if it is possible to navigate backward in the history of
        visited pages.
    */
    virtual bool CanGoBack() = 0;

    /** 
        Returns @true if it is possible to navigate forward in the history of
        visited pages.
    */
    virtual bool CanGoForward() = 0;

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
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetBackwardHistory() = 0;

    /**
        Returns a list of items in the forward history. The first item in the 
        vector is the next item in the history with respect to the curently 
        loaded page.
    */
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetForwardHistory() = 0;

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
    virtual void LoadHistoryItem(wxSharedPtr<wxWebHistoryItem> item) = 0;
    
    /**
        @name Selection
    */
    
    /**
        Clears the current selection. 
    */
    virtual void ClearSelection() = 0;
    
    /**
        Deletes the current selection. Note that for @c wxWEB_VIEW_BACKEND_WEBKIT
        the selection must be editable, either through SetEditable or the 
        correct HTML attribute.
    */
    virtual void DeleteSelection() = 0;
    
    /**
        Returns the currently selected source, if any.
    */
    virtual wxString GetSelectedSource() = 0;
    
    /**
        Returns the currently selected text, if any.
    */
    virtual wxString GetSelectedText() = 0;

    /**
        Returns @true if there is a current selection.
    */
    virtual bool HasSelection() = 0;

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
    virtual bool CanRedo() = 0;

    /**
        Returns @true if there is an action to undo.
    */
    virtual bool CanUndo() = 0;

    /**
        Redos the last action.
    */
    virtual void Redo() = 0;

    /**
        Undos the last action.
    */
    virtual void Undo() = 0;

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
    virtual wxWebViewZoom GetZoom() = 0;

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
    @class wxWebNavigationEvent

    A navigation  event holds information about events associated with 
    wxWebView objects.

    @beginEventEmissionTable{wxWebNavigationEvent}
    @event{EVT_WEB_VIEW_NAVIGATING(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NAVIGATING event, generated before trying
       to get a resource. This event may be vetoed to prevent navigating to this
       resource. Note that if the displayed HTML document has several frames, one
       such event will be generated per frame.
    @event{EVT_WEB_VIEW_NAVIGATED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NAVIGATED event generated after it was
       confirmed that a resource would be requested. This event may not be vetoed.
       Note that if the displayed HTML document has several frames, one such event
       will be generated per frame.
    @event{EVT_WEB_VIEW_LOADED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_LOADED event generated when the document
       is fully loaded and displayed. Note that if the displayed HTML document has 
       several frames, one such event will be generated per frame.
    @event{EVT_WEB_VIEW_ERRROR(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_ERROR event generated when a navigation
       error occurs.
       The integer associated with this event will be a wxWebNavigationError item.
       The string associated with this event may contain a backend-specific more
       precise error message/code.
    @event{EVT_WEB_VIEW_NEWWINDOW(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_NEWWINDOW event, generated when a new
       window is created. You must handle this event if you want anything to 
       happen, for example to load the page in a new window or tab.
    @event{EVT_WEB_VIEW_TITLE_CHANGED(id, func)}
       Process a @c wxEVT_COMMAND_WEB_VIEW_TITLE_CHANGED event, generated when 
       the page title changes. Use GetString to get the title.
    @endEventTable

    @library{wxweb}
    @category{events,web}

    @see wxWebView
*/
class wxWebNavigationEvent : public wxCommandEvent
{
public:
    wxWebNavigationEvent();
    wxWebNavigationEvent(wxEventType type, int id, const wxString href,
                         const wxString target, bool canVeto);

    /**
        Get the name of the target frame which the url of this event
        has been or will be loaded into. This may return an emptry string
        if the frame is not avaliable.
    */
    const wxString& GetTarget() const;

    /**
        Get the URL being visited
    */
    const wxString& GetURL() const;

    virtual wxEvent* Clone() const;

    /** 
        Get whether this event may be vetoed (stopped/prevented). Only
        meaningful for events fired before navigation takes place.
     */
    bool CanVeto() const;

    /** 
        Whether this event was vetoed (stopped/prevented). Only meaningful for
        events fired before navigation takes place or new window events.
     */
    bool IsVetoed() const;

    /** 
        Veto (prevent/stop) this event. Only meaningful for events fired
        before navigation takes place. Only valid if CanVeto() returned true.
     */
    void Veto();
};