///////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/uilocale.cpp
// Purpose:     wxUILocale implementation for classic Mac OS (Retro68)
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_INTL

#include "wx/uilocale.h"
#include "wx/private/uilocale.h"

#include "wx/intl.h"

#if wxUSE_DATETIME
    #include "wx/datetime.h"
#endif

namespace
{

// ----------------------------------------------------------------------------
// wxUILocaleImplRetro68: minimal implementation using fixed "C" locale data
// ----------------------------------------------------------------------------

// Classic Mac OS has International Utilities which could be used to retrieve
// the date and number formats, but for now just use the standard "C" locale
// conventions, which is enough for most purposes.
class wxUILocaleImplRetro68 : public wxUILocaleImpl
{
public:
    explicit wxUILocaleImplRetro68(const wxLocaleIdent& locId = wxLocaleIdent())
        : m_locId(locId)
    {
    }

    void Use() override
    {
    }

    wxString GetName() const override
    {
        return m_locId.IsEmpty() ? wxString("C") : m_locId.GetName();
    }

    wxLocaleIdent GetLocaleId() const override
    {
        return m_locId;
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const override
    {
        return wxGetStdCLocaleInfo(index, cat);
    }

    wxString GetLocalizedName(wxLocaleName name,
                              wxLocaleForm WXUNUSED(form)) const override
    {
        switch ( name )
        {
            case wxLOCALE_NAME_LOCALE:
                return "English (United States)";

            case wxLOCALE_NAME_LANGUAGE:
                return "English";

            case wxLOCALE_NAME_COUNTRY:
                return "United States";
        }

        return wxString();
    }

#if wxUSE_DATETIME
    wxString GetMonthName(wxDateTime::Month month,
                          wxDateTime::NameForm form) const override
    {
        return wxDateTime::GetEnglishMonthName(month, form);
    }

    wxString GetWeekDayName(wxDateTime::WeekDay weekday,
                            wxDateTime::NameForm form) const override
    {
        return wxDateTime::GetEnglishWeekDayName(weekday, form);
    }
#endif // wxUSE_DATETIME

    wxLayoutDirection GetLayoutDirection() const override
    {
        return wxLayout_LeftToRight;
    }

    wxLocaleNumberFormatting GetNumberFormatting() const override
    {
        return wxLocaleNumberFormatting(wxString(), std::vector<int>(), ".", 0);
    }

    wxString GetCurrencySymbol() const override
    {
        return "$";
    }

    wxString GetCurrencyCode() const override
    {
        return "USD";
    }

    wxCurrencySymbolPosition GetCurrencySymbolPosition() const override
    {
        return wxCurrencySymbolPosition::PrefixNoSep;
    }

    wxLocaleCurrencyInfo GetCurrencyInfo() const override
    {
        return wxLocaleCurrencyInfo(GetCurrencySymbol(), GetCurrencyCode(),
                                    GetCurrencySymbolPosition(),
                                    wxLocaleNumberFormatting(",", {3}, ".", 2));
    }

    wxMeasurementSystem UsesMetricSystem() const override
    {
        return wxMeasurementSystem::Unknown;
    }

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const override
    {
        const int rc = flags & wxCompare_CaseInsensitive ? lhs.CmpNoCase(rhs)
                                                         : lhs.Cmp(rhs);
        return rc < 0 ? -1 : rc > 0 ? 1 : 0;
    }

private:
    const wxLocaleIdent m_locId;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplRetro68);
};

} // anonymous namespace

// ============================================================================
// wxLocaleIdent and wxUILocaleImpl implementation
// ============================================================================

wxString wxLocaleIdent::GetName() const
{
    wxString name;
    if ( !m_language.empty() )
        name = GetTag(wxLOCALE_TAGTYPE_POSIX);

    return name;
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return new wxUILocaleImplRetro68();
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplRetro68(wxLocaleIdent().Language("en").Region("US"));
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locId)
{
    return new wxUILocaleImplRetro68(locId);
}

/* static */
wxVector<wxString> wxUILocaleImpl::GetPreferredUILanguages()
{
    wxVector<wxString> preferred;
    preferred.push_back("en_US");

    return preferred;
}

#endif // wxUSE_INTL
