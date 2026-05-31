/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/winui.h
// Purpose:     wxWinUI declarations
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_WINUI_H_
#define _WX_WINUI_WINUI_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

WXDLLIMPEXP_CORE bool wxWinUI3Initialize();
WXDLLIMPEXP_CORE void wxWinUI3Uninitialize();
WXDLLIMPEXP_CORE bool wxWinUI3PreTranslateMessage(WXMSG *msg);
WXDLLIMPEXP_CORE bool wxWinUI3ProcessTabNavigation(WXMSG *msg);

// Application-wide theme applied to all live WinUI islands.  System follows
// the OS light/dark setting; Light and Dark force the respective theme.
enum class wxWinUIAppTheme
{
    System,
    Light,
    Dark
};

// Set the theme used by all current and future WinUI controls.  This is what
// wxApp::SetAppearance() forwards to, so applications get themed controls
// without any WinUI-specific code.
WXDLLIMPEXP_CORE void wxWinUISetAppTheme(wxWinUIAppTheme theme);
WXDLLIMPEXP_CORE wxWinUIAppTheme wxWinUIGetAppTheme();

// Apply the current backdrop (Mica) and title-bar theme to a top-level window.
class WXDLLIMPEXP_FWD_CORE wxWindow;
WXDLLIMPEXP_CORE void wxWinUIApplyWindowBackdrop(wxWindow *tlw);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_WINUI_H_
