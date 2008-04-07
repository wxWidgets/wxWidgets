/////////////////////////////////////////////////////////////////////////////
// Name:        list.h
// Purpose:     interface of wxList<T>
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @wxheader{list.h}

    The wxList<T> class provides linked list functionality. It has been rewritten
    to be type safe and to provide the full API of the STL std::list container and
    should be used like it. The exception is that wxList<T> actually stores
    pointers and therefore its iterators return pointers and not references
    to the actual objets in the list (see example below) and @e value_type
    is defined as @e T*. wxList<T> destroys an object after removing it only
    if wxList::DeleteContents has been called.

    wxList<T> is not a real template and it requires that you declare and define
    each wxListT class in your program. This is done with @e WX_DECLARE_LIST
    and @e WX_DEFINE_LIST macros (see example). We hope that we'll be able
    to provide a proper template class providing both the STL std::list
    and the old wxList API in the future.

    Please refer to the STL std::list documentation for further
    information on how to use the class. Below we documented both
    the supported STL and the legacy API that originated from the
    old wxList class and which can still be used alternatively for
    the the same class.

    Note that if you compile wxWidgets in STL mode (wxUSE_STL defined as 1)
    then wxList<T> will actually derive from std::list and just add a legacy
    compatibility layer for the old wxList class.

    @library{wxbase}
    @category{FIXME}

    @see wxArray, wxVector
*/
template<typename T>
class wxList<T>
{
public:
    //@{
    /**
        Constructors.
    */
    wxList();
    wxList(size_t count, T* elements[]);
    //@}

    /**
        Destroys the list, but does not delete the objects stored in the list
        unless you called DeleteContents(@true ).
    */
    ~wxList();

    /**
        Appends the pointer to @a object to the list.
    */
    wxList<T>::compatibility_iterator Append(T* object);

    /**
        Clears the list, but does not delete the objects stored in the list
        unless you called DeleteContents(@true ).
    */
    void Clear();

    /**
        If @a destroy is @true, instructs the list to call @e delete
        on objects stored in the list whenever they are removed.
        The default is @false.
    */
    void DeleteContents(bool destroy);

    /**
        Deletes the given element refered to by @c iter from the list,
        returning @true if successful.
    */
    bool DeleteNode(const compatibility_iterator& iter);

    /**
        Finds the given @a object and removes it from the list, returning
        @true if successful. The application must delete the actual object
        separately.
    */
    bool DeleteObject(T* object);

    /**
        Removes element refered to be @c iter.
    */
    void Erase(const compatibility_iterator& iter);

    /**
        Returns the iterator refering to @a object or @NULL if none found.
    */
    wxList<T>::compatibility_iterator Find(T* object) const;

    /**
        Returns the number of elements in the list.
    */
    size_t GetCount() const;

    /**
        Returns the first iterator in the list (@NULL if the list is empty).
    */
    wxList<T>::compatibility_iterator GetFirst() const;

    /**
        Returns the last iterator in the list (@NULL if the list is empty).
    */
    wxList<T>::compatibility_iterator GetLast() const;

    /**
        Returns the index of @a obj within the list or @c wxNOT_FOUND if
        @a obj is not found in the list.
    */
    int IndexOf(T* obj) const;

    //@{
    /**
        Inserts the object before the object refered to be @e iter.
    */
    wxList<T>::compatibility_iterator Insert(T* object);
    wxList<T>::compatibility_iterator Insert(size_t position,
                                           T* object);
    wxList<T>::compatibility_iterator Insert(compatibility_iterator iter,
                                           T* object);
    //@}

    /**
        Returns @true if the list is empty, @false otherwise.
    */
    bool IsEmpty() const;

    /**
        Returns the iterator refering to the object at the given
        @c index in the list.
    */
    wxList<T>::compatibility_iterator Item(size_t index) const;

    /**
        @note This function is deprecated, use wxList::Find instead.
    */
    wxList<T>::compatibility_iterator Member(T* object) const;

    /**
        @note This function is deprecated, use @ref wxList::itemfunc Item instead.
        Returns the @e nth node in the list, indexing from zero (@NULL if the list is
        empty
        or the nth node could not be found).
    */
    wxList<T>::compatibility_iterator Nth(int n) const;

    /**
        @note This function is deprecated, use wxList::GetCount instead.
        Returns the number of elements in the list.
    */
    int Number() const;

    /**
        Allows the sorting of arbitrary lists by giving a function to compare
        two list elements. We use the system @b qsort function for the actual
        sorting process.
    */
    void Sort(wxSortCompareFunction compfunc);

    //@{
    /**
        )
    */
    void assign(const_iterator first, const const_iterator& last);
    void assign(size_type n);
    //@}

    //@{
    /**
        Returns the last item of the list.
    */
    reference back() const;
    const_reference back() const;
    //@}

    //@{
    /**
        Returns a (const) iterator pointing to the beginning of the list.
    */
    iterator begin() const;
    const_iterator begin() const;
    //@}

    /**
        Removes all items from the list.
    */
    void clear();

    /**
        Returns @e @true if the list is empty.
    */
    bool empty() const;

    //@{
    /**
        Returns a (const) iterator pointing at the end of the list.
    */
    iterator end() const;
    const_iterator end() const;
    //@}

    //@{
    /**
        Erases the items from @a first to @e last.
    */
    iterator erase(const iterator& it);
    iterator erase(const iterator& first,
                   const iterator& last);
    //@}

    //@{
    /**
        Returns the first item in the list.
    */
    reference front() const;
    const_reference front() const;
    //@}

    //@{
    /**
        Inserts an item (or several) at the given position.
    */
    iterator insert(const iterator& it);
    void insert(const iterator& it, size_type n);
    void insert(const iterator& it, const_iterator first,
                const const_iterator& last);
    //@}

    /**
        Returns the largest possible size of the list.
    */
    size_type max_size() const;

    /**
        Removes the last item from the list.
    */
    void pop_back();

    /**
        Removes the first item from the list.
    */
    void pop_front();

    /**
        )
        Adds an item to end of the list.
    */
    void push_back();

    /**
        )
        Adds an item to the front of the list.
    */
    void push_front();

    //@{
    /**
        Returns a (const) reverse iterator pointing to the beginning of the
        reversed list.
    */
    reverse_iterator rbegin() const;
    const_reverse_iterator rbegin() const;
    //@}

    /**
        Removes an item from the list.
    */
    void remove(const_reference v);

    //@{
    /**
        Returns a (const) reverse iterator pointing to the end of the
        reversed list.
    */
    reverse_iterator rend() const;
    const_reverse_iterator rend() const;
    //@}

    /**
        )
        Resizes the list. If the the list is enlarges items with
        the value @e v are appended to the list.
    */
    void resize(size_type n);

    /**
        Reverses the list.
    */
    void reverse();

    /**
        Returns the size of the list.
    */
    size_type size() const;
};



/**
    @class wxNode
    @wxheader{list.h}

    wxNodeBase is the node structure used in linked lists (see
    wxList) and derived classes. You should never use wxNodeBase
    class directly, however, because it works with untyped (@c void *) data and
    this is unsafe. Use wxNodeBase-derived classes which are automatically defined
    by WX_DECLARE_LIST and WX_DEFINE_LIST macros instead as described in
    wxList documentation (see example there). Also note that
    although there is a class called wxNode, it is defined for backwards
    compatibility only and usage of this class is strongly deprecated.

    In the documentation below, the type @c T should be thought of as a
    "template" parameter: this is the type of data stored in the linked list or,
    in other words, the first argument of WX_DECLARE_LIST macro. Also, wxNode is
    written as wxNodeT even though it isn't really a template class -- but it
    helps to think of it as if it were.

    @library{wxbase}
    @category{FIXME}

    @see wxList, wxHashTable
*/
class wxNode
{
public:
    /**
        Retrieves the client data pointer associated with the node.
    */
    T* GetData() const;

    /**
        Retrieves the next node or @NULL if this node is the last one.
    */
    wxNode<T>* GetNext() const;

    /**
        Retrieves the previous node or @NULL if this node is the first one in the list.
    */
    wxNode<T>* GetPrevious();

    /**
        Returns the zero-based index of this node within the list. The return value
        will be @c wxNOT_FOUND if the node has not been added to a list yet.
    */
    int IndexOf();

    /**
        Sets the data associated with the node (usually the pointer will have been
        set when the node was created).
    */
    void SetData(T* data);
};

