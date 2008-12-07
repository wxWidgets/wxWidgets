/////////////////////////////////////////////////////////////////////////////
// Name:        wx/headercol.h
// Purpose:     interface of wxHeaderColumn
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    Special value used for column width meaning unspecified or default.
 */
enum { wxCOL_WIDTH_DEFAULT = -1 };

/**
    Bit flags used as wxHeaderColumn flags.
 */
enum
{
    /// Column can be resized (included in default flags).
    wxCOL_RESIZABLE   = 1,

    /// Column can be clicked to toggle the sort order by its contents.
    wxCOL_SORTABLE    = 2,

    /// Column can be dragged to change its order (included in default).
    wxCOL_REORDERABLE = 4,

    /// Column is not shown at all.
    wxCOL_HIDDEN      = 8,

    /// Default flags for wxHeaderColumn ctor.
    wxCOL_DEFAULT_FLAGS = wxCOL_RESIZABLE | wxCOL_REORDERABLE
};

/**
    @class wxHeaderColumn

    Represents a column header in controls displaying tabular data such as
    wxDataViewCtrl or wxGrid.

    Notice that this is an abstract base class which is implemented (usually
    using the information stored in the associated control) by the different
    controls using wxHeaderCtrl. You may use wxHeaderCtrlSimple which uses
    concrete wxHeaderColumnSimple if you don't already store the column
    information somewhere.

    @library{wxcore}
    @category{ctrl}
 */
class wxHeaderColumn
{
public:
    /**
        Set the text to display in the column header.
     */
    virtual void SetTitle(const wxString& title) = 0;

    /**
        Get the text shown in the column header.
     */
    virtual wxString GetTitle() const = 0;

    /**
        Set the bitmap to be displayed in the column header.

        Notice that the bitmaps displayed in different columns of the same
        control must all be of the same size.
     */
    virtual void SetBitmap(const wxBitmap& bitmap) = 0;

    /**
        Returns the bitmap in the header of the column, if any.

        If the column has no associated bitmap, wxNullBitmap is returned.
    */
    virtual wxBitmap GetBitmap() const = 0;

    /**
        Set the column width.

        @param width
            The column width in pixels or the special wxCOL_WIDTH_DEFAULT value
            meaning to use default width.
     */
    virtual void SetWidth(int width) = 0;

    /**
        Returns the current width of the column.

        @return
            Width of the column in pixels, never wxCOL_WIDTH_DEFAULT.
    */
    virtual int GetWidth() const = 0;

    /**
        Set the minimal column width.

        This method can be used with resizeable columns (i.e. those for which
        wxCOL_RESIZABLE flag is set in GetFlags() or, alternatively,
        IsResizeable() returns @true) to prevent the user from making them
        narrower than the given width.

        @param minWidth
            The minimal column width in pixels, may be 0 to remove any
            previously set restrictions.
     */
    virtual void SetMinWidth(int minWidth) = 0;

    /**
        Return the minimal column width.

        @return
            The value previously set by SetMinWidth() or 0 by default.
     */
    virtual int GetMinWidth() const = 0;

    /**
        Set the alignment of the column header.

        @param align
            The text alignment in horizontal direction only or wxALIGN_NOT to
            use the default alignment, The possible values here are
            wxALIGN_CENTRE, wxALIGN_LEFT or wxALIGN_RIGHT with
            wxALIGN_CENTRE_HORIZONTAL being also supported as synonym for
            wxALIGN_CENTRE for consistency (but notice that GetAlignment()
            never returns it).
    */
    virtual void SetAlignment(wxAlignment align) = 0;

    /**
        Returns the current column alignment.

        @return
            One of wxALIGN_CENTRE, wxALIGN_LEFT or wxALIGN_RIGHT.
     */
    virtual wxAlignment GetAlignment() const = 0;


    /**
        Set the column flags.

        This method allows to set all flags at once, see also generic
        ChangeFlag(), SetFlag(), ClearFlag() and ToggleFlag() methods below as
        well as specific SetResizeable(), SetSortable(), SetReorderable() and
        SetHidden() ones.

        @param flags
            Combination of wxCOL_RESIZABLE, wxCOL_SORTABLE, wxCOL_REORDERABLE
            and wxCOL_HIDDEN bit flags.
     */
    virtual void SetFlags(int flags) = 0;

    /**
        Set or clear the given flag.

        @param flag
            The flag to set or clear.
        @param set
            If @true, set the flag, i.e. equivalent to calling SetFlag(),
            otherwise clear it, as ClearFlag().

        @see SetFlags()
     */
    void ChangeFlag(int flag, bool set);

    /**
        Set the specified flag for the column.

        @see SetFlags()
     */
    void SetFlag(int flag);

    /**
        Clear the specified flag for the column.

        @see SetFlags()
     */
    void ClearFlag(int flag);

    /**
        Toggle the specified flag for the column.

        If the flag is currently set, equivalent to ClearFlag(), otherwise --
        to SetFlag().

        @see SetFlags()
     */
    void ToggleFlag(int flag);

    /**
        Get the column flags.

        This method retrieves all the flags at once, you can also use HasFlag()
        to test for any individual flag or IsResizeable(), IsSortable(),
        IsReorderable() and IsHidden() to test for particular flags.

        @see SetFlags()
     */
    virtual int GetFlags() const = 0;

    /**
        Return @true if the specified flag is currently set for this column.
     */
    bool HasFlag(int flag) const;


    /**
        Call this to enable or disable interactive resizing of the column by
        the user.

        By default, the columns are resizeable.

        Equivalent to ChangeFlag(wxCOL_RESIZABLE, resizeable).
     */
    virtual void SetResizeable(bool resizeable);

    /**
        Return true if the column can be resized by the user.

        Equivalent to HasFlag(wxCOL_RESIZABLE).
     */
    virtual bool IsResizeable() const;

    /**
        Allow clicking the column to sort the control contents by the field in
        this column.

        By default, the columns are not sortable so you need to explicitly call
        this function to allow sorting by the field corresponding to this
        column.

        Equivalent to ChangeFlag(wxCOL_SORTABLE, sortable).
     */
    virtual void SetSortable(bool sortable);

    /**
        Returns @true if the column can be clicked by user to sort the control
        contents by the field in this column.

        This corresponds to wxCOL_SORTABLE flag which is off by default.

        @see SetSortable()
    */
    virtual bool IsSortable() const;

    /**
        Allow changing the column order by dragging it.

        Equivalent to ChangeFlag(wxCOL_REORDERABLE, reorderable).
     */
    virtual void SetReorderable(bool reorderable);

    /**
        Returns @true if the column can be dragged by user to change its order.

        This corresponds to wxCOL_REORDERABLE flag which is on by default.

        @see SetReorderable()
    */
    virtual bool IsReorderable() const;

    /**
        Hide or show the column.

        By default all columns are shown but some of them can be completely
        hidden from view by calling this function.

        Equivalent to ChangeFlag(wxCOL_HIDDEN, hidden).
     */
    virtual void SetHidden(bool hidden);

    /**
        Returns @true if the column is currently hidden.

        This corresponds to wxCOL_HIDDEN flag which is off by default.
     */
    virtual bool IsHidden() const;

    /**
        Returns @true if the column is currently shown.

        This corresponds to the absence of wxCOL_HIDDEN flag.
     */
    bool IsShown() const;



    /**
        Sets this column as the sort key for the associated control.

        Calling this function with @true argument means that this column is
        currently used for sorting the control contents and so should typically
        display an arrow indicating it (the direction of the arrow depends on
        IsSortOrderAscending() return value).

        Don't confuse this function with SetSortable() which should be used to
        indicate that the column @em may be used for sorting while this one is
        used to indicate that it currently @em is used for sorting. Of course,
        SetAsSortKey() can be only called for sortable columns.

        @param sort
            Sort (default) or don't sort the control contents by this column.
     */
    virtual void SetAsSortKey(bool sort = true) = 0;

    /**
        Don't use this column for sorting.

        This is equivalent to calling SetAsSortKey() with @false argument.
     */
    void UnsetAsSortKey();

    /**
        Returns @true if the column is currently used for sorting.

        Notice that this function simply returns the value last passed to
        SetAsSortKey() (or @false if SetAsSortKey() had never been called), it
        is up to the associated control to use this information to actually
        sort its contents.
     */
    virtual bool IsSortKey() const = 0;

    /**
        Sets the sort order for this column.

        This only makes sense for sortable columns which are currently used as
        sort key, i.e. for which IsSortKey() returns @true and is only taken
        into account by the control in which this column is inserted, this
        function just stores the sort order in the wxHeaderColumn object.

        @param ascending
            If @true, sort in ascending order, otherwise in descending order.
     */
    virtual void SetSortOrder(bool ascending) = 0;

    /**
        Inverses the sort order.

        This function is typically called when the user clicks on a column used
        for sorting to change sort order from ascending to descending or vice
        versa.

        @see SetSortOrder(), IsSortOrderAscending()
     */
    void ToggleSortOrder();

    /**
        Returns @true, if the sort order is ascending.

        Notice that it only makes sense to call this function if the column is
        used for sorting at all, i.e. if IsSortKey() returns @true.

        @see SetSortOrder()
    */
    virtual bool IsSortOrderAscending() const = 0;
};

/**
    @class wxHeaderColumnSimple

    Simple container for the information about the column.

    This is a concrete class implementing all base wxHeaderColumn class methods
    in a trivial way, i.e. by just storing the information in the object
    itself. It is used by and with wxHeaderCtrlSimple, e.g.
    @code
        wxHeaderCtrlSimple * header = new wxHeaderCtrlSimple(...);
        wxHeaderColumnSimple col("Title");
        col.SetWidth(100);
        col.SetSortable(100);
        header->AppendColumn(col);
    @endcode

    @library{wxcore}
    @category{ctrl}
 */
class wxHeaderColumnSimple : public wxHeaderColumn
{
public:
    //@{
    /**
        Constructor for a column header.

        The first constructor creates a header showing the given text @a title
        while the second one creates one showing the specified @a bitmap image.
     */
    wxHeaderColumnSimple(const wxString& title,
                         int width = wxCOL_WIDTH_DEFAULT,
                         wxAlignment align = wxALIGN_NOT,
                         int flags = wxCOL_DEFAULT_FLAGS);

    wxHeaderColumnSimple(const wxBitmap &bitmap,
                         int width = wxDVC_DEFAULT_WIDTH,
                         wxAlignment align = wxALIGN_CENTER,
                         int flags = wxCOL_DEFAULT_FLAGS);
    //@}

    //@{

    /// Trivial implementations of the base class pure virtual functions.

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;
    virtual void SetBitmap(const wxBitmap& bitmap);
    virtual wxBitmap GetBitmap() const;
    virtual void SetWidth(int width);
    virtual int GetWidth() const;
    virtual void SetMinWidth(int minWidth);
    virtual int GetMinWidth() const;
    virtual void SetAlignment(wxAlignment align);
    virtual wxAlignment GetAlignment() const;
    virtual void SetFlags(int flags);
    virtual int GetFlags() const;
    virtual void SetAsSortKey(bool sort = true);
    virtual bool IsSortKey() const;
    virtual void SetSortOrder(bool ascending);
    virtual bool IsSortOrderAscending() const;

    //@}
};
