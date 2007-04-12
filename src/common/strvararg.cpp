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

const wxChar *wxArgNormalizer<const wxCStrData&>::get() const
{
    // FIXME-UTF8: use some way that doesn't involve implicit conversion,
    //             so that we deallocate any converted buffer immediately;
    //             can't use AsString() because it returns wxString and not
    //             const wxString&, unfortunately; use As[W]CharBuf() when
    //             available.
    return m_value;
}

const wxChar *wxArgNormalizer<const wxString&>::get() const
{
#if wxUSE_UNICODE_UTF8 // FIXME-UTF8
    return (const wxChar*)m_value;
#else
    return m_value.wx_str();
#endif
}

#if wxUSE_UNICODE // FIXME-UTF8: should be wxUSE_UNICODE_WCHAR
wxArgNormalizer<const char*>::wxArgNormalizer(const char *value)
{
    // FIXME-UTF8: move this to the header so that m_value doesn't have
    //             to be dynamically allocated
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
#endif // wxUSE_UNICODE_WCHAR


#if /*wxUSE_UNICODE_UTF8 ||*/ !wxUSE_UNICODE // FIXME-UTF8
wxArgNormalizer<const wchar_t*>::wxArgNormalizer(const wchar_t *value)
{
#if wxUSE_UNICODE_UTF8 // FIXME-UTF8: this will be the only case
    m_value = new wxCharBuffer(wxConvUTF8.cWC2MB(value));
#else
    m_value = new wxCharBuffer(wxConvLibc.cWC2MB(value));
#endif
}

wxArgNormalizer<const wchar_t*>::~wxArgNormalizer()
{
    delete m_value;
}

const char *wxArgNormalizer<const wchar_t*>::get() const
{
    return m_value->data();
}
#endif // wxUSE_UNICODE_UTF8 || !wxUSE_UNICODE

#if 0 // wxUSE_UNICODE_UTF8 - FIXME-UTF8
wxArgNormalizer<const char*>::wxArgNormalizer(const char *value)
{
    // FIXME-UTF8: move this to the header so that m_value doesn't have
    //             to be dynamically allocated
    // FIXME-UTF8: optimize this if current locale is UTF-8 one

    // convert to widechar string first:
    wxWCharBuffer buf(wxConvLibc.cMB2WC(value));

    if ( buf )
    {
        // then to UTF-8:
        m_value = new wxCharBuffer(wxConvUTF8.cWC2MB(value));
    }
    else
    {
        m_value = new wxCharBuffer();
    }
}

wxArgNormalizer<const char*>::~wxArgNormalizer()
{
    delete m_value;
}

const char *wxArgNormalizer<const char*>::get() const
{
    return m_value->data();
}
#endif // wxUSE_UNICODE_UTF8



// FIXME-UTF8: move this to the header once it's possible to include buffer.h
//             without including wxcrt.h
wxArgNormalizer<wxCharBuffer>::wxArgNormalizer(const wxCharBuffer& buf)
    : wxArgNormalizer<const char*>(buf.data())
{
}

wxArgNormalizer<wxWCharBuffer>::wxArgNormalizer(const wxWCharBuffer& buf)
    : wxArgNormalizer<const wchar_t*>(buf.data())
{
}
