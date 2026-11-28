/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/shelltheme.h
// Purpose:     Native WinUI shell-theme policy
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

// Retire synchronous TLW epochs and per-descendant fallback provenance before
// an exact native window identity becomes unobservable in WM_DESTROY.
WXDLLIMPEXP_CORE void wxWinUIShellThemeWindowDestroyed(WXHWND hwnd);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_SHELLTHEME_H_
