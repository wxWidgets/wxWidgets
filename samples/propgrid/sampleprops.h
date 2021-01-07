/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/sampleprops.h
// Purpose:     wxPropertyGrid Sample Properties Header
// Author:      Jaakko Salli
// Modified by:
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
    virtual ~wxFontDataProperty ();

    void OnSetValue() wxOVERRIDE;

    // In order to have different value type in a derived property
    // class, we will override GetValue to return custom variant,
    // instead of changing the base m_value. This allows the methods
    // in base class to function properly.
    virtual wxVariant DoGetValue() const wxOVERRIDE;

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const wxOVERRIDE;
    virtual void RefreshChildren() wxOVERRIDE;

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) wxOVERRIDE;

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
    virtual ~wxSizeProperty();

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const wxOVERRIDE;
    virtual void RefreshChildren() wxOVERRIDE;

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
    virtual ~wxPointProperty();

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const wxOVERRIDE;
    virtual void RefreshChildren() wxOVERRIDE;

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

    virtual ~wxArrayDoubleProperty ();

    virtual void OnSetValue() wxOVERRIDE;
    virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const wxOVERRIDE;
    virtual bool StringToValue( wxVariant& variant,
                                const wxString& text,
                                int argFlags = 0 ) const wxOVERRIDE;
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value ) wxOVERRIDE;

    // Generates cache for displayed text
    virtual void GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const;

    wxValidator* DoGetValidator() const wxOVERRIDE;
    bool ValidateValue(wxVariant& value,
                       wxPGValidationInfo& validationInfo) const wxOVERRIDE;

protected:
    virtual bool DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value) wxOVERRIDE;

    wxString        m_display; // Stores cache for displayed text
    int             m_precision; // Used when formatting displayed string.
    wxChar          m_delimiter; // Delimiter between array entries.
};

// -----------------------------------------------------------------------

#endif // _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_
