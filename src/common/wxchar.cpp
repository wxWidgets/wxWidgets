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
#include <time.h>

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
      if (n) *buf = wxT('\0');
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

#ifndef HAVE_WCSLEN
size_t   WXDLLEXPORT wcslen(const wchar_t *s)
{
  size_t len = 0;
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

WXDLLEXPORT int wxIsalnum(wxChar ch) { return IsCharAlphaNumeric(ch); }
WXDLLEXPORT int wxIsalpha(wxChar ch) { return IsCharAlpha(ch); }
WXDLLEXPORT int wxIsctrl(wxChar ch) { return wxMSW_ctype(ch) & C1_CNTRL; }
WXDLLEXPORT int wxIsdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_DIGIT; }
WXDLLEXPORT int wxIsgraph(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_PUNCT|C1_ALPHA); }
WXDLLEXPORT int wxIslower(wxChar ch) { return IsCharLower(ch); }
WXDLLEXPORT int wxIsprint(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_SPACE|C1_PUNCT|C1_ALPHA); }
WXDLLEXPORT int wxIspunct(wxChar ch) { return wxMSW_ctype(ch) & C1_PUNCT; }
WXDLLEXPORT int wxIsspace(wxChar ch) { return wxMSW_ctype(ch) & C1_SPACE; }
WXDLLEXPORT int wxIsupper(wxChar ch) { return IsCharUpper(ch); }
WXDLLEXPORT int wxIsxdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_XDIGIT; }
WXDLLEXPORT int wxTolower(wxChar ch) { return (wxChar)CharLower((LPTSTR)(ch)); }
WXDLLEXPORT int wxToupper(wxChar ch) { return (wxChar)CharUpper((LPTSTR)(ch)); }
#endif

#ifndef wxStrdup
WXDLLEXPORT wxChar * wxStrdup(const wxChar *psz)
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
WXDLLEXPORT wxChar * wxStrtok(wxChar *psz, const wxChar *delim, wxChar **save_ptr)
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
    *psz = wxT('\0');
    *save_ptr = psz + 1;
  }
  return ret;
}
#endif

#ifndef wxSetlocale
WXDLLEXPORT wxChar * wxSetlocale(int category, const wxChar *locale)
{
  setlocale(category, wxConvLibc.cWX2MB(locale));
  // FIXME
  return (wxChar *)NULL;
}
#endif

#ifdef wxNEED_WX_STRING_H
WXDLLEXPORT wxChar * wxStrcat(wxChar *dest, const wxChar *src)
{
  wxChar *ret = dest;
  while (*dest) dest++;
  while ((*dest++ = *src++));
  return ret;
}

WXDLLEXPORT wxChar * wxStrchr(const wxChar *s, wxChar c)
{
  while (*s && *s != c) s++;
  return (*s) ? (wxChar *)s : (wxChar *)NULL;
}

WXDLLEXPORT int wxStrcmp(const wxChar *s1, const wxChar *s2)
{
  while ((*s1 == *s2) && *s1) s1++, s2++;
  if ((wxUChar)*s1 < (wxUChar)*s2) return -1;
  if ((wxUChar)*s1 > (wxUChar)*s2) return 1;
  return 0;
}

WXDLLEXPORT wxChar * wxStrcpy(wxChar *dest, const wxChar *src)
{
  wxChar *ret = dest;
  while ((*dest++ = *src++));
  return ret;
}

WXDLLEXPORT wxChar * wxStrncat(wxChar *dest, const wxChar *src, size_t n)
{
  wxChar *ret = dest;
  while (*dest) dest++;
  while (n && (*dest++ = *src++)) n--;
  return ret;
}

WXDLLEXPORT int wxStrncmp(const wxChar *s1, const wxChar *s2, size_t n)
{
  while (n && (*s1 == *s2) && *s1) n--, s1++, s2++;
  if (n) {
    if ((wxUChar)*s1 < (wxUChar)*s2) return -1;
    if ((wxUChar)*s1 > (wxUChar)*s2) return 1;
  }
  return 0;
}

WXDLLEXPORT wxChar * wxStrncpy(wxChar *dest, const wxChar *src, size_t n)
{
  wxChar *ret = dest;
  while (n && (*dest++ = *src++)) n--;
  while (n) *dest++=0, n--; // the docs specify padding with zeroes
  return ret;
}

WXDLLEXPORT wxChar * wxStrpbrk(const wxChar *s, const wxChar *accept)
{
  while (*s && !wxStrchr(accept, *s)) s++;
  return (*s) ? (wxChar *)s : (wxChar *)NULL;
}

WXDLLEXPORT wxChar * wxStrrchr(const wxChar *s, wxChar c)
{
  wxChar *ret = (wxChar *)NULL;
  while (*s) {
    if (*s == c) ret = (wxChar *)s;
    s++;
  }
  return ret;
}

WXDLLEXPORT size_t wxStrspn(const wxChar *s, const wxChar *accept)
{
  size_t len = 0;
  while (wxStrchr(accept, *s++)) len++;
  return len;
}

WXDLLEXPORT wxChar * wxStrstr(const wxChar *haystack, const wxChar *needle)
{
  wxChar *fnd;
  while ((fnd = wxStrchr(haystack, *needle))) {
    if (!wxStrcmp(fnd, needle)) return fnd;
    haystack = fnd + 1;
  }
  return (wxChar *)NULL;
}

WXDLLEXPORT double wxStrtod(const wxChar *nptr, wxChar **endptr)
{
  const wxChar *start = nptr;

  // FIXME: only correct for C locale
  while (wxIsspace(*nptr)) nptr++;
  if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
  while (wxIsdigit(*nptr)) nptr++;
  if (*nptr == wxT('.')) {
    nptr++;
    while (wxIsdigit(*nptr)) nptr++;
  }
  if (*nptr == wxT('E') || *nptr == wxT('e')) {
    nptr++;
    if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
    while (wxIsdigit(*nptr)) nptr++;
  }

  wxString data(nptr, nptr-start);
  wxWX2MBbuf dat = data.mb_str(wxConvLibc);
  char *rdat = wxMBSTRINGCAST dat;
  double ret = strtod(dat, &rdat);

  if (endptr) *endptr = (wxChar *)(start + (rdat - (const char *)dat));

  return ret;
}

WXDLLEXPORT long int wxStrtol(const wxChar *nptr, wxChar **endptr, int base)
{
  const wxChar *start = nptr;

  // FIXME: only correct for C locale
  while (wxIsspace(*nptr)) nptr++;
  if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
  if (((base == 0) || (base == 16)) &&
      (nptr[0] == wxT('0') && nptr[1] == wxT('x'))) {
    nptr += 2;
    base = 16;
  }
  else if ((base == 0) && (nptr[0] == wxT('0'))) base = 8;
  else if (base == 0) base = 10;

  while ((wxIsdigit(*nptr) && (*nptr - wxT('0') < base)) ||
         (wxIsalpha(*nptr) && (wxToupper(*nptr) - wxT('A') + 10 < base))) nptr++;

  wxString data(nptr, nptr-start);
  wxWX2MBbuf dat = data.mb_str(wxConvLibc);
  char *rdat = wxMBSTRINGCAST dat;
  long int ret = strtol(dat, &rdat, base);

  if (endptr) *endptr = (wxChar *)(start + (rdat - (const char *)dat));

  return ret;
}
#endif

#ifdef wxNEED_WX_STDIO_H
WXDLLEXPORT FILE * wxFopen(const wxChar *path, const wxChar *mode)
{
  return fopen(wxConvFile.cWX2MB(path), wxConvLibc.cWX2MB(mode));
}

WXDLLEXPORT FILE * wxFreopen(const wxChar *path, const wxChar *mode, FILE *stream)
{
  return freopen(wxConvFile.cWX2MB(path), wxConvLibc.cWX2MB(mode), stream);
}

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
  ret = vsscanf(wxConvLibc.cWX2MB(buf), wxConvLibc.cWX2MB(fmt), argptr);
  return ret;
}
#endif

#ifndef wxAtof
double   WXDLLEXPORT wxAtof(const wxChar *psz)
{
  return atof(wxConvLibc.cWX2MB(psz));
}
#endif

#ifdef wxNEED_WX_STDLIB_H
int      WXDLLEXPORT wxAtoi(const wxChar *psz)
{
  return atoi(wxConvLibc.cWX2MB(psz));
}

long     WXDLLEXPORT wxAtol(const wxChar *psz)
{
  return atol(wxConvLibc.cWX2MB(psz));
}

wxChar * WXDLLEXPORT wxGetenv(const wxChar *name)
{
  static wxHashTable env;
  // check if we already have stored the converted env var
  wxObject *data = env.Get(name);
  if (!data) {
    // nope, retrieve it,
    const char *val = getenv(wxConvLibc.cWX2MB(name));
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
  return system(wxConvLibc.cWX2MB(psz));
}

#endif

#ifdef wxNEED_WX_TIME_H
WXDLLEXPORT size_t   wxStrftime(wxChar *s, size_t max, const wxChar *fmt, const struct tm *tm)
{
  if (!max) return 0;
  char *buf = (char *)malloc(max);
  size_t ret = strftime(buf, max, wxConvLibc.cWX2MB(fmt), tm);
  if (ret) {
    wxStrcpy(s, wxConvLibc.cMB2WX(buf));
    free(buf);
    return wxStrlen(s);
  } else {
    free(buf);
    *s = 0;
    return 0;
  }
}
#endif
