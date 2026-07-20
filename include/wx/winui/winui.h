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

// When a WinUI island has the keyboard focus, dispatch keyboard messages
// directly to it, bypassing wxWidgets' dialog navigation which would otherwise
// swallow character keys (causing lost input and the Windows error beep).
WXDLLIMPEXP_CORE bool wxWinUI3DispatchIslandKeyboard(WXMSG *msg);

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

// Crash-safe debug logging for the WinUI port. Appends to
// %TEMP%\wxwinui-tooltip.log and flushes each line.  Compiled out (no-op)
// unless wxUSE_WINUI3_DEBUG_LOG is defined as 1 when building the library.
#ifndef wxUSE_WINUI3_DEBUG_LOG
    #define wxUSE_WINUI3_DEBUG_LOG 0
#endif
WXDLLIMPEXP_CORE void wxWinUIDebugLog(const char *format, ...);

// Apply the current backdrop (Mica) and title-bar theme to a top-level window.
class WXDLLIMPEXP_FWD_CORE wxWindow;
WXDLLIMPEXP_CORE void wxWinUIApplyWindowBackdrop(wxWindow *tlw);

// Return true for wxWindows whose HWND is used as a WinUI XAML island host.
// They must not be passed to native MSW tooltip registration directly.
WXDLLIMPEXP_CORE bool wxWinUIIsHostWindow(wxWindow *win);

// Reflect a wxWindow::SetCursor() onto its WinUI island, so per-window cursors
// (e.g. a wait cursor on a single control) are shown over the XAML content.
// Called from wxWindowMSW::WXUpdateCursor() under __WXWINUI__.
class WXDLLIMPEXP_FWD_CORE wxCursor;
WXDLLIMPEXP_CORE void wxWinUISetWindowCursor(wxWindow *win, const wxCursor& cursor);

// Whether wxRadioBox draws a Win32-style titled frame around its items (on by
// default, for consistency with the other toolkits).  Turn it off for a flatter,
// pure-WinUI look.  Affects radio boxes created afterwards.
WXDLLIMPEXP_CORE void wxWinUISetRadioBoxBorder(bool useBorder);
WXDLLIMPEXP_CORE bool wxWinUIGetRadioBoxBorder();

// The DWM Mica backdrop surface only initialises after the window is actually
// resized on a given monitor; until then it can render as an opaque rectangle
// (notably on SDR monitors).  This nudges the window size once per monitor to
// prime the backdrop without requiring a manual resize.  No-op after a monitor
// has already been primed.
WXDLLIMPEXP_CORE void wxWinUIPrimeBackdrop(WXHWND hwnd);

// Build (or rebuild) a WinUI MenuBar hosted across the top of a frame from the
// given wxMenuBar, replacing the native HMENU.  Pass the previously returned
// window as "existing" to replace it; returns the new menu-bar window (or null
// if menubar is null).  Menu commands are routed to the frame as wxEVT_MENU.
class WXDLLIMPEXP_FWD_CORE wxMenuBar;
WXDLLIMPEXP_CORE wxWindow* wxWinUIAttachFrameMenuBar(wxWindow *frame,
                                                     wxMenuBar *menubar,
                                                     wxWindow *existing);

// Rebuild the WinUI MenuBar of a frame after its wxMenuBar changed (menus
// added/removed/renamed while the bar is attached).
WXDLLIMPEXP_CORE void wxWinUIRefreshFrameMenuBar(wxWindow *menuBarWin);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_WINUI_H_
