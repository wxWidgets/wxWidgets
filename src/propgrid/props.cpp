/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/props.cpp
// Purpose:     Basic Property Classes
// Author:      Jaakko Salli
// Created:     2005-05-14
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/dirdlg.h"
    #include "wx/filedlg.h"
    #include "wx/log.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
#endif

#include "wx/numformatter.h"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/private.h"

#include <limits>
#include <utility>
#include <array>

constexpr double wxPG_DBL_MIN = std::numeric_limits<double>::min();
constexpr double wxPG_DBL_MAX = std::numeric_limits<double>::max();
constexpr int wxPG_INT_MIN = std::numeric_limits<int>::min();
constexpr int wxPG_INT_MAX = std::numeric_limits<int>::max();
constexpr long wxPG_LONG_MIN = std::numeric_limits<long>::min();
constexpr long wxPG_LONG_MAX = std::numeric_limits<long>::max();
constexpr long long wxPG_LLONG_MIN = std::numeric_limits<long long>::min();
constexpr long long wxPG_LLONG_MAX = std::numeric_limits<long long>::max();
constexpr unsigned long wxPG_ULONG_MAX = std::numeric_limits<unsigned long>::max();
constexpr unsigned long long wxPG_ULLONG_MAX = std::numeric_limits<unsigned long long>::max();

// -----------------------------------------------------------------------
// wxStringProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxStringProperty,wxPGProperty,TextCtrl)

wxStringProperty::wxStringProperty( const wxString& label,
                                    const wxString& name,
                                    const wxString& value )
    : wxPGProperty(label,name)
{
    SetValue(value);
}

void wxStringProperty::OnSetValue()
{
    if ( !m_value.IsNull() && m_value.GetString() == wxS("<composed>") )
        SetFlag(wxPGPropertyFlags::ComposedValue);

    if ( HasFlag(wxPGPropertyFlags::ComposedValue) )
    {
        wxString s;
        DoGenerateComposedValue(s);
        m_value = s;
    }
}

wxString wxStringProperty::ValueToString( wxVariant& value,
                                          wxPGPropValFormatFlags flags ) const
{
    wxString s = value.GetString();

    if ( HasAnyChild() && HasFlag(wxPGPropertyFlags::ComposedValue) )
    {
        // Value stored in m_value is non-editable, non-full value
        if ( !!(flags & wxPGPropValFormatFlags::FullValue) ||
             !!(flags & wxPGPropValFormatFlags::EditableValue) ||
             s.empty() )
        {
            // Calling this under incorrect conditions will fail
            wxASSERT_MSG( !!(flags & wxPGPropValFormatFlags::ValueIsCurrent),
                          wxS("Sorry, currently default wxPGProperty::ValueToString() ")
                          wxS("implementation only works if value is m_value.") );

            DoGenerateComposedValue(s, flags);
        }

        return s;
    }

    // If string is password and value is for visual purposes,
    // then return asterisks instead the actual string.
    if ( !!(m_flags & wxPGPropertyFlags_Password) && !(flags & (wxPGPropValFormatFlags::FullValue|wxPGPropValFormatFlags::EditableValue)) )
        return wxString(wxS('*'), s.length());

    return s;
}

bool wxStringProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    if ( HasAnyChild() && HasFlag(wxPGPropertyFlags::ComposedValue) )
        return wxPGProperty::StringToValue(variant, text, flags);

    if ( variant != text )
    {
        variant = text;
        return true;
    }

    return false;
}

bool wxStringProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_STRING_PASSWORD )
    {
        ChangeFlag(wxPGPropertyFlags_Password, value.GetBool());
        RecreateEditor();
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}

// -----------------------------------------------------------------------
// wxNumericPropertyValidator
// -----------------------------------------------------------------------

#if wxUSE_VALIDATORS

wxNumericPropertyValidator::
    wxNumericPropertyValidator( NumericType numericType, int base )
    : wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST)
{
    long style = GetStyle();

    wxString allowedChars;

    switch ( base )
    {
        case 2:
            allowedChars += wxS("01");
            break;
        case 8:
            allowedChars += wxS("01234567");
            break;
        case 10:
            style |= wxFILTER_DIGITS;
            break;
        case 16:
            style |= wxFILTER_XDIGITS;
            break;

        default:
            wxLogWarning( _("Unknown base %d. Base 10 will be used."), base );
            style |= wxFILTER_DIGITS;
    }

    if ( numericType == NumericType::Signed )
    {
        allowedChars += wxS("-+");
    }
    else if ( numericType == NumericType::Float )
    {
        allowedChars += wxS("-+eE");

        // Use locale-specific decimal point
        allowedChars.append(wxNumberFormatter::GetDecimalSeparator());
    }

    SetStyle(style);
    SetCharIncludes(allowedChars);
}

bool wxNumericPropertyValidator::Validate(wxWindow* parent)
{
    if ( !wxTextValidator::Validate(parent) )
        return false;

    wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
    if ( !tc )
        return true;

    // Do not allow zero-length string
    return !tc->IsEmpty();
}

#endif // wxUSE_VALIDATORS

// -----------------------------------------------------------------------
// wxNumericProperty
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxNumericProperty, wxPGProperty);

wxNumericProperty::wxNumericProperty(const wxString& label, const wxString& name)
    : wxPGProperty(label, name)
    , m_spinMotion(false)
    , m_spinStep(1L)
    , m_spinWrap(false)
{
}

bool wxNumericProperty::DoSetAttribute(const wxString& name, wxVariant& value)
{
    if ( name == wxPG_ATTR_MIN )
    {
        m_minVal = value;
        return true;
    }
    else if ( name == wxPG_ATTR_MAX )
    {
        m_maxVal = value;
        return true;
    }
    else if ( name == wxPG_ATTR_SPINCTRL_MOTION )
    {
        m_spinMotion = value.GetBool();
        return true;
    }
    else if ( name == wxPG_ATTR_SPINCTRL_STEP )
    {
        m_spinStep = value;
        return true;
    }
    else if ( name == wxPG_ATTR_SPINCTRL_WRAP )
    {
        m_spinWrap = value.GetBool();
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}
namespace {
    // This function by default doesn't modify the value.
    // For argument 'value' of type 'double' there is a specialized function (below).
    template<typename T>
    T GetRoundedValue(const wxPGProperty* prop, T value)
    {
        wxUnusedVar(prop);
        return value;
    }

    // Specialized function for floating-point values
    // takes into account required precision of the numbers
    // to avoid rounding and conversion errors.
    template<>
    double GetRoundedValue(const wxPGProperty* prop, double value)
    {
        // Round value to the required precision.
        wxVariant variant = value;
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        wxString strVal = prop->ValueToStringWithCheck(variant, wxPGPropValFormatFlags::FullValue);
#else
        wxString strVal = prop->ValueToString(variant, wxPGPropValFormatFlags::FullValue);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
        wxNumberFormatter::FromString(strVal, &value);
        return value;
    }
} // namespace

// Common validation code to be called in ValidateValue() implementations.
// Note that 'value' is reference on purpose, so we can write
// back to it when mode is wxPG_PROPERTY_VALIDATION_SATURATE or wxPG_PROPERTY_VALIDATION_WRAP.
#if WXWIN_COMPATIBILITY_3_2
template<typename T>
bool wxNumericProperty::DoNumericValidation(T& value, wxPGValidationInfo* pValidationInfo,
                                            int mode, T defMin, T defMax) const
{
    return DoNumericValidation<T>(value, pValidationInfo,
                                  static_cast<wxPGNumericValidationMode>(mode), defMin, defMax);
}
#endif // WXWIN_COMPATIBILITY_3_2

template<typename T>
bool wxNumericProperty::DoNumericValidation(T& value, wxPGValidationInfo* pValidationInfo,
                                            wxPGNumericValidationMode mode, T defMin, T defMax) const
{
    T min = defMin;
    T max = defMax;
    wxVariant variant;
    bool minOk = false;
    bool maxOk = false;

    // Round current value to the required precision, if applicable
    value = GetRoundedValue<T>(this, value);

    variant = m_minVal;
    if ( !variant.IsNull() )
    {
        minOk = variant.Convert(&min);
    }
    if ( minOk )
    {
        // Round minimal value to the required precision, if applicable
        min = GetRoundedValue<T>(this, min);
    }

    variant = m_maxVal;
    if ( !variant.IsNull() )
    {
        maxOk = variant.Convert(&max);
    }
    if ( maxOk )
    {
        // Round maximal value to the required precision, if applicable
        max = GetRoundedValue<T>(this, max);
    }

    if ( minOk )
    {
        if ( value < min )
        {
            if ( mode == wxPGNumericValidationMode::ErrorMessage )
            {
                wxString msg;
                wxVariant vmin = WXVARIANT(min);
                wxString smin = ValueToString(vmin);
                if ( !maxOk )
                    msg = wxString::Format(_("Value must be %s or higher."), smin);
                else
                {
                    wxVariant vmax = WXVARIANT(max);
                    wxString smax = ValueToString(vmax);
                    msg = wxString::Format(_("Value must be between %s and %s."), smin, smax);
                }
                pValidationInfo->SetFailureMessage(msg);
            }
            else if ( mode == wxPGNumericValidationMode::Saturate )
                value = min;
            else
                value = max - (min - value);
            return false;
        }
    }

    if ( maxOk )
    {
        if ( value > max )
        {
            if ( mode == wxPGNumericValidationMode::ErrorMessage )
            {
                wxString msg;
                wxVariant vmax = WXVARIANT(max);
                wxString smax = ValueToString(vmax);
                if ( !minOk )
                    msg = wxString::Format(_("Value must be %s or less."), smax);
                else
                {
                    wxVariant vmin = WXVARIANT(min);
                    wxString smin = ValueToString(vmin);
                    msg = wxString::Format(_("Value must be between %s and %s."), smin, smax);
                }
                pValidationInfo->SetFailureMessage(msg);
            }
            else if ( mode == wxPGNumericValidationMode::Saturate )
                value = max;
            else
                value = min + (value - max);
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
// wxIntProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxIntProperty,wxNumericProperty,TextCtrl)

wxIntProperty::wxIntProperty( const wxString& label, const wxString& name,
    long value ) : wxNumericProperty(label,name)
{
    SetValue(value);
}

wxIntProperty::wxIntProperty( const wxString& label, const wxString& name,
    const wxLongLong& value ) : wxNumericProperty(label,name)
{
    SetValue(wxVariant(value));
}

wxString wxIntProperty::ValueToString( wxVariant& value,
                                       wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    const wxString valType(value.GetType());
    if ( valType == wxPG_VARIANT_TYPE_LONG )
    {
        return wxString::Format(wxS("%li"),value.GetLong());
    }
    else if ( valType == wxPG_VARIANT_TYPE_LONGLONG )
    {
        wxLongLong ll = value.GetLongLong();
        return ll.ToString();
    }

    return wxString();
}

bool wxIntProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    if ( text.empty() )
    {
        variant.MakeNull();
        return true;
    }

    // We know it is a number, but let's still check
    // the return value.
    if ( text.IsNumber() )
    {
        // Remove leading zeros, so that the number is not interpreted as octal
        // Let's allow one, last zero though
        wxString::const_iterator itFirstNonZero = std::find_if(text.begin(), text.end()-1,
                                                               [](wxUniChar c) { return c != '0' && c != ' '; });
        wxString useText(itFirstNonZero, text.end());

        const wxString variantType(variant.GetType());
        bool isPrevLong = variantType == wxPG_VARIANT_TYPE_LONG;

        wxLongLong_t value64 = 0;

        if ( useText.ToLongLong(&value64, 10) &&
             ( value64 >= wxPG_INT_MAX || value64 <= wxPG_INT_MIN )
           )
        {
            bool doChangeValue = isPrevLong;

            if ( !isPrevLong && variantType == wxPG_VARIANT_TYPE_LONGLONG )
            {
                wxLongLong oldValue = variant.GetLongLong();
                if ( oldValue.GetValue() != value64 )
                    doChangeValue = true;
            }

            if ( doChangeValue )
            {
                wxLongLong ll(value64);
                variant = ll;
                return true;
            }
        }
        long value32;
        if ( useText.ToLong( &value32, 0 ) )
        {
            if ( !isPrevLong || variant != value32 )
            {
                variant = value32;
                return true;
            }
        }
    }
    else if ( !!(flags & wxPGPropValFormatFlags::ReportError) )
    {
    }
    return false;
}

bool wxIntProperty::IntToValue( wxVariant& variant, int value, wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    if ( !variant.IsType(wxPG_VARIANT_TYPE_LONG) || variant != (long)value )
    {
        variant = (long)value;
        return true;
    }
    return false;
}

bool wxIntProperty::DoValidation( const wxNumericProperty* property,
                                  wxLongLong& value,
                                  wxPGValidationInfo* pValidationInfo,
                                  wxPGNumericValidationMode mode )
{
    return property->DoNumericValidation<wxLongLong>(value,
                                           pValidationInfo,
                                           mode, wxLongLong(wxPG_LLONG_MIN), wxLongLong(wxPG_LLONG_MAX));
}

bool wxIntProperty::DoValidation(const wxNumericProperty* property,
                                 long& value,
                                 wxPGValidationInfo* pValidationInfo,
                                 wxPGNumericValidationMode mode)
{
    return property->DoNumericValidation<long>(value, pValidationInfo,
                                     mode, wxPG_LONG_MIN, wxPG_LONG_MAX);
}

bool wxIntProperty::ValidateValue( wxVariant& value,
                                   wxPGValidationInfo& validationInfo ) const
{
    wxLongLong ll = value.GetLongLong();
    return DoValidation(this, ll, &validationInfo,
                        wxPGNumericValidationMode::ErrorMessage);
}

wxValidator* wxIntProperty::GetClassValidator()
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    wxValidator* validator = new wxNumericPropertyValidator(
                                    wxNumericPropertyValidator::NumericType::Signed);

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return nullptr;
#endif
}

wxValidator* wxIntProperty::DoGetValidator() const
{
    return GetClassValidator();
}

wxVariant wxIntProperty::AddSpinStepValue(long stepScale) const
{
    wxPGNumericValidationMode mode = m_spinWrap ? wxPGNumericValidationMode::Wrap
        : wxPGNumericValidationMode::Saturate;
    wxVariant value = GetValue();
    if ( value.GetType() == wxPG_VARIANT_TYPE_LONG )
    {
        long v = value.GetLong();
        long step = m_spinStep.GetLong();
        v += (step * stepScale);
        DoValidation(this, v, nullptr, mode);
        value = v;
    }
    else if ( value.GetType() == wxPG_VARIANT_TYPE_LONGLONG )
    {
        wxLongLong v = value.GetLongLong();
        wxLongLong step = m_spinStep.GetLongLong();
        v += (step * stepScale);
        DoValidation(this, v, nullptr, mode);
        value = v;
    }
    else
    {
        wxFAIL_MSG("Unknown value type");
    }

    return value;
}

// -----------------------------------------------------------------------
// wxUIntProperty
// -----------------------------------------------------------------------

enum
{
    wxPG_UINT_HEX_LOWER,
    wxPG_UINT_HEX_LOWER_PREFIX,
    wxPG_UINT_HEX_LOWER_DOLLAR,
    wxPG_UINT_HEX_UPPER,
    wxPG_UINT_HEX_UPPER_PREFIX,
    wxPG_UINT_HEX_UPPER_DOLLAR,
    wxPG_UINT_DEC,
    wxPG_UINT_OCT,
    wxPG_UINT_TEMPLATE_MAX
};

wxPG_IMPLEMENT_PROPERTY_CLASS(wxUIntProperty,wxNumericProperty,TextCtrl)

void wxUIntProperty::Init()
{
    m_base = wxPG_UINT_DEC;
    m_realBase = 10;
    m_prefix = wxPG_PREFIX_NONE;
}

wxUIntProperty::wxUIntProperty( const wxString& label, const wxString& name,
    unsigned long value ) : wxNumericProperty(label,name)
{
    Init();
    SetValue((long)value);
}

wxUIntProperty::wxUIntProperty( const wxString& label, const wxString& name,
    const wxULongLong& value ) : wxNumericProperty(label,name)
{
    Init();
    SetValue(wxVariant(value));
}

wxString wxUIntProperty::ValueToString(wxVariant& value, wxPGPropValFormatFlags flags) const
{
    static constexpr std::array<const wxStringCharType*, wxPG_UINT_TEMPLATE_MAX> gs_uintTemplates32
    {
        wxS("%lx"), wxS("0x%lx"), wxS("$%lx"),
        wxS("%lX"), wxS("0x%lX"), wxS("$%lX"),
        wxS("%lu"), wxS("%lo")
    };

    // In the edit mode we want to display just the numeric value,
    // without prefixes.
    static constexpr std::array<const wxStringCharType*, wxPG_UINT_TEMPLATE_MAX> gs_uintEditTemplates32
    {
        wxS("%lx"), wxS("%lx"), wxS("%lx"),
        wxS("%lX"), wxS("%lX"), wxS("%lX"),
        wxS("%lu"), wxS("%lo")
    };

    static constexpr std::array<const wxStringCharType*, wxPG_UINT_TEMPLATE_MAX> gs_uintTemplates64
    {
        wxS("%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("0x%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("$%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("0x%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("$%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("u"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("o")
    };

    // In the edit mode we want to display just the numeric value,
    // without prefixes.
    static constexpr std::array<const wxStringCharType*, wxPG_UINT_TEMPLATE_MAX> gs_uintEditTemplates64
    {
        wxS("%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("x"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("X"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("u"),
        wxS("%") wxS(wxLongLongFmtSpec) wxS("o")
    };

    size_t index = m_base + m_prefix;
    if ( index >= wxPG_UINT_TEMPLATE_MAX )
        index = wxPG_UINT_DEC;

    const wxString valType(value.GetType());
    if ( valType == wxPG_VARIANT_TYPE_LONG )
    {
        const wxStringCharType* fmt = !!(flags & wxPGPropValFormatFlags::EditableValue) ?
                                        gs_uintEditTemplates32[index] :
                                        gs_uintTemplates32[index];
        return wxString::Format(fmt, (unsigned long)value.GetLong());
    }
    else if ( valType == wxPG_VARIANT_TYPE_ULONGLONG )
    {
        const wxStringCharType* fmt = !!(flags & wxPGPropValFormatFlags::EditableValue) ?
                                        gs_uintEditTemplates64[index] :
                                        gs_uintTemplates64[index];
        wxULongLong ull = value.GetULongLong();
        return wxString::Format(fmt, ull.GetValue());
    }
    return wxString();
}

bool wxUIntProperty::StringToValue(wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags) const
{
    if ( text.empty() )
    {
        variant.MakeNull();
        return true;
    }

    size_t start = 0;
    if ( text[0] == wxS('$') )
        start++;

    wxString s = text.substr(start, text.length() - start);

    const wxString variantType(variant.GetType());
    bool isPrevLong = variantType == wxPG_VARIANT_TYPE_LONG;

    wxULongLong_t value64 = 0;

    if ( s.ToULongLong(&value64, (unsigned int)m_realBase) )
    {
        if ( value64 >= wxPG_LONG_MAX )
        {
            bool doChangeValue = isPrevLong;

            if ( !isPrevLong && variantType == wxPG_VARIANT_TYPE_ULONGLONG )
            {
                wxULongLong oldValue = variant.GetULongLong();
                if ( oldValue.GetValue() != value64 )
                    doChangeValue = true;
            }

            if ( doChangeValue )
            {
                variant = wxULongLong(value64);
                return true;
            }
        }
    }
    unsigned long value32;
    if ( s.ToULong(&value32, m_realBase) && value32 <= wxPG_LONG_MAX )
    {
        if ( !isPrevLong || variant != (long)value32 )
        {
            variant = (long)value32;
            return true;
        }
    }
    else if ( !!(flags & wxPGPropValFormatFlags::ReportError) )
    {
    }

    return false;
}

bool wxUIntProperty::IntToValue( wxVariant& variant, int number, wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    if ( variant != (long)number )
    {
        variant = (long)number;
        return true;
    }
    return false;
}

bool wxUIntProperty::DoValidation(const wxNumericProperty* property,
                                  wxULongLong& value,
                                  wxPGValidationInfo* pValidationInfo,
                                  wxPGNumericValidationMode mode )
{
    return property->DoNumericValidation<wxULongLong>(value, pValidationInfo,
                                            mode, wxULongLong(0), wxULongLong(wxPG_ULLONG_MAX));
}

bool wxUIntProperty::DoValidation(const wxNumericProperty* property,
                                  long& value,
                                  wxPGValidationInfo* pValidationInfo,
                                  wxPGNumericValidationMode mode)
{
    return property->DoNumericValidation<long>(value, pValidationInfo,
                                     mode, 0, wxPG_ULONG_MAX);
}

bool wxUIntProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const
{
    wxULongLong uul = value.GetULongLong();
    return DoValidation(this, uul, &validationInfo,
                        wxPGNumericValidationMode::ErrorMessage);
}

wxValidator* wxUIntProperty::DoGetValidator() const
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    wxValidator* validator = new wxNumericPropertyValidator(
                                    wxNumericPropertyValidator::NumericType::Unsigned,
                                    m_realBase);

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return nullptr;
#endif
}

bool wxUIntProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_UINT_BASE )
    {
        int val = value.GetLong();

        m_realBase = (wxByte) val;
        if ( m_realBase > 16 )
            m_realBase = 16;

        //
        // Translate logical base to a template array index
        m_base = wxPG_UINT_OCT;
        if ( val == wxPG_BASE_HEX )
            m_base = wxPG_UINT_HEX_UPPER;
        else if ( val == wxPG_BASE_DEC )
            m_base = wxPG_UINT_DEC;
        else if ( val == wxPG_BASE_HEXL )
            m_base = wxPG_UINT_HEX_LOWER_DOLLAR;
        return true;
    }
    else if ( name == wxPG_UINT_PREFIX )
    {
        m_prefix = (wxByte) value.GetLong();
        return true;
    }
    return wxNumericProperty::DoSetAttribute(name, value);
}

wxVariant wxUIntProperty::AddSpinStepValue(long stepScale) const
{
    wxPGNumericValidationMode mode = m_spinWrap ? wxPGNumericValidationMode::Wrap
                          : wxPGNumericValidationMode::Saturate;
    wxVariant value = GetValue();
    if ( value.GetType() == wxPG_VARIANT_TYPE_LONG )
    {
        long v = value.GetLong();
        long step = m_spinStep.GetLong();
        v += (step * stepScale);
        DoValidation(this, v, nullptr, mode);
        value = v;
    }
    else if ( value.GetType() == wxPG_VARIANT_TYPE_ULONGLONG )
    {
        wxULongLong v = value.GetULongLong();
        wxULongLong step = m_spinStep.GetULongLong();
        v += (step * stepScale);
        DoValidation(this, v, nullptr, mode);
        value = v;
    }
    else
    {
        wxFAIL_MSG("Unknown value type");
    }

    return value;
}

// -----------------------------------------------------------------------
// wxFloatProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxFloatProperty,wxNumericProperty,TextCtrl)

wxFloatProperty::wxFloatProperty( const wxString& label,
                                            const wxString& name,
                                            double value )
    : wxNumericProperty(label,name)
{
    m_precision = -1;
    SetValue(value);
}

#if WXWIN_COMPATIBILITY_3_0
// This helper method provides standard way for floating point-using
// properties to convert values to string.
const wxString& wxPropertyGrid::DoubleToString(wxString& target,
                                               double value,
                                               int precision,
                                               bool removeZeroes,
                                               wxString* precTemplate)
{
    if ( precision >= 0 )
    {
        wxString text1;
        if (!precTemplate)
            precTemplate = &text1;

        if ( precTemplate->empty() )
        {
            *precTemplate = wxS("%.");
            *precTemplate << wxString::Format( wxS("%i"), precision );
            *precTemplate << wxS('f');
        }

        target.Printf( *precTemplate, value );
    }
    else
    {
        target.Printf( wxS("%f"), value );
    }

    if ( removeZeroes && precision != 0 && !target.empty() )
    {
        // Remove excess zeros (do not remove this code just yet,
        // since sprintf can't do the same consistently across platforms).
        wxString::const_iterator i = target.end() - 1;
        size_t new_len = target.length() - 1;

        for ( ; i != target.begin(); --i )
        {
            if ( *i != wxS('0') )
                break;
            new_len--;
        }

        wxUniChar cur_char = *i;
        if ( cur_char != wxS('.') && cur_char != wxS(',') )
            new_len++;

        if ( new_len != target.length() )
            target.resize(new_len);
    }

    // Remove sign from zero
    if ( target.length() >= 2 && target[0] == wxS('-') )
    {
        bool isZero = true;
        wxString::const_iterator i = target.begin() + 1;

        for ( ; i != target.end(); ++i )
        {
            if ( *i != wxS('0') && *i != wxS('.') && *i != wxS(',') )
            {
                isZero = false;
                break;
            }
        }

        if ( isZero )
            target.erase(target.begin());
    }

    return target;
}
#endif // WXWIN_COMPATIBILITY_3_0

wxString wxFloatProperty::ValueToString( wxVariant& value,
                                         wxPGPropValFormatFlags flags ) const
{
    wxString text;
    if ( !value.IsNull() )
    {
        text = wxNumberFormatter::ToString(value.GetDouble(), m_precision,
                                           !!(flags & wxPGPropValFormatFlags::FullValue) ? wxNumberFormatter::Style_None
                                                                                  : wxNumberFormatter::Style_NoTrailingZeroes);
    }
    return text;
}

bool wxFloatProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    if ( text.empty() )
    {
        variant.MakeNull();
        return true;
    }

    double value;
    bool res = wxNumberFormatter::FromString(text, &value);
    if ( res )
    {
        if ( variant != value )
        {
            variant = value;
            return true;
        }
    }
    else if ( !!(flags & wxPGPropValFormatFlags::ReportError) )
    {
    }
    return false;
}

bool wxFloatProperty::DoValidation( const wxNumericProperty* property,
                                    double& value,
                                    wxPGValidationInfo* pValidationInfo,
                                    wxPGNumericValidationMode mode )
{
    return property->DoNumericValidation<double>(value, pValidationInfo,
                                       mode, wxPG_DBL_MIN, wxPG_DBL_MAX);
}

bool
wxFloatProperty::ValidateValue( wxVariant& value,
                                wxPGValidationInfo& validationInfo ) const
{
    double fpv = value.GetDouble();
    return DoValidation(this, fpv, &validationInfo,
                        wxPGNumericValidationMode::ErrorMessage);
}

bool wxFloatProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_FLOAT_PRECISION )
    {
        m_precision = value.GetLong();
        return true;
    }
    return wxNumericProperty::DoSetAttribute(name, value);
}

wxValidator*
wxFloatProperty::GetClassValidator()
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    wxValidator* validator = new wxNumericPropertyValidator(
                                    wxNumericPropertyValidator::NumericType::Float);

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return nullptr;
#endif
}

wxValidator* wxFloatProperty::DoGetValidator() const
{
    return GetClassValidator();
}

wxVariant wxFloatProperty::AddSpinStepValue(long stepScale) const
{
    wxPGNumericValidationMode mode = m_spinWrap ? wxPGNumericValidationMode::Wrap
                          : wxPGNumericValidationMode::Saturate;
    wxVariant value = GetValue();
    double v = value.GetDouble();
    double step = m_spinStep.GetDouble();
    v += (step * stepScale);
    DoValidation(this, v, nullptr, mode);
    value = v;

    return value;
}

// -----------------------------------------------------------------------
// wxBoolProperty
// -----------------------------------------------------------------------

// We cannot use standard WX_PG_IMPLEMENT_PROPERTY_CLASS macro, since
// there is a custom GetEditorClass.

wxIMPLEMENT_DYNAMIC_CLASS(wxBoolProperty, wxPGProperty);

const wxPGEditor* wxBoolProperty::DoGetEditorClass() const
{
    // Select correct editor control.
#if wxPG_INCLUDE_CHECKBOX
    if ( !(m_flags & wxPGPropertyFlags_UseCheckBox) )
        return wxPGEditor_Choice;
    return wxPGEditor_CheckBox;
#else
    return wxPGEditor_Choice;
#endif
}

wxBoolProperty::wxBoolProperty( const wxString& label, const wxString& name, bool value ) :
    wxPGProperty(label,name)
{
    m_choices.Assign(wxPGGlobalVars->m_boolChoices);

    SetValue(wxVariant(value));

    m_flags |= wxPGPropertyFlags_UseDCC;
}

wxString wxBoolProperty::ValueToString( wxVariant& value,
                                        wxPGPropValFormatFlags flags ) const
{
    bool boolValue = value.GetBool();

    // As a fragment of composite string value,
    // make it a little more readable.
    if ( !!(flags & wxPGPropValFormatFlags::CompositeFragment) )
    {
        if ( boolValue )
        {
            return m_label;
        }
        else
        {
            if ( !!(flags & wxPGPropValFormatFlags::UneditableCompositeFragment) )
                return wxString();

            wxString notFmt;
            if ( wxPGGlobalVars->m_autoGetTranslation )
                notFmt = _("Not %s");
            else
                notFmt = wxS("Not %s");

            return wxString::Format(notFmt, m_label);
        }
    }

    if ( !(flags & wxPGPropValFormatFlags::FullValue) )
    {
        return wxPGGlobalVars->m_boolChoices[boolValue?1:0].GetText();
    }

    return boolValue? wxS("true"): wxS("false");
}

bool wxBoolProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    bool boolValue = false;
    if ( text.CmpNoCase(wxPGGlobalVars->m_boolChoices[1].GetText()) == 0 ||
         text.CmpNoCase(wxS("true")) == 0 ||
         text.CmpNoCase(m_label) == 0 )
        boolValue = true;

    if ( text.empty() )
    {
        variant.MakeNull();
        return true;
    }

    if ( variant != boolValue )
    {
        variant = wxVariant(boolValue);
        return true;
    }
    return false;
}

bool wxBoolProperty::IntToValue( wxVariant& variant, int value, wxPGPropValFormatFlags ) const
{
    bool boolValue = (bool)value;

    if ( variant != boolValue )
    {
        variant = wxVariant(boolValue);
        return true;
    }
    return false;
}

bool wxBoolProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
#if wxPG_INCLUDE_CHECKBOX
    if ( name == wxPG_BOOL_USE_CHECKBOX )
    {
        ChangeFlag(wxPGPropertyFlags_UseCheckBox, value.GetBool());
        return true;
    }
#endif
    if ( name == wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING )
    {
        ChangeFlag(wxPGPropertyFlags_UseDCC, value.GetBool());
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}

// -----------------------------------------------------------------------
// wxEnumProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxEnumProperty,wxPGProperty,Choice)

wxEnumProperty::wxEnumProperty( const wxString& label, const wxString& name, const wxChar* const* labels,
    const long* values, int value ) : wxPGProperty(label,name)
{
    SetIndex(0);

    if ( labels )
    {
        m_choices.Add(labels,values);

        if ( GetItemCount() )
            SetValue( (long)value );
    }
}

wxEnumProperty::wxEnumProperty( const wxString& label, const wxString& name,
    const char* const* untranslatedLabels,
    const long* values, wxPGChoices* choicesCache, int value )
    : wxPGProperty(label,name)
{
    SetIndex(0);

    wxASSERT( choicesCache );

    if ( choicesCache->IsOk() )
    {
        m_choices.Assign( *choicesCache );
        m_value = wxVariant(0L);
    }
    else
    {
        for ( int i = 0; *untranslatedLabels; untranslatedLabels++ )
        {
            const long val = values ? *values++ : i++;
            m_choices.Add(wxGetTranslation(*untranslatedLabels), val);
        }

        if ( GetItemCount() )
            SetValue( (long)value );
    }
}

wxEnumProperty::wxEnumProperty( const wxString& label, const wxString& name,
    const wxArrayString& labels, const wxArrayInt& values, int value )
    : wxPGProperty(label,name)
{
    SetIndex(0);

    if ( !labels.empty() )
    {
        m_choices.Set(labels, values);

        if ( GetItemCount() )
            SetValue( (long)value );
    }
}

wxEnumProperty::wxEnumProperty( const wxString& label, const wxString& name,
    wxPGChoices& choices, int value )
    : wxPGProperty(label,name)
{
    SetIndex(0);

    m_choices.Assign( choices );

    if ( GetItemCount() )
        SetValue( (long)value );
}

int wxEnumProperty::GetIndexForValue( int value ) const
{
    if ( !m_choices.IsOk() )
        return -1;

    const int intVal = m_choices.Index(value);
    if ( intVal >= 0 )
        return intVal;

    return -1;
}

void wxEnumProperty::OnSetValue()
{
    const wxString valType(m_value.GetType());

    int index = -1;
    if ( valType == wxPG_VARIANT_TYPE_LONG )
    {
        ValueFromInt_(m_value, &index, m_value.GetLong(), wxPGPropValFormatFlags::FullValue);
    }
    else if ( valType == wxPG_VARIANT_TYPE_STRING )
    {
        ValueFromString_(m_value, &index, m_value.GetString(), wxPGPropValFormatFlags::Null);
    }
    else
    {
        wxFAIL_MSG( wxS("Unexpected value type") );
        return;
    }

    m_index = index;
}

bool wxEnumProperty::ValidateValue( wxVariant& value, wxPGValidationInfo& WXUNUSED(validationInfo) ) const
{
    // Make sure string value is in the list,
    // unless property has string as preferred value type
    // To reduce code size, use conversion here as well
    if ( value.IsType(wxPG_VARIANT_TYPE_STRING) )
        return ValueFromString_(value, nullptr, value.GetString(), wxPGPropValFormatFlags::PropertySpecific);

    return true;
}

wxString wxEnumProperty::ValueToString( wxVariant& value,
                                        wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    if ( value.IsType(wxPG_VARIANT_TYPE_STRING) )
        return value.GetString();

    int index = m_choices.Index(value.GetLong());
    if ( index < 0 )
        return wxString();

    return m_choices.GetLabel(index);
}

bool wxEnumProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    return ValueFromString_(variant, nullptr, text, flags);
}

bool wxEnumProperty::IntToValue( wxVariant& variant, int intVal, wxPGPropValFormatFlags flags ) const
{
    return ValueFromInt_(variant, nullptr, intVal, flags);
}

bool wxEnumProperty::ValueFromString_(wxVariant& value, int* pIndex, const wxString& text, wxPGPropValFormatFlags WXUNUSED(flags)) const
{
    int useIndex = -1;
    long useValue = 0;

    for ( unsigned int i=0; i<m_choices.GetCount(); i++ )
    {
        const wxString& entryLabel = m_choices.GetLabel(i);
        if ( text.CmpNoCase(entryLabel) == 0 )
        {
            useIndex = (int)i;
            useValue = m_choices.GetValue(i);
            break;
        }
    }

    if (pIndex)
    {
        *pIndex = useIndex;
    }

    if ( useIndex != GetIndex() )
    {
        if ( useIndex != -1 )
        {
            value = (long)useValue;
            return true;
        }

        value.MakeNull();
    }

    return false;
}

bool wxEnumProperty::ValueFromInt_(wxVariant& value, int* pIndex, int intVal, wxPGPropValFormatFlags flags) const
{
    // If wxPGPropValFormatFlags::FullValue is *not* in flags, then intVal is index from combo box.
    //
    int setAsNextIndex = -2;

    if ( !!(flags & wxPGPropValFormatFlags::FullValue) )
    {
        setAsNextIndex = GetIndexForValue( intVal );
    }
    else
    {
        if ( intVal != GetIndex() )
        {
           setAsNextIndex = intVal;
        }
    }

    if ( setAsNextIndex != -2 )
    {
        if ( !(flags & wxPGPropValFormatFlags::FullValue) )
            intVal = m_choices.GetValue(intVal);

        value = (long)intVal;

        if (pIndex)
        {
            *pIndex = setAsNextIndex;
        }
        return true;
    }

    if (pIndex)
    {
        *pIndex = intVal;
    }
    return false;
}

void wxEnumProperty::SetIndex( int index )
{
    m_index = index;
}

int wxEnumProperty::GetIndex() const
{
    if ( m_value.IsNull() )
        return -1;

    return m_index;
}

// -----------------------------------------------------------------------
// wxEditEnumProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxEditEnumProperty, wxPGProperty, ComboBox)

wxEditEnumProperty::wxEditEnumProperty( const wxString& label, const wxString& name, const wxChar* const* labels,
    const long* values, const wxString& value )
    : wxEnumProperty(label,name,labels,values,0)
{
    SetValue( value );
}

wxEditEnumProperty::wxEditEnumProperty( const wxString& label, const wxString& name, const char* const* labels,
    const long* values, wxPGChoices* choicesCache, const wxString& value )
    : wxEnumProperty(label,name,labels,values,choicesCache,0)
{
    SetValue( value );
}

wxEditEnumProperty::wxEditEnumProperty( const wxString& label, const wxString& name,
    const wxArrayString& labels, const wxArrayInt& values, const wxString& value )
    : wxEnumProperty(label,name,labels,values,0)
{
    SetValue( value );
}

wxEditEnumProperty::wxEditEnumProperty( const wxString& label, const wxString& name,
    wxPGChoices& choices, const wxString& value )
    : wxEnumProperty(label,name,choices,0)
{
    SetValue( value );
}

void wxEditEnumProperty::OnSetValue()
{
    const wxString valType(m_value.GetType());

    int index = -1;
    if ( valType == wxPG_VARIANT_TYPE_LONG )
    {
        ValueFromInt_(m_value, &index, m_value.GetLong(), wxPGPropValFormatFlags::FullValue);
    }
    else if ( valType == wxPG_VARIANT_TYPE_STRING )
    {
        wxString val = m_value.GetString();
        ValueFromString_(m_value, &index, val, wxPGPropValFormatFlags::Null);
        // If text is not any of the choices, store as plain text instead.
        if (index == -1)
        {
            m_value = val;
        }
    }
    else
    {
        wxFAIL_MSG( wxS("Unexpected value type") );
        return;
    }

    SetIndex(index);
}

bool wxEditEnumProperty::StringToValue(wxVariant& variant,
                                       const wxString& text, wxPGPropValFormatFlags flags) const
{
    int index;
    bool res = ValueFromString_(variant, &index, text, flags);
    // If text is not any of the choices, store as plain text instead.
    if (index == -1)
    {
        variant = text;
        res = true;
    }
    return res;
}

bool wxEditEnumProperty::ValidateValue(
                          wxVariant& WXUNUSED(value),
                          wxPGValidationInfo& WXUNUSED(validationInfo)) const
{
    return true;
}

// -----------------------------------------------------------------------
// wxFlagsProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxFlagsProperty, wxPGProperty, TextCtrl)

void wxFlagsProperty::Init(long value)
{
    m_allValueFlags = 0;

    if ( m_choices.IsOk() )
    {
        for ( unsigned int i = 0; i < GetItemCount(); i++ )
        {
            m_allValueFlags |= m_choices.GetValue(i);
        }

        // Relay wxPG_BOOL_USE_CHECKBOX and wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING
        // to child bool property controls.
        bool attrUseCheckBox = !!(m_flags & wxPGPropertyFlags_UseCheckBox);
        bool attrUseDCC = !!(m_flags & wxPGPropertyFlags_UseDCC);
        for ( unsigned int i = 0; i < GetItemCount(); i++ )
        {
            bool child_val = (value & m_choices.GetValue(i)) != 0;

            wxString label = GetLabel(i);
            wxString name = label;
#if wxUSE_INTL
            if ( wxPGGlobalVars->m_autoGetTranslation )
            {
                label = ::wxGetTranslation(label);
            }
#endif

            wxPGProperty* boolProp = new wxBoolProperty(label, name, child_val);
            boolProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, attrUseCheckBox);
            boolProp->SetAttribute(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING, attrUseDCC);
            AddPrivateChild(boolProp);
        }
    }

    m_oldValue = value;
}

wxFlagsProperty::wxFlagsProperty( const wxString& label, const wxString& name,
    const wxChar* const* labels, const long* values, long value ) : wxPGProperty(label,name)
{
    m_flags |= wxPGPropertyFlags_UseDCC; // same default like wxBoolProperty

    if ( labels )
    {
        m_choices.Set(labels,values);

        wxASSERT( GetItemCount() > 0 );
        Init(value);

        SetValue( value );
    }
    else
    {
        m_value = m_oldValue = wxVariant(0L);
        m_allValueFlags = 0;
    }
}

wxFlagsProperty::wxFlagsProperty( const wxString& label, const wxString& name,
        const wxArrayString& labels, const wxArrayInt& values, int value )
    : wxPGProperty(label,name)
{
    m_flags |= wxPGPropertyFlags_UseDCC; // same default like wxBoolProperty

    if ( !labels.empty() )
    {
        m_choices.Set(labels,values);

        wxASSERT( GetItemCount() > 0 );
        Init(value);

        SetValue( (long)value );
    }
    else
    {
        m_value = m_oldValue = wxVariant(0L);
        m_allValueFlags = 0;
    }
}

wxFlagsProperty::wxFlagsProperty( const wxString& label, const wxString& name,
    wxPGChoices& choices, long value )
    : wxPGProperty(label,name)
{
    m_flags |= wxPGPropertyFlags_UseDCC; // same default like wxBoolProperty

    if ( choices.IsOk() )
    {
        m_choices.Assign(choices);

        wxASSERT( GetItemCount() > 0 );
        Init(value);

        SetValue( value );
    }
    else
    {
        m_value = m_oldValue = wxVariant(0L);
        m_allValueFlags = 0;
    }
}

void wxFlagsProperty::OnSetValue()
{
    if ( !m_choices.IsOk() || GetItemCount() == 0 )
    {
        m_value = wxVariant(0L);
    }
    else
    {
        // normalize the value (i.e. remove extra flags)
        long val = m_value.GetLong();
        m_value = val & m_allValueFlags;
    }

    long newFlags = m_value;

    if ( newFlags != m_oldValue )
    {
        // Set child modified states
        for ( unsigned int i = 0; i < GetItemCount(); i++ )
        {
            long flag = m_choices.GetValue(i);

            if ( (newFlags & flag) != (m_oldValue & flag) )
                Item(i)->ChangeFlag(wxPGPropertyFlags::Modified, true );
        }

        m_oldValue = newFlags;
    }
}

wxString wxFlagsProperty::ValueToString( wxVariant& value,
                                         wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    wxString text;

    if ( !m_choices.IsOk() )
        return text;

    long flags = value;

    const wxPGChoices& choices = m_choices;
    for ( unsigned int i = 0; i < GetItemCount(); i++ )
    {
        bool doAdd = ( (flags & choices.GetValue(i)) == choices.GetValue(i) );
        if ( doAdd )
        {
            text += choices.GetLabel(i);
            text += wxS(", ");
        }
    }

    // remove last comma
    if ( text.Len() > 1 )
        text.Truncate ( text.Len() - 2 );

    return text;
}

// Translate string into flag tokens
bool wxFlagsProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags ) const
{
    if ( !m_choices.IsOk() )
        return false;

    long newFlags = 0;

    // semicolons are no longer valid delimiters
    WX_PG_TOKENIZER1_BEGIN(text,wxS(','))

        if ( !token.empty() )
        {
            // Determine which one it is
            long bit = IdToBit( token );

            if ( bit != -1 )
            {
                // Changed?
                newFlags |= bit;
            }
            else
            {
                break;
            }
        }

    WX_PG_TOKENIZER1_END()

    if ( variant != newFlags )
    {
        variant = newFlags;
        return true;
    }

    return false;
}

// Converts string id to a relevant bit.
long wxFlagsProperty::IdToBit( const wxString& id ) const
{
    for ( unsigned int i = 0; i < GetItemCount(); i++ )
    {
        if ( id == GetLabel(i) )
        {
            return m_choices.GetValue(i);
        }
    }
    return -1;
}

void wxFlagsProperty::RefreshChildren()
{
    if ( !m_choices.IsOk() || !HasAnyChild() ) return;

    int flags = m_value.GetLong();

    const wxPGChoices& choices = m_choices;
    for ( unsigned int i = 0; i < GetItemCount(); i++ )
    {
        long flag = choices.GetValue(i);

        long subVal = flags & flag;
        wxPGProperty* p = Item(i);

        if ( subVal != (m_oldValue & flag) )
            p->ChangeFlag(wxPGPropertyFlags::Modified, true );

        p->SetValue( subVal == flag?true:false );
    }

    m_oldValue = flags;
}

wxVariant wxFlagsProperty::ChildChanged( wxVariant& thisValue,
                                         int childIndex,
                                         wxVariant& childValue ) const
{
    long oldValue = thisValue.GetLong();
    long val = childValue.GetLong();
    unsigned long vi = m_choices.GetValue(childIndex);

    if ( val )
        return (long) (oldValue | vi);

    return (long) (oldValue & ~(vi));
}

bool wxFlagsProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_BOOL_USE_CHECKBOX )
    {
        ChangeFlag(wxPGPropertyFlags_UseCheckBox, value.GetBool());

        for ( wxPGProperty* child : m_children )
        {
            child->SetAttribute(name, value);
        }
        return true;
    }
    else if ( name == wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING )
    {
        ChangeFlag(wxPGPropertyFlags_UseDCC, value.GetBool());

        for ( wxPGProperty* child : m_children )
        {
            child->SetAttribute(name, value);
        }
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}

// -----------------------------------------------------------------------
// wxDirProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxDirProperty, wxEditorDialogProperty, TextCtrlAndButton)

wxDirProperty::wxDirProperty( const wxString& label, const wxString& name, const wxString& value )
    : wxEditorDialogProperty(label, name)
{
    m_flags &= ~wxPGPropertyFlags_ActiveButton; // Property button enabled only in not read-only mode.
    SetValue(value);
}

wxString wxDirProperty::ValueToString(wxVariant& value, wxPGPropValFormatFlags WXUNUSED(flags)) const
{
    return value;
}

bool wxDirProperty::StringToValue(wxVariant& variant, const wxString& text, wxPGPropValFormatFlags) const
{
    if ( variant != text )
    {
        variant = text;
        return true;
    }
    return false;
}

wxValidator* wxDirProperty::DoGetValidator() const
{
    return wxFileProperty::GetClassValidator();
}

bool wxDirProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxS("string")), "Function called for incompatible property");

    // Update property value from editor, if necessary
    wxSize dlg_sz;
    wxPoint dlg_pos;

    if ( wxPropertyGrid::IsSmallScreen() )
    {
        dlg_sz = wxDefaultSize;
        dlg_pos = wxDefaultPosition;
    }
    else
    {
        dlg_sz.Set(300, 400);
        dlg_pos = pg->GetGoodEditorDialogPosition(this, dlg_sz);
    }

    wxDirDialog dlg(pg->GetPanel(),
                    m_dlgTitle.empty() ? _("Choose a directory:") : m_dlgTitle,
                    value.GetString(), m_dlgStyle, dlg_pos, dlg_sz);
    if ( dlg.ShowModal() == wxID_OK )
    {
        value = dlg.GetPath();
        return true;
    }
    return false;
}

#if WXWIN_COMPATIBILITY_3_0
bool wxDirProperty::DoSetAttribute(const wxString& name, wxVariant& value)
{
    if ( name == wxPG_DIR_DIALOG_MESSAGE )
    {
        m_dlgTitle = value.GetString();
        return true;
    }
    return wxEditorDialogProperty::DoSetAttribute(name, value);
}
#endif // WXWIN_COMPATIBILITY_3_0

// -----------------------------------------------------------------------
// wxPGDialogAdapter
// -----------------------------------------------------------------------

class WXDLLIMPEXP_PROPGRID wxPGDialogAdapter : public wxPGEditorDialogAdapter
{
public:
    wxPGDialogAdapter() = default;

    virtual ~wxPGDialogAdapter() = default;

    virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) override
    {
        wxEditorDialogProperty* dlgProp = wxDynamicCast(prop, wxEditorDialogProperty);
        wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

        wxVariant val = pg->GetUncommittedPropertyValue();
        if ( dlgProp->DisplayEditorDialog(pg, val) )
        {
            SetValue(val);
            return true;
        }

        return false;
    }
};

// -----------------------------------------------------------------------
// wxDialogProperty
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxEditorDialogProperty, wxPGProperty);

wxEditorDialogProperty::wxEditorDialogProperty(const wxString& label, const wxString& name)
    : wxPGProperty(label, name)
    , m_dlgStyle(0)
{
}

wxPGEditorDialogAdapter* wxEditorDialogProperty::GetEditorDialog() const
{
    return new wxPGDialogAdapter();
}

bool wxEditorDialogProperty::DoSetAttribute(const wxString& name, wxVariant& value)
{
    if ( name == wxPG_DIALOG_TITLE )
    {
        m_dlgTitle = value.GetString();
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}

// -----------------------------------------------------------------------
// wxFileProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxFileProperty,wxEditorDialogProperty,TextCtrlAndButton)

wxFileProperty::wxFileProperty( const wxString& label, const wxString& name,
                                const wxString& value )
    : wxEditorDialogProperty(label, name)
{
    m_flags |= wxPGPropertyFlags_ShowFullFileName;
    m_flags &= ~wxPGPropertyFlags_ActiveButton; // Property button enabled only in not read-only mode.
    m_indFilter = -1;
    m_wildcard = wxALL_FILES;

    SetValue(value);
}

wxValidator* wxFileProperty::GetClassValidator()
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()

    // At least wxPython 2.6.2.1 required that the string argument is given
    static wxString v;
    wxTextValidator* validator = new wxTextValidator(wxFILTER_EXCLUDE_CHAR_LIST,&v);

    validator->SetCharExcludes(wxString("?*|<>\""));

    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return nullptr;
#endif
}

wxValidator* wxFileProperty::DoGetValidator() const
{
    return GetClassValidator();
}

void wxFileProperty::OnSetValue()
{
    const wxString& fnstr = m_value.GetString();

    wxFileName filename = fnstr;

    if ( !filename.HasName() )
    {
        m_value = wxVariant(wxString());
    }

    // Find index for extension.
    if ( m_indFilter < 0 && !fnstr.empty() )
    {
        wxString ext = filename.GetExt();
        int curind = 0;
        size_t pos = 0;
        size_t len = m_wildcard.length();

        pos = m_wildcard.find(wxS("|"), pos);
        while ( pos != wxString::npos && pos < (len-3) )
        {
            size_t ext_begin = pos + 3;

            pos = m_wildcard.find(wxS("|"), ext_begin);
            if ( pos == wxString::npos )
                pos = len;
            wxString found_ext = m_wildcard.substr(ext_begin, pos-ext_begin);

            if ( !found_ext.empty() )
            {
                if ( found_ext[0] == wxS('*') )
                {
                    m_indFilter = curind;
                    break;
                }
                if ( ext.CmpNoCase(found_ext) == 0 )
                {
                    m_indFilter = curind;
                    break;
                }
            }

            if ( pos != len )
                pos = m_wildcard.find(wxS("|"), pos+1);

            curind++;
        }
    }
}

wxFileName wxFileProperty::GetFileName() const
{
    wxFileName filename;

    if ( !m_value.IsNull() )
        filename = m_value.GetString();

    return filename;
}

wxString wxFileProperty::ValueToString( wxVariant& value,
                                        wxPGPropValFormatFlags flags ) const
{
    wxFileName filename = value.GetString();

    if ( !filename.HasName() )
        return wxString();

    wxString fullName = filename.GetFullName();
    if ( fullName.empty() )
        return wxString();

    if ( !!(flags & wxPGPropValFormatFlags::FullValue) )
    {
        return filename.GetFullPath();
    }
    else if ( !!(m_flags & wxPGPropertyFlags::ShowFullFileName) )
    {
        if ( !m_basePath.empty() )
        {
            wxFileName fn2(filename);
            fn2.MakeRelativeTo(m_basePath);
            return fn2.GetFullPath();
        }
        return filename.GetFullPath();
    }

    return filename.GetFullName();
}

bool wxFileProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags flags ) const
{
    wxFileName filename = variant.GetString();

    if ( !!(m_flags & wxPGPropertyFlags::ShowFullFileName) || !!(flags & wxPGPropValFormatFlags::FullValue) )
    {
        if ( filename != text )
        {
            variant = text;
            return true;
        }
    }
    else
    {
        if ( filename.GetFullName() != text )
        {
            wxFileName fn = filename;
            fn.SetFullName(text);
            variant = fn.GetFullPath();
            return true;
        }
    }

    return false;
}

bool wxFileProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_FILE_SHOW_FULL_PATH )
    {
        ChangeFlag(wxPGPropertyFlags_ShowFullFileName, value.GetBool());
        return true;
    }
    else if ( name == wxPG_FILE_WILDCARD )
    {
        m_wildcard = value.GetString();
        return true;
    }
    else if ( name == wxPG_FILE_SHOW_RELATIVE_PATH )
    {
        m_basePath = value.GetString();

        // Make sure wxPG_FILE_SHOW_FULL_PATH is also set
        m_flags |= wxPGPropertyFlags_ShowFullFileName;
        return true;
    }
    else if ( name == wxPG_FILE_INITIAL_PATH )
    {
        m_initialPath = value.GetString();
        return true;
    }
#if WXWIN_COMPATIBILITY_3_0
    else if ( name == wxPG_FILE_DIALOG_TITLE )
    {
        m_dlgTitle = value.GetString();
        return true;
    }
#endif // WXWIN_COMPATIBILITY_3_0
    else if ( name == wxPG_FILE_DIALOG_STYLE )
    {
        m_dlgStyle = value.GetLong();
        return true;
    }
    return wxEditorDialogProperty::DoSetAttribute(name, value);
}

bool wxFileProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxS("string")), "Function called for incompatible property");

    wxFileName filename(value.GetString());
    wxString path = filename.GetPath();
    wxString file = filename.GetFullName();

    if ( path.empty() && !m_basePath.empty() )
        path = m_basePath;

    wxFileDialog dlg(pg->GetPanel(),
        m_dlgTitle.empty() ? _("Choose a file") : m_dlgTitle,
        m_initialPath.empty() ? path : m_initialPath,
        file,
        m_wildcard.empty() ? _(wxALL_FILES) : m_wildcard,
        m_dlgStyle,
        wxDefaultPosition);

    if ( m_indFilter >= 0 )
        dlg.SetFilterIndex(m_indFilter);

    if ( dlg.ShowModal() == wxID_OK )
    {
        m_indFilter = dlg.GetFilterIndex();
        value = dlg.GetPath();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// wxLongStringProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxLongStringProperty,wxEditorDialogProperty,TextCtrlAndButton)

wxLongStringProperty::wxLongStringProperty( const wxString& label, const wxString& name,
                                            const wxString& value )
    : wxEditorDialogProperty(label, name)
{
    m_flags |= wxPGPropertyFlags_ActiveButton; // Property button always enabled.
    m_dlgStyle = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN;
    SetValue(value);
}

wxString wxLongStringProperty::ValueToString( wxVariant& value,
                                              wxPGPropValFormatFlags WXUNUSED(flags) ) const
{
    return value;
}

bool wxLongStringProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxS("string")), "Function called for incompatible property");

    // launch editor dialog
    wxDialog* dlg = new wxDialog(pg->GetPanel(), wxID_ANY,
                                 m_dlgTitle.empty() ? GetLabel() : m_dlgTitle,
                                 wxDefaultPosition, wxDefaultSize, m_dlgStyle);

    dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

    // Multi-line text editor dialog.
    const int spacing = wxPropertyGrid::IsSmallScreen()? 4 : 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    long edStyle = wxTE_MULTILINE;
    if ( HasFlag(wxPGPropertyFlags::ReadOnly) )
        edStyle |= wxTE_READONLY;
    wxString strVal;
    wxPropertyGrid::ExpandEscapeSequences(strVal, value.GetString());
    wxTextCtrl* ed = new wxTextCtrl(dlg,wxID_ANY,strVal,
        wxDefaultPosition,wxDefaultSize,edStyle);
    if ( m_maxLen > 0 )
        ed->SetMaxLength(m_maxLen);

    rowsizer->Add(ed, wxSizerFlags(1).Expand().Border(wxALL, spacing));
    topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

    long btnSizerFlags = wxCANCEL;
    if ( !HasFlag(wxPGPropertyFlags::ReadOnly) )
        btnSizerFlags |= wxOK;
    wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(btnSizerFlags);
    topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM|wxRIGHT, spacing));

    dlg->SetSizer( topsizer );
    topsizer->SetSizeHints( dlg );

    if ( !wxPropertyGrid::IsSmallScreen())
    {
        dlg->SetSize(400,300);
        dlg->Move( pg->GetGoodEditorDialogPosition(this,dlg->GetSize()) );
    }

    int res = dlg->ShowModal();

    if ( res == wxID_OK )
    {
        strVal = ed->GetValue();
        wxString strValEscaped;
        wxPropertyGrid::CreateEscapeSequences(strValEscaped, strVal);
        value = strValEscaped;
        dlg->Destroy();
        return true;
    }
    dlg->Destroy();
    return false;
}

bool wxLongStringProperty::StringToValue( wxVariant& variant, const wxString& text, wxPGPropValFormatFlags ) const
{
    if ( variant != text )
    {
        variant = text;
        return true;
    }
    return false;
}

#if wxUSE_EDITABLELISTBOX

// -----------------------------------------------------------------------
// wxPGArrayEditorDialog
// -----------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxPGArrayEditorDialog, wxDialog)
    EVT_IDLE(wxPGArrayEditorDialog::OnIdle)
wxEND_EVENT_TABLE()

wxIMPLEMENT_ABSTRACT_CLASS(wxPGArrayEditorDialog, wxDialog);

#include "wx/editlbox.h"
#include "wx/listctrl.h"

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnIdle(wxIdleEvent& event)
{
    // Repair focus - wxEditableListBox has bitmap buttons, which
    // get focus, and lose focus (into the oblivion) when they
    // become disabled due to change in control state.

    wxWindow* lastFocused = m_lastFocused;
    wxWindow* focus = ::wxWindow::FindFocus();

    // If last focused control became disabled, set focus back to
    // wxEditableListBox
    if ( lastFocused && focus != lastFocused &&
         lastFocused->GetParent() == m_elbSubPanel &&
         !lastFocused->IsEnabled() )
    {
        m_elb->GetListCtrl()->SetFocus();
    }

    m_lastFocused = focus;

    event.Skip();
}

// -----------------------------------------------------------------------

wxPGArrayEditorDialog::wxPGArrayEditorDialog()
    : wxDialog()
{
    Init();
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::Init()
{
    m_elb = nullptr;
    m_elbSubPanel = nullptr;
    m_lastFocused = nullptr;
    m_hasCustomNewAction = false;
    m_itemPendingAtIndex = -1;
    m_modified = false;
}

// -----------------------------------------------------------------------

bool wxPGArrayEditorDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  long style,
                                  const wxPoint& pos,
                                  const wxSize& sz )
{
    // On wxMAC the dialog shows incorrectly if style is not exactly wxCAPTION
    // FIXME: This should be only a temporary fix.
#ifdef __WXMAC__
    wxUnusedVar(style);
    int useStyle = wxCAPTION;
#else
    int useStyle = style;
#endif

    bool res = wxDialog::Create(parent, wxID_ANY, caption, pos, sz, useStyle);

    SetFont(parent->GetFont()); // To allow entering chars of the same set as the propGrid

    const int spacing = wxPropertyGrid::IsSmallScreen()? 3: 4;

    m_modified = false;

    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

    // Message
    if ( !message.empty() )
        topsizer->Add( new wxStaticText(this, wxID_ANY, message),
            wxSizerFlags(0).Left().Border(wxALL, spacing) );

    m_elb = new wxEditableListBox(this, wxID_ANY, message,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxEL_ALLOW_NEW |
                                  wxEL_ALLOW_EDIT |
                                  wxEL_ALLOW_DELETE);

    // Set custom text for "New" button, if provided
    if ( !m_customNewButtonText.empty() )
    {
        m_elb->GetNewButton()->SetToolTip(m_customNewButtonText);
    }

    // Populate the list box
    wxArrayString arr;
    for ( size_t i=0; i<ArrayGetCount(); i++ )
        arr.push_back(ArrayGet(i));
    m_elb->SetStrings(arr);

    m_elbSubPanel = m_elb->GetNewButton()->GetParent();

    // Connect event handlers
    wxListCtrl* lc = m_elb->GetListCtrl();

    m_elb->GetNewButton()->Bind(wxEVT_BUTTON,
                                &wxPGArrayEditorDialog::OnAddClick, this);
    m_elb->GetDelButton()->Bind(wxEVT_BUTTON,
                                &wxPGArrayEditorDialog::OnDeleteClick, this);
    m_elb->GetUpButton()->Bind(wxEVT_BUTTON,
                               &wxPGArrayEditorDialog::OnUpClick, this);
    m_elb->GetDownButton()->Bind(wxEVT_BUTTON,
                                 &wxPGArrayEditorDialog::OnDownClick, this);

    lc->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT,
             &wxPGArrayEditorDialog::OnBeginLabelEdit, this);

    lc->Bind(wxEVT_LIST_END_LABEL_EDIT,
             &wxPGArrayEditorDialog::OnEndLabelEdit, this);

    topsizer->Add(m_elb, wxSizerFlags(1).Expand().Border(0, spacing));

    // Standard dialog buttons
    wxStdDialogButtonSizer* buttonSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxALL, spacing));

    m_elb->SetFocus();

    SetSizer( topsizer );
    topsizer->SetSizeHints( this );

    if ( !wxPropertyGrid::IsSmallScreen() )
    {
        if ( sz.x == wxDefaultSize.x &&
             sz.y == wxDefaultSize.y )
            SetSize(275, 360);
        else
            SetSize(sz);
    }

    return res;
}

// -----------------------------------------------------------------------

int wxPGArrayEditorDialog::GetSelection() const
{
    wxListCtrl* lc = m_elb->GetListCtrl();

    int index = lc->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( index == -1 )
        return wxNOT_FOUND;

    return index;
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnAddClick(wxCommandEvent& event)
{
    wxListCtrl* lc = m_elb->GetListCtrl();
    int newItemIndex = lc->GetItemCount() - 1;

    if ( m_hasCustomNewAction )
    {
        wxString str;
        if ( OnCustomNewAction(&str) )
        {
            if ( ArrayInsert(str, newItemIndex) )
            {
                lc->InsertItem(newItemIndex, str);
                m_modified = true;
            }
        }

        // Do *not* skip the event! We do not want the wxEditableListBox
        // to do anything.
    }
    else
    {
        event.Skip();
    }
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnDeleteClick(wxCommandEvent& event)
{
    int index = GetSelection();
    if ( index >= 0 )
    {
        ArrayRemoveAt( index );
        m_modified = true;
    }

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnUpClick(wxCommandEvent& event)
{
    int index = GetSelection();
    if ( index > 0 )
    {
        ArraySwap(index-1,index);
        m_modified = true;
    }

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnDownClick(wxCommandEvent& event)
{
    wxListCtrl* lc = m_elb->GetListCtrl();
    int index = GetSelection();
    int lastStringIndex = lc->GetItemCount() - 1;
    if ( index >= 0 && index < lastStringIndex )
    {
        ArraySwap(index, index+1);
        m_modified = true;
    }

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPGArrayEditorDialog::OnEndLabelEdit(wxListEvent& event)
{
    wxString str = event.GetLabel();

    if ( m_itemPendingAtIndex >= 0 )
    {
        // Add a new item
        if ( ArrayInsert(str, m_itemPendingAtIndex) )
        {
            m_modified = true;
        }
        else
        {
            // Editable list box doesn't really respect Veto(), but
            // it recognizes if no text was added, so we simulate
            // Veto() using it.
            event.m_item.SetText(wxString());
            m_elb->GetListCtrl()->SetItemText(m_itemPendingAtIndex,
                                              wxString());

            event.Veto();
        }
    }
    else
    {
        // Change an existing item
        long index = event.GetIndex();
        if ( ArraySet(index, str) )
            m_modified = true;
        else
            event.Veto();
    }

    event.Skip();
}

void wxPGArrayEditorDialog::OnBeginLabelEdit(wxListEvent& evt)
{
    wxListCtrl* lc = m_elb->GetListCtrl();
    const int lastStringIndex = lc->GetItemCount() - 1;
    const int curItemIndex = evt.GetIndex();
    // If current index is >= then last available index
    // then we have a new pending element.
    m_itemPendingAtIndex  = curItemIndex < lastStringIndex? -1: curItemIndex;

    evt.Skip();
}

#endif // wxUSE_EDITABLELISTBOX

// -----------------------------------------------------------------------
// wxPGArrayStringEditorDialog
// -----------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPGArrayStringEditorDialog, wxPGArrayEditorDialog);

wxBEGIN_EVENT_TABLE(wxPGArrayStringEditorDialog, wxPGArrayEditorDialog)
wxEND_EVENT_TABLE()

// -----------------------------------------------------------------------

wxString wxPGArrayStringEditorDialog::ArrayGet( size_t index )
{
    return m_array[index];
}

size_t wxPGArrayStringEditorDialog::ArrayGetCount()
{
    return m_array.size();
}

bool wxPGArrayStringEditorDialog::ArrayInsert( const wxString& str, int index )
{
    if (index<0)
        m_array.Add(str);
    else
        m_array.Insert(str,index);
    return true;
}

bool wxPGArrayStringEditorDialog::ArraySet( size_t index, const wxString& str )
{
    wxCHECK_MSG(index < m_array.size(), false, "Index out of range");
    m_array[index] = str;
    return true;
}

void wxPGArrayStringEditorDialog::ArrayRemoveAt( int index )
{
    m_array.RemoveAt(index);
}

void wxPGArrayStringEditorDialog::ArraySwap( size_t first, size_t second )
{
    std::swap(m_array[first], m_array[second]);
}

wxPGArrayStringEditorDialog::wxPGArrayStringEditorDialog()
    : wxPGArrayEditorDialog()
{
    Init();
}

void wxPGArrayStringEditorDialog::Init()
{
    m_pCallingClass = nullptr;
}

bool
wxPGArrayStringEditorDialog::OnCustomNewAction(wxString* resString)
{
    return m_pCallingClass->OnCustomStringEdit(m_parent, *resString);
}

// -----------------------------------------------------------------------
// wxArrayStringProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxArrayStringProperty, wxEditorDialogProperty, TextCtrlAndButton)

wxArrayStringProperty::wxArrayStringProperty( const wxString& label,
                                                        const wxString& name,
                                                        const wxArrayString& array )
    : wxEditorDialogProperty(label,name)
    , m_delimiter(',')
{
    m_dlgStyle = wxAEDIALOG_STYLE;
    SetValue( array );
}

void wxArrayStringProperty::OnSetValue()
{
    GenerateValueAsString();
}

wxString wxArrayStringProperty::ConvertArrayToString(const wxArrayString& arr,
                                                     const wxUniChar& delimiter) const
{
    int flag;
    if ( delimiter == '"' || delimiter == '\'' )
    {
        // Quoted strings
        flag = Escape | QuoteStrings;
    }
    else
    {
        // Regular delimiter
        flag = 0;
    }

    return ArrayStringToString(arr, delimiter, flag);
}

wxString wxArrayStringProperty::ValueToString( wxVariant& WXUNUSED(value),
                                               wxPGPropValFormatFlags flags ) const
{
    //
    // If this is called from GetValueAsString(), return cached string
    if ( !!(flags & wxPGPropValFormatFlags::ValueIsCurrent) )
    {
        return m_display;
    }

    wxArrayString arr = m_value.GetArrayString();
    return ConvertArrayToString(arr, m_delimiter);
}

// Converts wxArrayString to a string separated by delimiters and spaces.
// preDelim is useful for "str1" "str2" style. Set flags to 1 to do slash
// conversion.
wxString wxArrayStringProperty::ArrayStringToString(const wxArrayString& src,
                                                    wxUniChar delimiter, int flags)
{
    wxString pdr;
    wxString preas;

    size_t itemCount = src.size();

    if ( flags & Escape )
    {
        preas = delimiter;
        pdr = wxS("\\");
        pdr += delimiter;
    }

    wxString dst;

    if ( itemCount )
        dst.append( preas );

    wxString delimStr(delimiter);

    for ( size_t i = 0; i < itemCount; i++ )
    {
        wxString str( src.Item(i) );

        // Do some character conversion.
        // Converts \ to \\ and $delimiter to \$delimiter
        // Useful when quoting.
        if ( flags & Escape )
        {
            str.Replace( wxS("\\"), wxS("\\\\"), true );
            if ( !pdr.empty() )
                str.Replace( preas, pdr, true );
        }

        dst.append( str );

        if ( i < (itemCount-1) )
        {
            dst.append( delimStr );
            dst.append( wxS(" ") );
            dst.append( preas );
        }
        else if ( flags & QuoteStrings )
            dst.append( delimStr );
    }

    return dst;
}

void wxArrayStringProperty::GenerateValueAsString()
{
    wxArrayString arr = m_value.GetArrayString();
    m_display = ConvertArrayToString(arr, m_delimiter);
}

// Default implementation doesn't do anything.
bool wxArrayStringProperty::OnCustomStringEdit( wxWindow*, wxString& )
{
    return false;
}

#if WXWIN_COMPATIBILITY_3_0
bool wxArrayStringProperty::OnButtonClick(wxPropertyGrid* WXUNUSED(propgrid),
                                          wxWindow* WXUNUSED(primary),
                                          const wxChar* WXUNUSED(cbt))
{
    return false;
}
#endif // WXWIN_COMPATIBILITY_3_0

wxPGArrayEditorDialog* wxArrayStringProperty::CreateEditorDialog()
{
    return new wxPGArrayStringEditorDialog();
}

bool wxArrayStringProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxPG_VARIANT_TYPE_ARRSTRING), "Function called for incompatible property");
    if ( !pg->EditorValidate() )
        return false;

    // Create editor dialog.
    wxPGArrayEditorDialog* dlg = CreateEditorDialog();
#if wxUSE_VALIDATORS
    wxValidator* validator = GetValidator();
    wxPGInDialogValidator dialogValidator;
#endif

    wxPGArrayStringEditorDialog* strEdDlg = wxDynamicCast(dlg, wxPGArrayStringEditorDialog);

    if ( strEdDlg )
        strEdDlg->SetCustomButton(m_customBtnText, this);

    dlg->SetDialogValue( value );
    dlg->Create(pg->GetPanel(), wxString(),
                m_dlgTitle.empty() ? GetLabel() : m_dlgTitle, m_dlgStyle);

    if ( !wxPropertyGrid::IsSmallScreen() )
    {
        dlg->Move( pg->GetGoodEditorDialogPosition(this,dlg->GetSize()) );
    }

    bool retVal;

    for (;;)
    {
        retVal = false;

        int res = dlg->ShowModal();

        if ( res != wxID_OK || !dlg->IsModified() )
            break;

        wxVariant curValue = dlg->GetDialogValue();
        if ( curValue.IsNull() )
            break;

        wxArrayString actualValue = curValue.GetArrayString();
#if wxUSE_VALIDATORS
        wxString tempStr = ConvertArrayToString(actualValue, m_delimiter);
        if ( dialogValidator.DoValidate(pg, validator, tempStr) )
#endif
        {
            value = actualValue;
            retVal = true;
            break;
        }
    }

    delete dlg;

    return retVal;
}

bool wxArrayStringProperty::StringToValue( wxVariant& variant,
                                           const wxString& text, wxPGPropValFormatFlags ) const
{
    wxArrayString arr;

    if ( m_delimiter == '"' || m_delimiter == '\'' )
    {
        // Quoted strings
        WX_PG_TOKENIZER2_BEGIN(text, m_delimiter)

            // Need to replace backslashes with empty characters
            // (opposite what is done in ConvertArrayToString()).
            token.Replace ( wxS("\\\\"), wxS("\\"), true );

            arr.Add( token );

        WX_PG_TOKENIZER2_END()
    }
    else
    {
        // Regular delimiter
        WX_PG_TOKENIZER1_BEGIN(text, m_delimiter)
            arr.Add( token );
        WX_PG_TOKENIZER1_END()
    }

    variant = arr;

    return true;
}

bool wxArrayStringProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_ARRAY_DELIMITER )
    {
        m_delimiter = value.GetChar();
        GenerateValueAsString();
        return true;
    }
    return wxEditorDialogProperty::DoSetAttribute(name, value);
}

// -----------------------------------------------------------------------
// wxPGInDialogValidator
// -----------------------------------------------------------------------

#if wxUSE_VALIDATORS
bool wxPGInDialogValidator::DoValidate( wxPropertyGrid* propGrid,
                                        wxValidator* validator,
                                        const wxString& value )
{
    if ( !validator )
        return true;

    wxTextCtrl* tc = m_textCtrl;

    if ( !tc )
    {
        {
            tc = new wxTextCtrl( propGrid, wxID_ANY, wxString(),
                                 wxPoint(30000,30000));
            tc->Hide();
        }

        m_textCtrl = tc;
    }

    tc->SetValue(value);

    validator->SetWindow(tc);
    bool res = validator->Validate(propGrid);

    return res;
}
#else
bool wxPGInDialogValidator::DoValidate( wxPropertyGrid* WXUNUSED(propGrid),
                                        wxValidator* WXUNUSED(validator),
                                        const wxString& WXUNUSED(value) )
{
    return true;
}
#endif

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
