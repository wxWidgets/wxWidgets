/////////////////////////////////////////////////////////////////////////////
// Name:        html/htmlwin.h
// Purpose:     interface of wxHtmlWindow
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlWindow

    wxHtmlWindow is probably the only class you will directly use
    unless you want to do something special (like adding new tag
    handlers or MIME filters).

    The purpose of this class is to display HTML pages (either local
    file or downloaded via HTTP protocol) in a window. The width
    of the window is constant - given in the constructor - and virtual height
    is changed dynamically depending on page size.
    Once the window is created you can set its content by calling
    @ref wxHtmlWindow::setpage SetPage(text),
    @ref wxHtmlWindow::loadpage LoadPage(filename) or
    wxHtmlWindow::LoadFile.

    @beginStyleTable
    @style{wxHW_SCROLLBAR_NEVER}
           Never display scrollbars, not even when the page is larger than the
           window.
    @style{wxHW_SCROLLBAR_AUTO}
           Display scrollbars only if page's size exceeds window's size.
    @style{wxHW_NO_SELECTION}
           Don't allow the user to select text.
    @endStyleTable

    @library{wxhtml}
    @category{html}

    @see wxHtmlLinkEvent, wxHtmlCellEvent
*/
class wxHtmlWindow : public wxScrolledWindow
{
public:
    //@{
    /**
        Constructor. The parameters are the same as wxScrolled::wxScrolled()
        constructor.
        
        @param style
            Window style. See wxHtmlWindow.
    */
    wxHtmlWindow();
    wxHtmlWindow(wxWindow parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHW_DEFAULT_STYLE,
                 const wxString& name = "htmlWindow");
    //@}

    /**
        Adds @ref overview_filters "input filter" to the static list of available
        filters. These filters are present by default:
         @c text/html MIME type
         @c image/* MIME types
         Plain Text filter (this filter is used if no other filter matches)
    */
    static void AddFilter(wxHtmlFilter filter);

    /**
        Appends HTML fragment to currently displayed text and refreshes the window.
        
        @param source
            HTML code fragment
        
        @return @false if an error occurred, @true otherwise.
    */
    bool AppendToPage(const wxString& source);

    /**
        Returns pointer to the top-level container.
        See also: @ref overview_cells "Cells Overview",
        @ref overview_printing
    */
    wxHtmlContainerCell* GetInternalRepresentation() const;

    /**
        Returns anchor within currently opened page
        (see wxHtmlWindow::GetOpenedPage).
        If no page is opened or if the displayed page wasn't
        produced by call to LoadPage, empty string is returned.
    */
    wxString GetOpenedAnchor() const;

    /**
        Returns full location of the opened page. If no page is opened or if the
        displayed page wasn't
        produced by call to LoadPage, empty string is returned.
    */
    wxString GetOpenedPage() const;

    /**
        Returns title of the opened page or wxEmptyString if current page does not
        contain @c TITLE tag.
    */
    wxString GetOpenedPageTitle() const;

    /**
        Returns the related frame.
    */
    wxFrame* GetRelatedFrame() const;

    /**
        Moves back to the previous page. (each page displayed using
        LoadPage() is stored in history list.)
    */
    bool HistoryBack();

    /**
        Returns @true if it is possible to go back in the history (i.e. HistoryBack()
        won't fail).
    */
    bool HistoryCanBack();

    /**
        Returns @true if it is possible to go forward in the history (i.e. HistoryBack()
        won't fail).
    */
    bool HistoryCanForward();

    /**
        Clears history.
    */
    void HistoryClear();

    /**
        Moves to next page in history.
    */
    bool HistoryForward();

    /**
        Loads HTML page from file and displays it.
        
        @return @false if an error occurred, @true otherwise
        
        @see LoadPage()
    */
    bool LoadFile(const wxFileName& filename);

    /**
        Unlike SetPage this function first loads HTML page from @a location
        and then displays it. See example:
        
        @param location
            The address of document. See wxFileSystem for details on address format and
        behaviour of "opener".
        
        @return @false if an error occurred, @true otherwise
        
        @see LoadFile()
    */
    virtual bool LoadPage(const wxString& location);

    /**
        This method is called when a mouse button is clicked inside wxHtmlWindow.
        The default behaviour is to emit a wxHtmlCellEvent
        and, if the event was not processed or skipped, call
        OnLinkClicked() if the cell contains an
        hypertext link.
        Overloading this method is deprecated; intercept the event instead.
        
        @param cell
            The cell inside which the mouse was clicked, always a simple
            (i.e. non-container) cell
        @param x, y
            The logical coordinates of the click point
        @param event
            The mouse event containing other information about the click
        
        @return @true if a link was clicked, @false otherwise.
    */
    virtual bool OnCellClicked(wxHtmlCell cell, wxCoord x, wxCoord y,
                               const wxMouseEvent& event);

    /**
        This method is called when a mouse moves over an HTML cell.
        Default behaviour is to emit a wxHtmlCellEvent.
        Overloading this method is deprecated; intercept the event instead.
        
        @param cell
            The cell inside which the mouse is currently, always a simple
            (i.e. non-container) cell
        @param x, y
            The logical coordinates of the click point
    */
    virtual void OnCellMouseHover(wxHtmlCell cell, wxCoord x,
                                  wxCoord y);

    /**
        Called when user clicks on hypertext link. Default behaviour is to emit a
        wxHtmlLinkEvent and, if the event was not processed
        or skipped, call LoadPage() and do nothing else.
        Overloading this method is deprecated; intercept the event instead.
        Also see wxHtmlLinkInfo.
    */
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link);

    /**
        Called when an URL is being opened (either when the user clicks on a link or
        an image is loaded). The URL will be opened only if OnOpeningURL returns
        @c wxHTML_OPEN. This method is called by
        wxHtmlParser::OpenURL.
        You can override OnOpeningURL to selectively block some
        URLs (e.g. for security reasons) or to redirect them elsewhere. Default
        behaviour is to always return @c wxHTML_OPEN.
        
        @param type
            Indicates type of the resource. Is one of
        
        
        
        
        
        
            wxHTML_URL_PAGE
        
        
        
        
            Opening a HTML page.
        
        
        
        
        
            wxHTML_URL_IMAGE
        
        
        
        
            Opening an image.
        
        
        
        
        
            wxHTML_URL_OTHER
        
        
        
        
            Opening a resource that doesn't fall into
            any other category.
        @param url
            URL being opened.
        @param redirect
            Pointer to wxString variable that must be filled with an
            URL if OnOpeningURL returns wxHTML_REDIRECT.
    */
    virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type,
                                             const wxString& url,
                                             wxString* redirect) const;

    /**
        Called on parsing @c TITLE tag.
    */
    virtual void OnSetTitle(const wxString& title);

    /**
        This reads custom settings from wxConfig. It uses the path 'path'
        if given, otherwise it saves info into currently selected path.
        The values are stored in sub-path @c wxHtmlWindow
        Read values: all things set by SetFonts, SetBorders.
        
        @param cfg
            wxConfig from which you want to read the configuration.
        @param path
            Optional path in config tree. If not given current path is used.
    */
    virtual void ReadCustomization(wxConfigBase cfg,
                                   wxString path = wxEmptyString);

    /**
        Selects all text in the window.
        
        @see SelectLine(), SelectWord()
    */
    void SelectAll();

    /**
        Selects the line of text that @a pos points at. Note that @e pos
        is relative to the top of displayed page, not to window's origin, use
        wxScrolled::CalcUnscrolledPosition()
        to convert physical coordinate.
        
        @see SelectAll(), SelectWord()
    */
    void SelectLine(const wxPoint& pos);

    /**
        Selects the word at position @e pos. Note that @e pos
        is relative to the top of displayed page, not to window's origin, use
        wxScrolled::CalcUnscrolledPosition()
        to convert physical coordinate.
        
        @see SelectAll(), SelectLine()
    */
    void SelectWord(const wxPoint& pos);

    /**
        Returns current selection as plain text. Returns empty string if no text
        is currently selected.
    */
    wxString SelectionToText();

    /**
        This function sets the space between border of window and HTML contents. See
        image:
        
        @param b
            indentation from borders in pixels
    */
    void SetBorders(int b);

    /**
        This function sets font sizes and faces.
        
        @param normal_face
            This is face name for normal (i.e. non-fixed) font.
            It can be either empty string (then the default face is chosen) or
            platform-specific face name. Examples are "helvetica" under Unix or
            "Times New Roman" under Windows.
        @param fixed_face
            The same thing for fixed face ( TT../TT )
        @param sizes
            This is an array of 7 items of int type.
            The values represent size of font with HTML size from -2 to +4
            ( FONT SIZE=-2 to FONT SIZE=+4 ). Default sizes are used if sizes
            is @NULL.
    */
    void SetFonts(const wxString& normal_face,
                  const wxString& fixed_face,
                  const int sizes = NULL);

    /**
        Sets HTML page and display it. This won't @b load the page!!
        It will display the @e source. See example:
        
        If you want to load a document from some location use
        LoadPage() instead.
        
        @param source
            The HTML document source to be displayed.
        
        @return @false if an error occurred, @true otherwise.
    */
    virtual bool SetPage(const wxString& source);

    /**
        Sets the frame in which page title will be displayed. @a format is format of
        frame title, e.g. "HtmlHelp : %s". It must contain exactly one %s. This
        %s is substituted with HTML page title.
    */
    void SetRelatedFrame(wxFrame* frame, const wxString& format);

    /**
        @b After calling SetRelatedFrame(),
        this sets statusbar slot where messages will be displayed.
        (Default is -1 = no messages.)

        @param index
            Statusbar slot number (0..n)
    */
    void SetRelatedStatusBar(int index);

    /**
        @b Sets the associated statusbar where messages will be displayed.
        Call this instead of SetRelatedFrame() if you want statusbar updates only,
        no changing of the frame title.

        @param statusbar
            Statusbar pointer
        @param index
            Statusbar slot number (0..n)

        @since 2.9.0
    */
    void SetRelatedStatusBar(wxStatusBar* statusbar, int index = 0);

    /**
        Returns content of currently displayed page as plain text.
    */
    wxString ToText();

    /**
        Saves custom settings into wxConfig. It uses the path 'path'
        if given, otherwise it saves info into currently selected path.
        Regardless of whether the path is given or not, the function creates sub-path
        @c wxHtmlWindow.
        Saved values: all things set by SetFonts, SetBorders.
        
        @param cfg
            wxConfig to which you want to save the configuration.
        @param path
            Optional path in config tree. If not given, the current path is used.
    */
    virtual void WriteCustomization(wxConfigBase cfg,
                                    wxString path = wxEmptyString);
};



/**
    @class wxHtmlLinkEvent

    This event class is used for the events generated by wxHtmlWindow.

    @library{wxhtml}
    @category{FIXME}
*/
class wxHtmlLinkEvent : public wxCommandEvent
{
public:
    /**
        The constructor is not normally used by the user code.
    */
    wxHyperlinkEvent(int id, const wxHtmlLinkInfo& linkinfo);

    /**
        Returns the wxHtmlLinkInfo which contains info about the cell clicked and the
        hyperlink it contains.
    */
    const wxHtmlLinkInfo GetLinkInfo() const;
};



/**
    @class wxHtmlCellEvent

    This event class is used for the events generated by wxHtmlWindow.

    @library{wxhtml}
    @category{FIXME}
*/
class wxHtmlCellEvent : public wxCommandEvent
{
public:
    /**
        The constructor is not normally used by the user code.
    */
    wxHtmlCellEvent(wxEventType commandType, int id,
                    wxHtmlCell* cell,
                    const wxPoint& point);

    /**
        Returns the wxHtmlCellEvent associated with the event.
    */
    wxHtmlCell* GetCell() const;

    /**
        Returns @true if @ref setlinkclicked() SetLinkClicked(@true) has previously
        been called;
        @false otherwise.
    */
    bool GetLinkClicked() const;

    /**
        Returns the wxPoint associated with the event.
    */
    wxPoint GetPoint() const;

    /**
        Call this function with @c linkclicked set to @true if the cell which has
        been clicked contained a link or
        @false otherwise (which is the default). With this function the event handler
        can return info to the
        wxHtmlWindow which sent the event.
    */
    bool SetLinkClicked(bool linkclicked);
};

