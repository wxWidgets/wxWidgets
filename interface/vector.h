/////////////////////////////////////////////////////////////////////////////
// Name:        vector.h
// Purpose:     documentation for wxVector<T> class
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
    
    @seealso
    @ref overview_wxcontaineroverview "Container classes overview", wxListT,
    wxArrayT
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
        value_type at(size_type idx);
    //@}

    //@{
    /**
        Return last item.
    */
    const value_type back();
        value_type back();
    //@}

    //@{
    /**
        Return iterator to beginning of the vector.
    */
    const_iterator begin();
        iterator begin();
    //@}

    /**
        
    */
    size_type capacity();

    /**
        Clears the vector.
    */
    void clear();

    /**
        Returns @true if the vector is empty.
    */
    bool empty();

    //@{
    /**
        Returns iterator to the end of the vector.
    */
    const_iterator end();
        iterator end();
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
        value_type front();
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
        value_type operator[](size_type idx);
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
        Reserves more memory of @e n is greater then 
        wxVector::size. Other this call has
        no effect.
    */
    void reserve(size_type n);
};
