/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/numformatter.cpp
// Purpose:     wxNumberFormatter
// Author:      Fulvio Senore, Vadim Zeitlin
// Created:     2010-11-06
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/numformatter.h"
#include "wx/uilocale.h"

// ============================================================================
// wxNumberFormatter implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Locale information accessors
// ----------------------------------------------------------------------------

wxChar wxNumberFormatter::GetDecimalSeparator()
{
#if wxUSE_INTL
    const wxString
        s = wxUILocale::GetCurrent().GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
    if ( s.length() == 1 )
    {
        return s[0];
    }

    // We really must have something for decimal separator, so fall
    // back to the C locale default.
#endif // wxUSE_INTL

    return wxT('.');
}

bool wxNumberFormatter::GetThousandsSeparatorIfUsed(wxChar *sep)
{
#if wxUSE_INTL
    const wxString
        s = wxUILocale::GetCurrent().GetInfo(wxLOCALE_THOUSANDS_SEP, wxLOCALE_CAT_NUMBER);
    if ( s.length() == 1 )
    {
        if ( sep )
            *sep = s[0];

        return true;
    }
#endif // wxUSE_INTL

    wxUnusedVar(sep);
    return false;
}

// ----------------------------------------------------------------------------
// Conversion to string and helpers
// ----------------------------------------------------------------------------

namespace
{

void ReplaceSeparatorIfNecessary(wxString& s, wxChar sepOld, wxChar sepNew)
{
    if ( sepNew != sepOld )
    {
        const size_t posSep = s.find(sepOld);
        if ( posSep != wxString::npos )
            s[posSep] = sepNew;
    }
}

} // anonymous namespace

wxString wxNumberFormatter::PostProcessIntString(wxString s, int style)
{
    if ( style & Style_WithThousandsSep )
        AddThousandsSeparators(s);

    wxASSERT_MSG( !(style & Style_NoTrailingZeroes),
                  "Style_NoTrailingZeroes can't be used with integer values" );

    return s;
}

wxString wxNumberFormatter::ToString(long val, int style)
{
    return PostProcessIntString(wxString::Format("%ld", val), style);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

wxString wxNumberFormatter::ToString(wxLongLong_t val, int style)
{
    return PostProcessIntString(wxString::Format("%" wxLongLongFmtSpec "d", val),
                                style);
}

#endif // wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

wxString wxNumberFormatter::ToString(wxULongLong_t val, int style)
{
    return PostProcessIntString(wxString::Format("%" wxLongLongFmtSpec "u", val),
                                style);
}

wxString wxNumberFormatter::ToString(double val, int precision, int style)
{
    wxString s = wxString::FromCDouble(val,precision);

    ReplaceSeparatorIfNecessary(s, '.', GetDecimalSeparator());

    if ( style & Style_WithThousandsSep )
        AddThousandsSeparators(s);

    if ( style & Style_NoTrailingZeroes )
        RemoveTrailingZeroes(s);

    return s;
}

wxString wxNumberFormatter::Format(const wxString& format, double val)
{
    wxString s = wxString::Format(format, val);

    const wxChar sep = GetDecimalSeparator();
    if ( s.find(sep) == wxString::npos )
    {
        const wxChar other = sep == '.' ? ',' : '.';
        const size_t posSep = s.find(other);
        if ( posSep != wxString::npos )
            s[posSep] = sep;
    }
    //else: it already uses the correct separator

    return s;
}

void wxNumberFormatter::AddThousandsSeparators(wxString& s)
{
    // Thousands separators for numbers in scientific format are not relevant.
    if ( s.find_first_of("eE") != wxString::npos )
        return;

    wxChar thousandsSep;
    if ( !GetThousandsSeparatorIfUsed(&thousandsSep) )
        return;

    size_t pos = s.find(GetDecimalSeparator());
    if ( pos == wxString::npos )
    {
        // Start grouping at the end of an integer number.
        pos = s.length();
    }

    // End grouping at the beginning of the digits -- there could be at a sign
    // before their start.
    const size_t start = s.find_first_of("0123456789");

    // We currently group digits by 3 independently of the locale. This is not
    // the right thing to do and we should use lconv::grouping (under POSIX)
    // and GetLocaleInfo(LOCALE_SGROUPING) (under MSW) to get information about
    // the correct grouping to use. This is something that needs to be done at
    // wxLocale level first and then used here in the future (TODO).
    const size_t GROUP_LEN = 3;

    while ( pos > start + GROUP_LEN )
    {
        pos -= GROUP_LEN;
        s.insert(pos, thousandsSep);
    }
}

void wxNumberFormatter::RemoveTrailingZeroes(wxString& s)
{
    // If number is in scientific format, trailing zeroes belong to the exponent and cannot be removed.
    if ( s.find_first_of("eE") != wxString::npos )
        return;

    const size_t posDecSep = s.find(GetDecimalSeparator());
    // No decimal point => removing trailing zeroes irrelevant for integer number.
    if ( posDecSep == wxString::npos )
        return;
    wxCHECK_RET( posDecSep, "Can't start with decimal separator" );

    // Find the last character to keep.
    size_t posLastNonZero = s.find_last_not_of("0");

    // If it's the decimal separator itself, don't keep it either.
    if ( posLastNonZero == posDecSep )
        posLastNonZero--;

    s.erase(posLastNonZero + 1);
    // Remove sign from orphaned zero.
    if ( s.compare("-0") == 0 )
        s = "0";
}

// ----------------------------------------------------------------------------
// Conversion from strings
// ----------------------------------------------------------------------------

void wxNumberFormatter::RemoveThousandsSeparators(wxString& s)
{
    wxChar thousandsSep;
    if ( !GetThousandsSeparatorIfUsed(&thousandsSep) )
        return;

    s.Replace(wxString(thousandsSep), wxString());
}

bool wxNumberFormatter::FromString(wxString s, long *val)
{
    RemoveThousandsSeparators(s);
    return s.ToLong(val);
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

bool wxNumberFormatter::FromString(wxString s, wxLongLong_t *val)
{
    RemoveThousandsSeparators(s);
    return s.ToLongLong(val);
}

#endif // wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

bool wxNumberFormatter::FromString(wxString s, wxULongLong_t *val)
{
    RemoveThousandsSeparators(s);

    // wxString::ToULongLong() does accept minus sign for unsigned integers,
    // consistently with the standard functions behaviour, e.g. strtoul() does
    // the same thing, but here we really want to accept the "true" unsigned
    // numbers only, so check for leading minus, possibly preceded by some
    // whitespace.
    for ( wxString::const_iterator it = s.begin(); it != s.end(); ++it )
    {
        if ( *it == '-' )
            return false;

        if ( *it != ' ' && *it != '\t' )
            break;
    }

    return s.ToULongLong(val);
}

bool wxNumberFormatter::FromString(wxString s, double *val)
{
    RemoveThousandsSeparators(s);
    ReplaceSeparatorIfNecessary(s, GetDecimalSeparator(), '.');
    return s.ToCDouble(val);
}
