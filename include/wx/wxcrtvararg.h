///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxcrtvararg.h
// Purpose:     Type-safe ANSI and Unicode builds compatible wrappers for
//              printf(), scanf() and related CRT functions
// Author:      Joel Farley, Ove Kåven
// Modified by: Vadim Zeitlin, Robert Roebling, Ron Lee
// Created:     2007-02-19
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXCRTVARARG_H_
#define _WX_WXCRTVARARG_H_

#include "wx/wxcrt.h"
#include "wx/strvararg.h"


/* Required for wxPrintf() etc */
#include <stdarg.h>

#ifdef wxHAVE_TCHAR_SUPPORT
    WX_DEFINE_VARARG_FUNC(int, wxFprintf, _ftprintf)
    WX_DEFINE_VARARG_FUNC(int, wxPrintf, _tprintf)
    #define  wxPutc(c,f) _puttc(WXWCHAR_T_CAST(c),f)
    #define  wxPutchar   _puttchar
    #define  wxPuts      _putts
    #define  wxScanf     _tscanf  /* FIXME-UTF8: not wrapped */
    #if defined(__DMC__)
        #if wxUSE_UNICODE
            /* Digital Mars adds count to _stprintf (C99) so prototype conversion see wxchar.cpp */
            int wxDoSprintf (wchar_t * __RESTRICT s, const wchar_t * __RESTRICT format, ... ) ;
            WX_DEFINE_VARARG_FUNC(int, wxSprintf, wxDoSprintf)
        #else
            /* and there is a bug in D Mars tchar.h prior to 8.39.4n, so define
               as sprintf */
            WX_DEFINE_VARARG_FUNC(int, wxSprintf, sprintf)
        #endif
    #else
        WX_DEFINE_VARARG_FUNC(int, wxSprintf, _stprintf)
    #endif

    /* FIXME-UTF8: not wrapped */
    #define  wxSscanf    _stscanf
    #define  wxVfprintf  _vftprintf
    #define  wxVprintf   _vtprintf
    #define  wxVsscanf   _vstscanf
    #define  wxVsprintf  _vstprintf

#else /* !TCHAR-aware compilers */

    #if !wxUSE_UNICODE /* ASCII */
        WX_DEFINE_VARARG_FUNC(int, wxFprintf, fprintf)
        #define  wxFscanf    fscanf
        WX_DEFINE_VARARG_FUNC(int, wxPrintf, printf)
        #define  wxScanf     scanf
        WX_DEFINE_VARARG_FUNC(int, wxSprintf, sprintf)
        #define  wxSscanf    sscanf
        #define  wxVfprintf  vfprintf
        #define  wxVprintf   vprintf
        #define  wxVsscanf   vsscanf
        #define  wxVsprintf  vsprintf
    #endif /* ASCII */
#endif /* TCHAR-aware compilers/the others */

/* printf() family saga */

/*
   For some systems [v]snprintf() exists in the system libraries but not in the
   headers, so we need to declare it ourselves to be able to use it.
 */
#if defined(HAVE_VSNPRINTF) && !defined(HAVE_VSNPRINTF_DECL)
#ifdef __cplusplus
    extern "C"
#else
    extern
#endif
    int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif /* !HAVE_VSNPRINTF_DECL */

#if defined(HAVE_SNPRINTF) && !defined(HAVE_SNPRINTF_DECL)
#ifdef __cplusplus
    extern "C"
#else
    extern
#endif
    WXDLLIMPEXP_BASE int snprintf(char *str, size_t size, const char *format, ...);
#endif /* !HAVE_SNPRINTF_DECL */

/* Wrapper for vsnprintf if it's 3rd parameter is non-const. Note: the
 * same isn't done for snprintf below, the builtin wxSnprintf_ is used
 * instead since it's already a simple wrapper */
#if defined __cplusplus && defined HAVE_BROKEN_VSNPRINTF_DECL
    inline int wx_fixed_vsnprintf(char *str, size_t size, const char *format, va_list ap)
    {
        return vsnprintf(str, size, (char*)format, ap);
    }
#endif

/*
   MinGW MSVCRT has non-standard vswprintf() (for MSVC compatibility
   presumably) and normally _vsnwprintf() is used instead
 */
#if defined(HAVE_VSWPRINTF) && defined(__MINGW32__)
    #undef HAVE_VSWPRINTF
#endif

#if wxUSE_PRINTF_POS_PARAMS
    /*
        The systems where vsnprintf() supports positional parameters should
        define the HAVE_UNIX98_PRINTF symbol.

        On systems which don't (e.g. Windows) we are forced to use
        our wxVsnprintf() implementation.
    */
    #if defined(HAVE_UNIX98_PRINTF)
        #if wxUSE_UNICODE
            #ifdef HAVE_VSWPRINTF
                #define wxVsnprintf_        vswprintf
            #endif
        #else /* ASCII */
            #ifdef HAVE_BROKEN_VSNPRINTF_DECL
                #define wxVsnprintf_    wx_fixed_vsnprintf
            #else
                #define wxVsnprintf_    vsnprintf
            #endif
        #endif
    #else /* !HAVE_UNIX98_PRINTF */
        /*
            The only compiler with positional parameters support under Windows
            is VC++ 8.0 which provides a new xxprintf_p() functions family.
            The 2003 PSDK includes a slightly earlier version of VC8 than the
            main release and does not have the printf_p functions.
         */
        #if defined _MSC_FULL_VER && _MSC_FULL_VER >= 140050727 && !defined __WXWINCE__
            #if wxUSE_UNICODE
                #define wxVsnprintf_    _vswprintf_p
            #else
                #define wxVsnprintf_    _vsprintf_p
            #endif
        #endif
    #endif /* HAVE_UNIX98_PRINTF/!HAVE_UNIX98_PRINTF */
#else /* !wxUSE_PRINTF_POS_PARAMS */
    /*
       We always want to define safe snprintf() function to be used instead of
       sprintf(). Some compilers already have it (or rather vsnprintf() which
       we really need...), otherwise we implement it using our own printf()
       code.

       We define function with a trailing underscore here because the real one
       is a wrapper around it as explained below
     */

    /* first deal with TCHAR-aware compilers which have _vsntprintf */
    #ifndef wxVsnprintf_
        #if defined(__VISUALC__) || \
                (defined(__BORLANDC__) && __BORLANDC__ >= 0x540)
            #define wxSnprintf_     _sntprintf
            #define wxVsnprintf_    _vsntprintf
            WX_DEFINE_VARARG_FUNC(int, wxSnprintf_, _sntprintf)
        #endif
    #endif

    /* if this didn't work, define it separately for Unicode and ANSI builds */
    #ifndef wxVsnprintf_
        #if wxUSE_UNICODE
            #if defined(HAVE__VSNWPRINTF)
                #define wxVsnprintf_    _vsnwprintf
            #elif defined(HAVE_VSWPRINTF)
                #define wxVsnprintf_     vswprintf
            #elif defined(__WATCOMC__)
                #define wxVsnprintf_    _vsnwprintf
                WX_DEFINE_VARARG_FUNC(int, wxSnprintf_, _snwprintf)
            #endif
        #else /* ASCII */
            /*
               All versions of CodeWarrior supported by wxWidgets apparently
               have both snprintf() and vsnprintf()
             */
            #if defined(HAVE_SNPRINTF) \
                || defined(__MWERKS__) || defined(__WATCOMC__)
                #ifndef HAVE_BROKEN_SNPRINTF_DECL
                    WX_DEFINE_VARARG_FUNC(int, wxSnprintf_, snprintf)
                #endif
            #endif
            #if defined(HAVE_VSNPRINTF) \
                || defined(__MWERKS__) || defined(__WATCOMC__)
                #ifdef HAVE_BROKEN_VSNPRINTF_DECL
                    #define wxVsnprintf_    wx_fixed_vsnprintf
                #else
                    #define wxVsnprintf_    vsnprintf
                #endif
            #endif
        #endif /* Unicode/ASCII */
    #endif /* wxVsnprintf_ */
#endif /* wxUSE_PRINTF_POS_PARAMS/!wxUSE_PRINTF_POS_PARAMS */

#ifndef wxSnprintf_
    /* no snprintf(), cook our own */
    WXDLLIMPEXP_BASE int
    wxDoSnprintf_(wxChar *buf, size_t len,
                  const wxChar *format, ...) ATTRIBUTE_PRINTF_3;
    WX_DEFINE_VARARG_FUNC(int, wxSnprintf_, wxDoSnprintf_)
#endif
#ifndef wxVsnprintf_
    /* no (suitable) vsnprintf(), cook our own */
    WXDLLIMPEXP_BASE int
    wxVsnprintf_(wxChar *buf, size_t len,
                 const wxChar *format, va_list argptr);

    #define wxUSE_WXVSNPRINTF 1
#else
    #define wxUSE_WXVSNPRINTF 0
#endif

/*
   In Unicode mode we need to have all standard functions such as wprintf() and
   so on but not all systems have them so use our own implementations in this
   case.
 */
#if wxUSE_UNICODE && !defined(wxHAVE_TCHAR_SUPPORT) && !defined(HAVE_WPRINTF)
    #define wxNEED_WPRINTF
#endif

/*
   More Unicode complications: although both ANSI C and C++ define a number of
   wide character functions such as wprintf(), not all environments have them.
   Worse, those which do have different behaviours: under Windows, %s format
   specifier changes its meaning in Unicode build and expects a Unicode string
   while under Unix/POSIX it still means an ASCII string even for wprintf() and
   %ls has to be used for wide strings.

   We choose to always emulate Windows behaviour as more useful for us so even
   if we have wprintf() we still must wrap it in a non trivial wxPrintf().

*/

#if defined(wxNEED_PRINTF_CONVERSION) || defined(wxNEED_WPRINTF)
    /*
        we need to implement all wide character printf and scanf functions
        either because we don't have them at all or because they don't have the
        semantics we need
     */
    WX_DEFINE_VARARG_FUNC(int, wxScanf, wxDoScanf)
    int wxDoScanf( const wxChar *format, ... ) ATTRIBUTE_PRINTF_1;

    WX_DEFINE_VARARG_FUNC(int, wxSscanf, wxDoSscanf)
    int wxDoSscanf( const wxChar *str, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;

    WX_DEFINE_VARARG_FUNC(int, wxFscanf, wxDoFscanf)
    int wxDoFscanf( FILE *stream, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;

    WX_DEFINE_VARARG_FUNC(int, wxPrintf, wxDoPrintf)
    int wxDoPrintf( const wxChar *format, ... ) ATTRIBUTE_PRINTF_1;

    WX_DEFINE_VARARG_FUNC(int, wxSprintf, wxDoSprintf)
    int wxDoSprintf( wxChar *str, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;

    WX_DEFINE_VARARG_FUNC(int, wxFprintf, wxDoFprintf)
    int wxDoFprintf( FILE *stream, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;

    int wxVsscanf( const wxChar *str, const wxChar *format, va_list ap );
    int wxVfprintf( FILE *stream, const wxChar *format, va_list ap );
    int wxVprintf( const wxChar *format, va_list ap );
    int wxVsprintf( wxChar *str, const wxChar *format, va_list ap );
#endif /* wxNEED_PRINTF_CONVERSION */

/* these 2 can be simply mapped to the versions with underscore at the end */
/* if we don't have to do the conversion */
/*
   However, if we don't have any vswprintf() at all we don't need to redefine
   anything as our own wxVsnprintf_() already behaves as needed.
*/
#if defined(wxNEED_PRINTF_CONVERSION) && defined(wxVsnprintf_)
    WX_DEFINE_VARARG_FUNC(int, wxSnprintf, wxDoSnprintf)
    int wxDoSnprintf( wxChar *str, size_t size, const wxChar *format, ... ) ATTRIBUTE_PRINTF_3;
    int wxVsnprintf( wxChar *str, size_t size, const wxChar *format, va_list ap );
#else
    #define wxSnprintf wxSnprintf_
    #define wxVsnprintf wxVsnprintf_
#endif

#endif /* _WX_WXCRTVARARG_H_ */
