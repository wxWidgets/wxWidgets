///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/uilocale.cpp
// Purpose:     wxUILocale implementation for Unix systems
// Author:      Vadim Zeitlin
// Created:     2021-08-01
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_INTL

#include "wx/private/uilocale.h"

#include "wx/unix/private/uilocale.h"

#include "wx/intl.h"

#include <locale.h>
#ifdef HAVE_LANGINFO_H
    #include <langinfo.h>
#endif

namespace
{

// ----------------------------------------------------------------------------
// wxUILocale implementation using standard Unix/C functions
// ----------------------------------------------------------------------------

class wxUILocaleImplUnix : public wxUILocaleImpl
{
public:
    // Locale argument may be NULL to not change it at all.
    explicit wxUILocaleImplUnix(const char* locale);
    ~wxUILocaleImplUnix() wxOVERRIDE;

    bool Use() wxOVERRIDE;
    wxString GetName() const wxOVERRIDE;
    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE;

private:
    // This pointer is owned by this class and may be NULL.
    char* const m_name;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplUnix);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

wxString wxLocaleIdent::GetName() const
{
    // Construct name in the standard Unix format:
    // language[_territory][.codeset][@modifier]

    wxString name;
    if ( !m_language.empty() )
    {
        name << m_language;

        if ( !m_region.empty() )
            name << "_" << m_region;

        if ( !m_charset.empty() )
            name << "." << m_charset;

        if ( !m_modifier.empty() )
            name << "@" << m_modifier;
    }

    return name;
}

// Helper of wxSetlocaleTryAll() below which tries setting the given locale
// with and without UTF-8 suffix. Don't use this one directly.
static const char *wxSetlocaleTryUTF8(int c, const wxString& lc)
{
    const char *l = NULL;

    // NB: We prefer to set UTF-8 locale if it's possible and only fall back to
    //     non-UTF-8 locale if it fails, but this is not necessary under the
    //     supported macOS versions where xx_YY locales are just aliases to
    //     xx_YY.UTF-8 anyhow.
#if wxUSE_UNICODE && !defined(__WXMAC__)
    if ( !lc.empty() )
    {
        wxString buf(lc);
        wxString buf2;
        buf2 = buf + wxS(".UTF-8");
        l = wxSetlocale(c, buf2);
        if ( !l )
        {
            buf2 = buf + wxS(".utf-8");
            l = wxSetlocale(c, buf2);
        }
        if ( !l )
        {
            buf2 = buf + wxS(".UTF8");
            l = wxSetlocale(c, buf2);
        }
        if ( !l )
        {
            buf2 = buf + wxS(".utf8");
            l = wxSetlocale(c, buf2);
        }
    }

    // if we can't set UTF-8 locale, try non-UTF-8 one:
    if ( !l )
#endif // wxUSE_UNICODE && !__WXMAC__
        l = wxSetlocale(c, lc);

    return l;
}

// Try setting all possible versions of the given locale, i.e. with and without
// UTF-8 encoding, and with or without the "_territory" part.
const char *wxSetlocaleTryAll(int c, const wxString& lc)
{
    const char* l = wxSetlocaleTryUTF8(c, lc);
    if ( !l )
    {
        const wxString& lcOnlyLang = ExtractLang(lc);
        if ( lcOnlyLang != lc )
            l = wxSetlocaleTryUTF8(c, lcOnlyLang);
    }

    return l;
}

// ----------------------------------------------------------------------------
// wxUILocale implementation for Unix
// ----------------------------------------------------------------------------

wxUILocaleImplUnix::wxUILocaleImplUnix(const char* locale)
                  : m_name(locale ? strdup(locale) : NULL)
{
}

wxUILocaleImplUnix::~wxUILocaleImplUnix()
{
    free(m_name);
}

bool
wxUILocaleImplUnix::Use()
{
    if ( !m_name )
    {
        // This is the default locale, it is already in use.
        return true;
    }

    const wxString& shortName = wxString::FromAscii(m_name);

    if ( !wxSetlocaleTryAll(LC_ALL, shortName) )
    {
        // Some C libraries (namely glibc) still use old ISO 639,
        // so will translate the abbrev for them
        wxString localeAlt;
        const wxString& langOnly = ExtractLang(shortName);
        if ( langOnly == wxS("he") )
            localeAlt = wxS("iw") + ExtractNotLang(shortName);
        else if ( langOnly == wxS("id") )
            localeAlt = wxS("in") + ExtractNotLang(shortName);
        else if ( langOnly == wxS("yi") )
            localeAlt = wxS("ji") + ExtractNotLang(shortName);
        else if ( langOnly == wxS("nb") )
            localeAlt = wxS("no_NO");
        else if ( langOnly == wxS("nn") )
            localeAlt = wxS("no_NY");

        if ( localeAlt.empty() || !wxSetlocaleTryAll(LC_ALL, localeAlt) )
            return false;
    }

    return true;
}

wxString
wxUILocaleImplUnix::GetName() const
{
    return wxString::FromAscii(m_name ? m_name : setlocale(LC_ALL, NULL));
}

wxString
wxUILocaleImplUnix::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
#ifdef HAVE_LANGINFO_H
    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
#ifdef MON_THOUSANDS_SEP
            if ( cat == wxLOCALE_CAT_MONEY )
                return nl_langinfo(MON_THOUSANDS_SEP);
#endif
            return nl_langinfo(THOUSEP);

        case wxLOCALE_DECIMAL_POINT:
#ifdef MON_DECIMAL_POINT
            if ( cat == wxLOCALE_CAT_MONEY )
                return nl_langinfo(MON_DECIMAL_POINT);
#endif

            return nl_langinfo(RADIXCHAR);

        case wxLOCALE_SHORT_DATE_FMT:
            return nl_langinfo(D_FMT);

        case wxLOCALE_DATE_TIME_FMT:
            return nl_langinfo(D_T_FMT);

        case wxLOCALE_TIME_FMT:
            return nl_langinfo(T_FMT);

        case wxLOCALE_LONG_DATE_FMT:
            return wxGetDateFormatOnly(nl_langinfo(D_T_FMT));

        default:
            wxFAIL_MSG( "unknown wxLocaleInfo value" );
    }

    return wxString();
#else // !HAVE_LANGINFO_H
    // Currently we rely on the user code not calling setlocale() itself, so
    // that the current locale is still the same as was set in the ctor.
    //
    // If this assumption turns out to be wrong, we could use wxLocaleSetter to
    // temporarily change the locale here (maybe only if setlocale(NULL) result
    // differs from the expected one).
    return wxLocale::GetInfo(index, cat);
#endif // HAVE_LANGINFO_H/!HAVE_LANGINFO_H
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return new wxUILocaleImplUnix(NULL);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplUnix("");
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLanguage(const wxLanguageInfo& info)
{
    return new wxUILocaleImplUnix(info.CanonicalName);
}

#endif // wxUSE_INTL
