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


#if wxUSE_SECRETSTORE

#include "wx/dynlib.h"
#include "wx/log.h"
#include "wx/module.h"
#include "wx/secretstore.h"
#include "wx/private/secretstore.h"

// Older versions of libsecret, such as 0.16 from Ubuntu 14.04 LTS, require
// predefining this symbol in order to have access to the APIs we use below.
#define SECRET_API_SUBJECT_TO_CHANGE
#include <libsecret/secret.h>

#include "wx/gtk/private/error.h"
#include "wx/gtk/private/list.h"
#include "wx/gtk/private/object.h"

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

// We load libsecret dynamically: this allows the applications using this
// wxSecretStore implementation to run even on the systems without libsecret
// (but not work there, of course).
namespace wxLibSecret
{

// X-macro executing the given macro for all libsecret functions that we use.
#define wxFOR_ALL_SECRETLIB_SYMBOLS(m) \
    m(secret_attributes_build) \
    m(secret_item_get_attributes) \
    m(secret_item_get_secret) \
    m(secret_service_clear_sync) \
    m(secret_service_get_sync) \
    m(secret_service_search_sync) \
    m(secret_service_store_sync) \
    m(secret_value_get) \
    m(secret_value_new) \
    m(secret_value_unref)

#define DECLARE_SECRETLIB_FUNC(name) \
    decltype(::name)* name = nullptr;

wxFOR_ALL_SECRETLIB_SYMBOLS(DECLARE_SECRETLIB_FUNC)

#undef DECLARE_SECRETLIB_FUNC

wxDynamicLibrary gs_dynlib;

// Unload the library if it is loaded.
void UnloadAll()
{
    // Reset all function pointers before the library is unloaded.
    if ( wxLibSecret::gs_dynlib.IsLoaded() )
    {
        #define RESET_SECRETLIB_FUNC(name) name = nullptr;

        wxFOR_ALL_SECRETLIB_SYMBOLS(RESET_SECRETLIB_FUNC);

        #undef RESET_SECRETLIB_FUNC

        wxLibSecret::gs_dynlib.Unload();
    }
}

// Load the library and initialize all function pointers.
//
// If errmsg is non-null, fill it with the errors logged while trying to load
// on failure.
bool LoadAll(wxString* errmsg = nullptr)
{
    if ( gs_dynlib.IsLoaded() )
        return true;

    wxLogCollector logCollect;

    if ( gs_dynlib.Load("libsecret-1") )
    {
        for ( ;; )
        {
            #define LOAD_SECRETLIB_FUNC(name) \
                name = (decltype(::name)*)gs_dynlib.GetSymbol(#name); \
                if ( !name ) \
                    break;

            wxFOR_ALL_SECRETLIB_SYMBOLS(LOAD_SECRETLIB_FUNC)

            #undef LOAD_SECRETLIB_FUNC

            // If we didn't break out of the loop, we must have loaded all the
            // functions successfully.
            return true;
        }
    }

    // Get all the errors without the trailing new line, if any.
    if ( errmsg )
        *errmsg = logCollect.GetMessages().Strip();

    // If we failed to load all the functions, don't keep the library loaded.
    UnloadAll();

    return false;
}

} // namespace wxLibSecret

class wxLibSecretModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override { wxLibSecret::UnloadAll(); }
};

// ============================================================================
// wxSecretStoreImpl using libsecret
// ============================================================================

// Dummy implementation used when libsecret is not available.
class wxSecretValueNotAvailableImpl : public wxSecretValueImpl
{
public:
    wxSecretValueNotAvailableImpl(size_t size, const void* data)
        : m_buf(size)
    {
        memcpy(m_buf.data(), data, size);
    }

    virtual ~wxSecretValueNotAvailableImpl()
    {
        wxSecretValue::Wipe(m_buf.size(), m_buf.data());
    }

    virtual size_t GetSize() const override { return m_buf.size(); }
    virtual const void *GetData() const override { return m_buf.data(); }

private:
    std::vector<unsigned char> m_buf;
};

class wxSecretValueLibSecretImpl : public wxSecretValueImpl
{
public:
    // Create a new secret value for the given content type.
    wxSecretValueLibSecretImpl(size_t size, const void* data, const char* contentType)
        : m_value(wxLibSecret::secret_value_new(
                    static_cast<const gchar*>(data),
                    size,
                    contentType
                  ))
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
        wxLibSecret::secret_value_unref(m_value);
    }

    virtual size_t GetSize() const override
    {
        gsize length = 0;
        (void)wxLibSecret::secret_value_get(m_value, &length);
        return length;
    }

    virtual const void *GetData() const override
    {
        return wxLibSecret::secret_value_get(m_value, nullptr);
    }

    SecretValue* GetValue() const
    {
        return m_value;
    }

private:
    SecretValue* const m_value;
};

// Dummy implementation used when secret service is not available.
class wxSecretStoreNotAvailableImpl : public wxSecretStoreImpl
{
public:
    explicit wxSecretStoreNotAvailableImpl(const wxString& error)
        : m_error(error)
    {
    }

    virtual bool IsOk(wxString* errmsg) const override
    {
        if ( errmsg )
            *errmsg = m_error;

        return false;
    }

    virtual bool Save(const wxString& WXUNUSED(service),
                      const wxString& WXUNUSED(user),
                      const wxSecretValueImpl& WXUNUSED(secret),
                      wxString& errmsg) override
    {
        errmsg = m_error;
        return false;
    }

    virtual bool Load(const wxString& WXUNUSED(service),
                      wxString* WXUNUSED(user),
                      wxSecretValueImpl** WXUNUSED(secret),
                      wxString& errmsg) const override
    {
        errmsg = m_error;
        return false;
    }

    virtual bool Delete(const wxString& WXUNUSED(service),
                        wxString& errmsg) override
    {
        errmsg = m_error;
        return false;
    }

private:
    const wxString m_error;
};

// This implementation uses synchronous libsecret functions which is supposed
// to be a bad idea, but doesn't seem to be a big deal in practice and as there
// is no simple way to implement asynchronous API under the other platforms, it
// doesn't seem to be worth it to use it just under Unix, so keep things simple
// (even if blocking) for now.
class wxSecretStoreLibSecretImpl : public wxSecretStoreImpl
{
public:
    static wxSecretStoreLibSecretImpl* Create(wxString& errmsg)
    {
        // First of all, load the library if not done yet.
        if ( !wxLibSecret::LoadAll(&errmsg) )
            return nullptr;

        wxGtkError error;
        SecretService* const service = wxLibSecret::secret_service_get_sync
                                       (
                                            SECRET_SERVICE_OPEN_SESSION,
                                            nullptr,   // No cancellation
                                            error.Out()
                                       );
        if ( !service )
        {
            errmsg = error.GetMessage();
            return nullptr;
        }

        // This passes ownership of service to the new object.
        return new wxSecretStoreLibSecretImpl(service);
    }

    virtual bool Save(const wxString& service,
                      const wxString& user,
                      const wxSecretValueImpl& secret,
                      wxString& errmsg) override
    {
        // We don't have any argument for the user-visible secret description
        // supported by libsecret, so we just reuse the service string. It
        // might be a good idea to add a possibility to specify a more
        // informative description later.

        // Notice that we can't use secret_password_store_sync() here because
        // our secret can contain NULs, so we must pass by the lower level API.
        wxGtkError error;
        if ( !wxLibSecret::secret_service_store_sync
              (
                m_service,
                GetSchema(),
                BuildAttributes(service, user),
                SECRET_COLLECTION_DEFAULT,
                service.utf8_str(),
                static_cast<const wxSecretValueLibSecretImpl&>(secret).GetValue(),
                nullptr,                           // Can't be cancelled
                error.Out()
              ) )
        {
            errmsg = error.GetMessage();
            return false;
        }

        return true;
    }

    virtual bool Load(const wxString& service,
                      wxString* user,
                      wxSecretValueImpl** secret,
                      wxString& errmsg) const override
    {
        wxGtkError error;
        GList* const found = wxLibSecret::secret_service_search_sync
            (
                m_service,
                GetSchema(),
                BuildAttributes(service),
                static_cast<SecretSearchFlags>
                (
                    SECRET_SEARCH_UNLOCK |
                    SECRET_SEARCH_LOAD_SECRETS
                ),
                nullptr,                           // Can't be cancelled
                error.Out()
            );

        if ( !found )
        {
            // There can be no error message if the secret was just not found
            // and no other error occurred -- just leave the error message
            // empty in this case, this is exactly how our API is supposed to
            // behave.
            if ( error )
                errmsg = error.GetMessage();
            return false;
        }

        wxGtkList ensureListFreed(found);

        SecretItem* const item = static_cast<SecretItem*>(found->data);
        wxGtkObject<SecretItem> ensureItemFreed(item);

        const wxGHashTable attrs(wxLibSecret::secret_item_get_attributes(item));
        const gpointer field = g_hash_table_lookup(attrs, FIELD_USER);
        if ( field )
            *user = wxString::FromUTF8(static_cast<char*>(field));

        *secret = new wxSecretValueLibSecretImpl(wxLibSecret::secret_item_get_secret(item));

        return true;
    }

    virtual bool Delete(const wxString& service,
                        wxString& errmsg) override
    {
        wxGtkError error;
        if ( !wxLibSecret::secret_service_clear_sync
              (
                m_service,
                GetSchema(),
                BuildAttributes(service),
                nullptr,                           // Can't be cancelled
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
        // SecretSchema struct has some "reserved" fields in it which we don't
        // want to initialize, but this results in this warning if it's
        // enabled, so just suppress it here.
        wxGCC_WARNING_SUPPRESS(missing-field-initializers)

        static SecretSchema s_schema =
            {
                "org.freedesktop.Secret.Generic",
                SECRET_SCHEMA_NONE,
                {
                    { FIELD_SERVICE,    SECRET_SCHEMA_ATTRIBUTE_STRING },
                    { FIELD_USER,       SECRET_SCHEMA_ATTRIBUTE_STRING },
                    { nullptr }
                }
            };

        wxGCC_WARNING_RESTORE(missing-field-initializers)

        return &s_schema;
    }

    // Return attributes for the schema defined above.
    static wxGHashTable BuildAttributes(const wxString& service)
    {
        return wxGHashTable(wxLibSecret::secret_attributes_build
                            (
                                GetSchema(),
                                FIELD_SERVICE,  service.utf8_str().data(),
                                nullptr
                            ));
    }

    static wxGHashTable BuildAttributes(const wxString& service,
                                        const wxString& user)
    {
        return wxGHashTable(wxLibSecret::secret_attributes_build
                            (
                                GetSchema(),
                                FIELD_SERVICE,  service.utf8_str().data(),
                                FIELD_USER,     user.utf8_str().data(),
                                nullptr
                            ));
    }

    // Ctor is private, Create() should be used for creating objects of this
    // class.
    explicit wxSecretStoreLibSecretImpl(SecretService* service)
        : m_service(service)
    {
    }

    wxGtkObject<SecretService> m_service;
};

const char* wxSecretStoreLibSecretImpl::FIELD_SERVICE = "service";
const char* wxSecretStoreLibSecretImpl::FIELD_USER = "user";

} // anonymous namespace

// ============================================================================
// LibSecret-specific implementation of common methods
// ============================================================================

/* static */
wxSecretValueImpl*
wxSecretValue::NewImpl(size_t size,
                       const void *data,
                       const char* contentType)
{
    if ( !wxLibSecret::LoadAll() )
        return new wxSecretValueNotAvailableImpl(size, data);

    return new wxSecretValueLibSecretImpl(size, data, contentType);
}

/* static */
wxSecretStore wxSecretStore::GetDefault()
{
    // Try to create the real implementation.
    wxString errmsg;
    wxSecretStoreImpl* impl = wxSecretStoreLibSecretImpl::Create(errmsg);
    if ( !impl )
    {
        // But if we failed, fall back to a dummy one, so that we could at
        // least return the error to the code using this class.
        impl = new wxSecretStoreNotAvailableImpl(errmsg);
    }

    return wxSecretStore(impl);
}

#endif // wxUSE_SECRETSTORE
