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
// Diagnostic seam: false means the wx event-loop fallback must pre-translate
// messages; native nested loops remain partially degraded without the hook.
WXDLLIMPEXP_CORE bool wxWinUI3IsGetMessageHookActive();
WXDLLIMPEXP_CORE bool wxWinUI3PreTranslateMessage(WXMSG *msg);
WXDLLIMPEXP_CORE bool wxWinUI3ProcessTabNavigation(WXMSG *msg);
WXDLLIMPEXP_CORE bool
wxWinUI3ProcessTabNavigationWithModifiers(WXMSG *msg,
                                          bool shiftDown,
                                          bool controlDown,
                                          bool altDown);

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

// Crash-safe debug logging for the WinUI port. Always sends each line to the
// debugger and, when WX_WINUI_DIAGNOSTICS_DIR is explicitly set, also appends
// it to <directory>\winui-debug.txt and flushes it. Compiled out (no-op) unless
// wxUSE_WINUI3_DEBUG_LOG is defined as 1 when building the library.
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

// Notify the shared islands after application-wide cursor policy changes
// (busy cursor nesting or wxSetCursor()). This is intentionally projection
// free so the ordinary MSW cursor implementation can call it.
WXDLLIMPEXP_CORE void wxWinUINotifyGlobalCursorChanged();

// Re-apply cursor policy when a wxDialog enters or leaves its modal loop.
// Unlike an application cursor mutation, this transition must not synthesize
// wxEVT_SET_CURSOR while ShowModal() is still on a re-entrant stack.
WXDLLIMPEXP_CORE void wxWinUINotifyModalCursorChanged();

// How the WinUI-drawn common dialogs (wxMessageDialog, wxTextEntryDialog,
// wxColourDialog, ...) are presented.
enum class wxWinUIDialogPresentation
{
    // Classic behaviour, and the default: the dialog is a real top-level
    // window of its own, with its own title bar, which simply happens to be
    // drawn with WinUI controls.  It can be moved anywhere on screen and
    // blocks its parent like any other modal dialog.
    Window,

    // Modern in-window behaviour: the dialog is a WinUI ContentDialog shown
    // over the parent's client area, which is dimmed behind it.  It has no
    // window of its own and cannot leave the parent's bounds.
    Overlay
};

// Set/get the presentation used by the WinUI common dialogs. This is a global
// application setting; set it before showing a dialog. An explicit setting
// takes precedence over WX_WINUI_DIALOG_OVERLAY=1, which opts in to Overlay
// when the application has not selected a mode. Window is the default.
WXDLLIMPEXP_CORE void
wxWinUISetDialogPresentation(wxWinUIDialogPresentation presentation);
WXDLLIMPEXP_CORE wxWinUIDialogPresentation wxWinUIGetDialogPresentation();

// Whether wxRadioBox draws a Win32-style titled frame around its items (on by
// default, for consistency with the other toolkits).  Turn it off for a flatter,
// pure-WinUI look.  Affects radio boxes created afterwards.
WXDLLIMPEXP_CORE void wxWinUISetRadioBoxBorder(bool useBorder);
WXDLLIMPEXP_CORE bool wxWinUIGetRadioBoxBorder();

// The DWM Mica backdrop surface only initialises after the window is actually
// resized; until then it can render as an opaque rectangle (notably on SDR
// monitors). This transactionally nudges this exact native window identity by
// one pixel and restores it on the next event-loop turn. Repeated requests
// coalesce, and an already primed window is a no-op until DWM rebuilds it.
WXDLLIMPEXP_CORE void wxWinUIPrimeBackdrop(WXHWND hwnd);

// True when this window is composed with the DWM system backdrop, i.e.
// when its background has to be painted black for the Mica material to
// show through instead of being covered by an opaque surface. Anything
// drawing a background of its own -- a tool bar art provider, a custom
// panel -- has to ask this before filling.
WXDLLIMPEXP_CORE bool wxWinUIWindowUsesBackdrop(const wxWindow *win);

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
