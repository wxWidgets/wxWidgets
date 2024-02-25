/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/sampleprops.h
// Purpose:     wxPropertyGrid Sample Properties Header
// Author:      Jaakko Salli
// Created:     2006-03-05
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_
#define _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_

#include "wx/fontdata.h"

DECLARE_VARIANT_OBJECT(wxFontData)


class wxFontDataProperty : public wxFontProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFontDataProperty)
public:

    wxFontDataProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name = wxPG_LABEL,
                        const wxFontData& value = wxFontData() );
    virtual ~wxFontDataProperty() = default;

    void OnSetValue() override;

    // In order to have different value type in a derived property
    // class, we will override GetValue to return custom variant,
    // instead of changing the base m_value. This allows the methods
    // in base class to function properly.
    virtual wxVariant DoGetValue() const override;

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const override;
    virtual void RefreshChildren() override;

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) override;

    // Value must be stored as variant - otherwise it will be
    // decreffed to oblivion on GetValue().
    wxVariant  m_value_wxFontData;
};

// -----------------------------------------------------------------------

class wxSizeProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxSizeProperty)
public:

    wxSizeProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                    const wxSize& value = wxSize() );
    virtual ~wxSizeProperty() = default;

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const override;
    virtual void RefreshChildren() override;

protected:

    // I stands for internal
    void SetValueI( const wxSize& value )
    {
        m_value = WXVARIANT(value);
    }
};

// -----------------------------------------------------------------------

class wxPointProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxPointProperty)
public:

    wxPointProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                     const wxPoint& value = wxPoint() );
    virtual ~wxPointProperty() = default;

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const override;
    virtual void RefreshChildren() override;

protected:

    // I stands for internal
    void SetValueI( const wxPoint& value )
    {
        m_value = WXVARIANT(value);
    }
};

// -----------------------------------------------------------------------

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_VALIDATOR_WITH_DECL(wxDirsProperty, class wxEMPTY_PARAMETER_VALUE)

// -----------------------------------------------------------------------

WX_PG_DECLARE_VARIANT_DATA(wxArrayDouble)

class wxArrayDoubleProperty : public wxEditorDialogProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxArrayDoubleProperty)
public:

    wxArrayDoubleProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayDouble& value = wxArrayDouble() );

    virtual ~wxArrayDoubleProperty() = default;

    virtual void OnSetValue() override;
#if WXWIN_COMPATIBILITY_3_2
    // To prevent warnings that obsolete methods are hidden by overloads with new signature.
    using wxEditorDialogProperty::ValueToString;
    using wxEditorDialogProperty::StringToValue;
#endif // WXWIN_COMPATIBILITY_3_2
    virtual wxString ValueToString(wxVariant& value,
                                   wxPGPropValFormatFlags flags = wxPGPropValFormatFlags::Null) const override;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                wxPGPropValFormatFlags flags = wxPGPropValFormatFlags::Null ) const override;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value ) override;

    // Generates cache for displayed text
    virtual void GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const;

    wxValidator* DoGetValidator() const override;
    bool ValidateValue(wxVariant& value,
                       wxPGValidationInfo& validationInfo) const override;

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) override;

    wxString        m_display; // Stores cache for displayed text
    int             m_precision; // Used when formatting displayed string.
    wxChar          m_delimiter; // Delimiter between array entries.
};

// -----------------------------------------------------------------------

class MyColourProperty : public wxColourProperty
{
public:
    MyColourProperty(const wxString& label = wxPG_LABEL,
        const wxString& name = wxPG_LABEL,
        const wxColour& value = *wxWHITE);

    virtual ~MyColourProperty() = default;

    virtual wxColour GetColour(int index) const override;

#if WXWIN_COMPATIBILITY_3_2
    // To prevent warning that obsolete method is hidden by overload with new signature.
    using wxColourProperty::ColourToString;
#endif // WXWIN_COMPATIBILITY_3_2
    virtual wxString ColourToString(const wxColour& col, int index,
                                    wxPGPropValFormatFlags flags = wxPGPropValFormatFlags::Null) const override;

    virtual int GetCustomColourIndex() const override;
};

#endif // _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_
