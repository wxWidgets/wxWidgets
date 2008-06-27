/////////////////////////////////////////////////////////////////////////////
// Name:        hashmap.h
// Purpose:     interface of wxHashMap
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHashMap

    This is a simple, type-safe, and reasonably efficient hash map class,
    whose interface is a subset of the interface of STL containers. In
    particular, the interface is modeled after std::map, and the various,
    non-standard, std::hash_map.

    @library{wxbase}
    @category{FIXME}
*/
class wxHashMap
{
public:
    //@{
    /**
        Copy constructor.
    */
    wxHashMap(size_type size = 10);
    wxHashMap(const wxHashMap& map);
    //@}

    //@{
    /**
        Returns an iterator pointing at the first element of the hash map.
        Please remember that hash maps do not guarantee ordering.
    */
    const_iterator begin();
    const iterator begin();
    //@}

    /**
        Removes all elements from the hash map.
    */
    void clear();

    /**
        Counts the number of elements with the given key present in the map.
        This function returns only 0 or 1.
    */
    size_type count(const key_type& key) const;

    /**
        Returns @true if the hash map does not contain any elements, @false otherwise.
    */
    bool empty() const;

    //@{
    /**
        Returns an iterator pointing at the one-after-the-last element of the hash map.
        Please remember that hash maps do not guarantee ordering.
    */
    const_iterator end();
    const iterator end();
    //@}

    //@{
    /**
        Erases the element pointed to by the iterator. After the deletion
        the iterator is no longer valid and must not be used.
    */
    size_type erase(const key_type& key);
    void erase(iterator it);
    void erase(const_iterator it);
    //@}

    //@{
    /**
        If an element with the given key is present, the functions returns
        an iterator pointing at that element, otherwise an invalid iterator
        is returned (i.e. hashmap.find( non_existent_key ) == hashmap.end()).
    */
    iterator find(const key_type& key) const;
    const_iterator find(const key_type& key) const;
    //@}

    /**
        Inserts the given value in the hash map. The return value is
        equivalent to a @c std::pairiterator(), bool;
        the iterator points to the inserted element, the boolean value
        is @true if @c v was actually inserted.
    */
    Insert_Result insert(const value_type& v);

    /**
        Use the key as an array subscript. The only difference is that if the
        given key is not present in the hash map, an element with the
        default @c value_type() is inserted in the table.
    */
    mapped_type operator[](const key_type& key);

    /**
        Returns the number of elements in the map.
    */
    size_type size() const;
};

