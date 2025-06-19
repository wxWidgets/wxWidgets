///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/radiobutton.h
// Purpose:     Interface of wxPersistentRadioButton
// Author:      Vadim Zeitlin
// Created:     2025-06-15
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Persistence adapter for wxRadioButton controls.

    This adapter saves and restores the index of the selected wxRadioButton in
    a group of radio buttons, to allow to retain the selection across program
    executions.

    Example of using it:
    @code
    // Assume that all these controls are added to some sizer elsewhere.
    auto* label = new wxStaticText(this, wxID_ANY, "Play with:");

    auto* black = new wxRadioButton(this, wxID_ANY, "&Black",
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxRB_GROUP);
    auto* white = new wxRadioButton(this, wxID_ANY, "&White");

    // We register the first radio button here, but all radio buttons in the
    // same group are potentially affected by this call.
    wxPersistentRegisterAndRestore(black);
    @endcode

    During the first program execution, black colour will be selected, but if
    the user selects white pieces, this selection will be restored during the
    subsequent run.

    @since 3.3.1
 */
class wxPersistentRadioButton : public wxPersistentWindow<wxRadioButton>
{
public:
    /**
        Constructor.

        Please note that the radio button must be the first one in the group,
        i.e. have ::wxRB_GROUP style set, otherwise an assertion will be
        triggered.

        Also note that currently ::wxRB_SINGLE style is not supported.

        @param radiobutton
            The associated radiobutton.
     */
    explicit wxPersistentRadioButton(wxRadioButton *radiobutton);

    /**
        Save the currently selected button index.

        The 0-based index of the selected radio button in the group is saved as
        radio button value.
     */
    virtual void Save() const;

    /**
        Restore the previously saved selection.

        If the saved index is valid, i.e. is positive and less than the number
        of radio buttons in the group, the radio button with the corresponding
        index will be selected.
     */
    virtual bool Restore();
};

/// Overload allowing persistence adapter creation for wxRadioButton objects.
wxPersistentObject *wxCreatePersistentObject(wxRadioButton *radiobutton);
