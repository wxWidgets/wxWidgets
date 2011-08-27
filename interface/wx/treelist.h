///////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/treelist.h
// Purpose:     wxTreeListCtrl class documentation
// Author:      Vadim Zeitlin
// Created:     2011-08-17
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Unique identifier of an item in wxTreeListCtrl.

    This is an opaque class which can't be used by the application in any other
    way than receiving or passing it to wxTreeListCtrl and checking for
    validity.

    @see wxTreeListCtrl

    @library{wxadv}
    @category{ctrl}

    @since 2.9.3
 */
class wxTreeListItem
{
public:
    /**
        Only the default constructor is publicly accessible.

        Default constructing a wxTreeListItem creates an invalid item.
     */
    wxTreeListItem();

    /**
        Return true if the item is valid.
     */
    bool IsOk() const;
};

/**
    Container of multiple items.
 */
typedef wxVector<wxTreeListItem> wxTreeListItems;

/**
    Special wxTreeListItem value meaning "insert before the first item".

    This value can be passed to wxTreeListCtrl::InsertItem() to achieve the
    same effect as calling wxTreeListCtrl::PrependItem().
 */
extern const wxTreeListItem wxTLI_FIRST;

/**
    Special wxTreeListItem value meaning "insert after the last item".

    This value can be passed to wxTreeListCtrl::InsertItem() to achieve the
    same effect as calling wxTreeListCtrl::AppendItem().
 */
extern const wxTreeListItem wxTLI_LAST;

/**
    A control combining wxTreeCtrl and wxListCtrl features.

    This is a multi-column tree control optionally supporting images and
    checkboxes for the items in the first column.

    It is currently implemented using wxDataViewCtrl internally but provides a
    much simpler interface for the common use case it addresses. Thus, one of
    the design principles for this control is simplicity and intentionally
    doesn't provide all the features of wxDataViewCtrl. Most importantly, this
    class stores all its data internally and doesn't require you to define a
    custom model for it.

    Instead, this controls works like wxTreeCtrl or non-virtual wxListCtrl and
    allows you to simply add items to it using wxTreeListCtrl::AppendItem() and
    related methods. Typically, you start by setting up the columns (you must
    have at least one) by calling wxTreeListCtrl::AppendColumn() and then add
    the items. While only the text of the first column can be specified when
    adding them, you can use wxTreeListCtrl::SetItemText() to set the text of
    the other columns.


    Here are the styles supported by this control. Notice that using
    wxTL_USER_3STATE implies wxTL_3STATE and wxTL_3STATE in turn implies
    wxTL_CHECKBOX.

    @beginStyleTable
    @style{wxTL_SINGLE}
        Single selection, this is the default.
    @style{wxTL_MULTIPLE}
        Allow multiple selection, see GetSelections().
    @style{wxTL_CHECKBOX}
        Show the usual, 2 state, checkboxes for the items in the first column.
    @style{wxTL_3STATE}
        Show the checkboxes that can possibly be set by the program, but not
        the user, to a third, undetermined, state, for the items in the first
        column. Implies wxTL_CHECKBOX.
    @style{wxTL_USER_3STATE}
        Same as wxTL_3STATE but the user can also set the checkboxes to the
        undetermined state. Implies wxTL_3STATE.
    @style{wxTL_DEFAULT_STYLE}
        Style used by the control by default, just wxTL_SINGLE currently.
    @endStyleTable

    @beginEventTable{wxTreeListEvent}
    @event{EVT_TREELIST_SELECTION_CHANGED(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_SELECTION_CHANGED event and notifies
        about the selection change in the control. In the single selection case
        the item indicated by the event has been selected and previously
        selected item, if any, was deselected. In multiple selection case, the
        selection of this item has just changed (it may have been either
        selected or deselected) but notice that the selection of other items
        could have changed as well, use wxTreeListCtrl::GetSelections() to
        retrieve the new selection if necessary.
    @event{EVT_TREELIST_ITEM_EXPANDING(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_ITEM_EXPANDING event notifying about
        the given branch being expanded. This event is sent before the
        expansion occurs and can be vetoed to prevent it from happening.
    @event{EVT_TREELIST_ITEM_EXPANDED(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_ITEM_EXPANDED event notifying about
        the expansion of the given branch. This event is sent after the
        expansion occurs and can't be vetoed.
    @event{EVT_TREELIST_ITEM_CHECKED(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_ITEM_CHeCKED event notifying about
        the user checking or unchecking the item. You can use
        wxTreeListCtrl::GetCheckedState() to retrieve the new item state and
        wxTreeListEvent::GetOldCheckedState() to get the previous one.
    @event{EVT_TREELIST_ITEM_ACTIVATED(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_ITEM_ACTIVATED event notifying about
        the user double clicking the item or activating it from keyboard.
    @event{EVT_TREELIST_ITEM_CONTEXT_MENU(id, func)}
        Process @c wxEVT_COMMAND_TREELIST_ITEM_CONTEXT_MENU event indicating
        that the popup menu for the given item should be displayed.
    @endEventTable

    @library{wxadv}
    @category{ctrl}

    @since 2.9.3

    @see wxTreeCtrl, wxDataViewCtrl
 */
class wxTreeListCtrl : public wxWindow
{
public:
    /**
        Default constructor, call Create() later.

        This constructor is used during two-part construction process when it
        is impossible or undesirable to create the window when constructing the
        object.
     */
    wxTreeListCtrl();

    /**
        Full constructing, creating the object and its window.

        See Create() for the parameters description.
     */
    wxTreeListCtrl(wxWindow* parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxTL_DEFAULT_STYLE,
                   const wxString& name = wxTreeListCtrlNameStr);

    /**
        Create the control window.

        Can be only called for the objects created using the default
        constructor and exactly once.

        @param parent
            The parent window, must be non-NULL.
        @param id
            The window identifier, may be ::wxID_ANY.
        @param pos
            The initial window position, usually unused.
        @param size
            The initial window size, usually unused.
        @param style
            The window style, see their description in the class documentation.
        @param name
            The name of the window.
     */
    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTL_DEFAULT_STYLE,
                const wxString& name = wxTreeListCtrlNameStr);


    /**
        Image list methods.

        Like wxTreeCtrl and wxListCtrl this class uses wxImageList so if you
        intend to use item icons with it, you must construct wxImageList
        containing them first and then specify the indices of the icons in this
        image list when adding the items later.
     */
    //@{

    /// A constant indicating that no image should be used for an item.
    static const int NO_IMAGE = -1;

    /**
        Sets the image list and gives its ownership to the control.

        The image list assigned with this method will be automatically deleted
        by wxTreeCtrl as appropriate (i.e. it takes ownership of the list).

        @see SetImageList().
    */
    void AssignImageList(wxImageList* imageList);

    /**
        Sets the image list.

        The image list assigned with this method will @b not be deleted by the
        control itself and you will need to delete it yourself, use
        AssignImageList() to give the image list ownership to the control.

        @param imageList
            Image list to use, may be @NULL to not show any images any more.
    */
    void SetImageList(wxImageList* imageList);

    //@}


    /**
        Column methods.
     */
    //@{

    /**
        Add a column with the given title and attributes.

        @param title
            The column label.
        @param width
            The width of the column in pixels or the special
            wxCOL_WIDTH_AUTOSIZE value indicating that the column should adjust
            to its contents.
        @param align
            Alignment of both the column header and its items.
        @param flags
            Column flags, currently can only include wxCOL_RESIZABLE to allow
            the user to resize the column.
        @return
            Index of the new column or -1 on failure.
     */
    int AppendColumn(const wxString& title,
                     int width = wxCOL_WIDTH_AUTOSIZE,
                     wxAlignment align = wxALIGN_LEFT,
                     int flags = wxCOL_RESIZABLE);

    /// Return the total number of columns.
    unsigned GetColumnCount() const;

    /**
        Delete the column with the given index.

        @param col
            Column index in 0 to GetColumnCount() (exclusive) range.
        @return
            True if the column was deleted, false if index is invalid or
            deleting the column failed for some other reason.
     */
    bool DeleteColumn(unsigned col);

    /**
        Delete all columns.

        @see DeleteAllItems()
     */
    void ClearColumns();

    /**
        Change the width of the given column.

        Set column width to either the given value in pixels or to the value
        large enough to fit all of the items if width is wxCOL_WIDTH_AUTOSIZE.
     */
    void SetColumnWidth(unsigned col, int width);

    /// Get the current width of the given column in pixels.
    int GetColumnWidth(unsigned col) const;

    /**
        Get the width appropriate for showing the given text.

        This is typically used as second argument for AppendColumn() or with
        SetColumnWidth().
     */
    int WidthFor(const wxString& text) const;

    //@}


    /**
        Adding and removing items.

        When adding items, the parent and text of the first column of the new item
        must always be specified, the rest is optional.

        Each item can have two images: one used for closed state and another
        for opened one. Only the first one is ever used for the items that
        don't have children. And both are not set by default.

        It is also possible to associate arbitrary client data pointer with the
        new item. It will be deleted by the control when the item is deleted
        (either by an explicit DeleteItem() call or because the entire control
        is destroyed).
     */
    //@{

    /// Same as InsertItem() with wxTLI_LAST.
    wxTreeListItem AppendItem(wxTreeListItem parent,
                              const wxString& text,
                              int imageClosed = NO_IMAGE,
                              int imageOpened = NO_IMAGE,
                              wxClientData* data = NULL);

    /**
        Insert a new item into the tree.

        @param parent
            The item parent. Must be valid, may be GetRootItem().
        @param previous
            The previous item that this one should be inserted immediately
            after. It must be valid but may be one of the special values
            wxTLI_FIRST or wxTLI_LAST indicating that the item should be either
            inserted before the first child of its parent (if any) or after the
            last one.
        @param imageClosed
            The normal item image, may be NO_IMAGE to not show any image.
        @param imageOpened
            The item image shown when it's in the expanded state.
        @param data
            Optional client data pointer that can be later retrieved using
            GetItemData() and will be deleted by the tree when the item itself
            is deleted.
     */
    wxTreeListItem InsertItem(wxTreeListItem parent,
                              wxTreeListItem previous,
                              const wxString& text,
                              int imageClosed = NO_IMAGE,
                              int imageOpened = NO_IMAGE,
                              wxClientData* data = NULL);

    /// Same as InsertItem() with wxTLI_FIRST.
    wxTreeListItem PrependItem(wxTreeListItem parent,
                               const wxString& text,
                               int imageClosed = NO_IMAGE,
                               int imageOpened = NO_IMAGE,
                               wxClientData* data = NULL);

    /// Delete the specified item.
    void DeleteItem(wxTreeListItem item);

    /// Delete all tree items.
    void DeleteAllItems();

    //@}


    /**
        Methods for the tree navigation.

        The tree has an invisible root item which is the hidden parent of all
        top-level items in the tree. Starting from it it is possible to iterate
        over all tree items using GetNextItem().

        It is also possible to iterate over just the children of the given item
        by using GetFirstChild() to get the first of them and then calling
        GetNextSibling() to retrieve all the others.
     */
    //@{

    /// Return the (never shown) root item.
    wxTreeListItem GetRootItem() const;

    /**
        Return the parent of the given item.

        All the tree items visible in the tree have valid parent items, only
        the never shown root item has no parent.
     */
    wxTreeListItem GetItemParent(wxTreeListItem item) const;

    /**
        Return the first child of the given item.

        Item may be the root item.

        Return value may be invalid if the item doesn't have any children.
     */
    wxTreeListItem GetFirstChild(wxTreeListItem item) const;

    /**
        Return the next sibling of the given item.

        Return value may be invalid if there are no more siblings.
     */
    wxTreeListItem GetNextSibling(wxTreeListItem item) const;

    /**
        Return the first item in the tree.

        This is the first child of the root item.

        @see GetNextItem()
     */
    wxTreeListItem GetFirstItem() const;

    /**
        Get item after the given one in the depth-first tree-traversal order.

        Calling this function starting with the result of GetFirstItem() allows
        iterating over all items in the tree.

        The iteration stops when this function returns an invalid item, i.e.
        @code
            for ( wxTreeListItem item = tree->GetFirstItem();
                  item.IsOk();
                  item = tree->GetNextItem(item) )
            {
                ... Do something with every tree item ...
            }
        @endcode
     */
    wxTreeListItem GetNextItem(wxTreeListItem item) const;

    //@}


    /**
        Items attributes
     */
    //@{

    /**
        Return the text of the given item.

        By default, returns the text of the first column but any other one can
        be specified using @a col argument.
     */
    const wxString& GetItemText(wxTreeListItem item, unsigned col = 0) const;

    /**
        Set the text of the specified column of the given item.
     */
    void SetItemText(wxTreeListItem item, unsigned col, const wxString& text);

    /**
        Set the text of the first column of the given item.
     */
    void SetItemText(wxTreeListItem item, const wxString& text);

    /**
        Set the images for the given item.

        See InsertItem() for the images parameters descriptions.
     */
    void SetItemImage(wxTreeListItem item, int closed, int opened = NO_IMAGE);

    /**
        Get the data associated with the given item.

        The returned pointer may be @NULL.

        It must not be deleted by the caller as this will be done by the
        control itself.
     */
    wxClientData* GetItemData(wxTreeListItem item) const;

    /**
        Set the data associated with the given item.

        Previous client data, if any, is deleted when this function is called
        so it may be used to delete the current item data object and reset it
        by passing @NULL as @a data argument.
     */
    void SetItemData(wxTreeListItem item, wxClientData* data);

    //@}


    /**
        Expanding and collapsing tree branches.

        Notice that calling neither Expand() nor Collapse() method generates
        any events.
     */
    //@{

    /**
        Expand the given tree branch.
     */
    void Expand(wxTreeListItem item);

    /**
        Collapse the given tree branch.
     */
    void Collapse(wxTreeListItem item);

    /**
        Return whether the given item is expanded.
     */
    bool IsExpanded(wxTreeListItem item) const;

    //@}


    /**
        Selection methods.

        The behaviour of the control is different in single selection mode (the
        default) and multi-selection mode (if @c wxTL_MULTIPLE was specified
        when creating it). Not all methods can be used in both modes and some
        of those that can don't behave in the same way in two cases.
     */
    //@{

    /**
        Return the currently selected item.

        This method can't be used with multi-selection controls, use
        GetSelections() instead.

        The return value may be invalid if no item has been selected yet. Once
        an item in a single selection control was selected, it will keep a
        valid selection.
     */
    wxTreeListItem GetSelection() const;

    /**
        Fill in the provided array with all the selected items.

        This method can be used in both single and multi-selection case.

        The previous array contents is destroyed.

        Returns the number of selected items.
     */
    unsigned GetSelections(wxTreeListItems& selections) const;

    /**
        Select the given item.

        In single selection mode, deselects any other selected items, in
        multi-selection case it adds to the selection.
     */
    void Select(wxTreeListItem item);

    /**
        Deselect the given item.

        This method can be used in multiple selection mode only.
     */
    void Unselect(wxTreeListItem item);

    /**
        Return true if the item is selected.

        This method can be used in both single and multiple selection modes.
     */
    bool IsSelected(wxTreeListItem item) const;

    /**
        Select all the control items.

        Can be only used in multi-selection mode.
     */
    void SelectAll();

    /**
        Deselect all the control items.

        Can be only used in multi-selection mode.
     */
    void UnselectAll();

    //@}


    /**
        Checkbox handling

        Methods in this section can only be used with the controls created with
        wxTL_CHECKBOX style.
     */
    //@{

    /**
        Change the item checked state.

        @param item
            Valid non-root tree item.
        @param state
            One of wxCHK_CHECKED, wxCHK_UNCHECKED or, for the controls with
            wxTL_3STATE or wxTL_USER_3STATE styles, wxCHK_UNDETERMINED.
     */
    void CheckItem(wxTreeListItem item, wxCheckBoxState state = wxCHK_CHECKED);

    /**
        Change the checked state of the given item and all its children.

        This is the same as CheckItem() but checks or unchecks not only this
        item itself but all its children recursively as well.
     */
    void CheckItemRecursively(wxTreeListItem item,
                              wxCheckBoxState state = wxCHK_CHECKED);

    /**
        Uncheck the given item.

        This is synonymous with CheckItem(wxCHK_UNCHECKED).
     */
    void UncheckItem(wxTreeListItem item);

    /**
        Update the state of the parent item to reflect the checked state of its
        children.

        This method updates the parent of this item recursively: if this item
        and all its siblings are checked, the parent will become checked as
        well. If this item and all its siblings are unchecked, the parent will
        be unchecked. And if the siblings of this item are not all in the same
        state, the parent will be switched to indeterminate state. And then the
        same logic will be applied to the parents parent and so on recursively.

        This is typically called when the state of the given item has changed
        from EVT_TREELIST_ITEM_CHECKED() handler in the controls which have
        wxTL_3STATE flag. Notice that without this flag this function can't
        work as it would be unable to set the state of a parent with both
        checked and unchecked items so it's only allowed to call it when this
        flag is set.
     */
    void UpdateItemParentStateRecursively(wxTreeListItem item);

    /**
        Return the checked state of the item.

        The return value can be wxCHK_CHECKED, wxCHK_UNCHECKED or
        wxCHK_UNDETERMINED.
     */
    wxCheckBoxState GetCheckedState(wxTreeListItem item) const;

    /**
        Return true if all children of the given item are in the specified
        state.

        This is especially useful for the controls with @c wxTL_3STATE style to
        allow to decide whether the parent effective state should be the same
        @a state, if all its children are in it, or ::wxCHK_UNDETERMINED.

        @see UpdateItemParentStateRecursively()
     */
    bool AreAllChildrenInState(wxTreeListItem item,
                               wxCheckBoxState state) const;

    //@}
};

/**
    Event generated by wxTreeListCtrl.

    @since 2.9.3
 */
class wxTreeListEvent : public wxNotifyEvent
{
public:
    /**
        Return the item affected by the event.

        This is the item being selected, expanded, checked or activated
        (depending on the event type).
     */
    wxTreeListItem GetItem() const;

    /**
        Return the previous state of the item checkbox.

        This method can be used with @c wxEVT_COMMAND_TREELIST_ITEM_CHeCKED
        events only.

        Notice that the new state of the item can be retrieved using
        wxTreeListCtrl::GetCheckedState().
     */
    wxCheckBoxState GetOldCheckedState() const;
};


/**
    Type of wxTreeListEvent event handlers.

    This macro should be used with wxEvtHandler::Connect() when connecting to
    wxTreeListCtrl events.
 */
#define wxTreeListEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxTreeListEventFunction, func)

#endif // _WX_TREELIST_H_
