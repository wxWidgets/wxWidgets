///////////////////////////////////////////////////////////////////////////////
// Name:        strconv.h
// Purpose:     conversion routines for char sets any Unicode
// Author:      Robert Roebling, Ove Kaaven
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Ove Kaaven, Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTRCONVH__
#define _WX_WXSTRCONVH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma interface "strconv.h"
#endif

#include "wx/defs.h"
#include "wx/wxchar.h"
#include "wx/buffer.h"

#ifdef __DIGITALMARS__
#include "typeinfo.h"
#endif

#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
#  undef __BSEXCPT__
#endif

#include <stdlib.h>

#if wxUSE_WCHAR_T

// ----------------------------------------------------------------------------
// wxMBConv (abstract base class for conversions)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConv
{
public:
    // the actual conversion takes place here
    //
    // note that outputSize is the size of the output buffer, not the length of input
    // (the latter is always supposed to be NUL-terminated)
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const = 0;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const = 0;

    // actual conversion for strings with embedded null characters
    //
    // outputSize is the size of the output buffer
    // pszLen is the length of the input string (including all but last null character)
    size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize, size_t pszLen) const;
    size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize, size_t pszLen) const;

    // MB <-> WC
    const wxWCharBuffer cMB2WC(const char *psz) const;
    const wxCharBuffer cWC2MB(const wchar_t *psz) const;

    // convenience functions for converting MB or WC to/from wxWin default
#if wxUSE_UNICODE
    const wxWCharBuffer cMB2WX(const char *psz) const { return cMB2WC(psz); }
    const wxCharBuffer cWX2MB(const wchar_t *psz) const { return cWC2MB(psz); }
    const wchar_t* cWC2WX(const wchar_t *psz) const { return psz; }
    const wchar_t* cWX2WC(const wchar_t *psz) const { return psz; }
#else // ANSI
    const char* cMB2WX(const char *psz) const { return psz; }
    const char* cWX2MB(const char *psz) const { return psz; }
    const wxCharBuffer cWC2WX(const wchar_t *psz) const { return cWC2MB(psz); }
    const wxWCharBuffer cWX2WC(const char *psz) const { return cMB2WC(psz); }
#endif // Unicode/ANSI

    // virtual dtor for any base class
    virtual ~wxMBConv();
};

// ----------------------------------------------------------------------------
// wxMBConvLibc uses standard mbstowcs() and wcstombs() functions for
//              conversion (hence it depends on the current locale)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvLibc : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

// not very accurately named because it is not necessarily of type wxMBConvLibc
// (but the name can't eb changed because of backwards compatibility) default
// conversion
WXDLLIMPEXP_DATA_BASE(extern wxMBConv&) wxConvLibc;

// ----------------------------------------------------------------------------
// wxMBConvUTF7 (for conversion using UTF7 encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF7 : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

WXDLLIMPEXP_DATA_BASE(extern wxMBConvUTF7&) wxConvUTF7;

// ----------------------------------------------------------------------------
// wxMBConvUTF8 (for conversion using UTF8 encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF8 : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

WXDLLIMPEXP_DATA_BASE(extern wxMBConvUTF8&) wxConvUTF8;

// ----------------------------------------------------------------------------
// wxMBConvUTF16LE (for conversion using UTF16 Little Endian encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF16LE : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

// ----------------------------------------------------------------------------
// wxMBConvUTF16BE (for conversion using UTF16 Big Endian encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF16BE : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

// ----------------------------------------------------------------------------
// wxMBConvUCS4LE (for conversion using UTF32 Little Endian encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF32LE : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

// ----------------------------------------------------------------------------
// wxMBConvUCS4BE (for conversion using UTF32 Big Endian encoding)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConvUTF32BE : public wxMBConv
{
public:
    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;
};

// ----------------------------------------------------------------------------
// wxCSConv (for conversion based on loadable char sets)
// ----------------------------------------------------------------------------

#include "wx/fontenc.h"

class WXDLLIMPEXP_BASE wxCSConv : public wxMBConv
{
public:
    // we can be created either from charset name or from an encoding constant
    // but we can't have both at once
    wxCSConv(const wxChar *charset);
    wxCSConv(wxFontEncoding encoding);

    wxCSConv(const wxCSConv& conv);
    virtual ~wxCSConv();

    wxCSConv& operator=(const wxCSConv& conv);

    virtual size_t MB2WC(wchar_t *outputBuf, const char *psz, size_t outputSize) const;
    virtual size_t WC2MB(char *outputBuf, const wchar_t *psz, size_t outputSize) const;

    void Clear() ;

private:
    // common part of all ctors
    void Init();

    // creates m_convReal if necessary
    void CreateConvIfNeeded() const;

    // do create m_convReal (unconditionally)
    wxMBConv *DoCreate() const;

    // set the name (may be only called when m_name == NULL), makes copy of
    // charset string
    void SetName(const wxChar *charset);


    // note that we can't use wxString here because of compilation
    // dependencies: we're included from wx/string.h
    wxChar *m_name;
    wxFontEncoding m_encoding;

    // use CreateConvIfNeeded() before accessing m_convReal!
    wxMBConv *m_convReal;
    bool m_deferred;
};

#define wxConvFile wxConvLocal
WXDLLIMPEXP_DATA_BASE(extern wxCSConv&) wxConvLocal;
WXDLLIMPEXP_DATA_BASE(extern wxCSConv&) wxConvISO8859_1;
WXDLLIMPEXP_DATA_BASE(extern wxMBConv *) wxConvCurrent;

// ----------------------------------------------------------------------------
// endianness-dependent conversions
// ----------------------------------------------------------------------------

#ifdef WORDS_BIGENDIAN
    typedef wxMBConvUTF16BE wxMBConvUTF16;
    typedef wxMBConvUTF32BE wxMBConvUTF32;
#else
    typedef wxMBConvUTF16LE wxMBConvUTF16;
    typedef wxMBConvUTF32LE wxMBConvUTF32;
#endif

// ----------------------------------------------------------------------------
// filename conversion macros
// ----------------------------------------------------------------------------

// filenames are multibyte on Unix and probably widechar on Windows?
#if defined(__UNIX__) || defined(__BORLANDC__) || defined(__WXMAC__ )
    #define wxMBFILES 1
#else
    #define wxMBFILES 0
#endif

#if wxMBFILES && wxUSE_UNICODE
    #define wxFNCONV(name) wxConvFile.cWX2MB(name)
    #define wxFNSTRINGCAST wxMBSTRINGCAST
#else
    #define wxFNCONV(name) name
    #define wxFNSTRINGCAST WXSTRINGCAST
#endif

#else
  // !wxUSE_WCHAR_T

// ----------------------------------------------------------------------------
// stand-ins in absence of wchar_t
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMBConv
{
public:
    const char* cMB2WX(const char *psz) const { return psz; }
    const char* cWX2MB(const char *psz) const { return psz; }
};

#define wxConvFile wxConvLocal

WXDLLIMPEXP_DATA_BASE(extern wxMBConv) wxConvLibc,
                                       wxConvLocal,
                                       wxConvISO8859_1,
                                       wxConvUTF8;
WXDLLIMPEXP_DATA_BASE(extern wxMBConv *) wxConvCurrent;

#define wxFNCONV(name) name
#define wxFNSTRINGCAST WXSTRINGCAST

#endif
  // wxUSE_WCHAR_T

// ----------------------------------------------------------------------------
// macros for the most common conversions
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE
    #define wxConvertWX2MB(s)   wxConvCurrent->cWX2MB(s)
    #define wxConvertMB2WX(s)   wxConvCurrent->cMB2WX(s)
#else // ANSI
    // no conversions to do
    #define wxConvertWX2MB(s)   (s)
    #define wxConvertMB2WX(s)   (s)
#endif // Unicode/ANSI

#endif
  // _WX_WXSTRCONVH__

