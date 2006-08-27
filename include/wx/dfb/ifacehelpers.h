/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/ifacehelpers.h
// Purpose:     helpers for dealing with DFB interfaces
// Author:      Vaclav Slavik
// Created:     2006-08-09
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_IFACEHELPERS_H_
#define _WX_DFB_IFACEHELPERS_H_

//-----------------------------------------------------------------------------
// wxDFB_DECLARE_INTERFACE
//-----------------------------------------------------------------------------

/**
    Forward declares DirectFB interface @a name.

    Also declares name##Ptr typedef for wxDfbPtr<name> pointer.

    @param name  name of the DirectFB interface
 */
#define wxDFB_DECLARE_INTERFACE(name)            \
    struct _##name;                              \
    typedef _##name name;                        \
    typedef wxDfbPtr<name> name##Ptr;


//-----------------------------------------------------------------------------
// wxDfbPtr
//-----------------------------------------------------------------------------

// base class for wxDfbPtr
class wxDfbPtrBase
{
protected:
    // increment/decrement refcount; see ifacehelpers.cpp for why using
    // void* is safe
    static void DoAddRef(void *ptr);
    static void DoRelease(void *ptr);
};

/**
    This template implements smart pointer for keeping pointers to DirectFB
    interfaces. Interface's reference count is increased on copying and the
    interface is released when the pointer is deleted.
 */
template<typename T>
class wxDfbPtr : private wxDfbPtrBase
{
public:
    /**
        Creates the pointer from raw interface pointer.

        Takes ownership of @a ptr, i.e. AddRef() is @em not called on it.
     */
    wxDfbPtr(T *ptr = NULL) : m_ptr(ptr) {}

    /// Copy ctor
    wxDfbPtr(const wxDfbPtr& ptr) { InitFrom(ptr); }

    /// Dtor. Releases the interface
    ~wxDfbPtr() { Reset(); }

    /// Resets the pointer to NULL, decreasing reference count of the interface.
    void Reset()
    {
        if ( m_ptr )
        {
            DoRelease(m_ptr);
            m_ptr = NULL;
        }
    }

    /// Cast to raw pointer
    operator T*() const { return m_ptr; }

    /**
        Cast to @em writeable raw pointer so that code like
        "dfb->CreateFont(dfb, NULL, &desc, &fontPtr)" works.

        Note that this operator calls Reset(), so using it looses the value.
     */
    T** operator&()
    {
        Reset();
        return &m_ptr;
    }

    // standard operators:

    wxDfbPtr& operator=(const wxDfbPtr& ptr)
    {
        Reset();
        InitFrom(ptr);
        return *this;
    }

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

private:
    void InitFrom(const wxDfbPtr& ptr)
    {
        m_ptr = ptr.m_ptr;
        if ( m_ptr )
            DoAddRef(m_ptr);
    }

private:
    T *m_ptr;
};

#endif // _WX_DFB_IFACEHELPERS_H_
