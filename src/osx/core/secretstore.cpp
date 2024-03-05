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
        m_data = nullptr;
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
            SecKeychainItemFreeContent(nullptr, m_data);
    }

private:
    UInt32 m_size;
    void *m_data;

    wxDECLARE_NO_COPY_CLASS(PasswordData);
};

// Implement wxSecretStore API using OS X Keychain Services.
//
// Note that this implementation currently always uses the default keychain by
// passing nullptr to the keychain functions, but it would be simple to allow
// using other keychains if required.
class wxSecretStoreOSXImpl : public wxSecretStoreImpl
{
public:
    virtual bool Save(const wxString& service,
                      const wxString& user,
                      const wxSecretValueImpl& secret,
                      wxString& errmsg) override
    {
        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();
        const wxScopedCharBuffer userUTF8 = user.utf8_str();

        // It doesn't seem possible to use SecKeychainItemModifyContent() to
        // update the existing record, if any, in place: what happens instead
        // is that it implicitly creates a new copy of the key chain item with
        // the updated attributes, but still keeps the existing one. Perhaps
        // it's possible to update the existing item in some other way, but for
        // now just use brute force solution and delete any existing items
        // first and then create the new one.

        // Ignore the result of Delete(), it's not an error if it didn't delete
        // anything and it's not even an error if it failed to delete an
        // existing item, we're going to get an error when adding new one in
        // this case anyhow.
        Delete(service, errmsg);

        OSStatus err = SecKeychainAddGenericPassword
                       (
                            nullptr,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            userUTF8.length(),
                            userUTF8.data(),
                            secret.GetSize(),
                            secret.GetData(),
                            nullptr                    // no output item
                       );

        if ( err != errSecSuccess )
        {
            errmsg = GetSecurityErrorMessage(err);
            return false;
        }

        return true;
    }

    virtual bool Load(const wxString& service,
                      wxString* user,
                      wxSecretValueImpl** secret,
                      wxString& errmsg) const override
    {
        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();

        PasswordData password;
        SecKeychainItemRef itemRef = nullptr;
        OSStatus err = SecKeychainFindGenericPassword
                       (
                            nullptr,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            0,                      // no account name
                            nullptr,
                            password.SizePtr(),
                            password.DataPtr(),
                            &itemRef
                       );
        wxCFRef<SecKeychainItemRef> ensureItemRefReleased(itemRef);

        if ( err != errSecSuccess )
        {
            if ( err != errSecItemNotFound )
                errmsg = GetSecurityErrorMessage(err);

            return false;
        }

        UInt32 attrTag = kSecAccountItemAttr;
        UInt32 attrFormat = CSSM_DB_ATTRIBUTE_FORMAT_STRING;
        SecKeychainAttributeInfo attrInfo;
        attrInfo.count = 1;
        attrInfo.tag = &attrTag;
        attrInfo.format = &attrFormat;

        SecKeychainAttributeList* attrList = nullptr;
        err = SecKeychainItemCopyAttributesAndData
              (
                    itemRef,
                    &attrInfo,                      // attrs to get
                    nullptr,                           // no output item class
                    &attrList,                      // output attr
                    nullptr,                              // no output data
                    nullptr                            //  (length/data)
              );

        if ( err != errSecSuccess )
        {
            errmsg = GetSecurityErrorMessage(err);

            return false;
        }

        if ( SecKeychainAttribute* attr = attrList ? attrList->attr : nullptr )
        {
            *user = wxString::FromUTF8(static_cast<char*>(attr->data),
                                       attr->length);
        }

        SecKeychainItemFreeAttributesAndData(attrList, nullptr);

        *secret = new wxSecretValueGenericImpl(password.GetSize(),
                                               password.GetData());

        return true;
    }

    virtual bool Delete(const wxString& service,
                        wxString& errmsg) override
    {
        const wxScopedCharBuffer serviceUTF8 = service.utf8_str();

        SecKeychainItemRef itemRef = nullptr;
        OSStatus err = SecKeychainFindGenericPassword
                       (
                            nullptr,                   // default keychain
                            serviceUTF8.length(),
                            serviceUTF8.data(),
                            0,                      // no account name
                            nullptr,
                            nullptr,                      // no output password
                            nullptr,                   // (length/data)
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
        return wxCFStringRef(SecCopyErrorMessageString(err, nullptr)).AsString();
    }
};

} // anonymous namespace

// ============================================================================
// OSX-specific implementation of common methods
// ============================================================================

/* static */
wxSecretValueImpl*
wxSecretValue::NewImpl(size_t size,
                       const void *data,
                       const char* WXUNUSED(contentType))
{
    return new wxSecretValueGenericImpl(size, data);
}

/* static */
wxSecretStore wxSecretStore::GetDefault()
{
    return wxSecretStore(new wxSecretStoreOSXImpl());
}

#endif // wxUSE_SECRETSTORE
