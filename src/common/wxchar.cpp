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

size_t wxMB2WC(wchar_t *buf, const char *psz, size_t n)
{
  if (buf) {
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

size_t wxWC2MB(char *buf, const wchar_t *pwz, size_t n)
{
  if (buf) {
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

#ifndef wxStrdup
wxChar * WXDLLEXPORT wxStrdup(const wxChar *psz)
{
  size_t size = (wxStrlen(psz) + 1) * sizeof(wxChar);
  wxChar *ret = (wxChar *) malloc(size);
  memcpy(ret, psz, size);
  return ret;
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
  ret = wxFvprintf(stream, fmt, argptr);
  va_end(argptr);
  return ret;
}

int WXDLLEXPORT wxFvprintf(FILE *stream, const wxChar *fmt, va_list argptr)
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
