///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/private/available.h
// Purpose:     Helper for checking API availability under macOS.
// Author:      Vadim Zeitlin
// Created:     2019-04-17
// Copyright:   (c) 2019 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_PRIVATE_AVAILABLE_H_
#define _WX_OSX_PRIVATE_AVAILABLE_H_

// Xcode 9 adds new @available keyword and the corresponding __builtin_available
// builtin which should be used instead of manual checks for API availability
// as using this builtin suppresses the compiler -Wunguarded-availability
// warnings, so use it if possible for the implementation of our own macro.
#if defined(__clang__) && __has_builtin(__builtin_available)
    #define WX_IS_MACOS_AVAILABLE(major, minor) \
        __builtin_available(macOS major ## . ## minor, *)

    #define WX_IS_MACOS_AVAILABLE_FULL(major, minor, micro) \
        __builtin_available(macOS major ## . ## minor ## . ## micro, *)

    // Note that we can't easily forward to API_AVAILABLE macro here, so go
    // directly to its expansion instead.
    #define WX_API_AVAILABLE_MACOS(major, minor) \
         __attribute__((availability(macos,introduced=major ## . ## minor)))
#else // Not clang or old clang version without __builtin_available
    #include "wx/platinfo.h"

    #define WX_IS_MACOS_AVAILABLE(major, minor) \
        wxPlatformInfo::Get().CheckOSVersion(major, minor)

    #define WX_IS_MACOS_AVAILABLE_FULL(major, minor, micro) \
        wxPlatformInfo::Get().CheckOSVersion(major, minor, micro)

    #define WX_API_AVAILABLE_MACOS(major, minor)
#endif

#endif // _WX_OSX_PRIVATE_AVAILABLE_H_
