/////////////////////////////////////////////////////////////////////////////
// Name:        wxchar.h
// Purpose:     Declarations common to wx char/wchar_t usage (wide chars)
// Author:      Joel Farley
// Modified by:
// Created:     1998/06/12
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows copyright
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXCHAR_H_
#define _WX_WXCHAR_H_

#ifdef __GNUG__
#pragma interface "wxchar.h"
#endif

// only do SBCS or _UNICODE
#if defined (_MBCS )
#error "MBCS is not supported by wxChar"
#endif

// set wxUSE_UNICODE to 1 if UNICODE or _UNICODE is defined
#if defined(_UNICODE) || defined(UNICODE)
#undef wxUSE_UNICODE
#define wxUSE_UNICODE 1
#else
#ifndef wxUSE_UNICODE
#define wxUSE_UNICODE 0
#endif
#endif

// and vice versa: define UNICODE and _UNICODE if wxUSE_UNICODE is 1...
#if wxUSE_UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif
#endif

// Windows (VC++) has broad TCHAR support
#if defined(__VISUALC__) && defined(__WIN32__)

#include <tchar.h>
#if wxUSE_UNICODE // temporary - preserve binary compatibility
typedef  _TCHAR      wxChar;
typedef  _TSCHAR     wxSChar;
typedef  _TUCHAR     wxUChar;
#else
#define wxChar char
#define wxSChar signed char
#define wxUChar unsigned char
#endif

   // ctype.h functions
#define  wxIsalnum   _istalnum
#define  wxIsalpha   _istalpha
#define  wxIsctrl    _istctrl
#define  wxIsdigit   _istdigit
#define  wxIsgraph   _istgraph
#define  wxIslower   _istlower
#define  wxIsprint   _istprint
#define  wxIspunct   _istpunct
#define  wxIsspace   _istspace
#define  wxIsupper   _istupper
#define  wxIsxdigit  _istxdigit
#define  wxTolower   _totlower
#define  wxToupper   _totupper

   // locale.h functons
#define  wxSetlocale _tsetlocale

   // string.h functions
#define  wxStrcat    _tcscat
#define  wxStrchr    _tcschr
#define  wxStrcmp    _tcscmp
#define  wxStrcoll   _tcscoll
#define  wxStrcpy    _tcscpy
#define  wxStrcspn   _tcscspn
#define  wxStrftime  _tcsftime
#define  wxStricmp   _tcsicmp
#define  wxStrlen_   _tcslen // used in wxStrlen inline function
#define  wxStrncat   _tcsncat
#define  wxStrncmp   _tcsncmp
#define  wxStrncpy   _tcsncpy
#define  wxStrpbrk   _tcspbrk
#define  wxStrrchr   _tcsrchr
#define  wxStrspn    _tcsspn
#define  wxStrstr    _tcsstr
#define  wxStrtod    _tcstod
// is there a _tcstok[_r] ?
#define  wxStrtol    _tcstol
#define  wxStrtoul   _tcstoul
#define  wxStrxfrm   _tcsxfrm

   // stdio.h functions
#define  wxFgetc     _fgettc
#define  wxFgetchar  _fgettchar
#define  wxFgets     _fgetts
#define  wxFopen     _tfopen
#define  wxFputc     _fputtc
#define  wxFputchar  _fputtchar
#define  wxFprintf   _ftprintf
#define  wxFreopen   _tfreopen
#define  wxFscanf    _ftscanf
#define  wxGetc      _gettc
#define  wxGetchar   _gettchar
#define  wxGets      _getts
#define  wxPerror    _tperror
#define  wxPrintf    _tprintf
#define  wxPutc      _puttc
#define  wxPutchar   _puttchar
#define  wxPuts      _putts
#define  wxRemove    _tremove
#define  wxRename    _trename
#define  wxScanf     _tscanf
#define  wxSprintf   _stprintf
#define  wxSscanf    _stscanf
#define  wxTmpnam    _ttmpnam
#define  wxUngetc    _tungetc
#define  wxVfprint   _vftprintf
#define  wxVprintf   _vtprintf
#define  wxVsscanf   _vstscanf
#define  wxVsprintf  _vstprintf

   // stdlib.h functions
#if !wxUSE_UNICODE
#define  wxAtof      atof
#endif
#define  wxAtoi      _ttoi
#define  wxAtol      _ttol
#define  wxGetenv    _tgetenv
#define  wxSystem    _tsystem

   // time.h functions
#define  wxAsctime   _tasctime
#define  wxCtime     _tctime

// #elif defined(XXX)
   // #include XXX-specific files here
   // typeddef YYY wxChar;

   // translate wxZZZ names

#elif defined(__BORLANDC__) && defined(__WIN32__)

// Borland C++ 4.52 doesn't have much tchar support
// maybe Borland C++ 5.02 has, can't check right now
// but I'll use the Win32 API instead here

#include <tchar.h>
#if wxUSE_UNICODE // temporary - preserve binary compatibility
typedef  _TCHAR      wxChar;
typedef  _TSCHAR     wxSChar;
typedef  _TUCHAR     wxUChar;
#else
#define wxChar char
#define wxSChar signed char
#define wxUChar unsigned char
#endif

#include <windef.h>
#include <winbase.h>
#include <winnls.h>
#include <winnt.h>

   // ctype.h functions
inline WORD __wxMSW_ctype(wxChar ch)
{
  WORD ret;
  GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, &ch, 1, &ret);
  return ret;
}
#define  wxIsalnum(x)  IsCharAlphaNumeric
#define  wxIsalpha     IsCharAlpha
#define  wxIsctrl(x)   (__wxMSW_ctype(x) & C1_CNTRL)
#define  wxIsdigit(x)  (__wxMSW_ctype(x) & C1_DIGIT)
#define  wxIsgraph(x)  (__wxMSW_ctype(x) & (C1_DIGIT|C1_PUNCT|C1_ALPHA))
#define  wxIslower(x)  IsCharLower
#define  wxIsprint(x)  (__wxMSW_ctype(x) & (C1_DIGIT|C1_SPACE|C1_PUNCT|C1_ALPHA))
#define  wxIspunct(x)  (__wxMSW_ctype(x) & C1_PUNCT)
#define  wxIsspace(x)  (__wxMSW_ctype(x) & C1_SPACE)
#define  wxIsupper(x)  IsCharUpper
#define  wxIsxdigit(x) (__wxMSW_ctype(x) & C1_XDIGIT)
#define  wxTolower(x)  (wxChar)CharLower((LPTSTR)(x))
#define  wxToupper(x)  (wxChar)CharUpper((LPTSTR)(x))

// #define  wxStrtok    strtok_r // Borland C++ 4.52 doesn't have strtok_r
#define wxNEED_WX_STRING_H
#define wxNEED_WX_STDIO_H
#define wxNEED_WX_STDLIB_H
#define wxNEED_WX_TIME_H
#define wxNEED_WCSLEN

#else//!Windows

// check whether we are doing Unicode
#if wxUSE_UNICODE

#include <wchar.h>
#include <wctype.h>

// this is probably glibc-specific
#if defined(__WCHAR_TYPE__)

typedef __WCHAR_TYPE__          wxChar;
typedef signed __WCHAR_TYPE__   wxSChar;
typedef unsigned __WCHAR_TYPE__ wxUChar;

#define _T(x)                   L##x

   // ctype.h functions (wctype.h)
#define  wxIsalnum   iswalnum
#define  wxIsalpha   iswalpha
#define  wxIsctrl    iswcntrl
#define  wxIsdigit   iswdigit
#define  wxIsgraph   iswgraph
#define  wxIslower   iswlower
#define  wxIsprint   iswprint
#define  wxIspunct   iswpunct
#define  wxIsspace   iswspace
#define  wxIsupper   iswupper
#define  wxIsxdigit  iswxdigit

#if defined(__GLIBC__) && (__GLIBC__ == 2) && (__GLIBC_MINOR__ == 0)
    // /usr/include/wctype.h incorrectly declares translations tables which
    // provokes tons of compile-time warnings - try to correct this
    #define  wxTolower(wc)   towctrans((wc), (wctrans_t)__ctype_tolower)
    #define  wxToupper(wc)   towctrans((wc), (wctrans_t)__ctype_toupper)
#else
    #define  wxTolower   towlower
    #define  wxToupper   towupper
#endif // gcc/!gcc

   // string.h functions (wchar.h)
#define  wxStrcat    wcscat
#define  wxStrchr    wcschr
#define  wxStrcmp    wcscmp
#define  wxStrcoll   wcscoll
#define  wxStrcpy    wcscpy
#define  wxStrcspn   wcscspn
#define  wxStrlen_   wcslen // used in wxStrlen inline function
#define  wxStrncat   wcsncat
#define  wxStrncmp   wcsncmp
#define  wxStrncpy   wcsncpy
#define  wxStrpbrk   wcspbrk
#define  wxStrrchr   wcsrchr
#define  wxStrspn    wcsspn
#define  wxStrstr    wcsstr
#define  wxStrtod    wcstod
#define  wxStrtok    wcstok
#define  wxStrtol    wcstol
#define  wxStrtoul   wcstoul
#define  wxStrxfrm   wcsxfrm

// glibc doesn't have wc equivalents of the other stuff
#define wxNEED_WX_STDIO_H
#define wxNEED_WX_STDLIB_H
#define wxNEED_WX_TIME_H

#else//!glibc
#error   "Please define your compiler's Unicode conventions in wxChar.h"
#endif
#else//!Unicode

#include <ctype.h>
#include <string.h>

#if 0 // temporary - preserve binary compatibilty
typedef char            wxChar;
typedef signed char     wxSChar;
typedef unsigned char   wxUChar;
#else
#define wxChar char
#define wxSChar signed char
#define wxUChar unsigned char
#endif

#define _T(x)           x

   // ctype.h functions
#define  wxIsalnum   isalnum
#define  wxIsalpha   isalpha
#define  wxIsctrl    isctrl
#define  wxIsdigit   isdigit
#define  wxIsgraph   isgraph
#define  wxIslower   islower
#define  wxIsprint   isprint
#define  wxIspunct   ispunct
#define  wxIsspace   isspace
#define  wxIsupper   isupper
#define  wxIsxdigit  isxdigit
#define  wxTolower   tolower
#define  wxToupper   toupper

   // locale.h functons
#define  wxSetlocale setlocale

   // string.h functions
#define  wxStricmp   strcasecmp
// #define  wxStrtok    strtok_r // this needs a configure check

   // leave the rest to defaults below
#define wxNEED_WX_STRING_H
#define wxNEED_WX_STDIO_H
#define wxNEED_WX_STDLIB_H
#define wxNEED_WX_TIME_H

#endif//Unicode
#endif//TCHAR-aware compilers

// define wxStricmp for various compilers without Unicode possibilities
#if !defined(wxStricmp) && !wxUSE_UNICODE
#if defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__SALFORDC__)
    #define wxStricmp stricmp
#elif defined(__SC__) || defined(__VISUALC__) || (defined(__MWERKS) && defined(__INTEL__))
    #define wxStricmp _stricmp
#elif defined(__UNIX__) || defined(__GNUWIN32__)
    #define wxStricmp strcasecmp
#elif defined(__MWERKS__) && !defined(__INTEL__)
    // use wxWindows' implementation
#else
    // if you leave wxStricmp undefined, wxWindows' implementation will be used
    #error  "Please define string case-insensitive compare for your OS/compiler"
#endif
#endif

// if we need to define for standard headers, and we're not using Unicode,
// just define to standard C library routines
#if !wxUSE_UNICODE
#ifdef wxNEED_WX_STRING_H
#define  wxStrcat    strcat
#define  wxStrchr    strchr
#define  wxStrcmp    strcmp
#define  wxStrcoll   strcoll
#define  wxStrcpy    strcpy
#define  wxStrcspn   strcspn
#define  wxStrdup    strdup
#define  wxStrlen_   strlen // used in wxStrlen inline function
#define  wxStrncat   strncat
#define  wxStrncmp   strncmp
#define  wxStrncpy   strncpy
#define  wxStrpbrk   strpbrk
#define  wxStrrchr   strrchr
#define  wxStrspn    strspn
#define  wxStrstr    strstr
#define  wxStrtod    strtod
#define  wxStrtol    strtol
#define  wxStrtoul   strtoul
#define  wxStrxfrm   strxfrm
#undef wxNEED_WX_STRING_H
#endif

#ifdef wxNEED_WX_STDIO_H
#define  wxFgetc     fgetc
#define  wxFgetchar  fgetchar
#define  wxFgets     fgets
#define  wxFopen     fopen
#define  wxFputc     fputc
#define  wxFputchar  fputchar
#define  wxFprintf   fprintf
#define  wxFreopen   freopen
#define  wxFscanf    fscanf
#define  wxGetc      getc
#define  wxGetchar   getchar
#define  wxGets      gets
#define  wxPerror    perror
#define  wxPrintf    printf
#define  wxPutc      putc
#define  wxPutchar   putchar
#define  wxPuts      puts
#define  wxRemove    remove
#define  wxRename    rename
#define  wxScanf     scanf
#define  wxSprintf   sprintf
#define  wxSscanf    sscanf
#define  wxTmpnam    tmpnam
#define  wxUngetc    ungetc
#define  wxVfprint   vfprintf
#define  wxVprintf   vprintf
#define  wxVsscanf   vsscanf
#define  wxVsprintf  vsprintf
#undef wxNEED_WX_STDIO_H
#endif

#ifdef wxNEED_WX_STDLIB_H
#define  wxAtof      atof
#define  wxAtoi      atoi
#define  wxAtol      atol
#define  wxGetenv    getenv
#define  wxSystem    system
#undef wxNEED_WX_STDLIB_H
#endif

#ifdef wxNEED_WX_TIME_H
#define  wxAsctime   asctime
#define  wxCtime     ctime
#define  wxStrftime  strftime
#undef wxNEED_WX_TIME_H
#endif
#endif //!Unicode

#if defined(wxNEED_WCSLEN) && wxUSE_UNICODE
#define wcslen wxStrlen
#undef wxNEED_WCSLEN
#endif

/// checks whether the passed in pointer is NULL and if the string is empty
inline bool WXDLLEXPORT wxIsEmpty(const wxChar *p) { return !p || !*p; }

#ifndef wxNEED_WX_STRING_H
/// safe version of strlen() (returns 0 if passed NULL pointer)
inline size_t WXDLLEXPORT wxStrlen(const wxChar *psz)
   { return psz ? wxStrlen_(psz) : 0; }
#endif

// multibyte<->widechar conversion
size_t WXDLLEXPORT wxMB2WC(wchar_t *buf, const char *psz, size_t n);
size_t WXDLLEXPORT wxWC2MB(char *buf, const wchar_t *psz, size_t n);
#if wxUSE_UNICODE
#define wxMB2WX wxMB2WC
#define wxWX2MB wxWC2MB
#define wxWC2WX wxStrncpy
#define wxWX2WC wxStrncpy
#else
#define wxMB2WX wxStrncpy
#define wxWX2MB wxStrncpy
#define wxWC2WX wxWC2MB
#define wxWX2WC wxMB2WC
#endif

// if libc versions are not available, use replacements defined in wxchar.cpp
#ifndef wxStrdup
wxChar * WXDLLEXPORT wxStrdup(const wxChar *psz);
#endif

#ifndef wxStricmp
int      WXDLLEXPORT wxStricmp(const wxChar *psz1, const wxChar *psz2);
#endif

#ifndef wxStrtok
wxChar * WXDLLEXPORT wxStrtok(wxChar *psz, const wxChar *delim, wxChar **save_ptr);
#endif

#ifndef wxSetlocale
wxChar * WXDLLEXPORT wxSetlocale(int category, const wxChar *locale);
#endif

#ifdef wxNEED_WCSLEN // for use in buffer.h
size_t   WXDLLEXPORT wcslen(const wchar_t *s);
#endif

#ifdef wxNEED_WX_STRING_H
size_t   WXDLLEXPORT wxStrlen(const wxChar *s);
#endif

#ifdef wxNEED_WX_STDIO_H
#include <stdio.h>
#include <stdarg.h>
int      WXDLLEXPORT wxPrintf(const wxChar *fmt, ...);
int      WXDLLEXPORT wxVprintf(const wxChar *fmt, va_list argptr);
int      WXDLLEXPORT wxFprintf(FILE *stream, const wxChar *fmt, ...);
int      WXDLLEXPORT wxVfprintf(FILE *stream, const wxChar *fmt, va_list argptr);
int      WXDLLEXPORT wxSprintf(wxChar *buf, const wxChar *fmt, ...);
int      WXDLLEXPORT wxVsprintf(wxChar *buf, const wxChar *fmt, va_list argptr);
int      WXDLLEXPORT wxSscanf(const wxChar *buf, const wxChar *fmt, ...);
int      WXDLLEXPORT wxVsscanf(const wxChar *buf, const wxChar *fmt, va_list argptr);
#endif

#ifndef wxAtof
double   WXDLLEXPORT wxAtof(const wxChar *psz);
#endif

#ifdef wxNEED_WX_STDLIB_H
int      WXDLLEXPORT wxAtoi(const wxChar *psz);
long     WXDLLEXPORT wxAtol(const wxChar *psz);
wxChar * WXDLLEXPORT wxGetenv(const wxChar *name);
int      WXDLLEXPORT wxSystem(const wxChar *psz);
#endif

#endif
  //_WX_WXCHAR_H_
