/////////////////////////////////////////////////////////////////////////////
// Name:        props.h
// Purpose:     interface of some wxPGProperty subclasses
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////



/** @class wxPGInDialogValidator
    @ingroup classes
    Creates and manages a temporary wxTextCtrl for validation purposes.
    Uses wxPropertyGrid's current editor, if available.
*/
class wxPGInDialogValidator
{
public:
    wxPGInDialogValidator();
    ~wxPGInDialogValidator();

    bool DoValidate( wxPropertyGrid* propGrid,
                     wxValidator* validator,
                     const wxString& value );
};


// -----------------------------------------------------------------------
// Property classes
// -----------------------------------------------------------------------

#define wxPG_PROP_PASSWORD  wxPG_PROP_CLASS_SPECIFIC_2

/** @class wxStringProperty
    @ingroup classes
    Basic property with string value.

    <b>Supported special attributes:</b>
    - "Password": set to 1 in order to enable wxTE_PASSWORD on the editor.

    @remarks
    - If value "<composed>" is set, then actual value is formed (or composed)
      from values of child properties.
*/
class wxStringProperty : public wxPGProperty
{
public:
    wxStringProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxString& value = wxEmptyString );
    virtual ~wxStringProperty();

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;

    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    /** This is updated so "<composed>" special value can be handled.
    */
    virtual void OnSetValue();
};


/** Constants used with NumericValidation<>().
*/
enum wxPGNumericValidationConstants
{
    /** Instead of modifying the value, show an error message.
    */
    wxPG_PROPERTY_VALIDATION_ERROR_MESSAGE      = 0,

    /** Modify value, but stick with the limitations.
    */
    wxPG_PROPERTY_VALIDATION_SATURATE           = 1,

    /** Modify value, wrap around on overflow.
    */
    wxPG_PROPERTY_VALIDATION_WRAP               = 2
};



/**
    A more comprehensive numeric validator class.
*/
class wxNumericPropertyValidator : public wxTextValidator
{
public:
    enum NumericType
    {
        Signed = 0,
        Unsigned,
        Float
    };

    wxNumericPropertyValidator( NumericType numericType, int base = 10 );
    virtual ~wxNumericPropertyValidator() { }
    virtual bool Validate(wxWindow* parent);
};



/** @class wxIntProperty
    @ingroup classes
    Basic property with integer value.

    Seamlessly supports 64-bit integer (wxLongLong) on overflow.

    <b>Example how to use seamless 64-bit integer support</b>

      Getting value:

      @code
          wxLongLong_t value = pg->GetPropertyValueAsLongLong();
      @endcode

         or

      @code
          wxLongLong_t value;
          wxVariant variant = property->GetValue();
          if ( variant.GetType() == "wxLongLong" )
              value = wxLongLongFromVariant(variant);
          else
              value = variant.GetLong();
      @endcode

      Setting value:

       @code
          pg->SetPropertyValue(longLongVal);
      @endcode

         or

      @code
          property->SetValue(WXVARIANT(longLongVal));
      @endcode


    <b>Supported special attributes:</b>
    - "Min", "Max": Specify acceptable value range.
*/
class wxIntProperty : public wxPGProperty
{
public:
    wxIntProperty( const wxString& label = wxPG_LABEL,
                   const wxString& name = wxPG_LABEL,
                   long value = 0 );
    virtual ~wxIntProperty();

    wxIntProperty( const wxString& label,
                   const wxString& name,
                   const wxLongLong& value );
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool ValidateValue( wxVariant& value,
                                wxPGValidationInfo& validationInfo ) const;
    virtual bool IntToValue( wxVariant& variant,
                             int number,
                             int argFlags = 0 ) const;
    static wxValidator* GetClassValidator();
    virtual wxValidator* DoGetValidator() const;

    /** Validation helper.
    */
    static bool DoValidation( const wxPGProperty* property,
                              wxLongLong_t& value,
                              wxPGValidationInfo* pValidationInfo,
                              int mode =
                                wxPG_PROPERTY_VALIDATION_ERROR_MESSAGE );
};


/** @class wxUIntProperty
    @ingroup classes
    Basic property with unsigned integer value.
    Seamlessly supports 64-bit integer (wxULongLong) on overflow.

    <b>Supported special attributes:</b>
    - "Min", "Max": Specify acceptable value range.
    - "Base": Define base. Valid constants are wxPG_BASE_OCT, wxPG_BASE_DEC,
    wxPG_BASE_HEX and wxPG_BASE_HEXL (lowercase characters). Arbitrary bases
    are <b>not</b> supported.
    - "Prefix": Possible values are wxPG_PREFIX_NONE, wxPG_PREFIX_0x, and
    wxPG_PREFIX_DOLLAR_SIGN. Only wxPG_PREFIX_NONE works with Decimal and Octal
    numbers.

    @remarks
    - For example how to use seamless 64-bit integer support, see wxIntProperty
    documentation (just use wxULongLong instead of wxLongLong).
*/
class wxUIntProperty : public wxPGProperty
{
public:
    wxUIntProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    unsigned long value = 0 );
    virtual ~wxUIntProperty();
    wxUIntProperty( const wxString& label,
                    const wxString& name,
                    const wxULongLong& value );
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
    virtual bool ValidateValue( wxVariant& value,
                                wxPGValidationInfo& validationInfo ) const;
    virtual wxValidator* DoGetValidator () const;
    virtual bool IntToValue( wxVariant& variant,
                             int number,
                             int argFlags = 0 ) const;
protected:
    wxByte      m_base;
    wxByte      m_realBase; // translated to 8,16,etc.
    wxByte      m_prefix;
};


/** @class wxFloatProperty
    @ingroup classes
    Basic property with double-precision floating point value.

    <b>Supported special attributes:</b>
    - "Precision": Sets the (max) precision used when floating point value is
    rendered as text. The default -1 means infinite precision.
*/
class wxFloatProperty : public wxPGProperty
{
public:
    wxFloatProperty( const wxString& label = wxPG_LABEL,
                     const wxString& name = wxPG_LABEL,
                     double value = 0.0 );
    virtual ~wxFloatProperty();

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
    virtual wxVariant DoGetAttribute( const wxString& name ) const;
    virtual bool ValidateValue( wxVariant& value,
                                wxPGValidationInfo& validationInfo ) const;

    /** Validation helper.
    */
    static bool DoValidation( const wxPGProperty* property,
                              double& value,
                              wxPGValidationInfo* pValidationInfo,
                              int mode =
                                 wxPG_PROPERTY_VALIDATION_ERROR_MESSAGE );
    static wxValidator* GetClassValidator();
    virtual wxValidator* DoGetValidator () const;

protected:
    int m_precision;
};



/** @class wxBoolProperty
    @ingroup classes
    Basic property with boolean value.

    <b>Supported special attributes:</b>
    - "UseCheckbox": Set to 1 to use check box editor instead of combo box.
    - "UseDClickCycling": Set to 1 to cycle combo box instead showing the list.
*/
class wxBoolProperty : public wxPGProperty
{
public:
    wxBoolProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    bool value = false );
    virtual ~wxBoolProperty();

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool IntToValue( wxVariant& variant,
                             int number, int argFlags = 0 ) const;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
};



// If set, then selection of choices is static and should not be
// changed (i.e. returns NULL in GetPropertyChoices).
#define wxPG_PROP_STATIC_CHOICES    wxPG_PROP_CLASS_SPECIFIC_1

/** @class wxEnumProperty
    @ingroup classes
    You can derive custom properties with choices from this class. See
    wxBaseEnumProperty for remarks.

    @remarks
    - Updating private index is important. You can do this either by calling
    SetIndex() in IntToValue, and then letting wxBaseEnumProperty::OnSetValue
    be called (by not implementing it, or by calling super class function in
    it) -OR- you can just call SetIndex in OnSetValue.
*/
class wxEnumProperty : public wxPGProperty
{
public:
    wxEnumProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxChar* const* labels = NULL,
                    const long* values = NULL,
                    int value = 0 );
    
    wxEnumProperty( const wxString& label,
                    const wxString& name,
                    wxPGChoices& choices,
                    int value = 0 );

    // Special constructor for caching choices (used by derived class)
    wxEnumProperty( const wxString& label,
                    const wxString& name,
                    const wxChar* const* labels,
                    const long* values,
                    wxPGChoices* choicesCache,
                    int value = 0 );

    wxEnumProperty( const wxString& label,
                    const wxString& name,
                    const wxArrayString& labels,
                    const wxArrayInt& values = wxArrayInt(),
                    int value = 0 );

    virtual ~wxEnumProperty();

    size_t GetItemCount() const;

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool ValidateValue( wxVariant& value,
                                wxPGValidationInfo& validationInfo ) const;

    // If wxPG_FULL_VALUE is not set in flags, then the value is interpreted
    // as index to choices list. Otherwise, it is actual value.
    virtual bool IntToValue( wxVariant& variant,
                             int number,
                             int argFlags = 0 ) const;

    //
    // Additional virtuals

    // This must be overridden to have non-index based value
    virtual int GetIndexForValue( int value ) const;

    // GetChoiceSelection needs to overridden since m_index is
    // the true index, and various property classes derived from
    // this take advantage of it.
    virtual int GetChoiceSelection() const;

    virtual void OnValidationFailure( wxVariant& pendingValue );

protected:

    int GetIndex() const;
    void SetIndex( int index );

    bool ValueFromString_( wxVariant& value,
                           const wxString& text,
                           int argFlags ) const;
    bool ValueFromInt_( wxVariant& value, int intVal, int argFlags ) const;

    static void ResetNextIndex();

};



/** @class wxEditEnumProperty
    @ingroup classes
    wxEnumProperty with wxString value and writable combo box editor.

    @remarks
    Uses int value, similar to wxEnumProperty, unless text entered by user is
    is not in choices (in which case string value is used).
*/
class wxEditEnumProperty : public wxEnumProperty
{
public:

    wxEditEnumProperty( const wxString& label,
                        const wxString& name,
                        const wxChar* const* labels,
                        const long* values,
                        const wxString& value );

    wxEditEnumProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name = wxPG_LABEL,
                        const wxArrayString& labels = wxArrayString(),
                        const wxArrayInt& values = wxArrayInt(),
                        const wxString& value = wxEmptyString );

    wxEditEnumProperty( const wxString& label,
                        const wxString& name,
                        wxPGChoices& choices,
                        const wxString& value = wxEmptyString );

    // Special constructor for caching choices (used by derived class)
    wxEditEnumProperty( const wxString& label,
                        const wxString& name,
                        const wxChar* const* labels,
                        const long* values,
                        wxPGChoices* choicesCache,
                        const wxString& value );

    virtual ~wxEditEnumProperty();

};



/** @class wxFlagsProperty
    @ingroup classes
    Represents a bit set that fits in a long integer. wxBoolProperty
    sub-properties are created for editing individual bits. Textctrl is created
    to manually edit the flags as a text; a continuous sequence of spaces,
    commas and semicolons is considered as a flag id separator.
    <b>Note:</b> When changing "choices" (ie. flag labels) of wxFlagsProperty,
    you will need to use SetPropertyChoices - otherwise they will not get
    updated properly.
*/
class wxFlagsProperty : public wxPGProperty
{
public:

    wxFlagsProperty( const wxString& label,
                     const wxString& name,
                     const wxChar* const* labels,
                     const long* values = NULL,
                     long value = 0 );

    wxFlagsProperty( const wxString& label,
                     const wxString& name,
                     wxPGChoices& choices,
                     long value = 0 );

    wxFlagsProperty( const wxString& label = wxPG_LABEL,
                     const wxString& name = wxPG_LABEL,
                     const wxArrayString& labels = wxArrayString(),
                     const wxArrayInt& values = wxArrayInt(),
                     int value = 0 );

    virtual ~wxFlagsProperty ();

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int flags ) const;
    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const;
    virtual void RefreshChildren();
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    // GetChoiceSelection needs to overridden since m_choices is
    // used and value is integer, but it is not index.
    virtual int GetChoiceSelection() const;

    // helpers
    size_t GetItemCount() const;
    const wxString& GetLabel( size_t ind ) const;

protected:
    // Used to detect if choices have been changed
    wxPGChoicesData*        m_oldChoicesData;

    // Needed to properly mark changed sub-properties
    long                    m_oldValue;

    // Converts string id to a relevant bit.
    long IdToBit( const wxString& id ) const;

    // Creates children and sets value.
    void Init();
};



/** @class wxPGFileDialogAdapter
    @ingroup classes
*/
class wxPGFileDialogAdapter : public wxPGEditorDialogAdapter
{
public:
    virtual bool DoShowDialog( wxPropertyGrid* propGrid,
                               wxPGProperty* property );
};



// Indicates first bit useable by derived properties.
#define wxPG_PROP_SHOW_FULL_FILENAME  wxPG_PROP_CLASS_SPECIFIC_1

/** @class wxFileProperty
    @ingroup classes
    Like wxLongStringProperty, but the button triggers file selector instead.

    <b>Supported special attributes:</b>
    - "Wildcard": Sets wildcard (see wxFileDialog for format details), "All
    files..." is default.
    - "ShowFullPath": Default 1. When 0, only the file name is shown (i.e. drive
      and directory are hidden).
    - "ShowRelativePath": If set, then the filename is shown relative to the
    given path string.
    - "InitialPath": Sets the initial path of where to look for files.
    - "DialogTitle": Sets a specific title for the dir dialog.
*/
class wxFileProperty : public wxPGProperty
{
public:

    wxFileProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxString& value = wxEmptyString );
    virtual ~wxFileProperty ();

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual wxPGEditorDialogAdapter* GetEditorDialog() const;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    static wxValidator* GetClassValidator();
    virtual wxValidator* DoGetValidator() const;

    /**
        Returns filename to file represented by current value.
    */
    wxFileName GetFileName() const;

protected:
    wxString    m_wildcard;
    wxString    m_basePath; // If set, then show path relative to it
    wxString    m_initialPath; // If set, start the file dialog here
    wxString    m_dlgTitle; // If set, used as title for file dialog
    int         m_indFilter; // index to the selected filter
};



#define wxPG_PROP_NO_ESCAPE     wxPG_PROP_CLASS_SPECIFIC_1

/** @class wxPGLongStringDialogAdapter
    @ingroup classes
*/
class wxPGLongStringDialogAdapter : public wxPGEditorDialogAdapter
{
public:
    virtual bool DoShowDialog( wxPropertyGrid* propGrid,
                               wxPGProperty* property );
};


/** @class wxLongStringProperty
    @ingroup classes
    Like wxStringProperty, but has a button that triggers a small text
    editor dialog.
*/
class wxLongStringProperty : public wxPGProperty
{
public:

    wxLongStringProperty( const wxString& label = wxPG_LABEL,
                          const wxString& name = wxPG_LABEL,
                          const wxString& value = wxEmptyString );
    virtual ~wxLongStringProperty();

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event );

    // Shows string editor dialog. Value to be edited should be read from
    // value, and if dialog is not cancelled, it should be stored back and true
    // should be returned if that was the case.
    virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value );

    static bool DisplayEditorDialog( wxPGProperty* prop,
                                     wxPropertyGrid* propGrid,
                                     wxString& value );
};




/** @class wxDirProperty
    @ingroup classes
    Like wxLongStringProperty, but the button triggers dir selector instead.

    <b>Supported special attributes:</b>
    - "DialogMessage": Sets specific message in the dir selector.
*/
class wxDirProperty : public wxLongStringProperty
{
public:
    wxDirProperty( const wxString& name = wxPG_LABEL,
                   const wxString& label = wxPG_LABEL,
                   const wxString& value = wxEmptyString );
    virtual ~wxDirProperty();

    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
    virtual wxValidator* DoGetValidator() const;

    virtual bool OnButtonClick ( wxPropertyGrid* propGrid, wxString& value );

protected:
    wxString    m_dlgMessage;
};


// wxBoolProperty specific flags
#define wxPG_PROP_USE_CHECKBOX      wxPG_PROP_CLASS_SPECIFIC_1
// DCC = Double Click Cycles
#define wxPG_PROP_USE_DCC           wxPG_PROP_CLASS_SPECIFIC_2



/** @class wxArrayStringProperty
    @ingroup classes
    Property that manages a list of strings.
*/
class wxArrayStringProperty : public wxPGProperty
{
public:
    wxArrayStringProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayString& value = wxArrayString() );
    virtual ~wxArrayStringProperty();

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event );
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    // Implement in derived class for custom array-to-string conversion.
    virtual void ConvertArrayToString(const wxArrayString& arr,
                                      wxString* pString,
                                      const wxUniChar& delimiter) const;

    // Shows string editor dialog. Value to be edited should be read from
    // value, and if dialog is not cancelled, it should be stored back and true
    // should be returned if that was the case.
    virtual bool OnCustomStringEdit( wxWindow* parent, wxString& value );

    // Helper.
    virtual bool OnButtonClick( wxPropertyGrid* propgrid,
                                wxWindow* primary,
                                const wxChar* cbt );

    // Creates wxPGArrayEditorDialog for string editing. Called in OnButtonClick.
    virtual wxPGArrayEditorDialog* CreateEditorDialog();

    enum ConversionFlags
    {
        Escape          = 0x01,
        QuoteStrings    = 0x02
    };

    /**
        Generates contents for string dst based on the contents of
        wxArrayString src.
    */
    static void ArrayStringToString( wxString& dst, const wxArrayString& src,
                                     wxUniChar delimiter, int flags );

protected:
    // Previously this was to be implemented in derived class for array-to-
    // string conversion. Now you should implement ConvertValueToString()
    // instead.
    virtual void GenerateValueAsString();

    wxString        m_display; // Cache for displayed text.
    wxUniChar       m_delimiter;
};


// -----------------------------------------------------------------------
// wxPGArrayEditorDialog
// -----------------------------------------------------------------------

#define wxAEDIALOG_STYLE \
    (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxOK | wxCANCEL | wxCENTRE)

class wxPGArrayEditorDialog : public wxDialog
{
public:
    wxPGArrayEditorDialog();
    virtual ~wxPGArrayEditorDialog();

    void Init();

    wxPGArrayEditorDialog( wxWindow *parent,
                         const wxString& message,
                         const wxString& caption,
                         long style = wxAEDIALOG_STYLE,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& sz = wxDefaultSize );

    bool Create( wxWindow *parent,
                 const wxString& message,
                 const wxString& caption,
                 long style = wxAEDIALOG_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize );

    void EnableCustomNewAction();

    /** Sets tooltip text for button allowing the user to enter new string.
        @since 3.1.3
    */
    void SetNewButtonText(const wxString& text);

    /** Set value modified by dialog.
    */
    virtual void SetDialogValue( const wxVariant& value );

    /** Return value modified by dialog.
    */
    virtual wxVariant GetDialogValue() const;

    /** Override to return wxValidator to be used with the wxTextCtrl
        in dialog. Note that the validator is used in the standard
        wx way, ie. it immediately prevents user from entering invalid
        input.

        @remarks
        Dialog frees the validator.
    */
    virtual wxValidator* GetTextCtrlValidator() const;

    /** Returns true if array was actually modified
    */
    bool IsModified() const;

    int GetSelection() const;

protected:
    wxEditableListBox*  m_elb;
    wxWindow*           m_elbSubPanel;
    wxWindow*           m_lastFocused;

    /** A new item, edited by user, is pending at this index.
        It will be committed once list ctrl item editing is done.
    */
    int             m_itemPendingAtIndex;

    bool            m_modified;
    bool            m_hasCustomNewAction;

    virtual wxString ArrayGet( size_t index ) = 0;
    virtual size_t ArrayGetCount() = 0;
    virtual bool ArrayInsert( const wxString& str, int index ) = 0;
    virtual bool ArraySet( size_t index, const wxString& str ) = 0;
    virtual void ArrayRemoveAt( int index ) = 0;
    virtual void ArraySwap( size_t first, size_t second ) = 0;

    virtual bool OnCustomNewAction(wxString* resString);
};


// -----------------------------------------------------------------------
// wxPGArrayStringEditorDialog
// -----------------------------------------------------------------------

class wxPGArrayStringEditorDialog : public wxPGArrayEditorDialog
{
public:
    wxPGArrayStringEditorDialog();
    virtual ~wxPGArrayStringEditorDialog() { }

    void Init();

    virtual void SetDialogValue( const wxVariant& value );
    virtual wxVariant GetDialogValue() const;

    void SetCustomButton( const wxString& custBtText,
                          wxArrayStringProperty* pcc );

    virtual bool OnCustomNewAction(wxString* resString);

protected:
    wxArrayString   m_array;

    wxArrayStringProperty*     m_pCallingClass;

    virtual wxString ArrayGet( size_t index );
    virtual size_t ArrayGetCount();
    virtual bool ArrayInsert( const wxString& str, int index );
    virtual bool ArraySet( size_t index, const wxString& str );
    virtual void ArrayRemoveAt( int index );
    virtual void ArraySwap( size_t first, size_t second );
};

