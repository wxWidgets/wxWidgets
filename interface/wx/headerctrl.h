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
    independently.

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
    header.

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

    @beginEventTable{wxHeaderEvent}
    @event{EVT_HEADER_CLICK(id, func)}
        A column heading was clicked.
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
                 long style = 0,
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
                long style = 0,
                const wxString& name = wxHeaderCtrlNameStr);

    /**
        Return the number of columns in the control.

        @see IsEmpty()
     */
    unsigned int GetColumnCount() const;

    /**
        Return whether the control has any columns.

        @see GetColumnCount()
     */
    bool IsEmpty() const;

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
