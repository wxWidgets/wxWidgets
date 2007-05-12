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

// ----------------------------------------------------------------------------
// wxArgNormalizer<>
// ----------------------------------------------------------------------------

const wxStringCharType *wxArgNormalizerNative<const wxString&>::get() const
{
    return m_value.wx_str();
}

const wxStringCharType *wxArgNormalizerNative<const wxCStrData&>::get() const
{
    return m_value.AsInternal();
}

#if wxUSE_UNICODE_UTF8 && !wxUSE_UTF8_LOCALE_ONLY
wxArgNormalizerWchar<const wxString&>::wxArgNormalizerWchar(const wxString& s)
    : wxArgNormalizerWithBuffer<wchar_t>(s.wc_str())
{
}

wxArgNormalizerWchar<const wxCStrData&>::wxArgNormalizerWchar(const wxCStrData& s)
    : wxArgNormalizerWithBuffer<wchar_t>(s.AsWCharBuf())
{
}
#endif // wxUSE_UNICODE_UTF8 && !wxUSE_UTF8_LOCALE_ONLY

// ----------------------------------------------------------------------------
// wxArgNormalizedString
// ----------------------------------------------------------------------------

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
        return wxString(wx_reinterpret_cast(const wxChar*, m_ptr));
#endif // !wxUSE_UTF8_LOCALE_ONLY
}

wxArgNormalizedString::operator wxString() const
{
    return GetString();
}

// ----------------------------------------------------------------------------
// wxFormatString
// ----------------------------------------------------------------------------

#if !wxUSE_UNICODE_WCHAR
const char* wxFormatString::AsChar()
{
    if ( m_char )
        return m_char.data();

    // in ANSI build, wx_str() returns char*, in UTF-8 build, this function
    // is only called under UTF-8 locales, so we should return UTF-8 string,
    // which is, again, what wx_str() returns:
    if ( m_str )
        return m_str->wx_str();

    // ditto wxCStrData:
    if ( m_cstr )
        return m_cstr->AsInternal();

    // the last case is that wide string was passed in: in that case, we need
    // to convert it:
    wxASSERT( m_wchar );

    m_char = wxConvLibc.cWC2MB(m_wchar.data());

    return m_char.data();
}
#endif // !wxUSE_UNICODE_WCHAR

#if wxUSE_UNICODE && !wxUSE_UTF8_LOCALE_ONLY
const wchar_t* wxFormatString::AsWChar()
{
    if ( m_wchar )
        return m_wchar.data();

#if wxUSE_UNICODE_WCHAR
    if ( m_str )
        return m_str->wc_str();
    if ( m_cstr )
        return m_cstr->AsInternal();
#else // wxUSE_UNICODE_UTF8
    if ( m_str )
    {
        m_wchar = m_str->wc_str();
        return m_wchar.data();
    }
    if ( m_cstr )
    {
        m_wchar = m_cstr->AsWCharBuf();
        return m_wchar.data();
    }
#endif // wxUSE_UNICODE_WCHAR/UTF8

    // the last case is that narrow string was passed in: in that case, we need
    // to convert it:
    wxASSERT( m_char );

    m_wchar = wxConvLibc.cMB2WC(m_char.data());

    return m_wchar.data();
}
#endif // wxUSE_UNICODE && !wxUSE_UTF8_LOCALE_ONLY
