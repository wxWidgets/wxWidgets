/*
 * Name:        wx/chartype.h
 * Purpose:     Declarations of wxChar and related types
 * Author:      Joel Farley, Ove Kåven
 * Modified by: Vadim Zeitlin, Robert Roebling, Ron Lee
 * Created:     1998/06/12
 * Copyright:   (c) 1998-2006 wxWidgets dev team
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_WXCHARTYPE_H_
#define _WX_WXCHARTYPE_H_

/*
    wx/defs.h indirectly includes this file, so we can't include it here,
    include just its subset which defines SIZEOF_WCHAR_T that is used here
    (under Unix it's in configure-generated setup.h, so including wx/platform.h
    would have been enough, but this is not the case under other platforms).
 */
#include "wx/types.h"

/* This is kept only for backwards compatibility, in case some application code
   checks for it. It's always 1 and can't be changed. */
#define wxUSE_WCHAR_T 1

/*
   non Unix compilers which do have wchar.h (but not tchar.h which is included
   below and which includes wchar.h anyhow).

   Actually MinGW has tchar.h, but it does not include wchar.h
 */
#if defined(__MINGW32__)
    #ifndef HAVE_WCHAR_H
        #define HAVE_WCHAR_H
    #endif
#endif

#ifdef HAVE_WCHAR_H
    /* the current (as of Nov 2002) version of cygwin has a bug in its */
    /* wchar.h -- there is no extern "C" around the declarations in it */
    /* and this results in linking errors later; also, at least on some */
    /* Cygwin versions, wchar.h requires sys/types.h */
    #ifdef __CYGWIN__
        #include <sys/types.h>
        #ifdef __cplusplus
            extern "C" {
        #endif
    #endif /* Cygwin */

    #include <wchar.h>

    #if defined(__CYGWIN__) && defined(__cplusplus)
        }
    #endif /* Cygwin and C++ */

    /* the current (as of Mar 2014) version of Android (up to api level 19) */
    /* doesn't include some declarations (wscdup, wcslen, wcscasecmp, etc.) */
    /* (moved out from __CYGWIN__ block) */
    #if defined(__WXQT__) && !defined(wcsdup) && defined(__ANDROID__)
        #ifdef __cplusplus
            extern "C" {
        #endif
            extern wchar_t *wcsdup(const wchar_t *);
            extern size_t wcslen (const wchar_t *);
            extern size_t wcsnlen (const wchar_t *, size_t );
            extern int wcscasecmp (const wchar_t *, const wchar_t *);
            extern int wcsncasecmp (const wchar_t *, const wchar_t *, size_t);
        #ifdef __cplusplus
            }
        #endif
    #endif /* Android */

#elif defined(HAVE_WCSTR_H)
    /* old compilers have relevant declarations here */
    #include <wcstr.h>
#elif defined(__FreeBSD__) || defined(__DARWIN__)
    /* include stdlib.h for wchar_t */
    #include <stdlib.h>
#endif /* HAVE_WCHAR_H */

#ifdef HAVE_WIDEC_H
    #include <widec.h>
#endif

/* -------------------------------------------------------------------------- */
/* define wxHAVE_TCHAR_SUPPORT for the compilers which support the TCHAR type */
/* mapped to either char or wchar_t depending on the ASCII/Unicode mode and   */
/* have the function mapping _tfoo() -> foo() or wfoo()                       */
/* -------------------------------------------------------------------------- */

/* VC++ and BC++ starting with 5.2 have TCHAR support */
#ifdef __VISUALC__
    #define wxHAVE_TCHAR_SUPPORT
#elif defined(__MINGW32__)
    #define wxHAVE_TCHAR_SUPPORT
    #include <stddef.h>
    #include <string.h>
    #include <ctype.h>
#endif /* compilers with (good) TCHAR support */

#ifdef wxHAVE_TCHAR_SUPPORT
    /* get TCHAR definition if we've got it */
    #include <tchar.h>
#endif /* wxHAVE_TCHAR_SUPPORT */

/* ------------------------------------------------------------------------- */
/* define wxChar type for compatibility only                                 */
/* ------------------------------------------------------------------------- */

typedef wchar_t wxChar;
typedef wchar_t wxSChar;
typedef wchar_t wxUChar;

/* ------------------------------------------------------------------------- */
/* define wxStringCharType                                                   */
/* ------------------------------------------------------------------------- */

/* depending on the build options, strings can store their data either as
   wchar_t* or UTF-8 encoded char*: */
#if defined(wxUSE_UTF8_LOCALE_ONLY) && !defined(wxUSE_UNICODE_UTF8)
    #error "wxUSE_UTF8_LOCALE_ONLY only makes sense with wxUSE_UNICODE_UTF8"
#endif
#ifndef wxUSE_UTF8_LOCALE_ONLY
    #define wxUSE_UTF8_LOCALE_ONLY 0
#endif

#ifndef wxUSE_UNICODE_UTF8
    #define wxUSE_UNICODE_UTF8 0
#endif

#if wxUSE_UNICODE_UTF8
    #define wxUSE_UNICODE_WCHAR 0
#else
    #define wxUSE_UNICODE_WCHAR 1
#endif

#ifndef SIZEOF_WCHAR_T
    #error "SIZEOF_WCHAR_T must be defined before including this file in wx/defs.h"
#endif

#if wxUSE_UNICODE_WCHAR && SIZEOF_WCHAR_T == 2
    #define wxUSE_UNICODE_UTF16 1
#else
    #define wxUSE_UNICODE_UTF16 0
#endif

/* define char type used by wxString internal representation: */
#if wxUSE_UNICODE_WCHAR
    typedef wchar_t wxStringCharType;
#else /* wxUSE_UNICODE_UTF8 */
    typedef char wxStringCharType;
#endif


/* ------------------------------------------------------------------------- */
/* define wxT() and related macros                                           */
/* ------------------------------------------------------------------------- */

/* BSD systems define _T() to be something different in ctype.h, override it */
#if defined(__FreeBSD__) || defined(__DARWIN__)
    #include <ctype.h>
    #undef _T
#endif

/*
   wxT ("wx text") macro turns a literal string constant into a wide char
   constant. It is mostly unnecessary with wx 2.9 but defined for
   compatibility.
 */
#ifndef wxT
    /*
        Notice that we use an intermediate macro to allow x to be expanded
        if it's a macro itself.
     */
    #define wxT(x) wxCONCAT_HELPER(L, x)
#endif /* !defined(wxT) */

/*
    wxT_2 exists only for compatibility with wx 2.x and is the same as wxT() in
    that version but nothing in the newer ones.
 */
#define wxT_2(x) x

/*
   wxS ("wx string") macro can be used to create literals using the same
   representation as wxString does internally, i.e. wchar_t by default
   char in UTF-8-based builds.
 */
#if wxUSE_UNICODE_WCHAR
    /*
        As above with wxT(), wxS() argument is expanded if it's a macro.
     */
    #define wxS(x) wxCONCAT_HELPER(L, x)
#else /* wxUSE_UNICODE_UTF8 */
    #define wxS(x) x
#endif

/*
    _T() is a synonym for wxT() familiar to Windows programmers. As this macro
    has even higher risk of conflicting with system headers, its use is
    discouraged and you may predefine wxNO__T to disable it. Additionally, we
    do it ourselves for Sun CC which is known to use it in its standard headers
    (see #10660).
 */
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #ifndef wxNO__T
        #define wxNO__T
    #endif
#endif

#if !defined(_T) && !defined(wxNO__T)
    #define _T(x) wxT(x)
#endif

/* a helper macro allowing to make another macro Unicode-friendly, see below */
#define wxAPPLY_T(x) wxT(x)

/*
   Unicode-friendly analogs of the standard __FILE__, DATE and TIME macros.

   These macros exist only for backwards compatibility, there should be no
   reason to use them in the new code, just use the standard macros instead.

   Also note that we must not use the actual macro names for the two latter
   ones, as doing this would prevent ccache from caching the results of
   compiling any file including this header by default, rendering ccache
   ineffective for wxWidgets programs. Hence the use of "##" below and avoiding
   naming these macros in this comment.
 */
#ifndef __TFILE__
    #define __TFILE__ wxAPPLY_T(__FILE__)
#endif

#ifndef __TDATE__
    #define __TDATE__ wxAPPLY_T(__ ## DATE__)
#endif

#ifndef __TTIME__
    #define __TTIME__ wxAPPLY_T(__ ## TIME__)
#endif

#endif /* _WX_WXCHARTYPE_H_ */

