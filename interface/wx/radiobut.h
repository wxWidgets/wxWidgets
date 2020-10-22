/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     interface of wxRadioButton
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRadioButton

    A radio button item is a button which usually denotes one of several
    mutually exclusive options. It has a text label next to a (usually) round
    button.

    Radio buttons are typically used in groups of mutually-exclusive buttons,
    i.e. exactly one of the buttons in the group is checked, and the other ones
    are unchecked automatically. Such groups are created implicitly, but can
    also be started explicitly by using @c wxRB_GROUP style: a button with this
    style starts a new group and will become the initial selection in this
    group. Alternatively, a radio button may be excluded from the group that it
    would otherwise belong to by using @c wxRB_SINGLE style.

    To find the other elements of the same radio button group, you can use
    GetFirstInGroup(), GetPreviousInGroup(), GetNextInGroup() and
    GetLastInGroup() functions.


    @beginStyleTable
    @style{wxRB_GROUP}
           Marks the beginning of a new group of radio buttons.
    @style{wxRB_SINGLE}
           Creates a radio button which is not part of any radio button group.
           When this style is used, no other radio buttons will be turned off
           automatically when this button is turned on and such behaviour will
           need to be implemented manually, in the event handler for this
           button.
    @endStyleTable

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_RADIOBUTTON(id, func)}
           Process a @c wxEVT_RADIOBUTTON event, when the
           radiobutton is clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{radiobutton}

    @see @ref overview_events, wxRadioBox, wxCheckBox
*/
class wxRadioButton : public wxControl
{
public:
    /**
        Default constructor.

        @see Create(), wxValidator
    */
    wxRadioButton();

    /**
        Constructor, creating and showing a radio button.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value @c wxID_ANY indicates a default value.
        @param label
            Label for the radio button.
        @param pos
            Window position. If ::wxDefaultPosition is specified then a default
            position is chosen.
        @param size
            Window size. If ::wxDefaultSize is specified then a default size
            is chosen.
        @param style
            Window style. See wxRadioButton.
        @param validator
            Window validator.
        @param name
            Window name.

        @see Create(), wxValidator
    */
    wxRadioButton(wxWindow* parent, wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxRadioButtonNameStr);

    /**
        Destructor, destroying the radio button item.
    */
    virtual ~wxRadioButton();

    /**
        Creates the choice for two-step construction. See wxRadioButton() for
        further details.
    */
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxRadioButtonNameStr);

    /**
        Returns @true if the radio button is checked, @false otherwise.
    */
    virtual bool GetValue() const;

    /**
        Sets the radio button to checked or unchecked status. This does not cause a
        @c wxEVT_RADIOBUTTON event to get emitted.

        If the radio button belongs to a radio group exactly one button in the
        group may be checked and so this method can be only called with @a
        value set to @true. To uncheck a radio button in a group you must check
        another button in the same group.

        @note Under MSW, the focused radio button is always selected, i.e. its
            value is @true. And, conversely, calling @c SetValue(true) will
            also set focus to the radio button if the focus had previously been
            on another radio button in the same group -- as otherwise setting
            it on wouldn't work.

        @param value
            @true to check, @false to uncheck.
    */
    virtual void SetValue(bool value);

   /**
        Returns the first button of the radio button group this button belongs
        to.

        For a radio button with @c wxRB_SINGLE style, this function returns this
        button itself, as it is the only member of its group. Otherwise, the
        function returns the closest previous radio button with @c wxRB_GROUP
        style (which could still be this button itself) or the first radio
        button in the same window.

        The returned value is never @NULL.

        @see GetPreviousInGroup(), GetNextInGroup(), GetLastInGroup()

        @since 3.1.5
    */
    wxRadioButton* GetFirstInGroup() const;

   /**
        Returns the last button of the radio button group this button belongs
        to.

        Similarly to GetFirstInGroup(), this function returns this button
        itself if it has @c wxRB_SINGLE style. Otherwise, the function returns
        the last button before the next button with @c wxRB_GROUP style or the
        last radio button in the same window.

        The returned value is never @NULL.

        @see GetPreviousInGroup(), GetNextInGroup()

        @since 3.1.5
    */
    wxRadioButton* GetLastInGroup() const;

   /**
        Returns the previous radio button in the same group.

        The return value is @NULL if there is no predecessor or if this button
        has @c wxRB_SINGLE style.

        @see GetFirstInGroup(), GetNextInGroup(), GetLastInGroup()

        @since 3.1.5
    */
    wxRadioButton* GetPreviousInGroup() const;

   /**
        Returns the next radio button in the same group.

        The return value is @NULL if there is no successor or if this button
        has @c wxRB_SINGLE style.

        @see GetFirstInGroup(), GetPreviousInGroup(), GetLastInGroup()

        @since 3.1.5
    */
    wxRadioButton* GetNextInGroup() const;
};

