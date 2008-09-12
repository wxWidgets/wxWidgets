/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/sampleprops.h
// Purpose:     wxPropertyGrid Sample Properties Header
// Author:      Jaakko Salli
// Modified by:
// Created:     2006-03-05
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_
#define _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_


WX_PG_DECLARE_WXOBJECT_VARIANT_DATA(wxPGVariantDataFontData, wxFontData, wxEMPTY_PARAMETER_VALUE)


class wxFontDataProperty : public wxFontProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxFontDataProperty)
public:

    wxFontDataProperty( const wxString& label = wxPG_LABEL,
                        const wxString& name = wxPG_LABEL,
                        const wxFontData& value = wxFontData() );
    virtual ~wxFontDataProperty ();

    void OnSetValue();

    // Inorder to have different value type in a derived property
    // class, we will override GetValue to return custom variant,
    // instead of changing the base m_value. This allows the methods
    // in base class to function properly.
    virtual wxVariant DoGetValue() const;

    WX_PG_DECLARE_PARENTAL_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()

protected:
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

    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxSize& value )
    {
        m_value = wxSizeToVariant(value);
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

    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxPoint& value )
    {
        m_value = wxPointToVariant(value);
    }
};

// -----------------------------------------------------------------------

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_VALIDATOR_WITH_DECL(wxDirsProperty, class wxEMPTY_PARAMETER_VALUE)

// -----------------------------------------------------------------------

WX_PG_DECLARE_VARIANT_DATA(wxPGVariantDataArrayDouble, wxArrayDouble, wxEMPTY_PARAMETER_VALUE)

class wxArrayDoubleProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxArrayDoubleProperty)
public:

    wxArrayDoubleProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayDouble& value = wxArrayDouble() );

    virtual ~wxArrayDoubleProperty ();

    virtual void OnSetValue();
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()

    // Generates cache for displayed text
    virtual void GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const;

protected:
    wxString        m_display; // Stores cache for displayed text
    int             m_precision; // Used when formatting displayed string.
    wxChar          m_delimiter; // Delimiter between array entries.
};

// -----------------------------------------------------------------------

#endif // _WX_SAMPLES_PROPGRID_SAMPLEPROPS_H_
