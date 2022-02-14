/////////////////////////////////////////////////////////////////////////////
// Name:        maskededit.h
// Purpose:     interface of wxMaskedEdit
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
@defgroup group_maskededit_predef Masked edit predefined functions
*/

/**
    Struct with the colours used for different cases with wxMaskedEditText and
    wxMaskedEditCombo.

    If you don't set a member of this struct, the standard system default
    colour for the control is used for it.

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEditText, wxMaskedEditCombo

    @since 3.1.6

*/
struct wxMaskedEditColours
{
    /// Foreground colour used when everything is OK.
    wxColour colOKForeground;
    /// Background colour used when everything is OK.
    wxColour colOKBackground;

    /// Foreground colour used when something is wrong.
    wxColour colInvaldForeground;
    /// Background colour used when something is wrong.
    wxColour colInvaldBackground;

    /// Foreground colour used for an empty control's value.
    wxColour colBlankForeground;
    /// Background colour used for an empty control's value.
    wxColour colBlankBackground;
};

/**
    @class wxEditFieldFlags

    Container for flags used in a field of a masked edit control.

    There are three flags: alignment, fill char and padding char.

    You can create wxEditFieldFlags objects which can be reused for several
    masked controls.

    @code
    wxEditFieldFlags fieldflags;
    maskedCtrlA->SetFieldFlags(n, fieldflags);
    maskedCtrlB->SetAllFieldFlags(fieldflags);
    @endcode

    By specification, all methods of wxEditFieldFlags return the
    wxEditFieldFlags object itself to allow chaining multiple method calls,
    like in this example:

    @code
    maskedCtrl->SetFieldFlags(n, wxEditFieldFlags().SetFillChar('_'));
    @endcode

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEdit

    @since 3.1.6
*/
class wxMaskedEditFieldFlags
{
public:
    /**
        Constructor.
        Sets the alignment of this wxEditFieldFlags to @a alignment.

        Also, by default, sets the fill char to ' ' (space) and the
        padding char to '\0' (null).

        @param alignment
            @c wxALIGN_LEFT (default) or @c wxALIGN_RIGHT.

    */
    wxMaskedEditFieldFlags(wxAlignment alignment = wxALIGN_LEFT);

    /**
        Sets the alignment of this wxEditFieldFlags to @a alignment.

        @param alignment
            @c wxALIGN_LEFT (default) or @c wxALIGN_RIGHT.

    */
    wxMaskedEditFieldFlags& SetAlignment(wxAlignment alignment);

    /**
        Sets the fill char of this wxEditFieldFlags to @a fillchar.

        The fill char is used to fill the empty (i.e. not typed yet)
        positions in the field for showing them. Typically it is a space.

        @param fillchar
            Any printable character.

    */
    wxMaskedEditFieldFlags& SetFillChar(wxChar fillchar);

    /**
        Sets the padding char of this wxEditFieldFlags to @a paddingchar.

        The padding char is used to fill the empty (i.e. not typed yet)
        positions in the field for the return of wxMaskedEdit::GetAllFieldsValue()
        and wxMaskedEdit::GetFieldValue().

        Default is '\0' (null) which is interpreted as "don't fill"

        @param paddingchar
            Any printable character or '\0'.

    */
    wxMaskedEditFieldFlags& SetPaddingChar(wxChar paddingchar);

    /**
        Returns the alignment used in this object.

        @see SetAlignment()
    */
    wxAlignment GetAlignment() const;

    /**
        Returns the fill char used in this object.

        @see SetFillChar()
    */
    wxChar GetFillChar() const;

    /**
        Returns the padding char used in this object.

        @see SetPaddingChar()
    */
    wxChar GetPaddingChar() const;
};

/** @addtogroup group_maskededit_predef
/**@{*/
/**
    The prototype of function used to test a field of a masked edit control.

    The function accepts as parameters a pointer to the instance of the object
    that calls this function (your wxMaskedEditText or wxMaskedEditCombo), the
    field for which this function applies and a pointer to an object containing
    needed information for this function.

    An example of a currently defined function is wxMaskedRangeCheck().

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEdit::SetFieldFunction()

    @since 3.1.6

*/
typedef long wxMaskedFieldFunc(const wxMaskedEdit*, size_t, void*);

/**
    The prototype of function used to test the value of a masked edit control.

    The function accepts as parameters a pointer to the instance of the object
    that calls this function (your wxMaskedEditText or wxMaskedEditCombo) and
    a pointer to an object containing needed information for this function.

    An example of a currently defined function is wxMaskedDateShort().

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEdit::SetControlFunction()

    @since 3.1.6

    @header{wx/maskededit.h}
*/
typedef long wxMaskedFunc(const wxMaskedEdit*, void*);
/**@}*/

/**
    @class wxMaskedEdit

    This is the base class for all masked edit controls. Here is where most
    of their features are implemented.

    A masked edit control is like a form, with boxes where you write the data.
    You may have a 4-cells field to write a year, a 2-cells field to write a day
    of the month, a group of 4 fields, each of them is 3-cells long to write an
    IP v4 address, etc.

    Outside fields you can set "literals". They are characters that may help
    the user to type the data. This literals are fixed, the user can't change
    them.

    Each cell accepts a predefined type of character. These types can be digits,
    letters, digits or letters, any printable character, etc. All cells in a
    field must accept the same type; the field must be "homogeneous". You can't
    mix in the same field some cells accepting only digits and other cells
    accepting only letters.

    This per-cell definition, literals included, is called a "mask".

    Examples. With this code:
    @code
    wxMaskedEditText *ctr1, *ctr2;
    ctr1 = new wxMaskedEditText(this, ID1);
    ctr1->SetMask("99#.99#.99#.99#");
    ctr1->SetAllFieldsFlags(wxEditFieldFlags(wxALIGN_RIGHT));
    ctr2 = new wxMaskedEditText(this, ID2);
    ctr2->SetMask("####/9#/9#");
    ctr2->SetAllFieldsFlags(wxEditFieldFlags(wxALIGN_RIGHT));
    @endcode
    you could create two masked controls, and set an IP mask (4 fields) and
    a yyyy/mm/dd date mask (3 fields). All fields are also set to be right
    aligned. Their appearance, empty and with some characters typed, may be
    like these:
    @image html maskededit.png

    As said above, each cell is told to accept only one kind of characters,
    filtering all others. You can also set a function that test a specific
    field. You can assign a different function for each field. You can also
    set a function that works with the whole control's value. All of this
    functions just do test (i.e. they don't change anything) and inform the
    main code if that field/control has an acceptable value.

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEditText, wxMaskedEditCombo

    @since 3.1.6
*/
class wxMaskedEdit
{
public:
    /**
        Default constructor.
    */
    wxMaskedEdit();

    /**
        Default destructor.
    */
    ~wxMaskedEdit();

    /**
        Sets the mask.

    The mask is composed of command-characters and literals. A group of
    consecutive commands forms a field.

    If no field is defined with this new mask, nothing is changed and it
    returns @false. Otherwise, the previous per-field definitions (flags
    and test functions) are reset.

    Command-characters and their meaning:
    @beginTable
    @row2col{ # , Required digit.}
    @row2col{ 9 , Optional digit.}
    @row2col{ A , Required alpha character.}
    @row2col{ a , Optional alpha character.}
    @row2col{ N , Required alphanumeric character (i.e. letter or digit).}
    @row2col{ n , Optional alphanumeric character.}
    @row2col{ C , Required character (any with keycode > 31).}
    @row2col{ c , Optional character (any with keycode > 31).}
    @row2col{ X , Required hexadecimal character [0-F].}
    @row2col{ x , Optional hexadecimal character [0-F].}
    @row2col{{x}, The previous character (one of the above) is taken x times.<br>
        This is an "expander": #{3} expands into ### and X{4} expands into XXXX.}
    @row2col{ > , Convert all following characters to upper case.}
    @row2col{ < , Convert all following characters to lower case.}
    @row2col{ ^ , Switch off upper/lower case conversion.}
    @row2col{ \ , Escape the next character and use it as a literal.}
    @row2col{ | , Special literal. It's invisible, takes no space, but still
     is a fields separator.}
    @endTable
     Notice the above specifications (except 'C' and 'c') are tested using
     wxIsxxx functions. So, they are locale dependent.

     Any other char is a literal. It is shown in the control, but it can
     not be edited.

     Literals are also used as fields separators.

     Space is not a character (except for C and c commands), and it is typically
     used as fillchar (see wxEditFieldFlags). If you want to use C or c command
     you must set the fillchar to some character you don't allow.

     @param mask
        The string that defines the mask.
    */
    bool SetMask(const wxString& mask);

    /**
        Sets the colours for different cases.

        You don't need to set all of them. Default system values are used for
        those colours not set.

        @param colours
            The struct containing the different colours.
    */
    void SetMaskedColours(const wxMaskedEditColours& colours);

    /**
        Sets the flags for a field.

        You can set alignment, fill char and padding char.

        This is a per-field setter. Different fields may have different flags.

        @param index
            The field (0-based) to set the flags.

        @param flags
            The flags for that field.

        @see wxEditFieldFlags, SetAllFieldsFlags()
    */
    void SetFieldFlags(size_t index, const wxEditFieldFlags& flags);

    /**
        Sets all fields with the same flags.

        @param flags
            The flags to be set for all fields.

        @see wxEditFieldFlags, SetFieldFlags()
    */
    void SetAllFieldsFlags(const wxEditFieldFlags& flags);

    /**
        Sets the test function for a field.

        Apart from filtering characters as defined in the mask, a field can
        be tested. For example, for an "hour field" it is advisable to test
        the number to be in [0-23] range.

        The result of this test does not change the field's value. It only
        informs if the result of test is satisfactory or not.

        @param index
            The field (0-based) to which applies the function.

        @param fn
            A pointer to the function to be used for field @a index, defined as:
        @code
  typedef long wxMaskedFieldFunc(const wxMaskedEdit*, size_t, void*);
        @endcode

        @param param
            A pointer to the parameters to be passed to the function.

        Currently there is one predefined function, wxMaskedRangeCheck(), which
        uses the also predefined class wxRangeParams. An example on how to use
        it is:
        @code
        wxRangeParams rParm;
        rParm.rmin = 0;
        rParm.rmax = 23;
        maskedCtrl->SetFieldFunction(field, &wxMaskedRangeCheck, &rParm);
        @endcode

        You can also set your own function, which may use your own parameters.

        @see SetAllFieldsFunction(), SetControlFunction()
    */
    void SetFieldFunction(size_t index, wxMaskedFieldFunc* fn, void* param);

    /**
        Sets all fields with the same test function.

        @param fn
            A pointer to the function to be used for all fields, defined as:
        @code
  typedef long wxMaskedFieldFunc(const wxMaskedEdit*, size_t, void*);
        @endcode

        @param param
            A pointer to the parameters to be passed to the function.

        @see SetFieldFunction()
    */
    void SetAllFieldsFunction(wxMaskedFieldFunc* fn, void* param);

    /**
        Sets the test function for the whole control.

        The result of this test does not change the control's value. It only
        informs if the result of test is satisfactory or not.

        @param fn
            A pointer to the function to be used, defined as:
        @code
  typedef long wxMaskedFunc(const wxMaskedEdit*, void*);
        @endcode

        @param param
            A pointer to the parameters to be passed to the function.

        Currently there is one predefined function, wxMaskedDateShort(), which
        uses the also predefined struct wxDateParams. Here is an example on how
        to use it, being the date in shape yyyy/mm/dd and being the year the
        fourth field in the control:
        @code
        wxDateParams dparam;
        dparam.yearField = 3;
        dparam.monthField = 4;
        dparam.dayField = 5;
        maskedCtrl->SetControlFunction( &wxMaskedDateShort, &dparam );
        @endcode

        You can also set your own function, which may use your own parameters.

        @see SetFieldFunction()
    */
    void SetControlFunction(wxMaskedFunc* fn, void* param);

    /**
        Sets all parameters at once, replacing old ones.

        @param params
            All the parameters that govern this control: mask, flags, colours
            and test functions.
    */
    bool SetParams(const wxMaskedEditParams& params);

    /**
        Sets the control's value from an undecorated text.

        The plain value (i.e. undecorated text, without "literals") is supposed
        to resemble the user's input. So this text does not need to fill all
        cells. If this is the case, filling begins at the first field, this is,
        from the left of the control.

        @param plainValue
            The text resembling the characters typed by the user.

        If any character is not allowed, it returns @false.

        If the plain value is bigger than the mask, it returns @false.

        @see GetAllFieldsValue(), SetFieldValue(), wxMaskedEditText::SetValue()
    */
    bool SetPlainValue(const wxString& plainValue);

    /**
        Sets the value for a field.

        @param plainValue
            The text replacing the current field's one.

        If any character is not allowed, it returns @false.

        @see GetFieldValue(), SetPlainValue(), wxMaskedEditText::SetValue()
    */
    bool SetFieldValue(size_t index, const wxString& plainValue);

    /**
        Checks whether all cells, fields and the whole control's value are OK.

        Returns the position of the error found, or '-1' if everything is well.
    */
    long GetInvalidFieldIndex() const;

    /**
        Sets bell on/off when a character is not accepted.

        @param bell
            Set it to @false if you don't want the sound.
    */
    void SetBellOnError(bool bell);


    //Accessors -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    /**
        Gets the number of fields in the mask.
    */
    size_t GetFieldsCount() const;

    /**
        Gets control's value, without decorations.

        The return value is the current control's value, after removing
        all "literals" and filling untyped cells with per-field padding chars.

        @see SetPlainValue(), GetFieldValue(), wxEditFieldFlags
    */
    wxString GetAllFieldsValue() const;

    /**
        Gets the current text in the field @a index.

        Empty cells are filled with the padding character set for this field.
    */
    wxString GetFieldValue(size_t index) const;

    /**
        Gets the mask.

        Returns the string passed at SetMask().
    */
    wxString GetMask() const;

    /**
        Gets the colours used for different cases.
    */
    wxMaskedEditColours GetMaskedColours() const;

    /**
        Gets the wxEditFieldFlags instance used in the field @a index.
    */
    wxEditFieldFlags GetFieldFlags(size_t index) const;

    /**
        Gets the parameters that set the behaviour of this control.
    */
    wxMaskedEditParams GetParams() const;
};


/**
     @class wxMaskedEditText
 
     A specialized wxTextCtrl where the user can only fill some of its
     positions. The rest of positions are fixed literal characters.
 
     @remarks
     Please refer to wxMaskedEdit documentation for the description of methods
     operating with the text.
 
     The events this class emits are those of wxTextCtrl.
 
     @library{wxcore}
     @category{ctrl}
 
     @see wxMaskedEdit, wxMaskedEditCombo, wxTextCtrl
 
     @since 3.1.6
 */
 class wxMaskedEditText : public wxMaskedEdit, public wxTextCtrl
 {
 public:
     /**
         Default constructor.
     */
     wxMaskedEditText();
 
     /**
         Constructor, creating and showing a wxTextCtrl.
 
         Note the style wxTE_MULTILINE is not allowed.
 
         The control is set with a fixed pitch font.
 
         Refer to wxTextCtrl::wxTextCtrl() for the description of the parameters.
     */
     wxMaskedEditText(wxWindow *parent, wxWindowID id,
                      const wxString& value = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxTextCtrlNameStr);
 
     /**
         Creates the wxMaskedEditText for two-step construction. Derived classes
         should call or replace this function.
 
         Refer to wxMaskedEditText() for further details.
 
         Refer to wxTextCtrl::Create() for the description of the parameters.
     */
     bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    /**
        Destructor, destroying the masked control.
    */
    virtual ~wxMaskedEditText();

    /**
        Sets the whole text, decorations included.

        For a mask like "9#:9#" (likely used for hh:mm) if your value
        is 14 hours 25 minutes, you must pass "14:25". Also you must respect
        alignment and fill char for each field.

        @param value
            The text to set. It will be tested and adapted to the mask.

       Notice that this method will generate a @c wxEVT_COMMAND_TEXT_UPDATED
       event, use ChangeValue() if this is undesirable.

       @see wxTextEntry::SetValue(), wxMaskedEdit::SetPlainValue()
    */
    virtual void SetValue(const wxString& value);

    /**
        Same as SetValue(), except it does not generate the
        wxEVT_COMMAND_TEXT_UPDATED event.

        @see SetValue(), wxTextEntry::ChangeValue()
    */
    virtual void ChangeValue(const wxString& value);

};

/**
    @class wxMaskedEditCombo

    A specialized wxComboBox. For the edit part, the user can only fill
    some of its positions. The rest of positions are fixed literal characters.

    @remarks
    Please refer to wxMaskedEdit documentation for the description of methods
    operating with the text.

    The events this class emits are those of wxComboBox.

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEdit, wxMaskedEditText, wxComboBox

    @since 3.1.6
*/
class wxMaskedEditCombo : public wxMaskedEdit,  public wxComboBox
{
public:
    /**
        Default constructor.
    */
    wxMaskedEditCombo();

    /**
        Constructor, creating and showing a wxComboBox.

        The control is set with a fixed pitch font.

        Refer to wxComboBox::wxComboBox() for the description of the parameters.
    */
    wxMaskedEditCombo(wxWindow *parent, wxWindowID id,
                      const wxString& value = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      int n = 0, const wxString choices[] = NULL,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxComboBoxNameStr);

    /**
        Constructor, creating and showing a wxComboBox.

        The control is set with a fixed pitch font.

        Refer to wxComboBox::wxComboBox() for the description of the parameters.
    */
    wxMaskedEditCombo(wxWindow *parent, wxWindowID id,
                      const wxString& value,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxComboBoxNameStr);

    //@{
    /**
        Creates the wxMaskedEditCombo for two-step construction. Derived classes
        should call or replace this function.

        Refer to wxMaskedEditCombo() for further details.

        Refer to wxComboBox::Create() for the description of the parameters.
    */
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    //@}

    /**
        Destructor, destroying the masked control.
    */
    virtual ~wxMaskedEditCombo() {};

    /**
        Sets the whole text, decorations included, for the edit part of the
        wxComboBox.

        For a mask like "9#:9#" (likely used for hh:mm) if your value
        is 14 hours 25 minutes, you must pass "14:25". Also you must respect
        alignment and fill char for each field.

        @param value
            The text to set. It will be tested and adapted to the mask.

       Notice that this method will generate a @c wxEVT_COMMAND_TEXT_UPDATED
       event, use ChangeValue() if this is undesirable.

       @see wxComboBox::SetValue(), wxTextEntry::SetValue(),
       wxMaskedEdit::SetPlainValue()
    */
    virtual void SetValue(const wxString& value);

    /**
        Same as SetValue(), except it does not generate the
        wxEVT_COMMAND_TEXT_UPDATED event.

        @see SetValue(), wxTextEntry::ChangeValue()
    */
    virtual void ChangeValue(const wxString& value);

};


// ----------------------------------------------------------------------------
//                            Predefined functions
//
// They return '-1' if all is OK. Otherwise, return the position of the error.
// ----------------------------------------------------------------------------

/** @addtogroup group_maskededit_predef
/**@{*/
/**
    @class wxRangeParams

    Used to pass parameters to wxMaskedRangeCheck()

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEditText, wxMaskedEditCombo, wxMaskedEdit

    @since 3.1.6

*/
class wxRangeParams
{
public:
    wxRangeParams()
    {
        rmin = rmax = 0;
        base = 10;
    }
    ///min value
    long rmin;
    ///max value
    long rmax;
    ///base of the value to be checked. Default is 10.
    int base;
};

/**
    Predefined range check function used with wxMaskedEdit::SetFieldFunction().

    @param caller
        The masked edit control that calls this function.

    @param index
        The field (0-based) to test.

    @param params
        A pointer to a wxRangeParams object, containing @a min, @a max and
        @a base values.

    The functions tests the field's value to be in the [@a min-@a max] range.
    <br>@a min and @a max are valid values.

    Setting @a max < @a min makes the function to test the value to be outside
    the range (excluded range as opposed to included range). For example, 33
    or 90 are valid values for a [70-50] range.

    @library{wxcore}
    @category{ctrl}

    @since 3.1.6
*/
long wxMaskedRangeCheck(const wxMaskedEdit* caller, size_t index, void* params);

/**
    Used to pass parameters to wxMaskedDateShort()

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEditText, wxMaskedEditCombo, wxMaskedEdit

    @since 3.1.6
+*/
struct wxDateParams
{
    /// The field whose value is the day.
    size_t dayField;
    /// The field whose value is the month.
    size_t monthField;
    /// The field whose value is the year.
    size_t yearField;
};

/**
    Predefined date check function used with wxMaskedEdit::SetControlFunction().

    @param caller
        The masked edit control that calls this function.

    @param params
        A pointer to a wxDateParams object, that defines what fields match with
        day, month and year.

    Once the fields are known, their values are extracted and tested to form a
    valid date.

    @library{wxcore}
    @category{ctrl}

    @see wxMaskedEditText, wxMaskedEditCombo, wxMaskedEdit

    @since 3.1.6
*/
long wxMaskedDateShort(const wxMaskedEdit* caller, void* params);

/**@}*/

