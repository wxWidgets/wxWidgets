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
const char* validCharsTag = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.@";

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
    //   BCP47:   language[-script][-region][-extension]
    //   Windows: language[-script][-region][-extension][_sortorder]
    //   POSIX:   language[_region][.charset][@modifier]
    //   macOS:   language[-script][_region]

    // Locale tags must always use alphanumeric characters and certain special characters "-_.@"
    if (tag.find_first_not_of(validCharsTag) != wxString::npos)
    {
        wxFAIL_MSG("tag contains invalid characters (not alphanumeric) or invalid delimiters");
        return wxLocaleIdent();
    }

    wxLocaleIdent locId;

    // 1. Handle platform-dependent cases

    // 1a. Check for modifier in POSIX tag
    bool posixFound = false;
    wxString tagRest;
    wxString tagMain = tag.BeforeFirst('@', &tagRest);
    if (!tagRest.empty())
    {
        // POSIX modifier found
        posixFound = true;
        wxString script = wxUILocale::GetScriptNameFromAlias(tagRest);
        if (!script.empty())
            locId.Script(script);
        else
            locId.Modifier(tagRest);
    }
    else
    {
        wxASSERT_MSG(tag.find('@') == wxString::npos, "modifier delimiter found, but modifier is empty");
    }

    // 1b. Check for charset in POSIX tag
    tagMain = tagMain.BeforeFirst('.', &tagRest);
    if (!tagRest.empty())
    {
        // POSIX charset found
        posixFound = true;
        locId.Charset(tagRest);
    }
    else
    {
        wxASSERT_MSG(tagMain.find('.') == wxString::npos, "charset delimiter found, but charset is empty");
    }

    // 1c. Check for sort order in Windows tag
    //
    // Make sure we don't extract the region identifier erroneously as a sortorder identifier
    wxString tagTemp = tagMain.BeforeFirst('_', &tagRest);
    if (tagRest.length() > 4 && locId.m_modifier.empty() && locId.m_charset.empty())
    {
        // Windows sortorder found
        locId.Sortorder(tagRest);
        tagMain = tagTemp;
    }

    // 2. Handle remaining tag identifier as being BCP47-like

    // Now that special POSIX attributes have been handled
    // POSIX specific delimiters must no longer be present
    wxASSERT_MSG(tagMain.find_first_of(".@") == wxString::npos, "tag contains invalid delimiters");

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
            wxASSERT_MSG(locId.m_script.empty(), "script already set");
            locId.m_script = (*it).Left(1).Upper() + (*it).Mid(2).Lower();
            break;

        default:
            // This looks to be completely invalid.
            wxFAIL_MSG("invalid code length, script or region code expected");
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
    if (language.IsSameAs("C", false) || language.IsSameAs("POSIX", false))
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
        m_language = wxEmptyString;
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
        m_region = wxEmptyString;
    }
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Script(const wxString& script)
{
    if (script.length() == 4 &&
        script.find_first_not_of(validCharsAlpha) == wxString::npos)
    {
        // Capitalize first character
        m_script = script.Left(1).Upper() + script.Mid(2).Lower();
    }
    else if (!script.empty())
    {
        m_script = wxUILocale::GetScriptNameFromAlias(script.Lower());
    }
    else
    {
        m_script = wxEmptyString;
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
        m_charset = wxEmptyString;
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
        m_modifier = wxEmptyString;
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

wxLocaleIdent& wxLocaleIdent::Sortorder(const wxString& sortorder)
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
bool wxUILocale::UseLanguage(const wxLanguageInfo& info)
{
    wxUILocaleImpl* const impl = wxUILocaleImpl::CreateForLanguage(info);
    if ( !impl )
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
        m_impl = NULL;
        return;
    }

    m_impl = wxUILocaleImpl::CreateForLocale(localeId);
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
    return m_impl != NULL;
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
            wxString lang = localeId.GetLanguage();
            if (localeId.GetRegion().empty())
            {
                lang << '-' << localeId.GetRegion();
            }
            const wxLanguageInfo* const info = wxUILocale::FindLanguageInfo(lang);
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

#endif // wxUSE_INTL
