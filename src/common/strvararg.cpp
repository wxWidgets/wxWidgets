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
#include "wx/buffer.h"
#include "wx/strconv.h"
#include "wx/string.h"

// ============================================================================
// implementation
// ============================================================================

const wxArgNativeCharType *wxArgNormalizer<const wxCStrData&>::get() const
{
    return m_value;
}

const wxArgNativeCharType *wxArgNormalizer<const wxString&>::get() const
{
    return m_value.c_str();
}

#if wxUSE_UNICODE_WCHAR

wxArgNormalizer<const char*>::wxArgNormalizer(const char *value)
{
    m_value = new wxWCharBuffer(wxConvLibc.cMB2WC(value));
}

wxArgNormalizer<const char*>::~wxArgNormalizer()
{
    delete m_value;
}

const wchar_t *wxArgNormalizer<const char*>::get() const
{
    return m_value->data();
}

#elif wxUSE_WCHAR_T // !wxUSE_UNICODE_WCHAR && wxUSE_WCHAR_T

wxArgNormalizer<const wchar_t*>::wxArgNormalizer(const wchar_t *value)
{
    m_value = new wxCharBuffer(wxConvLibc.cWC2MB(value));
}

wxArgNormalizer<const wchar_t*>::~wxArgNormalizer()
{
    delete m_value;
}

const char *wxArgNormalizer<const wchar_t*>::get() const
{
    return m_value->data();
}

#endif // wxUSE_UNICODE_WCHAR / !wxUSE_UNICODE_WCHAR && wxUSE_WCHAR_T
