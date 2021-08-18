///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/combobox.h
// Purpose:     Interface of wxPersistentComboBox
// Author:      Vadim Zeitlin
// Created:     2020-11-19
// Copyright:   (c) 2020 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Persistence adapter for wxComboBox.

    This adapter saves and restores the items of wxComboBox. A persistent
    combobox can be used to preserve history of user entries.

    Example of using it:
    @code
    // Suppose you need to ask the user to select their favourite Linux
    // distribution, for some reason:
    wxComboBox* combo = new wxComboBox(this, wxID_ANY);
    if ( !wxPersistentRegisterAndRestore(combo, "distribution") )
    {
        // Seed it with some default contents.
        combo->Append("Debian");
        combo->Append("Fedora");
        combo->Append("Ubuntu");
    }

    // Optionally, you might want to restore the last used entry:
    combo->SetSelection(0);

    @endcode

    @since 3.1.5
 */
class wxPersistentComboBox : public wxPersistentWindow<wxComboBox>
{
public:
    /**
        Constructor.

        @param combobox
            The associated combobox.
     */
    explicit wxPersistentComboBox(wxComboBox *combobox);

    /**
        Save the current items and value.

        The current control value is saved as the first item, so that calling
        @c SetSelection(0) when the control is created the next time will
        restore the value which was last used. If the current value is the same
        as one of the existing items, this item is moved to the front of the
        list, instead of being added again.

        If the current value is empty, it is not saved at all.

        At most 10 items are saved, if the combobox has more than 10 items, or
        exactly 10 items and the current value is different from all of them,
        the items beyond the tenth one are discarded.
     */
    virtual void Save() const;

    /**
        Restore the combobox items.

        This function doesn't change the current combobox value, you need to
        call @c SetSelection(0) explicitly, after verifying that the combobox
        is not empty using its IsListEmpty() method, if you want to restore the
        last used value automatically. Otherwise the user can always do it by
        opening the combobox and selecting it manually.
     */
    virtual bool Restore();
};

/// Overload allowing persistence adapter creation for wxComboBox objects.
wxPersistentObject *wxCreatePersistentObject(wxComboBox *combobox);
