/////////////////////////////////////////////////////////////////////////////
// Name:        weakref.h
// Purpose:     interface of wxWeakRefDynamic<T>
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @wxheader{weakref.h}

    wxWeakRefDynamicT is a template class for weak references that is used in
    the same way as wxWeakRefT. The only difference is that wxWeakRefDynamic
    defaults to using @c dynamic_cast for establishing the object
    reference (while wxWeakRef defaults to @c static_cast).

    So, wxWeakRef will detect a type mismatch during compile time and will
    have a little better run-time performance. The role of wxWeakRefDynamic
    is to handle objects which derived type one does not know.

    @b Note: wxWeakRefT selects an implementation based on the static type
    of T. If T does not have wxTrackable statically, it defaults to to a mixed-
    mode operation, where it uses @c dynamic_cast as the last measure (if
    available from the compiler and enabled when building wxWidgets).

    For general cases, wxWeakRefT is the better choice.

    For API documentation, see: wxWeakRef

    @library{wxcore}
    @category{FIXME}
*/
class wxWeakRefDynamic<T>
{
public:

};



/**
    @wxheader{weakref.h}

    wxWeakRef is a template class for weak references to wxWidgets objects,
    such as wxEvtHandler, wxWindow and
    wxObject. A weak reference behaves much like an ordinary
    pointer, but when the object pointed is destroyed, the weak reference is
    automatically reset to a @NULL pointer.

    wxWeakRefT can be used whenever one must keep a pointer to an object
    that one does not directly own, and that may be destroyed before the object
    holding the reference.

    wxWeakRefT is a small object and the mechanism behind it is fast
    (@b O(1)). So the overall cost of using it is small.

    @library{wxbase}
    @category{FIXME}

    @see wxSharedPtr, wxScopedPtr
*/
class wxWeakRef<T>
{
public:
    /**
        Constructor. The weak reference is initialized to @e pobj.
    */
    wxWeakRefT(T* pobj = NULL);

    /**
        Destructor.
    */
    ~wxWeakRefT();

    /**
        Called when the tracked object is destroyed. Be default sets
        internal pointer to @NULL.
    */
    virtual void OnObjectDestroy();

    /**
        Release currently tracked object and rests object reference.
    */
    void Release();

    /**
        Returns pointer to the tracked object or @NULL.
    */
    T* get() const;

    /**
        Release currently tracked object and start tracking the same object as
        the wxWeakRef @e wr.
    */
    T* operator =(wxWeakRef<T>& wr);

    /**
        Implicit conversion to T*. Returns pointer to the tracked
        object or @NULL.
    */
    T* operator*() const;

    /**
        Returns a reference to the tracked object. If the internal pointer is @NULL
        this method will cause an assert in debug mode.
    */
    T operator*() const;

    /**
        Smart pointer member access. Returns a pointer to the
        tracked object. If the internal pointer is @NULL this
        method will cause an assert in debug mode.
    */
    T* operator-();

    /**
        Releases the currently tracked object and starts tracking @e pobj.
        A weak reference may be reset by passing @e @NULL as @e pobj.
    */
    T* operator=(T* pobj);
};

