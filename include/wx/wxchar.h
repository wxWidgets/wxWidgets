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

// only do Unicode for Windows (VC++), for now
#if defined(_MSC_VER) && defined(__WIN32__)

#include <tchar.h>
typedef  _TCHAR      wxChar;
typedef  _TSCHAR	 wxSChar;
typedef  _TUCHAR     wxUChar;

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
#define  wxStrncat   _tcsncat
#define  wxStrncmp   _tcsncmp
#define  wxStrncpy   _tcsncpy
#define  wxStrpbrk   _tcspbrk
#define  wxStrrchr   _tcsrchr
#define  wxStrspn    _tcsspn
#define  wxStrstr    _tcsstr
#define  wxStrtod    _tcstod
#define  wxStrtok    _tcstok
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
#define  wxVsprintf  _vstprintf

   // stdlib.h functions
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

#else//!Unicode

// make sure we aren't doing Unicode
#if defined ( __WXUNICODE__ ) || defined ( _UNICODE )
#error   "Please define your compiler's Unicode conventions in wxChar.h"
#endif

typedef char         	wxChar;
typedef signed char	 	wxSChar;
typedef unsigned char	wxUChar;

#define _T(x)        	x

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
#define  wxStrcat    strcat
#define  wxStrchr    strchr
#define  wxStrcmp    strcmp
#define  wxStrcoll   strcoll
#define  wxStrcpy    strcpy
#define  wxStrcspn   strcspn
#define  wxStrftime  strftime
#define  wxStrncat   strncat
#define  wxStrncmp   strncmp
#define  wxStrncpy   strncpy
#define  wxStrpbrk   strpbrk
#define  wxStrrchr   strrchr
#define  wxStrspn    strspn
#define  wxStrstr    strstr
#define  wxStrtod    strtod
#define  wxStrtok    strtok
#define  wxStrtol    strtol
#define  wxStrtoul   strtoul
#define  wxStrxfrm   strxfrm

   // stdio.h functions
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
#define  wxVsprintf  vsprintf

   // stdlib.h functions
#define  wxAtoi      atoi
#define  wxAtol      atol
#define  wxGetenv    getenv
#define  wxSystem    system

   // time.h functions
#define  wxAsctime   asctime
#define  wxCtime     ctime

#endif//Unicode


/// checks whether the passed in pointer is NULL and if the string is empty
inline bool WXDLLEXPORT wxIsEmpty(const wxChar *p) { return !p || !*p; }

/// safe version of strlen() (returns 0 if passed NULL pointer)
inline size_t  WXDLLEXPORT wxStrlen(const wxChar *psz)
#if defined(_MSC_VER)
   { return psz ? _tcslen(psz) : 0; }
#else
   { return psz ? strlen(psz) : 0; }
#endif

/// portable strcasecmp/_stricmp
inline int WXDLLEXPORT wxStricmp(const wxChar *, const wxChar *)
#if defined(_MSC_VER)
   { return _tcsicmp(psz1, psz2); }
#elif defined(__BORLANDC__)
   { return stricmp(psz1, psz2); }
#elif   defined(__UNIX__) || defined(__GNUWIN32__)
   { return strcasecmp(psz1, psz2); }
#else
  // almost all compilers/libraries provide this function (unfortunately under
  // different names), that's why we don't implement our own which will surely
  // be more efficient than this code (uncomment to use):
  /*
    register char c1, c2;
    do {
      c1 = tolower(*psz1++);
      c2 = tolower(*psz2++);
    } while ( c1 && (c1 == c2) );

    return c1 - c2;
  */
  
  #error  "Please define string case-insensitive compare for your OS/compiler"
#endif  // OS/compiler

#endif
  //_WX_WXCHAR_H_
