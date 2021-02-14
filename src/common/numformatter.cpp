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
#include "wx/intl.h"

#include <locale.h> // for setlocale and LC_ALL

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

namespace
{

// Contains information about the locale which was used to initialize our
// cached values of the decimal and thousands separators. Notice that it isn't
// enough to store just wxLocale because the user code may call setlocale()
// directly and storing just C locale string is not enough because we can use
// the OS API directly instead of the CRT ones on some platforms. So just store
// both.
class LocaleId
{
public:
    LocaleId()
    {
#if wxUSE_INTL
        m_wxloc = NULL;
#endif // wxUSE_INTL
        m_cloc = NULL;
    }

    ~LocaleId()
    {
        Free();
    }

#if wxUSE_INTL
    // Return true if this is the first time this function is called for this
    // object or if the program locale has changed since the last time it was
    // called. Otherwise just return false indicating that updating locale-
    // dependent information is not necessary.
    bool NotInitializedOrHasChanged()
    {
        wxLocale * const wxloc = wxGetLocale();
        const char * const cloc = setlocale(LC_ALL, NULL);
        if ( m_wxloc || m_cloc )
        {
            if ( m_wxloc == wxloc && strcmp(m_cloc, cloc) == 0 )
                return false;

            Free();
        }
        //else: Not initialized yet.

        m_wxloc = wxloc;
        m_cloc = wxCRT_StrdupA(cloc);

        return true;
    }
#endif // wxUSE_INTL

private:
    void Free()
    {
#if wxUSE_INTL
        free(m_cloc);
#endif // wxUSE_INTL
    }

#if wxUSE_INTL
    // Non-owned pointer to wxLocale which was used.
    wxLocale *m_wxloc;
#endif // wxUSE_INTL

    // Owned pointer to the C locale string.
    char *m_cloc;

    wxDECLARE_NO_COPY_CLASS(LocaleId);
};

} // anonymous namespace

// ============================================================================
// wxNumberFormatter implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Locale information accessors
// ----------------------------------------------------------------------------

wxChar wxNumberFormatter::GetDecimalSeparator()
{
#if wxUSE_INTL
    // Notice that while using static variable here is not MT-safe, the worst
    // that can happen is that we redo the initialization if we're called
    // concurrently from more than one thread so it's not a real problem.
    static wxChar s_decimalSeparator = 0;

    // Remember the locale which was current when we initialized, we must redo
    // the initialization if the locale changed.
    static LocaleId s_localeUsedForInit;

    if ( s_localeUsedForInit.NotInitializedOrHasChanged() )
    {
        const wxString
            s = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
        if ( s.length() == 1 )
        {
            s_decimalSeparator = s[0];
        }
        else
        {
            // We really must have something for decimal separator, so fall
            // back to the C locale default.
            s_decimalSeparator = '.';
        }
    }

    return s_decimalSeparator;
#else // !wxUSE_INTL
    return wxT('.');
#endif // wxUSE_INTL/!wxUSE_INTL
}

bool wxNumberFormatter::GetThousandsSeparatorIfUsed(wxChar *sep)
{
#if wxUSE_INTL
    static wxChar s_thousandsSeparator = 0;
    static LocaleId s_localeUsedForInit;

    if ( s_localeUsedForInit.NotInitializedOrHasChanged() )
    {
        const wxString
            s = wxLocale::GetInfo(wxLOCALE_THOUSANDS_SEP, wxLOCALE_CAT_NUMBER);
        if ( s.length() == 1 )
        {
            s_thousandsSeparator = s[0];
        }
        //else: Unlike above it's perfectly fine for the thousands separator to
        //      be empty if grouping is not used, so just leave it as 0.
    }

    if ( !s_thousandsSeparator )
        return false;

    if ( sep )
        *sep = s_thousandsSeparator;

    return true;
#else // !wxUSE_INTL
    wxUnusedVar(sep);
    return false;
#endif // wxUSE_INTL/!wxUSE_INTL
}

// ----------------------------------------------------------------------------
// Conversion to string and helpers
// ----------------------------------------------------------------------------

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

wxString wxNumberFormatter::ToString(double val, int precision, int style)
{
    wxString s = wxString::FromDouble(val,precision);

    if ( style & Style_WithThousandsSep )
        AddThousandsSeparators(s);

    if ( style & Style_NoTrailingZeroes )
        RemoveTrailingZeroes(s);

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

    // If it's the decimal separator itself, don't keep it neither.
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

bool wxNumberFormatter::FromString(wxString s, double *val)
{
    RemoveThousandsSeparators(s);
    return s.ToDouble(val);
}
