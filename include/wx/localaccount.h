///////////////////////////////////////////////////////////////////////////////
// Name:        wx/localaccount.h
// Purpose:     wxLocalAccount class declaration
// Author:      Blake Madden
// Created:     2026-10-11
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LOCALACCOUNT_H_
#define _WX_LOCALACCOUNT_H_

#include "wx/string.h"

class WXDLLIMPEXP_FWD_BASE wxFileName;

// ----------------------------------------------------------------------------
// wxLocalAccount: identifies a user account of the local system
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxWARN_UNUSED wxLocalAccount
{
public:
    // Creates an invalid account object.
    wxLocalAccount() = default;

    wxLocalAccount(const wxLocalAccount&) = default;
    wxLocalAccount& operator=(const wxLocalAccount&) = default;

    // Returns true if the account was determined, even if its name could not
    // be resolved. In that case, GetName() is empty but GetId() is not.
    bool IsOk() const { return !m_id.empty(); }

    // Name of the account without the domain, empty if it couldn't be found.
    const wxString& GetName() const { return m_name; }

    // Domain of the account (e.g., "CORP"). Always empty under Unix, which has
    // no account domains.
    const wxString& GetDomain() const { return m_domain; }

    // Numeric user ID under Unix, string form of the SID under MSW.
    const wxString& GetId() const { return m_id; }

    // Returns "domain\name" or just "name" if there is no domain, falling
    // back to GetId() if the name is unknown.
    wxString GetAsString() const
    {
        if ( m_name.empty() )
            return m_id;

        if ( m_domain.empty() )
            return m_name;

        return m_domain + wxS("\\") + m_name;
    }

private:
    friend class wxFileName;

    wxLocalAccount(const wxString& name,
                   const wxString& domain,
                   const wxString& id)
        : m_name(name), m_domain(domain), m_id(id)
    {
    }

    wxString m_name,
             m_domain,
             m_id;
};

#endif // _WX_LOCALACCOUNT_H_
