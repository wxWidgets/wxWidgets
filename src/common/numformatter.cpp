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
        AddThousandsSeparators(s, style);

    wxASSERT_MSG( !(style & Style_NoTrailingZeroes),
                  "Style_NoTrailingZeroes can't be used with integer values" );

    AddSignPrefix(s, style);

    AddCurrency(s, style);

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
        AddThousandsSeparators(s, style);

    if ( style & Style_NoTrailingZeroes )
        RemoveTrailingZeroes(s);

    AddSignPrefix(s, style);

    AddCurrency(s, style);

    return s;
}

wxString wxNumberFormatter::Format(const wxString& format, double val)
{
    wxString s = wxString::Format(format, val);

    const wxUniChar sep = GetDecimalSeparator();
    if ( s.find(sep) == wxString::npos )
    {
        const wxUniChar other = sep == '.' ? ',' : '.';
        const size_t posSep = s.find(other);
        if ( posSep != wxString::npos )
            s[posSep] = sep;
    }
    //else: it already uses the correct separator

    return s;
}

void wxNumberFormatter::AddThousandsSeparators(wxString& s, int style)
{
    // Thousands separators for numbers in scientific format are not relevant.
    if ( s.find_first_of("eE") != wxString::npos )
        return;

    wxString groupingSeparator;
    std::vector<int> grouping;
    wxString decimalSeparator = GetDecimalSeparator();
#if wxUSE_INTL
    wxLocaleNumberFormatting numForm;
    if (style & Style_Currency)
        numForm = wxUILocale::GetCurrent().GetCurrencyInfo().currencyFormat;
    else
        numForm = wxUILocale::GetCurrent().GetNumberFormatting();
    if (!numForm.groupSeparator.empty())
    {
        groupingSeparator = numForm.groupSeparator;
        grouping = numForm.grouping;
        decimalSeparator = numForm.decimalSeparator;
    }
#else
    wxUnusedVar(style);
#endif // wxUSE_INTL

    if (groupingSeparator.empty() )
        return;

    size_t pos = s.find(decimalSeparator);
    if ( pos == wxString::npos )
    {
        // Start grouping at the end of an integer number.
        pos = s.length();
    }

    // End grouping at the beginning of the digits -- there could be at a sign
    // before their start.
    const size_t start = s.find_first_of("0123456789");

    if (grouping.empty())
        return;

    // The vector grouping conatins a list of group length.
    // Beginning from the right, each group length is applied once
    // to determine the next position of a group separator,
    // unless the last entry entry is a zero, in which case
    // the last group length is applied repeatedly.
    size_t groupIndex = 0;
    size_t groupLen = grouping[0];
    size_t nextGroupLen = groupLen;

    // Repeat while the group length is valid (i.e. > 0)
    // and the potential group separator position lies
    // within the number.
    while (groupLen > 0 && pos > start)
    {
        // Determine next group separator position
        pos = (pos >= groupLen) ? pos - groupLen : 0;

        // Apply group separator if it is within the number
        if (pos > start)
            s.insert(pos, groupingSeparator);

        // Select next group length
        if (++groupIndex < grouping.size())
        {
            // Set the group length to the next group length entry
            // unless the next group length is zero,
            // in which case the last group length remains in effect.
            nextGroupLen = grouping[groupIndex];
            if (nextGroupLen != 0)
                groupLen = nextGroupLen;
        }
        else if (nextGroupLen != 0)
        {
            // If the next group length is not zero,
            // the last group length was already applied once, and
            // no further group is to be applied.
            groupLen = 0;
        }
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

// Add the sign prefix to a string representing a number without
// the prefix. This is used by ToString().
void wxNumberFormatter::AddSignPrefix(wxString& s, int style)
{
    wxASSERT_MSG( !(style & Style_SignPlus) ||
                  !(style & Style_SignSpace),
                  "Style_SignPlus can't be used with Style_SignSpace" );

    if ( style & Style_SignPlus )
    {
        if ( s[0] != '-' )
        {
            s = '+' + s;
        }
    }
    else if ( style & Style_SignSpace )
    {
        if ( s[0] != '-' )
        {
            s = ' ' + s;
        }
    }
}

void wxNumberFormatter::AddCurrency(wxString& s, int style)
{
#if wxUSE_INTL
    if (style & Style_Currency)
    {
        auto currencyInfo = wxUILocale::GetCurrent().GetCurrencyInfo();
        wxString currencyStr;
        wxCurrencySymbolPosition currencyPos{ wxCurrencySymbolPosition::PrefixWithSep };
        if (style & Style_CurrencySymbol)
        {
            currencyStr = currencyInfo.currencySymbol;
            currencyPos = currencyInfo.currencySymbolPos;
        }
        else if (style & Style_CurrencyCode)
        {
            currencyStr = currencyInfo.currencyCode;
        }

        if (!currencyStr.empty())
        {
            switch (currencyPos)
            {
                case wxCurrencySymbolPosition::PrefixWithSep:
                    s = currencyStr + wxString(" ") + s;
                    break;
                case wxCurrencySymbolPosition::PrefixNoSep:
                    s = currencyStr + s;
                    break;
                case wxCurrencySymbolPosition::SuffixWithSep:
                    s = s + wxString(" ") + currencyStr;
                    break;
                case wxCurrencySymbolPosition::SuffixNoSep:
                    s = s + currencyStr;
                    break;
            }
        }
    }
#else
    wxUnusedVar(s);
    wxUnusedVar(style);
#endif // wxUSE_INTL
}

wxString wxNumberFormatter::RemoveCurrencySymbolOrCode(wxString s, int style)
{
#if wxUSE_INTL
    if (style & Style_Currency)
    {
        auto currencyInfo = wxUILocale::GetCurrent().GetCurrencyInfo();
        wxString thousandsSep = currencyInfo.currencyFormat.groupSeparator;
        wxString currencyStr;
        wxCurrencySymbolPosition currencyPos{ wxCurrencySymbolPosition::PrefixWithSep };
        if (style & Style_CurrencySymbol)
        {
            currencyStr = currencyInfo.currencySymbol;
            currencyPos = currencyInfo.currencySymbolPos;
        }
        else if (style & Style_CurrencyCode)
        {
            currencyStr = currencyInfo.currencyCode;
        }

        if (!currencyStr.empty())
        {
            wxString valueStr;
            switch (currencyPos)
            {
            case wxCurrencySymbolPosition::PrefixWithSep:
                currencyStr += wxString(" ");
                // Fall through to case without separator
            case wxCurrencySymbolPosition::PrefixNoSep:
                if (s.StartsWith(currencyStr, &valueStr))
                    s = valueStr;
                break;
            case wxCurrencySymbolPosition::SuffixWithSep:
                currencyStr = wxString(" ") + currencyStr;
                // Fall through to case without separator
            case wxCurrencySymbolPosition::SuffixNoSep:
                if (s.EndsWith(currencyStr, &valueStr))
                    s = valueStr;
                break;
            }
        }
        if (style & Style_WithThousandsSep && !thousandsSep.empty())
        {
            s.Replace(thousandsSep, wxString());
        }
    }
    return s;
#else
    wxUnusedVar(style);
    return s;
#endif // wxUSE_INTL
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
