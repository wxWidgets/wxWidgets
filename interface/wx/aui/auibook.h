/////////////////////////////////////////////////////////////////////////////
// Name:        aui/auibook.h
// Purpose:     interface of wxAuiNotebook
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

enum wxAuiNotebookOption
{
    wxAUI_NB_TOP                 = 1 << 0,
    wxAUI_NB_LEFT                = 1 << 1,  // not implemented yet
    wxAUI_NB_RIGHT               = 1 << 2,  // not implemented yet
    wxAUI_NB_BOTTOM              = 1 << 3,
    wxAUI_NB_TAB_SPLIT           = 1 << 4,
    wxAUI_NB_TAB_MOVE            = 1 << 5,
    wxAUI_NB_TAB_EXTERNAL_MOVE   = 1 << 6,
    wxAUI_NB_TAB_FIXED_WIDTH     = 1 << 7,
    wxAUI_NB_SCROLL_BUTTONS      = 1 << 8,
    wxAUI_NB_WINDOWLIST_BUTTON   = 1 << 9,
    wxAUI_NB_CLOSE_BUTTON        = 1 << 10,
    wxAUI_NB_CLOSE_ON_ACTIVE_TAB = 1 << 11,
    wxAUI_NB_CLOSE_ON_ALL_TABS   = 1 << 12,
    wxAUI_NB_MIDDLE_CLICK_CLOSE  = 1 << 13,

    /// @since 3.3.0
    wxAUI_NB_MULTILINE           = 1 << 14,

    /**
        Allow the user to pin tabs by using the pin button.

        With this style, the active page shows either a "pin" icon allowing to
        pin it if it it's currently not pinned or an "unpin" icon if it is
        already pinned. Note that "unpin" icon may be shown even if this style
        is not specified, but ::wxAUI_NB_UNPIN_ON_ALL_PINNED is.

        Note that if this style is not specified, tabs can still be pinned
        programmatically using SetPageKind().

        @since 3.3.0
     */
    wxAUI_NB_PIN_ON_ACTIVE_TAB    = 1 << 15,

    /**
        Allow the user to unpin pinned tabs by using the unpin button.

        Specifying this style shows "unpin" button on all currently pinned
        tabs, allowing the user to unpin them, i.e. make them normal again.

        This style can be combined with ::wxAUI_NB_PIN_ON_ACTIVE_TAB or used on
        its own.

        @since 3.3.0
     */
    wxAUI_NB_UNPIN_ON_ALL_PINNED  = 1 << 16,

    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                             wxAUI_NB_MIDDLE_CLICK_CLOSE
};


/**
    Simple struct combining wxAuiTabCtrl with the position inside it.

    This information fully determines the position of the page in
    wxAuiNotebook, see wxAuiNotebook::GetPagePosition().

    @since 3.3.0
*/
struct wxAuiNotebookPosition
{
    /**
        Check if the position is valid.

        This overloaded operator allows to use objects of this class in
        conditional statements, e.g.

        @code
        if ( const auto pos = book->GetPagePosition(page) )
        {
            // pos is valid, can use its tabCtrl and tabIdx members here
        }
        @endcode
     */
    explicit operator bool() const;

    /// Tab control containing the page or nullptr if the position is invalid.
    wxAuiTabCtrl* tabCtrl = nullptr;

    /// Index of the page in this tab control or wxNOT_FOUND if invalid.
    int tabIdx = wxNOT_FOUND;
};


/**
    @class wxAuiNotebook

    wxAuiNotebook is part of the wxAUI class framework, which represents a
    notebook control, managing multiple windows with associated tabs.

    See also @ref overview_aui.

    wxAuiNotebook is a notebook control which implements many features common in
    applications with dockable panes.
    Specifically, wxAuiNotebook implements functionality which allows the user to
    rearrange tab order via drag-and-drop, split the tab window into many different
    splitter configurations, and toggle through different themes to customize
    the control's look and feel.

    The default theme since wxWidgets 3.3.0 is wxAuiFlatTabArt. If you would
    prefer to use the theme which used to be default in the previous versions,
    you can call wxAuiNotebook::SetArtProvider() with wxAuiNativeTabArt as the
    argument. Notice that wxAuiNativeTabArt may be not compatible with
    ::wxAUI_NB_BOTTOM, ::wxAUI_NB_PIN_ON_ACTIVE_TAB and
    ::wxAUI_NB_UNPIN_ON_ALL_PINNED styles, so using it is not recommended if
    you use any of them.

    @section auibook_tabs Multiple Tab Controls

    Initially, wxAuiNotebook creates the main tab control, which can be
    retrieved using GetMainTabCtrl(), and uses it for all tabs. However
    when ::wxAUI_NB_TAB_SPLIT style is used (which is the case by default), the
    user will be able to drag pages out of it and create new tab controls, that
    can then themselves be dragged to be docked in a different place inside the
    notebook. Also, whether ::wxAUI_NB_TAB_SPLIT is specified or not, Split()
    function can always be used to create new tab controls programmatically.

    When using multiple tab controls, exactly one of them is active at any
    time. This tab control can be retrieved by calling GetActiveTabCtrl() and
    is always used for appending or inserting new pages. You can also use
    GetAllTabCtrls() to get all existing tab controls.


    @section auibook_order Pages Indices and Positions

    Each notebook page has its logical index, which is determined by the order
    in which the pages are added, i.e. the first page added has index 0, the
    second one has index 1, and so on, but also has its physical display
    position, which corresponds to the position at which it is displayed.
    Initially the indices and positions are the same for all pages, but they
    may become different if the user reorders the pages by dragging them around
    (which is possible when ::wxAUI_NB_TAB_MOVE style, included in the default
    notebook flags, is on). Also note that it's possible to have multiple pages
    with the same physical position, in different tab controls (see the
    previous section), e.g. each first page in each tab control has physical
    position 0, but there is only one page with logical index 0.

    All functions taking a page index parameter, such as SetPageText(), work
    with logical indices. Similarly, functions returning a page index, such as
    GetSelection(), also always return logical indices. To get the physical
    position of a single page, use GetPagePosition() and to get all pages in
    some tab control in their physical, display order GetPagesInDisplayOrder()
    can be used.


    @section auibook_layout Pages Layout

    When the user can change the notebook layout interactively, i.e. when
    ::wxAUI_NB_TAB_MOVE and/or ::wxAUI_NB_TAB_SPLIT styles are used, it can be
    useful to remember the current layout on program exit and restore it when
    it is restarted. This can be done by saving, and reloading, the layout of
    the entire wxAuiManager containing this notebook using
    wxAuiManager::SaveLayout() and wxAuiManager::LoadLayout(), but it can also
    be done just for the given notebook, without affecting the other panes,
    using SaveLayout() and LoadLayout() functions of this class.

    Using them is similar to using wxAuiManager functions, except they only
    require implementing wxAuiBookSerializer or wxAuiBookDeserializer
    interface, which is a subset of the full wxAuiSerializer or
    wxAuiDeserializer. The @ref page_samples_aui shows how to use them.


    @beginStyleTable
    @style{wxAUI_NB_DEFAULT_STYLE}
           Defined as wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |
           wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
           wxAUI_NB_MIDDLE_CLICK_CLOSE.
    @style{wxAUI_NB_TAB_SPLIT}
           Allows the tab control to be split by dragging a tab.
    @style{wxAUI_NB_TAB_MOVE}
           Allows a tab to be moved horizontally by dragging.
    @style{wxAUI_NB_TAB_EXTERNAL_MOVE}
           Allows a tab to be moved to another tab control.
    @style{wxAUI_NB_TAB_FIXED_WIDTH}
           With this style, all tabs have the same width.
    @style{wxAUI_NB_SCROLL_BUTTONS}
           With this style, left and right scroll buttons are displayed.
           Note that this style is ignored if wxAUI_NB_MULTILINE is used.
    @style{wxAUI_NB_WINDOWLIST_BUTTON}
           With this style, a drop-down list of windows is available.
    @style{wxAUI_NB_CLOSE_BUTTON}
           With this style, a close button is available on the tab bar.
    @style{wxAUI_NB_CLOSE_ON_ACTIVE_TAB}
           With this style, the close button is visible on the active tab.
    @style{wxAUI_NB_CLOSE_ON_ALL_TABS}
           With this style, the close button is visible on all tabs.
    @style{wxAUI_NB_MIDDLE_CLICK_CLOSE}
           With this style, middle click on a tab closes the tab.
    @style{wxAUI_NB_TOP}
           With this style, tabs are drawn along the top of the notebook.
    @style{wxAUI_NB_BOTTOM}
           With this style, tabs are drawn along the bottom of the notebook.
    @style{wxAUI_NB_MULTILINE}
           If this style is specified and all the tabs don't fit in the visible
           area, multiple rows of tabs are used instead of adding a button
           allowing to scroll them. This style is only available in wxWidgets
           3.3.0 or later.
    @style{wxAUI_NB_PIN_ON_ACTIVE_TAB}
           If this style is specified, the active tab shows either a "pin" icon
           allowing to pin it (i.e. change its kind to wxAuiTabKind::Pinned) if
           it's not currently pinned or an "unpin" icon to change the kind back
           to normal. This style is not included in the default notebook style
           and has to be explicitly specified for the user to be able to pin
           the tabs interactively. It is available in wxWidgets 3.3.0 or later.
    @style{wxAUI_NB_UNPIN_ON_ALL_PINNED}
           If this style is specified, "unpin" button is shown on all currently
           pinned tabs, allowing the user to unpin them, i.e. make them normal
           again. This style can be combined with ::wxAUI_NB_PIN_ON_ACTIVE_TAB
           or used on its own. It is available in wxWidgets 3.3.0 or later.
    @endStyleTable

    @beginEventEmissionTable{wxAuiNotebookEvent}
    @event{EVT_AUINOTEBOOK_PAGE_CLOSE(id, func)}
        A page is about to be closed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CLOSE event.
    @event{EVT_AUINOTEBOOK_PAGE_CLOSED(winid, fn)}
        A page has been closed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CLOSED event.
    @event{EVT_AUINOTEBOOK_PAGE_CHANGED(id, func)}
        The page selection was changed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CHANGED event.
    @event{EVT_AUINOTEBOOK_PAGE_CHANGING(id, func)}
        The page selection is about to be changed. Processes a  @c wxEVT_AUINOTEBOOK_PAGE_CHANGING event. This event can be vetoed.
    @event{EVT_AUINOTEBOOK_BUTTON(id, func)}
        The window list button has been pressed. Processes a @c wxEVT_AUINOTEBOOK_BUTTON event.
    @event{EVT_AUINOTEBOOK_BEGIN_DRAG(id, func)}
        Dragging is about to begin. Processes a @c wxEVT_AUINOTEBOOK_BEGIN_DRAG event.
    @event{EVT_AUINOTEBOOK_END_DRAG(id, func)}
        Dragging has ended. Processes a @c wxEVT_AUINOTEBOOK_END_DRAG event.
    @event{EVT_AUINOTEBOOK_DRAG_MOTION(id, func)}
        Emitted during a drag and drop operation. Processes a @c wxEVT_AUINOTEBOOK_DRAG_MOTION event.
    @event{EVT_AUINOTEBOOK_ALLOW_DND(id, func)}
        Whether to allow a tab to be dropped. Processes a @c wxEVT_AUINOTEBOOK_ALLOW_DND event. This event must be specially allowed.
    @event{EVT_AUINOTEBOOK_DRAG_DONE(winid, fn)}
        Notify that the tab has been dragged. Processes a @c wxEVT_AUINOTEBOOK_DRAG_DONE event.
    @event{EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN(winid, fn)}
        The middle mouse button is pressed on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN event.
    @event{EVT_AUINOTEBOOK_TAB_MIDDLE_UP(winid, fn)}
        The middle mouse button is released on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP event.
    @event{EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(winid, fn)}
        The right mouse button is pressed on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN event.
    @event{EVT_AUINOTEBOOK_TAB_RIGHT_UP(winid, fn)}
        The right mouse button is released on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_RIGHT_UP event.
    @event{EVT_AUINOTEBOOK_BG_DCLICK(winid, fn)}
        Double clicked on the tabs background area. Processes a @c wxEVT_AUINOTEBOOK_BG_DCLICK event.
    @endEventTable

    Please see the note in wxAuiNotebookEvent documentation about handling
    these events.

    @library{wxaui}
    @category{aui}
*/
class wxAuiNotebook : public wxBookCtrlBase
{
public:
    /**
        Default ctor.
    */
    wxAuiNotebook();

    /**
        Constructor. Creates a wxAuiNotebok control.
    */
    wxAuiNotebook(wxWindow* parent, wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE);

    /**
        Adds a page.
        If the @a select parameter is @true, calling this will generate a page change event.
    */
    bool AddPage(wxWindow* page, const wxString& caption,
                 bool select = false,
                 const wxBitmapBundle& bitmap = wxBitmapBundle());

    /**
        Adds a new page.

        The page must have the book control itself as the parent and must not
        have been added to this control previously.

        The call to this function may generate the page changing events.

        @param page
            Specifies the new page.
        @param text
            Specifies the text for the new page.
        @param select
            Specifies whether the page should be selected.
        @param imageId
            Specifies the optional image index for the new page.

        @return @true if successful, @false otherwise.

        @remarks Do not delete the page, it will be deleted by the book control.

        @see InsertPage()
        @since 2.9.3
    */
    virtual bool AddPage(wxWindow *page, const wxString &text, bool select, int imageId);

    /**
        Sets the selection to the next or previous page in the same tab control.

        This function sets selection to the next (if @a forward is @a true) or
        previous (otherwise) page after or before the currently selected one in
        but without leaving the current tab control.
    */
    void AdvanceSelection(bool forward = true);

    /**
        Changes the selection for the given page, returning the previous selection.

        This function behaves as SetSelection() but does @em not generate the
        page changing events.

        See @ref overview_events_prog for more information.
        @since 2.9.3
    */
    virtual int ChangeSelection(size_t n);

    /**
        Creates the notebook window.
    */
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    /**
        Deletes all pages.
        @since 2.9.3
    */
    virtual bool DeleteAllPages();

    /**
        Deletes a page at the given index.
        Calling this method will generate a page change event.
    */
    bool DeletePage(size_t page);

    /**
        Returns the associated art provider.
    */
    wxAuiTabArt* GetArtProvider() const;

    /**
        Returns the currently selected page or @NULL.
        @since 2.9.3
    */
    wxWindow* GetCurrentPage () const;

    /**
        Returns the desired height of the notebook for the given page height.
        Use this to fit the notebook to a given page size.
    */
    int GetHeightForPageHeight(int pageHeight);

    /**
        Returns the page specified by the given index.
    */
    wxWindow* GetPage(size_t page_idx) const;

    /**
        Returns the tab bitmap for the page.
    */
    wxBitmap GetPageBitmap(size_t page) const;

    /**
        Returns the number of pages in the notebook.
    */
    size_t GetPageCount() const;

    /**
        Returns the page index for the specified window.
        If the window is not found in the notebook, wxNOT_FOUND is returned.

        This is AUI-specific equivalent to wxBookCtrl::FindPage() and it is
        recommended to use that generic method instead of this one.
    */
    int GetPageIndex(wxWindow* page_wnd) const;

    /**
        Returns the tab kind for the page.

        See SetPageKind().

        @since 3.3.0
     */
    wxAuiTabKind GetPageKind(size_t pageIdx) const;

    /**
        Returns the position of the page in the notebook.

        For example, to determine if one page is located immediately next to
        another one, the following code could be used:

        @code
        wxAuiNotebookPosition pos1 = notebook->GetPagePosition(page1);
        wxAuiNotebookPosition pos2 = notebook->GetPagePosition(page2);
        if ( pos1.tabctrl == pos2.tabctrl && pos1.page + 1 == pos2.page )
        {
            // page1 is immediately before page2
        }
        @endcode

        Note that it would be wrong to just check that `page1 + 1 == page2`
        here because the logical page index may not correspond to their visual
        position if they have been reordered by the user in a control with
        wxAUI_NB_TAB_MOVE style.

        @since 3.3.0
    */
    wxAuiNotebookPosition GetPagePosition(size_t page) const;

    /**
        Returns indices of all pages in the given tab control.

        The pages are returned in the order they are displayed in the tab,
        which may be different from the default order if they were rearranged
        by the user.

        The @a tabCtrl must be valid, see GetActiveTabCtrl() and
        GetAllTabCtrls() for the functions that can be used to get the tab
        control to use.

        The returned vector contains the logical page indices.

        @since 3.3.0
     */
    std::vector<size_t> GetPagesInDisplayOrder(wxAuiTabCtrl* tabCtrl) const;

    /**
        Returns the tab label for the page.
    */
    wxString GetPageText(size_t page) const;

    /**
        Returns the tooltip for the tab label of the page.

        @since 2.9.4
    */
    wxString GetPageToolTip(size_t pageIdx) const;

    /**
        Returns the currently selected page.
    */
    int GetSelection() const;

    /**
        Returns the height of the tab control.
    */
    int GetTabCtrlHeight() const;

    /**
        InsertPage() is similar to AddPage, but allows the ability to specify the
        insert location.

        If the @a select parameter is @true, calling this will generate a page change
        event.

        Page index @a page_idx specifies the page before which the new page
        should be inserted. It may be equal to the current number of pages in
        the notebook, in which case this function is equivalent to AddPage(),
        but can't be strictly greater than it.

        Note that if you want to insert the page at the specified physical
        position, e.g. at the beginning of the current tab control, you need to
        use GetPagesInDisplayOrder() to get the logical page index
        corresponding to the position 0 and then pass this index to this
        function.

        Note that if the page with the given index is not in the currently
        active tab control, the new page will be added at the end of the active
        tab instead of being inserted into another tab control.

        @param index
            Specifies the page before which the new page should be inserted.
        @param page
            Specifies the new page.
        @param text
            Specifies the text for the new page.
        @param select
            Specifies whether the page should be selected.
        @param bitmap
            Specifies the optional bitmap to use for the new page.

        @return @true if successful, @false otherwise.

        @remarks Do not delete the page, it will be deleted by the book control.

        @see AddPage()
        @since 2.9.3
    */
    bool InsertPage(size_t index, wxWindow* page,
                    const wxString& text,
                    bool select = false,
                    const wxBitmapBundle& bitmap = wxBitmapBundle());

    /// @overload
    virtual bool InsertPage(size_t index, wxWindow *page, const wxString &text,
                            bool select, int imageId);

    /**
        Load the previously saved layout of the notebook.

        This function is used to restore the layout previously saved by
        SaveLayout().

        @param name
            Used as argument for wxAuiBookDeserializer::LoadNotebookTabs()
            call.
        @param deserializer
            The object to use for restoring the layout.

        @see wxAuiManager::LoadLayout()

        @since 3.3.0
     */
    void LoadLayout(const wxString& name, wxAuiBookDeserializer& deserializer);

    /**
        Removes a page, without deleting the window pointer.
    */
    bool RemovePage(size_t page);

    /**
        Save the layout of the notebook using the provided serializer.

        The notebook layout includes the number and positions of all the tab
        controls as well as the pages contained in each of them and their
        order.

        The serializer defines how exactly this information is saved: it can
        use any form of serialization, e.g. XML or JSON, to do it, with the
        only requirement being that LoadLayout() should be able to restore it
        from the same @a name.

        @param name
            Used as argument for wxAuiBookSerializer::BeforeSaveNotebook() call.
        @param serializer
            The object to use for saving the layout.

        @see wxAuiManager::SaveLayout()

        @since 3.3.0
     */
    void SaveLayout(const wxString& name, wxAuiBookSerializer& serializer) const;

    /**
        Sets the art provider to be used by the notebook.
    */
    void SetArtProvider(wxAuiTabArt* art);

    /**
        Sets the font for drawing the tab labels, using a bold version of the font for
        selected tab labels.
    */
    virtual bool SetFont(const wxFont& font);

    /**
        Sets the flags for the wxAuiManager used by wxAuiNotebook.

        Please note that it makes sense to use only some of wxAuiManager flags,
        documented in wxAuiManagerOption, with wxAuiNotebook, but the other
        ones are simply ignored, so it is always possible to reuse the same
        flags for the main wxAuiManager and the one used by the notebook.

        Example of using this function to enable the Venetian blinds effect for
        the notebook:
        @code
            auiNotebook->SetManagerFlags(
                wxAuiManager::GetManager()->GetFlags() | ~wxAUI_MGR_VENETIAN_BLINDS_HINT
            );
        @endcode

        @see wxAuiManager::SetFlags(), wxAuiManagerOption

        @since 3.3.0
    */
    void SetManagerFlags(unsigned int flags);

    /**
        Sets the font for measuring tab labels.
    */
    void SetMeasuringFont(const wxFont& font);

    /**
        Sets the font for drawing unselected tab labels.
    */
    void SetNormalFont(const wxFont& font);

    /**
        Sets the bitmap for the page.  To remove a bitmap from the tab caption, pass
        an empty wxBitmapBundle.
    */
    bool SetPageBitmap(size_t page, const wxBitmapBundle& bitmap);

    /**
        Sets the image index for the given page. @a image is an index into
        the image list which was set with SetImageList().
        @since 2.9.3
    */
    virtual bool SetPageImage(size_t n, int imageId);

    /**
        Set the tab kind.

        Can be used to pin or lock a tab.

        Tabs are are grouped in 3 subsets (each of which can possibly be
        empty):

        - Shown first are locked tabs which are typically used for showing some
        different content from the normal (and pinned) tabs. These tabs are
        special, they're always shown and can't be closed nor moved, by
        dragging them, by the user.
        - Next are pinned tabs: these tabs can be closed and, depending on
        whether ::wxAUI_NB_PIN_ON_ACTIVE_TAB and ::wxAUI_NB_UNPIN_ON_ALL_PINNED
        styles are specified, can also be unpinned (i.e. made normal) by the
        user. If ::wxAUI_NB_TAB_MOVE is specified, they can be moved by
        dragging them, however they are restricted to remain in the pinned tabs
        group, i.e. only the order of the pinned tabs can be changed.
        - Finally, normal tabs are shown. These tabs can be closed and,
        depending on ::wxAUI_NB_PIN_ON_ACTIVE_TAB style, pinned by the user.
        They can also be moved by dragging them, but only inside the same
        group.

        @param pageIdx
            The index of the page to change.
        @param kind
            The new kind for the page.
        @return
            @true if the kind was changed, @false if it didn't change, either
            because the page already was of the specified @a kind or because
            the preconditions were not satisfied, e.g. the page index was
            invalid.

        @since 3.3.0
     */
    bool SetPageKind(size_t pageIdx, wxAuiTabKind kind);

    /**
        Sets the tab label for the page.
    */
    bool SetPageText(size_t page, const wxString& text);

    /**
        Sets the tooltip displayed when hovering over the tab label of the page.

        @return
            @true if tooltip was updated, @false if it failed, e.g. because the
            page index is invalid.

        @since 2.9.4
    */
    bool SetPageToolTip(size_t page, const wxString& text);

    /**
        Sets the font for drawing selected tab labels.
    */
    void SetSelectedFont(const wxFont& font);

    /**
        Sets the page selection.  Calling this method will generate a page change event.
    */
    int SetSelection(size_t new_page);

    /**
        Sets the tab height. By default, the tab control height is calculated
        by measuring the text height and bitmap sizes on the tab captions. Calling this
        method will override that calculation and set the tab control to the specified
        height parameter. A call to this method will override any call to
        SetUniformBitmapSize().

        Specifying -1 as the height will return the control to its default auto-sizing
        behaviour.

        If the control uses @c wxAUI_NB_MULTILINE style, the @a height
        parameter specifies the height of a single row of tabs and not the
        combined height of all rows.
    */
    virtual void SetTabCtrlHeight(int height);

    /**
        Ensure that all tabs have the same height, even if some of them don't
        have bitmaps.

        Passing ::wxDefaultSize as @a size undoes the effect of a previous call
        to this function and instructs the control to use dynamic tab height.
     */
    virtual void SetUniformBitmapSize(const wxSize& size);

    /**
        Split performs a split operation programmatically. The argument @a page
        indicates the page that will be split off.  This page will also become the
        active page after the split.

        The @a direction argument specifies where the pane should go, it should be one
        of the following: wxTOP, wxBOTTOM, wxLEFT, or wxRIGHT.

        @see UnsplitAll()
    */
    void Split(size_t page, int direction);

    /**
        Shows the window menu for the active tab control associated with this notebook,
        and returns @true if a selection was made.
    */
    bool ShowWindowMenu();

    /**
        Remove all split tab controls, leaving only the single one.

        This is the opposite of Split() function and collects all the pages
        from all tab controls in the central tab control and removes the other
        ones.

        If there are no other tab controls, this function doesn't do anything.

        Note that calling Split() followed by UnsplitAll() does _not_ preserve
        the page order, as all previously split pages are added at the end of
        the main tab control and not at their previous positions.

        @since 3.3.0
    */
    void UnsplitAll();

    /**
        Returns the image index for the given page.
    */
    virtual int GetPageImage(size_t nPage) const;

    /**
        Returns tab control based on point coordinates inside the tab frame.

        @since 3.1.4
    */
    wxAuiTabCtrl* GetTabCtrlFromPoint(const wxPoint& pt);

    /**
        Returns active tab control for this notebook.

        Active tab control is the one containing the currently selected page.
        If there is no selected page, the main tab control is returned, see
        GetMainTabCtrl().

        @return Non-@NULL pointer to either the active or main tab control.

        @since 3.1.4
    */
    wxAuiTabCtrl* GetActiveTabCtrl();

    /**
        Returns all tab controls for this notebook.

        @return Vector of all tab controls, never empty as it always contains
            at least the main tab control.

        @since 3.3.0
    */
    std::vector<wxAuiTabCtrl*> GetAllTabCtrls();

    /**
        Returns the main tab control for this notebook.

        The main tab control is the one created by the notebook itself
        initially to contain the pages added to it.

        @return Non-@NULL pointer to the main tab control.

        @since 3.3.0
     */
    wxAuiTabCtrl* GetMainTabCtrl();

    /**
        Finds tab control associated with a given window and its tab index.

        @return @true when the tab control is found, @false otherwise.

        @since 3.1.4
    */
    bool FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx);
};

/**
    Tab kind for wxAuiNotebook pages.

    See wxAuiNotebook::SetTabKind().

    @since 3.3.0
 */
enum class wxAuiTabKind
{
    Normal, ///< Can be closed and dragged by user.
    Pinned, ///< Can be closed and possibly unpinned by user.
    Locked  ///< Can't be closed, dragged nor unlocked by user.
};


/**
    @class wxAuiNotebookPage

    Holds information about a page in wxAuiNotebook.

    An object of this class is notably passed to wxAuiTabArt::DrawTab() and is
    used by it to render the pages tab accordingly.

    @library{wxaui}
    @category{aui}
*/
class wxAuiNotebookPage
{
public:
    /// Window shown on this page.
    wxWindow* window = nullptr;

    /// Text displayed on the tab.
    wxString caption;

    /// Tooltip displayed when hovering over tab title.
    wxString tooltip;

    /// Bitmap shown in the tab if valid.
    wxBitmapBundle bitmap;

    /// The bounding rectangle of this page tab.
    wxRect rect;

    /// True if the page is the currently selected page.
    bool active = false;

    /**
        Vector with per-page buttons.

        This vector may be empty.

        @since 3.3.0
     */
    std::vector<wxAuiTabContainerButton> buttons;

    // The rest of the fields are used internally by wxAUI and are
    // intentionally not documented here.
};

/**
    A vector of AUI notebook pages.

    This class is actually a legacy container (see @ref overview_container for
    more details), but it can, and should be, handled as just a vector of
    wxAuiNotebookPage objects in the application code.
*/
class wxAuiNotebookPageArray : public std::vector<wxAuiNotebookPage>
{
};


/**
    @class wxAuiTabContainerButton

    A simple class which holds information about wxAuiNotebook tab buttons and their state.

    @library{wxaui}
    @category{aui}
*/
class wxAuiTabContainerButton
{
public:
    /**
        The id of the button.

        E.g. ::wxAUI_BUTTON_CLOSE.
     */
    int id;

    /**
        State of the button.

        This is a combination of values from ::wxAuiPaneButtonState enum.

        The effect of specifying ::wxAUI_BUTTON_STATE_HIDDEN here is the same
        as not including information about this button at all.
     */
    int curState;

    /// buttons location (wxLEFT, wxRIGHT, or wxCENTER)
    int location;
    /// button's hover bitmap
    wxBitmapBundle bitmap;
    /// button's disabled bitmap
    wxBitmapBundle disBitmap;
    /// button's hit rectangle
    wxRect rect;
};


/**
    A vector of AUI tab buttons.

    This class is actually a legacy container (see @ref overview_container for
    more details), but it can, and should be, handled as just a vector of
    wxAuiTabContainerButton objects in the application code.
*/
class wxAuiTabContainerButtonArray : public std::vector<wxAuiTabContainerButton>
{
};


/**
    @class wxAuiTabContainer

    wxAuiTabContainer is a class which contains information about each tab.
    It also can render an entire tab control to a specified DC.
    It's not a window class itself, because this code will be used by
    the wxAuiNotebook, where it is disadvantageous to have separate
    windows for each tab control in the case of "docked tabs".

    A derived class, wxAuiTabCtrl, is an actual wxWindow - derived window
    which can be used as a tab control in the normal sense.

    @library{wxaui}
    @category{aui}
*/
class wxAuiTabContainer
{
public:

    /**
        Default ctor.
    */
    wxAuiTabContainer();

    /**
        Default dtor.
    */
    virtual ~wxAuiTabContainer();

    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider() const;

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    /**
        Returns @true if the given flag is set.

        @since 3.3.0
    */
    bool IsFlagSet(unsigned int flag) const;

    bool AddPage(const wxAuiNotebookPage& info);
    bool InsertPage(const wxAuiNotebookPage& info, size_t idx);
    bool MovePage(wxWindow* page, size_t newIdx);
    bool RemovePage(wxWindow* page);
    void RemovePageAt(size_t idx);
    bool SetActivePage(const wxWindow* page);
    bool SetActivePage(size_t page);
    void SetNoneActive();
    int GetActivePage() const;
    struct HitTestResult
    {
        explicit operator bool() const;
        wxWindow* window = nullptr;
        int pos = wxNOT_FOUND;
    };
    enum HitTestFlags
    {
        HitTest_Default = 0,
        HitTest_AllowAfterTab = 1
    };
    HitTestResult TabHitTest(const wxPoint& pt, int flags = HitTest_Default) const;
    const wxAuiTabContainerButton* ButtonHitTest(const wxPoint& pt) const;
    wxWindow* GetWindowFromIdx(size_t idx) const;
    int GetIdxFromWindow(const wxWindow* page) const;
    size_t GetPageCount() const;
    wxAuiNotebookPage& GetPage(size_t idx);
    const wxAuiNotebookPage& GetPage(size_t idx) const;
    wxAuiNotebookPageArray& GetPages();
    void SetNormalFont(const wxFont& normalFont);
    void SetSelectedFont(const wxFont& selectedFont);
    void SetMeasuringFont(const wxFont& measuringFont);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);
    void DoShowHide();
    void SetRect(const wxRect& rect, wxWindow* wnd = nullptr);

    void RemoveButton(int id);
    void AddButton(int id,
                   int location,
                   const wxBitmapBundle& normalBitmap = wxBitmapBundle(),
                   const wxBitmapBundle& disabledBitmap = wxBitmapBundle());

    size_t GetTabOffset() const;
    void SetTabOffset(size_t offset);

    // Is the tab visible?
    bool IsTabVisible(int tabPage, int tabOffset, wxDC* dc, wxWindow* wnd);

    // Make the tab visible if it wasn't already
    void MakeTabVisible(int tabPage, wxWindow* win);

protected:

    virtual void Render(wxDC* dc, wxWindow* wnd);

protected:

    wxAuiTabArt* m_art;
    wxAuiNotebookPageArray m_pages;
    wxAuiTabContainerButtonArray m_buttons;
    wxAuiTabContainerButtonArray m_tabCloseButtons;
    wxRect m_rect;
    size_t m_tabOffset;
    unsigned int m_flags;
};



/**
    @class wxAuiTabArt

    Tab art provider defines all the drawing functions used by wxAuiNotebook.

    This allows the wxAuiNotebook to have a pluggable look-and-feel.

    By default, a wxAuiNotebook uses an instance of class wxAuiDefaultTabArt,
    derived from this class, which provides bitmaps and a colour scheme that
    is adapted to the major platforms' look. You can either derive from that
    class to alter its behaviour or write a completely new tab art class.

    Another example of creating a new wxAuiNotebook tab bar is wxAuiSimpleTabArt.

    Call wxAuiNotebook::SetArtProvider() to make use of this new tab art.

    @library{wxaui}
    @category{aui}
*/
class wxAuiTabArt
{
public:
    /**
        Constructor.
    */
    wxAuiTabArt();

    /**
        Clones the art object.
    */
    virtual wxAuiTabArt* Clone() = 0;

    /**
        Draws a background on the given area.
    */
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) = 0;

    /**
        Draws a button.
    */
    virtual void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect,
                            int bitmap_id, int button_state, int orientation,
                            wxRect* out_rect) = 0;

    /**
        Draws a tab.

        This function used to be pure virtual and so had to be overridden in
        the derived classes in the previous versions of wxWidgets, however
        since version 3.3.0 it doesn't have to be overridden if
        DrawPageTab() is overridden and, moreover, it is recommended to
        override DrawPageTab() instead of this function in the new code.
    */
    virtual void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                         const wxRect& rect, int close_button_state,
                         wxRect* out_tab_rect, wxRect* out_button_rect, int* x_extent);

    /**
        Draws a tab for the specified notebook page.

        This function must be overridden if DrawTab() is not overridden and,
        also, if pinned tabs are used, as they are not supported by DrawTab().

        The @a pane contains the information about the page to draw, in
        particular its wxAuiNotebookPage::buttons specifies the buttons to
        draw if it is not empty and receives the rectangles where they were
        drawn on output in the wxAuiTabContainerButton::rect fields.

        Note that if a button state is ::wxAUI_BUTTON_STATE_HIDDEN, the effect
        is the same as not including this button at all, i.e. it is not drawn
        and the output field is not modified in this case.

        The wxAuiNotebookPage::rect field is also updated by this function to
        contain the bounding rectangle of the tab.

        @return
            The total horizontal span of the tab, which may be greater than the
            page bounding rectangle.

        @since 3.3.0
     */
    virtual int DrawPageTab(
                         wxDC& dc,
                         wxWindow* wnd,
                         wxAuiNotebookPage& page,
                         const wxRect& rect);

    /**
        Returns the tab control size.
    */
    virtual int GetBestTabCtrlSize(wxWindow*, const wxAuiNotebookPageArray&, const wxSize&) = 0;

    /**
        Returns the indent size.
    */
    virtual int GetIndentSize() = 0;

    /**
        Returns the font to use for normal, non-selected, tabs.

        By default, returns an invalid font, meaning that the font set for
        wxAuiNotebook itself should be used.

        This function should be overridden for SetNormalFont() to actually work.

        @since 3.3.0
    */
    virtual wxFont GetNormalFont() const;

    /**
        Returns the font to use for the selected tab.

        By default, returns an invalid font, meaning that the font set for
        wxAuiNotebook itself should be used.

        This function should be overridden for SetSelectedFont() to actually
        work.

        @since 3.3.0
    */
    virtual wxFont GetSelectedFont() const;

    /**
        Returns the tab size for the given caption, bitmap and state.

        This function used to be pure virtual and so had to be overridden in
        the derived classes in the previous versions of wxWidgets, however
        since version 3.3.0 it doesn't have to be overridden if
        GetPageTabSize() is overridden and it is recommended to override
        GetPageTabSize() instead of this function in the new code.
    */
    virtual wxSize GetTabSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxString& caption,
                              const wxBitmapBundle& bitmap, bool active,
                              int close_button_state, int* x_extent);

    /**
        Returns the size of the tab for the specified notebook page.

        This function must be overridden if GetTabSize() is not overridden and,
        also, if pinned tabs are used, as they are not supported by GetTabSize().

        @since 3.3.0
     */
    virtual wxSize GetPageTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& page,
                 int* xExtent = nullptr);

    /**
        Returns the rectangle for the given button.

        This function is not pure virtual because it is only for multi-line
        tabs, but it must be implemented if wxAUI_NB_MULTILINE is used.

        If specified, the returned rectangle must be filled with the same value
        as DrawButton() puts into its @a outRect but here it can also be null in
        which case just its width is returned.

        @since 3.3.0
    */
    virtual int GetButtonRect(
                         wxReadOnlyDC& dc,
                         wxWindow* wnd,
                         const wxRect& inRect,
                         int bitmapId,
                         int buttonState,
                         int orientation,
                         wxRect* outRect = nullptr) /* = 0 */;

    /**
        Sets flags.
    */
    virtual void SetFlags(unsigned int flags) = 0;

    /**
        Sets the font used for calculating measurements.
    */
    virtual void SetMeasuringFont(const wxFont& font) = 0;

    /**
        Sets the normal font for drawing labels.

        @see GetNormalFont()
    */
    virtual void SetNormalFont(const wxFont& font) = 0;

    /**
        Sets the font for drawing text for selected UI elements.

        @see GetSelectedFont()
    */
    virtual void SetSelectedFont(const wxFont& font) = 0;

    /**
        Sets the colour of the inactive tabs.

        @since 2.9.2
    */
    virtual void SetColour(const wxColour& colour) = 0;

    /**
        Sets the colour of the selected tab.

        @since 2.9.2
    */
    virtual void SetActiveColour(const wxColour& colour) = 0;

    /**
        Sets sizing information.

        The @a wnd argument is only present in wxWidgets 3.1.6 and newer and is
        required, it only has @NULL default value for compatibility reasons.
    */
    virtual void SetSizingInfo(const wxSize& tab_ctrl_size,
                               size_t tab_count,
                               wxWindow* wnd = nullptr) = 0;
};

/**
    wxAuiNativeTabArt is an alias for either the art provider providing
    native-like appearance or wxAuiGenericTabArt if not available.

    Currently wxAuiNativeTabArt uses platform-specific implementation in wxMSW
    and wxGTK2 ports and wxAuiGenericTabArt elsewhere. The preprocessor symbol
    @c wxHAS_NATIVE_TABART is defined if the native implementation is available
    (but note that at least under MSW even the native implementation falls back
    to wxAuiGenericTabArt if dark mode or any wxAuiNotebook styles not
    supported by it are used).

    This art provided used to be the default tab art provider in wxAuiNotebook
    before wxWidgets 3.3.0.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
*/
using wxAuiNativeTabArt = wxAuiGenericTabArt;

/**
    wxAuiDefaultTabArt is an alias for the tab art provider used by
    wxAuiNotebook by default.

    Since wxWidgets 3.3.0, this is wxAuiFlatTabArt under all platforms. In the
    previous versions, this was wxAuiNativeTabArt.
 */
using wxAuiDefaultTabArt = wxAuiFlatTabArt;

/**
    @class wxAuiNotebookEvent

    This class is used by the events generated by wxAuiNotebook.

    Please note that most events generated by wxAuiNotebook are handled by the
    notebook object itself, i.e. they do _not_ propagate upwards to the
    notebook parent window, in spite of being command events. In order to
    handle these events you should use wxEvtHandler::Bind() to connect to the
    events on the notebook object itself and don't forget to use
    wxEvent::Skip() to ensure that the notebook still processes them too.

    @beginEventEmissionTable{wxAuiNotebookEvent}
    @event{EVT_AUINOTEBOOK_PAGE_CLOSE(id, func)}
        A page is about to be closed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CLOSE event.
    @event{EVT_AUINOTEBOOK_PAGE_CLOSED(winid, fn)}
        A page has been closed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CLOSED event.
    @event{EVT_AUINOTEBOOK_PAGE_CHANGED(id, func)}
        The page selection was changed. Processes a @c wxEVT_AUINOTEBOOK_PAGE_CHANGED event.
    @event{EVT_AUINOTEBOOK_PAGE_CHANGING(id, func)}
        The page selection is about to be changed. Processes a  @c wxEVT_AUINOTEBOOK_PAGE_CHANGING event. This event can be vetoed.
    @event{EVT_AUINOTEBOOK_BUTTON(id, func)}
        The window list button has been pressed. Processes a @c wxEVT_AUINOTEBOOK_BUTTON event.
    @event{EVT_AUINOTEBOOK_BEGIN_DRAG(id, func)}
        Dragging is about to begin. Processes a @c wxEVT_AUINOTEBOOK_BEGIN_DRAG event.
    @event{EVT_AUINOTEBOOK_END_DRAG(id, func)}
        Dragging has ended. Processes a @c wxEVT_AUINOTEBOOK_END_DRAG event.
    @event{EVT_AUINOTEBOOK_DRAG_MOTION(id, func)}
        Emitted during a drag and drop operation. Processes a @c wxEVT_AUINOTEBOOK_DRAG_MOTION event.
    @event{EVT_AUINOTEBOOK_ALLOW_DND(id, func)}
        Whether to allow a tab to be dropped. Processes a @c wxEVT_AUINOTEBOOK_ALLOW_DND event. This event must be specially allowed.
    @event{EVT_AUINOTEBOOK_DRAG_DONE(winid, fn)}
        Notify that the tab has been dragged. Processes a @c wxEVT_AUINOTEBOOK_DRAG_DONE event.
    @event{EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN(winid, fn)}
        The middle mouse button is pressed on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN event.
    @event{EVT_AUINOTEBOOK_TAB_MIDDLE_UP(winid, fn)}
        The middle mouse button is released on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP event.
    @event{EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(winid, fn)}
        The right mouse button is pressed on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN event.
    @event{EVT_AUINOTEBOOK_TAB_RIGHT_UP(winid, fn)}
        The right mouse button is released on a tab. Processes a @c wxEVT_AUINOTEBOOK_TAB_RIGHT_UP event.
    @event{EVT_AUINOTEBOOK_BG_DCLICK(winid, fn)}
        Double clicked on the tabs background area. Processes a @c wxEVT_AUINOTEBOOK_BG_DCLICK event.
    @endEventTable

    @library{wxaui}
    @category{events,bookctrl}

    @see wxAuiNotebook, wxBookCtrlEvent
*/
class wxAuiNotebookEvent : public wxBookCtrlEvent
{
public:
    /**
        Constructor.
    */
    wxAuiNotebookEvent(wxEventType command_type = wxEVT_NULL, int win_id = 0);

    wxEvent *Clone();
};

wxEventType wxEVT_AUINOTEBOOK_PAGE_CLOSE;
wxEventType wxEVT_AUINOTEBOOK_PAGE_CHANGED;
wxEventType wxEVT_AUINOTEBOOK_PAGE_CHANGING;
wxEventType wxEVT_AUINOTEBOOK_PAGE_CLOSED;
wxEventType wxEVT_AUINOTEBOOK_BUTTON;
wxEventType wxEVT_AUINOTEBOOK_BEGIN_DRAG;
wxEventType wxEVT_AUINOTEBOOK_END_DRAG;
wxEventType wxEVT_AUINOTEBOOK_DRAG_MOTION;
wxEventType wxEVT_AUINOTEBOOK_ALLOW_DND;
wxEventType wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN;
wxEventType wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP;
wxEventType wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN;
wxEventType wxEVT_AUINOTEBOOK_TAB_RIGHT_UP;
wxEventType wxEVT_AUINOTEBOOK_DRAG_DONE;
wxEventType wxEVT_AUINOTEBOOK_BG_DCLICK;

/**
    An art provider for wxAuiNotebook implementing "flat" look.

    This art provider is currently used as the default art provider.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
 */
class wxAuiFlatTabArt : public wxAuiTabArt
{
public:
    /// Default constructor.
    wxAuiFlatTabArt();
};

/**
    An art provider for wxAuiNotebook implementing "glossy" look.

    This art provider is used as fallback art provider for wxAuiNativeTabArt if
    there is no native tab art provider, but may also be used directly.

    @see wxAuiTabArt

    @genericAppearance{auigenerictabart}

    @library{wxaui}
    @category{aui}
*/

class wxAuiGenericTabArt : public wxAuiTabArt
{
public:

    wxAuiGenericTabArt();
    virtual ~wxAuiGenericTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);
    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount,
                       wxWindow* wnd = nullptr);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmapBundle& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiNotebookPageArray& items,
                 int activeIdx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize);

protected:
    /**
        The font used for all tabs
     */
    wxFont m_normalFont;
    wxFont m_selectedFont; /// The font used on the selected tab
    wxFont m_measuringFont;
    wxColour m_baseColour;
    wxPen m_baseColourPen;
    wxPen m_borderPen;
    wxBrush m_baseColourBrush;
    wxColour m_activeColour;
    wxBitmapBundle m_activeCloseBmp;
    wxBitmapBundle m_disabledCloseBmp;
    wxBitmapBundle m_activeLeftBmp;
    wxBitmapBundle m_disabledLeftBmp;
    wxBitmapBundle m_activeRightBmp;
    wxBitmapBundle m_disabledRightBmp;
    wxBitmapBundle m_activeWindowListBmp;
    wxBitmapBundle m_disabledWindowListBmp;

    int m_fixedTabWidth;
    int m_tabCtrlHeight;
    unsigned int m_flags;
};


/**
    @class wxAuiSimpleTabArt

    Another standard tab art provider for wxAuiNotebook.

    wxAuiSimpleTabArt is derived from wxAuiTabArt demonstrating how to write a
    completely new tab art class. It can also be used as alternative to
    wxAuiDefaultTabArt.

    @genericAppearance{auisimpletabart}

    @library{wxaui}
    @category{aui}
*/

class wxAuiSimpleTabArt : public wxAuiTabArt
{

public:

    wxAuiSimpleTabArt();
    virtual ~wxAuiSimpleTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount,
                       wxWindow* wnd = nullptr);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiNotebookPageArray& items,
                 int activeIdx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize);

protected:

    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxPen m_normalBkPen;
    wxPen m_selectedBkPen;
    wxBrush m_normalBkBrush;
    wxBrush m_selectedBkBrush;
    wxBrush m_bkBrush;
    wxBitmapBundle m_activeCloseBmp;
    wxBitmapBundle m_disabledCloseBmp;
    wxBitmapBundle m_activeLeftBmp;
    wxBitmapBundle m_disabledLeftBmp;
    wxBitmapBundle m_activeRightBmp;
    wxBitmapBundle m_disabledRightBmp;
    wxBitmapBundle m_activeWindowListBmp;
    wxBitmapBundle m_disabledWindowListBmp;

    int m_fixedTabWidth;
    unsigned int m_flags;
};
