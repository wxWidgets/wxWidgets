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

#ifdef __GNUG__
#pragma interface "hash.h"
#endif

#include "wx/list.h"
#include "wx/dynarray.h"

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

class WXDLLEXPORT wxHashTableBase : public wxObject
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
    DECLARE_NO_COPY_CLASS(wxHashTableBase);
};

// ----------------------------------------------------------------------------
// a hash table which stores longs
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxHashTableLong : public wxObject
{
public:
    wxHashTableLong(size_t size = wxHASH_SIZE_DEFAULT) { Init(size); }
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
    DECLARE_NO_COPY_CLASS(wxHashTableLong);
};

// ----------------------------------------------------------------------------
// for compatibility only
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxHashTable : public wxObject
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
    wxHashTable(const wxHashTable& table) { DoCopy(table); }
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

private:
    size_t m_count;             // number of elements in the hashtable
    bool m_deleteContents;

    DECLARE_DYNAMIC_CLASS(wxHashTable)
};

// defines a new type safe hash table which stores the elements of type eltype
// in lists of class listclass
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

// this macro is to be used in the user code
#define WX_DECLARE_HASH(el, list, hash) \
    _WX_DECLARE_HASH(el, list, hash, class)

// and this one does exactly the same thing but should be used inside the
// library
#define WX_DECLARE_EXPORTED_HASH(el, list, hash)  \
    _WX_DECLARE_HASH(el, list, hash, class WXDLLEXPORT)

#endif
    // _WX_HASH_H__
