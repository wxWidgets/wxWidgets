///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/extradata.h
// Purpose:     Extra data stored in wxWithExtraData.
// Author:      Vadim Zeitlin
// Created:     2026-11-22
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_EXTRADATA_H_
#define _WX_PRIVATE_EXTRADATA_H_

#include <memory>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// Extra data for preserving ABI compatibility
// ----------------------------------------------------------------------------

// The classes and functions in this file allow associating arbitrary data
// with wx objects that are part of public API without changing their layout.
//
// To use it, define a class deriving from wxExtraData containing the fields
// that would otherwise be added to the class itself, e.g.
//
//      class wxMyControlExtraData : public wxExtraData
//      {
//      public:
//          int m_newField = 0;
//      };
//
// and then use wxGetExtraData<wxMyControlExtraData>(this)->m_newField in the
// wxMyControl code.
//
// Notes:
//
//  - Extra data is destroyed in the base class destructor, i.e. after the
//    derived class destructor has already run, so its destructor must not
//    use the object it is associated with.
//
//  - When the object is copied, only the data overriding Clone() is copied,
//    see wxCloneableExtraData.
//
//  - Each extra data class must only be used from the code in a single wx
//    library, as the key identifying it is not shared between them.

// Base class for all extra data classes.
class wxExtraData
{
public:
    wxExtraData() = default;
    virtual ~wxExtraData() = default;

    // Return a copy of this object or nullptr if it can't be copied, which is
    // the default.
    virtual wxExtraData* Clone() const { return nullptr; }

protected:
    wxExtraData(const wxExtraData&) = default;
    wxExtraData& operator=(const wxExtraData&) = default;
};

// Base class for the extra data which should be copied when the object it is
// associated with is copied: T must be the derived class itself and must be
// copyable.
template <typename T>
class wxCloneableExtraData : public wxExtraData
{
public:
    virtual wxExtraData* Clone() const override
    {
        return new T(static_cast<const T&>(*this));
    }
};

// Opaque key identifying the extra data of the given type.
using wxExtraDataKey = const void*;

template <typename T>
inline wxExtraDataKey wxGetExtraDataKey()
{
    static const char s_key = 0;
    return &s_key;
}

// Container for all extra data associated with a single object.
class wxExtraDataStorage
{
public:
    wxExtraDataStorage() = default;

    // Copy all the data which can be copied.
    wxExtraDataStorage(const wxExtraDataStorage& other)
    {
        for ( const auto& entry : other.m_entries )
        {
            if ( wxExtraData* const data = entry.data->Clone() )
                m_entries.emplace_back(entry.key, data);
        }
    }

    wxExtraDataStorage& operator=(const wxExtraDataStorage&) = delete;

    // Return the data associated with the given key or nullptr.
    wxExtraData* Get(wxExtraDataKey key) const
    {
        for ( const auto& entry : m_entries )
        {
            if ( entry.key == key )
                return entry.data.get();
        }

        return nullptr;
    }

    // Associate the given data with the given key, taking ownership of it and
    // replacing (and deleting) the previously associated data, if any. If the
    // data is null, just remove the existing data.
    void Set(wxExtraDataKey key, wxExtraData* data)
    {
        for ( auto it = m_entries.begin(); it != m_entries.end(); ++it )
        {
            if ( it->key == key )
            {
                if ( data )
                    it->data.reset(data);
                else
                    m_entries.erase(it);

                return;
            }
        }

        if ( data )
            m_entries.emplace_back(key, data);
    }

private:
    struct Entry
    {
        Entry(wxExtraDataKey key, wxExtraData* data)
            : key{key}, data{data}
        {
        }

        wxExtraDataKey key;
        std::unique_ptr<wxExtraData> data;
    };

    std::vector<Entry> m_entries;
};

// ----------------------------------------------------------------------------
// Functions for accessing extra data
// ----------------------------------------------------------------------------

// All these functions take a pointer to an object of a class deriving from
// wxWithExtraData.

// Return the extra data of the given type, creating it if necessary.
template <typename T, typename O>
inline T* wxGetExtraData(O* obj)
{
    wxExtraDataStorage& storage = obj->WXGetExtraDataStorage();

    const wxExtraDataKey key = wxGetExtraDataKey<T>();
    T* data = static_cast<T*>(storage.Get(key));
    if ( !data )
    {
        data = new T();
        storage.Set(key, data);
    }

    return data;
}

// Return the extra data of the given type if it exists or nullptr otherwise.
template <typename T, typename O>
inline T* wxGetExtraDataIfExists(const O* obj)
{
    const wxExtraDataStorage* const storage = obj->WXGetExtraDataStorageIfExists();
    if ( !storage )
        return nullptr;

    return static_cast<T*>(storage->Get(wxGetExtraDataKey<T>()));
}

// Destroy the extra data of the given type, if it exists.
template <typename T, typename O>
inline void wxResetExtraData(O* obj)
{
    if ( wxExtraDataStorage* const storage = obj->WXGetExtraDataStorageIfExists() )
        storage->Set(wxGetExtraDataKey<T>(), nullptr);
}

#endif // _WX_PRIVATE_EXTRADATA_H_
