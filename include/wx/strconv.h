///////////////////////////////////////////////////////////////////////////////
// Name:        strconv.h
// Purpose:     conversion routines for char sets any Unicode
// Author:      Robert Roebling, Ove Kaaven
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Ove Kaaven, Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTRCONVH__
#define _WX_WXSTRCONVH__

#ifdef __GNUG__
  #pragma interface "strconv.h"
#endif

#include "wx/defs.h"
#include "wx/wxchar.h"
#include "wx/buffer.h"

#include <stdlib.h>

#if wxUSE_WCHAR_T

//---------------------------------------------------------------------------
// wxMBConv (base class for conversions, using libc conversion itself)
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConv
{
public:

  // the actual conversion takes place here
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
  
  // No longer inline since BC++ complains.
  const wxWCharBuffer cMB2WC(const char *psz) const;
  const wxCharBuffer cWC2MB(const wchar_t *psz) const;
#if wxUSE_UNICODE
  const wxWCharBuffer cMB2WX(const char *psz) const { return cMB2WC(psz); }
  const wxCharBuffer cWX2MB(const wchar_t *psz) const { return cWC2MB(psz); }
  const wchar_t* cWC2WX(const wchar_t *psz) const { return psz; }
  const wchar_t* cMB2WC(const wchar_t *psz) const { return psz; }
#else
  const char* cMB2WX(const char *psz) const { return psz; }
  const char* cWX2MB(const char *psz) const { return psz; }
  const wxCharBuffer cWC2WX(const wchar_t *psz) const { return cWC2MB(psz); }
  const wxWCharBuffer cWX2WC(const char *psz) const { return cMB2WC(psz); }
#endif
};

WXDLLEXPORT_DATA(extern wxMBConv) wxConvLibc;

//---------------------------------------------------------------------------
// wxMBConvFile (for conversion to filenames)
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConvFile: public wxMBConv 
{
public:
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxMBConvFile) wxConvFile;

//---------------------------------------------------------------------------
// wxMBConvUTF7 (for conversion using UTF7 encoding)
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConvUTF7: public wxMBConv 
{
public:
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxMBConvUTF7) wxConvUTF7;

//---------------------------------------------------------------------------
// wxMBConvUTF8 (for conversion using UTF8 encoding)
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConvUTF8: public wxMBConv 
{
public:
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxMBConvUTF8) wxConvUTF8;

#ifdef __WXGTK12__
//---------------------------------------------------------------------------
// wxMBConvUTF8 (for conversion using GDK's internal converions)
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConvGdk: public wxMBConv
{
public:
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxMBConvGdk) wxConvGdk;
#endif

//---------------------------------------------------------------------------
// wxCSConv (for conversion based on laodable char sets)
//---------------------------------------------------------------------------

class wxCharacterSet;

class WXDLLEXPORT wxCSConv: public wxMBConv
{
private:
  wxChar *m_name;
  wxCharacterSet *m_cset;
  bool m_deferred;
  void SetName(const wxChar *charset);
  
public:
  wxCSConv(const wxChar *charset);
  virtual ~wxCSConv();
  void LoadNow();
  virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
  virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;
};

WXDLLEXPORT_DATA(extern wxCSConv) wxConvLocal;
#define wxConv_local wxConvLocal

WXDLLEXPORT_DATA(extern wxMBConv *) wxConvCurrent;
#define wxConv_current wxConvCurrent

//---------------------------------------------------------------------------
// filename conversion macros
//---------------------------------------------------------------------------

// filenames are multibyte on Unix and probably widechar on Windows?
#if defined(__UNIX__) || defined(__BORLANDC__)
#define wxMBFILES 1
#else
#define wxMBFILES 0
#endif

#if wxMBFILES
#define wxFNCONV(name) wxConvFile.cWX2MB(name)
#define FNSTRINGCAST MBSTRINGCAST
#else
#define wxFNCONV(name) name
#define FNSTRINGCAST WXSTRINGCAST
#endif

#else
  // !wxUSE_WCHAR_T

//---------------------------------------------------------------------------
// stand-ins in absence of wchar_t
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMBConv 
{
public:
  const char* cMB2WX(const char *psz) const { return psz; }
  const char* cWX2MB(const char *psz) const { return psz; }
};
WXDLLEXPORT_DATA(extern wxMBConv) wxConvLibc, wxConvFile;
WXDLLEXPORT_DATA(extern wxMBConv *) wxConvCurrent;

#define wxFNCONV(name) name
#define FNSTRINGCAST WXSTRINGCAST

#endif
  // wxUSE_WCHAR_T

#endif  
  // _WX_WXSTRCONVH__

