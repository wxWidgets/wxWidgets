/////////////////////////////////////////////////////////////////////////////
// Name:        wrapwin.h
// Purpose:     Wrapper around <windows.h>, to be included instead of it
// Author:      Vaclav Slavik
// Created:     2003/07/22
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WRAPWIN_H_
#define _WX_WRAPWIN_H_

#include "wx/platform.h"

#ifndef STRICT
    #define STRICT 1
#endif

// define _WIN32_IE to a high value because we always check for the version
// of installed DLLs at runtime anyway unless the user really doesn't want it
#ifndef _WIN32_IE
    // for compilers that use w32api headers: w32api must be >= 1.1:
    #if defined( __GNUWIN32__ ) || defined( __MINGW32__ ) || \
        defined( __CYGWIN__ ) || \
        (defined(__WATCOMC__) && __WATCOMC__ >= 1200) || \
        defined(__DIGITALMARS__) && \
        !wxCHECK_W32API_VERSION(1,1)
        #define _WIN32_IE 0x300
    #else
        // highest known value at the time of this writing (Windows Server 2003)
        #define _WIN32_IE 0x502
    #endif
#endif // !defined(_WIN32_IE)

#include <windows.h>
#include "wx/msw/winundef.h"

#endif // _WX_WRAPWIN_H_

