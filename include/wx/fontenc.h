/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fontenc.h
// Purpose:     wxFontEncoding constants
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.03.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTENC_H_
#define _WX_FONTENC_H_

#include "wx/string.h"

// font encodings
enum wxFontEncoding
{
    wxFONTENCODING_SYSTEM = -1,     // system default
    wxFONTENCODING_DEFAULT,         // current default encoding

    // ISO8859 standard defines a number of single-byte charsets
    wxFONTENCODING_ISO8859_1,       // West European (Latin1)
    wxFONTENCODING_ISO8859_2,       // Central and East European (Latin2)
    wxFONTENCODING_ISO8859_3,       // Esperanto (Latin3)
    wxFONTENCODING_ISO8859_4,       // Baltic (old) (Latin4)
    wxFONTENCODING_ISO8859_5,       // Cyrillic
    wxFONTENCODING_ISO8859_6,       // Arabic
    wxFONTENCODING_ISO8859_7,       // Greek
    wxFONTENCODING_ISO8859_8,       // Hebrew
    wxFONTENCODING_ISO8859_9,       // Turkish (Latin5)
    wxFONTENCODING_ISO8859_10,      // Variation of Latin4 (Latin6)
    wxFONTENCODING_ISO8859_11,      // Thai
    wxFONTENCODING_ISO8859_12,      // doesn't exist currently, but put it
                                    // here anyhow to make all ISO8859
                                    // consecutive numbers
    wxFONTENCODING_ISO8859_13,      // Baltic (Latin7)
    wxFONTENCODING_ISO8859_14,      // Latin8
    wxFONTENCODING_ISO8859_15,      // Latin9 (a.k.a. Latin0, includes euro)
    wxFONTENCODING_ISO8859_MAX,

    // Cyrillic charset soup (see http://czyborra.com/charsets/cyrillic.html)
    wxFONTENCODING_KOI8,            // we don't support any of KOI8 variants
    wxFONTENCODING_ALTERNATIVE,     // same as MS-DOS CP866
    wxFONTENCODING_BULGARIAN,       // used under Linux in Bulgaria

    // what would we do without Microsoft? They have their own encodings
        // for DOS
    wxFONTENCODING_CP437,           // original MS-DOS codepage
    wxFONTENCODING_CP850,           // CP437 merged with Latin1
    wxFONTENCODING_CP852,           // CP437 merged with Latin2
    wxFONTENCODING_CP855,           // another cyrillic encoding
    wxFONTENCODING_CP866,           // and another one
        // and for Windows
    wxFONTENCODING_CP874,           // WinThai
    wxFONTENCODING_CP1250,          // WinLatin2
    wxFONTENCODING_CP1251,          // WinCyrillic
    wxFONTENCODING_CP1252,          // WinLatin1
    wxFONTENCODING_CP1253,          // WinGreek (8859-7)
    wxFONTENCODING_CP1254,          // WinTurkish
    wxFONTENCODING_CP1255,          // WinHebrew
    wxFONTENCODING_CP1256,          // WinArabic
    wxFONTENCODING_CP1257,          // WinBaltic (same as Latin 7)
    wxFONTENCODING_CP12_MAX,

    wxFONTENCODING_UTF7,            // UTF-7 Unicode encoding
    wxFONTENCODING_UTF8,            // UTF-8 Unicode encoding

    wxFONTENCODING_UNICODE,         // Unicode - currently used only by
                                    // wxEncodingConverter class

    wxFONTENCODING_MAX
};

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

#if wxUSE_GUI

// This private structure specifies all the parameters needed to create a font
// with the given encoding on this platform.
//
// Under X, it contains the last 2 elements of the font specifications
// (registry and encoding).
//
// Under Windows, it contains a number which is one of predefined CHARSET_XXX
// values.
//
// Under all platforms it also contains a facename string which should be
// used, if not empty, to create fonts in this encoding (this is the only way
// to create a font of non-standard encoding (like KOI8) under Windows - the
// facename specifies the encoding then)

struct WXDLLEXPORT wxNativeEncodingInfo
{
    wxString facename;          // may be empty meaning "any"
    wxFontEncoding encoding;    // so that we know what this struct represents

#if defined(__WXMSW__) || defined(__WXPM__) || defined(__WXMAC__)
    wxNativeEncodingInfo() { charset = 0; /* ANSI_CHARSET */ }

    int      charset;
#elif defined(_WX_X_FONTLIKE)
    wxString xregistry,
             xencoding;
#else
    #error "Unsupported toolkit"
#endif

    // this struct is saved in config by wxFontMapper, so it should know to
    // serialise itself (implemented in platform-specific code)
    bool FromString(const wxString& s);
    wxString ToString() const;
};

#endif // wxUSE_GUI

#endif // _WX_FONTENC_H_
