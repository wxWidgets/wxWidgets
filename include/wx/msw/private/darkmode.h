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

// Return true if the application is using dark mode: note that this will only
// be the case if wxApp::MSWEnableDarkMode() was called.
bool IsActive();

// Enable dark mode for the given TLW if appropriate.
void EnableForTLW(HWND hwnd);

// Set dark theme for the given (child) window if appropriate.
//
// Optional theme name and ID can be specified if something other than the
// default "Explorer" should be used.
void AllowForWindow(HWND hwnd,
                    const wchar_t* themeName = nullptr,
                    const wchar_t* themeId = nullptr);

// Return the colour value appropriate for dark mode if it's used or an invalid
// colour if it isn't.
wxColour GetColour(wxSystemColour index);

// Return the background brush to be used by default in dark mode.
HBRUSH GetBackgroundBrush();

// If dark mode is active, paint the given window using inverted colours.
// Otherwise just return false without doing anything.
//
// This can only be called from WM_PAINT handler for a native control and
// assumes that this control handles WPARAM argument of WM_PAINT as HDC to
// paint on.
bool PaintIfNecessary(wxWindow* w);

} // namespace wxMSWDarkMode

#endif // _WX_MSW_PRIVATE_DARKMODE_H_
