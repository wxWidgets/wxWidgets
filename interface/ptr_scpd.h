/////////////////////////////////////////////////////////////////////////////
// Name:        ptr_scpd.h
// Purpose:     interface of wxScopedPtr
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxScopedPtr
    @wxheader{ptr_scpd.h}

    This is a simple scoped smart pointer implementation that is similar to
    the Boost smart pointers but rewritten to
    use macros instead.

    Since wxWidgets 2.9.0 there is also a templated version of this class
    with the same name. See wxScopedPtrT().

    A smart pointer holds a pointer to an object. The memory used by the object is
    deleted when the smart pointer goes out of scope. This class is different from
    the @c std::auto_ptr in so far as it doesn't provide copy constructor
    nor assignment operator. This limits what you can do with it but is much less
    surprizing than the "destructive copy'' behaviour of the standard class.

    @library{wxbase}
    @category{FIXME}

    @see wxScopedArray
*/
class wxScopedPtr
{
public:
    /**
        Creates the smart pointer with the given pointer or none if @NULL.  On
        compilers that support it, this uses the explicit keyword.
    */
    explicit wxScopedPtr(type T = NULL);

    /**
        Destructor frees the pointer help by this object if it is not @NULL.
    */
    ~wxScopedPtr();

    /**
        This operator gets the pointer stored in the smart pointer or returns @NULL if
        there is none.
    */
    const T* get();

    /**
        This operator works like the standard C++ pointer operator to return the object
        being pointed to by the pointer.  If the pointer is @NULL or invalid this will
        crash.
    */
    const T operator *();

    /**
        This operator works like the standard C++ pointer operator to return the pointer
        in the smart pointer or @NULL if it is empty.
    */
    const T* operator -();

    /**
        Returns the currently hold pointer and resets the smart pointer object to
        @NULL. After a call to this function the caller is responsible for
        deleting the pointer.
    */
    T* release();

    /**
        Deletes the currently held pointer and sets it to @a p or to @NULL if no
        arguments are specified. This function does check to make sure that the
        pointer you are assigning is not the same pointer that is already stored.
    */
    reset(T p  = NULL);

    /**
        Swap the pointer inside the smart pointer with @e other. The pointer being
        swapped must be of the same type (hence the same class name).
    */
    swap(wxScopedPtr amp; other);
};



/**
    @class wxScopedArray
    @wxheader{ptr_scpd.h}

    This is a simple scoped smart pointer array implementation that is similar to
    the Boost smart pointers but rewritten to
    use macros instead.

    @library{wxbase}
    @category{FIXME}

    @see wxScopedPtr
*/
class wxScopedArray
{
public:
    /**
        Creates the smart pointer with the given pointer or none if @NULL.  On
        compilers that support it, this uses the explicit keyword.
    */
    wxScopedArray(type T = NULL);

    /**
        This operator gets the pointer stored in the smart pointer or returns @NULL if
        there is none.
    */
    const T* get();

    /**
        This operator acts like the standard [] indexing operator for C++ arrays.  The
        function does not do bounds checking.
    */
    const T operator [](long int i);

    /**
        Deletes the currently held pointer and sets it to 'p' or to @NULL if no
        arguments are specified. This function does check to make sure that the
        pointer you are assigning is not the same pointer that is already stored.
    */
    reset(T p  = NULL);

    /**
        Swap the pointer inside the smart pointer with 'ot'. The pointer being swapped
        must be of the same type (hence the same class name).
    */
    swap(wxScopedPtr amp; ot);
};



/**
    @class wxScopedTiedPtr
    @wxheader{ptr_scpd.h}

    This is a variation on the topic of wxScopedPtr. This
    class is also a smart pointer but in addition it "ties'' the pointer value to
    another variable. In other words, during the life time of this class the value
    of that variable is set to be the same as the value of the pointer itself and
    it is reset to its old value when the object is destroyed. This class is
    especially useful when converting the existing code (which may already store
    the pointers value in some variable) to the smart pointers.

    @library{wxbase}
    @category{FIXME}
*/
class wxScopedTiedPtr
{
public:
    /**
        Constructor creates a smart pointer initialized with @a ptr and stores
        @a ptr in the location specified by @a ppTie which must not be
        @NULL.
    */
    wxScopedTiedPtr(T** ppTie, T* ptr);

    /**
        Destructor frees the pointer help by this object and restores the value stored
        at the tied location (as specified in the @ref ctor() constructor)
        to the old value.
        Warning: this location may now contain an uninitialized value if it hadn't been
        initialized previously, in particular don't count on it magically being
        @NULL!
    */
    ~wxScopedTiedPtr();
};



/**
    @wxheader{ptr_scpd.h}

    A scoped pointer template class. It is the template version of
    the old-style @ref overview_wxscopedptr "scoped pointer macros".

    @library{wxbase}
    @category{FIXME}

    @see wxSharedPtr, wxWeakRef
*/
class wxScopedPtr<T>
{
public:
    /**
        Constructor.
    */
    wxScopedPtrT(T* ptr = NULL);

    /**
        Destructor.
    */
    ~wxScopedPtrT();

    /**
        Returns pointer to object or @NULL.
    */
    T* get() const;

    /**
        Conversion to a boolean expression (in a variant which is not
        convertable to anything but a boolean expression). If this class
        contains a valid pointer it will return @e @true, if it contains
        a @NULL pointer it will return @e @false.
    */
    operator unspecified_bool_type() const;

    /**
        Returns a reference to the object. If the internal pointer is @NULL
        this method will cause an assert in debug mode.
    */
    T operator*() const;

    /**
        Returns pointer to object. If the pointer is @NULL this method will
        cause an assert in debug mode.
    */
    T* operator-() const;

    /**
        Releases the current pointer and returns it.
        Afterwards the caller is responsible for deleting
        the data contained in the scoped pointer before.
    */
    T* release();

    /**
        Reset pointer to the value of @e ptr. The
        previous pointer will be deleted.
    */
    void reset(T* ptr = NULL);

    /**
        Swaps pointers.
    */
    void swap(wxScopedPtr<T>& ot);
};

