/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgrid/advprops.h
// Purpose:     wxPropertyGrid Advanced Properties (font, colour, etc.)
// Author:      Jaakko Salli
// Modified by:
// Created:     2004-09-25
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_ADVPROPS_H_
#define _WX_PROPGRID_ADVPROPS_H_

#include "wx/defs.h"

#if wxUSE_PROPGRID

#include "wx/propgrid/props.h"

// -----------------------------------------------------------------------

//
// Additional Value Type Handlers
//
bool WXDLLIMPEXP_PROPGRID
operator==(const wxArrayInt& array1, const wxArrayInt& array2);

//
// Additional Property Editors
//
#if wxUSE_SPINBTN
WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl,WXDLLIMPEXP_PROPGRID)
#endif

#if wxUSE_DATEPICKCTRL
WX_PG_DECLARE_EDITOR_WITH_DECL(DatePickerCtrl,WXDLLIMPEXP_PROPGRID)
#endif

// -----------------------------------------------------------------------


// Web colour is currently unsupported
constexpr wxUint32 wxPG_COLOUR_WEB_BASE = 0x10000;
//#define wxPG_TO_WEB_COLOUR(A)   ((wxUint32)(A+wxPG_COLOUR_WEB_BASE))

constexpr wxUint32 wxPG_COLOUR_CUSTOM = 0xFFFFFF;
constexpr wxUint32 wxPG_COLOUR_UNSPECIFIED = wxPG_COLOUR_CUSTOM + 1;

// Because text, background and other colours tend to differ between
// platforms, wxSystemColourProperty must be able to select between system
// colour and, when necessary, to pick a custom one. wxSystemColourProperty
// value makes this possible.
class WXDLLIMPEXP_PROPGRID wxColourPropertyValue : public wxObject
{
public:
    // An integer value relating to the colour, and which exact
    // meaning depends on the property with which it is used.
    // For wxSystemColourProperty:
    // Any of wxSYS_COLOUR_XXX, or any web-colour ( use wxPG_TO_WEB_COLOUR
    // macro - (currently unsupported) ), or wxPG_COLOUR_CUSTOM.
    //
    // For custom colour properties without values array specified:
    // index or wxPG_COLOUR_CUSTOM
    // For custom colour properties with values array specified:
    // m_arrValues[index] or wxPG_COLOUR_CUSTOM
    wxUint32    m_type;

    // Resulting colour. Should be correct regardless of type.
    wxColour    m_colour;

    wxColourPropertyValue()
        : wxObject()
        , m_type(0)
    {
    }

    virtual ~wxColourPropertyValue() = default;

    wxColourPropertyValue( const wxColourPropertyValue& v )
        : wxObject()
        , m_type(v.m_type)
        , m_colour(v.m_colour)
    {
    }

    void Init( wxUint32 type, const wxColour& colour )
    {
        m_type = type;
        m_colour = colour;
    }

    wxColourPropertyValue( const wxColour& colour )
        : wxObject()
        , m_type(wxPG_COLOUR_CUSTOM)
        , m_colour(colour)
    {
    }

    wxColourPropertyValue( wxUint32 type )
        : wxObject()
        , m_type(type)
    {
    }

    wxColourPropertyValue( wxUint32 type, const wxColour& colour )
        : wxObject()
    {
        Init( type, colour );
    }

    void operator=(const wxColourPropertyValue& cpv)
    {
        if (this != &cpv)
            Init( cpv.m_type, cpv.m_colour );
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxColourPropertyValue);
};


bool WXDLLIMPEXP_PROPGRID
operator==(const wxColourPropertyValue&, const wxColourPropertyValue&);

DECLARE_VARIANT_OBJECT_EXPORTED(wxColourPropertyValue, WXDLLIMPEXP_PROPGRID)

// -----------------------------------------------------------------------

// Property representing wxFont.
class WXDLLIMPEXP_PROPGRID wxFontProperty : public wxEditorDialogProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFontProperty)
public:

    wxFontProperty(const wxString& label = wxPG_LABEL,
                   const wxString& name = wxPG_LABEL,
                   const wxFont& value = wxFont());
    virtual ~wxFontProperty() = default;
    virtual void OnSetValue() override;
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const override;
    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const override;
    virtual void RefreshChildren() override;

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) override;
};

// -----------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_2
// If set, then match from list is searched for a custom colour.
wxDEPRECATED_BUT_USED_INTERNALLY_MSG("wxPG_PROP_TRANSLATE_CUSTOM is intended for internal use.")
constexpr wxPGPropertyFlags wxPG_PROP_TRANSLATE_CUSTOM = wxPG_PROP_RESERVED_1;
#endif // WXWIN_COMPATIBILITY_3_2

// Has dropdown list of wxWidgets system colours. Value used is
// of wxColourPropertyValue type.
class WXDLLIMPEXP_PROPGRID wxSystemColourProperty : public wxEnumProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxSystemColourProperty)
public:

    wxSystemColourProperty( const wxString& label = wxPG_LABEL,
                            const wxString& name = wxPG_LABEL,
                            const wxColourPropertyValue&
                                value = wxColourPropertyValue() );
    virtual ~wxSystemColourProperty() = default;

    virtual void OnSetValue() override;
    virtual bool IntToValue(wxVariant& variant,
                            int number,
                            int argFlags = 0) const override;

    // Override in derived class to customize how colours are printed as
    // strings.
    virtual wxString ColourToString( const wxColour& col, int index,
                                     int argFlags = 0 ) const;

    // Returns index of entry that triggers colour picker dialog
    // (default is last).
    virtual int GetCustomColourIndex() const;

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const override;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const override;
    virtual bool OnEvent( wxPropertyGrid* propgrid,
                          wxWindow* primary, wxEvent& event ) override;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value ) override;
    virtual wxSize OnMeasureImage( int item ) const override;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata ) override;

    // Helper function to show the colour dialog
    bool QueryColourFromUser( wxVariant& variant ) const;

    // Default is to use wxSystemSettings::GetColour(index). Override to use
    // custom colour tables etc.
    virtual wxColour GetColour( int index ) const;

    wxColourPropertyValue GetVal( const wxVariant* pVariant = nullptr ) const;

protected:

    // Special constructors to be used by derived classes.
    wxSystemColourProperty( const wxString& label, const wxString& name,
        const char* const* labels, const long* values, wxPGChoices* choicesCache,
        const wxColourPropertyValue& value );
    wxSystemColourProperty( const wxString& label, const wxString& name,
        const char* const* labels, const long* values, wxPGChoices* choicesCache,
        const wxColour& value );

    void Init( int type, const wxColour& colour );

    // Utility functions for internal use
    virtual wxVariant DoTranslateVal( wxColourPropertyValue& v ) const;
    wxVariant TranslateVal( wxColourPropertyValue& v ) const
    {
        return DoTranslateVal( v );
    }
    wxVariant TranslateVal( int type, const wxColour& colour ) const
    {
        wxColourPropertyValue v(type, colour);
        return DoTranslateVal( v );
    }

    // Translates colour to a int value, return wxNOT_FOUND if no match.
    int ColToInd( const wxColour& colour ) const;
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_PROPGRID wxColourProperty : public wxSystemColourProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxColourProperty)
public:
    wxColourProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxColour& value = *wxWHITE );
    virtual ~wxColourProperty() = default;

    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const override;
    virtual wxColour GetColour( int index ) const override;

protected:
    virtual wxVariant DoTranslateVal( wxColourPropertyValue& v ) const override;

private:
    void Init( wxColour colour );
};

// -----------------------------------------------------------------------

// Property representing wxCursor.
class WXDLLIMPEXP_PROPGRID wxCursorProperty : public wxEnumProperty
{
    wxDECLARE_DYNAMIC_CLASS(wxCursorProperty);

    wxCursorProperty( const wxString& label= wxPG_LABEL,
                      const wxString& name= wxPG_LABEL,
                      int value = 0 );
    virtual ~wxCursorProperty() = default;

    virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override;
    virtual wxSize OnMeasureImage( int item ) const override;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata ) override;
};

// -----------------------------------------------------------------------

#if wxUSE_IMAGE

WXDLLIMPEXP_PROPGRID const wxString& wxPGGetDefaultImageWildcard();
class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxImage;

// Property representing image file(name).
class WXDLLIMPEXP_PROPGRID wxImageFileProperty : public wxFileProperty
{
    wxDECLARE_DYNAMIC_CLASS(wxImageFileProperty);
public:

    wxImageFileProperty( const wxString& label= wxPG_LABEL,
                         const wxString& name = wxPG_LABEL,
                         const wxString& value = wxString());
    virtual ~wxImageFileProperty() = default;

    virtual void OnSetValue() override;

    virtual wxSize OnMeasureImage( int item ) const override;
    virtual void OnCustomPaint( wxDC& dc,
                                const wxRect& rect, wxPGPaintData& paintdata ) override;

protected:
    void SetImage(const wxImage& img);
    wxImage    m_image; // original thumbnail area

private:
    // Initialize m_image using the current file name.
    void LoadImageFromFile();

    wxBitmap   m_bitmap; // final thumbnail area
};

#endif

#if wxUSE_CHOICEDLG

// Property that manages a value resulting from wxMultiChoiceDialog. Value is
// array of strings. You can get value as array of choice values/indices by
// calling wxMultiChoiceProperty::GetValueAsArrayInt().
class WXDLLIMPEXP_PROPGRID wxMultiChoiceProperty : public wxEditorDialogProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxMultiChoiceProperty)
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

    virtual ~wxMultiChoiceProperty() = default;

    virtual void OnSetValue() override;
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const override;
    virtual bool StringToValue(wxVariant& variant,
                               const wxString& text,
                               int argFlags = 0) const override;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value ) override;

    wxArrayInt GetValueAsArrayInt() const
    {
        return m_choices.GetValuesForStrings(m_value.GetArrayString());
    }

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) override;

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("use function GenerateValueAsString(val) returning wxString")
    void GenerateValueAsString( wxVariant& value, wxString* target ) const
    {
        *target = GenerateValueAsString(value);
    }
#endif // WXWIN_COMPATIBILITY_3_0
    wxString GenerateValueAsString(const wxVariant& value) const;

    // Returns translation of values into string indices.
    wxArrayInt GetValueAsIndices() const;

    // Cache displayed text since generating it is relatively complicated.
    wxString            m_display;
    // How to handle user strings
    int                 m_userStringMode;
};

#endif // wxUSE_CHOICEDLG

// -----------------------------------------------------------------------

#if wxUSE_DATETIME

// Property representing wxDateTime.
class WXDLLIMPEXP_PROPGRID wxDateProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxDateProperty)
public:

    wxDateProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxDateTime& value = wxDateTime() );
    virtual ~wxDateProperty() = default;

    virtual void OnSetValue() override;
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const override;
    virtual bool StringToValue(wxVariant& variant,
                               const wxString& text,
                               int argFlags = 0) const override;

    virtual bool DoSetAttribute( const wxString& name, wxVariant& value ) override;

    void SetFormat( const wxString& format )
    {
        m_format = format;
    }

    const wxString& GetFormat() const
    {
        return m_format;
    }

    void SetDateValue( const wxDateTime& dt )
    {
        //m_valueDateTime = dt;
        m_value = dt;
    }

    wxDateTime GetDateValue() const
    {
        //return m_valueDateTime;
        return m_value;
    }

    long GetDatePickerStyle() const
    {
        return m_dpStyle;
    }

protected:
    wxString        m_format;
    long            m_dpStyle;  // DatePicker style

    static wxString ms_defaultDateFormat;
    static wxString DetermineDefaultDateFormat( bool showCentury );
};

#endif // wxUSE_DATETIME

// -----------------------------------------------------------------------

#if wxUSE_SPINBTN

//
// Implement an editor control that allows using wxSpinCtrl (actually, a
// combination of wxTextCtrl and wxSpinButton) to edit value of wxIntProperty
// and wxFloatProperty (and similar).
//
// Note that new editor classes needs to be registered before use. This can be
// accomplished using wxPGRegisterEditorClass macro, which is used for SpinCtrl
// in wxPropertyGridInterface::RegisterAdditionalEditors (see below).
// Registration can also be performed in a constructor of a property that is
// likely to require the editor in question.
//


#include "wx/spinbutt.h"
#include "wx/propgrid/editors.h"


// NOTE: Regardless that this class inherits from a working editor, it has
//   all necessary methods to work independently. wxTextCtrl stuff is only
//   used for event handling here.
class WXDLLIMPEXP_PROPGRID wxPGSpinCtrlEditor : public wxPGTextCtrlEditor
{
    wxDECLARE_DYNAMIC_CLASS(wxPGSpinCtrlEditor);
public:
    virtual ~wxPGSpinCtrlEditor();

    wxString GetName() const override;
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override;
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
        wxWindow* wnd, wxEvent& event ) const override;

private:
    mutable wxString m_tempString;
};

#endif // wxUSE_SPINBTN

// -----------------------------------------------------------------------

#endif // wxUSE_PROPGRID

#endif // _WX_PROPGRID_ADVPROPS_H_
