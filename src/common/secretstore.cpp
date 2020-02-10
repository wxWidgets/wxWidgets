///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/secretstore.cpp
// Purpose:     Common parts of wxSecretStore implementation.
// Author:      Vadim Zeitlin
// Created:     2016-05-27
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
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

#if wxUSE_SECRETSTORE

#include "wx/secretstore.h"

#include "wx/log.h"
#include "wx/translation.h"

#include "wx/private/secretstore.h"

#include <string.h>

// ============================================================================
// wxSecretValue implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Memory management
// ----------------------------------------------------------------------------

wxSecretValue::wxSecretValue(const wxSecretValue& other)
    : m_impl(other.m_impl)
{
    if ( m_impl )
        m_impl->IncRef();
}

wxSecretValue& wxSecretValue::operator=(const wxSecretValue& other)
{
    // This code is written to be safe in case of self-assignment.

    wxSecretValueImpl* const impl = other.m_impl;
    if ( impl )
        impl->IncRef();

    if ( m_impl )
        m_impl->DecRef();

    m_impl = impl;

    return *this;
}

wxSecretValue::~wxSecretValue()
{
    if ( m_impl )
        m_impl->DecRef();
}

// ----------------------------------------------------------------------------
// Comparison
// ----------------------------------------------------------------------------

bool wxSecretValue::operator==(const wxSecretValue& other) const
{
    // Two empty secrets are equal but no other secret is equal to the empty
    // one (even the zero-length one).
    if ( !m_impl )
        return !other.m_impl;

    wxSecretValueImpl* const impl = other.m_impl;
    if ( !impl )
        return false;

    // Here both objects are not empty, so we can compare them.
    return m_impl->GetSize() == impl->GetSize() &&
            memcmp(m_impl->GetData(), impl->GetData(), m_impl->GetSize()) == 0;
}

// ----------------------------------------------------------------------------
// Methods forwarded to wxSecretValueImpl
// ----------------------------------------------------------------------------

size_t wxSecretValue::GetSize() const
{
    return m_impl ? m_impl->GetSize() : (size_t)0;
}

const void *wxSecretValue::GetData() const
{
    return m_impl ? m_impl->GetData() : NULL;
}

wxString wxSecretValue::GetAsString(const wxMBConv& conv) const
{
    if ( !m_impl )
        return wxString();

    return wxString
           (
                static_cast<const char*>(m_impl->GetData()),
                conv,
                m_impl->GetSize()
           );
}

#ifndef __WINDOWS__

/* static */
void wxSecretValue::Wipe(size_t size, void *data)
{
    // memset_s() is not present under non-MSW systems anyhow and there doesn't
    // seem to be any other way to portably ensure that the memory is really
    // cleared, so just do it in this obvious way.
    memset(data, 0, size);
}

#endif // __WINDOWS__

/* static */
void wxSecretValue::WipeString(wxString& str)
{
    for ( wxString::iterator it = str.begin(); it != str.end(); ++it )
        *it = '*';
}

// ============================================================================
// wxSecretStore implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Memory management
// ----------------------------------------------------------------------------

wxSecretStore::wxSecretStore(const wxSecretStore& other)
    : m_impl(other.m_impl)
{
    if ( m_impl )
        m_impl->IncRef();
}

wxSecretStore::~wxSecretStore()
{
    if ( m_impl )
        m_impl->DecRef();
}

// ----------------------------------------------------------------------------
// Methods forwarded to wxSecretStoreImpl
// ----------------------------------------------------------------------------

bool
wxSecretStore::IsOk(wxString* errmsg) const
{
    if ( !m_impl )
    {
        if ( errmsg )
            *errmsg = _("Not available for this platform");
        return false;
    }

    return m_impl->IsOk(errmsg);
}

bool
wxSecretStore::Save(const wxString& service,
                    const wxString& user,
                    const wxSecretValue& secret)
{
    if ( !m_impl )
        return false;

    if ( !secret.m_impl )
        return false;

    wxString err;
    if ( !m_impl->Save(service, user, *secret.m_impl, err) )
    {
        wxLogError(_("Saving password for \"%s\" failed: %s."),
                   service, err);
        return false;
    }

    return true;
}

bool
wxSecretStore::Load(const wxString& service,
                    wxString& user,
                    wxSecretValue& secret) const
{
    if ( !m_impl )
        return false;

    wxString err;
    wxSecretValueImpl* secretImpl = NULL;
    if ( !m_impl->Load(service, &user, &secretImpl, err) )
    {
        if ( !err.empty() )
        {
            wxLogError(_("Reading password for \"%s\" failed: %s."),
                       service, err);
        }

        return false;
    }

    secret = wxSecretValue(secretImpl);

    return true;
}

bool
wxSecretStore::Delete(const wxString& service)
{
    if ( !m_impl )
        return false;

    wxString err;
    if ( !m_impl->Delete(service, err) )
    {
        if ( !err.empty() )
        {
            wxLogError(_("Deleting password for \"%s\" failed: %s."),
                       service, err);
        }
        return false;
    }

    return true;
}

#endif // wxUSE_SECRETSTORE
