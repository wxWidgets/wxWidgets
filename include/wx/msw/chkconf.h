/*
 * Name:        wx/msw/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * RCS-ID:      $Id$
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_MSW_CHKCONF_H_
#define _WX_MSW_CHKCONF_H_

/*
 * disable the settings which don't work for some compilers
 */

/*
 * If using PostScript-in-MSW in Univ, must enable PostScript
 */

#if defined(__WXUNIVERSAL__) && wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW && !wxUSE_POSTSCRIPT
#undef wxUSE_POSTSCRIPT
#define wxUSE_POSTSCRIPT 1
#endif

#ifndef wxUSE_NORLANDER_HEADERS
#if (defined(__WATCOMC__) && (__WATCOMC__ >= 1200)) || defined(__WINE__) || ((defined(__MINGW32__) || defined(__CYGWIN__)) && ((__GNUC__>2) ||((__GNUC__==2) && (__GNUC_MINOR__>=95))))
#   define wxUSE_NORLANDER_HEADERS 1
#else
#   define wxUSE_NORLANDER_HEADERS 0
#endif
#endif

/*
 * GCC does not have SEH (__try/__catch)
 */

#if defined(__GNUG__)
    #undef wxUSE_ON_FATAL_EXCEPTION
    #define wxUSE_ON_FATAL_EXCEPTION 0
#endif

/*
 * wxUSE_DEBUG_NEW_ALWAYS doesn't work with CodeWarrior
 */

#if defined(__MWERKS__)
    #undef wxUSE_DEBUG_NEW_ALWAYS
    #define wxUSE_DEBUG_NEW_ALWAYS      0
#endif

#if defined(__GNUWIN32__)
/* These don't work as expected for mingw32 and cygwin32 */
#undef  wxUSE_MEMORY_TRACING
#define wxUSE_MEMORY_TRACING            0

#undef  wxUSE_GLOBAL_MEMORY_OPERATORS
#define wxUSE_GLOBAL_MEMORY_OPERATORS   0

#undef  wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS          0

/* Cygwin betas don't have wcslen */
#if defined(__CYGWIN__) || defined(__CYGWIN32__)
#  if ! ((__GNUC__>2) ||((__GNUC__==2) && (__GNUC_MINOR__>=95)))
#    undef wxUSE_WCHAR_T
#    define wxUSE_WCHAR_T 0
#  endif
#endif

#endif
 /* __GNUWIN32__ */

/* MFC duplicates these operators */
#if wxUSE_MFC
#undef  wxUSE_GLOBAL_MEMORY_OPERATORS
#define wxUSE_GLOBAL_MEMORY_OPERATORS   0

#undef  wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS          0
#endif
 /* wxUSE_MFC */

#if (defined(__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS)
/* GnuWin32 doesn't have appropriate headers for e.g. IUnknown. */
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

#if !wxUSE_OWNER_DRAWN
#undef wxUSE_CHECKLISTBOX
#define wxUSE_CHECKLISTBOX 0
#endif

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x500)
/* BC++ 4.0 can't compile JPEG library */
#undef wxUSE_LIBJPEG
#define wxUSE_LIBJPEG 0
#endif

/* wxUSE_DEBUG_NEW_ALWAYS = 1 not compatible with BC++ in DLL mode */
#if defined(__BORLANDC__) && (defined(WXMAKINGDLL) || defined(WXUSINGDLL))
#undef wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS 0
#endif

#if defined(__WXMSW__) && (defined(__WATCOMC__) && __WATCOMC__ < 1200)
#undef wxUSE_WCHAR_T
#define wxUSE_WCHAR_T 0
#endif

#endif
    /* _WX_MSW_CHKCONF_H_ */

