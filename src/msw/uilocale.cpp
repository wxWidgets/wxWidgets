///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uilocale.cpp
// Purpose:     wxUILocale implementation for MSW
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
#include "wx/private/uilocale.h"

#include "wx/msw/private/uilocale.h"

#include "wx/scopedarray.h"
#include "wx/dynlib.h"

#ifndef LOCALE_NAME_USER_DEFAULT
    #define LOCALE_NAME_USER_DEFAULT nullptr
#endif

#ifndef LOCALE_NAME_MAX_LENGTH
    #define LOCALE_NAME_MAX_LENGTH   85
#endif

#ifndef MUI_LANGUAGE_NAME
    #define MUI_LANGUAGE_NAME 8
#endif

#ifndef LOCALE_ICONSTRUCTEDLOCALE
#define LOCALE_ICONSTRUCTEDLOCALE 0x0000007d
#endif

#ifndef LOCALE_RETURN_NUMBER
#define LOCALE_RETURN_NUMBER 0x20000000
#endif

#ifndef LOCALE_SENGLISHDISPLAYNAME
#define LOCALE_SENGLISHDISPLAYNAME    0x00000072
#endif

#ifndef LOCALE_SNATIVEDISPLAYNAME
#define LOCALE_SNATIVEDISPLAYNAME     0x00000073
#endif

#ifndef LOCALE_SENGLISHLANGUAGENAME
#define LOCALE_SENGLISHLANGUAGENAME   0x00001001
#endif

#ifndef LOCALE_SNATIVELANGUAGENAME
#define LOCALE_SNATIVELANGUAGENAME    0x00000004
#endif

#ifndef LOCALE_SENGLISHCOUNTRYNAME
#define LOCALE_SENGLISHCOUNTRYNAME    0x00001002
#endif

#ifndef LOCALE_SNATIVECOUNTRYNAME
#define LOCALE_SNATIVECOUNTRYNAME     0x00000008
#endif

#ifndef LOCALE_IREADINGLAYOUT
#define LOCALE_IREADINGLAYOUT         0x00000070
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{

// Trivial wrapper for ::SetThreadUILanguage().
//
// TODO-XP: Drop this when we don't support XP any longer.
static void wxMSWSetThreadUILanguage(LANGID langid)
{
    // SetThreadUILanguage() is available on XP, but with unclear
    // behavior, so avoid calling it there.
    if ( wxGetWinVersion() >= wxWinVersion_Vista )
    {
        wxLoadedDLL dllKernel32(wxS("kernel32.dll"));
        typedef LANGID(WINAPI *SetThreadUILanguage_t)(LANGID);
        SetThreadUILanguage_t pfnSetThreadUILanguage = nullptr;
        wxDL_INIT_FUNC(pfn, SetThreadUILanguage, dllKernel32);
        if (pfnSetThreadUILanguage)
            pfnSetThreadUILanguage(langid);
    }
}

} // anonymous namespace

void wxUseLCID(LCID lcid)
{
    ::SetThreadLocale(lcid);

    wxMSWSetThreadUILanguage(LANGIDFROMLCID(lcid));
}

// ----------------------------------------------------------------------------
// wxLocaleIdent::GetName() implementation for MSW
// ----------------------------------------------------------------------------

wxString wxLocaleIdent::GetName() const
{
    // Construct name in right format:
    // Windows: <language>-<script>-<REGION>

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
            name << "-" << m_region;
        }

        if (!m_extension.empty())
        {
            name << "-" << m_extension;
        }

        if (!m_sortorder.empty())
        {
            name << "_" << m_sortorder;
        }
    }

    return name;
}

// ----------------------------------------------------------------------------
// Standard C wxUILocale implementation for MSW
// ----------------------------------------------------------------------------

class wxUILocaleImplStdC : public wxUILocaleImpl
{
public:

    // Create object corresponding to the given locale, return nullptr if not
    // supported.
    static wxUILocaleImplStdC* Create()
    {
        return new wxUILocaleImplStdC();
    }


    ~wxUILocaleImplStdC() override
    {
    }

    void Use() override
    {
    }

    wxString GetName() const override
    {
        return wxString("C");
    }

    wxLocaleIdent GetLocaleId() const override
    {
        return wxLocaleIdent().Language("C");
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const override
    {
        return wxGetStdCLocaleInfo(index, cat);
    }

    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm WXUNUSED(form)) const override
    {
        wxString str;
        switch (name)
        {
            case wxLOCALE_NAME_LOCALE:
            case wxLOCALE_NAME_LANGUAGE:
                str = wxString("English");
                break;
            case wxLOCALE_NAME_COUNTRY:
                str = wxString();
                break;
            default:
                wxFAIL_MSG("unknown wxLocaleInfo");
        }

        return str;
    }

    wxLayoutDirection GetLayoutDirection() const override
    {
        return wxLayout_Default;
    }

    int CompareStrings(const wxString& lhs, const wxString& rhs,
        int flags) const override
    {
        const int rc = flags & wxCompare_CaseInsensitive ? lhs.CmpNoCase(rhs)
            : lhs.Cmp(rhs);
        if (rc < 0)
            return -1;
        if (rc > 0)
            return 1;
        return 0;
    }

private:
    // Ctor is private, use Create() instead.
    explicit wxUILocaleImplStdC()
    {
    }

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplStdC);
};

// ----------------------------------------------------------------------------
// LCID-based wxUILocale implementation for MSW
// ----------------------------------------------------------------------------

// TODO-XP: Replace with wxUILocaleImplName when we don't support XP any longer.
class wxUILocaleImplLCID : public wxUILocaleImpl
{
public:
    explicit wxUILocaleImplLCID(LCID lcid)
        : m_lcid(lcid)
    {
    }

    void Use() override
    {
        wxUseLCID(m_lcid);
    }

    wxString GetName() const override
    {
        wxString str;

        // Try using newer constant available since Vista which produces names
        // more similar to the other platforms.
        if ( wxGetWinVersion() >= wxWinVersion_Vista )
        {
            str = DoGetInfo(LOCALE_SNAME);
        }
        else // TODO-XP: Drop this branch.
        {
            // This name constant is available under all systems, including
            // pre-Vista ones.
            str = DoGetInfo(LOCALE_SENGLANGUAGE);
        }

        return str;
    }

    wxLocaleIdent GetLocaleId() const override
    {
        return wxLocaleIdent::FromTag(GetName());
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const override
    {
        return wxGetInfoFromLCID(m_lcid, index, cat);
    }

    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const override
    {
        wxString str;
        switch (name)
        {
            case wxLOCALE_NAME_LOCALE:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        {
                            wxString strLang = DoGetInfo(LOCALE_SNATIVELANGNAME);
                            wxString strCtry = DoGetInfo(LOCALE_SNATIVECTRYNAME);
                            str << strLang << " (" << strCtry << ")";
                        }
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        {
                            wxString strLang = DoGetInfo(LOCALE_SENGLANGUAGE);
                            wxString strCtry = DoGetInfo(LOCALE_SENGCOUNTRY);
                            str << strLang << " (" << strCtry << ")";
                        }
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
            case wxLOCALE_NAME_LANGUAGE:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        str = DoGetInfo(LOCALE_SNATIVELANGNAME);
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        str = DoGetInfo(LOCALE_SENGLANGUAGE);
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
            case wxLOCALE_NAME_COUNTRY:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        str = DoGetInfo(LOCALE_SNATIVECTRYNAME);
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        str = DoGetInfo(LOCALE_SENGCOUNTRY);
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
        }

        return str;
    }

    wxLayoutDirection GetLayoutDirection() const override
    {
        return wxLayout_Default;
    }

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const override
    {
        // Can't be really implemented on the OS versions where this class is
        // used.
        return flags & wxCompare_CaseInsensitive
                ? lhs.CmpNoCase(rhs)
                : lhs.Cmp(rhs) ;
    }

private:
    const LCID m_lcid;

    wxString DoGetInfo(LCTYPE lctype) const
    {
        wxChar buf[256];
        if (!::GetLocaleInfo(m_lcid, lctype, buf, WXSIZEOF(buf)))
        {
            wxLogLastError(wxT("GetLocaleInfo"));
            return wxString();
        }

        return buf;
    }

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplLCID);
};

// ----------------------------------------------------------------------------
// Name-based wxUILocale implementation for MSW
// ----------------------------------------------------------------------------

class wxUILocaleImplName : public wxUILocaleImpl
{
public:
    // TODO-XP: Get rid of this function and all the code branches handling the
    // return value of "false" from it and just always use this class.
    static bool CanUse()
    {
        static int s_canUse = -1;

        if ( s_canUse == -1 )
        {
            // One time only initialization.
            s_canUse = 0;

            wxLoadedDLL dllKernel32(wxS("kernel32.dll"));
            wxDL_INIT_FUNC(ms_, GetLocaleInfoEx, dllKernel32);
            if ( !ms_GetLocaleInfoEx )
                return false;
            wxDL_INIT_FUNC(ms_, SetThreadPreferredUILanguages, dllKernel32);
            if ( !ms_SetThreadPreferredUILanguages )
                return false;
            wxDL_INIT_FUNC(ms_, GetUserPreferredUILanguages, dllKernel32);
            if (!ms_GetUserPreferredUILanguages)
                return false;
            wxDL_INIT_FUNC(ms_, GetUserDefaultLocaleName, dllKernel32);
            if (!ms_GetUserDefaultLocaleName)
                return false;

            wxDL_INIT_FUNC(ms_, CompareStringEx, dllKernel32);
            if ( !ms_CompareStringEx )
                return false;

            s_canUse = 1;
        }

        return s_canUse == 1;
    }

    static wxVector<wxString> GetPreferredUILanguages()
    {
        wxVector<wxString> preferred;

        if (CanUse())
        {
            // Check if Windows supports preferred UI languages.
            // Note: Windows 8.x might support them as well, but Windows 7
            // and below definitely do not.
            if (wxGetWinVersion() >= wxWinVersion_10)
            {
                ULONG numberOfLanguages = 0;
                ULONG bufferSize = 0;
                if (ms_GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numberOfLanguages, nullptr, &bufferSize))
                {
                    wxScopedArray<WCHAR> languages(bufferSize);
                    if (ms_GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numberOfLanguages, languages.get(), &bufferSize))
                    {
                        WCHAR* buf = languages.get();
                        for (unsigned k = 0; k < numberOfLanguages; ++k)
                        {
                            const wxString language(buf);
                            preferred.push_back(language);
                            buf += language.length() + 1;
                        }
                    }
                    else
                    {
                        wxLogLastError(wxT("GetUserPreferredUILanguages"));
                    }
                }
                else
                {
                    wxLogLastError(wxT("GetUserPreferredUILanguages"));
                }
            }
            else
            {
                // Use the default user locale for Windows 7 resp Windows 8.x and below
                wchar_t buf[LOCALE_NAME_MAX_LENGTH];
                if (!ms_GetUserDefaultLocaleName(buf, LOCALE_NAME_MAX_LENGTH))
                {
                    preferred.push_back(buf);
                }
                else
                {
                    wxLogLastError(wxT("GetUserDefaultLocaleName"));
                }
            }
        }
        else
        {
            const wxLanguageInfos& languagesDB = wxGetLanguageInfos();
            size_t count = languagesDB.size();
            const LANGID langid = ::GetUserDefaultUILanguage();
            if (langid != LOCALE_CUSTOM_UI_DEFAULT)
            {
                size_t ixLanguage = 0;
                wxUint32 lang = PRIMARYLANGID(langid);
                wxUint32 sublang = SUBLANGID(langid);

                for (ixLanguage = 0; ixLanguage < count; ++ixLanguage)
                {
                    if (languagesDB[ixLanguage].WinLang == lang &&
                        languagesDB[ixLanguage].WinSublang == sublang)
                    {
                        break;
                    }
                }
                if (ixLanguage < count)
                {
                    preferred.push_back(languagesDB[ixLanguage].CanonicalName);
                }
            }
        }

        return preferred;
    }

    // Create object corresponding to the default user locale.
    static wxUILocaleImplName* CreateDefault()
    {
        return new wxUILocaleImplName(LOCALE_NAME_USER_DEFAULT);
    }

    // Create object corresponding to the given locale, return nullptr if not
    // supported.
    static wxUILocaleImplName* Create(const wchar_t* name)
    {
        // Getting the locale name seems to be the simplest way to see if it's
        // really supported: unknown locale result in an error here.
        if ( !ms_GetLocaleInfoEx(name, LOCALE_SNAME, nullptr, 0) )
            return nullptr;

        // Unfortunately under Windows 10 the call above only fails if the given
        // locale name is not a valid BCP 47 identifier. For example,
        // valid language codes can have 2 or 3 letters:
        // - using name "w" fails
        // - using name "wx" succeeds
        // - using name "wxy" succeeds
        // - using name "wxyz" fails
        // and so we need another check on these systems (but note that this
        // check must not be done under Windows 7 because there plenty of
        // actually supported locales are "constructed") by checking
        // whether the locale is "constructed" or not: "not constructed"
        // means the locale is a predefined locale, "constructed"
        // means the locale is not predefined, but has to be constructed.
        // For example, "de-US" would be a constructed locale.
        if ( wxGetWinVersion() >= wxWinVersion_10 )
        {
            // Using LOCALE_ICONSTRUCTEDLOCALE to query the locale status is
            // discouraged by Microsoft (the constant is not even defined in a
            // Windows header file). However, for now constructed locales will
            // be rejected here.
            int isConstructed = 0;
            if (!ms_GetLocaleInfoEx
                 (
                    name,
                    LOCALE_ICONSTRUCTEDLOCALE | LOCALE_RETURN_NUMBER,
                    (LPWSTR)&isConstructed,
                    sizeof(int)
                 ) || isConstructed != 0)
                return nullptr;
        }

        return new wxUILocaleImplName(name);
    }


    ~wxUILocaleImplName() override
    {
        free(const_cast<wchar_t*>(m_name));
    }

    void Use() override
    {
        // Construct a double NUL-terminated buffer.
        wchar_t buf[256];
        if ( m_name )
            wxStrlcpy(buf, m_name, WXSIZEOF(buf) - 1);
        else
            buf[0] = L'\0';
        buf[wxWcslen(buf) + 1] = L'\0';

        ULONG num = 1;

        if ( !ms_SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, buf, &num) )
            wxLogLastError(wxT("SetThreadPreferredUILanguages"));
    }

    wxString GetName() const override
    {
        return DoGetInfo(LOCALE_SNAME);
    }

    wxLocaleIdent GetLocaleId() const override
    {
        return wxLocaleIdent::FromTag(GetName());
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const override
    {
        // TODO-XP: This duplicates code from in wxGetInfoFromLCID(), but
        // it's only temporary because we will drop all code using LCID soon.
        wxString str;
        switch ( index )
        {
            case wxLOCALE_THOUSANDS_SEP:
                str = DoGetInfo(LOCALE_STHOUSAND);
                break;

            case wxLOCALE_DECIMAL_POINT:
                str = DoGetInfo(cat == wxLOCALE_CAT_MONEY
                                    ? LOCALE_SMONDECIMALSEP
                                    : LOCALE_SDECIMAL);
                break;

            case wxLOCALE_SHORT_DATE_FMT:
            case wxLOCALE_LONG_DATE_FMT:
            case wxLOCALE_TIME_FMT:
                str = DoGetInfo(wxGetLCTYPEFormatFromLocalInfo(index));
                if ( !str.empty() )
                    str = wxTranslateFromUnicodeFormat(str);
                break;

            case wxLOCALE_DATE_TIME_FMT:
                // there doesn't seem to be any specific setting for this, so just
                // combine date and time ones
                //
                // we use the short date because this is what "%c" uses by default
                // ("%#c" uses long date but we have no way to specify the
                // alternate representation here)
                str << GetInfo(wxLOCALE_SHORT_DATE_FMT, cat)
                    << wxS(' ')
                    << GetInfo(wxLOCALE_TIME_FMT, cat);
                break;

            default:
                wxFAIL_MSG( "unknown wxLocaleInfo" );
        }

        return str;
    }

    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const override
    {
        // TODO-XP: This duplicates code from in wxGetInfoFromLCID(), but
        // it's only temporary because we will drop all code using LCID soon.
                // LOCALE_SINTLSYMBOL (Currency 3-letter ISO 4217)
        static wxWinVersion ver = wxGetWinVersion();
        wxString str;
        switch (name)
        {
            case wxLOCALE_NAME_LOCALE:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        if (ver >= wxWinVersion_7)
                        {
                            str = DoGetInfo(LOCALE_SNATIVEDISPLAYNAME);
                        }
                        else
                        {
                            wxString strLang = DoGetInfo(LOCALE_SNATIVELANGNAME);
                            wxString strCtry = DoGetInfo(LOCALE_SNATIVECTRYNAME);
                            str << strLang << " (" << strCtry << ")";
                        }
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        if (ver >= wxWinVersion_7)
                        {
                            str = DoGetInfo(LOCALE_SENGLISHDISPLAYNAME);
                        }
                        else
                        {
                            wxString strLang = DoGetInfo(LOCALE_SENGLANGUAGE);
                            wxString strCtry = DoGetInfo(LOCALE_SENGCOUNTRY);
                            str << strLang << " (" << strCtry << ")";
                        }
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
            case wxLOCALE_NAME_LANGUAGE:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        str = DoGetInfo(LOCALE_SNATIVELANGUAGENAME);
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        str = DoGetInfo(LOCALE_SENGLISHLANGUAGENAME);
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
            case wxLOCALE_NAME_COUNTRY:
                switch (form)
                {
                    case wxLOCALE_FORM_NATIVE:
                        str = DoGetInfo(LOCALE_SNATIVECOUNTRYNAME);
                        break;
                    case wxLOCALE_FORM_ENGLISH:
                        str = DoGetInfo(LOCALE_SENGLISHCOUNTRYNAME);
                        break;
                    default:
                        wxFAIL_MSG("unknown wxLocaleForm");
                }
                break;
        }

        return str;
    }

    wxLayoutDirection GetLayoutDirection() const override
    {
        if (wxGetWinVersion() >= wxWinVersion_7)
        {
            wxString str = DoGetInfo(LOCALE_IREADINGLAYOUT);
            // str contains a number between 0 and 3:
            // 0 = LTR, 1 = RTL, 2 = TTB+RTL, 3 = TTB + LTR
            // If str equals 1 return RTL, otherwise LTR
            return (str.IsSameAs("1") ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        }
        else
        {
            return wxLayout_Default;
        }
    }

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const override
    {
        DWORD dwFlags = 0;

        if ( flags & wxCompare_CaseInsensitive )
            dwFlags |= NORM_IGNORECASE;

        const int ret = ms_CompareStringEx
            (
                m_name,
                dwFlags,
                lhs.wc_str(), -1,
                rhs.wc_str(), -1,
                nullptr,               // [out] version information -- not needed
                wxRESERVED_PARAM,
                wxRESERVED_PARAM
            );

        switch ( ret )
        {
            case CSTR_LESS_THAN:
                return -1;
            case CSTR_EQUAL:
                return 0;
            case CSTR_GREATER_THAN:
                return 1;
        }

        wxFAIL_MSG(wxS("Unreachable"));
        return 0;
    }

private:
    typedef int (WINAPI *GetLocaleInfoEx_t)(LPCWSTR, LCTYPE, LPWSTR, int);
    static GetLocaleInfoEx_t ms_GetLocaleInfoEx;

    typedef BOOL (WINAPI *SetThreadPreferredUILanguages_t)(DWORD, CONST WCHAR*, ULONG*);
    static SetThreadPreferredUILanguages_t ms_SetThreadPreferredUILanguages;

    typedef BOOL (WINAPI *GetUserPreferredUILanguages_t)(DWORD, ULONG*, WCHAR*, ULONG*);
    static GetUserPreferredUILanguages_t ms_GetUserPreferredUILanguages;

    typedef int (WINAPI* GetUserDefaultLocaleName_t)(LPWSTR, int);
    static GetUserDefaultLocaleName_t ms_GetUserDefaultLocaleName;

    // Note: we currently don't use NLSVERSIONINFO output parameter and so we
    // don't bother dealing with the different sizes of this struct under
    // different OS versions and define the function type as using "void*" to
    // avoid using this parameter accidentally. If we ever really need to use
    // it, we'd need to check the OS version/struct size during run-time.
    typedef int (WINAPI *CompareStringEx_t)(LPCWSTR, DWORD,
                                            CONST WCHAR*, int,
                                            CONST WCHAR*, int,
                                            void*, // actually LPNLSVERSIONINFO
                                            void*,
                                            LPARAM);
    static CompareStringEx_t ms_CompareStringEx;


    // Ctor is private, use CreateDefault() or Create() instead.
    //
    // Note that "name" can be null here (LOCALE_NAME_USER_DEFAULT).
    explicit wxUILocaleImplName(const wchar_t* name)
        : m_name(name ? wxStrdup(name) : nullptr)
    {
    }

    wxString DoGetInfo(LCTYPE lctype) const
    {
        wchar_t buf[256];
        if ( !ms_GetLocaleInfoEx(m_name, lctype, buf, WXSIZEOF(buf)) )
        {
            wxLogLastError(wxT("GetLocaleInfoEx"));
            return wxString();
        }

        return buf;
    }

    const wchar_t* const m_name;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplName);
};

wxUILocaleImplName::GetLocaleInfoEx_t wxUILocaleImplName::ms_GetLocaleInfoEx;
wxUILocaleImplName::SetThreadPreferredUILanguages_t wxUILocaleImplName::ms_SetThreadPreferredUILanguages;
wxUILocaleImplName::GetUserPreferredUILanguages_t wxUILocaleImplName::ms_GetUserPreferredUILanguages;
wxUILocaleImplName::GetUserDefaultLocaleName_t wxUILocaleImplName::ms_GetUserDefaultLocaleName;
wxUILocaleImplName::CompareStringEx_t wxUILocaleImplName::ms_CompareStringEx;

// ----------------------------------------------------------------------------
// wxUILocaleImpl implementation
// ----------------------------------------------------------------------------

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return wxUILocaleImplStdC::Create();
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    if ( !wxUILocaleImplName::CanUse() )
        return new wxUILocaleImplLCID(LOCALE_USER_DEFAULT);

    return wxUILocaleImplName::CreateDefault();
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLanguage(const wxLanguageInfo& info)
{
    if (!wxUILocaleImplName::CanUse())
    {
        if (info.WinLang == 0)
        {
            wxLogWarning(wxS("Locale '%s' not supported by OS."), info.Description);
            return nullptr;
        }
        return new wxUILocaleImplLCID(info.GetLCID());
    }
    else
    {
        return wxUILocaleImplName::Create(info.LocaleTag.wc_str());
    }
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locId)
{
    if ( !wxUILocaleImplName::CanUse() )
    {
        // We could try finding the LCID matching the name, but support for XP
        // will be dropped soon, so it just doesn't seem worth to do it (note
        // that LocaleNameToLCID() itself is not available in XP either, so we
        // can't just use it here).
        return nullptr;
    }

    return wxUILocaleImplName::Create(locId.GetTag(wxLOCALE_TAGTYPE_WINDOWS).wc_str());
}

/* static */
wxVector<wxString> wxUILocaleImpl::GetPreferredUILanguages()
{
    return wxUILocaleImplName::GetPreferredUILanguages();
}

#endif // wxUSE_INTL
