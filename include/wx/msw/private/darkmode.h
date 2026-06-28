///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/darkmode.h
// Purpose:     Dark mode support in wxMSW
// Author:      Vadim Zeitlin
// Created:     2022-06-25
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_DARKMODE_H_
#define _WX_MSW_PRIVATE_DARKMODE_H_

#include "wx/settings.h"
#include <vsstyle.h>

namespace wxMSWDarkMode
{

// Return true if the application is using dark mode: note that this will only
// be the case if wxApp::MSWEnableDarkMode() was called.
WXDLLIMPEXP_CORE
bool IsActive();

// Return true if the system has switched between dark and light modes.
// Some controls need to take extra actions to switch from light mode to dark
// mode compared with just creating the window in dark mode. This function
// helps us do only what is needed, to avoid overwriting user settings such as
// background and foreground colours.
WXDLLIMPEXP_CORE
bool HasChanged();

/**
 * @brief Dynamic Runtime Check for Aero.msstyles Dark Mode Support
 *
 * Verifies if a specific dark-theme class variant is actively supported and
 * distinct from the standard light-theme appearance inside the active OS visual style.
 *
 * @param stdClass The standard/light window class name (e.g., L"EDIT", L"Button").
 * @param darkClass The specialized dark mode class path (e.g., L"DarkMode_DarkTheme::ListView").
 * @return true If the dark class successfully loads and provides a unique theme handle.
 * @return false If the theme subsystem falls back to light styles or fails to load.
 */
bool IsDarkThemeActive(const wchar_t* stdClass, const wchar_t* darkClass);

// Enable or disable dark mode for the given TLW if appropriate.
void ConfigureTLW(HWND hwnd);

// Helper function: call SetWindowTheme() and log a debug error if it fails.
void SetTheme(HWND hwnd,
              const wchar_t* themeName,
              const wchar_t* themeId = nullptr);

// Set dark theme for the given (child) window if appropriate.
//
// Optional theme name and ID can be specified if something other than the
// default "Explorer" should be used. If both theme name and theme ID are null,
// no theme is set.
void AllowForWindow(HWND hwnd,
                    const wchar_t* themeName = L"Explorer",
                    const wchar_t* themeId = nullptr);

// Draws a custom dark mode border for controls (e.g. EDIT, STATIC, etc.).
// Supports flat (SIMPLE/STATIC/THEME) and 3D (RAISED/SUNKEN) styles.
// Uses multi-tone dark colours to simulate classic Windows 3D depth.
// The rectangle `rc` is in device coordinates (full window size).
// `state` is one of ETS_NORMAL, ETS_DISABLED, ETS_FOCUSED, ETS_HOT, ETS_READONLY.
void DrawDarkModeBorder(wxDC& dc, const wxRect& rc, wxBorder borderStyle,
    int thickness, int state = ETS_NORMAL);
// Return the colour value appropriate for dark mode if it's used or an invalid
// colour if it isn't.
wxColour GetColour(wxSystemColour index);

// Get the pen to use for drawing the border, see wxDarkModeSettings.
wxPen GetBorderPen();

// Return the background brush to be used by default in dark mode.
HBRUSH GetBackgroundBrush();

// Invert the colours of the given bitmap trying to keep it readable.
wxBitmap InvertBitmap(const wxBitmap& bmp);

// If dark mode is active, paint the given window using inverted colours by
// drawing it normally and then applying InvertBitmap() to it.
//
// Otherwise just return false without doing anything.
//
// This can only be called from WM_PAINT handler for a native control and
// assumes that this control handles WPARAM argument of WM_PAINT as HDC to
// paint on.
bool PaintIfNecessary(HWND hwnd, WXWNDPROC defWndProc);

// If dark mode is active and if the message is one of those used for menu
// drawing, process it and return true, otherwise just return false without
// doing anything.
bool
HandleMenuMessage(WXLRESULT* result,
                  wxWindow* w,
                  WXUINT nMsg,
                  WXWPARAM wParam,
                  WXLPARAM lParam);

void NotifySysColorChange();

} // namespace wxMSWDarkMode

namespace wxMSWImpl
{

// This function is not dark mode specific but reuses the code in darkmode.cpp,
// so it's implemented there as well.
void EnableRoundCorners(HWND hwnd);

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_DARKMODE_H_
