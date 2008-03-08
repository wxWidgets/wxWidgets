/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     documentation for wxTreeItemData class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTreeItemData
    @wxheader{treectrl.h}

    wxTreeItemData is some (arbitrary) user class associated with some item. The
    main advantage of having this class is that wxTreeItemData objects are
    destroyed automatically by the tree and, as this class has virtual destructor,
    it means that the memory and any other resources associated with a tree item
    will be automatically freed when it is deleted. Note that we don't use wxObject
    as the base class for wxTreeItemData because the size of this class is
    critical: in many applications, each tree leaf will have wxTreeItemData
    associated with it and the number of leaves may be quite big.

    Also please note that because the objects of this class are deleted by the tree
    using the operator @c delete, they must always be allocated on the heap
    using @c new.

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxTreeCtrl
*/
class wxTreeItemData : public wxClientData
{
public:
    /**
        Default constructor.
        In addition, the following methods are added in wxPython for accessing
        the object:
        
        
        
        @b GetData()
        
        
        Returns a reference to the Python Object
        
        @b SetData(obj)
        
        
        Associates a new Python Object with the
        wxTreeItemData
    */
    wxTreeItemData();

    /**
        Virtual destructor.
    */
    ~wxTreeItemData();

    /**
        Returns the item associated with this node.
    */
    const wxTreeItemId GetId();

    /**
        Sets the item associated with this node.
    */
    void SetId(const wxTreeItemId& id);
};


/**
    @class wxTreeCtrl
    @wxheader{treectrl.h}

    A tree control presents information as a hierarchy, with items that may be
    expanded
    to show further items. Items in a tree control are referenced by wxTreeItemId
    handles,
    which may be tested for validity by calling wxTreeItemId::IsOk.

    To intercept events from a tree control, use the event table macros described
    in wxTreeEvent.

    @beginStyleTable
    @style{wxTR_EDIT_LABELS}:
           Use this style if you wish the user to be able to edit labels in
           the tree control.
    @style{wxTR_NO_BUTTONS}:
           For convenience to document that no buttons are to be drawn.
    @style{wxTR_HAS_BUTTONS}:
           Use this style to show + and - buttons to the left of parent items.
    @style{wxTR_NO_LINES}:
           Use this style to hide vertical level connectors.
    @style{wxTR_FULL_ROW_HIGHLIGHT}:
           Use this style to have the background colour and the selection
           highlight extend over the entire horizontal row of the tree control
           window. (This flag is ignored under Windows unless you specify
           wxTR_NO_LINES as well.)
    @style{wxTR_LINES_AT_ROOT}:
           Use this style to show lines between root nodes. Only applicable if
           wxTR_HIDE_ROOT is set and wxTR_NO_LINES is not set.
    @style{wxTR_HIDE_ROOT}:
           Use this style to suppress the display of the root node,
           effectively causing the first-level nodes to appear as a series of
           root nodes.
    @style{wxTR_ROW_LINES}:
           Use this style to draw a contrasting border between displayed rows.
    @style{wxTR_HAS_VARIABLE_ROW_HEIGHT}:
           Use this style to cause row heights to be just big enough to fit
           the content. If not set, all rows use the largest row height. The
           default is that this flag is unset. Generic only.
    @style{wxTR_SINGLE}:
           For convenience to document that only one item may be selected at a
           time. Selecting another item causes the current selection, if any,
           to be deselected.  This is the default.
    @style{wxTR_MULTIPLE}:
           Use this style to allow a range of items to be selected. If a
           second range is selected, the current range, if any, is deselected.
    @style{wxTR_DEFAULT_STYLE}:
           The set of flags that are closest to the defaults for the native
           control for a particular toolkit.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    @appearance{treectrl.png}

    @seealso
    wxTreeItemData, @ref overview_wxtreectrloverview "wxTreeCtrl overview",
    wxListBox, wxListCtrl, wxImageList, wxTreeEvent
*/
class wxTreeCtrl : public wxControl
{
public:
    //@{
    /**
        Constructor, creating and showing a tree control.
        
        @param parent
        Parent window. Must not be @NULL.
        
        @param id
        Window identifier. The value wxID_ANY indicates a default value.
        
        @param pos
        Window position.
        
        @param size
        Window size. If wxDefaultSize is specified then the window is sized
        appropriately.
        
        @param style
        Window style. See wxTreeCtrl.
        
        @param validator
        Window validator.
        
        @param name
        Window name.
        
        @sa Create(), wxValidator
    */
    wxTreeCtrl();
    wxTreeCtrl(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "treeCtrl");
    //@}

    /**
        Destructor, destroying the tree control.
    */
    ~wxTreeCtrl();

    /**
        Adds the root node to the tree, returning the new item.
        
        The @e image and @e selImage parameters are an index within
        the normal image list specifying the image to use for unselected and
        selected items, respectively.
        If @e image  -1 and @e selImage is -1, the same image is used for
        both selected and unselected items.
    */
    wxTreeItemId AddRoot(const wxString& text, int image = -1,
                         int selImage = -1,
                         wxTreeItemData* data = @NULL);

    /**
        Appends an item to the end of the branch identified by @e parent, return a new
        item id.
        
        The @e image and @e selImage parameters are an index within
        the normal image list specifying the image to use for unselected and
        selected items, respectively.
        If @e image  -1 and @e selImage is -1, the same image is used for
        both selected and unselected items.
    */
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = @NULL);

    /**
        Sets the buttons image list. The button images assigned with this method will
        be automatically deleted by wxTreeCtrl as appropriate
        (i.e. it takes ownership of the list).
        
        Setting or assigning the button image list enables the display of image buttons.
        Once enabled, the only way to disable the display of button images is to set
        the button image list to @NULL.
        
        This function is only available in the generic version.
        
        See also SetButtonsImageList().
    */
    void AssignButtonsImageList(wxImageList* imageList);

    /**
        Sets the normal image list. Image list assigned with this method will
        be automatically deleted by wxTreeCtrl as appropriate
        (i.e. it takes ownership of the list).
        
        See also SetImageList().
    */
    void AssignImageList(wxImageList* imageList);

    /**
        Sets the state image list. Image list assigned with this method will
        be automatically deleted by wxTreeCtrl as appropriate
        (i.e. it takes ownership of the list).
        
        See also SetStateImageList().
    */
    void AssignStateImageList(wxImageList* imageList);

    /**
        Collapses the given item.
    */
    void Collapse(const wxTreeItemId& item);

    /**
        Collapses the root item.
        
        @sa ExpandAll()
    */
    void CollapseAll();

    /**
        Collapses this item and all of its children, recursively.
        
        @sa ExpandAllChildren()
    */
    void CollapseAllChildren(const wxTreeItemId& item);

    /**
        Collapses the given item and removes all children.
    */
    void CollapseAndReset(const wxTreeItemId& item);

    /**
        Creates the tree control. See wxTreeCtrl() for further details.
    */
    bool wxTreeCtrl(wxWindow* parent, wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTR_HAS_BUTTONS,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = "treeCtrl");

    /**
        Deletes the specified item. A @c EVT_TREE_DELETE_ITEM event will be
        generated.
        
        This function may cause a subsequent call to GetNextChild to fail.
    */
    void Delete(const wxTreeItemId& item);

    /**
        Deletes all items in the control. Note that this may not generate
        @c EVT_TREE_DELETE_ITEM events under some Windows versions although
        normally such event is generated for each removed item.
    */
    void DeleteAllItems();

    /**
        Deletes all children of the given item (but not the item itself). Note that
        this will @b not generate any events unlike
        Delete() method.
        
        If you have called SetItemHasChildren(), you
        may need to call it again since @e DeleteChildren does not automatically
        clear the setting.
    */
    void DeleteChildren(const wxTreeItemId& item);

    /**
        Starts editing the label of the given item. This function generates a
        EVT_TREE_BEGIN_LABEL_EDIT event which can be vetoed so that no
        text control will appear for in-place editing.
        
        If the user changed the label (i.e. s/he does not press ESC or leave
        the text control without changes, a EVT_TREE_END_LABEL_EDIT event
        will be sent which can be vetoed as well.
        
        @sa EndEditLabel(), wxTreeEvent
    */
    void EditLabel(const wxTreeItemId& item);

    /**
        Ends label editing. If @e cancelEdit is @true, the edit will be cancelled.
        
        This function is currently supported under Windows only.
        
        @sa EditLabel()
    */
    void EndEditLabel(bool cancelEdit);

    /**
        Scrolls and/or expands items to ensure that the given item is visible.
    */
    void EnsureVisible(const wxTreeItemId& item);

    /**
        Expands the given item.
    */
    void Expand(const wxTreeItemId& item);

    /**
        Expands all items in the tree.
    */
    void ExpandAll();

    /**
        Expands the given item and all its children recursively.
    */
    void ExpandAllChildren(const wxTreeItemId& item);

    /**
        Retrieves the rectangle bounding the @e item. If @e textOnly is @true,
        only the rectangle around the item's label will be returned, otherwise the
        item's image is also taken into account.
        
        The return value is @true if the rectangle was successfully retrieved or @c
        @false
        if it was not (in this case @e rect is not changed) -- for example, if the
        item is currently invisible.
        
        Notice that the rectangle coordinates are logical, not physical ones. So, for
        example, the x coordinate may be negative if the tree has a horizontal
        scrollbar and its position is not 0.
    */
    bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect,
                         bool textOnly = @false);

    /**
        Returns the buttons image list (from which application-defined button images
        are taken).
        
        This function is only available in the generic version.
    */
    wxImageList* GetButtonsImageList();

    /**
        Returns the number of items in the branch. If @e recursively is @true,
        returns the total number
        of descendants, otherwise only one level of children is counted.
    */
    unsigned int GetChildrenCount(const wxTreeItemId& item,
                                  bool recursively = @true);

    /**
        Returns the number of items in the control.
    */
    unsigned int GetCount();

    /**
        Returns the edit control being currently used to edit a label. Returns @NULL
        if no label is being edited.
        
        @b NB: It is currently only implemented for wxMSW.
    */
    wxTextCtrl * GetEditControl();

    /**
        Returns the first child; call GetNextChild() for the next child.
        
        For this enumeration function you must pass in a 'cookie' parameter
        which is opaque for the application but is necessary for the library
        to make these functions reentrant (i.e. allow more than one
        enumeration on one and the same object simultaneously). The cookie passed to
        GetFirstChild and GetNextChild should be the same variable.
        
        Returns an invalid tree item (i.e. IsOk() returns @false) if there are no
        further children.
        
        @sa GetNextChild(), GetNextSibling()
    */
    wxTreeItemId GetFirstChild(const wxTreeItemId& item,
                               wxTreeItemIdValue & cookie);

    /**
        Returns the first visible item.
    */
    wxTreeItemId GetFirstVisibleItem();

    /**
        Returns the normal image list.
    */
    wxImageList* GetImageList();

    /**
        Returns the current tree control indentation.
    */
    int GetIndent();

    /**
        Returns the background colour of the item.
    */
    wxColour GetItemBackgroundColour(const wxTreeItemId& item);

    //@{
    /**
        Returns the font of the item label.
    */
    wxTreeItemData* GetItemData(const wxTreeItemId& item);
See also wxPython note:
wxPython provides the following shortcut method:







    GetPyData(item)




    Returns the Python Object
    associated with the wxTreeItemData for the given item Id.







    wxFont  GetItemFont(const wxTreeItemId& item);
    //@}

    /**
        Gets the specified item image. The value of @e which may be:
        
        wxTreeItemIcon_Normal to get the normal item image
        wxTreeItemIcon_Selected to get the selected item image (i.e. the image
        which is shown when the item is currently selected)
        wxTreeItemIcon_Expanded to get the expanded image (this only
        makes sense for items which have children - then this image is shown when the
        item is expanded and the normal image is shown when it is collapsed)
        wxTreeItemIcon_SelectedExpanded to get the selected expanded image
        (which is shown when an expanded item is currently selected)
    */
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal);

    /**
        Returns the item's parent.
    */
    wxTreeItemId GetItemParent(const wxTreeItemId& item);

    /**
        Gets the selected item image (this function is obsolete, use
        @c GetItemImage(item, wxTreeItemIcon_Selected) instead).
    */
    int GetItemSelectedImage(const wxTreeItemId& item);

    /**
        Returns the item label.
    */
    wxString GetItemText(const wxTreeItemId& item);

    /**
        Returns the colour of the item label.
    */
    wxColour GetItemTextColour(const wxTreeItemId& item);

    /**
        Returns the last child of the item (or an invalid tree item if this item has no
        children).
        
        @sa GetFirstChild(), GetNextSibling(),
              GetLastChild()
    */
    wxTreeItemId GetLastChild(const wxTreeItemId& item);

    /**
        Returns the next child; call GetFirstChild() for the first child.
        
        For this enumeration function you must pass in a 'cookie' parameter
        which is opaque for the application but is necessary for the library
        to make these functions reentrant (i.e. allow more than one
        enumeration on one and the same object simultaneously). The cookie passed to
        GetFirstChild and GetNextChild should be the same.
        
        Returns an invalid tree item if there are no further children.
        
        @sa GetFirstChild()
    */
    wxTreeItemId GetNextChild(const wxTreeItemId& item,
                              wxTreeItemIdValue & cookie);

    /**
        Returns the next sibling of the specified item; call GetPrevSibling() for the
        previous sibling.
        
        Returns an invalid tree item if there are no further siblings.
        
        @sa GetPrevSibling()
    */
    wxTreeItemId GetNextSibling(const wxTreeItemId& item);

    /**
        Returns the next visible item or an invalid item if this item is the last
        visible one.
        
        Notice that the @e item itself must be visible.
    */
    wxTreeItemId GetNextVisible(const wxTreeItemId& item);

    /**
        Returns the previous sibling of the specified item; call GetNextSibling() for
        the next sibling.
        
        Returns an invalid tree item if there are no further children.
        
        @sa GetNextSibling()
    */
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item);

    /**
        Returns the previous visible item or an invalid item if this item is the first
        visible one.
        
        Notice that the @e item itself must be visible.
    */
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item);

    /**
        Returns @true if the control will use a quick calculation for the best size,
        looking only at the first and last items. The default is @false.
        
        @sa SetQuickBestSize()
    */
    bool GetQuickBestSize();

    /**
        Returns the root item for the tree control.
    */
    wxTreeItemId GetRootItem();

    /**
        Returns the selection, or an invalid item if there is no selection.
        This function only works with the controls without wxTR_MULTIPLE style, use
        GetSelections() for the controls which do have
        this style.
    */
    wxTreeItemId GetSelection();

    /**
        Fills the array of tree items passed in with the currently selected items. This
        function can be called only if the control has the wxTR_MULTIPLE style.
        
        Returns the number of selected items.
    */
    unsigned int GetSelections(wxArrayTreeItemIds& selection);

    /**
        Returns the state image list (from which application-defined state images are
        taken).
    */
    wxImageList* GetStateImageList();

    /**
        Calculates which (if any) item is under the given point, returning the tree item
        id at this point plus extra information @e flags. @e flags is a bitlist of the
        following:
        
        
        wxTREE_HITTEST_ABOVE
        
        
        Above the client area.
        
        wxTREE_HITTEST_BELOW
        
        
        Below the client area.
        
        wxTREE_HITTEST_NOWHERE
        
        
        In the client area but below the last item.
        
        wxTREE_HITTEST_ONITEMBUTTON
        
        
        On the button associated with an item.
        
        wxTREE_HITTEST_ONITEMICON
        
        
        On the bitmap associated with an item.
        
        wxTREE_HITTEST_ONITEMINDENT
        
        
        In the indentation associated with an item.
        
        wxTREE_HITTEST_ONITEMLABEL
        
        
        On the label (string) associated with an item.
        
        wxTREE_HITTEST_ONITEMRIGHT
        
        
        In the area to the right of an item.
        
        wxTREE_HITTEST_ONITEMSTATEICON
        
        
        On the state icon for a tree view item that is in a user-defined state.
        
        wxTREE_HITTEST_TOLEFT
        
        
        To the right of the client area.
        
        wxTREE_HITTEST_TORIGHT
        
        
        To the left of the client area.
    */
    wxTreeItemId HitTest(const wxPoint& point, int& flags);

    //@{
    /**
        Inserts an item after a given one (@e previous) or before one identified by its
        position (@e before).
        @e before must be less than the number of children.
        
        The @e image and @e selImage parameters are an index within
        the normal image list specifying the image to use for unselected and
        selected items, respectively.
        If @e image  -1 and @e selImage is -1, the same image is used for
        both selected and unselected items.
    */
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& previous,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = @NULL);
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            size_t before,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = @NULL);
    //@}

    /**
        Returns @true if the given item is in bold state.
        
        See also: SetItemBold()
    */
    bool IsBold(const wxTreeItemId& item);

    /**
        Returns @true if the control is empty (i.e. has no items, even no root one).
    */
    bool IsEmpty();

    /**
        Returns @true if the item is expanded (only makes sense if it has children).
    */
    bool IsExpanded(const wxTreeItemId& item);

    /**
        Returns @true if the item is selected.
    */
    bool IsSelected(const wxTreeItemId& item);

    /**
        Returns @true if the item is visible on the screen.
    */
    bool IsVisible(const wxTreeItemId& item);

    /**
        Returns @true if the item has children.
    */
    bool ItemHasChildren(const wxTreeItemId& item);

    /**
        Override this function in the derived class to change the sort order of the
        items in the tree control. The function should return a negative, zero or
        positive value if the first item is less than, equal to or greater than the
        second one.
        
        Please note that you @b must use wxRTTI macros
        DECLARE_DYNAMIC_CLASS and
        IMPLEMENT_DYNAMIC_CLASS if you override this
        function because otherwise the base class considers that it is not overridden
        and uses the default comparison, i.e. sorts the items alphabetically, which
        allows it optimize away the calls to the virtual function completely.
        
        See also: SortChildren()
    */
    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2);

    /**
        Appends an item as the first child of @e parent, return a new item id.
        
        The @e image and @e selImage parameters are an index within
        the normal image list specifying the image to use for unselected and
        selected items, respectively.
        If @e image  -1 and @e selImage is -1, the same image is used for
        both selected and unselected items.
    */
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1,
                             int selImage = -1,
                             wxTreeItemData* data = @NULL);

    /**
        Scrolls the specified item into view.
    */
    void ScrollTo(const wxTreeItemId& item);

    /**
        Selects the given item. In multiple selection controls, can be also used to
        deselect a currently selected item if the value of @e select is @false.
    */
    void SelectItem(const wxTreeItemId& item, bool select = @true);

    /**
        Sets the buttons image list (from which application-defined button images are
        taken).
        The button images assigned with this method will
        @b not be deleted by wxTreeCtrl's destructor, you must delete it yourself.
        
        Setting or assigning the button image list enables the display of image buttons.
        Once enabled, the only way to disable the display of button images is to set
        the button image list to @NULL.
        
        This function is only available in the generic version.
        
        See also AssignButtonsImageList().
    */
    void SetButtonsImageList(wxImageList* imageList);

    /**
        Sets the normal image list. Image list assigned with this method will
        @b not be deleted by wxTreeCtrl's destructor, you must delete it yourself.
        
        See also AssignImageList().
    */
    void SetImageList(wxImageList* imageList);

    /**
        Sets the indentation for the tree control.
    */
    void SetIndent(int indent);

    /**
        Sets the colour of the item's background.
    */
    void SetItemBackgroundColour(const wxTreeItemId& item,
                                 const wxColour& col);

    /**
        Makes item appear in bold font if @e bold parameter is @true or resets it to
        the normal state.
        
        See also: IsBold()
    */
    void SetItemBold(const wxTreeItemId& item, bool bold = @true);

    //@{
    /**
        Gives the item the visual feedback for Drag'n'Drop actions, which is
        useful if something is dragged from the outside onto the tree control
        (as opposed to a DnD operation within the tree control, which already
        is implemented internally).
    */
    void SetItemData(const wxTreeItemId& item, wxTreeItemData* data);
wxPython note:
    SetPyData(item, obj)




    Associate the given Python
    Object with the wxTreeItemData for the given item Id.







    void SetItemDropHighlight(const wxTreeItemId& item,
                              bool highlight = @true);
    //@}

    /**
        Sets the item's font. All items in the tree should have the same height to avoid
        text clipping, so the fonts height should be the same for all of them,
        although font attributes may vary.
        
        @sa SetItemBold()
    */
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);

    /**
        Force appearance of the button next to the item. This is useful to
        allow the user to expand the items which don't have any children now,
        but instead adding them only when needed, thus minimizing memory
        usage and loading time.
    */
    void SetItemHasChildren(const wxTreeItemId& item,
                            bool hasChildren = @true);

    /**
        Sets the specified item image. See GetItemImage()
        for the description of the @e which parameter.
    */
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

    /**
        Sets the selected item image (this function is obsolete, use
        @c SetItemImage(item, wxTreeItemIcon_Selected) instead).
    */
    void SetItemSelectedImage(const wxTreeItemId& item, int selImage);

    /**
        Sets the item label.
    */
    void SetItemText(const wxTreeItemId& item, const wxString& text);

    /**
        Sets the colour of the item's text.
    */
    void SetItemTextColour(const wxTreeItemId& item,
                           const wxColour& col);

    /**
        If @true is passed, specifies that the control will use a quick calculation for
        the best size,
        looking only at the first and last items. Otherwise, it will look at all items.
        The default is @false.
        
        @sa GetQuickBestSize()
    */
    void SetQuickBestSize(bool quickBestSize);

    /**
        Sets the state image list (from which application-defined state images are
        taken).
        Image list assigned with this method will
        @b not be deleted by wxTreeCtrl's destructor, you must delete it yourself.
        
        See also AssignStateImageList().
    */
    void SetStateImageList(wxImageList* imageList);

    /**
        Sets the mode flags associated with the display of the tree control.
        The new mode takes effect immediately.
        (Generic only; MSW ignores changes.)
    */
    void SetWindowStyle(long styles);

    /**
        Sorts the children of the given item using
        OnCompareItems() method of wxTreeCtrl. You
        should override that method to change the sort order (the default is ascending
        case-sensitive alphabetical order).
        
        @sa wxTreeItemData, OnCompareItems()
    */
    void SortChildren(const wxTreeItemId& item);

    /**
        Toggles the given item between collapsed and expanded states.
    */
    void Toggle(const wxTreeItemId& item);

    /**
        Toggles the given item between selected and unselected states. For
        multiselection controls only.
    */
    void ToggleItemSelection(const wxTreeItemId& item);

    /**
        Removes the selection from the currently selected item (if any).
    */
    void Unselect();

    /**
        This function either behaves the same as Unselect()
        if the control doesn't have wxTR_MULTIPLE style, or removes the selection from
        all items if it does have this style.
    */
    void UnselectAll();

    /**
        Unselects the given item. This works in multiselection controls only.
    */
    void UnselectItem(const wxTreeItemId& item);
};


/**
    @class wxTreeEvent
    @wxheader{treectrl.h}

    A tree event holds information about events associated with wxTreeCtrl objects.

    @library{wxbase}
    @category{events}

    @seealso
    wxTreeCtrl
*/
class wxTreeEvent : public wxNotifyEvent
{
public:
    /**
        )
        
        Constructor, used by wxWidgets itself only.
    */
    wxTreeEvent(wxEventType commandType, wxTreeCtrl * tree);

    /**
        Returns the item (valid for all events).
    */
    wxTreeItemId GetItem();

    /**
        Returns the key code if the event is a key event. Use
        GetKeyEvent() to get the values of the
        modifier keys for this event (i.e. Shift or Ctrl).
    */
    int GetKeyCode();

    /**
        Returns the key event for @c EVT_TREE_KEY_DOWN events.
    */
    const wxKeyEvent GetKeyEvent();

    /**
        Returns the label if the event is a begin or end edit label event.
    */
    const wxString GetLabel();

    /**
        Returns the old item index (valid for EVT_TREE_ITEM_CHANGING and CHANGED events)
    */
    wxTreeItemId GetOldItem();

    /**
        Returns the position of the mouse pointer if the event is a drag or
        menu-context event.
        In both cases the position is in client coordinates - i.e. relative to the
        wxTreeCtrl
        window (so that you can pass it directly to e.g. wxWindow::PopupMenu).
    */
    wxPoint GetPoint();

    /**
        Returns @true if the label edit was cancelled. This should be
        called from within an EVT_TREE_END_LABEL_EDIT handler.
    */
    bool IsEditCancelled();

    /**
        Set the tooltip for the item (valid for EVT_TREE_ITEM_GETTOOLTIP events).
        Windows only.
    */
    void SetToolTip(const wxString& tooltip);
};
