/////////////////////////////////////////////////////////////////////////////
// Name:        ptr_shrd.h
// Purpose:     interface of wxSharedPtr<T>
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @wxheader{ptr_shrd.h}

    A smart pointer with non-intrusive reference counting. It is modeled after
    @c boost::shared_ptr and can be used with STL containers and wxVector<T> -
    unlike @c std::auto_ptr and wxScopedPtr<T>.

    @library{wxbase}
    @category{smartpointers}

    @see wxScopedPtr<T>, wxWeakRef<T>, wxObjectDataPtr
*/
template<typename T>
class wxSharedPtr<T>
{
public:
    /**
        Constructor.

        Creates shared pointer from the raw pointer @a ptr and takes ownership
        of it.
    */
    wxSharedPtr(T* ptr = NULL);

    /// Copy constructor.
    wxSharedPtr(const wxSharedPtr<T>& tocopy);

    /**
        Destructor.
    */
    ~wxSharedPtrT();

    /**
        Returns pointer to its object or @NULL.
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
        Returns pointer to its object or @NULL.
    */
    T* operator-() const;

    /**
        Assignment operator. Releases any previously held pointer
        and creates a reference to @e ptr.
    */
    wxSharedPtrT& operator operator=(T* ptr);

    /**
        Reset pointer to @e ptr. If the reference count of the
        previously owned pointer was 1 it will be deleted.
    */
    void reset(T* ptr = NULL);

    /**
        Returns @true if this is the only pointer pointing to its object.
    */
    bool unique() const;

    /**
        Returns the number of pointers pointing to its object.
    */
    long use_count() const;
};

