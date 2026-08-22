/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/shelltheme.h
// Purpose:     Native WinUI shell-theme policy and deterministic test seam
// Author:      wxWidgets development team
// Created:     2026-07-30
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_SHELLTHEME_H_
#define _WX_WINUI_PRIVATE_SHELLTHEME_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/winui/winui.h"

// This is the single policy consumed by the non-client area, DWM backdrop,
// native child themes and solid fallback. In particular, High Contrast wins
// over an explicit dark application theme.
struct WXDLLIMPEXP_CORE wxWinUIShellThemePolicy
{
    bool highContrast = false;
    bool dark = false;
    bool allowBackdrop = true;
};

// Every native boundary used when applying the policy is injectable. Calls
// are synchronous and never retain hwnd or context. setSystemBackdrop()
// returns whether the requested material is actually active; callers must
// still treat allowBackdrop=false as authoritative.
struct WXDLLIMPEXP_CORE wxWinUIShellThemeNativeOps
{
    void *context = nullptr;

    bool (*setImmersiveDarkMode)(void *, WXHWND, bool) = nullptr;
    bool (*setSystemBackdrop)(void *, WXHWND, bool) = nullptr;
    bool (*extendFrameIntoClientArea)(void *, WXHWND, bool) = nullptr;
    bool (*setBackdropMarker)(void *, WXHWND, bool) = nullptr;
    bool (*setControlTheme)(void *, WXHWND, bool, bool) = nullptr;
};

// Pure resolver used by the production System-theme adapter and by tests.
// systemDark is ignored for explicit Light/Dark themes and highContrast
// always suppresses both dark-mode forcing and decorative material.
WXDLLIMPEXP_CORE wxWinUIShellThemePolicy
wxWinUI3ResolveShellThemePolicyForTesting(wxWinUIAppTheme appTheme,
                                          bool systemDark,
                                          bool highContrast);

// Implementation-only deterministic seam. It must only be changed on the UI
// thread and reset after all synchronous applications have returned.
WXDLLIMPEXP_CORE void
wxWinUI3SetShellThemeNativeOpsForTesting(
    const wxWinUIShellThemeNativeOps& operations);
WXDLLIMPEXP_CORE void wxWinUI3ResetShellThemeNativeOpsForTesting();

// Retire synchronous TLW epochs and per-descendant fallback provenance before
// an exact native window identity becomes unobservable in WM_DESTROY.
WXDLLIMPEXP_CORE void wxWinUIShellThemeWindowDestroyed(WXHWND hwnd);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_SHELLTHEME_H_
