/////////////////////////////////////////////////////////////////////////////
// Name:        vlbox.h
// Purpose:     documentation for wxVListBox class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxVListBox
    @wxheader{vlbox.h}

    wxVListBox is a listbox-like control with the following two main differences
    from a regular listbox: it can have an arbitrarily huge number of items because
    it doesn't store them itself but uses wxVListBox::OnDrawItem
    callback to draw them (so it is a Virtual listbox) and its items can
    have variable height as determined by
    wxVListBox::OnMeasureItem (so it is also a listbox
    with the lines of Variable height).

    Also, as a consequence of its virtual nature, it doesn't have any methods to
    append or insert items in it as it isn't necessary to do it: you just have to
    call wxVListBox::SetItemCount to tell the control how
    many items it should display. Of course, this also means that you will never
    use this class directly because it has pure virtual functions, but will need to
    derive your own class, such as wxHtmlListBox, from it.

    However it emits the same events as wxListBox and the same
    event macros may be used with it. Since wxVListBox does not store its items
    itself, the events will only contain the index, not any contents such as the
    string of an item.

    @library{wxcore}
    @category{ctrl}
    @appearance{vlistbox.png}

    @seealso
    wxSimpleHtmlListBox, wxHtmlListBox
*/
class wxVListBox : public wxVScrolledWindow
{
public:
    //@{
    /**
        Default constructor, you must call Create() later.
    */
    wxVListBox(wxWindow* parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               size_t countItems = 0, long style = 0,
               const wxString& name = wxVListBoxNameStr);
    wxVListBox();
    //@}

    /**
        Deletes all items from the control.
    */
    void Clear();

    /**
        Creates the control. To finish creating it you also should call
        SetItemCount() to let it know about the
        number of items it contains.
        The only special style which may be used with wxVListBox is @c wxLB_MULTIPLE
        which indicates that the listbox should support multiple selection.
        Returns @true on success or @false if the control couldn't be created
    */
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxVListBoxNameStr);

    /**
        Deselects all the items in the listbox.
        Returns @true if any items were changed, i.e. if there had been any
        selected items before, or @false if all the items were already deselected.
        This method is only valid for multi selection listboxes.
        
        @see SelectAll(), Select()
    */
    bool DeselectAll();

    /**
        Returns the index of the first selected item in the listbox or
        @c wxNOT_FOUND if no items are currently selected.
        @a cookie is an opaque parameter which should be passed to the subsequent
        calls to GetNextSelected(). It is needed in
        order to allow parallel iterations over the selected items.
        Here is a typical example of using these functions:
        
        This method is only valid for multi selection listboxes.
    */
    int GetFirstSelected(unsigned long& cookie);

    /**
        Get the number of items in the control.
        
        @see SetItemCount()
    */
    size_t GetItemCount();

    /**
        Returns the margins used by the control. The @c x field of the returned
        point is the horizontal margin and the @c y field is the vertical one.
        
        @see SetMargins()
    */
    wxPoint GetMargins();

    /**
        Returns the index of the next selected item or @c wxNOT_FOUND if there are
        no more.
        This method is only valid for multi selection listboxes.
        
        @see GetFirstSelected()
    */
    int GetNextSelected(unsigned long& cookie);

    /**
        Returns the number of the items currently selected.
        It is valid for both single and multi selection controls. In the former case it
        may only return 0 or 1 however.
        
        @see IsSelected(), GetFirstSelected(),
             GetNextSelected()
    */
    size_t GetSelectedCount();

    /**
        Get the currently selected item or @c wxNOT_FOUND if there is no selection.
    */
    int GetSelection();

    /**
        Returns the background colour used for the selected cells. By default the
        standard system colour is used.
        
        @see wxSystemSettings::GetColour, SetSelectionBackground()
    */
    const wxColour GetSelectionBackground();

    /**
        Returns @true if the listbox was created with @c wxLB_MULTIPLE style
        and so supports multiple selection or @false if it is a single selection
        listbox.
    */
    bool HasMultipleSelection();

    /**
        Returns @true if this item is the current one, @false otherwise.
        Current item is always the same as selected one for the single selection
        listbox and in this case this method is equivalent to
        IsSelected() but they are different for multi
        selection listboxes where many items may be selected but only one (at most) is
        current.
    */
    bool IsCurrent(size_t item);

    /**
        Returns @true if this item is selected, @false otherwise.
    */
    bool IsSelected(size_t item);

    /**
        This method is used to draw the items background and, maybe, a border
        around it.
        The base class version implements a reasonable default behaviour which
        consists in drawing the selected item with the standard background
        colour and drawing a border around the item if it is either selected or
        current.
    */
    void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n);

    /**
        The derived class must implement this function to actually draw the item
        with the given index on the provided DC.
        
        @param dc
            The device context to use for drawing
        @param rect
            The bounding rectangle for the item being drawn (DC clipping
            region is set to this rectangle before calling this function)
        @param n
            The index of the item to be drawn
    */
    void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n);

    /**
        This method may be used to draw separators between the lines. The rectangle
        passed to it may be modified, typically to deflate it a bit before passing to
        OnDrawItem().
        The base class version of this method doesn't do anything.
        
        @param dc
            The device context to use for drawing
        @param rect
            The bounding rectangle for the item
        @param n
            The index of the item
    */
    void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n);

    /**
        The derived class must implement this method to return the height of the
        specified item (in pixels).
    */
    wxCoord OnMeasureItem(size_t n);

    /**
        Selects or deselects the specified item which must be valid (i.e. not
        equal to @c wxNOT_FOUND).
        Return @true if the items selection status has changed or @false
        otherwise.
        This function is only valid for the multiple selection listboxes, use
        SetSelection() for the single selection ones.
    */
    bool Select(size_t item, bool select = true);

    /**
        Selects all the items in the listbox.
        Returns @true if any items were changed, i.e. if there had been any
        unselected items before, or @false if all the items were already selected.
        This method is only valid for multi selection listboxes.
        
        @see DeselectAll(), Select()
    */
    bool SelectAll();

    /**
        Selects all items in the specified range which may be given in any order.
        Return @true if the items selection status has changed or @false
        otherwise.
        This method is only valid for multi selection listboxes.
        
        @see SelectAll(), Select()
    */
    bool SelectRange(size_t from, size_t to);

    /**
        Set the number of items to be shown in the control.
        This is just a synonym for
        wxVScrolledWindow::SetRowCount.
    */
    void SetItemCount(size_t count);

    //@{
    /**
        Set the margins: horizontal margin is the distance between the window
        border and the item contents while vertical margin is half of the
        distance between items.
        By default both margins are 0.
    */
    void SetMargins(const wxPoint& pt);
    void SetMargins(wxCoord x, wxCoord y);
    //@}

    /**
        Set the selection to the specified item, if it is -1 the selection is
        unset. The selected item will be automatically scrolled into view if it isn't
        currently visible.
        This method may be used both with single and multiple selection listboxes.
    */
    void SetSelection(int selection);

    /**
        Sets the colour to be used for the selected cells background. The background of
        the standard cells may be changed by simply calling
        wxWindow::SetBackgroundColour.
        Notice that using non-default background colour may result in control having
        appearance different from the similar native controls and so should in general
        be avoided.
        
        @see GetSelectionBackground()
    */
    void SetSelectionBackground(const wxColour& col);

    /**
        Toggles the state of the specified @e item, i.e. selects it if it was
        unselected and deselects it if it was selected.
        This method is only valid for multi selection listboxes.
    */
    void Toggle(size_t item);
};
