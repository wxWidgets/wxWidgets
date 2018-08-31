///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/winver.h
// Purpose:     Define Windows version macros if they're not predefined.
// Author:      Vadim Zeitlin
// Created:     2017-01-13 (extracted from wx/msw/wrapwin.h)
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WINVER_H_
#define _WX_MSW_WINVER_H_

// Notice that this header must not include any other wx headers as it's
// indirectly included from wx/defs.h itself when using gcc (via wx/platform.h,
// then wx/compiler.h and wx/msw/gccpriv.h).

// Define WINVER, _WIN32_WINNT and _WIN32_IE to the highest possible values
// because we always check for the version of installed DLLs at runtime anyway
// (see wxGetWinVersion() and wxApp::GetComCtl32Version()) unless the user
// really doesn't want to use APIs only available on later OS versions and had
// defined them to (presumably lower) values -- or, alternatively, wants to use
// even higher version of the API which will become available later.

#ifndef WINVER
    #define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0A00
#endif

#ifndef _WIN32_IE
    #define _WIN32_IE 0x0A00
#endif

#endif // _WX_MSW_WINVER_H_
