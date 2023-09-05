///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/private/webview_chromium.h
// Purpose:     Functions used in wxWebViewChromium Mac implementation
// Author:      Vadim Zeitlin
// Created:     2023-09-05
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_PRIVATE_WEBVIEW_CHROMIUM_H_
#define _WX_OSX_PRIVATE_WEBVIEW_CHROMIUM_H_

wxGCC_WARNING_SUPPRESS(unused-parameter)

#include "include/cef_base.h"

wxGCC_WARNING_RESTORE(unused-parameter)

// Called during startup to add CefAppProtocol support to wxNSApplication.
void wxWebViewChromium_InitOSX();

// Called to resize the given NSView to fit its parent.
void wxWebViewChromium_Resize(cef_window_handle_t handle, wxSize size);

#endif // _WX_OSX_PRIVATE_WEBVIEW_CHROMIUM_H_
