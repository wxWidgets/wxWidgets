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

#ifndef __WINDOWS__
    #include "wx/language.h"
#endif

#include "wx/private/uilocale.h"

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
    // See section 2.01 of https://www.rfc-editor.org/rfc/bcp/bcp47.txt for the
    // full syntax. Here we fully support just the subset we're interested in:
    //
    //  - Normal language tags (not private use or grandfathered ones).
    //  - Only script and region, but not the extensions or extlangs.

    // Language tags must always use ASCII.
    if ( tag != tag.ToAscii() )
        return wxLocaleIdent();

    const wxArrayString& parts = wxSplit(tag, '-', '\0');
    wxArrayString::const_iterator it = parts.begin();
    if ( it == parts.end() )
        return wxLocaleIdent();

    // We have at least the language, so we'll return a valid object.
    wxLocaleIdent locId;
    locId.m_language = *it;

    // Also store the full string, so that the platforms that support BCP 47
    // natively can use it instead of reconstructing the string from our fields.
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
            locId.m_region = *it;
            break;

        case 4:
            // Must be an ISO 15924 script.
            locId.m_script = *it;
            break;

        default:
            // This looks to be completely invalid.
            return wxLocaleIdent();
    }

    // If we got the language and the region, we can't parse anything else
    // (variants, extensions, private use) anyhow.
    if ( !locId.m_region.empty() )
        return locId;

    // Otherwise we must have got the script above, so check if we have the
    // region too.
    if ( ++it == parts.end() )
        return locId;

    switch ( it->length() )
    {
        case 2:
        case 3:
            locId.m_region = *it;
            break;
    }

    return locId;
}

wxLocaleIdent& wxLocaleIdent::Language(const wxString& language)
{
    m_language = language;
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Region(const wxString& region)
{
    m_region = region;
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Script(const wxString& script)
{
    m_script = script;
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Charset(const wxString& charset)
{
    m_charset = charset;
    return *this;
}

wxLocaleIdent& wxLocaleIdent::Modifier(const wxString& modifier)
{
    m_modifier = modifier;
    return *this;
}

wxString wxLocaleIdent::GetTag() const
{
    if ( !m_tag.empty() )
        return m_tag;

    wxString tag = m_language;

    if ( !m_script.empty() )
        tag << '-' << m_script;

    if ( !m_region.empty() )
        tag << '-' << m_region;

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

wxString wxUILocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
    if ( !m_impl )
        return wxGetStdCLocaleInfo(index, cat);

    return m_impl->GetInfo(index, cat);
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
