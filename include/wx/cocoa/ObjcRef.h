/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/ObjcRef.h
// Purpose:     wxObjcAutoRef template class
// Author:      David Elliott
// Modified by: 
// Created:     2004/03/28
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott <dfe@cox.net>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_OBJCREF_H__
#define _WX_COCOA_OBJCREF_H__

// Reuse wxCFRef-related code (e.g. wxCFRetain/wxCFRelease)
#include "wx/mac/corefoundation/cfref.h"

/*
wxObjcAutoRefFromAlloc: construct a reference to an object that was
[NSObject -alloc]'ed and thus does not need a retain
wxObjcAutoRef: construct a reference to an object that was
either autoreleased or is retained by something else.
*/

struct objc_object;

// We must do any calls to Objective-C from an Objective-C++ source file
class wxObjcAutoRefBase
{
protected:
    /*! @function   ObjcRetain
        @abstract   Simply does [p retain].
     */
    static struct objc_object* ObjcRetain(struct objc_object*);

    /*! @function   ObjcRelease
        @abstract   Simply does [p release].
     */
    static void ObjcRelease(struct objc_object*);
};

/*! @class      wxObjcAutoRefFromAlloc
    @templatefield  T       The type of _pointer_ (e.g. NSString*, NSRunLoop*)
    @abstract   Pointer-holder for Objective-C objects
    @discussion
    When constructing this object from a raw pointer, the pointer is assumed to have
    come from an alloc-style method.  That is, once you construct this object from
    the pointer you must not balance your alloc with a call to release.

    This class has been carefully designed to work with both the traditional Retain/Release
    and the new Garbage Collected modes.  In RR-mode it will prevent the object from being
    released by managing the reference count using the retain/release semantics.  In GC-mode
    it will use a method (currently CFRetain/CFRelease) to ensure the object will never be
    finalized until this object is destroyed.
 */
    
template <class T>
class wxObjcAutoRefFromAlloc: wxObjcAutoRefBase
{
public:
    wxObjcAutoRefFromAlloc(T p = 0)
    :   m_ptr(p)
    // NOTE: this is from alloc.  Do NOT retain
    {
        //  CFRetain
        //      GC:     Object is strongly retained and prevented from being collected
        //      non-GC: Simply realizes it's an Objective-C object and calls [p retain]
        wxCFRetain(p);
        //  ObjcRelease (e.g. [p release])
        //      GC:     Objective-C retain/release mean nothing in GC mode
        //      non-GC: This is a normal release call, balancing the retain
        ObjcRelease(static_cast<T>(p));
        //  The overall result:
        //      GC:     Object is strongly retained
        //      non-GC: Retain count is the same as it was (retain then release)
    }
    wxObjcAutoRefFromAlloc(const wxObjcAutoRefFromAlloc& otherRef)
    :   m_ptr(otherRef.m_ptr)
    {   wxCFRetain(m_ptr); }
    ~wxObjcAutoRefFromAlloc()
    {   wxCFRelease(m_ptr); }
    wxObjcAutoRefFromAlloc& operator=(const wxObjcAutoRefFromAlloc& otherRef)
    {   wxCFRetain(otherRef.m_ptr);
        wxCFRelease(m_ptr);
        m_ptr = otherRef.m_ptr;
        return *this;
    }
    operator T() const
    {   return static_cast<T>(m_ptr); }
    T operator->() const
    {   return static_cast<T>(m_ptr); }
protected:
    /*! @field      m_ptr       The pointer to the Objective-C object
        @discussion
        The pointer to the Objective-C object is typed as void* to avoid compiler-generated write
        barriers as would be used for implicitly __strong object pointers and to avoid the similar
        read barriers as would be used for an explicitly __weak object pointer.  The write barriers
        are unsuitable because they assume the pointer (e.g. this object) is located in the heap
        which we can't guarantee and in fact most often we are used as a global.  We therefore
        use the CFRetain/CFRelease functions which work regardless of our memory location.
     */
    void *m_ptr;
};

/*!
    @class      wxObjcAutoRef
    @description
    A pointer holder that does retain its argument.
    NOTE: It is suggest that you instead use wxObjcAutoRefFromAlloc<T> foo([aRawPointer retain])
 */
template <class T>
class wxObjcAutoRef: public wxObjcAutoRefFromAlloc<T>
{
public:
    /*! @method     wxObjcAutoRef
        @description
        Uses the underlying wxObjcAutoRefFromAlloc and simply does a typical [p retain] such that
        in RR-mode the object is in effectively the same retain-count state as it would have been
        coming straight from an alloc method.
     */
    wxObjcAutoRef(T p = 0)
    :   wxObjcAutoRefFromAlloc<T>(p)
    {   // NOTE: ObjcRetain is correct because in GC-mode it balances ObjcRelease in our superclass constructor
        // In RR mode it does retain and the superclass does retain/release thus resulting in an overall retain.
        ObjcRetain(static_cast<T>(wxObjcAutoRefFromAlloc<T>::m_ptr));
    }
    ~wxObjcAutoRef() {}
    wxObjcAutoRef(const wxObjcAutoRef& otherRef)
    :   wxObjcAutoRefFromAlloc<T>(otherRef)
    {}
    wxObjcAutoRef(const wxObjcAutoRefFromAlloc<T>& otherRef)
    :   wxObjcAutoRefFromAlloc<T>(otherRef)
    {}
    wxObjcAutoRef& operator=(const wxObjcAutoRef& otherRef)
    {   return wxObjcAutoRefFromAlloc<T>::operator=(otherRef); }
};

#endif //ndef _WX_COCOA_OBJCREF_H__
