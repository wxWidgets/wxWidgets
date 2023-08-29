///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/uilocale.mm
// Purpose:     wxUILocale implementation for macOS
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

#include "wx/log.h"
#include "wx/uilocale.h"
#include "wx/private/uilocale.h"

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>
#import <Foundation/NSLocale.h>
#import <Foundation/NSDateFormatter.h>

extern wxString
wxGetInfoFromCFLocale(CFLocaleRef cfloc, wxLocaleInfo index, wxLocaleCategory cat);

// ----------------------------------------------------------------------------
// wxLocaleIdent::GetName() implementation using Foundation
// ----------------------------------------------------------------------------

wxString wxLocaleIdent::GetName() const
{
    // Construct name in right format:
    // MacOS: <language>-<script>_<REGION>

    wxString name;
    if ( !m_language.empty() )
    {
        name << m_language;

        if ( !m_script.empty() )
        {
            name << "-" << m_script;
        }

        if ( !m_region.empty() )
        {
            name << "_" << m_region;
        }
    }

    return name;
}

namespace
{

// ----------------------------------------------------------------------------
// wxUILocale implementation using Core Foundation
// ----------------------------------------------------------------------------

class wxUILocaleImplCF : public wxUILocaleImpl
{
public:
    explicit wxUILocaleImplCF(NSLocale* nsloc)
        : m_nsloc([nsloc retain])
    {
    }

    ~wxUILocaleImplCF() override
    {
        [m_nsloc release];
    }

    static wxUILocaleImplCF* Create(const wxLocaleIdent& locId)
    {
        // Surprisingly, localeWithLocaleIdentifier: always succeeds, even for
        // completely invalid strings, so we need to check if the name is
        // actually in the list of the supported locales ourselves.
        bool isAvailable = false;
        for ( id nsLocId in [NSLocale availableLocaleIdentifiers] )
        {
            // We can't simply compare the names here because the list returned
            // by NSLocale is incomplete and doesn't contain all synonyms, e.g.
            // it only contains "zh_Hant_TW" but not "zh_TW" itself, so we need
            // to do our own matching.
            auto strId = wxCFStringRef::AsString(nsLocId);

            const auto availId = wxLocaleIdent::FromTag(strId);
            if ( availId.IsEmpty() )
            {
                wxLogDebug("Failed to parse locale identifier \"%s\"", strId);
                continue;
            }

            // The following conditions have to be checked:
            //   - The language must always match.
            //   - The script must match, if it given, otherwise the region must
            //     match (even though it might be empty).
            //   - If script and region are both given, they must both match.
            if ( availId.GetLanguage() == locId.GetLanguage() )
            {
                if ( !locId.GetScript().empty() )
                {
                    isAvailable = availId.GetScript() == locId.GetScript();
                    if ( isAvailable && !locId.GetRegion().empty() )
                    {
                        isAvailable = availId.GetRegion() == locId.GetRegion();
                    }
                }
                else
                {
                    isAvailable = availId.GetRegion() == locId.GetRegion();
                }
            }
            if ( isAvailable )
                break;
        }

        if ( !isAvailable )
            return nullptr;

        wxCFStringRef cfName(locId.GetName());
        auto nsloc = [NSLocale localeWithLocaleIdentifier: cfName.AsNSString()];
        if ( !nsloc )
            return nullptr;

        return new wxUILocaleImplCF(nsloc);
    }

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
    NSLocale* const m_nsloc;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplCF);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

void
wxUILocaleImplCF::Use()
{
    // There is no way to start using a locale other than default, so there is
    // nothing to do here.
}

wxString
wxUILocaleImplCF::GetName() const
{
    wxString name = wxCFStringRef::AsString([m_nsloc localeIdentifier]);

    // Check for the special case of the "empty" system locale, see CreateStdC()
    if ( name.empty() || name.IsSameAs("en_US_POSIX") )
        name = "C";

    return name;
}

wxLocaleIdent
wxUILocaleImplCF::GetLocaleId() const
{
    return wxLocaleIdent::FromTag(GetName());
}

wxString
wxUILocaleImplCF::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
    return wxGetInfoFromCFLocale((CFLocaleRef)m_nsloc, index, cat);
}

wxString
wxUILocaleImplCF::GetLocalizedName(wxLocaleName name, wxLocaleForm form) const
{
    NSLocale* convLocale = nullptr;
    switch (form)
    {
        case wxLOCALE_FORM_NATIVE:
            convLocale = m_nsloc;
            break;
        case wxLOCALE_FORM_ENGLISH:
            convLocale = [[[NSLocale alloc] initWithLocaleIdentifier:@"en_US"] autorelease];
            break;
        default:
            wxFAIL_MSG("unknown wxLocaleForm");
            return wxString();
    }

    NSString* str = nullptr;
    switch (name)
    {
        case wxLOCALE_NAME_LOCALE:
            str = [convLocale localizedStringForLocaleIdentifier:[m_nsloc localeIdentifier]];
            break;
        case wxLOCALE_NAME_LANGUAGE:
            str = [convLocale localizedStringForLanguageCode:[m_nsloc languageCode]];
            break;
        case wxLOCALE_NAME_COUNTRY:
            str = [convLocale localizedStringForCountryCode:[m_nsloc countryCode]];
            break;
    }
    return wxCFStringRef::AsString(str);
}

wxString
wxUILocaleImplCF::GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form) const
{
    NSDateFormatter* df = [NSDateFormatter new];
    df.locale = m_nsloc;

    NSArray* monthNames = nullptr;

    if (form.GetContext() == wxDateTime::Context_Standalone)
    {
        switch ( form.GetFlags() )
        {
            case wxDateTime::Name_Shortest:
                monthNames = [df veryShortStandaloneMonthSymbols];
                break;
            case wxDateTime::Name_Abbr:
                monthNames = [df shortStandaloneMonthSymbols];
                break;
            case wxDateTime::Name_Full:
            default:
                monthNames = [df standaloneMonthSymbols];
                break;
        }
    }
    else
    {
        switch ( form.GetFlags() )
        {
            case wxDateTime::Name_Shortest:
                monthNames = [df veryShortMonthSymbols];
                break;
            case wxDateTime::Name_Abbr:
                monthNames = [df shortMonthSymbols];
                break;
            case wxDateTime::Name_Full:
            default:
                monthNames = [df monthSymbols];
                break;
        }
    }

    NSString* monthName = [monthNames objectAtIndex:(month)];
    return wxCFStringRef::AsString(monthName);
}

wxString
wxUILocaleImplCF::GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form) const
{
    NSDateFormatter* df = [NSDateFormatter new];
    df.locale = m_nsloc;

    NSArray* weekdayNames = nullptr;

    if (form.GetContext() == wxDateTime::Context_Standalone)
    {
        switch ( form.GetFlags() )
        {
            case wxDateTime::Name_Shortest:
                weekdayNames = [df veryShortStandaloneWeekdaySymbols];
                break;
            case wxDateTime::Name_Abbr:
                weekdayNames = [df shortStandaloneWeekdaySymbols];
                break;
            case wxDateTime::Name_Full:
            default:
                weekdayNames = [df standaloneWeekdaySymbols];
                break;
        }
    }
    else
    {
        switch ( form.GetFlags() )
        {
            case wxDateTime::Name_Shortest:
                weekdayNames = [df veryShortWeekdaySymbols];
                break;
            case wxDateTime::Name_Abbr:
                weekdayNames = [df shortWeekdaySymbols];
                break;
            case wxDateTime::Name_Full:
            default:
                weekdayNames = [df weekdaySymbols];
                break;
        }
    }

    NSString* weekdayName = [weekdayNames objectAtIndex:(weekday)];
    return wxCFStringRef::AsString(weekdayName);
}

wxLayoutDirection
wxUILocaleImplCF::GetLayoutDirection() const
{
    NSLocaleLanguageDirection layoutDirection = [NSLocale characterDirectionForLanguage:[m_nsloc languageCode]];
    if (layoutDirection == NSLocaleLanguageDirectionLeftToRight)
        return wxLayout_LeftToRight;
    else if (layoutDirection == NSLocaleLanguageDirectionRightToLeft)
        return wxLayout_RightToLeft;
    return wxLayout_Default;
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    // This is an "empty" locale, but it seems to correspond rather well to the
    // "C" locale under POSIX systems and using localeWithLocaleIdentifier:@"C"
    // wouldn't be much better as we'd still need a hack for it in GetName()
    // because the locale names are always converted to lower case, while we
    // really want to return "C" rather than "c" as the name of this one.
    return new wxUILocaleImplCF([NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"]);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplCF([NSLocale currentLocale]);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locId)
{
    return wxUILocaleImplCF::Create(locId);
}

/* static */
wxVector<wxString> wxUILocaleImpl::GetPreferredUILanguages()
{
    wxVector<wxString> preferred;
    NSArray* preferredLangs = [NSLocale preferredLanguages];
    NSUInteger count = preferredLangs.count;

    for (NSUInteger j = 0; j < count; ++j)
        preferred.push_back(wxCFStringRef::AsString(preferredLangs[j]));

    return preferred;
}

int
wxUILocaleImplCF::CompareStrings(const wxString& lhs, const wxString& rhs,
                                 int flags) const
{
    const wxCFStringRef cfstr(lhs);
    auto ns_lhs = cfstr.AsNSString();

    NSInteger options = 0;
    if ( flags & wxCompare_CaseInsensitive )
        options |= NSCaseInsensitiveSearch;

    NSComparisonResult ret = [ns_lhs compare:wxCFStringRef(rhs).AsNSString()
                                     options:options
                                     range:(NSRange){0, [ns_lhs length]}
                                     locale:m_nsloc];

    switch (ret)
    {
        case NSOrderedAscending:
            return -1;
        case NSOrderedSame:
            return 0;
        case NSOrderedDescending:
            return 1;
    }

    return 0;
}

#endif // wxUSE_INTL
