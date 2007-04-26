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

#include "wx/string.h"

// ----------------------------------------------------------------------------
// CRT functions aliases
// ----------------------------------------------------------------------------

/* Required for wxPrintf() etc */
#include <stdarg.h>

#ifdef wxHAVE_TCHAR_SUPPORT
    #define  wxCRT_Fprintf   _ftprintf
    #define  wxCRT_Printf    _tprintf
    #define  wxCRT_Scanf     _tscanf

    #define  wxCRT_Sscanf    _stscanf
    #define  wxCRT_Vfprintf  _vftprintf
    #define  wxCRT_Vprintf   _vtprintf
    #define  wxCRT_Vsscanf   _vstscanf
    #define  wxCRT_Vsprintf  _vstprintf

#else /* !TCHAR-aware compilers */

    #if !wxUSE_UNICODE /* ASCII */
        #define  wxCRT_Fprintf   fprintf
        #define  wxCRT_Fscanf    fscanf
        #define  wxCRT_Printf    printf
        #define  wxCRT_Scanf     scanf
        #define  wxCRT_Sscanf    sscanf
        #define  wxCRT_Vfprintf  vfprintf
        #define  wxCRT_Vprintf   vprintf
        #define  wxCRT_Vsscanf   vsscanf
        #define  wxCRT_Vsprintf  vsprintf
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
            #define wxVsnprintf_    _vsntprintf
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
            #endif
        #else /* ASCII */
            /*
               All versions of CodeWarrior supported by wxWidgets apparently
               have both snprintf() and vsnprintf()
             */
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

#ifndef wxVsnprintf_
    /* no (suitable) vsnprintf(), cook our own */
    WXDLLIMPEXP_BASE int
    wxVsnprintf_(wxChar *buf, size_t len, const wxChar *format, va_list argptr);

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
    int wxCRT_Scanf( const wxChar *format, ... ) ATTRIBUTE_PRINTF_1;
    int wxCRT_Sscanf( const wxChar *str, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;
    int wxCRT_Fscanf( FILE *stream, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;
    int wxCRT_Vsscanf( const wxChar *str, const wxChar *format, va_list ap );
    int wxCRT_Printf( const wxChar *format, ... ) ATTRIBUTE_PRINTF_1;
    int wxCRT_Fprintf( FILE *stream, const wxChar *format, ... ) ATTRIBUTE_PRINTF_2;
    int wxCRT_Vfprintf( FILE *stream, const wxChar *format, va_list ap );
    int wxCRT_Vprintf( const wxChar *format, va_list ap );
    int wxCRT_Vsprintf( wxChar *str, const wxChar *format, va_list ap );
#endif /* wxNEED_PRINTF_CONVERSION */

/* these 2 can be simply mapped to the versions with underscore at the end */
/* if we don't have to do the conversion */
/*
   However, if we don't have any vswprintf() at all we don't need to redefine
   anything as our own wxVsnprintf_() already behaves as needed.
*/
#if defined(wxNEED_PRINTF_CONVERSION) && defined(wxVsnprintf_)
    int wxCRT_Vsnprintf( wxChar *str, size_t size, const wxChar *format, va_list ap );
#else
    #define wxCRT_Vsnprintf wxVsnprintf_
#endif


// ----------------------------------------------------------------------------
// user-friendly wrappers to CRT functions
// ----------------------------------------------------------------------------

#ifdef __WATCOMC__
    // workaround for http://bugzilla.openwatcom.org/show_bug.cgi?id=351
    #define wxPrintf    wxPrintf_Impl
    #define wxFprintf   wxFprintf_Impl
    #define wxSprintf   wxSprintf_Impl
    #define wxSnprintf  wxSnprintf_Impl
#endif

// FIXME-UTF8: explicit wide-string and short-string format specifiers
//             (%hs, %ls and variants) are currently broken, only %s works
//             as expected (regardless of the build)

// FIXME-UTF8: %c (and %hc, %lc) don't work as expected either: in UTF-8 build,
//             we should replace them with %s (as some Unicode chars may be
//             encoded with >1 bytes) and in all builds, we should use wchar_t
//             for all characters and convert char to it;
//             we'll also need wxArgNormalizer<T> specializations for char,
//             wchar_t, wxUniChar and wxUniCharRef to handle this correctly

WX_DEFINE_VARARG_FUNC2(int, wxPrintf, 1, (const wxString&),
                       wxCRT_Printf, printf)
WX_DEFINE_VARARG_FUNC2(int, wxFprintf, 2, (FILE*, const wxString&),
                       wxCRT_Fprintf, fprintf)

// va_list versions of printf functions simply forward to the respective
// CRT function; note that they assume that va_list was created using
// wxArgNormalizer<T>!
#if wxUSE_UNICODE_UTF8
    #if wxUSE_UTF8_LOCALE_ONLY
        #define WX_VARARG_VFOO_IMPL(args, implWchar, implUtf8)              \
            return implUtf8 args
    #else
        #define WX_VARARG_VFOO_IMPL(args, implWchar, implUtf8)              \
            if ( wxLocaleIsUtf8 ) return implUtf8 args;                     \
            else return implWchar args
    #endif
#else // wxUSE_UNICODE_WCHAR / ANSI
    #define WX_VARARG_VFOO_IMPL(args, implWchar, implUtf8)                  \
        return implWchar args
#endif

inline int
wxVprintf(const wxString& format, va_list ap)
{
    WX_VARARG_VFOO_IMPL((format, ap), wxCRT_Vprintf, vprintf);
}

inline int
wxVfprintf(FILE *f, const wxString& format, va_list ap)
{
    WX_VARARG_VFOO_IMPL((f, format, ap), wxCRT_Vfprintf, vfprintf);
}

#undef WX_VARARG_VFOO_IMPL


// wxSprintf() and friends have to be implemented in two forms, one for
// writing to char* buffer and one for writing to wchar_t*:

int WXDLLIMPEXP_BASE wxDoSprintf(char *str, const wxString& format, ...);
WX_DEFINE_VARARG_FUNC(int, wxSprintf, 2, (char*, const wxString&),
                      wxDoSprintf)

int WXDLLIMPEXP_BASE
wxVsprintf(char *str, const wxString& format, va_list argptr);

int WXDLLIMPEXP_BASE wxDoSnprintf(char *str, size_t size, const wxString& format, ...);
WX_DEFINE_VARARG_FUNC(int, wxSnprintf, 3, (char*, size_t, const wxString&),
                      wxDoSnprintf)

int WXDLLIMPEXP_BASE
wxVsnprintf(char *str, size_t size, const wxString& format, va_list argptr);

#if wxUSE_UNICODE
int WXDLLIMPEXP_BASE wxDoSprintf(wchar_t *str, const wxString& format, ...);
WX_DEFINE_VARARG_FUNC(int, wxSprintf, 2, (wchar_t*, const wxString&),
                      wxDoSprintf)

int WXDLLIMPEXP_BASE
wxVsprintf(wchar_t *str, const wxString& format, va_list argptr);

int WXDLLIMPEXP_BASE wxDoSnprintf(wchar_t *str, size_t size, const wxString& format, ...);
WX_DEFINE_VARARG_FUNC(int, wxSnprintf, 3, (wchar_t*, size_t, const wxString&),
                      wxDoSnprintf)

int WXDLLIMPEXP_BASE
wxVsnprintf(wchar_t *str, size_t size, const wxString& format, va_list argptr);
#endif // wxUSE_UNICODE

#ifdef __WATCOMC__
    // workaround for http://bugzilla.openwatcom.org/show_bug.cgi?id=351
    //
    // fortunately, OpenWatcom implements __VA_ARGS__, so we can provide macros
    // that cast the format argument to wxString:
    #undef wxPrintf
    #undef wxFprintf
    #undef wxSprintf
    #undef wxSnprintf

    #define wxPrintf(fmt, ...) \
            wxPrintf_Impl(wxString(fmt), __VA_ARGS__)
    #define wxFprintf(f, fmt, ...) \
            wxFprintf_Impl(f, wxString(fmt), __VA_ARGS__)
    #define wxSprintf(s, fmt, ...) \
            wxSprintf_Impl(s, wxString(fmt), __VA_ARGS__)
    #define wxSnprintf(s, n, fmt, ...) \
            wxSnprintf_Impl(s, n, wxString(fmt), __VA_ARGS__)
#endif // __WATCOMC__


// We can't use wxArgNormalizer<T> for variadic arguments to wxScanf() etc.
// because they are writable, so instead of providing friendly template
// vararg-like functions, we just provide both char* and wchar_t* variants
// of these functions. The type of output variadic arguments for %s must match
// the type of 'str' and 'format' arguments.

// FIXME-UTF8: actually do it, for now we only have wxChar* variants:
#define wxScanf   wxCRT_Scanf
#define wxFscanf  wxCRT_Fscanf
#define wxSscanf  wxCRT_Sscanf
#define wxVsscanf wxCRT_Vsscanf

#endif /* _WX_WXCRTVARARG_H_ */
