/////////////////////////////////////////////////////////////////////////////
// Name:        valtext.h
// Purpose:     interface of wxTextValidator
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    Styles used by wxTextValidator.
*/
enum wxTextValidatorStyle
{
    /// No filtering takes place.
    wxFILTER_NONE,

    /// Non-ASCII characters are filtered out.
    wxFILTER_ASCII,

    /// Non-alpha characters are filtered out.
    wxFILTER_ALPHA,

    /// Non-alphanumeric characters are filtered out.
    wxFILTER_ALPHANUMERIC,

    /// Non-numeric characters are filtered out.
    wxFILTER_NUMERIC,

    /// Use an include list. The validator checks if the user input is on
    /// the list, complaining if not. See wxTextValidator::SetIncludes().
    wxFILTER_INCLUDE_LIST,

    /// Use an exclude list. The validator checks if the user input is on
    /// the list, complaining if it is. See wxTextValidator::SetExcludes().
    wxFILTER_EXCLUDE_LIST,

    /// Use an include list. The validator checks if each input character is
    /// in the list (one character per list element), complaining if not.
    /// See wxTextValidator::SetIncludes().
    wxFILTER_INCLUDE_CHAR_LIST,

    /// Use an include list. The validator checks if each input character is
    /// in the list (one character per list element), complaining if it is.
    /// See wxTextValidator::SetExcludes().
    wxFILTER_EXCLUDE_CHAR_LIST
};

/**
    @class wxTextValidator

    wxTextValidator validates text controls, providing a variety of filtering
    behaviours.

    For more information, please see @ref overview_validator.

    @library{wxcore}
    @category{validator}

    @see @ref overview_validator, wxValidator, wxGenericValidator
*/
class wxTextValidator : public wxValidator
{
public:
    /**
        Default constructor.
    */
    wxTextValidator(const wxTextValidator& validator);

    /**
        Constructor taking a style and optional pointer to a wxString variable.

        @param style
            One of the ::wxTextValidatorStyle styles.
        @param valPtr
            A pointer to a wxString variable that contains the value. This
            variable should have a lifetime equal to or longer than the
            validator lifetime (which is usually determined by the lifetime of
            the window).
    */
    wxTextValidator(wxTextValidatorStyle style = wxFILTER_NONE, wxString* valPtr = NULL);

    /**
        Clones the text validator using the copy constructor.
    */
    virtual wxObject* Clone() const;

    /**
        Returns a reference to the exclude list (the list of invalid values).
    */
    wxArrayString& GetExcludes();

    /**
        Returns a reference to the include list (the list of valid values).
    */
    wxArrayString& GetIncludes();

    /**
        Returns the validator style.
    */
    long GetStyle() const;

    /**
        Receives character input from the window and filters it according to
        the current validator style.
    */
    void OnChar(wxKeyEvent& event);

    /**
        Sets the exclude list (invalid values for the user input).
    */
    void SetExcludes(const wxArrayString& stringList);

    /**
        Sets the include list (valid values for the user input).
    */
    void SetIncludes(const wxArrayString& stringList);

    /**
        Sets the validator style.
    */
    void SetStyle(wxTextValidatorStyle style);

    /**
        Transfers the value in the text control to the string.
    */
    virtual bool TransferFromWindow();

    /**
        Transfers the string value to the text control.
    */
    virtual bool TransferToWindow();

    /**
        Validates the window contents against the include or exclude lists,
        depending on the validator style.
    */
    virtual bool Validate(wxWindow* parent);
};

