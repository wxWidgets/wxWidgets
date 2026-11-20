/////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/buttonbox.h
// Purpose:     interface of wxButtonBox
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     2026-11-14
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

wxEventType wxEVT_BUTTONBOX;

/**
    @class wxButtonBox

    A wxButtonBox is a row of adjacent segments, each of which can show a
    label and/or a bitmap and which can be selected by the user. It is similar
    to a group of toggle buttons or to a horizontal wxRadioBox, but looks like
    a single control and is also known as a "segmented control".

    The behaviour of the control is determined by its style, which can be one
    of wxBB_SELECT_ONE (the default), wxBB_SELECT_ANY or wxBB_MOMENTARY:

    - With wxBB_SELECT_ONE at most one segment is selected at any given time,
      as in a wxRadioBox, but, contrary to it, it's possible to have no
      selected segment at all (which is the initial state). Clicking on the
      selected segment doesn't deselect it, but still generates an event.
    - With wxBB_SELECT_ANY segments are independent of each other and each of
      them is toggled by clicking on it, as with wxToggleButton.
    - With wxBB_MOMENTARY segments are just buttons: they are never selected
      and clicking on them only generates an event.

    The API of this class follows the one of @c NSSegmentedControl from macOS,
    which is used to implement it in wxOSX/Cocoa port. All the other ports use
    a generic implementation built from a row of wxToggleButton or, for the
    segments having only a bitmap and no label, wxBitmapToggleButton controls.
    This implementation is available under the name of wxGenericButtonBox in
    all ports, including wxOSX, where it can be used instead of the native
    control if desired. In wxOSX/Cocoa the wxHAS_NATIVE_BUTTONBOX symbol is
    defined in @c wx/buttonbox.h, while in all the other ports wxButtonBox is
    just another name for wxGenericButtonBox.

    The segments are identified by their (0-based) index in all the functions
    below. Using an invalid index results in an assert failure.

    @note This class is not available in wxUniversal, as it doesn't provide
        wxBitmapToggleButton (the @c wxUSE_BUTTONBOX is always 0 there).

    Please note that wxButtonBox only emits wxEVT_BUTTONBOX events when the
    user clicks on a segment and never when the selection is changed
    programmatically.

    You can see wxButtonBox in action in @ref page_samples_widgets.

    This control can be created from XRC, see @ref xrc_wxbuttonbox for its
    format.

    @beginStyleTable
    @style{wxBB_SELECT_ONE}
        At most one segment can be selected, this is the default. The same as
        @c NSSegmentSwitchTrackingSelectOne in macOS.
    @style{wxBB_SELECT_ANY}
        Any number of segments can be selected. The same as
        @c NSSegmentSwitchTrackingSelectAny in macOS.
    @style{wxBB_MOMENTARY}
        Segments are never selected, clicking them only generates the events.
        The same as @c NSSegmentSwitchTrackingMomentary in macOS.
    @endStyleTable

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_BUTTONBOX(id, func)}
        Process a wxEVT_BUTTONBOX event, generated when the user clicks on a
        segment. wxCommandEvent::GetInt() returns the index of the segment,
        wxCommandEvent::GetExtraLong() is 1 if the segment is selected after
        the click and 0 otherwise (it is always 1 for wxBB_MOMENTARY, meaning
        "clicked", and for wxBB_SELECT_ONE) and wxCommandEvent::GetString()
        returns its label.
    @endEventTable

    @library{wxcore}
    @category{ctrl}

    @see wxToggleButton, wxBitmapToggleButton, wxRadioBox, wxButton

    @since 3.3.5
*/
class wxButtonBox : public wxControl
{
public:
    /**
        Default constructor.

        Use Create() to really create the control.
    */
    wxButtonBox();

    /**
        Constructor, creating and showing the control.

        The control is created without any segments, use Append() or Insert()
        to add them.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Control identifier. The value wxID_ANY indicates a default value.
        @param pos
            Control position. If ::wxDefaultPosition is specified then a
            default position is chosen.
        @param size
            Control size. If ::wxDefaultSize is specified then the best size
            is used, which depends on the segments.
        @param style
            Window style. See wxButtonBox.
        @param name
            Window name.

        @see Create()
    */
    wxButtonBox(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBB_SELECT_ONE,
                const wxString& name = wxButtonBoxNameStr);

    /**
        Creates the control for two-step construction.

        See wxButtonBox() for the description of the parameters.

        @return @true if the control was successfully created or @false if
            creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBB_SELECT_ONE,
                const wxString& name = wxButtonBoxNameStr);

    /// @name Segments
    ///@{

    /**
        Returns the number of segments.
    */
    size_t GetCount() const;

    /**
        Adds a segment at the end.

        This is the same as calling Insert() with GetCount() as the position.

        @param label
            The label of the segment, may be empty if @a bitmap is specified.
        @param bitmap
            The bitmap of the segment, may be empty if @a label is specified.
    */
    void Append(const wxString& label,
                const wxBitmapBundle& bitmap = wxBitmapBundle());

    /**
        Inserts a new segment before the segment at the given position.

        The attributes (enabled state, tooltip, width and selection) of the
        existing segments are preserved, i.e. they move together with the
        segments.

        @param pos
            The position at which the segment is inserted, between 0 and
            GetCount() inclusive.
        @param label
            The label of the segment, may be empty if @a bitmap is specified.
        @param bitmap
            The bitmap of the segment, may be empty if @a label is specified.
    */
    void Insert(size_t pos, const wxString& label,
                const wxBitmapBundle& bitmap = wxBitmapBundle());

    /**
        Removes the segment at the given position.
    */
    void Delete(size_t pos);

    /**
        Removes all the segments.
    */
    void Clear();

    ///@}


    /// @name Selection
    ///@{

    /**
        Returns the index of the selected segment or ::wxNOT_FOUND if there is
        no selected segment.

        This function always returns ::wxNOT_FOUND for the controls with
        wxBB_MOMENTARY style. For the controls with wxBB_SELECT_ANY style it
        returns one of the selected segments, but which one is unspecified,
        use IsSelected() to find all of them.
    */
    int GetSelection() const;

    /**
        Selects the given segment and deselects all the others.

        Passing ::wxNOT_FOUND removes the selection completely. For the
        controls with wxBB_SELECT_ANY style, which can have several selected
        segments, this function selects the given segment without affecting
        the other ones, but ::wxNOT_FOUND still deselects all of them.

        This function does nothing for the controls with wxBB_MOMENTARY
        style. It doesn't generate any events.

        @see SetSelected()
    */
    void SetSelection(int n);

    /**
        Returns @true if the given segment is selected.

        This function always returns @false for the controls with
        wxBB_MOMENTARY style.
    */
    bool IsSelected(size_t n) const;

    /**
        Selects or deselects the given segment.

        For the controls with wxBB_SELECT_ONE style selecting a segment
        deselects the previously selected one, if any.

        This function does nothing for the controls with wxBB_MOMENTARY
        style. It doesn't generate any events.

        @param n
            The index of the segment.
        @param selected
            @true to select the segment or @false to deselect it.
    */
    void SetSelected(size_t n, bool selected = true);

    ///@}


    /// @name Segment attributes
    ///@{

    /**
        Sets the label of the given segment.

        The label may be empty if the segment has a bitmap.
    */
    void SetItemLabel(size_t n, const wxString& label);

    /**
        Returns the label of the given segment.
    */
    wxString GetItemLabel(size_t n) const;

    /**
        Sets the bitmap of the given segment.

        Passing an empty bundle removes the bitmap. If the segment has both a
        label and a bitmap, they are both shown.
    */
    void SetItemBitmap(size_t n, const wxBitmapBundle& bitmap);

    /**
        Returns the bitmap of the given segment, which is not valid if the
        segment doesn't have any bitmap.
    */
    wxBitmapBundle GetItemBitmap(size_t n) const;

    /**
        Sets the width of the given segment in pixels.

        Passing ::wxDefaultCoord (or 0) restores the default behaviour, i.e.
        the width is determined automatically from the segment contents.
    */
    void SetItemWidth(size_t n, int width);

    /**
        Returns the width of the given segment in pixels, or ::wxDefaultCoord
        if it is determined automatically.
    */
    int GetItemWidth(size_t n) const;

    /**
        Enables or disables the given segment.

        The user can't click on a disabled segment.

        @param n
            The index of the segment.
        @param enable
            @true to enable the segment or @false to disable it.
    */
    void EnableItem(size_t n, bool enable = true);

    /**
        Returns @true if the given segment is enabled.
    */
    bool IsItemEnabled(size_t n) const;

    /**
        Sets the tooltip of the given segment.

        Passing an empty string removes the tooltip. Note that this is
        different from wxWindow::SetToolTip(), which sets the tooltip of the
        entire control.

        @note Under macOS segment tooltips are only supported when running
            under macOS 10.13 or later, this function does nothing (and
            GetItemToolTip() returns an empty string) under the earlier
            versions.
    */
    void SetItemToolTip(size_t n, const wxString& tip);

    /**
        Returns the tooltip of the given segment, which is empty if it doesn't
        have any.
    */
    wxString GetItemToolTip(size_t n) const;

    ///@}
};
