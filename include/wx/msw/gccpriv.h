// MinGW w32api specific stuff

#ifndef _WX_MSW_GCCPRIV_H_
#define _WX_MSW_GCCPRIV_H_

// check for MinGW/Cygwin w32api version ( releases >= 0.5, only )
#if defined( HAVE_W32API_H )
#include <w32api.h>
#endif

#define wxCHECK_W32API_VERSION( major, minor ) \
 ( defined( __W32API_MAJOR_VERSION ) && defined( __W32API_MINOR_VERSION ) \
 && ( ( __W32API_MAJOR_VERSION > (major) ) \
      || ( __W32API_MAJOR_VERSION == (major) && __W32API_MINOR_VERSION >= (minor) ) ) )

// Cygwin / Mingw32 with gcc >= 2.95 use new windows headers which
// are more ms-like (header author is Anders Norlander, hence the name)
#if (defined(__MINGW32__) || defined(__CYGWIN__)) && ((__GNUC__>2) ||((__GNUC__==2) && (__GNUC_MINOR__>=95)))
    #ifndef wxUSE_NORLANDER_HEADERS
        #define wxUSE_NORLANDER_HEADERS 1
    #endif
#else
    #ifndef wxUSE_NORLANDER_HEADERS
        #define wxUSE_NORLANDER_HEADERS 0
    #endif
#endif

// "old" GNUWIN32 is the one without Norlander's headers: it lacks the
// standard Win32 headers and we define the used stuff ourselves for it
// in wx/msw/gnuwin32/extra.h
#if defined(__GNUC__) && !wxUSE_NORLANDER_HEADERS
    #define __GNUWIN32_OLD__
#endif

#endif
  // _WX_MSW_GCCPRIV_H_
