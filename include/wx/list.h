/////////////////////////////////////////////////////////////////////////////
// Name:        list.h
// Purpose:     wxList, wxStringList classes
// Author:      Julian Smart
// Modified by: VZ at 16/11/98: WX_DECLARE_LIST() and typesafe lists added
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*
  All this is quite ugly but serves two purposes:
    1. Be almost 100% compatible with old, untyped, wxList class
    2. Ensure compile-time type checking for the linked lists

  The idea is to have one base class (wxListBase) working with "void *" data,
  but to hide these untyped functions - i.e. make them protected, so they
  can only be used from derived classes which have inline member functions
  working with right types. This achieves the 2nd goal. As for the first one,
  we provide a special derivation of wxListBase called wxList which looks just
  like the old class.
*/

#ifndef _WX_LISTH__
#define _WX_LISTH__

#ifdef __GNUG__
#pragma interface "list.h"
#endif

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/debug.h"
#include "wx/object.h"
#include "wx/string.h"

// due to circular header dependencies this function has to be declared here
// (normally it's found in utils.h which includes itself list.h...)
extern WXDLLEXPORT char* copystring(const char *s);

class WXDLLEXPORT wxObjectListNode;
typedef wxObjectListNode wxNode;

// undef it to get rid of old, deprecated functions
#define wxLIST_COMPATIBILITY

// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------
enum wxKeyType
{
    wxKEY_NONE,
    wxKEY_INTEGER,
    wxKEY_STRING
};

// -----------------------------------------------------------------------------
// types
// -----------------------------------------------------------------------------

// type of compare function for list sort operation (as in 'qsort'): it should
// return a negative value, 0 or positive value if the first element is less
// than, equal or greater than the second
typedef int (*wxSortCompareFunction)(const void *elem1, const void *elem2);

//
typedef int (*wxListIterateFunction)(void *current);

// -----------------------------------------------------------------------------
// key stuff: a list may be optionally keyed on integer or string key
// -----------------------------------------------------------------------------

union wxListKeyValue
{
    long integer;
    char *string;
};

// a struct which may contain both types of keys
//
// implementation note: on one hand, this class allows to have only one function
// for any keyed operation instead of 2 almost equivalent. OTOH, it's needed to
// resolve ambiguity which we would otherwise have with wxStringList::Find() and
// wxList::Find(const char *).
class WXDLLEXPORT wxListKey
{
public:
    // implicit ctors
    wxListKey()
        { m_keyType = wxKEY_NONE; }
    wxListKey(long i)
        { m_keyType = wxKEY_INTEGER; m_key.integer = i; }
    wxListKey(const char *s)
        { m_keyType = wxKEY_STRING; m_key.string = strdup(s); }
    wxListKey(const wxString& s)
        { m_keyType = wxKEY_STRING; m_key.string = strdup(s.c_str()); }

    // accessors
    wxKeyType GetKeyType() const { return m_keyType; }
    const char *GetString() const
        { wxASSERT( m_keyType == wxKEY_STRING ); return m_key.string; }
    long GetNumber() const
        { wxASSERT( m_keyType == wxKEY_INTEGER ); return m_key.integer; }

    // comparison
    // Note: implementation moved to list.cpp to prevent BC++ inline
    // expansion warning.
    bool operator==(wxListKeyValue value) const ;

    // dtor
    ~wxListKey()
    {
        if ( m_keyType == wxKEY_STRING )
            free(m_key.string);
    }

private:
    wxKeyType m_keyType;
    wxListKeyValue m_key;
};

// -----------------------------------------------------------------------------
// wxNodeBase class is a (base for) node in a double linked list
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxNodeBase
{
friend class wxListBase;
public:
    // ctor
    wxNodeBase(wxListBase *list = (wxListBase *)NULL,
               wxNodeBase *previous = (wxNodeBase *)NULL,
               wxNodeBase *next = (wxNodeBase *)NULL,
               void *data = NULL,
               const wxListKey& key = wxListKey());

    virtual ~wxNodeBase();

    // @@ no check is done that the list is really keyed on strings
    const char *GetKeyString() const { return m_key.string; }
    long GetKeyInteger() const { return m_key.integer; }

    // Necessary for some existing code
    void SetKeyString(char* s) { m_key.string = s; }
    void SetKeyInteger(long i) { m_key.integer = i; }

#ifdef wxLIST_COMPATIBILITY
    // compatibility methods
    wxNode *Next() const { return (wxNode *)GetNext(); }
    wxNode *Previous() const { return (wxNode *)GetPrevious(); }
    wxObject *Data() const { return (wxObject *)GetData(); }
#endif // wxLIST_COMPATIBILITY

protected:
    // all these are going to be "overloaded" in the derived classes
    wxNodeBase *GetNext() const { return m_next; }
    wxNodeBase *GetPrevious() const { return m_previous; }

    void *GetData() const { return m_data; }
    void SetData(void *data) { m_data = data; }

    // get 0-based index of this node within the list or NOT_FOUND
    int IndexOf() const;

    virtual void DeleteData() { }

private:
    // optional key stuff
    wxListKeyValue m_key;

    void        *m_data;        // user data
    wxNodeBase  *m_next,        // next and previous nodes in the list
                *m_previous;

    wxListBase  *m_list;        // list we belong to
};

// -----------------------------------------------------------------------------
// a double-linked list class
// -----------------------------------------------------------------------------
class WXDLLEXPORT wxListBase : public wxObject
{
friend class wxNodeBase; // should be able to call DetachNode()
public:
    // default ctor & dtor
    wxListBase(wxKeyType keyType = wxKEY_NONE) { Init(keyType); }
    virtual ~wxListBase();

    // accessors
        // count of items in the list
    size_t GetCount() const { return m_count; }

    // operations
        // delete all nodes
    void Clear();
        // instruct it to destroy user data when deleting nodes
    void DeleteContents(bool destroy) { m_destroy = destroy; }

protected:
    // all methods here are "overloaded" in derived classes to provide compile
    // time type checking

    // create a node for the list of this type
    virtual wxNodeBase *CreateNode(wxNodeBase *prev, wxNodeBase *next,
                                   void *data,
                                   const wxListKey& key = wxListKey()) = 0;

    // ctors
        // from an array
    wxListBase(size_t count, void *elements[]);
        // from a sequence of objects
    wxListBase(void *object, ... /* terminate with NULL */);

        // copy ctor and assignment operator
    wxListBase(const wxListBase& list)
        { DoCopy(list); }
    wxListBase& operator=(const wxListBase& list)
        { Clear(); DoCopy(list); return *this; }

        // get list head/tail
    wxNodeBase *GetFirst() const { return m_nodeFirst; }
    wxNodeBase *GetLast() const { return m_nodeLast; }

        // by (0-based) index
    wxNodeBase *Item(size_t index) const;

        // get the list item's data
    void *operator[](size_t index) const
        { wxNodeBase *node = Item(index); return node ? node->GetData() : (wxNodeBase*)NULL; }

    // operations
        // append to end of list
    wxNodeBase *Append(void *object);
        // insert a new item at the beginning of the list
    wxNodeBase *Insert(void *object) { return Insert( (wxNodeBase*)NULL, object); }
        // insert before given node or at front of list if prev == NULL
    wxNodeBase *Insert(wxNodeBase *prev, void *object);

        // keyed append
    wxNodeBase *Append(long key, void *object);
    wxNodeBase *Append(const char *key, void *object);

        // removes node from the list but doesn't delete it (returns pointer
        // to the node or NULL if it wasn't found in the list)
    wxNodeBase *DetachNode(wxNodeBase *node);
        // delete element from list, returns FALSE if node not found
    bool DeleteNode(wxNodeBase *node);
        // finds object pointer and deletes node (and object if DeleteContents
        // is on), returns FALSE if object not found
    bool DeleteObject(void *object);

    // search (all return NULL if item not found)
        // by data
    wxNodeBase *Find(void *object) const;

        // by key
    wxNodeBase *Find(const wxListKey& key) const;

    // get 0-based index of object or NOT_FOUND
    int IndexOf( void *object ) const;

    // this function allows the sorting of arbitrary lists by giving
    // a function to compare two list elements. The list is sorted in place.
    void Sort(const wxSortCompareFunction compfunc);

    // functions for iterating over the list
    void *FirstThat(wxListIterateFunction func);
    void ForEach(wxListIterateFunction func);
    void *LastThat(wxListIterateFunction func);

private:
    // helpers
        // common part of all ctors
    void Init(wxKeyType keyType);
        // common part of copy ctor and assignment operator
    void DoCopy(const wxListBase& list);
        // common part of all Append()s
    wxNodeBase *AppendCommon(wxNodeBase *node);
        // free node's data and node itself
    void DoDeleteNode(wxNodeBase *node);

    size_t m_count;             // number of elements in the list
    bool m_destroy;             // destroy user data when deleting list items?
    wxNodeBase *m_nodeFirst,    // pointers to the head and tail of the list
               *m_nodeLast;

    wxKeyType m_keyType;        // type of our keys (may be wxKEY_NONE)
};

// -----------------------------------------------------------------------------
// macros for definition of "template" list type
// -----------------------------------------------------------------------------

// and now some heavy magic...

// declare a list type named 'name' and containing elements of type 'T *'
// (as a by product of macro expansion you also get wx##name##Node
// wxNode-dervied type)
//
// implementation details:
//  1. we define _WX_LIST_ITEM_TYPE_##name typedef to save in it the item type
//     for the list of given type - this allows us to pass only the list name
//     to WX_DEFINE_LIST() even if it needs both the name and the type
//
//  2. We redefine all not type-safe wxList functions withtype-safe versions
//     which don't take any place (everything is inline), but bring compile
//     time error checking.

#define WX_DECLARE_LIST_2(T, name, nodetype)                                \
    typedef int (*wxSortFuncFor_##name)(const T *, const T *);              \
                                                                            \
    class WXDLLEXPORT nodetype : public wxNodeBase                          \
    {                                                                       \
    public:                                                                 \
        nodetype(wxListBase *list = (wxListBase *)NULL,                     \
                 nodetype *previous = (nodetype *)NULL,                     \
                 nodetype *next = (nodetype *)NULL,                         \
                 T *data = (T *)NULL,                                       \
                 const wxListKey& key = wxListKey())                        \
            : wxNodeBase(list, previous, next, data, key) { }               \
                                                                            \
        nodetype *GetNext() const                                           \
            { return (nodetype *)wxNodeBase::GetNext(); }                   \
        nodetype *GetPrevious() const                                       \
            { return (nodetype *)wxNodeBase::GetPrevious(); }               \
                                                                            \
        T *GetData() const                                                  \
            { return (T *)wxNodeBase::GetData(); }                          \
        void SetData(T *data)                                               \
            { wxNodeBase::SetData(data); }                                  \
                                                                            \
        virtual void DeleteData();                                          \
    };                                                                      \
                                                                            \
    class WXDLLEXPORT name : public wxListBase                                          \
    {                                                                       \
    public:                                                                 \
        name(wxKeyType keyType = wxKEY_NONE) : wxListBase(keyType)          \
            { }                                                             \
        name(size_t count, T *elements[])                                   \
            : wxListBase(count, (void **)elements) { }                      \
                                                                            \
        nodetype *GetFirst() const                                          \
            { return (nodetype *)wxListBase::GetFirst(); }                  \
        nodetype *GetLast() const                                           \
            { return (nodetype *)wxListBase::GetLast(); }                   \
                                                                            \
        nodetype *Item(size_t index) const                                  \
            { return (nodetype *)wxListBase::Item(index); }                 \
                                                                            \
        T *operator[](size_t index) const                                   \
        {                                                                   \
            nodetype *node = Item(index);                                   \
            return node ? (T*)(node->GetData()) : (T*)NULL;                 \
        }                                                                   \
                                                                            \
        nodetype *Append(T *object)                                         \
            { return (nodetype *)wxListBase::Append(object); }              \
        nodetype *Insert(T *object)                                         \
            { return (nodetype *)Insert((nodetype*)NULL, object); }         \
        nodetype *Insert(nodetype *prev, T *object)                         \
            { return (nodetype *)wxListBase::Insert(prev, object); }        \
                                                                            \
        nodetype *Append(long key, void *object)                            \
            { return (nodetype *)wxListBase::Append(key, object); }         \
        nodetype *Append(const char *key, void *object)                     \
            { return (nodetype *)wxListBase::Append(key, object); }         \
                                                                            \
        nodetype *DetachNode(nodetype *node)                                \
            { return (nodetype *)wxListBase::DetachNode(node); }            \
        bool DeleteNode(nodetype *node)                                     \
            { return wxListBase::DeleteNode(node); }                        \
        bool DeleteObject(T *object)                                        \
            { return wxListBase::DeleteObject(object); }                    \
                                                                            \
        nodetype *Find(T *object) const                                     \
            { return (nodetype *)wxListBase::Find(object); }                \
                                                                            \
        virtual nodetype *Find(const wxListKey& key) const                  \
            { return (nodetype *)wxListBase::Find(key); }                   \
                                                                            \
        int IndexOf( T *object ) const                                      \
            { return wxListBase::IndexOf(object); }                         \
                                                                            \
        void Sort(wxSortFuncFor_##name func)                                \
            { wxListBase::Sort((wxSortCompareFunction)func); }              \
                                                                            \
    protected:                                                              \
        wxNodeBase *CreateNode(wxNodeBase *prev, wxNodeBase *next,          \
                               void *data,                                  \
                               const wxListKey& key = wxListKey())          \
            {                                                               \
                return new nodetype(this,                                   \
                                    (nodetype *)prev, (nodetype *)next,     \
                                    (T *)data, key);                        \
            }                                                               \
    }

#define WX_DECLARE_LIST(elementtype, listname)                              \
    typedef elementtype _WX_LIST_ITEM_TYPE_##listname;                      \
    WX_DECLARE_LIST_2(elementtype, listname, wx##listname##Node)

// this macro must be inserted in your program after
//      #include <wx/listimpl.cpp>
#define WX_DEFINE_LIST(name)    "don't forget to include listimpl.cpp!"


// =============================================================================
// now we can define classes 100% compatible with the old ones
// =============================================================================

#ifdef wxLIST_COMPATIBILITY

// -----------------------------------------------------------------------------
// wxList compatibility class: in fact, it's a list of wxObjects
// -----------------------------------------------------------------------------

WX_DECLARE_LIST_2(wxObject, wxObjectList, wxObjectListNode);

class WXDLLEXPORT wxList : public wxObjectList
{
public:
    wxList(int key_type = wxKEY_NONE) : wxObjectList((wxKeyType)key_type) { }

    // compatibility methods
    void Sort(wxSortCompareFunction compfunc) { wxListBase::Sort(compfunc); }

    int Number() const { return GetCount(); }
    wxNode *First() const { return (wxNode *)GetFirst(); }
    wxNode *Last() const { return (wxNode *)GetLast(); }
    wxNode *Nth(size_t index) const { return (wxNode *)Item(index); }
    wxNode *Member(wxObject *object) const { return (wxNode *)Find(object); }
};

// -----------------------------------------------------------------------------
// wxStringList class for compatibility with the old code
// -----------------------------------------------------------------------------

WX_DECLARE_LIST_2(char, wxStringListBase, wxStringListNode);

class WXDLLEXPORT wxStringList : public wxStringListBase
{
public:
    // ctors and such
        // default
    wxStringList() { DeleteContents(TRUE); }
    wxStringList(const char *first ...);

        // copying the string list: the strings are copied, too (extremely
        // inefficient!)
    wxStringList(const wxStringList& other) { DoCopy(other); }
    wxStringList& operator=(const wxStringList& other)
        { Clear(); DoCopy(other); return *this; }

    // operations
        // makes a copy of the string
    wxNode *Add(const char *s)
        { return (wxNode *)wxStringListBase::Append(copystring(s)); }

    void Delete(const char *s)
        { wxStringListBase::DeleteObject((char *)s); }

    char **ListToArray(bool new_copies = FALSE) const;
    bool Member(const char *s) const;

    // alphabetic sort
    void Sort();

    // compatibility methods
    int Number() const { return GetCount(); }
    wxNode *First() const { return (wxNode *)GetFirst(); }
    wxNode *Last() const { return (wxNode *)GetLast(); }
    wxNode *Nth(size_t index) const { return (wxNode *)Item(index); }

private:
    void DoCopy(const wxStringList&); // common part of copy ctor and operator=
};

#endif // wxLIST_COMPATIBILITY

#endif
    // _WX_LISTH__
