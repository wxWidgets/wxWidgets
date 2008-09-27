/////////////////////////////////////////////////////////////////////////////
// Name:        hash.h
// Purpose:     interface of wxHashTable
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHashTable

    @b Please note that this class is retained for backward compatibility
    reasons; you should use wxHashMap.

    This class provides hash table functionality for wxWidgets, and for an
    application if it wishes.  Data can be hashed on an integer or string
    key.

    @library{wxbase}
    @category{containers}

    @see wxList
*/
class wxHashTable : public wxObject
{
public:
    /**
        Constructor. @a key_type is one of wxKEY_INTEGER, or wxKEY_STRING,
        and indicates what sort of keying is required. @a size is optional.
    */
    wxHashTable(unsigned int key_type, int size = 1000);

    /**
        Destroys the hash table.
    */
    virtual ~wxHashTable();

    /**
        The counterpart of @e Next.  If the application wishes to iterate
        through all the data in the hash table, it can call @e BeginFind and
        then loop on @e Next.
    */
    void BeginFind();

    /**
        Clears the hash table of all nodes (but as usual, doesn't delete user data).
    */
    void Clear();

    //@{
    /**
        Deletes entry in hash table and returns the user's data (if found).
    */
    wxObject* Delete(long key);
    wxObject* Delete(const wxString& key);
    //@}

    /**
        If set to @true data stored in hash table will be deleted when hash table object
        is destroyed.
    */
    void DeleteContents(bool flag);

    //@{
    /**
        Gets data from the hash table, using an integer or string key (depending on
        which
        has table constructor was used).
    */
    wxObject* Get(long key);
    wxObject* Get(const char* key);
    //@}

    /**
        Returns the number of elements in the hash table.
    */
    size_t GetCount() const;

    /**
        Makes an integer key out of a string. An application may wish to make a key
        explicitly (for instance when combining two data values to form a key).
    */
    static long MakeKey(const wxString& string);

    /**
        If the application wishes to iterate through all the data in the hash
        table, it can call @e BeginFind and then loop on @e Next. This function
        returns a @b Node() pointer (or @NULL if there are no more nodes).
        The return value is functionally equivalent to @b wxNode but might not be
        implemented as a @b wxNode. The user will probably only wish to use the
        @b GetData method to retrieve the data; the node may also be deleted.
    */
    wxHashTable::Node* Next();

    //@{
    /**
        Inserts data into the hash table, using an integer or string key (depending on
        which
        has table constructor was used). The key string is copied and stored by the hash
        table implementation.
    */
    void Put(long key, wxObject* object);
    void Put(const char* key, wxObject* object);
    //@}
};

