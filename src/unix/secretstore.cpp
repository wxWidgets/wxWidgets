///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/secretstore.cpp
// Purpose:     wxSecretStore implementation using libsecret.
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

// Older versions of libsecret, such as 0.16 from Ubuntu 14.04 LTS, require
// predefining this symbol in order to have access to the APIs we use below.
#define SECRET_API_SUBJECT_TO_CHANGE
#include <libsecret/secret.h>

#include "wx/gtk/private/error.h"

namespace
{

// Tiny helper to ensure freeing GHashTable.
class wxGHashTable
{
public:
    // Ctor takes ownership of its argument.
    explicit wxGHashTable(GHashTable* hash) : m_hash(hash) { }

    // Must have a copy ctor to allow returning these objects from functions in
    // C++98, this class could/should be move-only in C++11.
    wxGHashTable(const wxGHashTable& other)
        : m_hash(other.m_hash)
    {
        g_hash_table_ref(m_hash);
    }

    ~wxGHashTable() { g_hash_table_unref(m_hash); }

    operator GHashTable *() const { return m_hash; }

private:
    GHashTable* const m_hash;

    wxDECLARE_NO_ASSIGN_CLASS(wxGHashTable);
};

// ============================================================================
// wxSecretStoreImpl using libsecret
// ============================================================================

class wxSecretValueLibSecretImpl : public wxSecretValueImpl
{
public:
    // Create a new secret value.
    //
    // Notice that we have to use text/plain as content type and not
    // application/octet-stream which would have been more logical because
    // libsecret accepts only valid UTF-8 strings for the latter, while our
    // data is not necessarily UTF-8 (nor even text at all...).
    wxSecretValueLibSecretImpl(size_t size, const void* data)
        : m_value(secret_value_new(static_cast<const gchar*>(data), size,
                                   "text/plain"))
    {
    }

    // Adopt an existing secret value.
    //
    // This ctor takes ownership of the provided pointer and will release it
    // when this object is destroyed.
    explicit wxSecretValueLibSecretImpl(SecretValue* value)
        : m_value(value)
    {
    }

    virtual ~wxSecretValueLibSecretImpl()
    {
        // No need to wipe memory, this will happen by default.
        secret_value_unref(m_value);
    }

    virtual size_t GetSize() const wxOVERRIDE
    {
        gsize length = 0;
        (void)secret_value_get(m_value, &length);
        return length;
    }

    virtual const void *GetData() const wxOVERRIDE
    {
        return secret_value_get(m_value, NULL);
    }

    SecretValue* GetValue() const
    {
        return m_value;
    }

private:
    SecretValue* const m_value;
};

// This implementation uses synchronous libsecret functions which is supposed
// to be a bad idea, but doesn't seem to be a big deal in practice and as there
// is no simple way to implement asynchronous API under the other platforms, it
// doesn't seem to be worth it to use it just under Unix, so keep things simple
// (even if blocking) for now.
class wxSecretStoreLibSecretImpl : public wxSecretStoreImpl
{
public:
    virtual bool Save(const wxString& service,
                      const wxString& user,
                      const wxSecretValueImpl& secret,
                      wxString& errmsg) wxOVERRIDE
    {
        // We don't have any argument for the user-visible secret description
        // supported by libsecret, so we just concatenate the service and user
        // strings. It might be a good idea to add a possibility to specify a
        // more informative description later.
        const wxString label = service + wxS("/") + user;

        // Notice that we can't use secret_password_store_sync() here because
        // our secret can contain NULs, so we must pass by the lower level API.
        wxGtkError error;
        if ( !secret_service_store_sync
              (
                NULL,                           // Default service
                GetSchema(),
                BuildAttributes(service, user),
                SECRET_COLLECTION_DEFAULT,
                label.utf8_str(),
                static_cast<const wxSecretValueLibSecretImpl&>(secret).GetValue(),
                NULL,                           // Can't be cancelled
                error.Out()
              ) )
        {
            errmsg = error.GetMessage();
            return false;
        }

        return true;
    }

    virtual wxSecretValueImpl* Load(const wxString& service,
                                    const wxString& user,
                                    wxString& errmsg) const wxOVERRIDE
    {
        wxGtkError error;
        SecretValue* const value = secret_service_lookup_sync
              (
                NULL,                           // Default service
                GetSchema(),
                BuildAttributes(service, user),
                NULL,                           // Can't be cancelled
                error.Out()
              );

        if ( !value )
        {
            // There can be no error message if the secret was just not found
            // and no other error occurred -- just leave the error message
            // empty in this case, this is exactly how our API is supposed to
            // behave.
            if ( error )
                errmsg = error.GetMessage();
            return NULL;
        }

        return new wxSecretValueLibSecretImpl(value);
    }

    virtual bool Delete(const wxString& service,
                        const wxString& user,
                        wxString& errmsg) wxOVERRIDE
    {
        wxGtkError error;
        if ( !secret_password_clearv_sync
              (
                GetSchema(),
                BuildAttributes(service, user),
                NULL,                           // Can't be cancelled
                error.Out()
              ) )
        {
            if ( error )
                errmsg = error.GetMessage();
            return false;
        }

        return true;
    }

private:
    // Constants for the schema fields.
    static const char* FIELD_SERVICE;
    static const char* FIELD_USER;

    // Currently we use a hard-coded schema, but we might allow customizing it
    // (or at least its name?) in the future, so wrap access to it in this
    // helper function to make changing the code later simpler.
    static SecretSchema* GetSchema()
    {
        static SecretSchema s_schema =
            {
                "org.freedesktop.Secret.Generic",
                SECRET_SCHEMA_NONE,
                {
                    { FIELD_SERVICE,    SECRET_SCHEMA_ATTRIBUTE_STRING },
                    { FIELD_USER,       SECRET_SCHEMA_ATTRIBUTE_STRING },
                    { NULL }
                }
            };

        return &s_schema;
    }

    // Return attributes for the schema defined above.
    static wxGHashTable BuildAttributes(const wxString& service,
                                       const wxString& user)
    {
        return wxGHashTable(secret_attributes_build
                            (
                                GetSchema(),
                                FIELD_SERVICE,  service.utf8_str().data(),
                                FIELD_USER,     user.utf8_str().data(),
                                NULL
                            ));
    }
};

const char* wxSecretStoreLibSecretImpl::FIELD_SERVICE = "service";
const char* wxSecretStoreLibSecretImpl::FIELD_USER = "user";

} // anonymous namespace

// ============================================================================
// LibSecret-specific implementation of common methods
// ============================================================================

/* static */
wxSecretValueImpl* wxSecretValue::NewImpl(size_t size, const void *data)
{
    return new wxSecretValueLibSecretImpl(size, data);
}

/* static */
wxSecretStore wxSecretStore::GetDefault()
{
    // There is only a single store under Windows anyhow.
    return wxSecretStore(new wxSecretStoreLibSecretImpl());
}

#endif // wxUSE_SECRETSTORE
