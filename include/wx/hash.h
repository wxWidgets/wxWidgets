/////////////////////////////////////////////////////////////////////////////
// Name:        wx/hash.h
// Purpose:     wxHashTable class
// Author:      Julian Smart
// Modified by: VZ at 25.02.00: type safe hashes with WX_DECLARE_HASH()
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HASH_H__
#define _WX_HASH_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "hash.h"
#endif

#include "wx/defs.h"

#if !wxUSE_STL
    #include "wx/list.h"
#endif
#if WXWIN_COMPATIBILITY_2_4
    #include "wx/dynarray.h"
#endif

class WXDLLIMPEXP_BASE wxObject;

// the default size of the hash
#define wxHASH_SIZE_DEFAULT     (1000)

/*
 * A hash table is an array of user-definable size with lists
 * of data items hanging off the array positions.  Usually there'll
 * be a hit, so no search is required; otherwise we'll have to run down
 * the list to find the desired item.
*/

// ----------------------------------------------------------------------------
// this is the base class for object hashes: hash tables which contain
// pointers to objects
// ----------------------------------------------------------------------------

#if !wxUSE_STL

class WXDLLIMPEXP_BASE wxHashTableBase : public wxObject
{
public:
    wxHashTableBase();

    void Create(wxKeyType keyType = wxKEY_INTEGER,
                size_t size = wxHASH_SIZE_DEFAULT);
    void Destroy();

    size_t GetSize() const { return m_hashSize; }
    size_t GetCount() const { return m_count; }

    void DeleteContents(bool flag);

protected:
    // find the node for (key, value)
    wxNodeBase *GetNode(long key, long value) const;

    // the array of lists in which we store the values for given key hash
    wxListBase **m_hashTable;

    // the size of m_lists array
    size_t m_hashSize;

    // the type of indexing we use
    wxKeyType m_keyType;

    // the total number of elements in the hash
    size_t m_count;

    // should we delete our data?
    bool m_deleteContents;

private:
    // no copy ctor/assignment operator (yet)
    DECLARE_NO_COPY_CLASS(wxHashTableBase)
};

#else

#include "wx/hashmap.h"

#if !defined(wxENUM_KEY_TYPE_DEFINED)
#define wxENUM_KEY_TYPE_DEFINED

enum wxKeyType
{
    wxKEY_NONE,
    wxKEY_INTEGER,
    wxKEY_STRING
};

#endif

union wxHashKeyValue
{
    long integer;
    wxChar *string;
};

struct WXDLLIMPEXP_BASE wxHashTableHash
{
    wxHashTableHash() { }
    wxHashTableHash( wxKeyType keyType ) : m_keyType( keyType ) { }

    wxKeyType m_keyType;

    unsigned long operator ()( const wxHashKeyValue& k ) const
    {
        if( m_keyType == wxKEY_STRING )
            return wxStringHash::wxCharStringHash( k.string );
        else
            return (unsigned long)k.integer;
    }
};

struct WXDLLIMPEXP_BASE wxHashTableEqual
{
    wxHashTableEqual() { }
    wxHashTableEqual( wxKeyType keyType ) : m_keyType( keyType ) { }

    wxKeyType m_keyType;

    bool operator ()( const wxHashKeyValue& k1, const wxHashKeyValue& k2 ) const
    {
        if( m_keyType == wxKEY_STRING )
            return wxStrcmp( k1.string, k2.string ) == 0;
        else
            return k1.integer == k2.integer;
    }
};

WX_DECLARE_EXPORTED_HASH_MAP( wxHashKeyValue, 
                              void*,
                              wxHashTableHash,
                              wxHashTableEqual,
                              wxHashTableBaseBase );

class WXDLLIMPEXP_BASE wxHashTableBase
{
public:
    wxHashTableBase( wxKeyType keyType = wxKEY_INTEGER,
                     size_t size = wxHASH_SIZE_DEFAULT )
        : m_map( size, wxHashTableHash( keyType ),
                 wxHashTableEqual( keyType ) ),
          m_keyType( keyType ) { }

    ~wxHashTableBase()
    {
        if( m_keyType == wxKEY_STRING )
        {
            for( wxHashTableBaseBase::iterator it = m_map.begin(),
                                               en = m_map.end();
                 it != en; )
            {
                wxChar* tmp = it->first.string;
                ++it;
                delete[] tmp; // used in operator++
            }
        }
    }

    size_t GetCount() const { return m_map.size(); }
protected:
    void DoPut( long key, void* data )
    {
        wxHashKeyValue k; k.integer = key;
        m_map[k] = data;
    }

    void DoPut( const wxChar* key, void* data )
    {
        wxHashKeyValue k;
        k.string = new wxChar[wxStrlen(key) + 1];
        wxStrcpy(k.string, key);
        m_map[k] = data;
    }

    void* DoGet( long key ) const
    {
        wxHashKeyValue k; k.integer = key;
        wxHashTableBaseBase::const_iterator it = m_map.find( k );

        return it != m_map.end() ? it->second : NULL;
    }

    void* DoGet( const wxChar* key ) const
    {
        wxHashKeyValue k; k.string = (wxChar*)key;
        wxHashTableBaseBase::const_iterator it = m_map.find( k );

        return it != m_map.end() ? it->second : NULL;
    }

    void* DoDelete( long key )
    {
        wxHashKeyValue k; k.integer = key;
        wxHashTableBaseBase::iterator it = m_map.find( k );
        
        if( it != m_map.end() )
        {
            void* data = it->second;

            m_map.erase( it );
            return data;
        }

        return NULL;
    }

    void* DoDelete( const wxChar* key )
    {
        wxHashKeyValue k; k.string = (wxChar*)key;
        wxHashTableBaseBase::iterator it = m_map.find( k );
        
        if( it != m_map.end() )
        {
            void* data = it->second;
            wxChar* k = it->first.string;

            m_map.erase( it );
            delete[] k;
            return data;
        }

        return NULL;
    }

    wxHashTableBaseBase m_map;
    wxKeyType m_keyType;
};

#endif // !wxUSE_STL

#if !wxUSE_STL

#if WXWIN_COMPATIBILITY_2_4

// ----------------------------------------------------------------------------
// a hash table which stores longs
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxHashTableLong : public wxObject
{
public:
    wxHashTableLong(size_t size = wxHASH_SIZE_DEFAULT)
        { Init(size); }
    virtual ~wxHashTableLong();

    void Create(size_t size = wxHASH_SIZE_DEFAULT);
    void Destroy();

    size_t GetSize() const { return m_hashSize; }
    size_t GetCount() const { return m_count; }

    void Put(long key, long value);
    long Get(long key) const;
    long Delete(long key);

protected:
    void Init(size_t size);

private:
    wxArrayLong **m_values,
                **m_keys;

    // the size of array above
    size_t m_hashSize;

    // the total number of elements in the hash
    size_t m_count;

    // not implemented yet
    DECLARE_NO_COPY_CLASS(wxHashTableLong)
};

// ----------------------------------------------------------------------------
// wxStringHashTable: a hash table which indexes strings with longs
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStringHashTable : public wxObject
{
public:
    wxStringHashTable(size_t sizeTable = wxHASH_SIZE_DEFAULT);
    virtual ~wxStringHashTable();

    // add a string associated with this key to the table
    void Put(long key, const wxString& value);

    // get the string from the key: if not found, an empty string is returned
    // and the wasFound is set to FALSE if not NULL
    wxString Get(long key, bool *wasFound = NULL) const;

    // remove the item, returning TRUE if the item was found and deleted
    bool Delete(long key) const;

    // clean up
    void Destroy();

private:
    wxArrayLong **m_keys;
    wxArrayString **m_values;

    // the size of array above
    size_t m_hashSize;

    DECLARE_NO_COPY_CLASS(wxStringHashTable)
};

#endif // WXWIN_COMPATIBILITY_2_4

#endif  // !wxUSE_STL

// ----------------------------------------------------------------------------
// for compatibility only
// ----------------------------------------------------------------------------

#if wxUSE_STL

class WXDLLIMPEXP_BASE wxHashTable : protected wxHashTableBase
{
    typedef wxHashTableBaseBase hash;
public:
    class dummy;

    struct compatibility_iterator
    {
        hash::iterator m_iter;
        hash* m_hash;

        operator bool() const { return m_iter != m_hash->end(); }
        bool operator !() const { return m_iter == m_hash->end(); }
        compatibility_iterator( hash* li, hash::iterator it )
            : m_iter( it ), m_hash( li ) {}
        compatibility_iterator() { }

        dummy* operator->() { return (dummy*)this; }
    };
    typedef class compatibility_iterator citer;

    class dummy
    {
        typedef hash::iterator it;
        typedef compatibility_iterator citer;
    public:
        wxObject* GetData() const
        {
            citer* i = (citer*)this;
            return (wxObject*)i->m_iter->second;
        }
        citer GetNext() const
        {
            citer* i = (citer*)this;
            it lit = i->m_iter;
            return citer( i->m_hash, ++lit );
        }
        citer GetPrevious() const
        {
            citer* i = (citer*)this;
            it lit = i->m_iter;
            return citer( i->m_hash, ++lit );
        }
        void SetData( wxObject* e )
        {
            citer* i = (citer*)this;
            i->m_iter->second = e;
        }
    private:
        dummy();
    };
public:
    wxHashTable( wxKeyType keyType = wxKEY_INTEGER,
                 size_t size = wxHASH_SIZE_DEFAULT )
        : wxHashTableBase( keyType, size ) { }

    void Destroy() { Clear(); }

    // key and value are the same
    void Put(long value, wxObject *object) { DoPut( value, object ); }
    void Put(const wxChar *value, wxObject *object) { DoPut( value, object ); }

    // key and value are the same
    wxObject *Get(long value) const { return (wxObject*)DoGet( value ); }
    wxObject *Get(const wxChar *value) const { return (wxObject*)DoGet( value ); }

    // Deletes entry and returns data if found
    wxObject *Delete(long key) { return (wxObject*)DoGet( key ); }
    wxObject *Delete(const wxChar *key) { return (wxObject*)DoGet( key ); }

#if 0
    // Construct your own integer key from a string, e.g. in case
    // you need to combine it with something
    long MakeKey(const wxChar *string) const;
#endif
    // Way of iterating through whole hash table (e.g. to delete everything)
    // Not necessary, of course, if you're only storing pointers to
    // objects maintained separately
    void BeginFind() { m_iter = citer( &this->m_map, this->m_map.begin() ); }
    compatibility_iterator Next()
    {
        compatibility_iterator it = m_iter;
        if( m_iter )
            m_iter = m_iter->GetNext();
        return it;
    }

    void Clear() { m_map.clear(); }
private:
    compatibility_iterator m_iter;
};

#else // if !wxUSE_STL

class WXDLLIMPEXP_BASE wxHashTable : public wxObject
{
public:
    int n;
    int current_position;
    wxNode *current_node;

    unsigned int key_type;
    wxList **hash_table;

    wxHashTable(int the_key_type = wxKEY_INTEGER,
                int size = wxHASH_SIZE_DEFAULT);
    ~wxHashTable();

    // copy ctor and assignment operator
    wxHashTable(const wxHashTable& table) : wxObject()
        { DoCopy(table); }
    wxHashTable& operator=(const wxHashTable& table)
        { Clear(); DoCopy(table); return *this; }

    void DoCopy(const wxHashTable& table);

    void Destroy();

    bool Create(int the_key_type = wxKEY_INTEGER,
                int size = wxHASH_SIZE_DEFAULT);

    // Note that there are 2 forms of Put, Get.
    // With a key and a value, the *value* will be checked
    // when a collision is detected. Otherwise, if there are
    // 2 items with a different value but the same key,
    // we'll retrieve the WRONG ONE. So where possible,
    // supply the required value along with the key.
    // In fact, the value-only versions make a key, and still store
    // the value. The use of an explicit key might be required
    // e.g. when combining several values into one key.
    // When doing that, it's highly likely we'll get a collision,
    // e.g. 1 + 2 = 3, 2 + 1 = 3.

    // key and value are NOT necessarily the same
    void Put(long key, long value, wxObject *object);
    void Put(long key, const wxChar *value, wxObject *object);

    // key and value are the same
    void Put(long value, wxObject *object);
    void Put(const wxChar *value, wxObject *object);

    // key and value not the same
    wxObject *Get(long key, long value) const;
    wxObject *Get(long key, const wxChar *value) const;

    // key and value are the same
    wxObject *Get(long value) const;
    wxObject *Get(const wxChar *value) const;

    // Deletes entry and returns data if found
    wxObject *Delete(long key);
    wxObject *Delete(const wxChar *key);

    wxObject *Delete(long key, int value);
    wxObject *Delete(long key, const wxChar *value);

    // Construct your own integer key from a string, e.g. in case
    // you need to combine it with something
    long MakeKey(const wxChar *string) const;

    // Way of iterating through whole hash table (e.g. to delete everything)
    // Not necessary, of course, if you're only storing pointers to
    // objects maintained separately

    void BeginFind();
    wxNode *Next();

    void DeleteContents(bool flag);
    void Clear();

    // Returns number of nodes
    size_t GetCount() const { return m_count; }

    typedef wxNode* compatibility_iterator;
private:
    size_t m_count;             // number of elements in the hashtable
    bool m_deleteContents;

    DECLARE_DYNAMIC_CLASS(wxHashTable)
};

#endif

#if wxUSE_STL

// defines a new type safe hash table which stores the elements of type eltype
// in lists of class listclass
#define _WX_DECLARE_HASH(eltype, dummy, hashclass, classexp)                  \
    classexp hashclass : public wxHashTableBase                               \
    {                                                                         \
    public:                                                                   \
        hashclass(wxKeyType keyType = wxKEY_INTEGER,                          \
                  size_t size = wxHASH_SIZE_DEFAULT)                          \
            : wxHashTableBase(keyType, size) { }                              \
                                                                              \
        ~hashclass() { Destroy(); }                                           \
                                                                              \
        void Destroy() { m_map.clear(); }                                     \
        void Put(long key, eltype *data) { DoPut(key, (void*)data); }         \
        eltype *Get(long key) const { return (eltype*)DoGet(key); }           \
        eltype *Delete(long key) { return (eltype*)DoDelete(key); }           \
    }

#else // if !wxUSE_STL

#define _WX_DECLARE_HASH(eltype, listclass, hashclass, classexp)               \
    classexp hashclass : public wxHashTableBase                                \
    {                                                                          \
    public:                                                                    \
        hashclass(wxKeyType keyType = wxKEY_INTEGER,                           \
                  size_t size = wxHASH_SIZE_DEFAULT)                           \
            { Create(keyType, size); }                                         \
                                                                               \
        ~hashclass() { Destroy(); }                                            \
                                                                               \
        void Put(long key, long val, eltype *data) { DoPut(key, val, data); }  \
        void Put(long key, eltype *data) { DoPut(key, key, data); }            \
                                                                               \
        eltype *Get(long key, long value) const                                \
        {                                                                      \
            wxNodeBase *node = GetNode(key, value);                            \
            return node ? ((listclass::Node *)node)->GetData() : (eltype *)0;  \
        }                                                                      \
        eltype *Get(long key) const { return Get(key, key); }                  \
                                                                               \
        eltype *Delete(long key, long value)                                   \
        {                                                                      \
            eltype *data;                                                      \
                                                                               \
            wxNodeBase *node = GetNode(key, value);                            \
            if ( node )                                                        \
            {                                                                  \
                data = ((listclass::Node *)node)->GetData();                   \
                                                                               \
                delete node;                                                   \
                m_count--;                                                     \
            }                                                                  \
            else                                                               \
            {                                                                  \
                data = (eltype *)0;                                            \
            }                                                                  \
                                                                               \
            return data;                                                       \
        }                                                                      \
        eltype *Delete(long key) { return Delete(key, key); }                  \
                                                                               \
    protected:                                                                 \
        void DoPut(long key, long value, eltype *data)                         \
        {                                                                      \
            size_t slot = (size_t)abs((int)(key % (long)m_hashSize));          \
                                                                               \
            if ( !m_hashTable[slot] )                                          \
            {                                                                  \
                m_hashTable[slot] = new listclass(m_keyType);                  \
                if ( m_deleteContents )                                        \
                    m_hashTable[slot]->DeleteContents(TRUE);                   \
            }                                                                  \
                                                                               \
            ((listclass *)m_hashTable[slot])->Append(value, data);             \
            m_count++;                                                         \
        }                                                                      \
    }

#endif

// this macro is to be used in the user code
#define WX_DECLARE_HASH(el, list, hash) \
    _WX_DECLARE_HASH(el, list, hash, class)

// and this one does exactly the same thing but should be used inside the
// library
#define WX_DECLARE_EXPORTED_HASH(el, list, hash)  \
    _WX_DECLARE_HASH(el, list, hash, class WXDLLEXPORT)

#define WX_DECLARE_USER_EXPORTED_HASH(el, list, hash, usergoo)  \
    _WX_DECLARE_HASH(el, list, hash, class usergoo)

// delete all hash elements
//
// NB: the class declaration of the hash elements must be visible from the
//     place where you use this macro, otherwise the proper destructor may not
//     be called (a decent compiler should give a warning about it, but don't
//     count on it)!
#define WX_CLEAR_HASH_TABLE(hash)                                            \
    {                                                                        \
        (hash).BeginFind();                                                  \
        wxHashTable::compatibility_iterator it = (hash).Next();              \
        while( it )                                                          \
        {                                                                    \
            delete it->GetData();                                            \
            it = (hash).Next();                                              \
        }                                                                    \
        (hash).Clear();                                                      \
    }

#endif
    // _WX_HASH_H__
