/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/sampleprops.cpp
// Purpose:     wxPropertyGrid Sample Properties
// Author:      Jaakko Salli
// Modified by:
// Created:     2006-03-05
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fontdlg.h"
#include "wx/numformatter.h"

// -----------------------------------------------------------------------


#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#ifndef WX_PROPGRID_SAMPLEPROPS_H
    #include "sampleprops.h"
#endif


// -----------------------------------------------------------------------
// wxFontDataProperty
// -----------------------------------------------------------------------

// Dummy comparison required by value type implementation.
bool operator == (const wxFontData&, const wxFontData&)
{
    return false;
}

// Custom version of wxFontProperty that also holds colour in the value.
// Original version by Vladimir Vainer.

IMPLEMENT_VARIANT_OBJECT_SHALLOWCMP(wxFontData)

wxPG_IMPLEMENT_PROPERTY_CLASS(wxFontDataProperty,wxFontProperty,TextCtrlAndButton)

wxFontDataProperty::wxFontDataProperty( const wxString& label, const wxString& name,
   const wxFontData& value ) : wxFontProperty(label,name,value.GetInitialFont())
{
    wxFontData fontData(value);

    // Fix value.
    wxFont font;
    font << m_value;  // Get font data from base object.
    fontData.SetChosenFont(font);
    if ( !fontData.GetColour().IsOk() )
        fontData.SetColour(*wxBLACK);

    // Set initial value - should be done in a simpler way like this
    // (instead of calling SetValue) in derived (wxObject) properties.
    m_value_wxFontData << fontData;

    // Add extra children.
    AddPrivateChild( new wxColourProperty(_("Colour"), wxPG_LABEL,
                                          fontData.GetColour() ) );
}

wxFontDataProperty::~wxFontDataProperty () { }

void wxFontDataProperty::OnSetValue()
{
    if ( m_value.GetType() != "wxFontData" )
    {
        if ( m_value.GetType() == "wxFont" )
        {
            wxFont font;
            font << m_value;
            wxFontData fontData;
            fontData.SetChosenFont(font);
            if ( !m_value_wxFontData.IsNull() )
            {
                wxFontData oldFontData;
                oldFontData << m_value_wxFontData;
                fontData.SetColour(oldFontData.GetColour());
            }
            else
            {
                fontData.SetColour(*wxBLACK);
            }
            wxVariant variant;
            variant << fontData;
            m_value_wxFontData = variant;
        }
        else
        {
            wxFAIL_MSG(wxT("Value to wxFontDataProperty must be either wxFontData or wxFont"));
        }
    }
    else
    {
        // Set m_value to wxFont so that wxFontProperty methods will work
        // correctly.
        m_value_wxFontData = m_value;

        wxFontData fontData;
        fontData << m_value_wxFontData;

        wxFont font = fontData.GetChosenFont();
        if ( !font.IsOk() )
            font = wxFontInfo(10).Family(wxFONTFAMILY_SWISS);

        m_value = WXVARIANT(font);
    }
}

wxVariant wxFontDataProperty::DoGetValue() const
{
    return m_value_wxFontData;
}

// Must re-create font dialog displayer.
bool wxFontDataProperty::OnEvent( wxPropertyGrid* propgrid,
                                  wxWindow* WXUNUSED(primary), wxEvent& event )
{
    if ( propgrid->IsMainButtonEvent(event) )
    {
        wxVariant useValue = propgrid->GetUncommittedPropertyValue();

        wxFontData fontData;
        fontData << useValue;

        fontData.SetInitialFont(fontData.GetChosenFont());

        wxFontDialog dlg(propgrid, fontData);

        if ( dlg.ShowModal() == wxID_OK )
        {
            wxVariant variant;
            variant << dlg.GetFontData();
            SetValueInEvent( variant );
            return true;
        }
    }
    return false;
}

void wxFontDataProperty::RefreshChildren()
{
    wxFontProperty::RefreshChildren();
    if ( GetChildCount() < 6 ) // Number is count of wxFontProperty's children + 1.
        return;
    wxFontData fontData; fontData << m_value_wxFontData;
    wxVariant variant; variant << fontData.GetColour();
    Item(6)->SetValue( variant );
}

wxVariant wxFontDataProperty::ChildChanged( wxVariant& thisValue,
                                            int childIndex,
                                            wxVariant& childValue ) const
{
    wxFontData fontData;
    fontData << thisValue;
    wxColour col;
    wxVariant variant;

    switch ( childIndex )
    {
        case 6:
            col << childValue;
            fontData.SetColour( col );
            break;
        default:
            // Transfer from subset to superset.
            wxFont font = fontData.GetChosenFont();
            variant = WXVARIANT(font);
            variant = wxFontProperty::ChildChanged( variant, childIndex, childValue );
            font << variant;
            fontData.SetChosenFont(font);
    }

    wxVariant newVariant;
    newVariant << fontData;
    return newVariant;
}

// -----------------------------------------------------------------------
// wxSizeProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxSizeProperty,wxPGProperty,TextCtrl)

wxSizeProperty::wxSizeProperty( const wxString& label, const wxString& name,
    const wxSize& value) : wxPGProperty(label,name)
{
    SetValueI(value);
    AddPrivateChild( new wxIntProperty(wxT("Width"),wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty(wxT("Height"),wxPG_LABEL,value.y) );
}

wxSizeProperty::~wxSizeProperty() { }

void wxSizeProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxSize& size = wxSizeRefFromVariant(m_value);
    Item(0)->SetValue( (long)size.x );
    Item(1)->SetValue( (long)size.y );
}

wxVariant wxSizeProperty::ChildChanged( wxVariant& thisValue,
                                        int childIndex,
                                        wxVariant& childValue ) const
{
    wxSize& size = wxSizeRefFromVariant(thisValue);
    int val = childValue.GetLong();
    switch ( childIndex )
    {
        case 0: size.x = val; break;
        case 1: size.y = val; break;
    }
    wxVariant newVariant;
    newVariant << size;
    return newVariant;
}

// -----------------------------------------------------------------------
// wxPointProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPointProperty,wxPGProperty,TextCtrl)

wxPointProperty::wxPointProperty( const wxString& label, const wxString& name,
    const wxPoint& value) : wxPGProperty(label,name)
{
    SetValueI(value);
    AddPrivateChild( new wxIntProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty(wxT("Y"),wxPG_LABEL,value.y) );
}

wxPointProperty::~wxPointProperty() { }

void wxPointProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxPoint& point = wxPointRefFromVariant(m_value);
    Item(0)->SetValue( (long)point.x );
    Item(1)->SetValue( (long)point.y );
}

wxVariant wxPointProperty::ChildChanged( wxVariant& thisValue,
                                         int childIndex,
                                         wxVariant& childValue ) const
{
    wxPoint& point = wxPointRefFromVariant(thisValue);
    int val = childValue.GetLong();
    switch ( childIndex )
    {
        case 0: point.x = val; break;
        case 1: point.y = val; break;
    }
    wxVariant newVariant;
    newVariant << point;
    return newVariant;
}


// -----------------------------------------------------------------------
// Dirs Property
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxDirsProperty, ',',
                                                    "Browse")

#if wxUSE_VALIDATORS

wxValidator* wxDirsProperty::DoGetValidator() const
{
    return wxFileProperty::GetClassValidator();
}

#endif


bool wxDirsProperty::OnCustomStringEdit( wxWindow* parent, wxString& value )
{
    wxDirDialog dlg(parent,
                    _("Select a directory to be added to the list:"),
                    value,
                    0);

    if ( dlg.ShowModal() == wxID_OK )
    {
        value = dlg.GetPath();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// wxArrayDoubleEditorDialog
// -----------------------------------------------------------------------


//
// You can *almost* convert wxArrayDoubleEditorDialog to wxArrayXXXEditorDialog
// by replacing each ArrayDouble with ArrayXXX.
//

class wxArrayDoubleEditorDialog : public wxPGArrayEditorDialog
{
public:
    wxArrayDoubleEditorDialog();

    void Init();

    wxArrayDoubleEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayDouble& array,
                              long style = wxAEDIALOG_STYLE,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& sz = wxDefaultSize );

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayDouble& array,
                long style = wxAEDIALOG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize );

    const wxArrayDouble& GetArray() const { return m_array; }

    // Extra method for this type of array
    void SetPrecision ( int precision )
    {
        m_precision = precision;
    }

protected:
    // Mandatory array of type
    wxArrayDouble   m_array;

    int             m_precision;

    // Mandatory overridden methods
    virtual wxString ArrayGet( size_t index ) wxOVERRIDE;
    virtual size_t ArrayGetCount() wxOVERRIDE;
    virtual bool ArrayInsert( const wxString& str, int index ) wxOVERRIDE;
    virtual bool ArraySet( size_t index, const wxString& str ) wxOVERRIDE;
    virtual void ArrayRemoveAt( int index ) wxOVERRIDE;
    virtual void ArraySwap( size_t first, size_t second ) wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxArrayDoubleEditorDialog);
};

IMPLEMENT_DYNAMIC_CLASS(wxArrayDoubleEditorDialog, wxPGArrayEditorDialog)

//
// Array dialog array access and manipulation
//

wxString wxArrayDoubleEditorDialog::ArrayGet( size_t index )
{
    return wxNumberFormatter::ToString(
        m_array[index], m_precision, wxNumberFormatter::Style_NoTrailingZeroes);
}

size_t wxArrayDoubleEditorDialog::ArrayGetCount()
{
    return m_array.GetCount();
}

bool wxArrayDoubleEditorDialog::ArrayInsert( const wxString& str, int index )
{
    double d;
    if ( !str.ToDouble(&d) )
        return false;

    if (index<0)
        m_array.Add(d);
    else
        m_array.Insert(d,index);
    return true;
}

bool wxArrayDoubleEditorDialog::ArraySet( size_t index, const wxString& str )
{
    double d;
    if ( !str.ToDouble(&d) )
        return false;
    m_array[index] = d;
    return true;
}

void wxArrayDoubleEditorDialog::ArrayRemoveAt( int index )
{
    m_array.RemoveAt(index);
}

void wxArrayDoubleEditorDialog::ArraySwap( size_t first, size_t second )
{
    wxSwap(m_array[first], m_array[second]);
}

//
// Array dialog construction etc.
//

wxArrayDoubleEditorDialog::wxArrayDoubleEditorDialog()
    : wxPGArrayEditorDialog()
{
    Init();
}

void wxArrayDoubleEditorDialog::Init()
{
    wxPGArrayEditorDialog::Init();
    SetPrecision(-1);
}

wxArrayDoubleEditorDialog::wxArrayDoubleEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayDouble& array,
                              long style,
                              const wxPoint& pos,
                              const wxSize& sz )
                              : wxPGArrayEditorDialog()
{
    Init();
    Create(parent,message,caption,array,style,pos,sz);
}

bool wxArrayDoubleEditorDialog::Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayDouble& array,
                long style,
                const wxPoint& pos,
                const wxSize& sz )
{

    m_array = array;

    return wxPGArrayEditorDialog::Create (parent,message,caption,style,pos,sz);
}

// -----------------------------------------------------------------------
// wxArrayDoubleProperty
// -----------------------------------------------------------------------

#include <math.h> // for fabs

// Comparison required by value type implementation.
bool operator == (const wxArrayDouble& a, const wxArrayDouble& b)
{
    if ( a.GetCount() != b.GetCount() )
        return false;

    size_t i;

    for ( i=0; i<a.GetCount(); i++ )
    {
        // Can't do direct equality comparison with floating point numbers.
        if ( fabs(a[i] - b[i]) > 0.0000000001 )
        {
            //wxLogDebug(wxT("%f != %f"),a[i],b[i]);
            return false;
        }
    }
    return true;
}

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wxArrayDouble)

wxPG_IMPLEMENT_PROPERTY_CLASS(wxArrayDoubleProperty,
                              wxPGProperty,
                              TextCtrlAndButton)


wxArrayDoubleProperty::wxArrayDoubleProperty (const wxString& label,
                                                        const wxString& name,
                                                        const wxArrayDouble& array )
    : wxPGProperty(label,name)
{
    m_precision = -1;

    //
    // Need to figure out delimiter needed for this locale
    // (ie. can't use comma when comma acts as decimal point in float).
    wxChar use_delimiter = wxT(',');

    if (wxString::Format(wxT("%.2f"),12.34).Find(use_delimiter) >= 0)
        use_delimiter = wxT(';');

    m_delimiter = use_delimiter;

    SetValue( WXVARIANT(array) );
}

wxArrayDoubleProperty::~wxArrayDoubleProperty () { }

void wxArrayDoubleProperty::OnSetValue()
{
    // Generate cached display string, to optimize grid drawing
    GenerateValueAsString( m_display, m_precision, true );
}

wxString wxArrayDoubleProperty::ValueToString( wxVariant& value,
                                               int argFlags ) const
{
    wxString s;

    if ( argFlags & wxPG_FULL_VALUE )
    {
        GenerateValueAsString(s,-1,false);
    }
    else
    {
        //
        // Display cached string only if value truly matches m_value
        if ( value.GetData() == m_value.GetData() )
            return m_display;
        else
            GenerateValueAsString( s, m_precision, true );
    }

    return s;
}

void wxArrayDoubleProperty::GenerateValueAsString( wxString& target, int prec, bool removeZeroes ) const
{
    wxChar between[3] = wxT(", ");
    size_t i;

    between[0] = m_delimiter;

    target.Empty();

    const wxArrayDouble& value = wxArrayDoubleRefFromVariant(m_value);
    wxNumberFormatter::Style style = wxNumberFormatter::Style_None;
    if (removeZeroes)
        style = wxNumberFormatter::Style_NoTrailingZeroes;

    for ( i=0; i<value.GetCount(); i++ )
    {
        target += wxNumberFormatter::ToString(value[i], prec, style);

        if ( i<(value.GetCount()-1) )
            target += between;
    }
}

bool wxArrayDoubleProperty::OnEvent( wxPropertyGrid* propgrid,
                                     wxWindow* WXUNUSED(primary),
                                     wxEvent& event)
{
    if ( propgrid->IsMainButtonEvent(event) )
    {
        // Update the value in case of last minute changes
        wxVariant useValue = propgrid->GetUncommittedPropertyValue();

        wxArrayDouble& value = wxArrayDoubleRefFromVariant(useValue);

        // Create editor dialog.
        wxArrayDoubleEditorDialog dlg;
        dlg.SetPrecision(m_precision);
        dlg.Create( propgrid, wxEmptyString, m_label, value );
        dlg.Move( propgrid->GetGoodEditorDialogPosition(this,dlg.GetSize()) );

        // Execute editor dialog
        int res = dlg.ShowModal();
        if ( res == wxID_OK && dlg.IsModified() )
        {
            SetValueInEvent( WXVARIANT(dlg.GetArray()) );
            return true;
        }
        return false;
    }
    return false;
}

bool wxArrayDoubleProperty::StringToValue( wxVariant& variant, const wxString& text, int ) const
{
    // Add values to a temporary array so that in case
    // of error we can opt not to use them.
    wxArrayDouble new_array;

    bool ok = true;

    wxChar delimiter = m_delimiter;

    WX_PG_TOKENIZER1_BEGIN(text,delimiter)

        if ( !token.empty() )
        {
            double tval;
            // If token was invalid, exit the loop now
            if ( !token.ToDouble(&tval) )
            {
                wxLogDebug( _("\"%s\" is not a floating-point number."), token.c_str() );
                ok = false;
                break;
            }

            new_array.Add(tval);
        }

    WX_PG_TOKENIZER1_END()

    // When invalid token found don't change anything
    if ( !ok )
    {
        return false;
    }

    if ( !(wxArrayDoubleRefFromVariant(m_value) == new_array) )
    {
        variant = WXVARIANT(new_array);
        return true;
    }

    return false;
}

bool wxArrayDoubleProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_FLOAT_PRECISION )
    {
        m_precision = value.GetLong();
        GenerateValueAsString( m_display, m_precision, true );
        return true;
    }
    return false;
}

wxValidator* wxArrayDoubleProperty::DoGetValidator() const
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    wxTextValidator* validator = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);

    // Accept characters for numeric elements
    wxNumericPropertyValidator numValidator(wxNumericPropertyValidator::Float);
    wxArrayString incChars(numValidator.GetIncludes());
    // Accept also a delimiter and space character
    incChars.Add(m_delimiter);
    incChars.Add(" ");

    validator->SetIncludes(incChars);

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return NULL;
#endif
}
