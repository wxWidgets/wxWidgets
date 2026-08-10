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

namespace wxMSWDarkMode
{

// Colour values needed for dark mode support. Unlike wxSystemColour, these
// colours have no useful equivalent on other platforms and are only needed
// for Windows dark mode support. When possible, these colours are chosen to
// accurately match the native Windows look.
enum Colour {
    // Generic border. Value chosen arbitrarily. There does not seem to be
    // any standard colour.
    COLOUR_BORDER = 0x808080,
    // Simple EDIT control border. Taken from appearance with
    // DarkMode_DarkTheme on Windows 11 25H2.
    COLOUR_EDIT_OUTER = 0x383838,
    COLOUR_EDIT_INNER_FOCUS = 0x212121,
    COLOUR_EDIT_INNER_NOFOCUS = 0x2c2c2c,
    COLOUR_EDIT_BOTTOM_FOCUS = 0x4cc2ff,
    COLOUR_EDIT_BOTTOM_NOFOCUS = 0xa4a4a4,
    // wxGauge background and bar. Dark colours taken from appearance with
    // DarkMode_DarkTheme on Windows 11 25H2. Light colours taken from the
    // default theme on the most recent Windows without DarkMode_DarkTheme,
    // Windows 11 24H2.
    COLOUR_GAUGE_DARK_BK = 0x131313,
    COLOUR_GAUGE_DARK_BAR = 0x6ccb5f,
    COLOUR_GAUGE_LIGHT_BK = 0xe6e6e6,
    COLOUR_GAUGE_LIGHT_BAR = 0x0f7b0f,
};

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

// This function draws over the section where the scroll bars meet
// to maintain a consistent theme
void PaintScrollBarCorner(HWND hwnd);

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_DARKMODE_H_
