///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/uilocale.cpp
// Purpose:     wxUILocale implementation
// Author:      Vadim Zeitlin
// Created:     2021-07-31
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

#include "wx/uilocale.h"

#include "wx/arrstr.h"
#include "wx/intl.h"

#ifndef __WINDOWS__
    #include "wx/language.h"
#endif

#include "wx/private/uilocale.h"

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{

const char* validCharsAlpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* validCharsAlnum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char* validCharsModExt = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";

// Handle special case "ca-ES-valencia"
// Sync attributes modifier and extension
inline void CheckLanguageVariant(wxLocaleIdent& locId)
{
    if (locId.GetModifier().IsSameAs("valencia"))
    {
        locId.Extension(locId.GetModifier());
    }
    else if (locId.GetExtension().IsSameAs("valencia") && locId.GetModifier().empty())
    {
        locId.Modifier(locId.GetExtension());
    }
}

inline bool IsDefaultCLocale(const wxString& locale)
{
    return locale.IsSameAs("C", false) || locale.IsSameAs("POSIX", false);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// This static global variable doesn't need to be protected from concurrent
// access as it's only supposed to be used from the UI thread.
/* static */
wxUILocale wxUILocale::ms_current;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxLocaleIdent
// ----------------------------------------------------------------------------

/* static */
wxLocaleIdent wxLocaleIdent::FromTag(const wxString& tag)
{
    // This method accepts tags in various formats: BCP47, Windows, POSIX, and macOS.
    //
    // See section 2.01 of https://www.rfc-editor.org/rfc/bcp/bcp47.txt for the
    // full syntax. Here we fully support just the subset we're interested in:
    //
    //  - Normal language tags (not private use or grandfathered ones).
    //  - Script and region.
    //
    // Additionally platform-specific tags are supported:
    //  - Extensions (without validity checks) (Windows only).
    //  - Charset and modifier (POSIX only)
    //
    // Only language, script, and region are supported across all platforms.
    // The script tag is mapped to the modifier for POSIX platforms.
    // The script tag takes precedence, if a modifier is also specified.
    //
    // The following tag syntax is accepted:
    //   BCP47:    language[-script][-region][-extension]
    //   Windows:  language[-script][-region][-extension][_sortorder]
    //   POSIX:    language[_region][.charset][@modifier]
    //   macOS:    language[-script][_region]
    //   MSVC CRT: language[_region][.codepage]

    wxLocaleIdent locId;

    // 0. Check for special locale identifiers "C" and "POSIX"
    if (IsDefaultCLocale(tag))
    {
        locId.Language(tag);
        return locId;
    }

    // 1. Handle platform-dependent cases

    // 1a. Check for modifier in POSIX tag
    wxString tagRest;
    wxString tagMain = tag.BeforeFirst('@', &tagRest);
    if (!tagRest.empty())
    {
        // POSIX modifier found
        wxString script = wxUILocale::GetScriptNameFromAlias(tagRest);
        if (!script.empty())
            locId.Script(script);
        else
            locId.Modifier(tagRest);
    }

    // 1b. Check for charset in POSIX tag
    tagMain = tagMain.BeforeFirst('.', &tagRest);
    if (!tagRest.empty())
    {
        // POSIX charset found
        locId.Charset(tagRest);
    }

    // 1c. Check for Windows CRT language and region names
    {
        // The tag is potentially a Windows CRT language/region name,
        // if language and region part both have a length greater 3
        // (that is, they are not given as ISO codes)
        wxString tagTemp = tagMain.BeforeFirst('_', &tagRest);
        if (tagTemp.length() > 3 && (tagRest.empty() || tagRest.length() > 3))
        {
            const wxLanguageInfo* const info = wxUILocale::FindLanguageInfo(tagMain);
            if (info)
            {
                tagMain = info->LocaleTag;
            }
        }
    }

    // 1d. Check for sort order in Windows tag
    //
    // Make sure we don't extract the region identifier erroneously as a sortorder identifier
    {
        wxString tagTemp = tagMain.BeforeLast('_', &tagRest);
        if (!tagTemp.empty() &&
                tagRest.length() > 4 &&
                    locId.m_modifier.empty() &&
                        locId.m_charset.empty())
        {
            // Windows sortorder found
            locId.SortOrder(tagRest);
            tagMain = tagTemp;
        }
    }

    // 2. Handle remaining tag identifier as being BCP47-like

    // Now that special POSIX attributes have been handled
    // POSIX specific delimiters must no longer be present

    // Replace '_' separators by '-' to simplify further processing
    tagMain.Replace("_", "-");

    const wxArrayString& parts = wxSplit(tagMain, '-', '\0');
    wxArrayString::const_iterator it = parts.begin();
    if ( it == parts.end() )
        return wxLocaleIdent();

    // We have at least the language, so we'll return a valid object.
    locId.m_language = (*it).Lower();

    // Also store the full string.
    locId.m_tag = tag;

    if ( ++it == parts.end() )
        return locId;

    // Advance to the next component we know about.
    switch ( locId.m_language.length() )
    {
        case 2:
        case 3:
            // Looks like an ISO 639 code.
            break;

        default:
            // It may be private use or grandfathered tag or just invalid
            // syntax, but in any case we can't parse it further.
            return locId;
    }

    // Skip extlangs that are 3 letters long, in contrast to 3 digit region
    // codes.
    while ( it->length() == 3 && !isdigit((*it)[0]) )
    {
        if ( ++it == parts.end() )
            return locId;
    }

    switch ( it->length() )
    {
        case 2:
        case 3:
            // Either an ISO 3166-1 or UN M.49 region code.
            locId.m_region = (*it).Upper();
            break;

        case 4:
            // Must be an ISO 15924 script.
            locId.m_script = it->Capitalize();
            break;

        default:
            // This looks to be completely invalid.
            return wxLocaleIdent();
    }

    // Check whether we have got the region above.
    // If not, we must have got the script. So, check if we have the region, too.
    if (++it == parts.end())
    {
        CheckLanguageVariant(locId);
        return locId;
    }

    if (locId.m_region.empty())
    {
        switch (it->length())
        {
            case 2:
            case 3:
                locId.m_region = (*it).Upper(); ++it;
                break;
        }
    }

    // If there is still anything to parse (variants, extensions, private use),
    // we assign it to the extension.
    if (it != parts.end())
    {
        wxString custom = *it;
        while (++it != parts.end())
        {
            custom << "-" << *it;
        }
        locId.m_extension = custom;
    }

    // We also handle the only language variant known at the time of writing:
    // valencia (ca-ES-valencia resp ca_ES@valencia).
    CheckLanguageVariant(locId);

    return locId;
}

wxLocaleIdent& wxLocaleIdent::Language(const wxString& language)
{
    if (IsDefaultCLocale(language))
    {
        m_language = language.Upper();
    }
    else if ((language.length() == 2 || language.length() == 3) &&
             language.find_first_not_of(validCharsAlpha) == wxString::npos)
    {
        m_language = language.Lower();
    }
    else
    {
        m_language.clear();
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Region(const wxString& region)
{
    if ((region.length() == 2 || region.length() == 3) &&
        region.find_first_not_of(validCharsAlnum) == wxString::npos)
    {
        m_region = region.Upper();
    }
    else
    {
        m_region.clear();
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Script(const wxString& script)
{
    if (script.length() == 4 &&
        script.find_first_not_of(validCharsAlpha) == wxString::npos)
    {
        // Capitalize first character
        m_script = script.Capitalize();
    }
    else if (!script.empty())
    {
        m_script = wxUILocale::GetScriptNameFromAlias(script.Lower());
    }
    else
    {
        m_script.clear();
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Charset(const wxString& charset)
{
    if (charset.find_first_not_of(validCharsModExt) == wxString::npos)
    {
        m_charset = charset;
    }
    else
    {
        m_charset.clear();
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Modifier(const wxString& modifier)
{
    if (modifier.find_first_not_of(validCharsModExt) == wxString::npos)
    {
        m_modifier = modifier;
    }
    else
    {
        m_modifier.clear();
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Extension(const wxString& extension)
{
    // Windows extensions follow the BCP 47 syntax
    if (extension.find_first_not_of(validCharsModExt) == wxString::npos)
    {
        m_extension = extension;
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::SortOrder(const wxString& sortorder)
{
    // Windows sortorder identifiers all seem to have a length of 6 characters.
    // To distinguish sortorder from script and region identifiers require length > 4.
    if (sortorder.length() > 4 &&
        sortorder.find_first_not_of(validCharsAlpha) == wxString::npos)
    {
        m_sortorder = sortorder;
    }
    return *this;
}

wxString wxLocaleIdent::GetTag(wxLocaleTagType tagType) const
{
    if (tagType == wxLOCALE_TAGTYPE_DEFAULT && !m_tag.empty() )
        return m_tag;

    wxString tag = m_language;
    switch (tagType)
    {
        case wxLOCALE_TAGTYPE_BCP47:
            if (!m_script.empty())
                tag << '-' << m_script;
            if (!m_region.empty())
                tag << '-' << m_region;
            if (!m_extension.empty())
                tag << '-' << m_extension;
            break;

        case wxLOCALE_TAGTYPE_MACOS:
            if (!m_script.empty())
                tag << '-' << m_script;
            if (!m_region.empty())
                tag << '_' << m_region;
            break;

        case wxLOCALE_TAGTYPE_POSIX:
            if (!m_region.empty())
                tag << '_' << m_region;
            if (!m_charset.empty())
                tag << '.' << m_charset;
            if (!m_script.empty())
                tag << '@' << wxUILocale::GetScriptAliasFromName(m_script);
            else if (!m_modifier.empty())
                tag << '@' << m_modifier;
            break;

        case wxLOCALE_TAGTYPE_WINDOWS:
            if (!m_script.empty())
                tag << '-' << m_script;
            if (!m_region.empty())
                tag << '-' << m_region;
            if (!m_extension.empty())
                tag << '-' << m_extension;
            if (!m_sortorder.empty())
                tag << '-' << m_sortorder;
            break;

        case wxLOCALE_TAGTYPE_SYSTEM:
        default:
            tag = GetName();
            break;
    }

    return tag;
}

// ----------------------------------------------------------------------------
// wxUILocale
// ----------------------------------------------------------------------------

#ifndef __WINDOWS__

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLanguage(const wxLanguageInfo& info)
{
    wxLocaleIdent locId;

    // Strings in our language database are of the form "lang[_region[@mod]]".
    wxString rest;
    locId.Language(info.CanonicalName.BeforeFirst('_', &rest));

    if ( !rest.empty() )
    {
        wxString mod;
        locId.Region(rest.BeforeFirst('@', &mod));

        if ( !mod.empty() )
            locId.Modifier(mod);
    }

    wxUILocaleImpl* impl = CreateForLocale(locId);
    if ( !impl &&
            (info.Language == wxLANGUAGE_ENGLISH ||
             info.Language == wxLANGUAGE_ENGLISH_US) )
    {
        // For compatibility, never fail creating locale for neutral or US
        // English, even if it's unavailable on the current system somehow.
        impl = CreateStdC();
    }

    return impl;
}

#endif // !__WINDOWS__

/* static */
bool wxUILocale::UseDefault()
{
    // We don't attempt to optimize this function by checking whether
    // ms_current is already set to the user default locale, as we're
    // supposed to be called just once during the program lifetime anyhow.

    wxUILocaleImpl* const impl = wxUILocaleImpl::CreateUserDefault();
    if ( !impl )
        return false;

    impl->Use();
    ms_current = wxUILocale(impl);

    return true;
}

/* static */
bool wxUILocale::UseLocaleName(const wxString& localeName)
{
    wxUILocaleImpl* impl = nullptr;
    if (IsDefaultCLocale(localeName))
    {
        impl = wxUILocaleImpl::CreateStdC();
    }
    else
    {
        wxLocaleIdent localeId = wxLocaleIdent::FromTag(localeName);
        impl = wxUILocaleImpl::CreateForLocale(localeId);
        if (!impl)
        {
            // Creating the locale may have failed due to lacking support for wxUILocaleImplName
            // Try to locate the locale in our language database
            const wxLanguageInfo* const info = wxUILocale::FindLanguageInfo(localeId);
            if (info)
            {
                // Language found in language database
                // Try to create a locale based on the language
                impl = wxUILocaleImpl::CreateForLanguage(*info);
            }
        }
    }
    if (!impl)
        return false;

    impl->Use();
    ms_current = wxUILocale(impl);

    return true;
}

/* static */
const wxUILocale& wxUILocale::GetCurrent()
{
    // We initialize it on demand.
    if ( !ms_current.m_impl )
    {
        ms_current = wxUILocale(wxUILocaleImpl::CreateStdC());
    }

    return ms_current;
}

wxUILocale::wxUILocale(const wxLocaleIdent& localeId)
{
    if ( localeId.IsEmpty() )
    {
        wxFAIL_MSG( "Locale identifier must be initialized" );
        m_impl = nullptr;
        return;
    }

    if (IsDefaultCLocale(localeId.GetLanguage()))
    {
        m_impl = wxUILocaleImpl::CreateStdC();
    }
    else
    {
        m_impl = wxUILocaleImpl::CreateForLocale(localeId);
    }
}

wxUILocale::wxUILocale(const wxUILocale& loc)
{
    m_impl = loc.m_impl;
    if ( m_impl )
        m_impl->IncRef();
}

wxUILocale& wxUILocale::operator=(const wxUILocale& loc)
{
    if ( m_impl )
        m_impl->DecRef();

    m_impl = loc.m_impl;
    if ( m_impl )
        m_impl->IncRef();

    return *this;
}

bool wxUILocale::IsSupported() const
{
    return m_impl != nullptr;
}

wxString wxUILocale::GetName() const
{
    if ( !m_impl )
        return wxString();

    return m_impl->GetName();
}

wxLocaleIdent wxUILocale::GetLocaleId() const
{
    if (!m_impl)
        return wxLocaleIdent();

    return m_impl->GetLocaleId();
}

wxString wxUILocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
    if ( !m_impl )
        return wxGetStdCLocaleInfo(index, cat);

    return m_impl->GetInfo(index, cat);
}

wxString wxUILocale::GetLocalizedName(wxLocaleName name, wxLocaleForm form) const
{
    if (!m_impl)
        return wxString();

    return m_impl->GetLocalizedName(name, form);
}

#if wxUSE_DATETIME
wxString wxUILocale::GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form) const
{
    if (!m_impl)
        return wxString();

    return m_impl->GetMonthName(month, form);
}

wxString wxUILocale::GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form) const
{
    if (!m_impl)
        return wxString();

    return m_impl->GetWeekDayName(weekday, form);
}
#endif // wxUSE_DATETIME

wxLayoutDirection wxUILocale::GetLayoutDirection() const
{
    if (!m_impl)
        return wxLayout_Default;

    wxLayoutDirection dir = m_impl->GetLayoutDirection();
    if (dir == wxLayout_Default)
    {
        wxLocaleIdent localeId = m_impl->GetLocaleId();
        if (!localeId.IsEmpty())
        {
            const wxLanguageInfo* const info = wxUILocale::FindLanguageInfo(localeId);
            if (info)
            {
                dir = info->LayoutDirection;
            }
        }
    }
    return dir;
}

int
wxUILocale::CompareStrings(const wxString& lhs,
                           const wxString& rhs,
                           int flags) const
{
    if ( !m_impl )
    {
        const int rc = flags & wxCompare_CaseInsensitive ? lhs.CmpNoCase(rhs)
                                                         : lhs.Cmp(rhs);
        if ( rc < 0 )
            return -1;
        if ( rc > 0 )
            return 1;
        return 0;
    }

    return m_impl->CompareStrings(lhs, rhs, flags);
}

wxUILocale::~wxUILocale()
{
    if ( m_impl )
        m_impl->DecRef();
}


/* static */
wxLocaleIdent wxUILocale::GetSystemLocaleId()
{
    wxUILocale defaultLocale(wxUILocaleImpl::CreateUserDefault());
    return defaultLocale.GetLocaleId();
}

/*static*/
int wxUILocale::GetSystemLanguage()
{
    const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
    size_t count = languagesDB.size();
    wxVector<wxString> preferred = wxUILocaleImpl::GetPreferredUILanguages();

    for (wxVector<wxString>::const_iterator j = preferred.begin();
        j != preferred.end();
        ++j)
    {
        wxLocaleIdent localeId = wxLocaleIdent::FromTag(*j);
        wxString lang = localeId.GetTag(wxLOCALE_TAGTYPE_BCP47);
        size_t pos = lang.find('-');
        wxString langShort = (pos != wxString::npos) ? lang.substr(0, pos) : wxString();
        size_t ixShort = count;

        for (size_t ixLanguage = 0; ixLanguage < count; ++ixLanguage)
        {
            if (languagesDB[ixLanguage].LocaleTag == lang)
            {
                return languagesDB[ixLanguage].Language;
            }
            if (pos != wxString::npos)
            {
                if (languagesDB[ixLanguage].LocaleTag == langShort)
                {
                    ixShort = ixLanguage;
                }
            }
        }
        if (ixShort < count)
        {
            return languagesDB[ixShort].Language;
        }
    }

    // no info about the preferred UI language in the database
    // fall back to default locale
    return GetSystemLocale();
}

/*static*/
int wxUILocale::GetSystemLocale()
{
    const wxLocaleIdent locId = GetSystemLocaleId();

    // Find wxLanguageInfo corresponding to the default locale.
    const wxLanguageInfo* defaultLanguage = wxUILocale::FindLanguageInfo(locId);

    // Check if it really corresponds to this locale: we could find it via the
    // fallback on the language, which is something that it generally makes
    // sense for FindLanguageInfo() to do, but in this case we really need the
    // locale.
    if ( defaultLanguage )
    {
        // We have to handle the "C" locale specially as its name is different
        // from the "en-US" tag found for it, but we do still want to return
        // English for it.
        const wxString tag = locId.GetTag(wxLOCALE_TAGTYPE_BCP47);
        if ( tag == defaultLanguage->LocaleTag || IsDefaultCLocale(tag) )
            return defaultLanguage->Language;
    }

    return wxLANGUAGE_UNKNOWN;
}

/* static */
wxVector<wxString> wxUILocale::GetPreferredUILanguages()
{
    return wxUILocaleImpl::GetPreferredUILanguages();
}

/* static */
const wxLanguageInfo* wxUILocale::GetLanguageInfo(int lang)
{
    CreateLanguagesDB();

    // calling GetLanguageInfo(wxLANGUAGE_DEFAULT) is a natural thing to do, so
    // make it work
    if (lang == wxLANGUAGE_DEFAULT)
        lang = GetSystemLanguage();

    if (lang == wxLANGUAGE_UNKNOWN)
        return nullptr;

    const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
    const size_t count = languagesDB.size();
    for (size_t i = 0; i < count; i++)
    {
        if (languagesDB[i].Language == lang)
            return &languagesDB[i];
    }

    return nullptr;
}

/* static */
wxString wxUILocale::GetLanguageName(int lang)
{
    wxString string;

    if (lang == wxLANGUAGE_DEFAULT || lang == wxLANGUAGE_UNKNOWN)
        return string;

    const wxLanguageInfo* info = GetLanguageInfo(lang);
    if (info)
        string = info->Description;

    return string;
}

/* static */
wxString wxUILocale::GetLanguageCanonicalName(int lang)
{
    wxString string;

    if (lang == wxLANGUAGE_DEFAULT || lang == wxLANGUAGE_UNKNOWN)
        return string;

    const wxLanguageInfo* info = GetLanguageInfo(lang);
    if (info)
        string = info->CanonicalName;

    return string;
}

/* static */
const wxLanguageInfo* wxUILocale::FindLanguageInfo(const wxString& localeOrig)
{
    if (localeOrig.empty())
        return nullptr;

    CreateLanguagesDB();

    // Determine full language and region names, which will be compared
    // to the entry description in the language database.
    // The locale string may have the form "language[_region][.codeset]".
    // We ignore the "codeset" part here.
    wxString locale = localeOrig;
    if (IsDefaultCLocale(locale))
    {
        locale = "en_US";
    }
    wxString region;
    wxString languageOnly = locale.BeforeFirst('.').BeforeFirst('_', &region);
    wxString language = languageOnly;
    if (!region.empty())
    {
        // Construct description consisting of language and region
        language << " (" << region << ")";
    }

    const wxLanguageInfo* infoRet = nullptr;

    const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
    const size_t count = languagesDB.size();
    for (size_t i = 0; i < count; i++)
    {
        const wxLanguageInfo* info = &languagesDB[i];

        if (wxStricmp(locale, info->CanonicalName) == 0 ||
            wxStricmp(language, info->Description) == 0)
        {
            // exact match, stop searching
            infoRet = info;
            break;
        }

        if (wxStricmp(locale, info->CanonicalName.BeforeFirst(wxS('_'))) == 0 ||
            wxStricmp(languageOnly, info->Description) == 0)
        {
            // a match -- but maybe we'll find an exact one later, so continue
            // looking
            //
            // OTOH, maybe we had already found a language match and in this
            // case don't overwrite it because the entry for the default
            // country always appears first in gs_languagesDB
            if (!infoRet)
                infoRet = info;
        }
    }

    return infoRet;
}

/* static */
const wxLanguageInfo* wxUILocale::FindLanguageInfo(const wxLocaleIdent& locId)
{
    if (locId.IsEmpty())
        return nullptr;

    CreateLanguagesDB();

    const wxLanguageInfo* infoRet = nullptr;

    wxString lang = locId.GetLanguage();
    wxString localeTag = locId.GetTag(wxLOCALE_TAGTYPE_BCP47);
    if (IsDefaultCLocale(lang))
    {
        lang = wxS("en");
        localeTag = "en-US";
    }

    const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
    const size_t count = languagesDB.size();
    for (size_t i = 0; i < count; i++)
    {
        const wxLanguageInfo* info = &languagesDB[i];

        if (wxStricmp(localeTag, info->LocaleTag) == 0)
        {
            // exact match, stop searching
            infoRet = info;
            break;
        }

        if (wxStricmp(lang, info->LocaleTag.BeforeFirst(wxS('-'))) == 0)
        {
            // a match -- but maybe we'll find an exact one later, so continue
            // looking
            //
            // OTOH, maybe we had already found a language match and in this
            // case don't overwrite it because the entry for the default
            // country always appears first in gs_languagesDB
            if (!infoRet)
                infoRet = info;
        }
    }

    return infoRet;
}

#if wxUSE_DATETIME
int wxUILocaleImpl::ArrayIndexFromFlag(wxDateTime::NameFlags flags)
{
    switch (flags)
    {
        case wxDateTime::Name_Full:
            return 0;

        case wxDateTime::Name_Abbr:
            return 1;

        case wxDateTime::Name_Shortest:
            return 2;

        default:
            wxFAIL_MSG("unknown wxDateTime::NameFlags value");
    }

    return -1;
}
#endif // wxUSE_DATETIME

#endif // wxUSE_INTL
