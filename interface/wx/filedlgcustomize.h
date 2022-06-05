///////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/filedlgcustomize.h
// Purpose:     Documentation of classes used for wxFileDialog customization.
// Author:      Vadim Zeitlin
// Created:     2022-06-03
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    The base class for all wxFileDialog custom controls.

    Unlike normal controls, inheriting from wxWindow, custom controls in
    wxFileDialog are not actual windows, but they do provide some window-like
    operations, and can be disabled or hidden, just as the windows are.

    Also, similarly to the normal windows, objects of this and derived classes
    belong to wxWidgets and must @e not be deleted by the application code.

    Unlike windows, custom controls cannot be created directly, but can only be
    returned from wxFileDialogCustomize functions that are specifically
    provided for creating them.

    @since 3.1.7
 */
class wxFileDialogCustomControl : public wxEvtHandler
{
public:
    /// Show or hide this control.
    void Show(bool show = true);

    /// Hide this control.
    ///
    /// This is equivalent to @c Show(false).
    void Hide();

    /// Enable or disable this control.
    void Enable(bool enable = true);

    /// Disable this control.
    ///
    /// This is equivalent to @c Enable(false).
    void Disable();
};

/**
    Represents a custom button inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddButton().

    It is possible to bind to wxEVT_BUTTON events on this object, which will be
    generated when the button is clicked.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogButton : public wxFileDialogCustomControl
{
};

/**
    Represents a custom checkbox inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddCheckBox().

    It is possible to bind to wxEVT_CHECKBOX events on this object, which will
    be generated when the checkbox is clicked.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogCheckBox : public wxFileDialogCustomControl
{
public:
    /// Return @true if the checkbox is checked.
    bool GetValue() const;

    /// Check or uncheck the checkbox.
    void SetValue(bool value);
};

/**
    Represents a custom radio button inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddRadioButton().

    It is possible to bind to wxEVT_RADIOBUTTON events on this object, which
    will be generated when the radio button is selected.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogRadioButton : public wxFileDialogCustomControl
{
public:
    /// Return @true if the radio button is selected.
    bool GetValue() const;

    /// Select the value of the radio button.
    ///
    /// Using @false for @a value is not supported, this argument only exists
    /// for consistency with wxRadioButton::SetValue().
    void SetValue(bool value);
};

/**
    Represents a custom read-only combobox inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddChoice().

    It is possible to bind to wxEVT_CHOICE events on this object, which
    will be generated when the selection in the combobox changes.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogChoice : public wxFileDialogCustomControl
{
public:
    /// Return the index of the selected item, possibly wxNOT_FOUND.
    int GetSelection() const;

    /// Set the selection to the item with the given index.
    ///
    /// Using @c wxNOT_FOUND for @a n is not supported, once a selection is
    /// made it cannot be undone.
    void SetSelection(int n);
};

/**
    Represents a custom text control inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddTextCtrl().

    Objects of this class don't generate any events currently.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogTextCtrl : public wxFileDialogCustomControl
{
public:
    /// Get the current value entered into the control.
    wxString GetValue() const;

    /// Set the current control value.
    void SetValue(const wxString& text);
};

/**
    Represents a custom static text inside wxFileDialog.

    Objects of this class can only be created by
    wxFileDialogCustomize::AddStaticText().

    Objects of this class don't generate any events.

    See wxFileDialogCustomControl for more information.

    @since 3.1.7
 */
class wxFileDialogStaticText : public wxFileDialogCustomControl
{
public:
    /**
        Set the text shown in the label.

        Any ampersands in the @a text will be escaped, there is no need to do
        it manually, e.g. using wxControl::EscapeMnemonics().
     */
    void SetLabelText(const wxString& text);
};


/**
    Used with wxFileDialogCustomizeHook to add custom controls to wxFileDialog.

    An object of this class is passed to wxFileDialogCustomizeHook::AddCustomControls()
    to allow it to actually add controls to the dialog.

    The pointers returned by the functions of this class belong to wxWidgets
    and should @e not be deleted by the application, just as wxWindow-derived
    objects (even if these controls do not inherit from wxWindow). These
    pointers become invalid when wxFileDialog::ShowModal() returns, and the
    dialog containing them is destroyed, and the latest point at which they can
    be still used is when wxFileDialogCustomizeHook::TransferDataFromCustomControls()
    is called.

    @library{wxcore}
    @category{cmndlg}

    @see wxFileDialog

    @since 3.1.7
 */
class wxFileDialogCustomize
{
public:
    /**
        Add a button with the specified label.
     */
    wxFileDialogButton* AddButton(const wxString& label);

    /**
        Add a checkbox with the specified label.
     */
    wxFileDialogCheckBox* AddCheckBox(const wxString& label);

    /**
        Add a radio button with the specified label.

        The first radio button added will be initially checked. All the radio
        buttons added immediately after it will become part of the same radio
        group and will not be checked, but checking any one of them later will
        uncheck the first button and all the other ones.

        If two consecutive but distinct radio groups are required,
        AddStaticText() with an empty label can be used to separate them.
     */
    wxFileDialogRadioButton* AddRadioButton(const wxString& label);

    /**
        Add a read-only combobox with the specified contents.

        The combobox doesn't have any initial selection, i.e.
        wxFileDialogChoice::GetSelection() returns @c wxNOT_FOUND, if some item
        must be selected, use wxFileDialogChoice::SetSelection() explicitly to
        do it.

        @param n The number of strings, must be positive, as there is no way to
            add more strings later and creating an empty combobox is not very
            useful.
        @param strings A non-@NULL pointer to an array of @a n strings.
     */
    wxFileDialogChoice* AddChoice(size_t n, const wxString* strings);

    /**
        Add a text control with an optional label preceding it.

        Unlike all the other functions for adding controls, the @a label
        parameter here doesn't specify the contents of the text control itself,
        but rather the label appearing before it. Unlike static controls added
        by AddStaticText(), this label is guaranteed to be immediately adjacent
        to it.

        If @a label is empty, no label is created.
     */
    wxFileDialogTextCtrl* AddTextCtrl(const wxString& label = wxString());

    /**
        Add a static text with the given contents.

        The contents of the static text can be updated later, i.e. it doesn't
        need to be actually static.
     */
    wxFileDialogStaticText* AddStaticText(const wxString& label);
};

/**
    Base class for customization hooks used with wxFileDialog.

    wxFileDialogCustomizeHook is an abstract base class, i.e. in order to use a
    concrete class inheriting from it and implementing its pure virtual
    AddCustomControls() function must be defined. Then an object of this class
    should be passed to wxFileDialog::SetCustomizeHook(), which will result in
    its AddCustomControls() being called before the dialog is shown,
    UpdateCustomControls() being called whenever something changes in the
    dialog while it is shown and, finally, TransferDataFromCustomControls()
    being called when the user accepts their choice in the dialog.

    Putting all this together, here is an example of customizing the file
    dialog using this class:
    @code
    class EncryptHook : public wxFileDialogCustomizeHook
    {
    public:
        // Override to add custom controls using the provided customizer object.
        void AddCustomControls(wxFileDialogCustomize& customizer) override
        {
            // Suppose we can encrypt files when saving them.
            m_checkbox = customizer.AddCheckBox("Encrypt");

            // While m_checkbox is not a wxCheckBox, it looks almost like one
            // and, in particular, we can bind to custom control events as usual.
            m_checkbox->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent& event) {
                // We can also call wxWindow-like functions on them.
                m_button->Enable(event.IsChecked());
            });

            // The encryption parameters can be edited in a dedicated dialog.
            m_button = customizer.AddButton("Parameters...");
            m_button->Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
                ... show the encryption parameters dialog here ...
            });
        }

        // Override to save the values of the custom controls.
        void TransferDataFromCustomControls() override
        {
            // Save the checkbox value, as we won't be able to use it any more
            // once this function returns.
            m_encrypt = m_checkbox->GetValue();
        }

        // Just a simple accessor to get the results.
        bool Encrypt() const { return m_encrypt; }

    private:
        wxFileDialogButton* m_button;
        wxFileDialogCheckBox* m_checkbox;

        bool m_encrypt = false;
    };

    void SomeFunc()
    {
        wxFileDialog dialog(NULL, "Save document", wxString(), "file.my",
                            "My files (*.my)|*.my",
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        // This object may be destroyed before the dialog, but must remain
        // alive until ShowModal() returns.
        EncryptHook customizeHook;

        if ( dialog.ShowModal() == wxID_OK )
        {
            if ( customizeHook.Encrypt() )
                ... save with encryption ...
            else
                ... save without encryption ...
        }
    }
    @endcode

    @library{wxcore}
    @category{cmndlg}

    @see wxFileDialog

    @since 3.1.7
*/
class wxFileDialogCustomizeHook
{
public:
    /**
        Must be overridden to add custom controls to the dialog using the
        provided customizer object.

        Call wxFileDialogCustomize functions to add controls and possibly bind
        to their events.

        Note that there is no possibility to define the custom controls layout,
        they will appear more or less consecutively, but the exact layout is
        determined by the current platform.
     */
    virtual void AddCustomControls(wxFileDialogCustomize& customizer) = 0;

    /**
        May be overridden to update the custom controls whenever something
        changes in the dialog.

        This function is called when the user selects a file, changes the
        directory or changes the current filter in the dialog, for example.
        It can be used to update the custom controls state depending on the
        currently selected file, for example.

        Note that it is @e not necessarily called when the value of a custom
        control changes.

        Base class version does nothing.
     */
    virtual void UpdateCustomControls();

    /**
        Should typically be overridden to save the values of the custom
        controls when the dialog is accepted.

        Custom controls are destroyed and cannot be used any longer once
        wxFileDialog::ShowModal() returns, so their values must be retrieved in
        this function, which is called just before this happens.

        This function is @e not called if the user cancels the dialog.

        Base class version does nothing.
     */
    virtual void TransferDataFromCustomControls();
};
