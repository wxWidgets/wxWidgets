/////////////////////////////////////////////////////////////////////////////
// Name:        wx/headerctrl.h
// Purpose:     interface of wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHeaderCtrl

    wxHeaderCtrl is the control containing the column headings which is usually
    used for display of tabular data.

    It is used as part of wxGrid and (will be used in the near future) in
    in wxDataViewCtrl and report view of wxListCtrl but can be also used
    independently. In general this class is meant to be used as part of another
    control which already stores the column information somewhere as it can't
    be used directly: instead you need to inherit from it and implement the
    GetColumn() method to provide column information. See wxHeaderCtrlSimple
    for a concrete control class which can be used directly.

    In addition to labeling the columns, the control has the following
    features:
        - Column reordering support, either by explicitly configuring the
          columns order and calling SetColumnsOrder() or by dragging the
          columns interactively (if enabled).
        - Display of the icons in the header: this is often used to display a
          sort or reverse sort indicator when the column header is clicked.

    Notice that this control itself doesn't do anything other than displaying
    the column headers. In particular column reordering and sorting must still
    be supported by the associated control displaying the real data under the
    header. Also remember to call ScrollWindow() method of the control if the
    associated data display window has a horizontal scrollbar, otherwise the
    headers wouldn't align with the data when the window is scrolled.

    This control is implemented using the native header control under MSW
    systems and a generic implementation elsewhere.

    @beginStyleTable
        @style{wxHD_DRAGDROP}
            If this style is specified (it is by default), the user can reorder
            the control columns by dragging them.
        @style{wxHD_DEFAULT_STYLE}
            Symbolic name for the default control style, currently equal to @c
            wxHD_DRAGDROP.
    @endStyleTable

    @beginEventTable{wxHeaderCtrlEvent}
        @event{EVT_HEADER_CLICK(id, func)}
            A column heading was clicked.
        @event{EVT_HEADER_RIGHT_CLICK(id, func)}
            A column heading was right clicked.
        @event{EVT_HEADER_MIDDLE_CLICK(id, func)}
            A column heading was clicked with the middle mouse button.

        @event{EVT_HEADER_DCLICK(id, func)}
            A column heading was double clicked.
        @event{EVT_HEADER_RIGHT_DCLICK(id, func)}
            A column heading was right double clicked.
        @event{EVT_HEADER_MIDDLE_DCLICK(id, func)}
            A column heading was double clicked with the middle mouse button.

        @event{EVT_HEADER_SEPARATOR_DCLICK(id, func)}
            Separator to the right of the specified column was double clicked
            (this action is commonly used to resize the column to fit its
            contents width and the control provides UpdateColumnWidthToFit() method
            to make implementing this easier).
    @endEventTable

    @library{wxcore}
    @category{ctrl}

    @see wxGrid, wxListCtrl, wxDataViewCtrl


    @section headerctrl_improvements Future Improvements

    Some features are supported by the native MSW control and so could be
    easily implemented in this version of wxHeaderCtrl but need to be
    implemented in the generic version as well to be really useful. Please let
    us know if you need or, better, plan to work on implementing, any of them:
        - Displaying bitmaps instead of or together with the text
        - Custom drawn headers
        - Filters associated with a column.
*/
class wxHeaderCtrl
{
public:
    /**
        Default constructor not creating the underlying window.

        You must use Create() after creating the object using this constructor.
     */
    wxHeaderCtrl();

    /**
        Constructor creating the window.

        Please see Create() for the parameters documentation.
     */
    wxHeaderCtrl(wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHD_DEFAULT_STYLE,
                 const wxString& name = wxHeaderCtrlNameStr);

    /**
        Create the header control window.

        @param parent
            The parent window. The header control should be typically
            positioned along the top edge of this window.
        @param winid
            Id of the control or @c wxID_ANY if you don't care.
        @param pos
            The initial position of the control.
        @param size
            The initial size of the control (usually not very useful as this
            control will typically be resized to have the same width as the
            associated data display control).
        @param style
            The control style, @c wxHD_DEFAULT_STYLE by default. Notice that
            the default style allows the user to reorder the columns by
            dragging them and you need to explicitly turn this feature off by
            using @code wxHD_DEFAULT_STYLE & ~wxHD_DRAGDROP @endcode if this is
            undesirable.
        @param name
            The name of the control.
     */
    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHD_DEFAULT_STYLE,
                const wxString& name = wxHeaderCtrlNameStr);

    /**
        Set the number of columns in the control.

        The control will use GetColumn() to get information about all the
        new columns and refresh itself, i.e. this method also has the same
        effect as calling UpdateColumn() for all columns but it should only be
        used if the number of columns really changed.
     */
    void SetColumnCount(unsigned int count);

    /**
        Return the number of columns in the control.

        @return
            Number of columns as previously set by SetColumnCount().

        @see IsEmpty()
     */
    unsigned int GetColumnCount() const;

    /**
        Return whether the control has any columns.

        @see GetColumnCount()
     */
    bool IsEmpty() const;

protected:
    /**
        Method to be implemented by the derived classes to return the
        information for the given column.

        @param idx
            The column index, between 0 and the value last passed to
            SetColumnCount().
     */
    virtual wxHeaderColumnBase& GetColumn(unsigned int idx) = 0;

    /**
        Method which may be implemented by the derived classes to allow double
        clicking the column separator to resize the column to fit its contents.

        When a separator is double clicked, the default handler of
        EVT_HEADER_SEPARATOR_DCLICK event calls this function and refreshes the
        column if it returns @true so to implement the resizing of the column
        to fit its width on header double click you need to implement this
        method using logic similar to this example:
        @code
            class MyHeaderCtrl : public wxHeaderColumnBase
            {
            public:
                ...

                void SetWidth(int width) { m_width = width; }
                virtual int GetWidth() const { return m_width; }

            private:
                int m_width;
            };

            class MyHeaderCtrl : public wxHeaderCtrl
            {
            public:
            protected:
                virtual wxHeaderColumnBase& GetColumn(unsigned int idx)
                {
                    return m_cols[idx];
                }

                virtual bool UpdateColumnWidthToFit(unsigned int idx, int widthTitle)
                {
                    int widthContents = ... compute minimal width for column idx ...
                    m_cols[idx].SetWidth(wxMax(widthContents, widthTitle));
                    return true;
                }

                wxVector<MyHeaderColumn> m_cols;
            };
        @endcode

        Base class version simply returns @false.

        @param width
            Contains minimal width needed to display the column header itself
            and will usually be used as a starting point for the fitting width
            calculation.
        @return
            @true to indicate that the column was resized, i.e. GetColumn() now
            returns the new width value, and so must be refreshed or @false
            meaning that the control didn't reach to the separator double click.
     */
    virtual bool UpdateColumnWidthToFit(unsigned int idx, int widthTitle);
};


/**
    @class wxHeaderCtrlSimple

    wxHeaderCtrlSimple is a concrete header control which can be used directly,
    without inheriting from it as you need to do when using wxHeaderCtrl
    itself.

    When using it, you need to use simple AppendColumn(), InsertColumn() and
    DeleteColumn() methods instead of setting the number of columns with
    SetColumnCount() and returning the information about them from the
    overridden GetColumn().

    @library{wxcore}
    @category{ctrl}

    @see wxHeaderCtrl
*/
class wxHeaderCtrlSimple : public wxHeaderCtrl
{
public:
    /**
        Default constructor not creating the underlying window.

        You must use Create() after creating the object using this constructor.
     */
    wxHeaderCtrlSimple();

    /**
        Constructor creating the window.

        Please see the base class wxHeaderCtrl::Create() method for the
        parameters description.
     */
    wxHeaderCtrlSimple(wxWindow *parent,
                       wxWindowID winid = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxHD_DEFAULT_STYLE,
                       const wxString& name = wxHeaderCtrlNameStr);

    /**
        Insert the column at the given position.

        @param col
            The column to insert. Notice that because of the existence of
            implicit conversion from wxString to wxHeaderColumn a string
            can be passed directly here.
        @param idx
            The position of the new column, from 0 to GetColumnCount(). Using
            GetColumnCount() means to append the column to the end.

        @see AppendColumn()
     */
    void InsertColumn(const wxHeaderColumn& col, unsigned int idx);

    /**
        Append the column to the end of the control.

        @see InsertColumn()
     */
    void AppendColumn(const wxHeaderColumn& col);

    /**
        Delete the column at the given position.

        @see InsertColumn(), AppendColumn()
     */
    void DeleteColumn(unsigned int idx);

    /**
        Show or hide the column.

        Initially the column is shown by default or hidden if it was added with
        wxCOL_HIDDEN flag set.

        When a column is hidden, it doesn't appear at all on the screen but its
        index is still taken into account when working with other columns. E.g.
        if there are three columns 0, 1 and 2 and the column 1 is hidden you
        still need to use index 2 to refer to the last visible column.

        @param idx
            The index of the column to show or hide, from 0 to GetColumnCount().
        @param show
            Indicates whether the column should be shown (default) or hidden.
     */
    void ShowColumn(unsigned int idx, bool show = true);

    /**
        Hide the column with the given index.

        This is the same as calling @code ShowColumn(idx, false) @endcode.

        @param idx
            The index of the column to show or hide, from 0 to GetColumnCount().
     */
    void HideColumn(unsigned int idx);

    /**
        Update the column sort indicator.

        The sort indicator, if shown, is typically an arrow pointing upwards or
        downwards depending on whether the control contents is sorted in
        ascending or descending order.

        @param idx
            The column to set the sort indicator for.
        @param sortOrder
            If @true or @false show the sort indicator corresponding to
            ascending or descending sort order respectively, if @c -1 remove
            the currently shown sort indicator.
     */
    virtual void ShowSortIndicator(unsigned int idx, int sortOrder);

    /**
        Remove the sort indicator from the given column.

        This is the same as calling ShowSortIndicator() with @c -1 argument.

        @param idx
            The column to remove sort indicator for.
     */
    void RemoveSortIndicator(unsigned int idx);
};

/**
    @class wxHeaderCtrlEvent

    Event class representing the events generated by wxHeaderCtrl.

    @library{wxcore}
    @category{ctrl}

    @see wxHeaderCtrl
*/
class wxHeaderCtrlEvent : public wxNotifyEvent
{
public:
    /**
        Return the index of the column affected by this event.
     */
    int GetColumn() const;
};
