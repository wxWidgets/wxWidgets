/////////////////////////////////////////////////////////////////////////////
// Name:        property.h
// Purpose:     interface of wxPGProperty
// Author:      wxWidgets team
// RCS-ID:      $Id:
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#define wxNullProperty  ((wxPGProperty*)NULL)


/** @section propgrid_property_attributes wxPropertyGrid Property Attribute Identifiers

    wxPGProperty::SetAttribute() and
    wxPropertyGridInterface::SetPropertyAttribute()
    accept one of these as attribute name argument .

    You can use strings instead of constants. However, some of these
    constants are redefined to use cached strings which may reduce
    your binary size by some amount.

    @{
*/

/** Set default value for property.
*/
#define wxPG_ATTR_DEFAULT_VALUE           wxS("DefaultValue")

/** Universal, int or double. Minimum value for numeric properties.
*/
#define wxPG_ATTR_MIN                     wxS("Min")

/** Universal, int or double. Maximum value for numeric properties.
*/
#define wxPG_ATTR_MAX                     wxS("Max")

/** Universal, string. When set, will be shown as text after the displayed
    text value. Alternatively, if third column is enabled, text will be shown
    there (for any type of property).
*/
#define wxPG_ATTR_UNITS                     wxS("Units")

/** Universal, string. When set, will be shown in property's value cell
    when displayed value string is empty, or value is unspecified.
*/
#define wxPG_ATTR_INLINE_HELP               wxS("InlineHelp")

/** wxBoolProperty specific, int, default 0. When 1 sets bool property to
    use checkbox instead of choice.
*/
#define wxPG_BOOL_USE_CHECKBOX              wxS("UseCheckbox")

/** wxBoolProperty specific, int, default 0. When 1 sets bool property value
    to cycle on double click (instead of showing the popup listbox).
*/
#define wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING  wxS("UseDClickCycling")

/** wxFloatProperty (and similar) specific, int, default -1. Sets the (max) precision
    used when floating point value is rendered as text. The default -1 means infinite
    precision.
*/
#define wxPG_FLOAT_PRECISION                wxS("Precision")

/** The text will be echoed as asterisks (wxTE_PASSWORD will be passed to textctrl etc).
*/
#define wxPG_STRING_PASSWORD                wxS("Password")

/** Define base used by a wxUIntProperty. Valid constants are
    wxPG_BASE_OCT, wxPG_BASE_DEC, wxPG_BASE_HEX and wxPG_BASE_HEXL
    (lowercase characters).
*/
#define wxPG_UINT_BASE                      wxS("Base")

/** Define prefix rendered to wxUIntProperty. Accepted constants
    wxPG_PREFIX_NONE, wxPG_PREFIX_0x, and wxPG_PREFIX_DOLLAR_SIGN.
    <b>Note:</b> Only wxPG_PREFIX_NONE works with Decimal and Octal
    numbers.
*/
#define wxPG_UINT_PREFIX                    wxS("Prefix")

/** wxFileProperty/wxImageFileProperty specific, wxChar*, default is detected/varies.
    Sets the wildcard used in the triggered wxFileDialog. Format is the
    same.
*/
#define wxPG_FILE_WILDCARD                  wxS("Wildcard")

/** wxFileProperty/wxImageFileProperty specific, int, default 1.
    When 0, only the file name is shown (i.e. drive and directory are hidden).
*/
#define wxPG_FILE_SHOW_FULL_PATH            wxS("ShowFullPath")

/** Specific to wxFileProperty and derived properties, wxString, default empty.
    If set, then the filename is shown relative to the given path string.
*/
#define wxPG_FILE_SHOW_RELATIVE_PATH        wxS("ShowRelativePath")

/** Specific to wxFileProperty and derived properties, wxString, default is empty.
    Sets the initial path of where to look for files.
*/
#define wxPG_FILE_INITIAL_PATH              wxS("InitialPath")

/** Specific to wxFileProperty and derivatives, wxString, default is empty.
    Sets a specific title for the dir dialog.
*/
#define wxPG_FILE_DIALOG_TITLE              wxS("DialogTitle")

/** Specific to wxDirProperty, wxString, default is empty.
    Sets a specific message for the dir dialog.
*/
#define wxPG_DIR_DIALOG_MESSAGE             wxS("DialogMessage")

/** Sets displayed date format for wxDateProperty.
*/
#define wxPG_DATE_FORMAT                    wxS("DateFormat")

/** Sets wxDatePickerCtrl window style used with wxDateProperty. Default
    is wxDP_DEFAULT | wxDP_SHOWCENTURY.
*/
#define wxPG_DATE_PICKER_STYLE              wxS("PickerStyle")

/** SpinCtrl editor, int or double. How much number changes when button is
    pressed (or up/down on keybard).
*/
#define wxPG_ATTR_SPINCTRL_STEP             wxS("Step")

/** SpinCtrl editor, bool. If true, value wraps at Min/Max.
*/
#define wxPG_ATTR_SPINCTRL_WRAP             wxS("Wrap")

/** wxMultiChoiceProperty, int. If 0, no user strings allowed. If 1, user strings
    appear before list strings. If 2, user strings appear after list string.
*/
#define wxPG_ATTR_MULTICHOICE_USERSTRINGMODE    wxS("UserStringMode")

/** wxColourProperty and its kind, int, default 1. Setting this attribute to 0 hides custom
    colour from property's list of choices.
*/
#define wxPG_COLOUR_ALLOW_CUSTOM            wxS("AllowCustom")

/** @}
*/

// -----------------------------------------------------------------------

/** @class wxPGProperty

    wxPGProperty is base class for all wxPropertyGrid properties. In
    sections below we cover few related topics.

    @li @ref pgproperty_properties
    @li @ref pgproperty_creating

    @section pgproperty_properties Supplied Ready-to-use Property Classes

    Here is a list and short description of supplied fully-functional
    property classes. They are located in either props.h or advprops.h.

    @li @ref wxArrayStringProperty
    @li @ref wxBoolProperty
    @li @ref wxColourProperty
    @li @ref wxCursorProperty
    @li @ref wxDateProperty
    @li @ref wxDirProperty
    @li @ref wxEditEnumProperty
    @li @ref wxEnumProperty
    @li @ref wxFileProperty
    @li @ref wxFlagsProperty
    @li @ref wxFloatProperty
    @li @ref wxFontProperty
    @li @ref wxImageFileProperty
    @li @ref wxIntProperty
    @li @ref wxLongStringProperty
    @li @ref wxMultiChoiceProperty
    @li @ref wxPropertyCategory
    @li @ref wxStringProperty
    @li @ref wxSystemColourProperty
    @li @ref wxUIntProperty

    @subsection wxPropertyCategory

    Not an actual property per se, but a header for a group of properties.
    Regardless inherits from wxPGProperty.

    @subsection wxStringProperty

    Simple string property. wxPG_STRING_PASSWORD attribute may be used
    to echo value as asterisks and use wxTE_PASSWORD for wxTextCtrl.

    @remarks wxStringProperty has a special trait: if it has value of
            "<composed>", and also has child properties, then its displayed
            value becomes composition of child property values, similar as
            with wxFontProperty, for instance.

    @subsection wxIntProperty

    Like wxStringProperty, but converts text to a signed long integer.
    wxIntProperty seamlessly supports 64-bit integers (ie. wxLongLong).
    To safely convert variant to integer, use code like this:

    @code
        wxLongLong ll;
        ll << property->GetValue();

        // or
        wxLongLong ll = propertyGrid->GetPropertyValueAsLong(property);
    @endcode

    @subsection wxUIntProperty

    Like wxIntProperty, but displays value as unsigned int. To set
    the prefix used globally, manipulate wxPG_UINT_PREFIX string attribute.
    To set the globally used base, manipulate wxPG_UINT_BASE int
    attribute. Regardless of current prefix, understands (hex) values starting
    with both "0x" and "$".
    Like wxIntProperty, wxUIntProperty seamlessly supports 64-bit unsigned
    integers (ie. wxULongLong). Same wxVariant safety rules apply.

    @subsection wxFloatProperty

    Like wxStringProperty, but converts text to a double-precision floating point.
    Default float-to-text precision is 6 decimals, but this can be changed
    by modifying wxPG_FLOAT_PRECISION attribute.

    @subsection wxBoolProperty

    Represents a boolean value. wxChoice is used as editor control, by the
    default. wxPG_BOOL_USE_CHECKBOX attribute can be set to true inorder to use
    check box instead.

    @subsection wxLongStringProperty

    Like wxStringProperty, but has a button that triggers a small text editor
    dialog. Note that in long string values, tabs are represented by "\t" and
    line break by "\n".

    @subsection wxDirProperty

    Like wxLongStringProperty, but the button triggers dir selector instead.
    Supported properties (all with string value): wxPG_DIR_DIALOG_MESSAGE.

    @subsection wxFileProperty

    Like wxLongStringProperty, but the button triggers file selector instead.
    Default wildcard is "All files..." but this can be changed by setting
    wxPG_FILE_WILDCARD attribute (see wxFileDialog for format details).
    Attribute wxPG_FILE_SHOW_FULL_PATH can be set to false inorder to show
    only the filename, not the entire path.

    @subsection wxEnumProperty

    Represents a single selection from a list of choices -
    wxOwnerDrawnComboBox is used to edit the value.

    @subsection wxFlagsProperty

    Represents a bit set that fits in a long integer. wxBoolProperty sub-properties
    are created for editing individual bits. Textctrl is created to manually edit
    the flags as a text; a continous sequence of spaces, commas and semicolons
    is considered as a flag id separator.
    <b>Note: </b> When changing "choices" (ie. flag labels) of wxFlagsProperty, you
    will need to use wxPGProperty::SetChoices() - otherwise they will not get updated
    properly.

    @subsection wxArrayStringProperty

    Allows editing of a list of strings in wxTextCtrl and in a separate dialog.

    @subsection wxDateProperty

    wxDateTime property. Default editor is DatePickerCtrl, altough TextCtrl
    should work as well. wxPG_DATE_FORMAT attribute can be used to change
    string wxDateTime::Format uses (altough default is recommended as it is
    locale-dependant), and wxPG_DATE_PICKER_STYLE allows changing window
    style given to DatePickerCtrl (default is wxDP_DEFAULT|wxDP_SHOWCENTURY).

    @subsection wxEditEnumProperty

    Represents a string that can be freely edited or selected from list of choices -
    custom combobox control is used to edit the value.

    @subsection wxMultiChoiceProperty

    Allows editing a multiple selection from a list of strings. This is
    property is pretty much built around concept of wxMultiChoiceDialog.
    It uses wxArrayString value.

    @subsection wxImageFileProperty

    Like wxFileProperty, but has thumbnail of the image in front of
    the filename and autogenerates wildcard from available image handlers.

    @subsection wxColourProperty

    <b>Useful alternate editor:</b> Choice.

    Represents wxColour. wxButton is used to trigger a colour picker dialog.

    @subsection wxFontProperty

    Represents wxFont. Various sub-properties are used to edit individual
    subvalues.

    @subsection wxSystemColourProperty

    Represents wxColour and a system colour index. wxChoice is used to edit
    the value. Drop-down list has color images. Note that value type
    is wxColourPropertyValue instead of wxColour.
    @code
        class wxColourPropertyValue : public wxObject
        {
        public:
            //  An integer value relating to the colour, and which exact
            //  meaning depends on the property with which it is used.
            //
            //  For wxSystemColourProperty:
            //  Any of wxSYS_COLOUR_XXX, or any web-colour ( use wxPG_TO_WEB_COLOUR
            //  macro - (currently unsupported) ), or wxPG_COLOUR_CUSTOM.
            wxUint32    m_type;

            // Resulting colour. Should be correct regardless of type.
            wxColour    m_colour;
        };
    @endcode

    @subsection wxCursorProperty

    Represents a wxCursor. wxChoice is used to edit the value.
    Drop-down list has cursor images under some (wxMSW) platforms.


    @section pgproperty_creating Creating Custom Properties

    New properties can be created by subclassing wxPGProperty or one
    of the provided property classes, and (re)implementing necessary
    member functions. Below, each virtual member function has ample
    documentation about its purpose and any odd details which to keep
    in mind.

    Here is a very simple 'template' code:

    @code
        class MyProperty : public wxPGProperty
        {
        public:
            // All arguments of ctor must have a default value -
            // use wxPG_LABEL for label and name
            MyProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name = wxPG_LABEL,
                        const wxString& value = wxEmptyString )
            {
                // m_value is wxVariant
                m_value = value;
            }

            virtual ~MyProperty() { }

            const wxPGEditor* DoGetEditorClass() const
            {
                // Determines editor used by property.
                // You can replace 'TextCtrl' below with any of these
                // builtin-in property editor identifiers: Choice, ComboBox,
                // TextCtrlAndButton, ChoiceAndButton, CheckBox, SpinCtrl,
                // DatePickerCtrl.
                return wxPGEditor_TextCtrl;
            }

            virtual wxString GetValueAsString( int argFlags ) const
            {
                // TODO: Return property value in string format
            }

            virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags )
            {
                // TODO: Adapt string to property value.
            }

        protected:
        };
    @endcode

    Since wxPGProperty derives from wxObject, you can use standard
    DECLARE_DYNAMIC_CLASS and IMPLEMENT_DYNAMIC_CLASS macros. From the
    above example they were omitted for sake of simplicity, and besides,
    they are only really needed if you need to use wxRTTI with your
    property class.

    You can change the 'value type' of a property by simply assigning different
    type of variant with SetValue. <b>It is mandatory to implement
    wxVariantData class for all data types used as property values.</b>
    You can use macros declared in wxPropertyGrid headers. For instance:

    @code
        // In header file:
        // (If you need to have export declaration, use version of macros
        // with _EXPORTED postfix)
        WX_PG_DECLARE_VARIANT_DATA(MyDataClass)

        // In sources file:
        WX_PG_IMPLEMENT_VARIANT_DATA(MyDataClass)

        // Or, if you don't have valid == operator:
        WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(MyDataClass)
    @endcode

    @library{wxpropgrid}
    @category{propgrid}
*/
class wxPGProperty : public wxObject
{
public:
    typedef wxUint32 FlagType;

    /**
        Default constructor.
    */
    wxPGProperty();

    /**
        Constructor.
        Non-abstract property classes should have constructor of this style:

        @code

        MyProperty( const wxString& label, const wxString& name, const T& value )
            : wxPGProperty()
        {
            // Generally recommended way to set the initial value
            // (as it should work in pretty much 100% of cases).
            wxVariant variant;
            variant << value;
            SetValue(variant);

            // If has private child properties then create them here. For example:
            //     AddChild( new wxStringProperty( "Subprop 1", wxPG_LABEL, value.GetSubProp1() ) );
        }

        @endcode
    */
    wxPGProperty( const wxString& label, const wxString& name );

    /**
        Virtual destructor. It is customary for derived properties to implement this.
    */
    virtual ~wxPGProperty();

    /**
        This virtual function is called after m_value has been set.

        @remarks
        - If m_value was set to Null variant (ie. unspecified value), OnSetValue()
          will not be called.
        - m_value may be of any variant type. Typically properties internally support only
          one variant type, and as such OnSetValue() provides a good opportunity to convert
          supported values into internal type.
        - Default implementation does nothing.
    */
    virtual void OnSetValue();

    /**
        Override this to return something else than m_value as the value.
    */
    virtual wxVariant DoGetValue() const { return m_value; }

    /**
        Implement this function in derived class to check the value.
        Return true if it is ok. Returning false prevents property change events
        from occurring.

        @remarks
        - Default implementation always returns true.
    */
    virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const;

    /**
        Converts text into wxVariant value appropriate for this property.

        @param variant
            On function entry this is the old value (should not be wxNullVariant
            in normal cases). Translated value must be assigned back to it.

        @param text
            Text to be translated into variant.

        @param argFlags
            If wxPG_FULL_VALUE is set, returns complete, storable value instead
            of displayable one (they may be different).
            If wxPG_COMPOSITE_FRAGMENT is set, text is interpreted as a part of
            composite property string value (as generated by GetValueAsString()
            called with this same flag).

        @return Returns @true if resulting wxVariant value was different.

        @remarks Default implementation converts semicolon delimited tokens into
                child values. Only works for properties with children.

                You might want to take into account that m_value is Null variant
                if property value is unspecified (which is usually only case if
                you explicitly enabled that sort behavior).
    */
    virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags = 0 ) const;

    /**
        Converts integer (possibly a choice selection) into wxVariant value
        appropriate for this property.

        @param variant
            On function entry this is the old value (should not be wxNullVariant
            in normal cases). Translated value must be assigned back to it.

        @param number
            Integer to be translated into variant.

        @param argFlags
            If wxPG_FULL_VALUE is set, returns complete, storable value instead
            of displayable one.

        @return Returns @true if resulting wxVariant value was different.

        @remarks
        - If property is not supposed to use choice or spinctrl or other editor
          with int-based value, it is not necessary to implement this method.
        - Default implementation simply assign given int to m_value.
        - If property uses choice control, and displays a dialog on some choice
          items, then it is preferred to display that dialog in IntToValue
          instead of OnEvent.
        - You might want to take into account that m_value is Null variant if
          property value is unspecified (which is usually only case if you
          explicitly enabled that sort behavior).
    */
    virtual bool IntToValue( wxVariant& value, int number, int argFlags = 0 ) const;

    /**
        Returns text representation of property's value.

        @param argFlags
        If wxPG_FULL_VALUE is set, returns complete, storable string value instead of displayable.
        If wxPG_EDITABLE_VALUE is set, returns string value that must be editable in textctrl.
        If wxPG_COMPOSITE_FRAGMENT is set, returns text that is appropriate to display
        as a part of composite property string value.

        @remarks
        Default implementation returns string composed from text representations of
        child properties.
    */
    virtual wxString GetValueAsString( int argFlags = 0 ) const;

    /**
        Converts string to a value, and if successful, calls SetValue() on it.
        Default behavior is to do nothing.
        @param text
        String to get the value from.
        @retval
        true if value was changed.
    */
    bool SetValueFromString( const wxString& text, int flags = 0 );

    /**
        Converts integer to a value, and if succesful, calls SetValue() on it.
        Default behavior is to do nothing.
        @param value
        Int to get the value from.
        @param flags
        If has wxPG_FULL_VALUE, then the value given is a actual value and not an index.
        @retval
        True if value was changed.
    */
    bool SetValueFromInt( long value, int flags = 0 );

    /**
        Returns size of the custom painted image in front of property. This method
        must be overridden to return non-default value if OnCustomPaint is to be
        called.
        @param item
        Normally -1, but can be an index to the property's list of items.
        @remarks
        - Default behavior is to return wxSize(0,0), which means no image.
        - Default image width or height is indicated with dimension -1.
        - You can also return wxPG_DEFAULT_IMAGE_SIZE which equals wxSize(-1, -1).
    */
    virtual wxSize OnMeasureImage( int item = -1 ) const;

    /**
        Events received by editor widgets are processed here. Note that editor class
        usually processes most events. Some, such as button press events of
        TextCtrlAndButton class, can be handled here. Also, if custom handling
        for regular events is desired, then that can also be done (for example,
        wxSystemColourProperty custom handles wxEVT_COMMAND_CHOICE_SELECTED
        to display colour picker dialog when 'custom' selection is made).

        If the event causes value to be changed, SetValueInEvent()
        should be called to set the new value.

        @param event
        Associated wxEvent.
        @retval
        Should return true if any changes in value should be reported.
        @remarks
        - If property uses choice control, and displays a dialog on some choice items,
          then it is preferred to display that dialog in IntToValue instead of OnEvent.
    */
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxWindow* wnd_primary, wxEvent& event );

    /**
        Called after value of a child property has been altered. Note that this function is
        usually called at the time that value of this property, or given child property, is
        still pending for change.

        Sample pseudo-code implementation:

        @code
        void MyProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
        {
            // Acquire reference to actual type of data stored in variant
            // (TFromVariant only exists if wxPropertyGrid's wxVariant-macros were used to create
            // the variant class).
            T& data = TFromVariant(thisValue);

            // Copy childValue into data.
            switch ( childIndex )
            {
                case 0:
                    data.SetSubProp1( childvalue.GetLong() );
                    break;
                case 1:
                    data.SetSubProp2( childvalue.GetString() );
                    break;
                ...
            }
        }
        @endcode

        @param thisValue
        Value of this property, that should be altered.
        @param childIndex
        Index of child changed (you can use Item(childIndex) to get).
        @param childValue
        Value of the child property.
    */
    virtual void ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const;

    /**
        Returns pointer to an instance of used editor.
    */
    virtual const wxPGEditor* DoGetEditorClass() const;

    /**
        Returns pointer to the wxValidator that should be used
        with the editor of this property (NULL for no validator).
        Setting validator explicitly via SetPropertyValidator
        will override this.

        In most situations, code like this should work well
        (macros are used to maintain one actual validator instance,
        so on the second call the function exits within the first
        macro):

        @code

        wxValidator* wxMyPropertyClass::DoGetValidator () const
        {
            WX_PG_DOGETVALIDATOR_ENTRY()

            wxMyValidator* validator = new wxMyValidator(...);

            ... prepare validator...

            WX_PG_DOGETVALIDATOR_EXIT(validator)
        }

        @endcode

        @remarks
        You can get common filename validator by returning
        wxFileProperty::GetClassValidator(). wxDirProperty,
        for example, uses it.
    */
    virtual wxValidator* DoGetValidator () const;

    /**
        Override to paint an image in front of the property value text or drop-down
        list item (but only if wxPGProperty::OnMeasureImage is overridden as well).

        If property's OnMeasureImage() returns size that has height != 0 but less than
        row height ( < 0 has special meanings), wxPropertyGrid calls this method to
        draw a custom image in a limited area in front of the editor control or
        value text/graphics, and if control has drop-down list, then the image is
        drawn there as well (even in the case OnMeasureImage() returned higher height
        than row height).

        NOTE: Following applies when OnMeasureImage() returns a "flexible" height (
        using wxPG_FLEXIBLE_SIZE(W,H) macro), which implies variable height items:
        If rect.x is < 0, then this is a measure item call, which means that
        dc is invalid and only thing that should be done is to set paintdata.m_drawnHeight
        to the height of the image of item at index paintdata.m_choiceItem. This call
        may be done even as often as once every drop-down popup show.

        @param dc
        wxDC to paint on.
        @param rect
        Box reserved for custom graphics. Includes surrounding rectangle, if any.
        If x is < 0, then this is a measure item call (see above).
        @param paintdata
        wxPGPaintData structure with much useful data about painted item.
        @code
        struct wxPGPaintData
        {
            // wxPropertyGrid.
            const wxPropertyGrid*   m_parent;

            // Normally -1, otherwise index to drop-down list item that has to be drawn.
            int                     m_choiceItem;

            // Set to drawn width in OnCustomPaint (optional).
            int                     m_drawnWidth;

            // In a measure item call, set this to the height of item at m_choiceItem index
            int                     m_drawnHeight;
        };
        @endcode

        @remarks
            - You can actually exceed rect width, but if you do so then paintdata.m_drawnWidth
              must be set to the full width drawn in pixels.
            - Due to technical reasons, rect's height will be default even if custom height
              was reported during measure call.
            - Brush is guaranteed to be default background colour. It has been already used to
              clear the background of area being painted. It can be modified.
            - Pen is guaranteed to be 1-wide 'black' (or whatever is the proper colour) pen for
              drawing framing rectangle. It can be changed as well.

        @see GetValueAsString()
    */
    virtual void OnCustomPaint( wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata );

    /**
        Returns used wxPGCellRenderer instance for given property column (label=0, value=1).

        Default implementation returns editor's renderer for all columns.
    */
    virtual wxPGCellRenderer* GetCellRenderer( int column ) const;

    /**
        Returns which choice is currently selected. Only applies to properties
        which have choices.

        Needs to reimplemented in derived class if property value does not
        map directly to a choice. Integer as index, bool, and string usually do.
    */
    virtual int GetChoiceSelection() const;

    /**
        Refresh values of child properties. Automatically called after value is set.
    */
    virtual void RefreshChildren();

    /**
        Special handling for attributes of this property.

        If returns false, then the attribute will be automatically stored in
        m_attributes.

        Default implementation simply returns false.
    */
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    /**
        Returns value of an attribute.

        Override if custom handling of attributes is needed.

        Default implementation simply return NULL variant.
    */
    virtual wxVariant DoGetAttribute( const wxString& name ) const;

    /**
        Returns instance of a new wxPGEditorDialogAdapter instance, which is
        used when user presses the (optional) button next to the editor control;

        Default implementation returns NULL (ie. no action is generated when
        button is pressed).
    */
    virtual wxPGEditorDialogAdapter* GetEditorDialog() const;

    /**
        Returns wxPGCell of given column, NULL if none. If valid
        object is returned, caller will gain its ownership.
    */
    wxPGCell* AcquireCell( unsigned int column )
    {
        if ( column >= m_cells.size() )
            return NULL;

        wxPGCell* cell = (wxPGCell*) m_cells[column];
        m_cells[column] = NULL;
        return cell;
    }

    /**
        Append a new choice to property's list of choices.

        @param label
            Label for added choice.

        @param value
            Value for new choice. Do not specify if you wish this
            to equal choice index.

        @return
            Index to added choice.
    */
    int AddChoice( const wxString& label, int value = wxPG_INVALID_VALUE );

    /**
        Properties which have private child properties should add them
        with this function, called in their constructor.
    */
    void AddChild( wxPGProperty* property );

    /**
        Adapts list variant into proper value using consecutive
        ChildChanged() calls.
    */
    void AdaptListToValue( wxVariant& list, wxVariant* value ) const;

    /**
        Determines, recursively, if all children are not unspecified.

        @param pendingList
            Assumes members in this wxVariant list as pending
            replacement values.
    */
    bool AreAllChildrenSpecified( wxVariant* pendingList = NULL ) const;

    /**
        Returns true if children of this property are component values (for instance,
        points size, face name, and is_underlined are component values of a font).
    */
    bool AreChildrenComponents() const
    {
        if ( m_flags & (wxPG_PROP_COMPOSED_VALUE|wxPG_PROP_AGGREGATE) )
            return true;

        return false;
    }

    /**
        Removes entry from property's wxPGChoices and editor control (if it is active).

        If selected item is deleted, then the value is set to unspecified.
    */
    void DeleteChoice( int index );

    /** Deletes all child properties. */
    void Empty();

    /** Composes text from values of child properties. */
    void GenerateComposedValue( wxString& text, int argFlags = 0 ) const;

    /**
        Returns property attribute value, null variant if not found.
    */
    wxVariant GetAttribute( const wxString& name ) const;

    /** Returns named attribute, as string, if found. Otherwise defVal is returned.
    */
    wxString GetAttribute( const wxString& name, const wxString& defVal ) const;

    /** Returns named attribute, as long, if found. Otherwise defVal is returned.
    */
    long GetAttributeAsLong( const wxString& name, long defVal ) const;

    /** Returns named attribute, as double, if found. Otherwise defVal is returned.
    */
    double GetAttributeAsDouble( const wxString& name, double defVal ) const;

    /**
        Returns attributes as list wxVariant.
    */
    wxVariant GetAttributesAsList() const;

    /**
        Returns editor used for given column. NULL for no editor.
    */
    const wxPGEditor* GetColumnEditor( int column ) const
    {
        if ( column == 1 )
            return GetEditorClass();

        return NULL;
    }

    /** Returns property's base name (ie. parent's name is not added in any case) */
    const wxString& GetBaseName() const { return m_name; }

    /**
        Returns wxPGCell of given column, NULL if none. wxPGProperty
        will retain ownership of the cell object.
    */
    wxPGCell* GetCell( unsigned int column ) const
    {
        if ( column >= m_cells.size() )
            return NULL;

        return (wxPGCell*) m_cells[column];
    }

    /**
        Returns number of child properties.
    */
    unsigned int GetChildCount() const;

    /**
        Returns height of children, recursively, and
        by taking expanded/collapsed status into account.

        @param lh
            Line height. Pass result of GetGrid()->GetRowHeight() here.

        @param iMax
            Only used (internally) when finding property y-positions.
    */
    int GetChildrenHeight( int lh, int iMax = -1 ) const;

    /**
        Returns read-only reference to property's list of choices.
    */
    const wxPGChoices& GetChoices() const;

    /**
        Returns client data (void*) of a property.
    */
    void* GetClientData() const;

    /** Sets managed client object of a property.
    */
    wxClientData *GetClientObject() const;

    /**
        Returns property's default value. If property's value type is not
        a built-in one, and "DefaultValue" attribute is not defined, then
        this function usually returns Null variant.
    */
    wxVariant GetDefaultValue() const;

    /** Returns property's displayed text.
    */
    wxString GetDisplayedString() const
    {
        return GetValueString(0);
    }

    /**
        Returns wxPGEditor that will be used and created when
        property becomes selected. Returns more accurate value
        than DoGetEditorClass().
    */
    const wxPGEditor* GetEditorClass() const;

    /** Returns property grid where property lies. */
    wxPropertyGrid* GetGrid() const;

    /**
        Returns owner wxPropertyGrid, but only if one is currently on a page
        displaying this property.
    */
    wxPropertyGrid* GetGridIfDisplayed() const;

    /**
        Returns property's help or description text.

        @see SetHelpString()
    */
    const wxString& GetHelpString() const;

    /**
        Returns position in parent's array.
    */
    unsigned int GetIndexInParent() const;

    /** Returns property's label. */
    const wxString& GetLabel() const { return m_label; }

    /**
        Returns last visible child property, recursively.
    */
    const wxPGProperty* GetLastVisibleSubItem() const;

    /**
        Returns highest level non-category, non-root parent. Useful when you
        have nested properties with children.

        @remarks If immediate parent is root or category, this will return the
                property itself.
    */
    wxPGProperty* GetMainParent() const;

    /** Returns maximum allowed length of property's text value.
    */
    int GetMaxLength() const
    {
        return (int) m_maxLen;
    }

    /** Returns property's name with all (non-category, non-root) parents. */
    wxString GetName() const;

    /** Return parent of property */
    wxPGProperty* GetParent() const { return m_parent; }

    /**
        Returns (direct) child property with given name (or NULL if not found).
    */
    wxPGProperty* GetPropertyByName( const wxString& name ) const;

    /** Gets assignable version of property's validator. */
    wxValidator* GetValidator() const;

    /**
        Returns property's value.
    */
    wxVariant GetValue() const
    {
        return DoGetValue();
    }

    /**
        Returns bitmap that appears next to value text. Only returns non-NULL
        bitmap if one was set with SetValueImage().
    */
    wxBitmap* GetValueImage() const;

    /**
        To acquire property's value as string, you should use this
        function (instead of GetValueAsString()), as it may produce
        more accurate value in future versions.

        @see GetValueAsString()
    */
    wxString GetValueString( int argFlags = 0 ) const;

    /**
        Returns value type used by this property.
    */
    wxString GetValueType() const
    {
        return m_value.GetType();
    }

    /**
        Returns coordinate to the top y of the property. Note that the
        position of scrollbars is not taken into account.
    */
    int GetY() const;

    /**
        Returns true if property has even one visible child.
    */
    bool HasVisibleChildren() const;

    /**
        Hides or reveals the property.

        @param hide
            @true for hide, @false for reveal.

        @param flags
            By default changes are applied recursively. Set this parameter wxPG_DONT_RECURSE to prevent this.
    */
    bool Hide( bool hide, int flags = wxPG_RECURSE );

    /**
        Returns index of given child property. wxNOT_FOUND if
        given property is not child of this.
    */
    int Index( const wxPGProperty* p ) const;

    /**
        Inserts a new choice to property's list of choices.

        @param label
            Text for new choice

        @param index
            Insertion position. Use wxNOT_FOUND to append.

        @param value
            Value for new choice. Do not specify if you wish this
            to equal choice index.
    */
    int InsertChoice( const wxString& label, int index, int value = wxPG_INVALID_VALUE );

    /**
        Returns @true if this property is actually a wxPropertyCategory.
    */
    bool IsCategory() const;

    /**
        Returns @true if property is enabled.
    */
    bool IsEnabled() const;

    /**
        Returns @true if property has visible children.
    */
    bool IsExpanded() const;

    /**
        Returns @true if this property is actually a wxRootProperty.
    */
    bool IsRoot() const { return (m_parent == NULL); }

    /**
        Returns @true if candidateParent is some parent of this property.
    */
    bool IsSomeParent( wxPGProperty* candidateParent ) const;

    /**
        Returns true if property has editable wxTextCtrl when selected.

        @remarks Altough disabled properties do not displayed editor, they still
                return @true here as being disabled is considered a temporary
                condition (unlike being read-only or having limited editing enabled).
    */
    bool IsTextEditable() const;

    /**
        Returns @true if property's value is considered unspecified. This
        usually means that value is Null variant.
    */
    bool IsValueUnspecified() const;

    /**
        Returns true if all parents expanded.
    */
    bool IsVisible() const;

    /**
        Returns child property at index i.
    */
    wxPGProperty* Item( size_t i ) const;

    /**
        Updates property value in case there were last minute
        changes. If value was unspecified, it will be set to default.
        Use only for properties that have TextCtrl-based editor.

        @remarks If you have code similar to
                @code
                // Update the value in case of last minute changes
                if ( primary && propgrid->IsEditorsValueModified() )
                     GetEditorClass()->CopyValueFromControl( this, primary );
                @endcode
                in wxPGProperty::OnEvent wxEVT_COMMAND_BUTTON_CLICKED handler,
                then replace it with call to this method.

        @return Returns @true if value changed.
    */
    bool PrepareValueForDialogEditing( wxPropertyGrid* propgrid );

    /**
        If property's editor is active, then update it's value.
    */
    void RefreshEditor();

    /**
        Sets an attribute for this property.

        @param name
            Text identifier of attribute. See @ref propgrid_property_attributes.

        @param value
            Value of attribute.

        @remarks Setting attribute's value to Null variant will simply remove it
                from property's set of attributes.
    */
    void SetAttribute( const wxString& name, wxVariant value );

    /**
        Sets editor for a property.

        @param editor
            For builtin editors, use wxPGEditor_X, where X is builtin editor's
            name (TextCtrl, Choice, etc. see wxPGEditor documentation for full list).

        For custom editors, use pointer you received from wxPropertyGrid::RegisterEditorClass().
    */
    void SetEditor( const wxPGEditor* editor );

    /**
        Sets editor for a property, by editor name.
    */
    void SetEditor( const wxString& editorName );

    /**
        Sets cell information for given column.

        Note that the property takes ownership of given wxPGCell instance.
    */
    void SetCell( int column, wxPGCell* cellObj );

    /**
        Sets new set of choices for property.

        @remarks This operation clears the property value.
    */
    bool SetChoices( wxPGChoices& choices );

    /**
        If property has choices and they are not yet exclusive, new such copy
        of them will be created.
    */
    void SetChoicesExclusive();

    /**
        Sets client data (void*) of a property.

        @remarks This untyped client data has to be deleted manually.
    */
    void SetClientData( void* clientData );

    /** Returns client object of a property.
    */
    void SetClientObject(wxClientData* clientObject);

    /**
        Sets selected choice and changes property value.

        Tries to retain value type, although currently if it is not string,
        then it is forced to integer.
    */
    void SetChoiceSelection( int newValue );

    /**
        Sets property's help string, which is shown, for example, in
        wxPropertyGridManager's description text box.
    */
    void SetHelpString( const wxString& helpString );

    /**
        Sets property's label.

        @remarks Properties under same parent may have same labels. However,
                property names must still remain unique.
    */
    void SetLabel( const wxString& label ) { m_label = label; }

    /**
        Set max length of text in text editor.
    */
    bool SetMaxLength( int maxLen );

    /**
        Sets property's "is it modified?" flag. Affects children recursively.
    */
    void SetModifiedStatus( bool modified );

    /**
        Sets new (base) name for property.
    */
    void SetName( const wxString& newName );

    /** Sets wxValidator for a property */
    void SetValidator( const wxValidator& validator );

    /**
        Call this to set value of the property. Unlike methods in wxPropertyGrid,
        this does not automatically update the display.

        @remarks
            Use wxPropertyGrid::ChangePropertyValue() instead if you need to run through
            validation process and send property change event.

            If you need to change property value in event, based on user input, use
            SetValueInEvent() instead.

        @param pList
            Pointer to list variant that contains child values. Used to indicate
            which children should be marked as modified. Usually you just use NULL.

        @param flags
            Use wxPG_SETVAL_REFRESH_EDITOR to update editor control, if it
            was selected.
    */
    void SetValue( wxVariant value, wxVariant* pList = NULL, int flags = 0 );

    /**
        Set wxBitmap in front of the value. This bitmap may be ignored
        by custom cell renderers.
    */
    void SetValueImage( wxBitmap& bmp );

    /**
        Call this function in OnEvent(), OnButtonClick() etc. to change the
        property value based on user input.

        @remarks This method is const since it doesn't actually modify value, but posts
                given variant as pending value, stored in wxPropertyGrid.
    */
    void SetValueInEvent( wxVariant value ) const;

    /**
        Sets property's value to unspecified (ie. Null variant).
    */
    void SetValueToUnspecified();

    /**
        Call with @false in OnSetValue() to cancel value changes after all
        (ie. cancel @true returned by StringToValue() or IntToValue()).
    */
    void SetWasModified( bool set = true );

    /**
        Updates composed values of parent non-category properties, recursively.
        Returns topmost property updated.
    */
    wxPGProperty* UpdateParentValues();

    /**
        Returns true if containing grid uses wxPG_EX_AUTO_UNSPECIFIED_VALUES.
    */
    bool UsesAutoUnspecified() const;
};


/** @class wxPGChoices

    Helper class for managing choices of wxPropertyGrid properties.
    Each entry can have label, value, bitmap, text colour, and background colour.

    @library{wxpropgrid}
    @category{propgrid}
*/
class WXDLLIMPEXP_PROPGRID wxPGChoices
{
public:
    typedef long ValArrItem;

    /**
        Default constructor.
    */
    wxPGChoices();

    /** Copy constructor. */
    wxPGChoices( const wxPGChoices& a );

    /** Constructor. */
    wxPGChoices( const wxChar** labels, const long* values = NULL );

    /** Constructor. */
    wxPGChoices( const wxArrayString& labels, const wxArrayInt& values = wxArrayInt() );

    /** Constructor. */
    wxPGChoices( wxPGChoicesData* data );

    /** Destructor. */
    ~wxPGChoices();

    /**
        Adds to current. If did not have own copies, creates them now. If was empty,
        identical to set except that creates copies.
    */
    void Add( const wxChar** labels, const ValArrItem* values = NULL );

    /** Version that works with wxArrayString. */
    void Add( const wxArrayString& arr, const ValArrItem* values = NULL );

    /** Version that works with wxArrayString and wxArrayInt. */
    void Add( const wxArrayString& arr, const wxArrayInt& arrint );

    /** Adds single item. */
    wxPGChoiceEntry& Add( const wxString& label, int value = wxPG_INVALID_VALUE );

    /** Adds a single item, with bitmap. */
    wxPGChoiceEntry& Add( const wxString& label, const wxBitmap& bitmap,
                          int value = wxPG_INVALID_VALUE );

    /** Adds a single item with full entry information. */
    wxPGChoiceEntry& Add( const wxPGChoiceEntry& entry )
    {
        return Insert(entry, -1);
    }

    /** Adds single item, sorted. */
    wxPGChoiceEntry& AddAsSorted( const wxString& label, int value = wxPG_INVALID_VALUE );

    /**
        Assigns data from another set of choices.
    */
    void Assign( const wxPGChoices& a )
    {
        AssignData(a.m_data);
    }

    /**
        Assigns data from another set of choices.
    */
    void AssignData( wxPGChoicesData* data );

    /**
        Deletes all items.
    */
    void Clear()
    {
        if ( m_data != wxPGChoicesEmptyData )
            m_data->Clear();
    }

    /**
        Returns labe of item.
    */
    const wxString& GetLabel( size_t ind ) const;

    /**
        Returns number of items.
    */
    size_t GetCount () const;

    /**
        Returns value of item;
    */
    int GetValue( size_t ind ) const;

    /**
        Returns array of values matching the given strings. Unmatching strings
        result in wxPG_INVALID_VALUE entry in array.
    */
    wxArrayInt GetValuesForStrings( const wxArrayString& strings ) const;

    /**
        Returns array of indices matching given strings. Unmatching strings
        are added to 'unmatched', if not NULL.
    */
    wxArrayInt GetIndicesForStrings( const wxArrayString& strings,
                                     wxArrayString* unmatched = NULL ) const;

    /**
        Returns @true if item at given index has a valid value;
    */
    bool HasValue( unsigned int i ) const;

    /**
        Returns index of item with given label.
    */
    int Index( const wxString& label ) const;

    /**
        Returns index of item with given value.
    */
    int Index( int val ) const;

    /**
        Inserts single item.
    */
    wxPGChoiceEntry& Insert( const wxString& label, int index, int value = wxPG_INVALID_VALUE );

    /**
        Inserts a single item with full entry information.
    */
    wxPGChoiceEntry& Insert( const wxPGChoiceEntry& entry, int index );

    /**
        Returns false if this is a constant empty set of choices,
        which should not be modified.
    */
    bool IsOk() const
    {
        return ( m_data != wxPGChoicesEmptyData );
    }

    /**
        Returns item at given index.
    */
    const wxPGChoiceEntry& Item( unsigned int i ) const
    {
        wxASSERT( IsOk() );
        return *m_data->Item(i);
    }

    /**
        Returns item at given index.
    */
    wxPGChoiceEntry& Item( unsigned int i )
    {
        wxASSERT( IsOk() );
        return *m_data->Item(i);
    }

    /**
        Removes count items starting at position nIndex.
    */
    void RemoveAt(size_t nIndex, size_t count = 1);

    /**
        Sets contents from lists of strings and values.
    */
    void Set( const wxChar** labels, const long* values = NULL );

    /**
        Sets contents from lists of strings and values.
    */
    void Set( const wxArrayString& labels, const wxArrayInt& values = wxArrayInt() );

    /**
        Creates exclusive copy of current choices.
    */
    void SetExclusive();

    /**
        Returns array of choice labels.
    */
    wxArrayString GetLabels() const;

    void operator= (const wxPGChoices& a)
    {
        AssignData(a.m_data);
    }

    wxPGChoiceEntry& operator[](unsigned int i)
    {
        return Item(i);
    }

    const wxPGChoiceEntry& operator[](unsigned int i) const
    {
        return Item(i);
    }
};

// -----------------------------------------------------------------------
