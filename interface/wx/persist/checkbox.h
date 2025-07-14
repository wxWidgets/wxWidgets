///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/checkbox.h
// Purpose:     Interface of wxPersistentCheckBox
// Author:      Vadim Zeitlin
// Created:     2025-06-15
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Persistence adapter for wxCheckBox controls.

    This adapter saves and restores the state of a wxCheckBox control,

    Example of using it:
    @code
    auto* checkbox = new wxCheckBox(this, wxID_ANY, "&Remember me");

    wxPersistentRegisterAndRestore(checkbox);
    @endcode

    If the checkbox is checked, it will be checked again after the application
    restart.

    @since 3.3.1
 */
class wxPersistentCheckBox : public wxPersistentWindow<wxCheckBox>
{
public:
    /**
        Constructor.

        Note that 3-state checkboxes are also supported.

        @param checkbox
            The associated checkbox.
     */
    explicit wxPersistentCheckBox(wxCheckBox *checkbox);

    /**
        Save the current checkbox state.

        The state is saved as an integer value corresponding to the numeric
        value returned by wxCheckBox::Get3StateValue(). For unchecked and
        checked checkboxes this value is 0 and 1 respectively, as is customary.
     */
    virtual void Save() const;

    /**
        Restore the previously saved checkbox state.

        If the saved index is valid, i.e. is ::wxCHK_UNCHECKED, ::wxCHK_CHECKED
        or ::wxCHK_UNDETERMINED if checkbox supports 3rd state, the checkbox
        state is set to the previously saved value (otherwise it is ignored).
     */
    virtual bool Restore();
};

/// Overload allowing persistence adapter creation for wxCheckBox objects.
wxPersistentObject *wxCreatePersistentObject(wxCheckBox *checkbox);
