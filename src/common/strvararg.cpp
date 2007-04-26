///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/strvararg.cpp
// Purpose:     macros for implementing type-safe vararg passing of strings
// Author:      Vaclav Slavik
// Created:     2007-02-19
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/strvararg.h"
#include "wx/string.h"

// ============================================================================
// implementation
// ============================================================================


const wxStringCharType *wxArgNormalizerNative<const wxString&>::get() const
{
    return m_value.wx_str();
}

const wxStringCharType *wxArgNormalizerNative<const wxCStrData&>::get() const
{
    return m_value.AsInternal();
}

#if wxUSE_UNICODE_UTF8
wxArgNormalizerWchar<const wxString&>::wxArgNormalizerWchar(const wxString& s)
    : wxArgNormalizerWithBuffer<wchar_t>(s.wc_str())
{
}

wxArgNormalizerWchar<const wxCStrData&>::wxArgNormalizerWchar(const wxCStrData& s)
    : wxArgNormalizerWithBuffer<wchar_t>(s.AsWCharBuf())
{
}
#endif // wxUSE_UNICODE_UTF8

wxString wxArgNormalizedString::GetString() const
{
    if ( !IsValid() )
        return wxEmptyString;

#if wxUSE_UTF8_LOCALE_ONLY
    return wxString(wx_reinterpret_cast(const char*, m_ptr));
#else
    #if wxUSE_UNICODE_UTF8
        if ( wxLocaleIsUtf8 )
            return wxString(wx_reinterpret_cast(const char*, m_ptr));
        else
    #endif
        return wxString(wx_reinterpret_cast(const wchar_t*, m_ptr));
#endif // !wxUSE_UTF8_LOCALE_ONLY
}

wxArgNormalizedString::operator wxString() const
{
    return GetString();
}
