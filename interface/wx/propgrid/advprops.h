/////////////////////////////////////////////////////////////////////////////
// Name:        advprops.h
// Purpose:     interfaces of wxPropertyGrid Advanced Properties (font,
//              colour, etc.)
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////




// Web colour is currently unsupported
#define wxPG_COLOUR_WEB_BASE        0x10000


#define wxPG_COLOUR_CUSTOM      0xFFFFFF
#define wxPG_COLOUR_UNSPECIFIED (wxPG_COLOUR_CUSTOM+1)

/** @class wxColourPropertyValue

    Because text, background and other colours tend to differ between
    platforms, wxSystemColourProperty must be able to select between system
    colour and, when necessary, to pick a custom one. wxSystemColourProperty
    value makes this possible.
*/
class wxColourPropertyValue : public wxObject
{
public:
    /** An integer value relating to the colour, and which exact
        meaning depends on the property with which it is used.

        For wxSystemColourProperty:

        Any of wxSYS_COLOUR_XXX, or any web-colour ( use wxPG_TO_WEB_COLOUR
        macro - (currently unsupported) ), or wxPG_COLOUR_CUSTOM.

        For custom colour properties without values array specified:

        index or wxPG_COLOUR_CUSTOM

        For custom colour properties <b>with</b> values array specified:

        m_arrValues[index] or wxPG_COLOUR_CUSTOM
    */
    wxUint32    m_type;

    /** Resulting colour. Should be correct regardless of type. */
    wxColour    m_colour;

    wxColourPropertyValue();
    wxColourPropertyValue( const wxColourPropertyValue& v );
    wxColourPropertyValue( const wxColour& colour );
    wxColourPropertyValue( wxUint32 type );
    wxColourPropertyValue( wxUint32 type, const wxColour& colour );

    virtual ~wxColourPropertyValue();

    void Init( wxUint32 type, const wxColour& colour );

    void operator=(const wxColourPropertyValue& cpv);
};



/** @class wxFontProperty
    @ingroup classes
    Property representing wxFont.
*/
class wxFontProperty : public wxPGProperty
{
public:

    wxFontProperty(const wxString& label = wxPG_LABEL,
                   const wxString& name = wxPG_LABEL,
                   const wxFont& value = wxFont());
    virtual ~wxFontProperty();
    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event );
    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const;
    virtual void RefreshChildren();
};




/** If set, then match from list is searched for a custom colour. */
#define wxPG_PROP_TRANSLATE_CUSTOM      wxPG_PROP_CLASS_SPECIFIC_1


/** @class wxSystemColourProperty
    @ingroup classes
    Has dropdown list of wxWidgets system colours. Value used is
    of wxColourPropertyValue type.
*/
class wxSystemColourProperty : public wxEnumProperty
{
public:

    wxSystemColourProperty( const wxString& label = wxPG_LABEL,
                            const wxString& name = wxPG_LABEL,
                            const wxColourPropertyValue&
                                value = wxColourPropertyValue() );
    virtual ~wxSystemColourProperty();

    virtual void OnSetValue();
    virtual bool IntToValue(wxVariant& variant,
                            int number,
                            int argFlags = 0) const;

    /**
        Override in derived class to customize how colours are printed as
        strings.
    */
    virtual wxString ColourToString( const wxColour& col, int index,
                                     int argFlags = 0 ) const;

    /** Returns index of entry that triggers colour picker dialog
        (default is last).
    */
    virtual int GetCustomColourIndex() const;

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event );
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
    virtual wxSize OnMeasureImage( int item ) const;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata );

    // Helper function to show the colour dialog
    bool QueryColourFromUser( wxVariant& variant ) const;

    /** Default is to use wxSystemSettings::GetColour(index). Override to use
        custom colour tables etc.
    */
    virtual wxColour GetColour( int index ) const;

    wxColourPropertyValue GetVal( const wxVariant* pVariant = NULL ) const;

protected:

    // Special constructors to be used by derived classes.
    wxSystemColourProperty( const wxString& label, const wxString& name,
        const wxChar* const* labels, const long* values, wxPGChoices* choicesCache,
        const wxColourPropertyValue& value );

    wxSystemColourProperty( const wxString& label, const wxString& name,
        const wxChar* const* labels, const long* values, wxPGChoices* choicesCache,
        const wxColour& value );

    void Init( int type, const wxColour& colour );

    // Utility functions for internal use
    virtual wxVariant DoTranslateVal( wxColourPropertyValue& v ) const;
    wxVariant TranslateVal( wxColourPropertyValue& v ) const;
    wxVariant TranslateVal( int type, const wxColour& colour ) const;

    // Translates colour to a int value, return wxNOT_FOUND if no match.
    int ColToInd( const wxColour& colour ) const;
};



class wxColourProperty : public wxSystemColourProperty
{
public:
    wxColourProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxColour& value = *wxWHITE );
    virtual ~wxColourProperty();

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual wxColour GetColour( int index ) const;

protected:
    virtual wxVariant DoTranslateVal( wxColourPropertyValue& v ) const;
};



/** @class wxCursorProperty
    @ingroup classes
    Property representing wxCursor.
*/
class wxCursorProperty : public wxEnumProperty
{
public:
    wxCursorProperty( const wxString& label= wxPG_LABEL,
                      const wxString& name= wxPG_LABEL,
                      int value = 0 );
    virtual ~wxCursorProperty();

    virtual wxSize OnMeasureImage( int item ) const;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata );
};


const wxString& wxPGGetDefaultImageWildcard();

/** @class wxImageFileProperty
    @ingroup classes
    Property representing image file(name).
*/
class wxImageFileProperty : public wxFileProperty
{
public:

    wxImageFileProperty( const wxString& label= wxPG_LABEL,
                         const wxString& name = wxPG_LABEL,
                         const wxString& value = wxEmptyString);
    virtual ~wxImageFileProperty();

    virtual void OnSetValue();

    virtual wxSize OnMeasureImage( int item ) const;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata );

protected:
    wxBitmap*   m_pBitmap; // final thumbnail area
    wxImage*    m_pImage; // intermediate thumbnail area
};



/** @class wxMultiChoiceProperty
    @ingroup classes
    Property that manages a value resulting from wxMultiChoiceDialog. Value is
    array of strings. You can get value as array of choice values/indices by
    calling wxMultiChoiceProperty::GetValueAsArrayInt().

    <b>Supported special attributes:</b>
    - "UserStringMode": If > 0, allow user to manually enter strings that are
      not in the list of choices. If this value is 1, user strings are
      preferably placed in front of valid choices. If value is 2, then those
      strings will placed behind valid choices.
*/
class wxMultiChoiceProperty : public wxPGProperty
{
public:

    wxMultiChoiceProperty( const wxString& label,
                           const wxString& name,
                           const wxArrayString& strings,
                           const wxArrayString& value );
    wxMultiChoiceProperty( const wxString& label,
                           const wxString& name,
                           const wxPGChoices& choices,
                           const wxArrayString& value = wxArrayString() );

    wxMultiChoiceProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayString& value = wxArrayString() );

    virtual ~wxMultiChoiceProperty();

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue(wxVariant& variant,
                               const wxString& text,
                               int argFlags = 0) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event );

    wxArrayInt GetValueAsArrayInt() const;

protected:

    void GenerateValueAsString( wxVariant& value, wxString* target ) const;

    // Returns translation of values into string indices.
    wxArrayInt GetValueAsIndices() const;

    wxArrayString       m_valueAsStrings;  // Value as array of strings

    // Cache displayed text since generating it is relatively complicated.
    wxString            m_display;
};



/** @class wxDateProperty
    @ingroup classes
    Property representing wxDateTime.

    <b>Supported special attributes:</b>
    - "DateFormat": Determines displayed date format.
    - "PickerStyle": Determines window style used with wxDatePickerCtrl.
       Default is wxDP_DEFAULT | wxDP_SHOWCENTURY. Using wxDP_ALLOWNONE
       enables additional support for unspecified property value.
*/
class wxDateProperty : public wxPGProperty
{
public:

    wxDateProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxDateTime& value = wxDateTime() );
    virtual ~wxDateProperty();

    virtual void OnSetValue();
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
    virtual bool StringToValue(wxVariant& variant,
                               const wxString& text,
                               int argFlags = 0) const;

    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );

    void SetFormat( const wxString& format );
    const wxString& GetFormat() const;

    void SetDateValue( const wxDateTime& dt );
    wxDateTime GetDateValue() const;

    long GetDatePickerStyle() const;

protected:
    wxString        m_format;
    long            m_dpStyle;  // DatePicker style

    static wxString ms_defaultDateFormat;
    static wxString DetermineDefaultDateFormat( bool showCentury );
};



class wxPGSpinCtrlEditor : public wxPGTextCtrlEditor
{
public:
    virtual ~wxPGSpinCtrlEditor();

    wxString GetName() const;
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const;
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
        wxWindow* wnd, wxEvent& event ) const;
};


extern wxPGEditor* wxPGEditor_SpinCtrl;
extern wxPGEditor* wxPGEditor_DatePickerCtrl;
