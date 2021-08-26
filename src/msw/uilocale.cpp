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

#include "wx/private/uilocale.h"

#include "wx/msw/private/uilocale.h"

#include "wx/dynlib.h"

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
// wxUILocale implementation for MSW
// ----------------------------------------------------------------------------

// TODO-XP: Replace this with an implementation using GetLocaleInfoEx() when we
// don't support XP any longer.
class wxUILocaleImplLCID : public wxUILocaleImpl
{
public:
    explicit wxUILocaleImplLCID(LCID lcid)
        : m_lcid(lcid)
    {
        wxUseLCID(lcid);
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

private:
    const LCID m_lcid;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplLCID);
};

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    // There is no LCID for "C" locale, but US English is basically the same.
    LCID lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
    return new wxUILocaleImplLCID(lcid);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplLCID(LOCALE_USER_DEFAULT);
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

#endif // wxUSE_INTL
