/////////////////////////////////////////////////////////////////////////////
// Name:        hashset.h
// Purpose:     interface of wxHashSet
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHashSet
    @wxheader{hashset.h}

    This is a simple, type-safe, and reasonably efficient hash set class,
    whose interface is a subset of the interface of STL containers. In
    particular, the interface is modeled after std::set, and the various,
    non-standard, std::hash_map.

    @library{wxbase}
    @category{FIXME}
*/
class wxHashSet
{
public:
    //@{
    /**
        Copy constructor.
    */
    wxHashSet(size_type size = 10);
    wxHashSet(const wxHashSet& set);
    //@}

    //@{
    /**
        Returns an iterator pointing at the first element of the hash set.
        Please remember that hash sets do not guarantee ordering.
    */
    const_iterator begin();
    const iterator begin();
    //@}

    /**
        Removes all elements from the hash set.
    */
    void clear();

    /**
        Counts the number of elements with the given key present in the set.
        This function returns only 0 or 1.
    */
    size_type count(const key_type& key) const;

    /**
        Returns @true if the hash set does not contain any elements, @false otherwise.
    */
    bool empty() const;

    //@{
    /**
        Returns an iterator pointing at the one-after-the-last element of the hash set.
        Please remember that hash sets do not guarantee ordering.
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
        is returned (i.e. hashset.find( non_existent_key ) == hashset.end()).
    */
    iterator find(const key_type& key) const;
    const_iterator find(const key_type& key) const;
    //@}

    /**
        Inserts the given value in the hash set. The return value is
        equivalent to a @c std::pairwxHashMap::iterator, bool;
        the iterator points to the inserted element, the boolean value
        is @true if @c v was actually inserted.
    */
    Insert_Result insert(const value_type& v);

    /**
        Returns the number of elements in the set.
    */
    size_type size() const;
};

