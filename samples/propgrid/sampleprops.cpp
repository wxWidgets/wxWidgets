/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/sampleprops.cpp
// Purpose:     wxPropertyGrid Sample Properties
// Author:      Jaakko Salli
// Created:     2006-03-05
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fontdlg.h"
#include "wx/numformatter.h"

// -----------------------------------------------------------------------


#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"

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

template<> inline wxVariant WXVARIANT(const wxFontData& value)
{
    wxVariant variant;
    variant << value;
    return variant;
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
    m_value_wxFontData = WXVARIANT(fontData);

    // Add extra children.
    AddPrivateChild( new wxColourProperty("Colour", wxPG_LABEL,
                                          fontData.GetColour() ) );
}

void wxFontDataProperty::OnSetValue()
{
    if ( !m_value.IsType("wxFontData") )
    {
        if ( m_value.IsType("wxFont") )
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
            m_value_wxFontData = WXVARIANT(fontData);
        }
        else
        {
            wxFAIL_MSG("Value to wxFontDataProperty must be either wxFontData or wxFont");
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

bool wxFontDataProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxS("wxFontData")), "Function called for incompatible property");

    wxFontData fontData;
    fontData << value;

    fontData.SetInitialFont(fontData.GetChosenFont());

    wxFontDialog dlg(pg->GetPanel(), fontData);
    if ( !m_dlgTitle.empty() )
    {
        dlg.SetTitle(m_dlgTitle);
    }

    if ( dlg.ShowModal() == wxID_OK )
    {
        value = WXVARIANT(dlg.GetFontData());
        return true;
    }
    return false;
}

void wxFontDataProperty::RefreshChildren()
{
    wxFontProperty::RefreshChildren();
    if ( GetChildCount() < 6 ) // Number is count of wxFontProperty's children + 1.
        return;
    wxFontData fontData; fontData << m_value_wxFontData;
    wxVariant variant = WXVARIANT(fontData.GetColour());
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

    wxVariant newVariant = WXVARIANT(fontData);
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
    AddPrivateChild( new wxIntProperty("Width",wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty("Height",wxPG_LABEL,value.y) );
}

void wxSizeProperty::RefreshChildren()
{
    if ( !HasAnyChild() ) return;
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
    AddPrivateChild( new wxIntProperty("X",wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty("Y",wxPG_LABEL,value.y) );
}

void wxPointProperty::RefreshChildren()
{
    if ( !HasAnyChild() ) return;
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

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxDirsProperty, ',', "Browse")

#if wxUSE_VALIDATORS

wxValidator* wxDirsProperty::DoGetValidator() const
{
    return wxFileProperty::GetClassValidator();
}

#endif


bool wxDirsProperty::OnCustomStringEdit( wxWindow* parent, wxString& value )
{
    wxDirDialog dlg(parent,
                    "Select a directory to be added to the list:",
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
    virtual wxString ArrayGet( size_t index ) override;
    virtual size_t ArrayGetCount() override;
    virtual bool ArrayInsert( const wxString& str, int index ) override;
    virtual bool ArraySet( size_t index, const wxString& str ) override;
    virtual void ArrayRemoveAt( int index ) override;
    virtual void ArraySwap( size_t first, size_t second ) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxArrayDoubleEditorDialog);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxArrayDoubleEditorDialog, wxPGArrayEditorDialog);

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
            //wxLogDebug("%f != %f",a[i],b[i]);
            return false;
        }
    }
    return true;
}

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wxArrayDouble)

wxPG_IMPLEMENT_PROPERTY_CLASS(wxArrayDoubleProperty,
                              wxEditorDialogProperty,
                              TextCtrlAndButton)


wxArrayDoubleProperty::wxArrayDoubleProperty (const wxString& label,
                                                        const wxString& name,
                                                        const wxArrayDouble& array )
    : wxEditorDialogProperty(label,name)
    , m_precision(-1)
{
    m_dlgStyle = wxAEDIALOG_STYLE;

    // Need to figure out delimiter needed for this locale
    // (i.e. can't use comma when comma acts as decimal point in float).
    wxChar use_delimiter = ',';

    if ( wxNumberFormatter::GetDecimalSeparator() == use_delimiter )
        use_delimiter = ';';

    m_delimiter = use_delimiter;

    SetValue( WXVARIANT(array) );
}

void wxArrayDoubleProperty::OnSetValue()
{
    // Generate cached display string, to optimize grid drawing
    GenerateValueAsString( m_display, m_precision, true );
}

wxString wxArrayDoubleProperty::ValueToString( wxVariant& value,
                                               wxPGPropValFormatFlags flags ) const
{
    wxString s;

    if ( !!(flags & wxPGPropValFormatFlags::FullValue) )
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
    wxString between = ", ";

    between[0] = m_delimiter;

    target.Empty();

    const wxArrayDouble& value = wxArrayDoubleRefFromVariant(m_value);
    wxNumberFormatter::Style style = wxNumberFormatter::Style_None;
    if (removeZeroes)
        style = wxNumberFormatter::Style_NoTrailingZeroes;

    for ( size_t i=0; i<value.GetCount(); i++ )
    {
        target += wxNumberFormatter::ToString(value[i], prec, style);

        if ( i<(value.GetCount()-1) )
            target += between;
    }
}

bool wxArrayDoubleProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType("wxArrayDouble"), "Function called for incompatible property");

    wxArrayDouble& curValue = wxArrayDoubleRefFromVariant(value);

    // Create editor dialog.
    wxArrayDoubleEditorDialog dlg;
    dlg.SetPrecision(m_precision);
    dlg.Create(pg->GetPanel(), "",
               m_dlgTitle.empty() ? GetLabel() : m_dlgTitle, curValue, m_dlgStyle);
    dlg.Move( pg->GetGoodEditorDialogPosition(this,dlg.GetSize()) );

    // Execute editor dialog
    int res = dlg.ShowModal();
    if ( res == wxID_OK && dlg.IsModified() )
    {
        value = WXVARIANT(dlg.GetArray());
        return true;
    }
    return false;
}

bool wxArrayDoubleProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags ) const
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
                ok = false;
                break;
            }

            new_array.Add(tval);
        }

    WX_PG_TOKENIZER1_END()

    // When invalid token found signal the error
    // by returning pending value of non-wxArrayDouble type.
    if ( !ok )
    {
        variant = 0L;
        return true;
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
    return wxEditorDialogProperty::DoSetAttribute(name, value);
}

wxValidator* wxArrayDoubleProperty::DoGetValidator() const
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    wxTextValidator* validator =
        new wxNumericPropertyValidator(wxNumericPropertyValidator::NumericType::Float);

    // Accept also a delimiter and space character
    validator->AddCharIncludes(m_delimiter);
    validator->AddCharIncludes(" ");

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return nullptr;
#endif
}

bool wxArrayDoubleProperty::ValidateValue(wxVariant& value,
                                 wxPGValidationInfo& validationInfo) const
{
    if (!value.IsType("wxArrayDouble"))
    {
        validationInfo.SetFailureMessage("At least one element is not a valid floating-point number.");
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------
// MyColourProperty
// -----------------------------------------------------------------------

// Test customizing wxColourProperty via subclassing
// * Includes custom colour entry.
// * Includes extra custom entry.
MyColourProperty::MyColourProperty(const wxString& label,
                                   const wxString& name,
                                   const wxColour& value)
    : wxColourProperty(label, name, value)
{
    wxPGChoices colours;
    colours.Add("White");
    colours.Add("Black");
    colours.Add("Red");
    colours.Add("Green");
    colours.Add("Blue");
    colours.Add("Custom");
    colours.Add("None");
    m_choices = colours;
    SetIndex(0);
    wxVariant variant;
    variant << value;
    SetValue(variant);
}

wxColour MyColourProperty::GetColour(int index) const
{
    switch ( index )
    {
    case 0: return *wxWHITE;
    case 1: return *wxBLACK;
    case 2: return *wxRED;
    case 3: return *wxGREEN;
    case 4: return *wxBLUE;
    case 5:
        // Return current colour for the custom entry
        wxColour col;
        if ( GetIndex() == GetCustomColourIndex() )
        {
            if ( m_value.IsNull() )
                return col;
            col << m_value;
            return col;
        }
        return *wxWHITE;
    }
    return wxColour();
}

wxString MyColourProperty::ColourToString(const wxColour& col, int index, wxPGPropValFormatFlags flags) const
{
    if ( index == (int)(m_choices.GetCount() - 1) )
        return wxString();

    return wxColourProperty::ColourToString(col, index, flags);
}

int MyColourProperty::GetCustomColourIndex() const
{
    return m_choices.GetCount() - 2;
}
