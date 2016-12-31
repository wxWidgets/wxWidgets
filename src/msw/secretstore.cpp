///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/secretstore.cpp
// Purpose:     wxSecretStore implementation for MSW.
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
#include "wx/private/secretstore.h"

#include "wx/log.h"                     // wxSysErrorMsgStr()

// Somewhat surprisingly, wincred.h is not self-contained and relies on some
// standard Windows macros being defined without including the headers defining
// them on its own, so we must include <windows.h> (from our private header)
// before including it.
#include "wx/msw/private.h"
#include <wincred.h>

namespace
{

// ============================================================================
// wxSecretStoreImpl for MSW
// ============================================================================

// Helper class to ensure that a CREDENTIAL pointer is always freed.
class CredentialPtr
{
public:
    explicit CredentialPtr(CREDENTIAL* cred) : m_cred(cred) { }
    ~CredentialPtr() { ::CredFree(m_cred); }

private:
    CREDENTIAL* const m_cred;

    wxDECLARE_NO_COPY_CLASS(CredentialPtr);
};

class wxSecretStoreMSWImpl : public wxSecretStoreImpl
{
public:
    virtual bool Save(const wxString& service,
                      const wxString& user,
                      const wxSecretValueImpl& secret,
                      wxString& errmsg) wxOVERRIDE
    {
        const wxString target = MakeTargetName(service, user);

        CREDENTIAL cred;
        wxZeroMemory(cred);
        cred.Type = CRED_TYPE_GENERIC;
        cred.TargetName = const_cast<TCHAR*>(static_cast<const TCHAR*>(target.t_str()));
        cred.UserName = const_cast<TCHAR*>(static_cast<const TCHAR*>(user.t_str()));
        cred.CredentialBlobSize = secret.GetSize();
        cred.CredentialBlob = static_cast<BYTE *>(const_cast<void*>(secret.GetData()));

        // We could also use CRED_PERSIST_ENTERPRISE here to store the password
        // in the roaming section of the profile and this could arguably be
        // more useful. However it might also be unexpected if the user really
        // only wants to store the password on the local machine, so choose
        // security over convenience here. Maybe in the future we should have
        // an option for the password persistence scope.
        cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

        if ( !::CredWrite(&cred, 0) )
        {
            errmsg = wxSysErrorMsgStr();
            return false;
        }

        return true;
    }

    virtual wxSecretValueImpl* Load(const wxString& service,
                                    const wxString& user,
                                    wxString& errmsg) const wxOVERRIDE
    {
        const wxString target = MakeTargetName(service, user);

        CREDENTIAL* pcred = NULL;
        if ( !::CredRead(target.t_str(), CRED_TYPE_GENERIC, 0, &pcred) || !pcred )
        {
            // Not having the password for this service/user combination is not
            // an error, but anything else is.
            if ( ::GetLastError() != ERROR_NOT_FOUND )
                errmsg = wxSysErrorMsgStr();

            return NULL;
        }

        CredentialPtr ensureFree(pcred);
        return new wxSecretValueGenericImpl(pcred->CredentialBlobSize,
                                            pcred->CredentialBlob);
    }

    virtual bool Delete(const wxString& service,
                        const wxString& user,
                        wxString& errmsg) wxOVERRIDE
    {
        const wxString target = MakeTargetName(service, user);

        if ( !::CredDelete(target.t_str(), CRED_TYPE_GENERIC, 0) )
        {
            // Same logic as in Load() above.
            if ( ::GetLastError() != ERROR_NOT_FOUND )
                errmsg = wxSysErrorMsgStr();

            return false;
        }

        return true;
    }

private:
    // Return the string used as the "target name" for the API functions.
    // We need to combine both service and user into a single string as it
    // needs to be unique, i.e. otherwise we couldn't store two different
    // passwords for two different users of the same service and we do want to
    // be able to do this.
    static
    wxString MakeTargetName(const wxString& service, const wxString& user)
    {
        // The exact way of combining the service and user strings together is
        // completely arbitrary, we could also use "service:user" or
        // "user@service", there doesn't seem to be any standard about it, but
        // doing it like this has the advantage of keeping all passwords for
        // the given service together in the "Credential Manager" GUI and slash
        // is used by some standard programs, e.g. the built-in RDP client
        // stores its passwords under "TERMSRV/hostname".
        return service + wxS("/") + user;
    }
};

} // anonymous namespace

// ============================================================================
// MSW-specific implementation of common methods
// ============================================================================

/* static */
wxSecretValueImpl* wxSecretValue::NewImpl(size_t size, const void *data)
{
    return new wxSecretValueGenericImpl(size, data);
}

/* static */
void wxSecretValue::Wipe(size_t size, void *data)
{
    ::SecureZeroMemory(data, size);
}

/* static */
wxSecretStore wxSecretStore::GetDefault()
{
    // There is only a single store under Windows anyhow.
    return wxSecretStore(new wxSecretStoreMSWImpl());
}

#endif // wxUSE_SECRETSTORE
