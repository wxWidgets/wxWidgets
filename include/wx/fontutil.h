/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fontutil.h
// Purpose:     font-related helper functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// General note: this header is private to wxWindows and is not supposed to be
// included by user code. The functions declared here are implemented in
// msw/fontutil.cpp for Windows, unix/fontutil.cpp for GTK/Motif &c.

#ifndef _WX_FONTUTIL_H_
#define _WX_FONTUTIL_H_

#ifdef __GNUG__
    #pragma interface "fontutil.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/font.h"        // for wxFont and wxFontEncoding

// for our purposes here, GDK and X are identical
#if defined(__WXGTK__) || defined(__X__)
    #define _WX_X_FONTLIKE
#endif

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

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

struct wxNativeEncodingInfo
{
    wxString facename;          // may be empty meaning "any"

#if defined(__WXMSW__)
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

// ----------------------------------------------------------------------------
// font-related functions (common)
// ----------------------------------------------------------------------------

// translate a wxFontEncoding into native encoding parameter (defined above),
// returning TRUE if an (exact) macth could be found, FALSE otherwise (without
// attempting any substitutions)
extern bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                                    wxNativeEncodingInfo *info);

// test for the existence of the font described by this facename/encoding,
// return TRUE if such font(s) exist, FALSE otherwise
extern bool wxTestFontEncoding(const wxNativeEncodingInfo& info);

// ----------------------------------------------------------------------------
// font-related functions (X and GTK)
// ----------------------------------------------------------------------------

#ifdef _WX_X_FONTLIKE
    #include "wx/unix/fontutil.h"
#endif // X || GDK

#endif // _WX_FONTUTIL_H_
