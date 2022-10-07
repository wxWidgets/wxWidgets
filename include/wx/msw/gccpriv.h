/*
 Name:        wx/msw/gccpriv.h
 Purpose:     MinGW/Cygwin definitions
 Author:      Vadim Zeitlin
 Modified by:
 Created:
 Copyright:   (c) Vadim Zeitlin
 Licence:     wxWindows Licence
*/

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_MSW_GCCPRIV_H_
#define _WX_MSW_GCCPRIV_H_

#if defined(__MINGW32__) && !defined(__GNUWIN32__)
    #define __GNUWIN32__
#endif

#if defined(__MINGW32__)
    /*
        Include the header defining __MINGW32_{MAJ,MIN}OR_VERSION but check
        that UNICODE or _UNICODE is already defined, as _mingw.h relies on them
        being set and we'd get weird compilation errors later if it is included
        without them being defined, better give a clearer error right now.
     */
    #if !defined(UNICODE)
        #ifndef wxUSE_UNICODE
            #error "wxUSE_UNICODE must be defined before including this header."
        #endif
        #if wxUSE_UNICODE
            #error "UNICODE must be defined before including this header."
        #endif
    #endif

    /*
        MinGW 5.3.0 (and presumably later) predefines _WIN32_WINNT and WINVER
        in sdkddkver.h included from _mingw.h to very low (Windows 2000!)
        values. We really want to predefine them ourselves instead, so do it
        before including _mingw.h.
     */
    #include "wx/msw/winver.h"

    #include <_mingw.h>

    /*
        MinGW-w64 project provides compilers for both Win32 and Win64 but only
        defines the same __MINGW32__ symbol for the former as MinGW32 toolchain
        which is quite different (notably doesn't provide many SDK headers that
        MinGW-w64 does include). So we define a separate symbol which, unlike
        the predefined __MINGW64__, can be used to detect this toolchain in
        both 32 and 64 bit builds.

        And define __MINGW32_TOOLCHAIN__ for consistency and also because it's
        convenient as we often want to have some workarounds only for the (old)
        MinGW32 but not (newer) MinGW-w64, which still predefines __MINGW32__.
     */
    #ifdef __MINGW64_VERSION_MAJOR
        #ifndef __MINGW64_TOOLCHAIN__
            #define __MINGW64_TOOLCHAIN__
        #endif
    #else
        #ifndef __MINGW32_TOOLCHAIN__
            #define __MINGW32_TOOLCHAIN__
        #endif
    #endif

    #define wxCHECK_MINGW32_VERSION( major, minor ) \
 ( ( ( __MINGW32_MAJOR_VERSION > (major) ) \
      || ( __MINGW32_MAJOR_VERSION == (major) && __MINGW32_MINOR_VERSION >= (minor) ) ) )
#else
    #define wxCHECK_MINGW32_VERSION( major, minor ) (0)
#endif

/* check for MinGW/Cygwin w32api version ( releases >= 0.5, only ) */
#include <w32api.h>

#if defined(__W32API_MAJOR_VERSION) && defined(__W32API_MINOR_VERSION)
    #define wxCHECK_W32API_VERSION( major, minor ) \
 ( ( ( __W32API_MAJOR_VERSION > (major) ) \
      || ( __W32API_MAJOR_VERSION == (major) && __W32API_MINOR_VERSION >= (minor) ) ) )
#else
    #define wxCHECK_W32API_VERSION( major, minor ) (0)
#endif

/* Cygwin 1.0 */
#if defined(__CYGWIN__) && ((__GNUC__==2) && (__GNUC_MINOR__==9))
    #define __CYGWIN10__
#endif

/* Prevents conflicts between sys/types.h and winsock.h with Cygwin, */
/* when using Windows sockets. */
#if defined(__CYGWIN__)
    #define __USE_W32_SOCKETS
#endif

/*
    Traditional MinGW (but not MinGW-w64 nor TDM-GCC) omits many POSIX
    functions from their headers when compiled with __STRICT_ANSI__ defined.
    Unfortunately this means that they are not available when using -std=c++98
    (not very common) or -std=c++11 (much more so), but we still need them even
    in this case. As the intention behind using -std=c++11 is probably to get
    the new C++11 features and not disable the use of POSIX functions, we just
    manually declare the functions we need in this case if necessary.
 */
#ifdef __MINGW32_TOOLCHAIN__
    /*
        The macro below is used in wx/wxcrtbase.h included from C regex library
        code, so make sure it compiles in non-C++ code too.
     */
    #ifdef __cplusplus
        #define wxEXTERNC extern "C"
    #else
        #define wxEXTERNC
    #endif

    /*
        This macro is somewhat unusual as it takes the list of parameters
        inside parentheses and includes semicolon inside it as putting the
        semicolon outside wouldn't do the right thing when this macro is empty.
     */
    #define wxDECL_FOR_MINGW32_ALWAYS(rettype, func, params) \
        wxEXTERNC _CRTIMP rettype __cdecl __MINGW_NOTHROW func params ;

    #ifdef __STRICT_ANSI__
        #define wxNEEDS_STRICT_ANSI_WORKAROUNDS

        #define wxDECL_FOR_STRICT_MINGW32(rettype, func, params) \
            wxDECL_FOR_MINGW32_ALWAYS(rettype, func, params)

        /*
            There is a bug resulting in a compilation error in MinGW standard
            math.h header, see https://sourceforge.net/p/mingw/bugs/2250/, work
            around it here because math.h is also included from several other
            standard headers (e.g. <algorithm>) and we don't want to duplicate this
            hack everywhere this happens.
         */
        wxDECL_FOR_STRICT_MINGW32(double, _hypot, (double, double))
    #else
        #define wxDECL_FOR_STRICT_MINGW32(rettype, func, params)
    #endif
#else
    #define wxDECL_FOR_MINGW32_ALWAYS(rettype, func, params)
    #define wxDECL_FOR_STRICT_MINGW32(rettype, func, params)
#endif

#endif
  /* _WX_MSW_GCCPRIV_H_ */
