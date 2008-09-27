/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     interface of wxTreeItemData
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    @class wxTreeCtrl

    A tree control presents information as a hierarchy, with items that may be
    expanded to show further items. Items in a tree control are referenced by
    wxTreeItemId handles, which may be tested for validity by calling
    wxTreeItemId::IsOk().

    A similar control with a fully native implemtation for GTK+ and OS X
    as well is wxDataViewTreeCtrl.

    To intercept events from a tree control, use the event table macros
    described in wxTreeEvent.

    @beginStyleTable
    @style{wxTR_EDIT_LABELS}
        Use this style if you wish the user to be able to edit labels in the
        tree control.
    @style{wxTR_NO_BUTTONS}
        For convenience to document that no buttons are to be drawn.
    @style{wxTR_HAS_BUTTONS}
        Use this style to show + and - buttons to the left of parent items.
    @style{wxTR_NO_LINES}
        Use this style to hide vertical level connectors.
    @style{wxTR_FULL_ROW_HIGHLIGHT}
        Use this style to have the background colour and the selection highlight
        extend over the entire horizontal row of the tree control window. (This
        flag is ignored under Windows unless you specify @c wxTR_NO_LINES as
        well.)
    @style{wxTR_LINES_AT_ROOT}
        Use this style to show lines between root nodes. Only applicable if @c
        wxTR_HIDE_ROOT is set and @c wxTR_NO_LINES is not set.
    @style{wxTR_HIDE_ROOT}
        Use this style to suppress the display of the root node, effectively
        causing the first-level nodes to appear as a series of root nodes.
    @style{wxTR_ROW_LINES}
        Use this style to draw a contrasting border between displayed rows.
    @style{wxTR_HAS_VARIABLE_ROW_HEIGHT}
        Use this style to cause row heights to be just big enough to fit the
        content. If not set, all rows use the largest row height. The default is
        that this flag is unset. Generic only.
    @style{wxTR_SINGLE}
        For convenience to document that only one item may be selected at a
        time. Selecting another item causes the current selection, if any, to be
        deselected. This is the default.
    @style{wxTR_MULTIPLE}
        Use this style to allow a range of items to be selected. If a second
        range is selected, the current range, if any, is deselected.
    @style{wxTR_DEFAULT_STYLE}
        The set of flags that are closest to the defaults for the native control
        for a particular toolkit.
    @endStyleTable

    @beginEventTable{wxTreeEvent}
    @event{EVT_TREE_BEGIN_DRAG(id, func)}
          Begin dragging with the left mouse button.
    @event{EVT_TREE_BEGIN_RDRAG(id, func)}
          Begin dragging with the right mouse button.
    @event{EVT_TREE_END_DRAG(id, func)}
          End dragging with the left or right mouse button.
    @event{EVT_TREE_BEGIN_LABEL_EDIT(id, func)}
          Begin editing a label. This can be prevented by calling Veto().
    @event{EVT_TREE_END_LABEL_EDIT(id, func)}
          Finish editing a label. This can be prevented by calling Veto().
    @event{EVT_TREE_DELETE_ITEM(id, func)}
          An item was deleted.
    @event{EVT_TREE_GET_INFO(id, func)}
          Request information from the application.
    @event{EVT_TREE_SET_INFO(id, func)}
          Information is being supplied.
    @event{EVT_TREE_ITEM_ACTIVATED(id, func)}
          The item has been activated, i.e. chosen by double clicking it with mouse or from keyboard.
    @event{EVT_TREE_ITEM_COLLAPSED(id, func)}
          The item has been collapsed.
    @event{EVT_TREE_ITEM_COLLAPSING(id, func)}
          The item is being collapsed. This can be prevented by calling Veto().
    @event{EVT_TREE_ITEM_EXPANDED(id, func)}
          The item has been expanded.
    @event{EVT_TREE_ITEM_EXPANDING(id, func)}
          The item is being expanded. This can be prevented by calling Veto().
    @event{EVT_TREE_ITEM_RIGHT_CLICK(id, func)}
          The user has clicked the item with the right mouse button.
    @event{EVT_TREE_ITEM_MIDDLE_CLICK(id, func)}
          The user has clicked the item with the middle mouse button.
    @event{EVT_TREE_SEL_CHANGED(id, func)}
          Selection has changed.
    @event{EVT_TREE_SEL_CHANGING(id, func)}
          Selection is changing. This can be prevented by calling Veto().
    @event{EVT_TREE_KEY_DOWN(id, func)}
          A key has been pressed.
    @event{EVT_TREE_ITEM_GETTOOLTIP(id, func)}
          The opportunity to set the item tooltip is being given to the application (call wxTreeEvent::SetToolTip). Windows only.
    @event{EVT_TREE_ITEM_MENU(id, func)}
          The context menu for the selected item has been requested, either by a right click or by using the menu key.
    @event{EVT_TREE_STATE_IMAGE_CLICK(id, func)}
          The state image has been clicked. Windows only.
    @endEventTable


    See also @ref overview_windowstyles.

    @b Win32 @b notes:

    wxTreeCtrl class uses the standard common treeview control under Win32
    implemented in the system library comctl32.dll. Some versions of this
    library are known to have bugs with handling the tree control colours: the
    usual symptom is that the expanded items leave black (or otherwise
    incorrectly coloured) background behind them, especially for the controls
    using non-default background colour. The recommended solution is to upgrade
    the comctl32.dll to a newer version: see
    http://www.microsoft.com/downloads/details.aspx?familyid=cb2cf3a2-8025-4e8f-8511-9b476a8d35d2

    @library{wxcore}
    @category{ctrl}
    <!-- @appearance{treectrl.png} -->

    @see wxDataViewTreeCtrl, wxTreeEvent, wxTreeItemData, @ref overview_treectrl, wxListBox,
         wxListCtrl, wxImageList
*/
class wxTreeCtrl : public wxControl
{
public:
    /**
        Default Constructor.
    */
    wxTreeCtrl();

    /**
        Constructor, creating and showing a tree control.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value @c wxID_ANY indicates a default value.
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

        @see Create(), wxValidator
    */
    wxTreeCtrl(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "treeCtrl");


    /**
        Destructor, destroying the tree control.
    */
    virtual ~wxTreeCtrl();

    /**
        Adds the root node to the tree, returning the new item.

        The @a image and @a selImage parameters are an index within the normal
        image list specifying the image to use for unselected and selected
        items, respectively. If @a image -1 and @a selImage is -1, the same
        image is used for both selected and unselected items.
    */
    virtual wxTreeItemId AddRoot(const wxString& text, int image = -1,
                                 int selImage = -1,
                                 wxTreeItemData* data = NULL);

    /**
        Appends an item to the end of the branch identified by @a parent, return
        a new item id.

        The @a image and @a selImage parameters are an index within the normal
        image list specifying the image to use for unselected and selected
        items, respectively. If @a image -1 and @a selImage is -1, the same
        image is used for both selected and unselected items.
    */
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = NULL);

    /**
        Sets the buttons image list. The button images assigned with this method
        will be automatically deleted by wxTreeCtrl as appropriate (i.e. it
        takes ownership of the list).

        Setting or assigning the button image list enables the display of image
        buttons. Once enabled, the only way to disable the display of button
        images is to set the button image list to @NULL.

        This function is only available in the generic version.

        @see SetButtonsImageList().
    */
    void AssignButtonsImageList(wxImageList* imageList);

    /**
        Sets the normal image list. The image list assigned with this method
        will be automatically deleted by wxTreeCtrl as appropriate (i.e. it
        takes ownership of the list).

        @see SetImageList().
    */
    void AssignImageList(wxImageList* imageList);

    /**
        Sets the state image list. Image list assigned with this method will be
        automatically deleted by wxTreeCtrl as appropriate (i.e. it takes
        ownership of the list).

        @see SetStateImageList().
    */
    void AssignStateImageList(wxImageList* imageList);

    /**
        Collapses the given item.
    */
    virtual void Collapse(const wxTreeItemId& item);

    /**
        Collapses the root item.

        @see ExpandAll()
    */
    void CollapseAll();

    /**
        Collapses this item and all of its children, recursively.

        @see ExpandAllChildren()
    */
    void CollapseAllChildren(const wxTreeItemId& item);

    /**
        Collapses the given item and removes all children.
    */
    virtual void CollapseAndReset(const wxTreeItemId& item);

    /**
        Creates the tree control. See wxTreeCtrl::wxTreeCtrl() for further
        details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "treeCtrl");

    /**
        Deletes the specified item. A EVT_TREE_DELETE_ITEM() event will be
        generated.

        This function may cause a subsequent call to GetNextChild() to fail.
    */
    virtual void Delete(const wxTreeItemId& item);

    /**
        Deletes all items in the control. Note that this may not generate
        EVT_TREE_DELETE_ITEM() events under some Windows versions although
        normally such event is generated for each removed item.
    */
    virtual void DeleteAllItems();

    /**
        Deletes all children of the given item (but not the item itself). Note
        that this will @b not generate any events unlike Delete() method.

        If you have called SetItemHasChildren(), you may need to call it again
        since DeleteChildren() does not automatically clear the setting.
    */
    virtual void DeleteChildren(const wxTreeItemId& item);

    /**
        Starts editing the label of the given @a item. This function generates a
        EVT_TREE_BEGIN_LABEL_EDIT() event which can be vetoed so that no text
        control will appear for in-place editing.

        If the user changed the label (i.e. s/he does not press ESC or leave the
        text control without changes, a EVT_TREE_END_LABEL_EDIT() event will be
        sent which can be vetoed as well.

        @see EndEditLabel(), wxTreeEvent
    */
    void EditLabel(const wxTreeItemId& item);

    /**
        Ends label editing. If @a cancelEdit is @true, the edit will be
        cancelled.

        @note
            This function is currently supported under Windows only.

        @see EditLabel()
    */
    void EndEditLabel(bool cancelEdit);

    /**
        Scrolls and/or expands items to ensure that the given item is visible.
    */
    virtual void EnsureVisible(const wxTreeItemId& item);

    /**
        Expands the given item.
    */
    virtual void Expand(const wxTreeItemId& item);

    /**
        Expands all items in the tree.
    */
    void ExpandAll();

    /**
        Expands the given item and all its children recursively.
    */
    void ExpandAllChildren(const wxTreeItemId& item);

    /**
        Retrieves the rectangle bounding the @a item. If @a textOnly is @true,
        only the rectangle around the item's label will be returned, otherwise
        the item's image is also taken into account.

        The return value is @true if the rectangle was successfully retrieved or
        @c @false if it was not (in this case @a rect is not changed) -- for
        example, if the item is currently invisible.

        Notice that the rectangle coordinates are logical, not physical ones.
        So, for example, the x coordinate may be negative if the tree has a
        horizontal scrollbar and its position is not 0.

        @beginWxPythonOnly
        The wxPython version of this method requires only the @a item and @a
        textOnly parameters. The return value is either a wxRect object or @c
        None.
        @endWxPythonOnly
    */
    bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect,
                         bool textOnly = false) const;

    /**
        Returns the buttons image list (from which application-defined button
        images are taken).

        This function is only available in the generic version.
    */
    wxImageList* GetButtonsImageList() const;

    /**
        Returns the number of items in the branch. If @a recursively is @true,
        returns the total number of descendants, otherwise only one level of
        children is counted.
    */
    unsigned int GetChildrenCount(const wxTreeItemId& item,
                                  bool recursively = true) const;

    /**
        Returns the number of items in the control.
    */
    virtual unsigned int GetCount() const;

    /**
        Returns the edit control being currently used to edit a label. Returns
        @NULL if no label is being edited.

        @note This is currently only implemented for wxMSW.
    */
    virtual wxTextCtrl* GetEditControl() const;

    /**
        Returns the first child; call GetNextChild() for the next child.

        For this enumeration function you must pass in a 'cookie' parameter
        which is opaque for the application but is necessary for the library to
        make these functions reentrant (i.e. allow more than one enumeration on
        one and the same object simultaneously). The cookie passed to
        GetFirstChild() and GetNextChild() should be the same variable.

        Returns an invalid tree item (i.e. wxTreeItemId::IsOk() returns @false)
        if there are no further children.

        @beginWxPythonOnly
        In wxPython the returned wxTreeItemId and the new cookie value are both
        returned as a tuple containing the two values.
        @endWxPythonOnly

        @see GetNextChild(), GetNextSibling()
    */
    wxTreeItemId GetFirstChild(const wxTreeItemId& item,
                               wxTreeItemIdValue& cookie) const;

    /**
        Returns the first visible item.
    */
    virtual wxTreeItemId GetFirstVisibleItem() const;

    /**
        Returns the normal image list.
    */
    wxImageList* GetImageList() const;

    /**
        Returns the current tree control indentation.
    */
    int GetIndent() const;

    /**
        Returns the background colour of the item.
    */
    virtual wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;

    /**
        Returns the tree item data associated with the item.

        @see wxTreeItemData

        @beginWxPythonOnly
        wxPython provides the following shortcut method:
        @li GetPyData(item): Returns the Python Object associated with the
            wxTreeItemData for the given item Id.
        @endWxPythonOnly
    */
    virtual wxTreeItemData* GetItemData(const wxTreeItemId& item) const;

    /**
        Returns the font of the item label.
    */
    virtual wxFont GetItemFont(const wxTreeItemId& item) const;

    /**
        Gets the specified item image. The value of @a which may be:
        - ::wxTreeItemIcon_Normal: to get the normal item image.
        - ::wxTreeItemIcon_Selected: to get the selected item image (i.e. the
            image which is shown when the item is currently selected).
        - ::wxTreeItemIcon_Expanded: to get the expanded image (this only makes
            sense for items which have children - then this image is shown when
            the item is expanded and the normal image is shown when it is
            collapsed).
        - ::wxTreeItemIcon_SelectedExpanded: to get the selected expanded image
            (which is shown when an expanded item is currently selected).
    */
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

    /**
        Returns the item's parent.
    */
    virtual wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

    /**
        Gets the selected item image (this function is obsolete, use @ref
        GetItemImage() "GetItemImage"( @a item, ::wxTreeItemIcon_Selected)
        instead).
    */
    int GetItemSelectedImage(const wxTreeItemId& item) const;

    /**
        Gets the specified item state.
    */
    int GetItemState(const wxTreeItemId& item) const;

    /**
        Returns the item label.
    */
    virtual wxString GetItemText(const wxTreeItemId& item) const;

    /**
        Returns the colour of the item label.
    */
    virtual wxColour GetItemTextColour(const wxTreeItemId& item) const;

    /**
        Returns the last child of the item (or an invalid tree item if this item
        has no children).

        @see GetFirstChild(), GetNextSibling(), GetLastChild()
    */
    virtual wxTreeItemId GetLastChild(const wxTreeItemId& item) const;

    /**
        Returns the next child; call GetFirstChild() for the first child. For
        this enumeration function you must pass in a 'cookie' parameter which is
        opaque for the application but is necessary for the library to make
        these functions reentrant (i.e. allow more than one enumeration on one
        and the same object simultaneously). The cookie passed to
        GetFirstChild() and GetNextChild() should be the same.

        Returns an invalid tree item if there are no further children.

        @beginWxPythonOnly
        In wxPython the returned wxTreeItemId and the new cookie value are both
        returned as a tuple containing the two values.
        @endWxPythonOnly

        @see GetFirstChild()
    */
    wxTreeItemId GetNextChild(const wxTreeItemId& item,
                              wxTreeItemIdValue& cookie) const;

    /**
        Returns the next sibling of the specified item; call GetPrevSibling()
        for the previous sibling.

        Returns an invalid tree item if there are no further siblings.

        @see GetPrevSibling()
    */
    virtual wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;

    /**
        Returns the next visible item or an invalid item if this item is the
        last visible one.

        @note The @a item itself must be visible.
    */
    virtual wxTreeItemId GetNextVisible(const wxTreeItemId& item) const;

    /**
        Returns the previous sibling of the specified item; call
        GetNextSibling() for the next sibling.

        Returns an invalid tree item if there are no further children.

        @see GetNextSibling()
    */
    virtual wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

    /**
        Returns the previous visible item or an invalid item if this item is the
        first visible one.

        @note The @a item itself must be visible.
    */
    virtual wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;

    /**
        Returns @true if the control will use a quick calculation for the best
        size, looking only at the first and last items. The default is @false.

        @see SetQuickBestSize()
    */
    bool GetQuickBestSize() const;

    /**
        Returns the root item for the tree control.
    */
    virtual wxTreeItemId GetRootItem() const;

    /**
        Returns the selection, or an invalid item if there is no selection. This
        function only works with the controls without @c wxTR_MULTIPLE style,
        use GetSelections() for the controls which do have this style.
    */
    virtual wxTreeItemId GetSelection() const;

    /**
        Fills the array of tree items passed in with the currently selected
        items. This function can be called only if the control has the @c
        wxTR_MULTIPLE style.

        Returns the number of selected items.

        @beginWxPythonOnly
        The wxPython version of this method accepts no parameters and returns a
        Python list of @ref wxTreeItemId "wxTreeItemId"s.
        @endWxPythonOnly
    */
    unsigned int GetSelections(wxArrayTreeItemIds& selection) const;

    /**
        Returns the state image list (from which application-defined state
        images are taken).
    */
    wxImageList* GetStateImageList() const;

    /**
        Calculates which (if any) item is under the given @a point, returning
        the tree item id at this point plus extra information @a flags. @a flags
        is a bitlist of the following:

        - @c wxTREE_HITTEST_ABOVE: Above the client area.
        - @c wxTREE_HITTEST_BELOW: Below the client area.
        - @c wxTREE_HITTEST_NOWHERE: In the client area but below the last item.
        - @c wxTREE_HITTEST_ONITEMBUTTON: On the button associated with an item.
        - @c wxTREE_HITTEST_ONITEMICON: On the bitmap associated with an item.
        - @c wxTREE_HITTEST_ONITEMINDENT: In the indentation associated with an
            item.
        - @c wxTREE_HITTEST_ONITEMLABEL: On the label (string) associated with
            an item.
        - @c wxTREE_HITTEST_ONITEMRIGHT: In the area to the right of an item.
        - @c wxTREE_HITTEST_ONITEMSTATEICON: On the state icon for a tree view
            item that is in a user-defined state.
        - @c wxTREE_HITTEST_TOLEFT: To the right of the client area.
        - @c wxTREE_HITTEST_TORIGHT: To the left of the client area.

        @beginWxPythonOnly
        In wxPython both the wxTreeItemId and the flags are returned as a tuple.
        @endWxPythonOnly
    */
    wxTreeItemId HitTest(const wxPoint& point, int& flags) const;


    /**
        Inserts an item after a given one (@a previous).

        The @a image and @a selImage parameters are an index within the normal
        image list specifying the image to use for unselected and selected
        items, respectively. If @a image -1 and @a selImage is -1, the same
        image is used for both selected and unselected items.
    */
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& previous,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = NULL);

    /**
        Inserts an item before one identified
        by its position (@a before). @a before must be less than the number of
        children.

        The @a image and @a selImage parameters are an index within the normal
        image list specifying the image to use for unselected and selected
        items, respectively. If @a image -1 and @a selImage is -1, the same
        image is used for both selected and unselected items.

        @beginWxPythonOnly
        In wxPython, this form of this method is called @c InsertItemBefore().
        @endWxPythonOnly
    */
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            size_t before,
                            const wxString& text,
                            int image = -1,
                            int selImage = -1,
                            wxTreeItemData* data = NULL);

    /**
        Returns @true if the given item is in bold state.

        @see SetItemBold()
    */
    virtual bool IsBold(const wxTreeItemId& item) const;

    /**
        Returns @true if the control is empty (i.e. has no items, even no root
        one).
    */
    bool IsEmpty() const;

    /**
        Returns @true if the item is expanded (only makes sense if it has
        children).
    */
    virtual bool IsExpanded(const wxTreeItemId& item) const;

    /**
        Returns @true if the item is selected.
    */
    virtual bool IsSelected(const wxTreeItemId& item) const;

    /**
        Returns @true if the item is visible on the screen.
    */
    virtual bool IsVisible(const wxTreeItemId& item) const;

    /**
        Returns @true if the item has children.
    */
    virtual bool ItemHasChildren(const wxTreeItemId& item) const;

    /**
        Override this function in the derived class to change the sort order of
        the items in the tree control. The function should return a negative,
        zero or positive value if the first item is less than, equal to or
        greater than the second one.

        Please note that you @b must use wxRTTI macros DECLARE_DYNAMIC_CLASS()
        and IMPLEMENT_DYNAMIC_CLASS() if you override this function because
        otherwise the base class considers that it is not overridden and uses
        the default comparison, i.e. sorts the items alphabetically, which
        allows it optimize away the calls to the virtual function completely.

        @see SortChildren()
    */
    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2);

    /**
        Appends an item as the first child of @a parent, return a new item id.

        The @a image and @a selImage parameters are an index within the normal
        image list specifying the image to use for unselected and selected
        items, respectively. If @a image -1 and @a selImage is -1, the same
        image is used for both selected and unselected items.
    */
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1,
                             int selImage = -1,
                             wxTreeItemData* data = NULL);

    /**
        Scrolls the specified item into view.
    */
    virtual void ScrollTo(const wxTreeItemId& item);

    /**
        Selects the given item. In multiple selection controls, can be also used
        to deselect a currently selected item if the value of @a select is
        @false.
    */
    virtual void SelectItem(const wxTreeItemId& item, bool select = true);

    /**
        Sets the buttons image list (from which application-defined button
        images are taken).

        The button images assigned with this method will @b not be deleted by
        @ref wxTreeCtrl "wxTreeCtrl"'s destructor, you must delete it yourself.
        Setting or assigning the button image list enables the display of image
        buttons. Once enabled, the only way to disable the display of button
        images is to set the button image list to @NULL.

        @note This function is only available in the generic version.

        @see AssignButtonsImageList().
    */
    void SetButtonsImageList(wxImageList* imageList);

    /**
        Sets the normal image list. The image list assigned with this method
        will @b not be deleted by @ref wxTreeCtrl "wxTreeCtrl"'s destructor, you
        must delete it yourself.

        @see AssignImageList().
    */
    virtual void SetImageList(wxImageList* imageList);

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
        Makes item appear in bold font if @a bold parameter is @true or resets
        it to the normal state.

        @see IsBold()
    */
    virtual void SetItemBold(const wxTreeItemId& item, bool bold = true);

    /**
        Sets the item client data.

        @beginWxPythonOnly
        - @b SetPyData( @a item, @c obj): Associate the given Python Object with
            the wxTreeItemData for the given item Id.
        @endWxPythonOnly

    */
    virtual void SetItemData(const wxTreeItemId& item, wxTreeItemData* data);


    /**
        Gives the item the visual feedback for Drag'n'Drop actions, which is
        useful if something is dragged from the outside onto the tree control
        (as opposed to a DnD operation within the tree control, which already
        is implemented internally).
    */
    void SetItemDropHighlight(const wxTreeItemId& item,
                              bool highlight = true);

    /**
        Sets the item's font. All items in the tree should have the same height
        to avoid text clipping, so the fonts height should be the same for all
        of them, although font attributes may vary.

        @see SetItemBold()
    */
    virtual void SetItemFont(const wxTreeItemId& item, const wxFont& font);

    /**
        Force appearance of the button next to the item. This is useful to
        allow the user to expand the items which don't have any children now,
        but instead adding them only when needed, thus minimizing memory
        usage and loading time.
    */
    void SetItemHasChildren(const wxTreeItemId& item,
                            bool hasChildren = true);

    /**
        Sets the specified item's image. See GetItemImage() for the description
        of the @a which parameter.
    */
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

    /**
        Sets the selected item image (this function is obsolete, use @ref
        SetItemImage() "SetItemImage"( @a item, ::wxTreeItemIcon_Selected )
        instead).
    */
    void SetItemSelectedImage(const wxTreeItemId& item, int selImage);

    /**
        Sets the specified item state. The value of @a state may be:
        - @c wxTREE_ITEMSTATE_NONE: to disable the item state (the state image will
            be not displayed).
        - @c wxTREE_ITEMSTATE_NEXT: to set the next item state.
        - @c wxTREE_ITEMSTATE_PREV: to set the previous item state.
    */
    void SetItemState(const wxTreeItemId& item, int state);

    /**
        Sets the item label.
    */
    virtual void SetItemText(const wxTreeItemId& item, const wxString& text);

    /**
        Sets the colour of the item's text.
    */
    void SetItemTextColour(const wxTreeItemId& item,
                           const wxColour& col);

    /**
        If @true is passed, specifies that the control will use a quick
        calculation for the best size, looking only at the first and last items.
        Otherwise, it will look at all items. The default is @false.

        @see GetQuickBestSize()
    */
    void SetQuickBestSize(bool quickBestSize);

    /**
        Sets the state image list (from which application-defined state images
        are taken). Image list assigned with this method will @b not be deleted
        by @ref wxTreeCtrl "wxTreeCtrl"'s destructor, you must delete it
        yourself.

        @see AssignStateImageList().
    */
    virtual void SetStateImageList(wxImageList* imageList);

    /**
        Sets the mode flags associated with the display of the tree control. The
        new mode takes effect immediately.

        @note Generic only; MSW ignores changes.
    */
    void SetWindowStyle(long styles);

    /**
        Sorts the children of the given item using OnCompareItems().
        You should override that method to change the sort order (the default is
        ascending case-sensitive alphabetical order).

        @see wxTreeItemData, OnCompareItems()
    */
    virtual void SortChildren(const wxTreeItemId& item);

    /**
        Toggles the given item between collapsed and expanded states.
    */
    virtual void Toggle(const wxTreeItemId& item);

    /**
        Toggles the given item between selected and unselected states. For
        multiselection controls only.
    */
    void ToggleItemSelection(const wxTreeItemId& item);

    /**
        Removes the selection from the currently selected item (if any).
    */
    virtual void Unselect();

    /**
        This function either behaves the same as Unselect() if the control
        doesn't have @c wxTR_MULTIPLE style, or removes the selection from all
        items if it does have this style.
    */
    virtual void UnselectAll();

    /**
        Unselects the given item. This works in multiselection controls only.
    */
    void UnselectItem(const wxTreeItemId& item);
};



/**
    @class wxTreeEvent

    A tree event holds information about events associated with wxTreeCtrl
    objects.

    To process input from a tree control, use these event handler macros to
    direct input to member functions that take a wxTreeEvent argument.

    @beginEventTable{wxTreeEvent}
    @event{EVT_TREE_BEGIN_DRAG(id, func)}
        Begin dragging with the left mouse button.
    @event{EVT_TREE_BEGIN_RDRAG(id, func)}
        Begin dragging with the right mouse button.
    @event{EVT_TREE_END_DRAG(id, func)}
        End dragging with the left or right mouse button.
    @event{EVT_TREE_BEGIN_LABEL_EDIT(id, func)}
        Begin editing a label. This can be prevented by calling Veto().
    @event{EVT_TREE_END_LABEL_EDIT(id, func)}
        Finish editing a label. This can be prevented by calling Veto().
    @event{EVT_TREE_DELETE_ITEM(id, func)}
        Delete an item.
    @event{EVT_TREE_GET_INFO(id, func)}
        Request information from the application.
    @event{EVT_TREE_SET_INFO(id, func)}
        Information is being supplied.
    @event{EVT_TREE_ITEM_ACTIVATED(id, func)}
        The item has been activated, i.e. chosen by double clicking it with
        mouse or from keyboard.
    @event{EVT_TREE_ITEM_COLLAPSED(id, func)}
        The item has been collapsed.
    @event{EVT_TREE_ITEM_COLLAPSING(id, func)}
        The item is being collapsed. This can be prevented by calling Veto().
    @event{EVT_TREE_ITEM_EXPANDED(id, func)}
        The item has been expanded.
    @event{EVT_TREE_ITEM_EXPANDING(id, func)}
        The item is being expanded. This can be prevented by calling Veto().
    @event{EVT_TREE_ITEM_RIGHT_CLICK(id, func)}
        The user has clicked the item with the right mouse button.
    @event{EVT_TREE_ITEM_MIDDLE_CLICK(id, func)}
        The user has clicked the item with the middle mouse button.
    @event{EVT_TREE_SEL_CHANGED(id, func)}
        Selection has changed.
    @event{EVT_TREE_SEL_CHANGING(id, func)}
        Selection is changing. This can be prevented by calling Veto().
    @event{EVT_TREE_KEY_DOWN(id, func)}
        A key has been pressed.
    @event{EVT_TREE_ITEM_GETTOOLTIP(id, func)}
        The opportunity to set the item tooltip is being given to the
        application (call SetToolTip()). Windows only.
    @event{EVT_TREE_ITEM_MENU(id, func)}
        The context menu for the selected item has been requested, either by a
        right click or by using the menu key.
    @event{EVT_TREE_STATE_IMAGE_CLICK(id, func)}
        The state image has been clicked.
    @endEventTable

    @library{wxbase}
    @category{events}

    @see wxTreeCtrl
*/
class wxTreeEvent : public wxNotifyEvent
{
public:
    /**
        Constructor, used by wxWidgets itself only.
    */
    wxTreeEvent(wxEventType commandType, wxTreeCtrl* tree);

    /**
        Returns the item (valid for all events).
    */
    wxTreeItemId GetItem() const;

    /**
        Returns the key code if the event is a key event. Use GetKeyEvent() to
        get the values of the modifier keys for this event (i.e. Shift or Ctrl).
    */
    int GetKeyCode() const;

    /**
        Returns the key event for EVT_TREE_KEY_DOWN() events.
    */
    const wxKeyEvent GetKeyEvent() const;

    /**
        Returns the label if the event is a begin or end edit label event.
    */
    const wxString GetLabel() const;

    /**
        Returns the old item index (valid for EVT_TREE_ITEM_CHANGING() and
        EVT_TREE_ITEM_CHANGED() events).
    */
    wxTreeItemId GetOldItem() const;

    /**
        Returns the position of the mouse pointer if the event is a drag or
        menu-context event.

        In both cases the position is in client coordinates - i.e. relative to
        the wxTreeCtrl window (so that you can pass it directly to e.g.
        wxWindow::PopupMenu()).
    */
    wxPoint GetPoint() const;

    /**
        Returns @true if the label edit was cancelled. This should be called
        from within an EVT_TREE_END_LABEL_EDIT() handler.
    */
    bool IsEditCancelled() const;

    /**
        Set the tooltip for the item (valid for EVT_TREE_ITEM_GETTOOLTIP()
        events). Windows only.
    */
    void SetToolTip(const wxString& tooltip);
};
