/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/msvcrt.h
// Purpose:     macros to use some non-standard features of MS Visual C++
//              C run-time library
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.01.1999
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// the goal of this file is to define wxCrtSetDbgFlag() macro which may be
// used like this:
//      wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
// to turn on memory leak checks for programs compiled with Microsoft Visual
// C++ (5.0+). The macro will not be defined under other compilers or if it
// can't be used with MSVC for whatever reason.

#ifndef _MSW_MSVCRT_H_
#define _MSW_MSVCRT_H_

// use debug CRT functions for memory leak detections in VC++ 5.0+ in debug
// builds
#undef wxUSE_VC_CRTDBG
#if defined(_DEBUG) && defined(__VISUALC__)
    // it doesn't combine well with wxWin own memory debugging methods
    #if !wxUSE_GLOBAL_MEMORY_OPERATORS && !wxUSE_MEMORY_TRACING && !defined(__NO_VC_CRTDBG__)
        #define wxUSE_VC_CRTDBG
    #endif
#endif

#ifdef wxUSE_VC_CRTDBG
    // Need to undef new if including crtdbg.h which may redefine new itself
    #ifdef new
        #undef new
    #endif

    #include <stdlib.h>

    #include <crtdbg.h>

    #undef WXDEBUG_NEW
    #define WXDEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)

    // this define works around a bug with inline declarations of new, see
    //
    //      http://support.microsoft.com/kb/q140858/
    //
    // for the details
    #define new  WXDEBUG_NEW

    #define wxCrtSetDbgFlag(flag) \
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | (flag))
#else // !using VC CRT
    #define wxCrtSetDbgFlag(flag)
#endif // wxUSE_VC_CRTDBG

#endif // _MSW_MSVCRT_H_

