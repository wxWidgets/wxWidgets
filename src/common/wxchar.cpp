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

#include "wx/msgdlg.h"

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

  // assume that we have mbsrtowcs() too if we have wcsrtombs()
#ifdef HAVE_WCSRTOMBS
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

#if HAVE_WCSRTOMBS
  mbstate_t mbstate;
  return wcsrtombs((char *) NULL, &pwz, 0, &mbstate);
#else  // !GNU libc
  return wcstombs((char *) NULL, pwz, 0);
#endif // GNU
}
#endif // wxUSE_WCHAR_T

bool WXDLLEXPORT wxOKlibc()
{
#if wxUSE_WCHAR_T && defined(__UNIX__) && defined(__GLIBC__)
  // glibc 2.0 uses UTF-8 even when it shouldn't
  wchar_t res = 0;
  if ((MB_CUR_MAX == 2) &&
      (wxMB2WC(&res, "\xdd\xa5", 1) == 1) &&
      (res==0x765)) {
    // this is UTF-8 allright, check whether that's what we want
    char *cur_locale = setlocale(LC_CTYPE, NULL);
    if ((strlen(cur_locale) < 4) ||
	(strcasecmp(cur_locale + strlen(cur_locale) - 4, "utf8")) ||
	(strcasecmp(cur_locale + strlen(cur_locale) - 5, "utf-8"))) {
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

#ifndef wxStricmp
int WXDLLEXPORT wxStrnicmp(const wxChar *s1, const wxChar *s2, size_t n)
{
  register wxChar c1, c2;
  while (n && ((c1 = wxTolower(*s1)) == (c2 = wxTolower(*s2)) ) && c1) n--, s1++, s2++;
  if (n) {
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
  }
  return 0;
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
WXDLLEXPORT wxWCharBuffer wxSetlocale(int category, const wxChar *locale)
{
    char *localeOld = setlocale(category, wxConvLocal.cWX2MB(locale));

    return wxWCharBuffer(wxConvLocal.cMB2WC(localeOld));
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

WXDLLEXPORT const wxChar * wxStrchr(const wxChar *s, wxChar c)
{
    // be careful here as the terminating NUL makes part of the string
    while ( *s != c )
    {
        if ( !*s++ )
            return NULL;
    }

    return s;
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

WXDLLEXPORT const wxChar * wxStrpbrk(const wxChar *s, const wxChar *accept)
{
  while (*s && !wxStrchr(accept, *s))
      s++;

  return *s ? s : NULL;
}

WXDLLEXPORT const wxChar * wxStrrchr(const wxChar *s, wxChar c)
{
    const wxChar *ret = NULL;
    do
    {
        if ( *s == c )
            ret = s;
        s++;
    }
    while ( *s );

    return ret;
}

WXDLLEXPORT size_t wxStrspn(const wxChar *s, const wxChar *accept)
{
  size_t len = 0;
  while (wxStrchr(accept, *s++)) len++;
  return len;
}

WXDLLEXPORT const wxChar *wxStrstr(const wxChar *haystack, const wxChar *needle)
{
    wxCHECK_RET( needle, NULL, _T("NULL argument in wxStrstr") );

    // VZ: this is not exactly the most efficient string search algorithm...

    const size_t len = wxStrlen(needle);

    while ( const wxChar *fnd = wxStrchr(haystack, *needle) )
    {
        if ( !wxStrncmp(fnd, needle, len) )
            return fnd;

        haystack = fnd + 1;
    }

    return NULL;
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
  wxWX2MBbuf dat = data.mb_str(wxConvLocal);
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
  wxWX2MBbuf dat = data.mb_str(wxConvLocal);
  char *rdat = wxMBSTRINGCAST dat;
  long int ret = strtol(dat, &rdat, base);

  if (endptr) *endptr = (wxChar *)(start + (rdat - (const char *)dat));

  return ret;
}
#endif

#ifdef wxNEED_WX_STDIO_H
WXDLLEXPORT FILE * wxFopen(const wxChar *path, const wxChar *mode)
{
    char mode_buffer[10];
    for (size_t i = 0; i < wxStrlen(mode)+1; i++)
       mode_buffer[i] = (char) mode[i];
    
    return fopen( wxConvFile.cWX2MB(path), mode_buffer );
}

WXDLLEXPORT FILE * wxFreopen(const wxChar *path, const wxChar *mode, FILE *stream)
{
    char mode_buffer[10];
    for (size_t i = 0; i < wxStrlen(mode)+1; i++)
       mode_buffer[i] = (char) mode[i];
    
    return freopen( wxConvFile.cWX2MB(path), mode_buffer, stream );
}

WXDLLEXPORT int wxRemove(const wxChar *path)
{
    return remove( wxConvFile.cWX2MB(path) );
}

WXDLLEXPORT int wxRename(const wxChar *oldpath, const wxChar *newpath)
{
    return rename( wxConvFile.cWX2MB(oldpath), wxConvFile.cWX2MB(newpath) );
}
#endif

#ifndef wxAtof
double   WXDLLEXPORT wxAtof(const wxChar *psz)
{
  return atof(wxConvLocal.cWX2MB(psz));
}
#endif

#ifdef wxNEED_WX_STDLIB_H
int      WXDLLEXPORT wxAtoi(const wxChar *psz)
{
  return atoi(wxConvLocal.cWX2MB(psz));
}

long     WXDLLEXPORT wxAtol(const wxChar *psz)
{
  return atol(wxConvLocal.cWX2MB(psz));
}

wxChar * WXDLLEXPORT wxGetenv(const wxChar *name)
{
  static wxHashTable env;
  
  // check if we already have stored the converted env var
  wxObject *data = env.Get(name);
  if (!data)
  {
    // nope, retrieve it,
#if wxUSE_UNICODE
    wxCharBuffer buffer = wxConvLocal.cWX2MB(name);
    // printf( "buffer %s\n", (const char*) buffer );
    const char *val = getenv( (const char *)buffer );
#else
    const char *val = getenv( name );
#endif

    if (!val) return (wxChar *)NULL;
    // printf( "home %s\n", val );
    
    // convert it,
#ifdef wxUSE_UNICODE
    data = (wxObject *)new wxString(val, wxConvLocal);
#else
    data = (wxObject *)new wxString(val);
#endif

    // and store it
    env.Put(name, data);
  }
  // return converted env var
  return (wxChar *)((wxString *)data)->c_str();
}

int WXDLLEXPORT wxSystem(const wxChar *psz)
{
    return system(wxConvLocal.cWX2MB(psz));
}

#endif

#ifdef wxNEED_WX_TIME_H
WXDLLEXPORT size_t   wxStrftime(wxChar *s, size_t max, const wxChar *fmt, const struct tm *tm)
{
    if (!max) return 0;
    
    char *buf = (char *)malloc(max);
    size_t ret = strftime(buf, max, wxConvLocal.cWX2MB(fmt), tm);
    if (ret)
    {
        wxStrcpy(s, wxConvLocal.cMB2WX(buf));
        free(buf);
        return wxStrlen(s);
    } 
    else
    {
        free(buf);
        *s = 0;
        return 0;
  }
}
#endif
