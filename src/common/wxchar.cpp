/////////////////////////////////////////////////////////////////////////////
// Name:        wxchar.cpp
// Purpose:     wxChar implementation
// Author:      Ove Kåven
// Modified by:
// Created:     09/04/99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows copyright
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "wxchar.h"
#endif

// ===========================================================================
// headers, declarations, constants
// ===========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#define _ISOC9X_SOURCE 1 // to get vsscanf()
#define _BSD_SOURCE    1 // to still get strdup()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifndef WX_PRECOMP
  #include "wx/defs.h"
  #include "wx/wxchar.h"
  #include "wx/string.h"
  #include "wx/hash.h"
#endif

#if defined(__WIN32__) && defined(wxNEED_WX_CTYPE_H)
#include <windef.h>
#include <winbase.h>
#include <winnls.h>
#include <winnt.h>
#endif

#if wxUSE_WCHAR_T
size_t WXDLLEXPORT wxMB2WC(wchar_t *buf, const char *psz, size_t n)
{
  if (buf) {
    if (!n || !*psz) {
      if (n) *buf = _T('\0');
      return 0;
    }
    return mbstowcs(buf, psz, n);
  }

  // NB: GNU libc5 wcstombs() is completely broken, don't use it (it doesn't
  //     honor the 3rd parameter, thus it will happily crash here).
#if wxUSE_WCSRTOMBS
  // don't know if it's really needed (or if we can pass NULL), but better safe
  // than quick
  mbstate_t mbstate;  
  return mbsrtowcs((wchar_t *) NULL, &psz, 0, &mbstate);
#else  // !GNU libc
  return mbstowcs((wchar_t *) NULL, psz, 0);
#endif // GNU
}

size_t WXDLLEXPORT wxWC2MB(char *buf, const wchar_t *pwz, size_t n)
{
  if (buf) {
    if (!n || !*pwz) {
      // glibc2.1 chokes on null input
      if (n) *buf = '\0';
      return 0;
    }
    return wcstombs(buf, pwz, n);
  }

  // NB: GNU libc5 wcstombs() is completely broken, don't use it (it doesn't
  //     honor the 3rd parameter, thus it will happily crash here).
#if wxUSE_WCSRTOMBS
  // don't know if it's really needed (or if we can pass NULL), but better safe
  // than quick
  mbstate_t mbstate;  
  return wcsrtombs((char *) NULL, &pwz, 0, &mbstate);
#else  // !GNU libc
  return wcstombs((char *) NULL, pwz, 0);
#endif // GNU
}
#endif

bool WXDLLEXPORT wxOKlibc()
{
#if wxUSE_WCHAR_T && defined(__UNIX__) && defined(__GLIBC__)
  // GNU libc uses UTF-8 even when it shouldn't
  wchar_t res;
  if ((MB_CUR_MAX == 2) &&
      (wxMB2WC(&res, "\xdd\xa5", 1)>0) &&
      (res==0x765)) {
    // this is UTF-8 allright, check whether that's what we want
    char *cur_locale = setlocale(LC_ALL, NULL);
    if ((strlen(cur_locale) < 4) ||
	(strcasecmp(cur_locale + strlen(cur_locale) - 4, "utf8"))) {
      // nope, don't use libc conversion
      return FALSE;
    }
  }
#endif
  return TRUE;
}

#ifdef wxNEED_WCSLEN
size_t   WXDLLEXPORT wcslen(const wchar_t *s)
{
  size_t len;
  while (s[len]) len++;
  return len;
}
#endif

#if defined(__WIN32__) && defined(wxNEED_WX_CTYPE_H)
inline WORD wxMSW_ctype(wxChar ch)
{
  WORD ret;
  GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, &ch, 1, &ret);
  return ret;
}

int WXDLLEXPORT wxIsalnum(wxChar ch) { return IsCharAlphaNumeric(ch); }
int WXDLLEXPORT wxIsalpha(wxChar ch) { return IsCharAlpha(ch); }
int WXDLLEXPORT wxIsctrl(wxChar ch) { return wxMSW_ctype(ch) & C1_CNTRL; }
int WXDLLEXPORT wxIsdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_DIGIT; }
int WXDLLEXPORT wxIsgraph(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_PUNCT|C1_ALPHA); }
int WXDLLEXPORT wxIslower(wxChar ch) { return IsCharLower(ch); }
int WXDLLEXPORT wxIsprint(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_SPACE|C1_PUNCT|C1_ALPHA); }
int WXDLLEXPORT wxIspunct(wxChar ch) { return wxMSW_ctype(ch) & C1_PUNCT; }
int WXDLLEXPORT wxIsspace(wxChar ch) { return wxMSW_ctype(ch) & C1_SPACE; }
int WXDLLEXPORT wxIsupper(wxChar ch) { return IsCharUpper(ch); }
int WXDLLEXPORT wxIsxdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_XDIGIT; }
int WXDLLEXPORT wxTolower(wxChar ch) { return (wxChar)CharLower((LPTSTR)(ch); }
int WXDLLEXPORT wxToupper(wxChar ch) { return (wxChar)CharUpper((LPTSTR)(ch); }
#endif

#ifndef wxStrdup
wxChar * WXDLLEXPORT wxStrdup(const wxChar *psz)
{
  size_t size = (wxStrlen(psz) + 1) * sizeof(wxChar);
  wxChar *ret = (wxChar *) malloc(size);
  memcpy(ret, psz, size);
  return ret;
}
#endif

#ifndef wxStricmp
int WXDLLEXPORT wxStricmp(const wxChar *psz1, const wxChar *psz2)
{
  register wxChar c1, c2;
  do {
    c1 = wxTolower(*psz1++);
    c2 = wxTolower(*psz2++);
  } while ( c1 && (c1 == c2) );
  return c1 - c2;
}
#endif

#ifndef wxStrtok
wxChar * WXDLLEXPORT wxStrtok(wxChar *psz, const wxChar *delim, wxChar **save_ptr)
{
  if (!psz) psz = *save_ptr;
  psz += wxStrspn(psz, delim);
  if (!*psz) {
    *save_ptr = (wxChar *)NULL;
    return (wxChar *)NULL;
  }
  wxChar *ret = psz;
  psz = wxStrpbrk(psz, delim);
  if (!psz) *save_ptr = (wxChar*)NULL;
  else {
    *psz = _T('\0');
    *save_ptr = psz + 1;
  }
  return ret;
}
#endif

#ifndef wxSetlocale
wxChar * WXDLLEXPORT wxSetlocale(int category, const wxChar *locale)
{
  setlocale(category, wxConv_libc.cWX2MB(locale));
  // FIXME
  return (wxChar *)NULL;
}
#endif

#ifdef wxNEED_WX_STDIO_H
int WXDLLEXPORT wxPrintf(const wxChar *fmt, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, fmt);
  ret = wxVprintf(fmt, argptr);
  va_end(argptr);
  return ret;
}

int WXDLLEXPORT wxVprintf(const wxChar *fmt, va_list argptr)
{
  wxString str;
  str.PrintfV(fmt,argptr);
  printf("%s", (const char*)str.mb_str());
  return str.Len();
}

int WXDLLEXPORT wxFprintf(FILE *stream, const wxChar *fmt, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, fmt);
  ret = wxVfprintf(stream, fmt, argptr);
  va_end(argptr);
  return ret;
}

int WXDLLEXPORT wxVfprintf(FILE *stream, const wxChar *fmt, va_list argptr)
{
  wxString str;
  str.PrintfV(fmt,argptr);
  fprintf(stream, "%s", (const char*)str.mb_str());
  return str.Len();
}

int WXDLLEXPORT wxSprintf(wxChar *buf, const wxChar *fmt, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, fmt);
  ret = wxVsprintf(buf, fmt, argptr);
  va_end(argptr);
  return ret;
}

int WXDLLEXPORT wxVsprintf(wxChar *buf, const wxChar *fmt, va_list argptr)
{
  // this might be sort of inefficient, but it doesn't matter since
  // we'd prefer people to use wxString::Printf directly instead anyway
  wxString str;
  str.PrintfV(fmt,argptr);
  wxStrcpy(buf,str.c_str());
  return str.Len();
}

int WXDLLEXPORT wxSscanf(const wxChar *buf, const wxChar *fmt, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, fmt);
  ret = wxVsscanf(buf, fmt, argptr);
  va_end(argptr);
  return ret;
}

int WXDLLEXPORT wxVsscanf(const wxChar *buf, const wxChar *fmt, va_list argptr)
{
  int ret;
  // this will work only for numeric conversion! Strings will not be converted correctly
  // hopefully this is all we'll need
  ret = vsscanf(wxConv_libc.cWX2MB(buf), wxConv_libc.cWX2MB(fmt), argptr);
  return ret;
}
#endif

#ifndef wxAtof
double   WXDLLEXPORT wxAtof(const wxChar *psz)
{
  return atof(wxConv_libc.cWX2MB(psz));
}
#endif

#ifdef wxNEED_WX_STDLIB_H
int      WXDLLEXPORT wxAtoi(const wxChar *psz)
{
  return atoi(wxConv_libc.cWX2MB(psz));
}

long     WXDLLEXPORT wxAtol(const wxChar *psz)
{
  return atol(wxConv_libc.cWX2MB(psz));
}

wxChar * WXDLLEXPORT wxGetenv(const wxChar *name)
{
  static wxHashTable env;
  // check if we already have stored the converted env var
  wxObject *data = env.Get(name);
  if (!data) {
    // nope, retrieve it,
    const char *val = getenv(wxConv_libc.cWX2MB(name));
    if (!val) return (wxChar *)NULL;
    // convert it,
    data = (wxObject *)new wxString(val);
    // and store it
    env.Put(name, data);
  }
  // return converted env var
  return (wxChar *)((wxString *)data)->c_str();
}

int      WXDLLEXPORT wxSystem(const wxChar *psz)
{
  return system(wxConv_libc.cWX2MB(psz));
}

#endif
