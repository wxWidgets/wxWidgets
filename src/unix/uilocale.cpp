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

#include <locale.h>
#ifdef HAVE_LANGINFO_H
    #include <langinfo.h>
#endif

#include "wx/uilocale.h"
#include "wx/private/glibc.h"
#include "wx/private/uilocale.h"

#include "wx/unix/private/uilocale.h"

#include "wx/intl.h"
#include "wx/log.h"
#include "wx/tokenzr.h"
#include "wx/utils.h"

#define TRACE_I18N wxS("i18n")

namespace
{

// Small helper function: get the value of the given environment variable and
// return true only if the variable was found and has non-empty value.
inline bool wxGetNonEmptyEnvVar(const wxString& name, wxString* value)
{
    return wxGetEnv(name, value) && !value->empty();
}

// Get locale information from the specified environment variable: the output
// variables are filled with the locale part (xx_XX) and the modifier is filled
// with the optional part following "@".
//
// Return false if there is no locale information in the environment variables
// or if it is just "C" or "POSIX".
bool GetLocaleFromEnvVar(const char* var, wxString& langFull, wxString& modifier)
{
    if ( !wxGetNonEmptyEnvVar(var, &langFull) )
    {
        return false;
    }

    // the language string has the following form
    //
    //      lang[_LANG][.encoding][@modifier]
    //
    // (see environ(5) in the Open Unix specification)
    //
    // where lang is the primary language, LANG is a sublang/territory,
    // encoding is the charset to use and modifier "allows the user to select
    // a specific instance of localization data within a single category"
    //
    // for example, the following strings are valid:
    //      fr
    //      fr_FR
    //      de_DE.iso88591
    //      de_DE@euro
    //      de_DE.iso88591@euro

    // for now we don't use the encoding, although we probably should (doing
    // translations of the msg catalogs on the fly as required) (TODO)
    //
    // we need the modifier for languages like Valencian: ca_ES@valencia
    // though, remember it
    size_t posModifier = langFull.find_first_of(wxS("@"));
    if (posModifier != wxString::npos)
        modifier = langFull.Mid(posModifier);

    size_t posEndLang = langFull.find_first_of(wxS("@."));
    if (posEndLang != wxString::npos)
    {
        langFull.Truncate(posEndLang);
    }

    if (langFull == wxS("C") || langFull == wxS("POSIX"))
    {
        // we handle default C locale the same as if no locale were defined
        return false;
    }

    // do we have just the language (or sublang too)?
    const bool justLang = langFull.find('_') == wxString::npos;

    if (justLang && langFull.length() > 2)
    {
        const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
        size_t count = languagesDB.size();

        // In addition to the format above, we also can have full language
        // names in LANG env var - for example, SuSE is known to use
        // LANG="german" - so check for use of non-standard format and try to
        // find the name in verbose description.
        for (size_t i = 0; i < count; i++)
        {
            if (languagesDB[i].Description.CmpNoCase(langFull) == 0)
            {
                break;
            }
            if (i < count)
                langFull = languagesDB[i].CanonicalName;
        }
    }

    // 0. Make sure the lang is according to latest ISO 639
    //    (this is necessary because glibc uses iw and in instead
    //    of he and id respectively).

    // the language itself (second part is the region)
    wxString langOrig = ExtractLang(langFull);
    wxString region = ExtractNotLang(langFull);

    wxString lang;
    if (langOrig == wxS("iw"))
        lang = wxS("he");
    else if (langOrig == wxS("in"))
        lang = wxS("id");
    else if (langOrig == wxS("ji"))
        lang = wxS("yi");
    else if (langOrig == wxS("no") && region == wxS("_NO"))
        lang = wxS("nb");
    else if (langOrig == wxS("no") && region == wxS("_NY"))
    {
        lang = wxS("nn");
        region = wxS("_NO");
    }
    else if (langOrig == wxS("no"))
        lang = wxS("nb");
    else
        lang = langOrig;

    // did we change it?
    if (lang != langOrig)
    {
        langFull = lang + region;
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxUILocale implementation using standard Unix/C functions
// ----------------------------------------------------------------------------

class wxUILocaleImplUnix : public wxUILocaleImpl
{
public:
    // If "loc" is non-null, this object takes ownership of it and will free it,
    // otherwise it creates its own locale_t corresponding to locId.
    explicit wxUILocaleImplUnix(wxLocaleIdent locId
#ifdef HAVE_LOCALE_T
                               , locale_t loc = nullptr
#endif // HAVE_LOCALE_T
                               );
    ~wxUILocaleImplUnix() override;

    void Use() override;

    wxString GetName() const override;
    wxLocaleIdent GetLocaleId() const override;
    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const override;
    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const override;
    wxString GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form) const override;
    wxString GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form) const override;
    wxLayoutDirection GetLayoutDirection() const override;

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const override;

private:
#ifdef HAVE_LANGINFO_H
    // Call nl_langinfo_l() if available, or nl_langinfo() otherwise.
    const char* GetLangInfo(nl_item item) const;
    const wchar_t* GetLangInfoWide(nl_item item) const;

#ifdef __LINUX__
    // Call GetLangInfo() using either the native or English item depending on
    // the form needed.
    wxString GetFormOfLangInfo(wxLocaleForm form,
                               nl_item nlNative,
                               nl_item nlEnglish) const;
#endif

#endif // HAVE_LANGINFO_H

    void InitLocaleNameAndCodeset() const;

    const wxString& GetCodeSet() const;


    wxLocaleIdent m_locId;

    // Both m_codeset and m_name are initialized on demand, so GetCodeSet() and
    // GetName() must always be used to access them instead of using them
    // directly.
    mutable wxString m_codeset;
    mutable wxString m_name;

#ifdef HAVE_LOCALE_T
    const locale_t m_locale;
#endif // HAVE_LOCALE_T

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplUnix);
};

#ifdef HAVE_LOCALE_T

// Simple wrapper around newlocale().
inline locale_t TryCreateLocale(const wxLocaleIdent& locId)
{
    return newlocale(LC_ALL_MASK, locId.GetName().mb_str(), nullptr);
}

// Wrapper around newlocale() also trying to append UTF-8 codeset (and
// modifying its wxLocaleIdent argument if it succeeds).
locale_t TryCreateLocaleWithUTF8(wxLocaleIdent& locId)
{
    locale_t loc = nullptr;

    if ( locId.GetCharset().empty() )
    {
        wxLocaleIdent locIdUTF8(locId);
        locIdUTF8.Charset(wxS("UTF-8"));

        loc = TryCreateLocale(locIdUTF8);
        if ( !loc )
        {
            locIdUTF8.Charset(wxS("utf-8"));
            loc = TryCreateLocale(locIdUTF8);
        }
        if ( !loc )
        {
            locIdUTF8.Charset(wxS("UTF8"));
            loc = TryCreateLocale(locIdUTF8);
        }
        if ( !loc )
        {
            locIdUTF8.Charset(wxS("utf8"));
            loc = TryCreateLocale(locIdUTF8);
        }
        if ( loc )
            locId = locIdUTF8;
    }

    // if we can't set UTF-8 locale, try non-UTF-8 one:
    if ( !loc )
        loc = TryCreateLocale(locId);

    return loc;
}

// Try finding a locale for the given identifier, modifying the argument to
// match the found locale if necessary.
locale_t TryCreateMatchingLocale(wxLocaleIdent& locId)
{
    locale_t loc = TryCreateLocaleWithUTF8(locId);
    if ( !loc && locId.GetRegion().empty() )
    {
        // Try to find a variant of this locale available on this system: as
        // using just the language, without the territory, typically does _not_
        // work under Linux, we try adding one if we don't have it.
        const wxString lang = locId.GetLanguage();

        const wxLanguageInfos& infos = wxGetLanguageInfos();
        for ( wxLanguageInfos::const_iterator it = infos.begin();
              it != infos.end();
              ++it )
        {
            const wxString& fullname = it->GetCanonicalWithRegion();
            if ( fullname.BeforeFirst('_') == lang )
            {
                // We never have encoding in our canonical names, but we
                // can have modifiers, so get rid of them if necessary.
                const wxString&
                    region = fullname.AfterFirst('_').BeforeFirst('@');
                if ( !region.empty() )
                {
                    loc = TryCreateLocaleWithUTF8(locId.Region(region));
                    if ( loc )
                    {
                        // We take the first available region, we don't
                        // have enough data to know how to prioritize them
                        // (and wouldn't want to start any geopolitical
                        // disputes).
                        break;
                    }
                }

                // Don't bother reverting region to the old value as it will
                // be overwritten during the next loop iteration anyhow.
            }
        }
    }

    return loc;
}

#endif // HAVE_LOCALE_T

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

        if ( !m_script.empty() )
            name << "@" << wxUILocale::GetScriptAliasFromName(m_script);
        else if ( !m_modifier.empty() )
            name << "@" << m_modifier;
    }

    return name;
}

// Helper of wxSetlocaleTryAll() below which tries setting the given locale
// with and without UTF-8 suffix. Don't use this one directly.
static const char *wxSetlocaleTryUTF8(int c, const wxLocaleIdent& locId)
{
    const char *l = nullptr;

    // NB: We prefer to set UTF-8 locale if it's possible and only fall back to
    //     non-UTF-8 locale if it fails.
    if ( locId.GetCharset().empty() )
    {
        wxLocaleIdent locIdUTF8(locId);
        locIdUTF8.Charset(wxS("UTF-8"));

        l = wxSetlocale(c, locIdUTF8.GetName());
        if ( !l )
        {
            locIdUTF8.Charset(wxS("utf-8"));
            l = wxSetlocale(c, locIdUTF8.GetName());
        }
        if ( !l )
        {
            locIdUTF8.Charset(wxS("UTF8"));
            l = wxSetlocale(c, locIdUTF8.GetName());
        }
        if ( !l )
        {
            locIdUTF8.Charset(wxS("utf8"));
            l = wxSetlocale(c, locIdUTF8.GetName());
        }
    }

    // if we can't set UTF-8 locale, try non-UTF-8 one:
    if ( !l )
        l = wxSetlocale(c, locId.GetName());

    return l;
}

// Try setting all possible versions of the given locale, i.e. with and without
// UTF-8 encoding, and with or without the "_territory" part.
const char *wxSetlocaleTryAll(int c, const wxLocaleIdent& locId)
{
    const char* l = wxSetlocaleTryUTF8(c, locId);
    if ( !l )
    {
        if ( !locId.GetRegion().empty() )
            l = wxSetlocaleTryUTF8(c, wxLocaleIdent(locId).Region(wxString()));
    }

    return l;
}

// ----------------------------------------------------------------------------
// wxUILocale implementation for Unix
// ----------------------------------------------------------------------------

wxUILocaleImplUnix::wxUILocaleImplUnix(wxLocaleIdent locId
#ifdef HAVE_LOCALE_T
                                      , locale_t loc
#endif // HAVE_LOCALE_T
                                      )
                  : m_locId(locId)
#ifdef HAVE_LOCALE_T
                  , m_locale(loc ? loc : TryCreateLocale(locId))
#endif // HAVE_LOCALE_T
{
}

wxUILocaleImplUnix::~wxUILocaleImplUnix()
{
#ifdef HAVE_LOCALE_T
    if ( m_locale )
        freelocale(m_locale);
#endif // HAVE_LOCALE_T
}

void
wxUILocaleImplUnix::Use()
{
    // Note that we don't need to test if m_locId is not empty here, as we
    // still want to call setlocale() with the empty string to use the default
    // locale settings in this case.
    if ( !wxSetlocaleTryAll(LC_ALL, m_locId) )
    {
        // Some C libraries (namely glibc) still use old ISO 639,
        // so will translate the abbrev for them
        wxLocaleIdent locIdAlt(m_locId);

        const wxString& langOnly = m_locId.GetLanguage();
        if ( langOnly == wxS("he") )
            locIdAlt.Language(wxS("iw"));
        else if ( langOnly == wxS("id") )
            locIdAlt.Language(wxS("in"));
        else if ( langOnly == wxS("yi") )
            locIdAlt.Language(wxS("ji"));
        else if ( langOnly == wxS("nb") || langOnly == wxS("nn") )
        {
            locIdAlt.Language(wxS("no"));
            locIdAlt.Region(langOnly == wxS("nb") ? wxS("NO") : wxS("NY"));
        }
        else
        {
            // Nothing else to try.
            return;
        }

        wxSetlocaleTryAll(LC_ALL, locIdAlt);
    }
}

void
wxUILocaleImplUnix::InitLocaleNameAndCodeset() const
{
#ifdef HAVE_LANGINFO_H
    // If extended locale support is not available, we need to temporarily
    // switch the global locale to the one we use.
#ifndef HAVE_LOCALE_T
    TempLocaleSetter setThisLocale(LC_CTYPE, m_locId.GetName());
#endif

#ifdef _NL_LOCALE_NAME
    m_name = GetLangInfo(_NL_LOCALE_NAME(LC_CTYPE));
#else
    m_name = m_locId.GetName();
    if ( m_name.empty() )
    {
        // This must be the default locale.
        wxString locName,
                 modifier;
        if ( !GetLocaleFromEnvVar("LC_ALL", locName, modifier) &&
                !GetLocaleFromEnvVar("LANG", locName, modifier) )
        {
            // This is the default locale if nothing is specified.
            locName = "en_US";
        }

        m_name = locName + modifier;
    }
#endif

    m_codeset = GetLangInfo(CODESET);
#endif // HAVE_LANGINFO_H
}

wxString
wxUILocaleImplUnix::GetName() const
{
    if ( m_name.empty() )
        InitLocaleNameAndCodeset();

    return m_name;
}

const wxString&
wxUILocaleImplUnix::GetCodeSet() const
{
    if ( m_codeset.empty() )
        InitLocaleNameAndCodeset();

    return m_codeset;
}

wxLocaleIdent
wxUILocaleImplUnix::GetLocaleId() const
{
    return wxLocaleIdent::FromTag(GetName());
}

#ifdef HAVE_LANGINFO_H

const char*
wxUILocaleImplUnix::GetLangInfo(nl_item item) const
{
#ifdef HAVE_LOCALE_T
    // We assume that we have nl_langinfo_l() if we have locale_t.
    if ( m_locale )
        return nl_langinfo_l(item, m_locale);
#else
    TempLocaleSetter setThisLocale(LC_CTYPE, m_locId.GetName());
#endif // HAVE_LOCALE_T

    return nl_langinfo(item);
}

const wchar_t*
wxUILocaleImplUnix::GetLangInfoWide(nl_item item) const
{
#ifdef HAVE_LOCALE_T
    // We assume that we have nl_langinfo_l() if we have locale_t.
    if ( m_locale )
        return (wchar_t*) nl_langinfo_l(item, m_locale);
#else
    TempLocaleSetter setThisLocale(LC_CTYPE, m_locId.GetName());
#endif // HAVE_LOCALE_T

    return (wchar_t*) nl_langinfo(item);
}

#ifdef __LINUX__
wxString
wxUILocaleImplUnix::GetFormOfLangInfo(wxLocaleForm form,
                                      nl_item nlNative,
                                      nl_item nlEnglish) const
{
    // Choose a random value that we're sure is never going to be used with
    // this function (it doesn't make sense for this one to exist in native in
    // English versions, so it seems to fit the bill well).
    nl_item item = RADIXCHAR;
    switch ( form )
    {
        case wxLOCALE_FORM_NATIVE:
            item = nlNative;
            break;

        case wxLOCALE_FORM_ENGLISH:
            item = nlEnglish;
            break;
    }

    wxCHECK_MSG( item != RADIXCHAR, wxString(), "unknown wxLocaleForm" );

    return wxString(GetLangInfo(item), wxCSConv(GetCodeSet()));
}
#endif

#endif // HAVE_LANGINFO_H

wxString
wxUILocaleImplUnix::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
#ifdef HAVE_LANGINFO_H
    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
#ifdef MON_THOUSANDS_SEP
            if ( cat == wxLOCALE_CAT_MONEY )
                return GetLangInfo(MON_THOUSANDS_SEP);
#else
            wxUnusedVar(cat);
#endif
            return GetLangInfo(THOUSEP);

        case wxLOCALE_DECIMAL_POINT:
#ifdef MON_DECIMAL_POINT
            if ( cat == wxLOCALE_CAT_MONEY )
                return GetLangInfo(MON_DECIMAL_POINT);
#else
            wxUnusedVar(cat);
#endif

            return GetLangInfo(RADIXCHAR);

        case wxLOCALE_SHORT_DATE_FMT:
            return GetLangInfo(D_FMT);

        case wxLOCALE_DATE_TIME_FMT:
            return GetLangInfo(D_T_FMT);

        case wxLOCALE_TIME_FMT:
            return GetLangInfo(T_FMT);

        case wxLOCALE_LONG_DATE_FMT:
            return wxGetDateFormatOnly(GetLangInfo(D_T_FMT));

        default:
            wxFAIL_MSG( "unknown wxLocaleInfo value" );
    }

    return wxString();
#else // !HAVE_LANGINFO_H
    // Currently we rely on the user code not calling setlocale() itself, so
    // that the current locale is still the same as was set in the ctor.
    //
    // If this assumption turns out to be wrong, we could use wxLocaleSetter to
    // temporarily change the locale here (maybe only if setlocale(nullptr) result
    // differs from the expected one).
    return wxLocale::GetInfo(index, cat);
#endif // HAVE_LANGINFO_H/!HAVE_LANGINFO_H
}

wxString
wxUILocaleImplUnix::GetLocalizedName(wxLocaleName name, wxLocaleForm form) const
{
    wxString str;
#if defined(HAVE_LANGINFO_H) && defined(__LINUX__) && defined(__GLIBC__)
    switch (name)
    {
        case wxLOCALE_NAME_LOCALE:
            {
                str = GetFormOfLangInfo(form,
                                        _NL_ADDRESS_LANG_NAME,
                                        _NL_IDENTIFICATION_LANGUAGE);
                const wxString
                    strCtry = GetFormOfLangInfo(form,
                                                _NL_ADDRESS_COUNTRY_NAME,
                                                _NL_IDENTIFICATION_TERRITORY);
                if (!strCtry.empty())
                {
                    str << " (" << strCtry << ")";
                }
            }
            break;

        case wxLOCALE_NAME_LANGUAGE:
            str = GetFormOfLangInfo(form,
                                    _NL_ADDRESS_LANG_NAME,
                                    _NL_IDENTIFICATION_LANGUAGE);
            break;

        case wxLOCALE_NAME_COUNTRY:
            str = GetFormOfLangInfo(form,
                                    _NL_ADDRESS_COUNTRY_NAME,
                                    _NL_IDENTIFICATION_TERRITORY);
            break;

        default:
            wxFAIL_MSG("unknown wxLocaleName");
    }
#else // !HAVE_LANGINFO_H || !__LINUX__
    // If HAVE_LANGINFO_H is not available, or system is not Linux-like,
    // use our own language database to retrieve the requested information.
    const wxLanguageInfo* langInfo = wxUILocale::FindLanguageInfo(wxLocaleIdent::FromTag(GetName()));
    if (langInfo)
    {
        wxString langDesc;
        switch ( form )
        {
            case wxLOCALE_FORM_NATIVE:
                langDesc = langInfo->Description;
                break;

            case wxLOCALE_FORM_ENGLISH:
                langDesc = langInfo->DescriptionNative;
                break;
            default:
                break;
        }
        switch (name)
        {
            case wxLOCALE_NAME_LOCALE:
                str = langDesc;
                break;

            case wxLOCALE_NAME_LANGUAGE:
                str = langDesc.BeforeFirst('(').Trim();
                break;

            case wxLOCALE_NAME_COUNTRY:
                str = langDesc.AfterFirst('(').BeforeLast(')');
                break;

            default:
                wxFAIL_MSG("unknown wxLocaleName");
        }
    }
#endif // HAVE_LANGINFO_H && __LINUX__/!HAVE_LANGINFO_H || !__LINUX__
    return str;
}

wxString
wxUILocaleImplUnix::GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form) const
{
    // This really should be a configure/CMake test, but for now the only
    // environment known to provide _NL_WALTMON_xxx is Linux with glibc 2.27+.
#if defined(__LINUX__) && wxCHECK_GLIBC_VERSION(2, 27)
    static int monthNameIndex[6][12] =
    {
        // Formatting context
        { _NL_WMON_1,  _NL_WMON_2,  _NL_WMON_3,
          _NL_WMON_4,  _NL_WMON_5,  _NL_WMON_6,
          _NL_WMON_7,  _NL_WMON_8,  _NL_WMON_9,
          _NL_WMON_10, _NL_WMON_11, _NL_WMON_12 },
        { _NL_WABMON_1,  _NL_WABMON_2,  _NL_WABMON_3,
          _NL_WABMON_4,  _NL_WABMON_5,  _NL_WABMON_6,
          _NL_WABMON_7,  _NL_WABMON_8,  _NL_WABMON_9,
          _NL_WABMON_10, _NL_WABMON_11, _NL_WABMON_12 },
        { _NL_WABMON_1,  _NL_WABMON_2,  _NL_WABMON_3,
          _NL_WABMON_4,  _NL_WABMON_5,  _NL_WABMON_6,
          _NL_WABMON_7,  _NL_WABMON_8,  _NL_WABMON_9,
          _NL_WABMON_10, _NL_WABMON_11, _NL_WABMON_12 },
        // Standalone context
        { _NL_WALTMON_1,  _NL_WALTMON_2,  _NL_WALTMON_3,
          _NL_WALTMON_4,  _NL_WALTMON_5,  _NL_WALTMON_6,
          _NL_WALTMON_7,  _NL_WALTMON_8,  _NL_WALTMON_9,
          _NL_WALTMON_10, _NL_WALTMON_11, _NL_WALTMON_12 },
        { _NL_WABALTMON_1,  _NL_WABALTMON_2,  _NL_WABALTMON_3,
          _NL_WABALTMON_4,  _NL_WABALTMON_5,  _NL_WABALTMON_6,
          _NL_WABALTMON_7,  _NL_WABALTMON_8,  _NL_WABALTMON_9,
          _NL_WABALTMON_10, _NL_WABALTMON_11, _NL_WABALTMON_12 },
        { _NL_WABALTMON_1,  _NL_WABALTMON_2,  _NL_WABALTMON_3,
          _NL_WABALTMON_4,  _NL_WABALTMON_5,  _NL_WABALTMON_6,
          _NL_WABALTMON_7,  _NL_WABALTMON_8,  _NL_WABALTMON_9,
          _NL_WABALTMON_10, _NL_WABALTMON_11, _NL_WABALTMON_12 }
    };

    int idx = ArrayIndexFromFlag(form.GetFlags());
    if (idx == -1)
        return wxString();

    if (form.GetContext() == wxDateTime::Context_Standalone)
        idx += 3;

    return wxString(GetLangInfoWide(monthNameIndex[idx][month]));
#elif defined(HAVE_LANGINFO_H)
    // If system is not Linux-like or doesn't have new enough GLIBC, fall back
    // to LC_TIME symbols that should be defined according to POSIX.
    static int monthNameIndex[3][12] =
    {
        // Formatting context
        { MON_1,  MON_2,  MON_3,
          MON_4,  MON_5,  MON_6,
          MON_7,  MON_8,  MON_9,
          MON_10, MON_11, MON_12 },
        { ABMON_1,  ABMON_2,  ABMON_3,
          ABMON_4,  ABMON_5,  ABMON_6,
          ABMON_7,  ABMON_8,  ABMON_9,
          ABMON_10, ABMON_11, ABMON_12 },
        { ABMON_1,  ABMON_2,  ABMON_3,
          ABMON_4,  ABMON_5,  ABMON_6,
          ABMON_7,  ABMON_8,  ABMON_9,
          ABMON_10, ABMON_11, ABMON_12 }
    };

    int idx = ArrayIndexFromFlag(form.GetFlags());
    if (idx == -1)
        return wxString();

    return wxString(GetLangInfo(monthNameIndex[idx][month]), wxCSConv(GetCodeSet()));
#else // !HAVE_LANGINFO_H
    // If HAVE_LANGINFO_H is not available, fall back to English names.
    return wxDateTime::GetEnglishMonthName(month, form);
#endif // HAVE_LANGINFO_H
}

wxString
wxUILocaleImplUnix::GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form) const
{
#if defined(HAVE_LANGINFO_H)
#if defined(__LINUX__) && defined(__GLIBC__)
    static int weekdayNameIndex[3][12] =
    {
        { _NL_WDAY_1, _NL_WDAY_2, _NL_WDAY_3,
          _NL_WDAY_4, _NL_WDAY_5, _NL_WDAY_6, _NL_WDAY_7 },
        { _NL_WABDAY_1, _NL_WABDAY_2, _NL_WABDAY_3,
          _NL_WABDAY_4, _NL_WABDAY_5, _NL_WABDAY_6, _NL_WABDAY_7 },
        { _NL_WABDAY_1, _NL_WABDAY_2, _NL_WABDAY_3,
          _NL_WABDAY_4, _NL_WABDAY_5, _NL_WABDAY_6, _NL_WABDAY_7 }
    };

    const int idx = ArrayIndexFromFlag(form.GetFlags());
    if (idx == -1)
        return wxString();

    return wxString(GetLangInfoWide(weekdayNameIndex[idx][weekday]));
#else // !__LINUX__ || !__GLIBC__
    // If system is not Linux-like or does not have GLIBC, fall back
    // to LC_TIME symbols that should be defined according to POSIX.
    static int weekdayNameIndex[3][12] =
    {
        { DAY_1, DAY_2, DAY_3,
          DAY_4, DAY_5, DAY_6, DAY_7 },
        { ABDAY_1, ABDAY_2, ABDAY_3,
          ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7 },
        { ABDAY_1, ABDAY_2, ABDAY_3,
          ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7 }
    };

    const int idx = ArrayIndexFromFlag(form.GetFlags());
    if (idx == -1)
        return wxString();

    return wxString(GetLangInfo(weekdayNameIndex[idx][weekday]), wxCSConv(GetCodeSet()));
#endif //  __LINUX__ && __GLIBC__ / !__LINUX__ || !__GLIBC__
#else // !HAVE_LANGINFO_H
    // If HAVE_LANGINFO_H is not available, fall back to English names.
    return wxDateTime::GetEnglishWeekDayName(weekday, form);
#endif // HAVE_LANGINFO_H / !HAVE_LANGINFO_H
}

wxLayoutDirection
wxUILocaleImplUnix::GetLayoutDirection() const
{
    // Under Linux/Unix the locale data do not contain information
    // about layout direction. For now, return wxLayout_Default.
    // wxUILocale will try to use the language database as a fallback.
    return wxLayout_Default;
}

int
wxUILocaleImplUnix::CompareStrings(const wxString& lhs, const wxString& rhs,
                                   int WXUNUSED(flags)) const
{
    int rc;

#ifdef HAVE_LOCALE_T
    if ( m_locale )
        rc = wcscoll_l(lhs.wc_str(), rhs.wc_str(), m_locale);
    else
#endif // HAVE_LOCALE_T
        rc = wcscoll(lhs.wc_str(), rhs.wc_str());

    if ( rc < 0 )
        return -1;

    if ( rc > 0 )
        return 1;

    return 0;
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return new wxUILocaleImplUnix(wxLocaleIdent().Language("C"));
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
#ifdef HAVE_LOCALE_T
    // Setting default locale can fail under Unix if LANG or LC_ALL are set to
    // an unsupported value, so check for this here to let the caller know if
    // we can't do it.
    wxLocaleIdent locDef;
    locale_t loc = TryCreateLocaleWithUTF8(locDef);
    if ( !loc )
        return nullptr;

    return new wxUILocaleImplUnix(wxLocaleIdent(), loc);
#else // !HAVE_LOCALE_T
    // We could temporarily change the locale here to check if it's supported,
    // but for now don't bother and assume it is.
    return new wxUILocaleImplUnix(wxLocaleIdent());
#endif // HAVE_LOCALE_T/!HAVE_LOCALE_T
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locIdOrig)
{
#ifdef HAVE_LOCALE_T
    // Make a copy of it because it can be modified below.
    wxLocaleIdent locId = locIdOrig;

    const locale_t loc = TryCreateMatchingLocale(locId);
    if ( !loc )
        return nullptr;

    return new wxUILocaleImplUnix(locId, loc);
#else // !HAVE_LOCALE_T
    // We can't check locale availability without changing it in this case, so
    // just assume it's valid.
    return new wxUILocaleImplUnix(locIdOrig);
#endif // HAVE_LOCALE_T/!HAVE_LOCALE_T
}

/* static */
wxVector<wxString> wxUILocaleImpl::GetPreferredUILanguages()
{
    wxVector<wxString> preferred;

    // When the preferred UI language is determined, the LANGUAGE environment
    // variable is the primary source of preference.
    // http://www.gnu.org/software/gettext/manual/html_node/Locale-Environment-Variables.html

    // Since the first item in LANGUAGE is supposed to be equal to LANG resp LC_ALL,
    // determine the default language based on the locale related environment variables
    // as the first entry in the list of preferred languages.
    wxString langFull;
    wxString modifier;

    // Check LC_ALL first, as it's supposed to override everything else, then
    // for LC_MESSAGES because this is the variable defining the translations
    // language and so must correspond to the language the user wants to use
    // and, otherwise, fall back on LANG which is the normal way to specify
    // both the locale and the language.
    if ( GetLocaleFromEnvVar("LC_ALL", langFull, modifier) ||
            GetLocaleFromEnvVar("LC_MESSAGES", langFull, modifier) ||
                GetLocaleFromEnvVar("LANG", langFull, modifier) )
    {
        if (!modifier.empty())
        {
            // Locale name with modifier
            if (const wxLanguageInfo* li = wxUILocale::FindLanguageInfo(langFull + modifier))
            {
                preferred.push_back(li->CanonicalName);
            }
        }
        // Locale name without modifier
        if (const wxLanguageInfo* li = wxUILocale::FindLanguageInfo(langFull))
        {
            preferred.push_back(li->CanonicalName);
        }
    }

    // The LANGUAGE variable may contain a colon separated list of language
    // codes in the order of preference.
    // http://www.gnu.org/software/gettext/manual/html_node/The-LANGUAGE-variable.html
    wxString languageFromEnv;
    if (wxGetNonEmptyEnvVar("LANGUAGE", &languageFromEnv))
    {
        wxStringTokenizer tknzr(languageFromEnv, ":");
        while (tknzr.HasMoreTokens())
        {
            const wxString tok = tknzr.GetNextToken();
            if (const wxLanguageInfo* li = wxUILocale::FindLanguageInfo(tok))
            {
                preferred.push_back(li->CanonicalName);
            }
        }
        if (!preferred.empty())
            return preferred;
        wxLogTrace(TRACE_I18N, " - LANGUAGE was set, but it didn't contain any languages recognized by the system");
    }
    else
    {
        wxLogTrace(TRACE_I18N, " - LANGUAGE was not set or empty, check LC_ALL, LC_MESSAGES, and LANG");
    }

    if (preferred.empty())
    {
        // no language specified, treat it as English
        langFull = "en_US";
        preferred.push_back(langFull);
        wxLogTrace(TRACE_I18N, " - LC_ALL, LC_MESSAGES, and LANG were not set or empty, use English");
    }

    return preferred;
}

#endif // wxUSE_INTL
