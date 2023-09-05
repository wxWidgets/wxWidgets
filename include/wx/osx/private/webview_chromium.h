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

// Called during startup to add CefAppProtocol support to wxNSApplication.
void wxWebViewChromium_InitOSX();

#endif // _WX_OSX_PRIVATE_WEBVIEW_CHROMIUM_H_
