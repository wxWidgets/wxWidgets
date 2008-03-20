/////////////////////////////////////////////////////////////////////////////
// Name:        valgen.h
// Purpose:     interface of wxGenericValidator
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGenericValidator
    @wxheader{valgen.h}

    wxGenericValidator performs data transfer (but not validation or filtering) for
    the following
    basic controls: wxButton, wxCheckBox, wxListBox, wxStaticText, wxRadioButton,
    wxRadioBox,
    wxChoice, wxComboBox, wxGauge, wxSlider, wxScrollBar, wxSpinButton, wxTextCtrl,
    wxCheckListBox.

    It checks the type of the window and uses an appropriate type for that window.
    For example,
    wxButton and wxTextCtrl transfer data to and from a wxString variable;
    wxListBox uses a
    wxArrayInt; wxCheckBox uses a bool.

    For more information, please see @ref overview_validatoroverview "Validator
    overview".

    @library{wxcore}
    @category{validator}

    @see @ref overview_validatoroverview "Validator overview", wxValidator,
    wxTextValidator
*/
class wxGenericValidator : public wxValidator
{
public:
    //@{
    /**
        Constructor taking a wxDateTime pointer. This will be
        used for wxDatePickerCtrl.

        @param validator
            Validator to copy.
        @param valPtr
            A pointer to a variable that contains the value. This variable
            should have a lifetime equal to or longer than the validator lifetime
        (which is usually
            determined by the lifetime of the window).
    */
    wxGenericValidator(const wxGenericValidator& validator);
    wxGenericValidator(bool* valPtr);
    wxGenericValidator(wxString* valPtr);
    wxGenericValidator(int* valPtr);
    wxGenericValidator(wxArrayInt* valPtr);
    wxGenericValidator(wxDateTime* valPtr);
    //@}

    /**
        Destructor.
    */
    ~wxGenericValidator();

    /**
        Clones the generic validator using the copy constructor.
    */
    virtual wxValidator* Clone() const;

    /**
        Transfers the value from the window to the appropriate data type.
    */
    virtual bool TransferFromWindow();

    /**
        Transfers the value to the window.
    */
    virtual bool TransferToWindow();
};

