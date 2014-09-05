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

#if defined( __MINGW32__ ) && !defined(__WINE__) && !defined( HAVE_W32API_H )
    #if __MINGW32_MAJOR_VERSION >= 1
        #define HAVE_W32API_H
    #endif
#elif defined( __CYGWIN__ ) && !defined( HAVE_W32API_H )
    #if ( __GNUC__ > 2 )
        #define HAVE_W32API_H
    #endif
#endif

/* check for MinGW/Cygwin w32api version ( releases >= 0.5, only ) */
#if defined( HAVE_W32API_H )
#include <w32api.h>
#endif

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

/* Mingw runtime 1.0-20010604 has some missing _tXXXX functions,
   so let's define them ourselves: */
#if defined(__GNUWIN32__) && wxCHECK_W32API_VERSION( 1, 0 ) \
    && !wxCHECK_W32API_VERSION( 1, 1 )
    #ifndef _tsetlocale
      #if wxUSE_UNICODE
      #define _tsetlocale _wsetlocale
      #else
      #define _tsetlocale setlocale
      #endif
    #endif
    #ifndef _tgetenv
      #if wxUSE_UNICODE
      #define _tgetenv _wgetenv
      #else
      #define _tgetenv getenv
      #endif
    #endif
    #ifndef _tfopen
      #if wxUSE_UNICODE
      #define _tfopen _wfopen
      #else
      #define _tfopen fopen
      #endif
    #endif
#endif

/* current (= before mingw-runtime 3.3) mingw32 headers forget to
   define _puttchar, this will probably be fixed in the next versions but
   for now do it ourselves
 */
#if defined( __MINGW32__ ) && \
        !wxCHECK_MINGW32_VERSION(3,3) && !defined( _puttchar )
    #ifdef wxUSE_UNICODE
        #define  _puttchar   putwchar
    #else
        #define  _puttchar   puttchar
    #endif
#endif

#endif
  /* _WX_MSW_GCCPRIV_H_ */
