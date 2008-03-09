/////////////////////////////////////////////////////////////////////////////
// Name:        list.h
// Purpose:     documentation for wxList<T> class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxListT
    @wxheader{list.h}

    The wxListT class provides linked list functionality. It has been rewritten
    to be type safe and to provide the full API of the STL std::list container and
    should be used like it. The exception is that wxListT actually stores
    pointers and therefore its iterators return pointers and not references
    to the actual objets in the list (see example below) and @e value_type
    is defined as @e T*. wxListT destroys an object after removing it only
    if wxList::DeleteContents has been called.

    wxListT is not a real template and it requires that you declare and define
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
    then wxListT will actually derive from std::list and just add a legacy
    compatibility layer for the old wxList class.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxArrayT, wxVectorT
*/
class wxList<T>
{
public:
    //@{
    /**
        Constructors.
    */
    wxListT();
    wxListT(size_t count, T* elements[]);
    //@}

    /**
        Destroys the list, but does not delete the objects stored in the list
        unless you called DeleteContents(@true ).
    */
    ~wxListT();

    /**
        Appends the pointer to @a object to the list.
    */
    wxListT::compatibility_iterator Append(T* object);

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
    wxListT::compatibility_iterator Find(T* object);

    /**
        Returns the number of elements in the list.
    */
    size_t GetCount();

    /**
        Returns the first iterator in the list (@NULL if the list is empty).
    */
    wxListT::compatibility_iterator GetFirst();

    /**
        Returns the last iterator in the list (@NULL if the list is empty).
    */
    wxListT::compatibility_iterator GetLast();

    /**
        Returns the index of @a obj within the list or @c wxNOT_FOUND if
        @a obj is not found in the list.
    */
    int IndexOf(T* obj);

    //@{
    /**
        Inserts the object before the object refered to be @e iter.
    */
    wxListT::compatibility_iterator Insert(T* object);
    wxListT::compatibility_iterator Insert(size_t position,
                                           T* object);
    wxListT::compatibility_iterator Insert(compatibility_iterator iter,
                                           T* object);
    //@}

    /**
        Returns @true if the list is empty, @false otherwise.
    */
    bool IsEmpty();

    /**
        Returns the iterator refering to the object at the given
        @c index in the list.
    */
    wxListT::compatibility_iterator Item(size_t index);

    /**
        @b NB: This function is deprecated, use wxList::Find instead.
    */
    wxListT::compatibility_iterator Member(T* object);

    /**
        @b NB: This function is deprecated, use @ref wxList::itemfunc Item instead.
        Returns the @e nth node in the list, indexing from zero (@NULL if the list is
        empty
        or the nth node could not be found).
    */
    wxListT::compatibility_iterator Nth(int n);

    /**
        @b NB: This function is deprecated, use wxList::GetCount instead.
        Returns the number of elements in the list.
    */
    int Number();

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
    reference back();
    const_reference back();
    //@}

    //@{
    /**
        Returns a (const) iterator pointing to the beginning of the list.
    */
    iterator begin();
    const_iterator begin();
    //@}

    /**
        Removes all items from the list.
    */
    void clear();

    /**
        Returns @e @true if the list is empty.
    */
    bool empty();

    //@{
    /**
        Returns a (const) iterator pointing at the end of the list.
    */
    iterator end();
    const_iterator end();
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
    reference front();
    const_reference front();
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
    size_type max_size();

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
    reverse_iterator rbegin();
    const_reverse_iterator rbegin();
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
    reverse_iterator rend();
    const_reverse_iterator rend();
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
    size_type size();
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
    "template'' parameter: this is the type of data stored in the linked list or,
    in other words, the first argument of WX_DECLARE_LIST macro. Also, wxNode is
    written as wxNodeT even though it isn't really a template class -- but it
    helps to think of it as if it were.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxList, wxHashTable
*/
class wxNode
{
public:
    /**
        Retrieves the client data pointer associated with the node.
    */
    T* GetData();

    /**
        Retrieves the next node or @NULL if this node is the last one.
    */
    wxNodeT* GetNext();

    /**
        Retrieves the previous node or @NULL if this node is the first one in the list.
    */
    wxNodeT* GetPrevious();

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
