///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/darkmode.cpp
// Purpose:     Support for dark mode in wxMSW
// Author:      Vadim Zeitlin
// Created:     2022-06-24
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    The code in this file is based on the following sources:

    - win32-darkmode by Richard Yu (https://github.com/ysc3839/win32-darkmode)
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// Allow predefining this as 0 to disable dark mode support completely.
#ifndef wxUSE_DARK_MODE
    // Otherwise enable it by default.
    #define wxUSE_DARK_MODE 1
#endif

#if wxUSE_DARK_MODE

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/dynlib.h"

static const char* TRACE_DARKMODE = "msw-darkmode";

namespace
{

// Constants for use with SetPreferredAppMode().
enum PreferredAppMode
{
    AppMode_Default,
    AppMode_AllowDark,
    AppMode_ForceDark,
    AppMode_ForceLight
};

PreferredAppMode gs_appMode = AppMode_Default;

template <typename T>
bool TryLoadByOrd(T& func, const wxDynamicLibrary& lib, int ordinal)
{
    func = (T)::GetProcAddress(lib.GetLibHandle(), MAKEINTRESOURCEA(ordinal));
    if ( !func )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "Required function with ordinal %d not found", ordinal);
        return false;
    }

    return true;
}

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

namespace wxMSWImpl
{

// Global pointers of the functions we use: they're not only undocumented, but
// don't appear in the SDK headers at all.
BOOL (WINAPI *ShouldAppsUseDarkMode)() = nullptr;
DWORD (WINAPI *SetPreferredAppMode)(DWORD) = nullptr;

bool InitDarkMode()
{
    // Note: for simplicity, we support dark mode only in Windows 10 v2004
    // ("20H1", build number 19041) and later, even if, in principle, it could
    // be supported as far back as v1809 (build 17763) -- but very few people
    // must still use it by now and so it just doesn't seem to be worth it.
    if ( !wxCheckOsVersion(10, 0, 19041) )
    {
        wxLogTrace(TRACE_DARKMODE, "Unsupported due to OS version");
        return false;
    }

    wxLoadedDLL dllUxTheme(wxS("uxtheme.dll"));

    // These functions are not only undocumented but are not even exported by
    // name, and have to be resolved using their ordinals.
    if ( !TryLoadByOrd(ShouldAppsUseDarkMode, dllUxTheme, 132) )
        return false;

    if ( !TryLoadByOrd(SetPreferredAppMode, dllUxTheme, 135) )
        return false;

    return true;
}

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

bool wxApp::MSWEnableDarkMode(int flags)
{
    if ( !wxMSWImpl::InitDarkMode() )
        return false;

    const PreferredAppMode mode = flags & DarkMode_Always ? AppMode_ForceDark
                                                          : AppMode_AllowDark;
    const DWORD rc = wxMSWImpl::SetPreferredAppMode(mode);

    // It's supposed to return the old mode normally.
    if ( rc != static_cast<DWORD>(gs_appMode) )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "SetPreferredAppMode(%d) unexpectedly returned %d",
                   mode, rc);
    }

    gs_appMode = mode;

    return true;
}

#else // !wxUSE_DARK_MODE

bool wxApp::MSWEnableDarkMode(int WXUNUSED(flags))
{
    return false;
}

#endif // wxUSE_DARK_MODE/!wxUSE_DARK_MODE
