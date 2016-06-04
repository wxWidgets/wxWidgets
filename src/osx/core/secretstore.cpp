///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/secretstore.cpp
// Purpose:     wxSecretStore implementation for OSX.
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

#include "wx/translation.h"

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#include <Security/Security.h>

namespace
{

// ============================================================================
// wxSecretStoreImpl for OSX
// ============================================================================

// Helper class to ensure that password data is always freed.
class PasswordData
{
public:
    PasswordData()
    {
        m_size = 0;
        m_data = NULL;
    }

    // These methods are only supposed to be used once to fill in the data.
    UInt32* SizePtr() { return &m_size; }
    void** DataPtr() { return &m_data; }

    // These methods can be used to access the data later.
    UInt32 GetSize() const { return m_size; }
    void* GetData() const { return m_data; }

    ~PasswordData()
    {
        if ( m_data )
            SecKeychainItemFreeContent(NULL, m_data);
    }

private:
    UInt32 m_size;
    void *m_data;

    wxDECLARE_NO_COPY_CLASS(PasswordData);
};

// Implement wxSecretStore API using OS X Keychain Services.
//
// Note that this implementation currently always uses the default keychain by
// passing NULL to the keychain functions, but it would be simple to allow
// using other keychains if required.
class wxSecretStoreOSXImpl : public wxSecretStoreImpl
{
public:
    virtual bool Save(const wxString& service,
                      const wxString& user,
                      const wxSecretValueImpl& secret,
                      wxString& errmsg) wxOVERRIDE
    {
        wxString errmsgExtra;

        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();
        const wxScopedCharBuffer userUTF8 = user.utf8_str();

        OSStatus err = SecKeychainAddGenericPassword
                       (
                            NULL,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            userUTF8.length(),
                            userUTF8.data(),
                            secret.GetSize(),
                            secret.GetData(),
                            NULL                    // no output item
                       );

        // Our API allows to use Save() to overwrite an existing password, so
        // check for failure due to the item already existing and overwrite it
        // if necessary.
        if ( err == errSecDuplicateItem )
        {
            SecKeychainItemRef itemRef = NULL;
            err = SecKeychainFindGenericPassword
                       (
                            NULL,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            userUTF8.length(),
                            userUTF8.data(),
                            NULL,                   // no output password
                            NULL,                   // (length/data)
                            &itemRef
                       );
            wxCFRef<SecKeychainItemRef> ensureItemRefReleased(itemRef);

            if ( err == errSecSuccess )
            {
                err = SecKeychainItemModifyContent
                      (
                        itemRef,
                        NULL,                           // no attributes to modify
                        secret.GetSize(),
                        secret.GetData()
                      );
            }

            // Try to provide a better error message, the last error on its own
            // is not informative enough.
            if ( err != errSecSuccess )
                errmsgExtra = _(" (while overwriting an existing item)");
        }

        if ( err != errSecSuccess )
        {
            errmsg = GetSecurityErrorMessage(err) + errmsgExtra;
            return false;
        }

        return true;
    }

    virtual wxSecretValueImpl* Load(const wxString& service,
                                    const wxString& user,
                                    wxString& errmsg) const wxOVERRIDE
    {
        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();
        const wxScopedCharBuffer userUTF8 = user.utf8_str();

        PasswordData password;
        OSStatus err = SecKeychainFindGenericPassword
                       (
                            NULL,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            userUTF8.length(),
                            userUTF8.data(),
                            password.SizePtr(),
                            password.DataPtr(),
                            NULL                    // no output item
                       );

        if ( err != errSecSuccess )
        {
            if ( err != errSecItemNotFound )
                errmsg = GetSecurityErrorMessage(err);

            return NULL;
        }

        return new wxSecretValueGenericImpl(password.GetSize(),
                                            password.GetData());
    }

    virtual bool Delete(const wxString& service,
                        const wxString& user,
                        wxString& errmsg) wxOVERRIDE
    {
        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();
        const wxScopedCharBuffer userUTF8 = user.utf8_str();

        SecKeychainItemRef itemRef = NULL;
        OSStatus err = SecKeychainFindGenericPassword
                       (
                            NULL,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            userUTF8.length(),
                            userUTF8.data(),
                            NULL,                   // no output password
                            NULL,                   // (length/data)
                            &itemRef
                       );
        wxCFRef<SecKeychainItemRef> ensureItemRefReleased(itemRef);

        if ( err == errSecItemNotFound )
        {
            // This is not an error, just indicate that nothing was done by
            // returning false.
            return false;
        }

        err = SecKeychainItemDelete(itemRef);
        if ( err != errSecSuccess )
        {
            errmsg = GetSecurityErrorMessage(err);
            return false;
        }

        return true;
    }

private:
    // Just a tiny helper wrapper.
    static wxString GetSecurityErrorMessage(OSStatus err)
    {
        return wxCFStringRef(SecCopyErrorMessageString(err, NULL)).AsString();
    }
};

} // anonymous namespace

// ============================================================================
// OSX-specific implementation of common methods
// ============================================================================

/* static */
wxSecretValueImpl* wxSecretValue::NewImpl(size_t size, const void *data)
{
    return new wxSecretValueGenericImpl(size, data);
}

/* static */
wxSecretStore wxSecretStore::GetDefault()
{
    return wxSecretStore(new wxSecretStoreOSXImpl());
}

#endif // wxUSE_SECRETSTORE
