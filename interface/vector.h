/////////////////////////////////////////////////////////////////////////////
// Name:        vector.h
// Purpose:     interface of wxVector<T>
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxVectorT
    @wxheader{vector.h}

    wxVectorT is a template class which implements most of the std::vector
    class and can be used like it. If wxWidgets is compiled in STL mode,
    wxVector will just be a typedef to std::vector. Just like for std::vector,
    objects stored in wxVectorT need to be @e assignable but don't have to
    be @e default constructible.

    You can refer to the STL documentation for further information.

    @library{wxbase}
    @category{FIXME}

    @see @ref overview_wxcontaineroverview, wxListT(), wxArrayT()
*/
class wxVector<T>
{
public:
    //@{
    /**
        Constructor.
    */
    wxVectorT();
    wxVectorT(const wxVector<T>& c);
    //@}

    /**
        Destructor.
    */
    ~wxVectorT();

    //@{
    /**
        Returns item at position @e idx.
    */
    const value_type at(size_type idx);
    const value_type at(size_type idx);
    //@}

    //@{
    /**
        Return last item.
    */
    const value_type back();
    const value_type back();
    //@}

    //@{
    /**
        Return iterator to beginning of the vector.
    */
    const_iterator begin();
    const iterator begin();
    //@}

    /**

    */
    size_type capacity() const;

    /**
        Clears the vector.
    */
    void clear();

    /**
        Returns @true if the vector is empty.
    */
    bool empty() const;

    //@{
    /**
        Returns iterator to the end of the vector.
    */
    const_iterator end();
    const iterator end();
    //@}

    //@{
    /**
        Erase items. When using values other than built-in integrals
        or classes with reference counting this can be an inefficient
        operation.
    */
    iterator erase(iterator it);
    iterator erase(iterator first, iterator last);
    //@}

    //@{
    /**
        Returns first item.
    */
    const value_type front();
    const value_type front();
    //@}

    /**
        )
        Insert an item. When using values other than built-in integrals
        or classes with reference counting this can be an inefficient
        operation.
    */
    iterator insert(iterator it);

    /**
        Assignment operator.
    */
    wxVectorT& operator operator=(const wxVector<T>& vb);

    //@{
    /**
        Returns item at position @e idx.
    */
    const value_type operator[](size_type idx);
    const value_type operator[](size_type idx);
    //@}

    /**
        Removes the last item.
    */
    void pop_back();

    /**
        Adds an item to the end of the vector.
    */
    void push_back(const value_type& v);

    /**
        Reserves more memory of @a n is greater then
        wxVector::size. Other this call has
        no effect.
    */
    void reserve(size_type n);
};

