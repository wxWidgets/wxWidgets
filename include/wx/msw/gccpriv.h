// MinGW w32api specific stuff

#ifndef _WX_MSW_GCCPRIV_H_
#define _WX_MSW_GCCPRIV_H_

#if defined( __MINGW32__ ) && !defined( HAVE_W32API_H )
    #if ( __GNUC__ > 2 ) || ( ( __GNUC__ == 2 ) && ( __GNUC_MINOR__ >= 95 ) )
        #include <_mingw.h>
        #if __MINGW32_MAJOR_VERSION >= 1
            #define HAVE_W32API_H
        #endif
    #endif
#elif defined( __CYGWIN__ ) && !defined( HAVE_W32API_H )
    #if ( __GNUC__ > 2 )
        #define HAVE_W32API_H
    #endif
#endif

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
#if (defined(__MINGW32__) || defined(__CYGWIN__)) && ((__GNUC__>2) || ((__GNUC__==2) && (__GNUC_MINOR__>=95)))
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

// Cygwin 1.0
#if defined(__CYGWIN__) && ((__GNUC__==2) && (__GNUC_MINOR__==9))
    #define __CYGWIN10__
#endif

// Mingw runtime 1.0-20010604 has some missing _tXXXX functions,
// so let's define them ourselves:
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


#endif
  // _WX_MSW_GCCPRIV_H_
