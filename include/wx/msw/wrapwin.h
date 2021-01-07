/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrapwin.h
// Purpose:     Wrapper around <windows.h>, to be included instead of it
// Author:      Vaclav Slavik
// Created:     2003/07/22
// Copyright:   (c) 2003 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WRAPWIN_H_
#define _WX_WRAPWIN_H_

#include "wx/platform.h"

// before including windows.h, define version macros at (currently) maximal
// values because we do all our checks at run-time anyhow
#include "wx/msw/winver.h"

// strict type checking to detect conversion from HFOO to HBAR at compile-time
#ifndef STRICT
    #define STRICT 1
#endif

// this macro tells windows.h to not define min() and max() as macros: we need
// this as otherwise they conflict with standard C++ functions
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX


// When the application wants to use <winsock2.h> (this is required for IPv6
// support, for example), we must include it before winsock.h, and as windows.h
// includes winsock.h, we have to do it before including it.
#if wxUSE_WINSOCK2
    #include <winsock2.h>
#endif

// Disable any warnings inside Windows headers.
#ifdef __VISUALC__
    #pragma warning(push, 1)
#endif

#include <windows.h>

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

// #undef the macros defined in winsows.h which conflict with code elsewhere
#include "wx/msw/winundef.h"

#endif // _WX_WRAPWIN_H_
