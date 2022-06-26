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

namespace wxMSWDarkMode
{

// Enable dark mode for the given TLW if appropriate.
void EnableForTLW(HWND hwnd);

// Set dark theme for the given (child) window if appropriate.
void AllowForWindow(HWND hwnd);

} // namespace wxMSWDarkMode

#endif // _WX_MSW_PRIVATE_DARKMODE_H_
