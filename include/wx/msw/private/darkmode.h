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
#include "wx/msw/wrapwin.h"

namespace wxMSWDarkMode
{

// Return true if the application is using dark mode: note that this will only
// be the case if wxApp::MSWEnableDarkMode() was called.
WXDLLIMPEXP_CORE
bool IsActive();

// Enable dark mode for the given TLW if appropriate.
void EnableForTLW(HWND hwnd);

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

// ----------------------------------------------------------------------------
// TaskDialog dark mode helpers
// ----------------------------------------------------------------------------
//
// These two functions bracket the lifetime of dark-mode theming for any
// TaskDialog-based dialog (wxMessageDialog, wxRichMessageDialog,
// wxProgressDialog, wxAboutBox).
//
// Typical usage inside a PFTASKDIALOGCALLBACK:
//
//   case TDN_CREATED:
//       wxMSWDarkMode::ApplyToTaskDialog(hwnd, pCfg);
//       break;
//   case TDN_DESTROYED:
//       wxMSWDarkMode::RemoveFromTaskDialog(hwnd);
//       break;
//
// Both functions are no-ops when IsActive() returns false, so no guard is
// needed at the call site.

// Apply dark mode theming to an existing TaskDialog window.
//
// Must be called from TDN_CREATED (not TDN_DIALOG_CONSTRUCTED) so that the
// complete child-window hierarchy exists and UI Automation can walk it.
//
// pCfg may be nullptr; when provided it enables correct icon overdraw on
// Windows 10 where there is no native dark TaskDialog theme.
void AllowForTaskDialog(HWND hwnd, const TASKDIALOGCONFIG* pCfg = nullptr);

// Remove all subclasses and free all GDI/theme resources installed by
// ApplyToTaskDialog().
//
// Must be called from TDN_DESTROYED or WM_DESTROY.
// Safe to call even if ApplyToTaskDialog() was never called for this HWND.
void RemoveFromTaskDialog(HWND hwnd);

} // namespace wxMSWDarkMode

namespace wxMSWImpl
{

// This function is not dark mode specific but reuses the code in darkmode.cpp,
// so it's implemented there as well.
void EnableRoundCorners(HWND hwnd);

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_DARKMODE_H_
