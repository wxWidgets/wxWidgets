/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/corefoundation/cfref.h
// Purpose:     wxCFRef template class
// Author:      David Elliott <dfe@cox.net>
// Modified by: Stefan Csomor
// Created:     2007/05/10
// RCS-ID:      $Id$
// Copyright:   (c) 2007 David Elliott <dfe@cox.net>, Stefan Csomor
// Licence:     wxWindows licence
// Notes:       See http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/index.html
/////////////////////////////////////////////////////////////////////////////
/*! @header     wx/mac/corefoundation/cfref.h
    @abstract   wxCFRef template class
    @discussion FIXME: Convert doc tags to something less buggy with C++
*/

#ifndef _WX_MAC_COREFOUNDATION_CFREF_H__
#define _WX_MAC_COREFOUNDATION_CFREF_H__

#include <CoreFoundation/CFBase.h>

/*! @function   wxCFRelease
    @abstract   A CFRelease variant that checks for NULL before releasing.
*/
inline void wxCFRelease(CFTypeRef r)
{
    if ( r != NULL )
        ::CFRelease(r);
}

/*! @function   wxCFRetain
    @abstract   A typesafe CFRetain variant that checks for NULL.
*/
template <class Type>
inline Type* wxCFRetain(Type *r)
{
    // NOTE(DE): Setting r to the result of CFRetain improves efficiency on both x86 and PPC
    // Casting r to CFTypeRef ensures we are calling the real C version defined in CFBase.h
    // and not any possibly templated/overloaded CFRetain.
    if ( r != NULL )
        r = (Type*)::CFRetain((CFTypeRef)r);
    return r;
}

/*! @class wxCFRef
    @templatefield  refType     The CF reference type (e.g. CFStringRef, CFRunLoopRef, etc.)
                                It should already be a pointer.  This is different from
                                shared_ptr where the template parameter is the pointee type.
    @discussion Properly retains/releases reference to CoreFoundation objects
*/
template <class refType>
class wxCFRef
{
public:
    /*! @method     wxCFRef
        @abstract   Creates a NULL reference
    */
    wxCFRef()
    :   m_ptr(NULL)
    {}

    /*! @method     wxCFRef
        @abstract   Assumes ownership of p and creates a reference to it.
        @templatefield otherType    Any type.
        @param p        The raw pointer to assume ownership of.  May be NULL.
        @discussion Like shared_ptr, it is assumed that the caller has a strong reference to p and intends
                    to transfer ownership of that reference to this ref holder.  If the object comes from
                    a Create or Copy method then this is the correct behavior.  If the object comes from
                    a Get method then you must CFRetain it yourself before passing it to this constructor.
                    A handy way to do this is to use the non-member wxCFRefFromGet factory funcion.
                    This method is templated and takes an otherType *p.  This prevents implicit conversion
                    using an operator refType() in a different ref-holding class type.
    */
    template <class otherType>
    explicit wxCFRef(otherType *p)
    :   m_ptr(p) // Implicit conversion from otherType* to refType should occur.
    {}

    /*! @method     wxCFRef
        @abstract   Copies a ref holder of the same type
        @param otherRef The other ref holder to copy.
        @discussion Ownership will be shared by the original ref and the newly created ref. That is,
                    the object will be explicitly retained by this new ref.
    */
    wxCFRef(const wxCFRef& otherRef)
    :   m_ptr(wxCFRetain(otherRef.m_ptr))
    {}

    /*! @method     wxCFRef
        @abstract   Copies a ref holder where its type can be converted to ours
        @templatefield otherRefType     Any type held by another wxCFRef.
        @param otherRef The other ref holder to copy.
        @discussion Ownership will be shared by the original ref and the newly created ref. That is,
                    the object will be explicitly retained by this new ref.
    */
    template <class otherRefType>
    wxCFRef(const wxCFRef<otherRefType>& otherRef)
    :   m_ptr(wxCFRetain(otherRef.get())) // Implicit conversion from otherRefType to refType should occur
    {}

    /*! @method     ~wxCFRef
        @abstract   Releases (potentially shared) ownership of the ref.
        @discussion A ref holder instance is always assumed to have ownership so ownership is always
                    released (CFRelease called) upon destruction.
    */
    ~wxCFRef()
    {   reset(); }

    /*! @method     operator=
        @abstract   Assigns the other ref's pointer to us when the otherRef is the same type.
        @param otherRef The other ref holder to copy.
        @discussion The incoming pointer is retained, the original pointer is released, and this object
                    is made to point to the new pointer.
    */
    wxCFRef& operator=(const wxCFRef& otherRef)
    {
        wxCFRetain(otherRef.m_ptr);
        wxCFRelease(m_ptr);
        m_ptr = otherRef.m_ptr;
        return *this;
    }

    /*! @method     operator=
        @abstract   Assigns the other ref's pointer to us when the other ref can be converted to our type.
        @templatefield otherRefType     Any type held by another wxCFRef
        @param otherRef The other ref holder to copy.
        @discussion The incoming pointer is retained, the original pointer is released, and this object
                    is made to point to the new pointer.
    */
    template <class otherRefType>
    wxCFRef& operator=(const wxCFRef<otherRefType>& otherRef)
    {
        wxCFRetain(otherRef.get());
        wxCFRelease(m_ptr);
        m_ptr = otherRef.get(); // Implicit conversion from otherRefType to refType should occur
        return *this;
    }

    /*! @method     get
        @abstract   Explicit conversion to the underlying pointer type
        @discussion Allows the caller to explicitly get the underlying pointer.
    */
    refType get() const
    {   return m_ptr; }

    /*! @method     operator refType
        @abstract   Implicit conversion to the underlying pointer type
        @discussion Allows the ref to be used in CF function calls.
    */
    operator refType() const
    {   return m_ptr; }

#if 0
    <   // HeaderDoc is retarded and thinks the GT from operator-> is part of a template param.
        // So give it that < outside of a comment to fake it out. (if 0 is not a comment to HeaderDoc)
#endif

    /*! @method     operator-&gt;
        @abstract   Implicit conversion to the underlying pointer type
        @discussion This is nearly useless for CF types which are nearly always opaque
    */
    refType operator-> () const
    {   return m_ptr; }

    /*! @method     reset
        @abstract   Nullifies the reference
        @discussion Releases ownership (calls CFRelease) before nullifying the pointer.
    */
    void reset()
    {
        wxCFRelease(m_ptr);
        m_ptr = NULL;
    }

    /*! @method     reset
        @abstract   Sets this to a new reference
        @templatefield otherType    Any type.
        @param p        The raw pointer to assume ownership of
        @discussion The existing reference is released (like destruction).  It is assumed that the caller
                    has a strong reference to the new p and intends to transfer ownership of that reference
                    to this ref holder.  Take care to call CFRetain if you received the object from a Get method.
                    This method is templated and takes an otherType *p.  This prevents implicit conversion
                    using an operator refType() in a different ref-holding class type.
    */
    template <class otherType>
    void reset(otherType* p)
    {
        wxCFRelease(m_ptr);
        m_ptr = p; // Automatic conversion should occur
    }
protected:
    /*! @var m_ptr      The raw pointer.
    */
    refType m_ptr;
};

/*! @function   wxCFRefFromGet
    @abstract   Factory function to create wxCFRef from a raw pointer obtained from a Get-rule function
    @param  p           The pointer to retain and create a wxCFRef from.  May be NULL.
    @discussion Unlike the wxCFRef raw pointer constructor, this function explicitly retains its
                argument.  This can be used for functions such as CFDictionaryGetValue() or
                CFAttributedStringGetString() which return a temporary reference (Get-rule functions).
                FIXME: Anybody got a better name?
*/
template <typename Type>
inline wxCFRef<Type*> wxCFRefFromGet(Type *p)
{
    return wxCFRef<Type*>(wxCFRetain(p));
}

/*! @function   static_cfref_cast
    @abstract   Works like static_cast but from one wxCFRef to another
    @param  refType     Template parameter.  The destination raw pointer type
    @param  otherRef    Normal parameter.  The source wxCFRef<> object.
    @discussion This is modeled after shared_ptr's static_pointer_cast.  Just as wxCFRef is
                parameterized on a pointer to an opaque type so is this class.  Note that
                this differs from shared_ptr which is parameterized on the pointee type.
                FIXME: Anybody got a better name?
*/
template <class refType, class otherRefType>
inline wxCFRef<refType> static_cfref_cast(const wxCFRef<otherRefType> &otherRef);

template <class refType, class otherRefType>
inline wxCFRef<refType> static_cfref_cast(const wxCFRef<otherRefType> &otherRef)
{
    return wxCFRef<refType>(static_cast<refType>(wxCFRetain(otherRef.get())));
}

/*! @function   CFRelease
    @abstract   Overloads CFRelease so that the user is warned of bad behavior.
    @discussion It is rarely appropriate to retain or release a wxCFRef.  If one absolutely
                must do it he can explicitly get() the raw pointer
                Normally, this function is unimplemented resulting in a linker error if used.
*/
template <class T>
inline void CFRelease(const wxCFRef<T*> & cfref) DEPRECATED_ATTRIBUTE;

/*! @function   CFRetain
    @abstract   Overloads CFRetain so that the user is warned of bad behavior.
    @discussion It is rarely appropriate to retain or release a wxCFRef.  If one absolutely
                must do it he can explicitly get() the raw pointer
                Normally, this function is unimplemented resulting in a linker error if used.
*/
template <class T>
inline void CFRetain(const wxCFRef<T*>& cfref) DEPRECATED_ATTRIBUTE;

// Change the 0 to a 1 if you want the functions to work (no link errors)
// Neither function will cause retain/release side-effects if implemented.
#if 0
template <class T>
void CFRelease(const wxCFRef<T*> & cfref)
{
    CFRelease(cfref.get());
}

template <class T>
void CFRetain(const wxCFRef<T*> & cfref)
{
    CFRetain(cfref.get());
}
#endif

#endif //ndef _WX_MAC_COREFOUNDATION_CFREF_H__

