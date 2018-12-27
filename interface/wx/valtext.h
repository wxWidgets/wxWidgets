/////////////////////////////////////////////////////////////////////////////
// Name:        valtext.h
// Purpose:     interface of wxTextValidator
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    Styles used by wxTextValidator.

    Notice that wxFILTER_{INCLUDE,EXCLUDE}[_CHAR]_LIST pairs can be used to
    document the purpose of the validator only and are not enforced in the
    implementation of the wxTextValidator. Therefore, calling the corresponding
    member functions:
    wxTextValidator::SetCharIncludes(), wxTextValidator::SetCharExcludes(),
    wxTextValidator::SetIncludes() or wxTextValidator::SetExcludes()
    after creating the validator with or without those flags changes nothing.

    Caveat
        wxFILTER_INCLUDE_CHAR_LIST is an exception from the notice above:
        If this style is set with one or more of the following styles:
        wxFILTER_ASCII, wxFILTER_ALPHA, wxFILTER_ALPHANUMERIC, wxFILTER_DIGITS,
        wxFILTER_XDIGITS, wxFILTER_NUMERIC it just extends the character class
        denoted by the aforementioned styles with those specified in the include
        char list. If set alone, then the charactes allowed to be in the user input
        are restricted to those, and only those, present in the include char list. 
*/
enum wxTextValidatorStyle
{
    /// No filtering takes place.
    wxFILTER_NONE,

    /// Empty strings are filtered out.
    /// If this style is not specified then empty strings are accepted
    /// only if they pass the other checks (if you use more than one wxTextValidatorStyle).
    wxFILTER_EMPTY,

    /// Non-ASCII characters are filtered out. See wxString::IsAscii.
    wxFILTER_ASCII,

    /// Non-alpha characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isalpha
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_ALPHA,

    /// Non-alphanumeric characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isalnum
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_ALPHANUMERIC,

    /// Non-digit characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isdigit
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_DIGITS,

    /// Non-numeric characters are filtered out.
    /// Works like @c wxFILTER_DIGITS but allows also decimal points,
    /// minus/plus signs and the 'e' or 'E' character to input exponents.
    /// Note that this is not the same behaviour of wxString::IsNumber().
    wxFILTER_NUMERIC,

    /// Use an include list. The validator checks if the user input is on
    /// the list, complaining if not. See wxTextValidator::SetIncludes().
    wxFILTER_INCLUDE_LIST,

    /// Use an include char list.
    /// Characters in the include char list will be allowed to be in the
    /// user input. See wxTextValidator::SetCharIncludes().
    wxFILTER_INCLUDE_CHAR_LIST,

    /// Use an exclude list. The validator checks if the user input is on
    /// the list, complaining if it is. See wxTextValidator::SetExcludes().
    wxFILTER_EXCLUDE_LIST,

    /// Use an exclude char list.
    /// Characters in the exclude char list won't be allowed to be in the
    /// user input. See wxTextValidator::SetCharExcludes().
    wxFILTER_EXCLUDE_CHAR_LIST,

    /// Non-hexadecimal characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isxdigit
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_XDIGITS,

    /// A convenience flag for use with the other flags.
    /// The space character is more often used with alphanumeric characters
    /// which makes setting a flag more easier than calling SetCharIncludes(" ")
    /// for that matter.
    wxFILTER_SPACE
};

/**
    @class wxTextValidator

    wxTextValidator validates text controls, providing a variety of filtering
    behaviours.

    For more information, please see @ref overview_validator.

    @library{wxcore}
    @category{validator}

    @see @ref overview_validator, wxValidator, wxGenericValidator,
        wxIntegerValidator, wxFloatingPointValidator
*/
class wxTextValidator : public wxValidator
{
public:
    /**
        Copy constructor.
    */
    wxTextValidator(const wxTextValidator& validator);

    /**
        Constructor taking a style and optional pointer to a wxString variable.

        @param style
            One or more of the ::wxTextValidatorStyle styles. See SetStyle().
        @param valPtr
            A pointer to a wxString variable that contains the value. This
            variable should have a lifetime equal to or longer than the
            validator lifetime (which is usually determined by the lifetime of
            the window).
    */
    wxTextValidator(long style = wxFILTER_NONE, wxString* valPtr = NULL);

    /**
        Clones the text validator using the copy constructor.
    */
    virtual wxObject* Clone() const;

    /**
        Returns a copy of the exclude char list (the list of invalid characters).

        @since 3.1.3
    */
    wxString GetCharExcludes() const;

    /**
        Returns a copy of the include char list (the list of additional valid characters).

        @since 3.1.3
    */
    wxString GetCharIncludes() const;

    /**
        Returns a const reference to the exclude list (the list of invalid values).
    */
    const wxArrayString& GetExcludes() const;

    /**
        Returns a const reference to the include list (the list of valid values).
    */
    const wxArrayString& GetIncludes() const;

    /**
        Returns the validator style.

        @see HasFlag()
    */
    long GetStyle() const;

    /**
        Returns @true if the given @a style bit is set in the current style.
    */
    bool HasFlag(wxTextValidatorStyle style) const;

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
        Sets the exclude char list (invalid characters for the user input).

        @note It's an error to exclude an already included character.
        @note This function may cancel the effect of @c wxFILTER_SPACE if the passed
        in string @a chars contains the @b space character.
    */
    void SetCharExcludes(const wxString& chars);

    /**
        Sets the include list (valid values for the user input).
    */
    void SetIncludes(const wxArrayString& stringList);

    /**
        Sets the include char list (additional valid values for the user input).

        @note It's an error to include an already excluded character.
    */
    void SetCharIncludes(const wxString& chars);

    /**
        Adds @a exclude to the list of excluded values.

        @since 3.1.3
    */
    void AddExclude(const wxString& exclude);

    /**
        Adds @a include to the list of included values.

        @since 3.1.3
    */
    void AddInclude(const wxString& include);

    /**
        Adds @a chars to the list of excluded characters.

        @since 3.1.3

        @note It's an error to exclude an already included character.
    */
    void AddCharExcludes(const wxString& chars);

    /**
        Adds @a chars to the list of included characters.

        @since 3.1.3

        @note It's an error to include an already excluded character.
    */
    void AddCharIncludes(const wxString& chars);


    /**
        Sets the validator style which must be a combination of one or more
        of the ::wxTextValidatorStyle values.

        Note that wxFILTER_{INCLUDE,EXCLUDE}[_CHAR]_LIST pairs are no longer
        required and are kept for backward compatibility only.
    */
    void SetStyle(long style);

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

protected:

    /**
        Returns @true if the char @a c is allowed to be in the user input string.
        additional characters, set by SetCharIncludes() or AddCharIncludes() are
        also considered.

        @since 3.1.3
    */
    bool IsCharIncluded(const wxUniChar& c) const;

    /**
        Returns @true if the char @a c is not allowed to be in the user input string.
        (characters set by SetCharExcludes() or AddCharExcludes()).

        @since 3.1.3
    */
    bool IsCharExcluded(const wxUniChar& c) const;

    /**
        Returns @true if the string @a str is one of the includes strings set by
        SetIncludes() or AddInclude().

        Notice that an empty include list is ignored (i.e. we should return true
        if the include list is empty).

        @since 3.1.3
    */
    bool IsIncluded(const wxString& str) const;

    /**
        Returns @true if the string @a str is one of the excludes strings set by
        SetExcludes() or AddExclude().

        @since 3.1.3
    */
    bool IsExcluded(const wxString& str) const;

    /**
        Returns the error message if the contents of @a val are invalid
        or the empty string if @a val is valid.
    */
    virtual wxString IsValid(const wxString& val) const;

    /// Returns false if the character @a c is invalid.
    bool IsValidChar(const wxUniChar& c) const;
};

