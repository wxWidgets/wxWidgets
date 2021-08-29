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

#include "wx/dynlib.h"

#ifndef LOCALE_NAME_USER_DEFAULT
    #define LOCALE_NAME_USER_DEFAULT NULL
#endif

#ifndef MUI_LANGUAGE_NAME
    #define MUI_LANGUAGE_NAME 8
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
        SetThreadUILanguage_t pfnSetThreadUILanguage = NULL;
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
    }

    return name;
}

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

    bool Use() wxOVERRIDE
    {
        wxUseLCID(m_lcid);

        // As long as we use a valid LCID (and we always do), it shouldn't fail.
        return true;
    }

    wxString GetName() const wxOVERRIDE
    {
        wxChar buf[256];
        buf[0] = wxT('\0');

        // Try using newer constant available since Vista which produces names
        // more similar to the other platforms.
        if ( wxGetWinVersion() >= wxWinVersion_Vista )
        {
            ::GetLocaleInfo(m_lcid, LOCALE_SNAME, buf, WXSIZEOF(buf));
        }
        else // TODO-XP: Drop this branch.
        {
            // This name constant is available under all systems, including
            // pre-Vista ones.
            ::GetLocaleInfo(m_lcid, LOCALE_SENGLANGUAGE, buf, WXSIZEOF(buf));
        }

        return buf;
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE
    {
        return wxGetInfoFromLCID(m_lcid, index, cat);
    }

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const wxOVERRIDE
    {
        // Can't be really implemented on the OS versions where this class is
        // used.
        return flags & wxCompare_CaseInsensitive
                ? lhs.CmpNoCase(rhs)
                : lhs.Cmp(rhs) ;
    }

private:
    const LCID m_lcid;

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

            wxDL_INIT_FUNC(ms_, CompareStringEx, dllKernel32);
            if ( !ms_CompareStringEx )
                return false;

            s_canUse = 1;
        }

        return s_canUse == 1;
    }

    // Note that "name" can be NULL here (LOCALE_NAME_USER_DEFAULT).
    explicit wxUILocaleImplName(const wchar_t* name)
        : m_name(name ? wxStrdup(name) : NULL)
    {
    }

    ~wxUILocaleImplName() wxOVERRIDE
    {
        free(const_cast<wchar_t*>(m_name));
    }

    bool Use() wxOVERRIDE
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
            return false;

        return true;
    }

    wxString GetName() const wxOVERRIDE
    {
        return DoGetInfo(LOCALE_SNAME);
    }

    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE
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

    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const wxOVERRIDE
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
                NULL,               // [out] version information -- not needed
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
wxUILocaleImplName::CompareStringEx_t wxUILocaleImplName::ms_CompareStringEx;

// ----------------------------------------------------------------------------
// wxUILocaleImpl implementation
// ----------------------------------------------------------------------------

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    if ( !wxUILocaleImplName::CanUse() )
    {
        // There is no LCID for "C" locale, but US English is basically the same.
        LCID lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
        return new wxUILocaleImplLCID(lcid);
    }

    return new wxUILocaleImplName(L"en-US");
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    if ( !wxUILocaleImplName::CanUse() )
        return new wxUILocaleImplLCID(LOCALE_USER_DEFAULT);

    return new wxUILocaleImplName(LOCALE_NAME_USER_DEFAULT);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLanguage(const wxLanguageInfo& info)
{
    if ( info.WinLang == 0 )
    {
        wxLogWarning(wxS("Locale '%s' not supported by OS."), info.Description);

        return NULL;
    }

    return new wxUILocaleImplLCID(info.GetLCID());
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locId)
{
    if ( !wxUILocaleImplName::CanUse() )
    {
        // We could try finding the LCID matching the name, but support for XP
        // will be dropped soon, so it just doesn't seem worth to do it (note
        // that LocaleNameToLCID() itself is not available in XP neither, so we
        // can't just use it here).
        return NULL;
    }

    return new wxUILocaleImplName(locId.GetName());
}

#endif // wxUSE_INTL
