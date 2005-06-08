/////////////////////////////////////////////////////////////////////////////
// Name:        msw/wrapwin.h
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

// strict type checking to detect conversion from HFOO to HBAR at compile-time
#ifndef STRICT
    #define STRICT 1
#endif

// this macro tells windows.h to not define min() and max() as macros: we need
// this as otherwise they conflict with standard C++ functions
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX


// before including windows.h, define version macros at (currently) maximal
// values because we do all our checks at run-time anyhow
#ifndef WINVER
    // the only exception to the above is MSVC 6 which has a time bomb in its
    // headers: they warn against using them with WINVER >= 0x0500 as they
    // contain only part of the declarations and they're not always correct, so
    // don't define WINVER for it at all as this allows everything to work as
    // expected both with standard VC6 headers (which define WINVER as 0x0400
    // by default) and headers from a newer SDK (which may define it as 0x0500)
    #if !defined(__VISUALC__) || (__VISUALC__ >= 1300)
        #define WINVER 0x0600
    #endif
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#endif


#include <windows.h>

#ifdef __WXWINCE__
    // this doesn't make any sense knowing that windows.h includes all these
    // headers anyhow, but the fact remains that when building using eVC 4 the
    // functions and constants from these headers are not defined unless we
    // explicitly include them ourselves -- how is it possible is beyond me...
    #include <winbase.h>
    #include <wingdi.h>
    #include <winuser.h>

    // this one OTOH contains many useful CE-only functions
    #include <shellapi.h>
#endif // __WXWINCE__


// #undef the macros defined in winsows.h which conflict with code elsewhere
#include "wx/msw/winundef.h"


// types DWORD_PTR, ULONG_PTR and so on might be not defined in old headers but
// unfortunately I don't know of any standard way to test for this (as they're
// typedefs and not #defines), so simply overwrite them in any case in Win32
// mode -- and if compiling for Win64 they'd better have new headers anyhow
//
// this is ugly but what else can we do? even testing for compiler version
// wouldn't help as you can perfectly well be using an older compiler (VC6)
// with newer SDK headers
#if !defined(__WIN64__) && !defined(__WXWINCE__)
    #define UINT_PTR unsigned int
    #define LONG_PTR long
    #define ULONG_PTR unsigned long
    #define DWORD_PTR unsigned long
#endif // !__WIN64__

#endif // _WX_WRAPWIN_H_

